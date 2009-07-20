/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/ZYppCommitPolicy.h
 *
*/
#ifndef ZYPP_ZYPPCOMMITPOLICY_H
#define ZYPP_ZYPPCOMMITPOLICY_H

#include <iosfwd>

#include "zypp/base/PtrTypes.h"

#include "zypp/target/rpm/RpmFlags.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  /** Supported commit download policies. */
  enum DownloadMode
  {
    DownloadOnly,	//!< Just download all packages to the local cache.
			//!< Do not install. Implies a dry-run.
    DownloadInAdvance,	//!< First download all packages to the local cache.
			//!< Then start to install.
    DownloadImHeaps,	//!< Similar to DownloadInAdvance, but try to split
			//!< the transaction into heaps, where at the end of
			//!< each heap a consistent system state is reached.
    DownloadAsNeeded	//!< Alternating download and install. Packages are
			//!< cached just to avid CD/DVD hopping. This is the
			//!< traditional behaviour.
  };

  /** \relates DownloadMode Stream output. */
  std::ostream & operator<<( std::ostream & str, DownloadMode obj );

  ///////////////////////////////////////////////////////////////////
  //
  //	CLASS NAME : ZYppCommitPolicy
  //
  /** */
  class ZYppCommitPolicy
  {
    public:

      ZYppCommitPolicy();

    public:
      /** Restrict commit to media 1.
       * Fake outstanding YCP fix: Honour restriction to media 1
       * at installation, but install all remaining packages if
       * post-boot (called with <tt>mediaNr_r &gt; 1</tt>).
       */
      ZYppCommitPolicy & restrictToMedia( unsigned mediaNr_r );

      /** Process all media (default) */
      ZYppCommitPolicy & allMedia()
      { return restrictToMedia( 0 ); }

      unsigned restrictToMedia() const;


      /** Set dry run (default: false).
       * Dry-run should not change anything on the system, unless
       * the \ref downloadMode is set to \ref DownloadOnly. In that
       * case packages are downloaded to the local cache.
      */
      ZYppCommitPolicy & dryRun( bool yesNo_r );

      bool dryRun() const;


      /** Commit download policy to use. (default: \ref DownloadAsNeeded)
       *  \note \ref DownloadOnly also implies a \ref dryRun.
       */
      ZYppCommitPolicy & downloadMode( DownloadMode val_r );

      DownloadMode downloadMode() const;


      /** The default \ref target::rpm::RpmInstFlags. (default: none)*/
      ZYppCommitPolicy &  rpmInstFlags( target::rpm::RpmInstFlags newFlags_r );

      /** Use rpm option --nosignature (default: false) */
      ZYppCommitPolicy & rpmNoSignature( bool yesNo_r );

      /** Use rpm option --excludedocs (default: false) */
      ZYppCommitPolicy & rpmExcludeDocs( bool yesNo_r );

      target::rpm::RpmInstFlags rpmInstFlags() const;

      bool rpmNoSignature() const;

      bool rpmExcludeDocs() const;


      /** Kepp pool in sync with the Target databases after commit (default: true) */
      ZYppCommitPolicy & syncPoolAfterCommit( bool yesNo_r );

      bool syncPoolAfterCommit() const;

    public:
      /** Implementation  */
      class Impl;
    private:
      /** Pointer to data. */
      RWCOW_pointer<Impl> _pimpl;
  };
  ///////////////////////////////////////////////////////////////////

  /** \relates ZYppCommitPolicy Stream output. */
  std::ostream & operator<<( std::ostream & str, const ZYppCommitPolicy & obj );

  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
#endif // ZYPP_ZYPPCOMMITPOLICY_H
