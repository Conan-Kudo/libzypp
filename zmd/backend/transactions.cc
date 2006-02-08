/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/** file: zmd/backend/transactions.cc
    read/write 'transactions' table to/from ResPool
 */

#include "transactions.h"

#include "zypp/ZYpp.h"
#include "zypp/ZYppFactory.h"

#include "zypp/base/Logger.h"
#include "zypp/base/Exception.h"
#include "zypp/base/Algorithm.h"

#include "zypp/ResPool.h"
#include "zypp/ResFilters.h"
#include "zypp/CapFilters.h"

#include "zypp/solver/detail/ResolverContext.h"
#include "zypp/solver/detail/ResolverInfo.h"

using std::endl;
using namespace zypp;

#include <sqlite3.h>
#include "dbsource/DbAccess.h"
#include "dbsource/DbSources.h"

typedef enum {
    PACKAGE_OP_REMOVE  = 0,
    PACKAGE_OP_INSTALL = 1,
    PACKAGE_OP_UPGRADE = 2		// unused
} PackageOpType;

using namespace std;
using namespace zypp;
using solver::detail::ResolverInfo_Ptr;
using solver::detail::ResolverContext_Ptr;

//-----------------------------------------------------------------------------

bool
read_transactions (const ResPool & pool, sqlite3 *db, const DbSources & sources)
{
    MIL << "read_transactions" << endl;

    sqlite3_stmt *handle = NULL;
    const char  *sql = "SELECT action, id FROM transactions";
    int rc = sqlite3_prepare (db, sql, -1, &handle, NULL);
    if (rc != SQLITE_OK) {
	ERR << "Can not prepare transaction selection clause: " << sqlite3_errmsg (db) << endl;
        return false;
    }

    while ((rc = sqlite3_step (handle)) == SQLITE_ROW) {
        int id;
        PackageOpType action;
	ResObject::constPtr obj;

        action = (PackageOpType) sqlite3_column_int (handle, 0);
        id = sqlite3_column_int (handle, 1);

	obj = sources.getById (id);
	if (obj == NULL) {
	    ERR << "Can't find resolvable id " << id << endl;
	    break;
	}

	ResPool::const_nameiterator pend = pool.nameend(obj->name());
	for (ResPool::const_nameiterator it = pool.namebegin(obj->name()); it != pend; ++it) {
	    PoolItem pos = it->second;
	    if (pos.resolvable() == obj)
	    {
		MIL << "Found item " << id << ": " << pos << ", action " << action << endl;
		switch (action) {
		    case PACKAGE_OP_REMOVE:
			pos.status().setToBeUninstalled(ResStatus::USER);
			break;
		    case PACKAGE_OP_INSTALL:
			pos.status().setToBeInstalled(ResStatus::USER);
			break;
		    case PACKAGE_OP_UPGRADE:
			pos.status().setToBeInstalled(ResStatus::USER);
			break;
		    default:
			ERR << "Ignoring unknown action " << action << endl;
			break;
		}
		break;
	    }
	}
    }

    sqlite3_finalize (handle);

    if (rc != SQLITE_DONE) {
	ERR << "Error reading transaction packages: " << sqlite3_errmsg (db) << endl;
	return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

static void
insert_item( PoolItem_Ref item, const ResStatus & status, void *data)
{
    PoolItemSet *pis = (PoolItemSet *)data;
    pis->insert( item );
}

static void
insert_item_pair (PoolItem_Ref install, const ResStatus & status1, PoolItem_Ref remove, const ResStatus & status2, void *data)
{
    PoolItemSet *pis = (PoolItemSet *)data;
    pis->insert( install );		// only the install
}



static void
dep_get_package_info_cb (ResolverInfo_Ptr info, void *user_data)
{
    string *msg = (string *)user_data;

    msg->append( info->message() );
    msg->append( "|" );

} /* dep_get_package_info_cb */


static string
dep_get_package_info (ResolverContext_Ptr context, PoolItem_Ref item)
{
    string info;

    context->foreachInfo (item, RESOLVER_INFO_PRIORITY_USER, dep_get_package_info_cb, &info);

    return info;
} /* dep_get_package_info */


bool
write_resobject_set( sqlite3_stmt *handle, const PoolItemSet & objects, PackageOpType op_type, ResolverContext_Ptr context)
{
    int rc = SQLITE_DONE;

    for (PoolItemSet::const_iterator iter = objects.begin(); iter != objects.end(); ++iter) {

	string details = dep_get_package_info( context, *iter );

        sqlite3_bind_int (handle, 1, (int) op_type);
        sqlite3_bind_int (handle, 2, iter->resolvable()->zmdid());
        sqlite3_bind_text (handle, 3, details.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step (handle);
        sqlite3_reset (handle);
    }
    return (rc == SQLITE_DONE);
}


bool
write_transactions (const ResPool & pool, sqlite3 *db, ResolverContext_Ptr context)
{
    MIL << "write_transactions" << endl;

    sqlite3_stmt *handle = NULL;
    const char *sql = "INSERT INTO transactions (action, id, details) VALUES (?, ?, ?)";
    int rc = sqlite3_prepare (db, sql, -1, &handle, NULL);
    if (rc != SQLITE_OK) {
	ERR << "Can not prepare transaction insertion clause: " << sqlite3_errmsg (db) << endl;
        return false;
    }
    PoolItemSet install_set;
    PoolItemSet remove_set;

    context->foreachInstall( insert_item, &install_set);
    context->foreachUninstall( insert_item, &remove_set);
    context->foreachUpgrade( insert_item_pair, &install_set);

    bool result;
    result = write_resobject_set (handle, install_set, PACKAGE_OP_INSTALL, context);
    if (!result) {
	ERR << "Error writing transaction install set: " << sqlite3_errmsg (db) << endl;
    }
    result = write_resobject_set (handle, remove_set, PACKAGE_OP_REMOVE, context);
    if (!result) {
	ERR << "Error writing transaction remove set: " << sqlite3_errmsg (db) << endl;
    }

    sqlite3_finalize (handle);

    return result;
}




