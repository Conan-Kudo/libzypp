/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/parser/yum/schemanames.h
 *
*/


#ifndef schemanames_h
#define schemanames_h

namespace zypp {
  namespace parser {
    namespace yum {
    /* FIXME: How do I do this properly? */
    #define SCHEMABASE "/usr/share/YaST2/schema/packagemanager/"
    #define REPOMDSCHEMA (SCHEMABASE "repomd.rng")
    #define PRIMARYSCHEMA (SCHEMABASE "suse-primary.rng")
    #define GROUPSCHEMA (SCHEMABASE "groups.rng")
    #define FILELISTSCHEMA (SCHEMABASE "filelists.rng")
    #define OTHERSCHEMA (SCHEMABASE "other.rng")
    #define PATCHSCHEMA (SCHEMABASE "patch.rng")
    #define PRODUCTSCHEMA (SCHEMABASE "product.rng")
    } // namespace yum
  } // namespace parser
} // namespace zypp


#endif
