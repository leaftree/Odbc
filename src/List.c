#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include <mcheck.h>

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

int FetchNextFieldAttrNode(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR **ppField);
void FreeFieldAttrList(TABLE_STRUCTURE *pTabStruct);

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

int AddFieldAttrNode(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR *new)
{
    if(!pTabStruct || !new || !pTabStruct->pListRoot)
        return -1;

    list_add(&new->List, pTabStruct->pListRoot);

    return 0;
}

int AddFieldAttrNodeTail(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR *pNew)
{
    if(!pTabStruct || !pTabStruct->pListRoot || !pNew)
        return -1;

    list_add_tail(&pNew->List, pTabStruct->pListRoot);

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

/*
int FetchAllFieldAttrNode(TABLE_STRUCTURE *pTabStruct, FIELD_ATTR *ppField)
{
    if(!pTabStruct || !pTabStruct->pCursor || !ppField)
        return -1;

    static list_head *pCursor;

   list_for_each_safe(pos, n, TabStruct.pListRoot)
   {
   field = list_entry(pos, FIELD_ATTR, List);
return 0;
}
*/

TABLE_STRUCTURE *NewTableStruct()
{
    TABLE_STRUCTURE *pTabStruct = malloc(sizeof(TABLE_STRUCTURE));

    list_head *root = malloc(sizeof(list_head));
    INIT_LIST_HEAD(root);

    pTabStruct->pCursor = root;
    pTabStruct->pListRoot = root;
    pTabStruct->FreeFieldAttrList = FreeFieldAttrList;
    pTabStruct->AddFieldAttrNodeTail = AddFieldAttrNodeTail;
    pTabStruct->FetchNextFieldAttrNode = FetchNextFieldAttrNode;

    return pTabStruct;
}

int main()
{
    TABLE_STRUCTURE TabStruct;

    FIELD_ATTR *new = NULL;

    new = NewFieldAttrNode();
    new->nFieldSize = 10;
    new->nFieldType = 2;
    sprintf(new->szFieldName, "%s", "ShenSiYuan");
    TabStruct.AddFieldAttrNodeTail(&TabStruct, new);

    new = NewFieldAttrNode();
    new->nFieldSize = 110;
    new->nFieldType = 2;
    sprintf(new->szFieldName, "%s", "LiuYF");
    TabStruct.AddFieldAttrNodeTail(&TabStruct, new);

    new = NewFieldAttrNode();
    new->nFieldSize = 1110;
    new->nFieldType = 3;
    sprintf(new->szFieldName, "%s", "And Love ");
    TabStruct.AddFieldAttrNodeTail(&TabStruct, new);

    int iRet=0;
    new=NULL;
    while(iRet==0)
    {
        iRet = TabStruct.FetchNextFieldAttrNode(&TabStruct, &new);
        if(iRet!=0)
            break;
        printf("%d %d %s\n", new->nFieldSize, new->nFieldType, new->szFieldName);
    }

    printf("abc\n");

    iRet=0;
    while(iRet==0)
    {
        iRet = TabStruct.FetchNextFieldAttrNode(&TabStruct, &new);
        if(iRet!=0)
            break;
        printf("%d %d %s\n", new->nFieldSize, new->nFieldType, new->szFieldName);
    }

    TabStruct.FreeFieldAttrList(&TabStruct);

    return 0;
}
