#include "HTSVector.h"
/* Author : mesriy at Tue Jul 24 15:28:21 2012
 * Generated by createNew
 */

#include <alien/kernels/hts/data_structure/HTSInternal.h>
#include <alien/kernels/hts/HTSBackEnd.h>
#include <alien/core/block/Block.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace Alien {

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
template <typename ValueT, bool is_mpi>
HTSVector<ValueT, is_mpi>::HTSVector(const MultiVectorImpl* multi_impl)
: IVectorImpl(multi_impl, AlgebraTraits<BackEnd::tag::hts>::name())
, m_local_offset(0)
{
}

/*---------------------------------------------------------------------------*/
template <typename ValueT, bool is_mpi> HTSVector<ValueT, is_mpi>::~HTSVector()
{
}

/*---------------------------------------------------------------------------*/
template <typename ValueT, bool is_mpi>
void
HTSVector<ValueT, is_mpi>::init(const VectorDistribution& dist, const bool need_allocate)
{
  if (need_allocate)
    allocate();
}

/*---------------------------------------------------------------------------*/
template <typename ValueT, bool is_mpi>
void
HTSVector<ValueT, is_mpi>::allocate()
{
  const VectorDistribution& dist = this->distribution();
  m_local_offset = dist.offset();

  m_internal.reset(new VectorInternal(this->scalarizedLocalSize()));

  // m_internal->m_internal = 0.;
}

/*---------------------------------------------------------------------------*/

template <typename ValueT, bool is_mpi>
void
HTSVector<ValueT, is_mpi>::setValues(const int nrow, const ValueT* values)
{
  m_internal->m_data = values;
  // for (int i= 0; i < nrow; ++i)
  //{
  //  m_internal->m_data[i] = values[i] ;
  //}
}

/*---------------------------------------------------------------------------*/
template <typename ValueT, bool is_mpi>
void
HTSVector<ValueT, is_mpi>::getValues(const int nrow, ValueT* values) const
{
  // for (int i = 0; i < nrow; ++i)
  //  values[i] = m_internal->m_data[i];
}

template <typename ValueT, bool is_mpi>
void
HTSVector<ValueT, is_mpi>::dump() const
{
  for (int i = 0; i < m_internal->m_local_size; ++i)
    std::cout << m_internal->m_data[i] << " " << std::endl;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
template class HTSVector<double, true>;
} // namespace Alien

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/