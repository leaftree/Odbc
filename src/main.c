
#include "DB_Api.h"
#define Tracer(str) __tracer(__FILE__, __LINE__, __func__, str)

#if 0
static void ListDSN(SQLHANDLE hEnv);
static void GetError(SQLINTEGER hType, SQLHENV EnvironmentHandle, SQLHDBC ConnectionHandle, SQLHSTMT StatementHandle);

struct type {
    char *pname;
} Type[] = {
    [SQL_UNKNOWN_TYPE   ] = "SQL_UNKNOWN_TYPE",
    [SQL_CHAR           ] = "SQL_CHAR",
    [SQL_NUMERIC        ] = "SQL_NUMERIC",
    [SQL_DECIMAL        ] = "SQL_DECIMAL",
    [SQL_INTEGER        ] = "SQL_INTEGER",
    [SQL_SMALLINT       ] = "SQL_SMALLINT",
    [SQL_FLOAT          ] = "SQL_FLOAT",
    [SQL_REAL           ] = "SQL_REAL",
    [SQL_DOUBLE         ] = "SQL_DOUBLE",
    [SQL_VARCHAR        ] = "SQL_VARCHAR",
    [SQL_DATETIME       ] = "SQL_DATETIME",
    [SQL_TYPE_DATE      ] = "SQL_TYPE_DATE",
    [SQL_TYPE_TIME      ] = "SQL_TYPE_TIME",
    [SQL_TYPE_TIMESTAMP ] = "SQL_TYPE_TIMESTAMP",
};

int main()
{
    SQLRETURN sRes = 0;

    SQLHANDLE hEnv  = NULL;
    SQLHANDLE hDbc  = NULL;
    SQLHANDLE hStmt = NULL;
    /**
     * SQL_HANDLE_ENV
     * SQL_HANDLE_DBC
     * SQL_HANDLE_STMT
     */
    sRes = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &hEnv);
    printf("LINE[%d] %d\n", __LINE__, sRes);

    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);

    ListDSN(hEnv);

    sRes = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    printf("LINE[%d] %d\n", __LINE__, sRes);

    sRes = SQLConnect(hDbc, "MySQL", SQL_NTS, "mysql", SQL_NTS, "mysqlv", SQL_NTS);
    //sRes = SQLConnect(hDbc, "MySQL", -3, "mysql", 0, "mysql", 0);
    printf("LINE[%d] %d\n", __LINE__, sRes);
    if(sRes != SQL_SUCCESS_WITH_INFO && sRes != SQL_SUCCESS)
    {
        GetError(SQL_HANDLE_DBC, hEnv, hDbc, hStmt);
        return -1;
    }

    sRes = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    printf("LINE[%d] %d\n", __LINE__, sRes);
    if(sRes != SQL_SUCCESS_WITH_INFO && sRes != SQL_SUCCESS)
    {
        GetError(SQL_HANDLE_STMT, hEnv, hDbc, hStmt);
        return -1;
    }

    SQLCHAR sqlUsed[100] = "use test";
    SQLCHAR sqlStmt[100] = "select * from table_v1";

    sRes = SQLExecDirect(hStmt, sqlUsed, SQL_NTS);
    printf("LINE[%d] %d\n", __LINE__, sRes);
    if(sRes != SQL_SUCCESS_WITH_INFO && sRes != SQL_SUCCESS)
    {
        GetError(SQL_HANDLE_STMT, hEnv, hDbc, hStmt);
        return -1;
    }

    sRes = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
    printf("LINE[%d] %d\n", __LINE__, sRes);

    //sRes = SQLExecDirect(hStmt, "delete from table_v1 where id > 20", SQL_NTS);
    /*
    int i=0;
    for(i=1; i<10000; i++)
    {
        sprintf(sqlStmt, "insert into table_v1 values(%d, 'ssy', 'my love', '2017-11-11 11:11:11')",
                i);
        sRes = SQLExecDirect(hStmt, sqlStmt, SQL_NTS);
        printf("LINE[%d] %d %d\n", __LINE__, sRes, i);
        if(sRes == SQL_ERROR)
            break;
    }
    */

    printf("¶ÁÈ¡Êý¾Ý...\n");

    int iColCnt = 0;
    SQLLEN iRealSize = 0;
    SQLCHAR caColVal[100] = "";
    while(1==1)
    {
        sRes =  SQLFetch(hStmt);

        if(sRes == SQL_ERROR || sRes == SQL_SUCCESS_WITH_INFO)
        {
            printf("error SQLFetch [%d]\n", sRes);
            if(sRes == SQL_ERROR)
                break;
        }
        else if(sRes == SQL_NO_DATA)
        {
            printf("SQLFetch data end...\n");
            break;
        }

        SQLSMALLINT iCols = 0;
        sRes = SQLNumResultCols(hStmt, &iCols);
        //printf("LINE[%d] %d\n", __LINE__, sRes);

        if(sRes == SQL_ERROR || sRes == SQL_INVALID_HANDLE)
            break;
        
        //printf("idx name    Type    Size    DecDig  Nullable\n");
        /*
        printf("%6s%10s%25s%5s%8s%10s\n", "idx", "name", "Type", "Size", "DecDig", "Nullable");
        int i=0;
        for(i=1; i<=iCols; i++)
        {
            SQLCHAR ColName[100] = "";
            SQLSMALLINT ColAttrLen = 0;
            SQLSMALLINT ColType=-1;
            SQLULEN ColSize;
            SQLSMALLINT ColDecDigit;
            SQLSMALLINT ColNullable;
            sRes = SQLDescribeCol(hStmt,
                    i,
                    ColName,
                    100,
                    &ColAttrLen,
                    &ColType,
                    &ColSize,
                    &ColDecDigit,
                    &ColNullable);

            if(sRes == SQL_ERROR)
            {
                printf("SQLDescribeCol error %d\n", sRes);
                break;
            }
            //printf("%6d%10s%25s%5u%8d%10d\n", i, ColName, Type[ColType], ColSize, ColDecDigit, ColNullable);
        }
        */

        iColCnt=1;
        while( iColCnt <= iCols )
        {
            sRes = SQLGetData(hStmt, iColCnt, SQL_C_CHAR, caColVal, 100, &iRealSize);

            if(sRes == SQL_ERROR || sRes == SQL_SUCCESS_WITH_INFO)
            {
                printf("error SQLGetData [%d]\n", sRes);
                if(sRes == SQL_ERROR)
                    break;
            }
            else if(sRes == SQL_NO_DATA)
            {
                printf("SQLGetData data end...\n");
                break;
            }

            iColCnt++;
            printf("%s  ", caColVal);
        }
        printf("\n");
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    return 0;
}

void GetError(SQLINTEGER iType, SQLHENV hEnv, SQLHDBC hDbc, SQLHSTMT hStmt)
{
    /*
    SQLINTEGER NativeError = 0;
    SQLSMALLINT TextLength = 0;
    SQLSMALLINT BufferLength = 512;

    SQLCHAR SqlState[512] = "";
    SQLCHAR MessageText[512] = "";

    SQLRETURN sRes = SQL_SUCCESS;

    while(SQL_SUCCESS == sRes)
    {
        sRes = SQLError(hEnv,
                hDbc,
                hStmt,
                SqlState,
                &NativeError,
                MessageText,
                BufferLength,
                &TextLength);
        if(sRes == SQL_NO_DATA)
            break;

        printf("[%s(%d)-%s] SQLError res[%d]\n", __FILE__, __LINE__, __func__, sRes);
        if(sRes != SQL_SUCCESS && sRes != SQL_SUCCESS_WITH_INFO)
            return;

        printf("[%s(%d)-%s] SQLError Message: %s\n", __FILE__, __LINE__, __func__, MessageText);
    }
    */

    SQLHANDLE hType;
    SQLCHAR caSqlState[6] ;
    SQLCHAR caSqlStmt[100];
    SQLCHAR caSqlMsg[SQL_MAX_MESSAGE_LENGTH] ;
    SQLINTEGER NativeError;
    SQLSMALLINT i, MsgLen;

    SQLRETURN sRes = SQL_SUCCESS;

    printf("iType %d\n", iType);
    switch(iType)
    {
        case SQL_HANDLE_ENV:
            hType = hEnv; break;
        case SQL_HANDLE_DBC:
            hType = hDbc; break;
        case SQL_HANDLE_STMT:
            hType = hStmt; break;
        default:
            return ;
    }

    i = 1;
    while((sRes=SQLGetDiagRec(iType, hType, i, caSqlState,
                    &NativeError, caSqlMsg, sizeof(caSqlMsg), 
                    &MsgLen)) != SQL_NO_DATA)
    {
        i++;

        printf("LoopTime(%d) State[%s] NativeError(%d) SqlMsg[%s] SqlMsgLen(%d)""\n",
                i, caSqlState, NativeError, caSqlMsg, MsgLen);
    }
    // SQLGetDiagField
    printf("LINE(%d) sRes(%d)\n", __LINE__, sRes);
}

void ListDSN(SQLHANDLE hEnv)
{
    SQLCHAR dsn[100] = "", desc[100] = "";
    SQLSMALLINT len1, len2;
    SQLUSMALLINT next;

    next = SQL_FETCH_FIRST;
    while(SQLDataSources(hEnv, next,
                dsn, 100, &len1,
                desc, 100, &len2) == SQL_SUCCESS)
    {
        printf("Server=(%s) Beschreibung=(%s)\n", dsn, desc);
        next = SQL_FETCH_NEXT;
    }
}
#endif

#include "DB_Api.h"

extern SQLHANDLE ghEnv ;
extern SQLHANDLE ghDbc ;
extern SQLHANDLE ghStmt ;

int main()
{
    SQLRETURN iRet = SQL_SUCCESS;

    SQLHENV  hEnv  = NULL;
    SQLHDBC  hDbc  = NULL;
    SQLHSTMT hStmt = NULL;

    SQLCHAR caSqlStmt[1024] = "";

    if(DBOP_NO == DBApiInitEnv(&hEnv, &hDbc))
    {
			Tracer("DBApiInitEnv");
        return 1;
    }
    ghEnv = hEnv;
    ghDbc = hDbc;

    if(DBOP_NO == DBApiConnectDatabase(&hDbc, "MySQL", "root", "123kbc,./"))
    {
			Tracer("DBApiConnectDatabase");
        return 1;
    }

    if(DBOP_NO == DBApiPreExecSQL(hDbc, &hStmt))
    {
        return 1;
    }
    ghStmt = hStmt;

    Tracer("DBApiExecSQL beg");
    sprintf(caSqlStmt, "use fyl");
    if(DBOP_NO == DBApiExecSQL(hStmt, caSqlStmt))
    {
        return 1;
    }
    Tracer("DBApiExecSQL end");

    sprintf(caSqlStmt, "select * from BASI_STATION_INFO ");
		DBQueryResult stDbQueryHandle;

		void *xp = malloc(1000);
		iRet = DBApiQuery( hStmt,  caSqlStmt, &stDbQueryHandle, xp);
		if(iRet != DBOP_OK)
		{
			Tracer("fail to query data");
		}

		Tracer("end to query data");

		Tracer("DBApiFreeStmt");
    DBApiFreeStmt(hStmt);
		Tracer("DBApiDisConnectDatabase");
    DBApiDisConnectDatabase(hDbc);
		Tracer("DBApiFreeDbc");
    DBApiFreeDbc(hDbc);
		Tracer("DBApiFreeEnv");
    DBApiFreeEnv(hEnv);

    Tracer("Main End");

    return 0;
}
