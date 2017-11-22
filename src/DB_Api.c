#include "main.h"

SQLHANDLE ghEnv = NULL;
SQLHANDLE ghDbc = NULL;
SQLHANDLE ghStmt = NULL;

void writelog(const char *fmt, ...)
{
#define K8 (1024*8)
    char caLog[K8] = "";
    va_list args;

    va_start(args, fmt);
    vsnprintf(caLog, K8-1, fmt, args);
    va_end(args);

    printf("%s\n", caLog);
}

#define FREE(mem) if(mem) free(mem)

#define Tracer(str) __tracer(__FILE__, __LINE__, __func__, str)
#define TracerNumber(v) __tracerNumber(__FILE__, __LINE__, __func__, v)

void __tracer(const char *file, int line, const char *func, const char *str)
{
    writelog("[%s(%d)-%s] %s", file, line, func, str);
}
void __tracerNumber(const char *file, int line, const char *func, const int v)
{
    writelog("[%s(%d)-%s] %d", file, line, func, v);
}

static SQLINTEGER __DBApiFreeHandle(SQLSMALLINT hType, SQLHANDLE hHandle)
{
    SQLCHAR caMsgText[K8] = "";

    SQLRETURN iRet = SQLFreeHandle(hType, hHandle);
    if(!SQL_SUCCEEDED(iRet))
    {
        if(DBOP_OK == DBApiGetErrorInfo(hType, hHandle, caMsgText))
        {
            writelog("%s", caMsgText);
        }
        return DBOP_NO;
    }
    return DBOP_OK;
}

SQLINTEGER DBApiFreeEnv(SQLHENV hEnv)
{
    return __DBApiFreeHandle(SQL_HANDLE_ENV, (SQLHANDLE)hEnv);
}

SQLINTEGER DBApiFreeDbc(SQLHDBC hDbc)
{
    return __DBApiFreeHandle(SQL_HANDLE_DBC, (SQLHANDLE)hDbc);
}

SQLINTEGER DBApiFreeStmt(SQLHSTMT hStmt)
{
    return __DBApiFreeHandle(SQL_HANDLE_STMT, (SQLHANDLE)hStmt);
}

/**
 * __DBApiCheckSQLReturn : 
 *
 * @hType : 
 * @hHandle :
 * @iRetValue :
 *
 * return values
 * DBOP_OK : SQL_SUCCEED or SQL_SUCCESS_WITH_INFO
 * DBOP_NO : other values
 */
static SQLINTEGER __DBApiCheckSQLReturn(SQLSMALLINT hType,
        SQLHANDLE   hHandle,
        SQLINTEGER  iRetValue)
{
    SQLCHAR caMessageText[1024*4] = "";

    if(iRetValue == SQL_SUCCESS_WITH_INFO)
    {
        if(DBOP_OK == DBApiGetErrorInfo(hType, hHandle, caMessageText))
        {
            writelog("%s", caMessageText);
        }
    }
    else if(SQL_SUCCESS != iRetValue)
    {
        if(DBOP_OK == DBApiGetErrorInfo(hType, hHandle, caMessageText))
        {
            writelog("%s", caMessageText);
        }
        return DBOP_NO;
    }
    return DBOP_OK;
}

SQLINTEGER DBApiInitEnv(SQLHENV *hEnv, SQLHDBC *hDbc)
{
    SQLRETURN iRet = SQL_SUCCESS;

    iRet = SQLAllocHandle(SQL_HANDLE_ENV, NULL, hEnv);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_ENV, *hEnv, iRet))
    {
        return DBOP_NO;
    }

    /**
     * Set ODBC as Version-3 to support more api
     */
    SQLSetEnvAttr(*hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3,
            SQL_IS_INTEGER);

    SQLSetEnvAttr(*hEnv, SQL_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF,
            SQL_IS_INTEGER);

    iRet = SQLAllocHandle(SQL_HANDLE_DBC, *hEnv, hDbc);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_DBC, *hDbc, iRet))
    {
        DBApiFreeEnv(hEnv);
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER DBApiConnectDatabase(SQLHDBC *hDbc,
        SQLCHAR *pcaDBName,
        SQLCHAR *pcaDBUserName,
        SQLCHAR *pcaDBUserPassword)
{
    SQLRETURN iRet = SQL_SUCCESS;

    iRet = SQLConnect(*hDbc, pcaDBName, SQL_NTS, pcaDBUserName, SQL_NTS,
            pcaDBUserPassword, SQL_NTS);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_DBC, *hDbc, iRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER DBApiDisConnectDatabase(SQLHDBC hDbc)
{
    SQLRETURN iRet = SQL_SUCCESS;

    iRet = SQLDisconnect(hDbc);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_DBC, hDbc, iRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER DBApiPreExecSQL(SQLHDBC hDbc, SQLHSTMT *hStmt)
{
    SQLRETURN iRet = SQL_SUCCESS;

    iRet = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, hStmt);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, *hStmt, iRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER DBApiExecSQL(SQLHSTMT hStmt, SQLCHAR *pcaSqlStmt)
{
    SQLRETURN iRet = SQL_SUCCESS;

    iRet = SQLExecDirect(hStmt, pcaSqlStmt, SQL_NTS);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, iRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER __DBGetColsInfo(SQLHSTMT hStmt, TableCacheList *pTableInfo)
{
    SQLRETURN   iRet       = SQL_SUCCESS;
    SQLINTEGER  iTotalSize = 0;
    SQLSMALLINT iLoop      = 0;
    SQLSMALLINT iColCnt    = 0;

    SQLULEN     iColSize       = 0;
    SQLCHAR     caColName[100] = "";
    SQLSMALLINT iColAttrLen    = 0;
    SQLSMALLINT iColType       = 0;
    SQLSMALLINT iColDecDigit   = 0;
    SQLSMALLINT iColNullable   = 0;

    FieldCacheList *pFieldList = NULL;

    iRet = SQLNumResultCols(hStmt, &iColCnt);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, iRet))
    {
        return DBOP_NO;
    }

    pTableInfo->FieldCounter = iColCnt;

    if(iColCnt==0)
    {
        return DBOP_NO;
    }

    if(NULL==NEW_FIELD_LIST(pFieldList, iColCnt))
    {
        return DBOP_NO;
    }

    for(iLoop=1; iLoop<=iColCnt; iLoop++)
    {
        iRet = SQLDescribeCol(hStmt,
                iLoop,
                caColName,
                sizeof(caColName),
                &iColAttrLen,
                &iColType,
                &iColSize,
                &iColDecDigit,
                &iColNullable);

        if(iRet == SQL_ERROR)
        {
            goto err;
        }

        (pFieldList+iLoop-1)->FieldSize = iColSize;
        (pFieldList+iLoop-1)->FieldType = iColType;
        sprintf((pFieldList+iLoop-1)->FieldName, "%s", caColName);

        iTotalSize += iColSize;
    }

    pTableInfo->FieldList = pFieldList;

    /* Add one char '\0' for every end-of-c-string */
    pTableInfo->TotalSize = iTotalSize+iColCnt;

    return DBOP_OK;

err:
    if(pFieldList)
        free(pFieldList);
    return DBOP_NO;
}

SQLINTEGER DBApiQuery(SQLHSTMT hStmt, SQLCHAR *pcaSqlStmt, DBQueryResult **pDBQueryRes)
{
    SQLRETURN   iRet    = SQL_SUCCESS;
    SQLLEN      iRowCnt = 0; /* Row counter of query result     */
    SQLSMALLINT iColCnt = 0; /* Columns counter of query result */
    SQLINTEGER  iResOfs = 0;

    TableCacheList stTableList = {
        .TotalSize    = 0,
        .FieldCounter = 0,
        .FieldList    = NULL,
    };

    DBQueryResult *pstDBQueryRes = NULL;

    iRet = SQLExecDirect(hStmt, pcaSqlStmt, SQL_NTS);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, iRet))
    {
        return DBOP_NO;
    }

    if(DBOP_OK != __DBGetColsInfo(hStmt, &stTableList))
    {
        return DBOP_NO;
    }
    iColCnt = stTableList.FieldCounter;

    iRet = SQLRowCount(hStmt, &iRowCnt);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, iRet))
    {
        goto err;
    }

    /*
    if(NULL==(pstDBQueryRes=NEW_DB_QUERY_RESULT(iRowCnt, stTableList.TotalSize)))
    {
        goto err;
    }
    */

    SQLLEN iRealSize = 0;
    SQLCHAR caColVal[100] = "";
    for(;;)
    {
        iRet =  SQLFetch(hStmt);

        if(iRet == SQL_ERROR || iRet == SQL_SUCCESS_WITH_INFO)
        {
            if(iRet == SQL_ERROR)
                break;
        }
        else if(iRet == SQL_NO_DATA)
        {
            break;
        }

        SQLSMALLINT iColLoop;
        SQLSMALLINT iFieldMaxSize = 0;
        for(iColLoop=1; iColLoop <= iColCnt; iColLoop++)
        {
            iFieldMaxSize = (stTableList.FieldList+iColLoop-1)->FieldSize;
            iRet = SQLGetData(hStmt, iColLoop, SQL_C_CHAR, caColVal, 100, &iRealSize);

            if(iRet == SQL_ERROR || iRet == SQL_SUCCESS_WITH_INFO)
            {
                if(iRet == SQL_ERROR)
                    break;
            }
            else if(iRet == SQL_NO_DATA)
            {
                break;
            }
            /*
            memcpy(pstDBQueryRes->ResultSet+iResOfs, (void*)caColVal, iRealSize);
            iResOfs += (iFieldMaxSize+1);
            */
        }
    }
    TracerNumber(stTableList.TotalSize);

    /*
    *pDBQueryRes = pstDBQueryRes;
    (*pDBQueryRes)->ResultCounter   = iRowCnt;
    (*pDBQueryRes)->TableSize       = stTableList.TotalSize;
    */

    FREE(stTableList.FieldList);

    return DBOP_OK;

err:
    FREE(stTableList.FieldList);
    return DBOP_NO;
}

SQLINTEGER DBApiGetErrorInfo(SQLSMALLINT hType,
        SQLHANDLE   hHandle,
        SQLCHAR     *pcaMsgText)
{
    SQLCHAR caSqlState[8] = "";
    SQLCHAR caMsgText[SQL_MAX_MESSAGE_LENGTH] = "";

    SQLINTEGER iNativeError;
    SQLINTEGER iTextRealLength;
    SQLSMALLINT iRecNumber;
    SQLSMALLINT iTextLength;

    SQLRETURN iRet = SQL_SUCCESS;

    iRecNumber = 1;
    iTextRealLength = 0;

    while(1)
    {
        iRet=SQLGetDiagRec(hType,
                hHandle,
                iRecNumber,
                caSqlState,
                &iNativeError,
                caMsgText,
                SQL_MAX_MESSAGE_LENGTH-1,
                &iTextLength);
        if(SQL_SUCCEEDED(iRet))
        {
            iTextRealLength += sprintf((char*)pcaMsgText+iTextRealLength, "%s", caMsgText);

            if(iTextRealLength >= 1024*4 )
                *(pcaMsgText+1024*4) = 0;
            *caMsgText = 0;
            iRecNumber ++;
        }
        else
        {
            break;
        }
    }

    if(iRet != SQL_NO_DATA || !iTextRealLength)
        return DBOP_NO;

    return DBOP_OK;
}

/*
void *NEW_DB_QUERY_RESULT(int nmemb, int size)
{
    DBQueryResult *p = NULL;
    p = malloc(SIZE_OF_DB_QUERY_RESULT());
    p->ResultSet = malloc(nmemb*size);
    memset(p->ResultSet, 0x0, nmemb*size);
    return p;
}
*/

//#define ITER_AUTO(type) struct list_head *type

/*
static inline struct list_head *begin(DBQueryResult *dbqr)
{
    dbqr->iter = dbqr->root->next;
    return dbqr->iter;
}

static inline struct list_head *next(DBQueryResult *dbqr)
{
    dbqr->iter = dbqr->iter->next;
    return dbqr->iter;
}

static inline struct list_head *end(DBQueryResult *dbqr)
{
    return dbqr->root;
}

static inline void *fetch(struct list_head *iter)
{
    return list_entry(iter, DBRow, list);
}

void InitDbQueryResult(DBQueryResult *dbqr)
{
    dbqr->TableSize     = 0;
    dbqr->ResultCounter = 0;
    dbqr->begin         = begin;
    dbqr->next          = next;
    dbqr->end           = end;
    dbqr->fetch         = fetch;
    return;
}

*/
