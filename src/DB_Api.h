
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

typedef struct list_head           list_head;
typedef struct FIELD_ATTR          FIELD_ATTR;
typedef struct TABLE_STRUCTURE     TABLE_STRUCTURE;
typedef struct DB_TABLE_INFO_SET   DB_TABLE_INFO_SET;
typedef struct DB_TABLE_SET_HEAD   DB_TABLE_SET_HEAD;
typedef struct DB_QUERY_RESULT_SET DB_QUERY_RESULT_SET;

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
	int         nMaxFieldSize;
	int         nFieldCounter;
	char        szTableName[128];
	list_head  *pCursor;
	list_head  *pListRoot;

	void *(*NewField) ();
	void  (*Free)     (TABLE_STRUCTURE *);
	int   (*AddTail)  (TABLE_STRUCTURE *, FIELD_ATTR *);
	int   (*AddHead)  (TABLE_STRUCTURE *, FIELD_ATTR *);
	int   (*Next)     (TABLE_STRUCTURE *, FIELD_ATTR **);
};

/**
 * 库表行记录
 */
typedef struct ROW_DATA
{
	struct list_head  List;
	int               nLength;
	void             *pValue;
} ROW_DATA;

/**
 * 库表结构列表
 */
struct DB_TABLE_INFO_SET
{
	list_head     List;
	char         *pszTableName;
	TABLE_STRUCTURE *pTableStruct;
};

/**
 * 库表结构列表头
 */
struct DB_TABLE_SET_HEAD
{
	list_head List;
	void *(*New)();
	void *(*Find)(DB_TABLE_SET_HEAD *head, char *pszTableName);
	int (*AddHead)(DB_TABLE_SET_HEAD *head, DB_TABLE_INFO_SET *new);
};

/**
 * 库表查询结果集合
 */
struct DB_QUERY_RESULT_SET
{
	int               nTableSize;
	int               nRowCounter;
	TABLE_STRUCTURE  *pTableStruct;
	ROW_DATA         *pRow;
	list_head        *pRowCursor;
	SQLHDBC           hDbc;
	SQLHSTMT          hStmt;

	void             *(*New)     (int nSize);
	void              (*Free)    (ROW_DATA **);
	void              (*Destroy) (DB_QUERY_RESULT_SET **);
	int               (*Next)    (DB_QUERY_RESULT_SET *, ROW_DATA **);
	int               (*AddHead) (DB_QUERY_RESULT_SET *, ROW_DATA *);
	int               (*AddTail) (DB_QUERY_RESULT_SET *, ROW_DATA *);
};

int FetchNextRow(DB_QUERY_RESULT_SET *pDbQrs, ROW_DATA **pRow);
void *InitDbQuerySet(SQLHDBC hDbc, SQLHSTMT hStmt);
void FreeDbQuerySet(DB_QUERY_RESULT_SET **pDbQrs);
void FreeDBRow(ROW_DATA **pRow);
int DBRowAddHead(DB_QUERY_RESULT_SET *pDBQrs, ROW_DATA *pRow);
int DBRowAddTail(DB_QUERY_RESULT_SET *pDBQrs, ROW_DATA *pRow);

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

SQLINTEGER DBApiQuery(DB_QUERY_RESULT_SET *pDBQueryRes,
                      SQLCHAR             *pcaSqlStmt);

SQLINTEGER DBApiGetErrorInfo(SQLSMALLINT hType,
		                         SQLHANDLE   hHandle,
		                         SQLCHAR    *pcaMsgText);

#endif /* _DB_API_H_ */
