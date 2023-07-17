// -*- C++ -*-
#ifndef ALIEN_INDEXMANAGER_ANYITEMABSTRACTFAMILY_H
#define ALIEN_INDEXMANAGER_ANYITEMABSTRACTFAMILY_H
/* Author : havep at Mon Jun  3 12:49:08 2013
 * Generated by createNew
 */

#include "alien/AlienArcaneToolsPrecomp.h"
#include <arcane/IItemFamily.h>
#include <arcane/anyitem/AnyItemFamily.h>
#include "alien/arcane_tools/IIndexManager.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Alien {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcaneTools {

  /*---------------------------------------------------------------------------*/
  /*---------------------------------------------------------------------------*/

  /* Dans cette implémentation, implicitement le localId du i_ème item est i */
  class ALIEN_ARCANE_TOOLS_EXPORT AnyItemAbstractFamily
      : public IIndexManager::IAbstractFamily
  {
   public:
    //! Construit une famille abstraite à partir d'une famille d'AnyItem
    AnyItemAbstractFamily(const Arcane::AnyItem::Family& family, IIndexManager* manager);

    virtual ~AnyItemAbstractFamily();

   public:
    IIndexManager::IAbstractFamily* clone() const;

   public:
    Arccore::Integer maxLocalId() const;
    void uniqueIdToLocalId(
        Arccore::Int32ArrayView localIds, Arccore::Int64ConstArrayView uniqueIds) const;
    IAbstractFamily::Item item(Arccore::Integer localId) const;
    Arccore::SharedArray<Arccore::Integer> owners(
        Arccore::Int32ConstArrayView localIds) const;
    Arccore::SharedArray<Arccore::Int64> uids(
        Arccore::Int32ConstArrayView localIds) const;
    Arccore::SharedArray<Arccore::Int32> allLocalIds() const;

   private:
    const Arcane::AnyItem::Family& m_family;
    IIndexManager* m_manager;
    UniqueArray<Arccore::Integer> m_lower_bounds;
  };

  /*---------------------------------------------------------------------------*/
  /*---------------------------------------------------------------------------*/

} // namespace Alien

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ALIEN_INDEXMANAGER_ANYITEMABSTRACTFAMILY_H */