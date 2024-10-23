﻿// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2024 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* MemoryPool.cc                                               (C) 2000-2024 */
/*                                                                           */
/* Classe pour gérer une liste de zone allouées.                             */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "arcane/utils/internal/MemoryPool.h"

#include "arcane/utils/FatalErrorException.h"
#include "arcane/utils/PlatformUtils.h"

#include <unordered_map>
#include <map>
#include <atomic>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Arcane::impl
{

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class MemoryPool::Impl
{
  //! Tableau associatif des pointeurs alloués et la taille associée
  class AllocatedMap
  {
   public:

    using MapType = std::unordered_map<void*, size_t>;
    using ValueType = MapType::value_type;
    using MapIterator = MapType::iterator;

   public:

    AllocatedMap(const String& name)
    : m_name(name)
    {}

   public:

    void removePointer(void* ptr, size_t size)
    {
      auto x = m_allocated_memory_map.find(ptr);
      if (x == m_allocated_memory_map.end())
        ARCANE_FATAL("MemoryPool '{0}': pointer {1} is not in the allocated map", m_name, ptr);

      size_t allocated_size = x->second;
      if (size != allocated_size)
        ARCANE_FATAL("MemoryPool '{0}': Incoherent size saved_size={1} arg_size={2}",
                     m_name, allocated_size, size);

      m_allocated_memory_map.erase(x);
    }

    void addPointer(void* ptr, size_t size)
    {
      auto x = m_allocated_memory_map.find(ptr);
      if (x != m_allocated_memory_map.end())
        ARCANE_FATAL("MemoryPool '{0}': pointer {1} (for size={2}) is already in the allocated map (with size={3})",
                     m_name, ptr, size, x->second);

      m_allocated_memory_map.insert(std::make_pair(ptr, size));
    }

    size_t size() const { return m_allocated_memory_map.size(); }

   private:

    MapType m_allocated_memory_map;
    String m_name;
  };

 public:

  explicit Impl(IMemoryPoolAllocator* allocator, const String& name)
  : m_allocator(allocator)
  , m_allocated_map(name)
  , m_name(name)
  {
    //m_max_memory_size_to_pool = 0;
  }

 public:

  void* allocateMemory(size_t size);
  void freeMemory(void* ptr, size_t size);
  void dumpStats(std::ostream& ostr);
  void dumpFreeMap(std::ostream& ostr);

 public:

  IMemoryPoolAllocator* m_allocator = nullptr;
  // Contient une liste de couples (taille_mémoire,pointeur) de mémoire allouée.
  AllocatedMap m_allocated_map;

  std::unordered_multimap<size_t, void*> m_free_memory_map;
  std::atomic<size_t> m_total_allocated = 0;
  std::atomic<size_t> m_total_free = 0;
  std::atomic<Int32> m_nb_cached = 0;
  size_t m_max_memory_size_to_pool = 1024 * 64 * 4 * 4;
  String m_name;

 private:

  void _freeMemory(void* ptr);
  void _addAllocated(void* ptr, size_t size);
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

MemoryPool::
MemoryPool(IMemoryPoolAllocator* allocator, const String& name)
: m_p(std::make_shared<Impl>(allocator, name))
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

MemoryPool::
~MemoryPool()
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* MemoryPool::Impl::
allocateMemory(size_t size)
{
  if (m_max_memory_size_to_pool != 0 && size > m_max_memory_size_to_pool)
    return m_allocator->allocateMemory(size);

  auto x = m_free_memory_map.find(size);
  void* ptr = nullptr;
  if (x != m_free_memory_map.end()) {
    ptr = x->second;
    m_free_memory_map.erase(x);
    m_total_free -= size;
    ++m_nb_cached;
  }
  else {
    ptr = m_allocator->allocateMemory(size);
  }
  _addAllocated(ptr, size);
  return ptr;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MemoryPool::Impl::
freeMemory(void* ptr, size_t size)
{
  if (m_max_memory_size_to_pool != 0 && size > m_max_memory_size_to_pool)
    return m_allocator->freeMemory(ptr, size);

  m_allocated_map.removePointer(ptr, size);

  m_free_memory_map.insert(std::make_pair(size, ptr));
  m_total_allocated -= size;
  m_total_free += size;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MemoryPool::Impl::
_addAllocated(void* ptr, size_t size)
{
  m_allocated_map.addPointer(ptr, size);
  m_total_allocated += size;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MemoryPool::Impl::
dumpStats(std::ostream& ostr)
{
  ostr << "Stats '" << m_name << "' TotalAllocated=" << m_total_allocated
       << " TotalFree=" << m_total_free
       << " nb_allocated=" << m_allocated_map.size()
       << " nb_free=" << m_free_memory_map.size()
       << " nb_cached=" << m_nb_cached
       << "\n";
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void MemoryPool::Impl::
dumpFreeMap(std::ostream& ostr)
{
  std::map<size_t, Int32> nb_alloc_per_size;
  for (const auto& [key, value] : m_free_memory_map) {
    auto x = nb_alloc_per_size.find(key);
    if (x == nb_alloc_per_size.end())
      nb_alloc_per_size.insert(std::make_pair(key, 1));
    else
      ++x->second;
  }
  ostr << "FreeMap '" << m_name << "\n";
  for (const auto& [key, value] : nb_alloc_per_size)
    ostr << "Map size=" << key << " nb_allocated=" << value << " page_modulo=" << (key % 4096) << "\n";
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void* MemoryPool::allocateMemory(size_t size)
{
  return m_p->allocateMemory(size);
}
void MemoryPool::freeMemory(void* ptr, size_t size)
{
  m_p->freeMemory(ptr, size);
}
void MemoryPool::dumpStats(std::ostream& ostr)
{
  m_p->dumpStats(ostr);
}
void MemoryPool::
dumpFreeMap(std::ostream& ostr)
{
  m_p->dumpFreeMap(ostr);
}
String MemoryPool::name() const
{
  return m_p->m_name;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

} // namespace Arcane::impl

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
