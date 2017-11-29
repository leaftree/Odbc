#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include <mcheck.h>

//#define xFree(var) var=(var==NULL?NULL:(free(var), NULL)

#if 0
int main()
{
    setenv("MALLOC_TRACE", "mtrace.log", 1);
    mtrace();
    Logger log;
    //InitLogger(&log, MESSAGE, "", "app.log");
    InitLogger(&log, MESSAGE, "", "");

    DB_QUERY_RESULT_SET *pDbQrs = InitDbQuerySet(NULL, NULL);

    TABLE_STRUCTURE *pTabStruct = pDbQrs->pTableStruct;

    FIELD_ATTR *new = NULL;

    new = pTabStruct->NewField();
    new->nFieldSize = 10;
    new->nFieldType = 2;
    sprintf(new->szFieldName, "%s", "ShenSiYuan");
    pTabStruct->AddTail(pTabStruct, new);

    new = pTabStruct->NewField();
    new->nFieldSize = 110;
    new->nFieldType = 2;
    sprintf(new->szFieldName, "%s", "LiuYF");
    pTabStruct->AddTail(pTabStruct, new);

    new = pTabStruct->NewField();
    new->nFieldSize = 1110;
    new->nFieldType = 3;
    sprintf(new->szFieldName, "%s", "And Love ");
    pTabStruct->AddTail(pTabStruct, new);

    int iRet=0;
    new=NULL;
    while(iRet==0)
    {
        iRet = pTabStruct->Next(pTabStruct, &new);
        if(iRet!=0)
            break;
        Log(log, MESSAGE, "%d %d %s\n", new->nFieldSize, new->nFieldType, new->szFieldName);
    }

    Log(log, MESSAGE, "abc\n");
    LogDumpHex(log, MESSAGE,
            "I love Shensiyuan any more",
            sizeof("I love Shensiyuan any more"),
            "I love Shensiyuan any more");

    iRet=0;
    while(iRet==0)
    {
        iRet = pTabStruct->Next(pTabStruct, &new);
        if(iRet!=0)
            break;
        Log(log, MESSAGE, "%d %d %s\n", new->nFieldSize, new->nFieldType, new->szFieldName);
    }

    Log(log, MESSAGE, "----------------------------------------------\n");

    ROW_DATA *row = NULL;

    row=pDbQrs->New(10);
    row->nLength=101;
    sprintf(row->pValue, "%s", "shensiyuan");
    pDbQrs->AddTail(pDbQrs, row);

    row = pDbQrs->New(10);
    row->nLength=121;
    sprintf(row->pValue, "%s", "liuyunfeng");
    pDbQrs->AddHead(pDbQrs, row);

		row=NULL;
    while(0==pDbQrs->Next(pDbQrs, &row))
    {
        Log(log, MESSAGE, "length=%d value=%s\n", row->nLength, row->pValue);
    }

    Log(log, MESSAGE, "----------------------------------------------\n");

    list_head *pos, *n;
    list_for_each_safe(pos, n, pDbQrs->pRowCursor)
    {
        row=list_entry(pos, ROW_DATA, List);
        Log(log, MESSAGE, "length=%d value=%s list=%p\n", row->nLength, row->pValue, &row->List);
    }
    Log(log, MESSAGE, "----------------------------------------------\n");


    pDbQrs->Destroy(&pDbQrs);

    CloseLogger(&log);

    muntrace();

    return 0;
}
#endif
