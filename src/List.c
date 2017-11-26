#include <stdio.h>
#include <stdlib.h>
#include "main.h"

int main()
{
	Logger log;
	//InitLogger(&log, MESSAGE, "", "app.log");
	InitLogger(&log, MESSAGE, "", "");

    TABLE_STRUCTURE *pTabStruct = NULL;

		pTabStruct = NewTableStruct();

    FIELD_ATTR *new = NULL;

    new = NewFieldAttrNode();
    new->nFieldSize = 10;
    new->nFieldType = 2;
    sprintf(new->szFieldName, "%s", "ShenSiYuan");
    pTabStruct->AddFieldAttrNodeTail(pTabStruct, new);

    new = NewFieldAttrNode();
    new->nFieldSize = 110;
    new->nFieldType = 2;
    sprintf(new->szFieldName, "%s", "LiuYF");
    pTabStruct->AddFieldAttrNodeTail(pTabStruct, new);

    new = NewFieldAttrNode();
    new->nFieldSize = 1110;
    new->nFieldType = 3;
    sprintf(new->szFieldName, "%s", "And Love ");
    pTabStruct->AddFieldAttrNodeTail(pTabStruct, new);

    int iRet=0;
    new=NULL;
    while(iRet==0)
    {
        iRet = pTabStruct->FetchNextFieldAttrNode(pTabStruct, &new);
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
        iRet = pTabStruct->FetchNextFieldAttrNode(pTabStruct, &new);
        if(iRet!=0)
            break;
        Log(log, MESSAGE, "%d %d %s\n", new->nFieldSize, new->nFieldType, new->szFieldName);
    }

    pTabStruct->FreeFieldAttrList(pTabStruct);

		CloseLogger(&log);

    return 0;
}
