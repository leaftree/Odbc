
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

typedef struct list_head list_head;

typedef struct FIELD_ATTR FIELD_ATTR;
typedef struct TABLE_STRUCTURE TABLE_STRUCTURE;

/**
 * 字段域信息
 */
struct FIELD_ATTR
{
	int              nFieldSize;
	int              nFieldType;
	char             szFieldName[128];
	struct list_head List;
};

/**
 * 表结构信息
 */
struct TABLE_STRUCTURE
{
	int         nTotalSize;
	int         nFieldCounter;
	char        szTableName[128];
	list_head  *pCursor;
	list_head  *pListRoot;

	void (*FreeFieldAttrList)     (TABLE_STRUCTURE *);
	int  (*AddFieldAttrNodeTail)  (TABLE_STRUCTURE *, FIELD_ATTR *);
	int  (*FetchNextFieldAttrNode)(TABLE_STRUCTURE *, FIELD_ATTR **);
};

void *NewFieldAttrNode();
TABLE_STRUCTURE *NewTableStruct();
void FreeFieldAttrList(TABLE_STRUCTURE *pTabStruct);
int AddFieldAttrNode(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR *new);
int AddFieldAttrNodeTail(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR *pNew);
int FetchNextFieldAttrNode(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR **ppField);

/**
 * 库表行记录
 */
typedef struct ROW_DATA
{
	int               nLength;
	void             *pValue;
	struct list_head  List;
} ROW_DATA;

/**
 * 库表查询结果集合
 */
typedef struct DB_QUERY_RESULT_SET
{
	int               nTableSize;
	int               nRowCounter;
	//struct list_head *pList;
	TABLE_STRUCTURE  *pTableStruct;
	SQLHDBC           hDbc;
	SQLHSTMT          hStmt;
} DB_QUERY_RESULT_SET;

#if 0
/**
 * 字段域信息
 */
typedef struct FILDE_ATTR
{
	int              nFieldSize;
	int              nFieldType;
	char             szFieldName[128];
	struct list_head List;
} FIELD_ATTR;

/**
 * 表结构信息
 */
typedef struct TABLE_INFO
{
	int         nTotalSize;
	int         nFieldCounter;
	char        szTableName[128];
	FIELD_ATTR *pFieldAttrList;
} TABLE_STRUCTURE;

/**
 * 库表行记录
 */
typedef struct ROW_DATA
{
	struct list_head  List;
	void             *pValue;
} ROW_SET;

/**
 * 库表查询结果集合
 */
typedef struct DB_QUERY_RESULT_SET
{
	struct list_head *ListRoot;
	int               nTableSize;
	int               nRowCounter;
	TABLE_STRUCTURE   TableStruct;
	SQLHDBC           hDbc;
	SQLHSTMT          hStmt;
} DB_QUERY_RESULT_SET;
#endif

#define SIZE_OF_FIELD_CACHE(field) \
	(sizeof(TableCacheList))

#define NEW_FIELD_LIST(field, cnt) \
	(field=(void*)malloc(sizeof(FieldCacheList)*cnt))

#define SIZE_OF_DB_QUERY_RESULT() \
	(sizeof(DBQueryResult))

#define INIT_ONE_LIST_ROOT(root) LIST_HEAD(root)

#define INSERT_ONE_RECORD_INTO_LIST(ptr, root) \
	list_add((ptr)->Root.list, &(root))

#define INSERT_ONE_RECORD_INTO_LIST_TAIL(new, root) \
	list_add_tail(&((new)->list), &(root))

#define DELETE_ONE_RECORD_FROM_LIST(ptr) \
	list_del(&((ptr)->Root).list)

#define DB_INIT_POS(pos) DBRow *pos

#define DB_FOREACH_ENTRY(pos, root) \
	list_for_each_entry(pos, &(root), list)

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

/*
	 SQLINTEGER DBApiQuery(SQLHSTMT             hStmt,
	 SQLCHAR             *pcaSqlStmt,
	 DB_QUERY_RESULT_SET *pDBQueryRes);
	 */

SQLINTEGER DBApiGetErrorInfo(SQLSMALLINT hType,
		SQLHANDLE   hHandle,
		SQLCHAR    *pcaMsgText);

#endif /* _DB_API_H_ */
