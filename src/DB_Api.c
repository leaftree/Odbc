
#include "DB_Api.h"

SQLHANDLE ghEnv = NULL;
SQLHANDLE ghDbc = NULL;
SQLHANDLE ghStmt = NULL;

SQLINTEGER DBApiGetErrorInfo(SQLSMALLINT hType,
                             SQLHANDLE   hHandle,
                             SQLCHAR     *pcaMsgText);

void writelog(const char *fmt, ...)
{
#define K8 (1024*8)
    char szLog[K8] = "";
    va_list args;

    va_start(args, fmt);
    vsnprintf(szLog, K8-1, fmt, args);
    va_end(args);

    printf("%s\n", szLog);
}

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
    SQLCHAR szMsgText[K8] = "";

    SQLRETURN iRet = SQLFreeHandle(hType, hHandle);
    if(!SQL_SUCCEEDED(iRet))
    {
        if(DBOP_OK == DBApiGetErrorInfo(hType, hHandle, szMsgText))
        {
            writelog("%s", szMsgText);
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
    SQLCHAR szMessageText[1024*4] = "";

    if(iRetValue == SQL_SUCCESS_WITH_INFO)
    {
        if(DBOP_OK == DBApiGetErrorInfo(hType, hHandle, szMessageText))
        {
            writelog("%s", szMessageText);
        }
    }
    else if(SQL_SUCCESS != iRetValue)
    {
        if(DBOP_OK == DBApiGetErrorInfo(hType, hHandle, szMessageText))
        {
            writelog("%s", szMessageText);
        }
        return DBOP_NO;
    }
    return DBOP_OK;
}

SQLINTEGER DBApiInitEnv(SQLHENV *hEnv, SQLHDBC *hDbc)
{
    SQLRETURN iRet = SQL_SUCCESS;

    SQLCHAR szMessageText[1024*4] = "";

    //SQLHENV hEnv  = NULL;
    //SQLHDBC hDbc  = NULL;

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
        if(SQL_NO_DATA == iRet)
            break;
        if(SQL_ERROR == iRet || SQL_INVALID_HANDLE == iRet)
            break;
        if(!SQL_SUCCEEDED(iRet))
        {

            iTextLength ++;
            iTextRealLength += sprintf(pcaMsgText, "%s", caMsgText);

            if(iTextRealLength >= 1024*4 )
                *(pcaMsgText+1024*4) = 0;
        }
        iRecNumber ++;
    }

    if(iRet != SQL_NO_DATA || !iTextRealLength)
        return DBOP_NO;

    return DBOP_OK;
}
