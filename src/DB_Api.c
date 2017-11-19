#include "DB_Api.h"
#include <malloc.h>

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

	SQLCHAR caMessageText[1024*4] = "";

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

	Tracer(__func__);
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

		(pFieldList+iLoop)->FieldSize = iColSize;
		(pFieldList+iLoop)->FieldType = iColType;
		sprintf((pFieldList+iLoop)->FieldName, "%s", caColName);

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

SQLINTEGER DBApiQuery(SQLHSTMT hStmt, SQLCHAR *pcaSqlStmt, DBQueryResult *pDBQueryRes, void *xp)
{
	SQLRETURN   iRet    = SQL_SUCCESS;
	SQLLEN      iRowCnt = 0; /* Row counter of query result     */
	SQLSMALLINT iColCnt = 0; /* Columns counter of query result */

	TableCacheList stTableList = {
		.TotalSize    = 0,
		.FieldCounter = 0,
		.FieldList    = NULL,
	};

	Tracer(__func__);

	DBQueryResult *pstDBQueryRes = NULL;

	iRet = SQLExecDirect(hStmt, pcaSqlStmt, SQL_NTS);
	if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, iRet))
	{
		return DBOP_NO;
	}
	Tracer(__func__);

	/*
	if(DBOP_OK != __DBGetColsInfo(hStmt, &stTableList))
	{
		return DBOP_NO;
	}
	iColCnt = stTableList.FieldCounter;
	Tracer(__func__);
	*/

	iRet = SQLRowCount(hStmt, &iRowCnt);
	if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, iRet))
	{
		goto err;
		//free(stTableList.FieldList);
		//return DBOP_NO;
	}
	Tracer(__func__);

	TracerNumber(iRowCnt);
	TracerNumber(stTableList.TotalSize);
	if(NULL==NEW_DB_QUERY_RESULT(pstDBQueryRes, iRowCnt, stTableList.TotalSize))
	{
		printf("error\n");
		goto err;
	}

	//TracerNumber(sizeof(DBQueryResult)+iRowCnt*stTableList.TotalSize);
	//pstDBQueryRes = malloc(sizeof(DBQueryResult)+iRowCnt/**stTableList.TotalSize*/);
	//if(pstDBQueryRes==NULL)
	//	goto err;
	pDBQueryRes->CurrentPosition = xp;
	pDBQueryRes->ResultSet = xp;
	Tracer(__func__);

	SQLLEN iRealSize = 0;
	SQLCHAR caColVal[100] = "";
	for(;;)
	{
		iRet =  SQLFetch(hStmt);

		if(iRet == SQL_ERROR || iRet == SQL_SUCCESS_WITH_INFO)
		{
			printf("error SQLFetch [%d]\n", iRet);
			if(iRet == SQL_ERROR)
				break;
		}
		else if(iRet == SQL_NO_DATA)
		{
			printf("SQLFetch data end...\n");
			break;
		}

		SQLSMALLINT iColLoop;
		for(iColLoop=1; iColLoop <= iColCnt; iColLoop++)
		{
			iRet = SQLGetData(hStmt, iColLoop, SQL_C_CHAR, caColVal, 100, &iRealSize);

			if(iRet == SQL_ERROR || iRet == SQL_SUCCESS_WITH_INFO)
			{
				printf("error SQLGetData [%d]\n", iRet);
				if(iRet == SQL_ERROR)
					break;
			}
			else if(iRet == SQL_NO_DATA)
			{
				printf("SQLGetData data end...\n");
				break;
			}

			printf("%s  ", caColVal);
		}
		printf("\n");
	}

	pDBQueryRes->ResultCounter   = iRowCnt;
	pDBQueryRes->TableSize       = stTableList.TotalSize;
	pDBQueryRes->ResultSet       = (void*)pDBQueryRes;
	pDBQueryRes->CurrentPosition = (void*)pDBQueryRes;

	return DBOP_OK;

err:
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
			iTextRealLength += sprintf(pcaMsgText+iTextRealLength, "%s", caMsgText);

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

void *xMalloc(int size)
{
	return malloc(size);
}
