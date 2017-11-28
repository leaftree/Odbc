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

static SQLINTEGER __DBApiFreeHandle(SQLSMALLINT hType, SQLHANDLE hHandle)
{
    SQLCHAR caMsgText[K8] = "";

    SQLRETURN nRet = SQLFreeHandle(hType, hHandle);
    if(!SQL_SUCCEEDED(nRet))
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
    SQLRETURN nRet = SQL_SUCCESS;

    nRet = SQLAllocHandle(SQL_HANDLE_ENV, NULL, hEnv);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_ENV, *hEnv, nRet))
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

    nRet = SQLAllocHandle(SQL_HANDLE_DBC, *hEnv, hDbc);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_DBC, *hDbc, nRet))
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
    SQLRETURN nRet = SQL_SUCCESS;

    nRet = SQLConnect(*hDbc, pcaDBName, SQL_NTS, pcaDBUserName, SQL_NTS,
            pcaDBUserPassword, SQL_NTS);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_DBC, *hDbc, nRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER DBApiDisConnectDatabase(SQLHDBC hDbc)
{
    SQLRETURN nRet = SQL_SUCCESS;

    nRet = SQLDisconnect(hDbc);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_DBC, hDbc, nRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER DBApiPreExecSQL(SQLHDBC hDbc, SQLHSTMT *hStmt)
{
    SQLRETURN nRet = SQL_SUCCESS;

    nRet = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, hStmt);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, *hStmt, nRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER DBApiExecSQL(SQLHSTMT hStmt, SQLCHAR *pcaSqlStmt)
{
    SQLRETURN nRet = SQL_SUCCESS;

    nRet = SQLExecDirect(hStmt, pcaSqlStmt, SQL_NTS);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, nRet))
    {
        return DBOP_NO;
    }

    return DBOP_OK;
}

SQLINTEGER __DBGetColsInfo(DB_QUERY_RESULT_SET *pDbQueryRes)
{
    SQLRETURN   nRet       = SQL_SUCCESS;
    SQLSMALLINT nLoop      = 0;
    SQLSMALLINT nColCnt    = 0;

    SQLULEN     nColSize       = 0;
    SQLCHAR     szColName[100] = "";
    SQLSMALLINT nColAttrLen    = 0;
    SQLSMALLINT nColType       = 0;
    SQLSMALLINT nColDecDigit   = 0;
    SQLSMALLINT nColNullable   = 0;

		SQLHSTMT hStmt = NULL;

		if(!pDbQueryRes)
		{
			return -1;
		}
		
		hStmt = pDbQueryRes->hStmt;

    nRet = SQLNumResultCols(hStmt, &nColCnt);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, nRet))
    {
        return DBOP_NO;
    }

    if(nColCnt==0)
    {
        return DBOP_NO;
    }

    for(nLoop=1; nLoop<=nColCnt; nLoop++)
    {
        nRet = SQLDescribeCol(hStmt,
                nLoop,
                szColName,
                sizeof(szColName),
                &nColAttrLen,
                &nColType,
                &nColSize,
                &nColDecDigit,
                &nColNullable);

        if(nRet == SQL_ERROR)
        {
					return -1;
        }

				FIELD_ATTR *new = pDbQueryRes->pTableStruct->NewField();
				if(!new)
				{
					return -1;
				}

				new->nFieldSize = nColSize;
				new->nFieldType = nColType;
				snprintf(new->szFieldName, 128, "%s", szColName);

				pDbQueryRes->pTableStruct->nTotalSize += nColSize;
				pDbQueryRes->pTableStruct->nFieldCounter += 1;

				pDbQueryRes->pTableStruct->AddHead(pDbQueryRes->pTableStruct, new);
    }

		pDbQueryRes->nTableSize = pDbQueryRes->pTableStruct->nTotalSize;

    return DBOP_OK;
}

SQLINTEGER DBApiQuery(DB_QUERY_RESULT_SET *pDbQueryRes,
                      SQLCHAR             *pszSqlStmt)
{
    SQLRETURN   nRet    = SQL_SUCCESS;
    SQLLEN      iRowCnt = 0; /* Row counter of query result     */
    SQLSMALLINT nColCnt = 0; /* Columns counter of query result */
    //SQLINTEGER  iResOfs = 0;
		SQLHSTMT    hStmt   = NULL;

		if(!pDbQueryRes || !pszSqlStmt || !*pszSqlStmt)
		{
			return -1;
		}

		hStmt = pDbQueryRes->hStmt;

    nRet = SQLExecDirect(pDbQueryRes->hStmt, pszSqlStmt, SQL_NTS);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, hStmt, nRet))
    {
        return DBOP_NO;
    }

    if(DBOP_OK != __DBGetColsInfo(pDbQueryRes))
    {
        return DBOP_NO;
    }

    nRet = SQLRowCount(pDbQueryRes->hStmt, &iRowCnt);
    if(DBOP_OK != __DBApiCheckSQLReturn(SQL_HANDLE_STMT, pDbQueryRes->hStmt, nRet))
    {
        goto err;
    }

    SQLLEN iRealSize = 0;
    SQLCHAR caColVal[100] = "";
    for(;;)
    {
        nRet =  SQLFetch(hStmt);

        if(nRet == SQL_ERROR || nRet == SQL_SUCCESS_WITH_INFO)
        {
            if(nRet == SQL_ERROR)
                break;
        }
        else if(nRet == SQL_NO_DATA)
        {
            break;
        }

        SQLSMALLINT iColLoop;
        //SQLSMALLINT iFieldMaxSize = 0;
        for(iColLoop=1; iColLoop <= nColCnt; iColLoop++)
        {
     //       iFieldMaxSize = (stTableList.field_attr_list+iColLoop-1)->FieldSize;
            nRet = SQLGetData(hStmt, iColLoop, SQL_C_CHAR, caColVal, 100, &iRealSize);

            if(nRet == SQL_ERROR || nRet == SQL_SUCCESS_WITH_INFO)
            {
                if(nRet == SQL_ERROR)
                    break;
            }
            else if(nRet == SQL_NO_DATA)
            {
                break;
            }
            printf("%s ", caColVal);
            /*
            memcpy(pstDBQueryRes->ResultSet+iResOfs, (void*)caColVal, iRealSize);
            iResOfs += (iFieldMaxSize+1);
            */
        }
    }

    /*
    *pDBQueryRes = pstDBQueryRes;
    (*pDBQueryRes)->ResultCounter   = iRowCnt;
    (*pDBQueryRes)->TableSize       = stTableList.TotalSize;
    */


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

    SQLRETURN nRet = SQL_SUCCESS;

    iRecNumber = 1;
    iTextRealLength = 0;

    while(1)
    {
        nRet=SQLGetDiagRec(hType,
                hHandle,
                iRecNumber,
                caSqlState,
                &iNativeError,
                caMsgText,
                SQL_MAX_MESSAGE_LENGTH-1,
                &iTextLength);
        if(SQL_SUCCEEDED(nRet))
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

    if(nRet != SQL_NO_DATA || !iTextRealLength)
        return DBOP_NO;

    return DBOP_OK;
}

void *NewFieldAttrNode()
{
    FIELD_ATTR *new = malloc(sizeof(FIELD_ATTR));
    if(!new)
        return NULL;

    new->nFieldSize = 0;
    new->nFieldType = 0;
    *new->szFieldName= 0;
    INIT_LIST_HEAD(&new->List);

    return new;
}

void FreeFieldAttrList(TABLE_STRUCTURE *pTabStruct)
{
    list_head *n, *pos, *root;

    if(!pTabStruct)
        return;

    FIELD_ATTR *entry;

    root = pTabStruct->pListRoot;

    list_for_each_safe(pos, n, root)
    {
        list_del(pos);
        entry = list_entry(pos, FIELD_ATTR, List);
        free(entry);
    }

    free(pTabStruct->pListRoot);
    pTabStruct->pListRoot = NULL;
    pTabStruct->pCursor = NULL;
}

int AddFieldAttrNodeTail(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR *pNew)
{
    if(!pTabStruct || !pTabStruct->pListRoot || !pNew)
        return -1;

    list_add_tail(&pNew->List, pTabStruct->pListRoot);

    return 0;
}

int AddFieldAttrNodeHead(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR *pNew)
{
    if(!pTabStruct || !pTabStruct->pListRoot || !pNew)
        return -1;

    list_add(&pNew->List, pTabStruct->pListRoot);

    return 0;
}

int FetchNextFieldAttrNode(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR **ppField)
{
    FIELD_ATTR *field;

    if(!pTabStruct || !pTabStruct->pCursor || !ppField)
        return -1;

    if(list_empty(pTabStruct->pListRoot) || pTabStruct->pCursor->next==pTabStruct->pListRoot)
    {
        pTabStruct->pCursor = pTabStruct->pListRoot;
        return 1;
    }

    field = list_entry(pTabStruct->pCursor->next, FIELD_ATTR, List);
    pTabStruct->pCursor = pTabStruct->pCursor->next;

    *ppField=field;

    return 0;
}

void *NewTableStruct()
{
    TABLE_STRUCTURE *pTabStruct = malloc(sizeof(TABLE_STRUCTURE));

    list_head *root = malloc(sizeof(list_head));
    INIT_LIST_HEAD(root);

    pTabStruct->pCursor   = root;
    pTabStruct->pListRoot = root;
    pTabStruct->Free      = FreeFieldAttrList;
    pTabStruct->Next      = FetchNextFieldAttrNode;
    pTabStruct->AddTail   = AddFieldAttrNodeTail;
    pTabStruct->AddHead   = AddFieldAttrNodeHead;
    pTabStruct->NewField  = NewFieldAttrNode;

    return pTabStruct;
}

void *NewRowNode(int nSize)
{
    ROW_DATA *new = malloc(sizeof(ROW_DATA)+nSize);
    if(!new)
        return NULL;

    new->nLength = nSize;
    new->pValue = new+sizeof(ROW_DATA);
    INIT_LIST_HEAD(&new->List);

    return new;
}

void *InitDbQuerySet(SQLHDBC hDbc, SQLHSTMT hStmt)
{
    ROW_DATA *pRow = NULL;
    TABLE_STRUCTURE *pTabStruct = NULL;
    DB_QUERY_RESULT_SET *pDbQrs = NULL;

    pDbQrs = malloc(sizeof(DB_QUERY_RESULT_SET));
    if(!pDbQrs)
        goto err;

    pTabStruct = NewTableStruct();
    if(!pTabStruct)
        goto err;

    pRow = NewRowNode(0);
    if(!pRow)
        goto err;

    pDbQrs->nTableSize = 0;
    pDbQrs->nRowCounter = 0;
    pDbQrs->pTableStruct = pTabStruct;
    pDbQrs->pRow = pRow;
    pDbQrs->pRowCursor = pRow->List.next;
    pDbQrs->hDbc = hDbc;
    pDbQrs->hStmt= hStmt;

    pDbQrs->New = NewRowNode;
    pDbQrs->Free = FreeDBRow;
    pDbQrs->Next = FetchNextRow;
    pDbQrs->AddHead = DBRowAddHead;
    pDbQrs->AddTail = DBRowAddTail;
    pDbQrs->Destroy = FreeDbQuerySet;

    return pDbQrs;

err:
  if(pRow)
      free(pRow);
  if(pTabStruct)
      free(pTabStruct);
  if(pDbQrs)
      free(pDbQrs);
  return NULL;
}

void FreeDbQuerySet(DB_QUERY_RESULT_SET **pDbQrs)
{
    if(!pDbQrs || !*pDbQrs)
        return ;

    DB_QUERY_RESULT_SET *dbset = *pDbQrs;

    if(dbset->pTableStruct)
    {
        dbset->pTableStruct->Free(dbset->pTableStruct);
        free(dbset->pTableStruct);
    }

    if(dbset->pRow)
    {
        dbset->Free(&(*pDbQrs)->pRow);
    }
    
    free(dbset);
    dbset=NULL;
    pDbQrs=NULL;

    return;
}

void FreeDBRow(ROW_DATA **pRow)
{
    list_head *n, *pos, *root;

    if(!pRow || !*pRow)
        return;

    ROW_DATA *entry;

    root = &(*pRow)->List;

    list_for_each_safe(pos, n, root)
    {
        list_del(pos);
        entry = list_entry(pos, ROW_DATA, List);
        free(entry);
    }

    free(*pRow);
    *pRow = NULL;

    return;
}

int DBRowAddHead(DB_QUERY_RESULT_SET *pDbQrs, ROW_DATA *pRow)
{
    if(!pDbQrs || !pRow)
    {
        return DBOP_NO;
    }

    list_add(&pRow->List, &pDbQrs->pRow->List);

    return DBOP_OK;
}

int DBRowAddTail(DB_QUERY_RESULT_SET *pDbQrs, ROW_DATA *pRow)
{
    if(!pDbQrs || !pRow)
    {
        return DBOP_NO;
    }

    list_add_tail(&pRow->List, &pDbQrs->pRow->List);

    return DBOP_OK;
}

int FetchNextRow(DB_QUERY_RESULT_SET *pDbQrs, ROW_DATA **pRow)
{
    ROW_DATA *row;
    list_head *cur = pDbQrs->pRowCursor->next;

    if(!pDbQrs || !pRow)
        return -1;

    if(list_empty(cur) || cur==&pDbQrs->pRow->List)
    {
        pDbQrs->pRowCursor=&pDbQrs->pRow->List;
        return 1;
    }

    row = list_entry(cur, ROW_DATA, List);
    pDbQrs->pRowCursor = cur;

    *pRow = row;

    return 0;
}
