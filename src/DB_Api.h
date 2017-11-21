
#ifndef _DB_API_H_
#define _DB_API_H_ 1

#include <sqlext.h>

#include "main.h"

#ifdef true
# undef true
# define true 0
#endif

#ifdef false
# undef false
# define false (!true)
#endif

#define DBOP_OK (SQL_SUCCESS)
#define DBOP_NO (SQL_ERROR)

#define DB_NO_DATA 100

#ifdef SUCC
# undef DBOP_OK
# define DBOP_OK (SUCC)
#endif

#ifdef FAIL
# undef DBOP_NO
# define DBOP_NO (FAIL)
#endif

#ifdef NOT_FOUND
# undef DB_NO_DATA
# define DB_NO_DATA (NOT_FOUND)
#endif

typedef struct filde_cache_list_ {
	int  FieldSize;
	int  FieldType;
	char FieldName[128];
} FieldCacheList;

typedef struct table_cache_list_ {
	int             TotalSize;
	int             FieldCounter;
	char            TableName[128];
	FieldCacheList *FieldList;
} TableCacheList;

typedef struct db_row_data_ {
	struct list_head  list;
	void             *value;
} DBRow;

typedef struct db_query_result_ {
	int               TableSize;
	int               ResultCounter;
	struct list_head *root;
} DBQueryResult;

#define SIZE_OF_FIELD_CACHE(field) \
	(sizeof(TableCacheList))

#define NEW_FIELD_LIST(field, cnt) \
	(field=(void*)malloc(sizeof(FieldCacheList)*cnt))

#define SIZE_OF_DB_QUERY_RESULT() \
	(sizeof(DBQueryResult))

#define INIT_ONE_LIST_ROOT(root) LIST_HEAD(root)

#define INSERT_ONE_RECORD_INTO_LIST(ptr, root) \
	list_add((ptr)->Root.list, &(root))

/*
#define INSERT_ONE_RECORD_INTO_LIST_TAIL(ptr, root) \
	list_add_tail(&((ptr)->Root).list, &(root))
	*/
#define INSERT_ONE_RECORD_INTO_LIST_TAIL(new, root) \
	list_add_tail(&((new)->list), &(root))

#define DELETE_ONE_RECORD_FROM_LIST(ptr) \
	list_del(&((ptr)->Root).list)

#define DB_INIT_POS(pos) DBRow *pos

#define DB_FOREACH_ENTRY(pos, root) \
	list_for_each_entry(pos, &(root), list)

#define NEW_DB_QUERY_RESULT(name, list) DBQueryResult name = {0, 0, .root = &list}

//void *NEW_DB_QUERY_RESULT(int nmemb, int size);

SQLINTEGER DBApiInitEnv(SQLHENV *hEnv,
	                    	SQLHDBC *hDbc);

SQLINTEGER DBApiConnectDatabase(SQLHDBC *hDbc,
                                SQLCHAR *pcaDBName,
                                SQLCHAR *pcaDBUserName,
                                SQLCHAR *pcaDBUserPassword);

SQLINTEGER DBApiDisConnectDatabase(SQLHDBC hDbc);

SQLINTEGER DBApiPreExecSQL(SQLHDBC   hDbc,
                           SQLHSTMT *hStmt);

SQLINTEGER DBApiExecSQL(SQLHSTMT hStmt,
                        SQLCHAR *pcaSqlStmt);

SQLINTEGER DBApiFreeEnv(SQLHENV hEnv);

SQLINTEGER DBApiFreeDbc(SQLHDBC hDbc);

SQLINTEGER DBApiFreeStmt(SQLHSTMT hStmt);

SQLINTEGER DBApiQuery(SQLHSTMT        hStmt,
		                  SQLCHAR        *pcaSqlStmt,
		                  DBQueryResult **pDBQueryRes);

SQLINTEGER DBApiGetErrorInfo(SQLSMALLINT hType,
                             SQLHANDLE   hHandle,
                             SQLCHAR    *pcaMsgText);

#endif /* _DB_API_H_ */
