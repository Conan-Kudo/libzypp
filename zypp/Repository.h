
#ifndef ZYPP_REPOSITORY_H
#define ZYPP_REPOSITORY_H

#include <iosfwd>
#include <string>

#include "zypp/base/PtrTypes.h"
#include "zypp/base/SafeBool.h"
#include "zypp/ResStore.h"
#include "zypp/RepoInfo.h"

namespace zypp
{
  namespace repo
  {
    DEFINE_PTR_TYPE(RepositoryImpl);
    class RepositoryImpl;
  }

  class Repository : private base::SafeBool<Repository>
  {
    public:
    friend std::ostream & operator<<( std::ostream & str, const Repository & obj );
    friend bool operator==( const Repository & lhs, const Repository & rhs );
    friend bool operator<( const Repository & lhs, const Repository & rhs );

  public:
    typedef repo::RepositoryImpl     Impl;
    typedef repo::RepositoryImpl_Ptr Impl_Ptr;

  public:

    /** Default ctor: noRepository.
     * \see RepoManager::createFromCache.
    */
    Repository();

    /** A dummy Repository (Id \c 0) providing nothing, doing nothing.
     * \todo provide a _constRef
    */
    static const Repository noRepository;

    /** Validate Repository in a boolean context.
     * \c FALSE iff == noRepository.
    */
    using base::SafeBool<Repository>::operator bool_type;

  public:
    typedef unsigned long NumericId;

    /** Runtime unique numeric Repository Id. */
    NumericId numericId() const;

    const ResStore & resolvables();

    const RepoInfo info() const;

  private:
    friend base::SafeBool<Repository>::operator bool_type() const;
    /** \ref SafeBool test. */
    bool boolTest() const
    { return _pimpl != noRepository._pimpl; }

  private:
    /** Factory */
    friend class RepoManager;
    friend class repo::RepositoryImpl;

  private:
    /** Factory ctor */
    explicit
    Repository( const Impl_Ptr & impl_r );

  private:
    /** Pointer to implementation */
    RW_pointer<Impl,rw_pointer::Intrusive<Impl> > _pimpl;
  };

}

#endif


