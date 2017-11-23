#include <stdio.h>
#include <stdlib.h>
#include "main.h"

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

typedef struct list_head list_head;

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

int main()
{
	list_head *root = malloc(sizeof(list_head));
	INIT_LIST_HEAD(root);

	FIELD_ATTR *new = NULL;

	new = NewFieldAttrNode();
	list_add_tail(&new->List, root);
	new->nFieldSize = 10;
	new->nFieldType = 2;
	sprintf(new->szFieldName, "%s", "ShenSiYuan");

	new = NewFieldAttrNode();
	list_add_tail(&new->List, root);
	new->nFieldSize = 110;
	new->nFieldType = 2;
	sprintf(new->szFieldName, "%s", "I love");

	struct list_head *pos;
	FIELD_ATTR *field;

	list_for_each(pos, root)
	{
		field = list_entry(pos, FIELD_ATTR, List);
		printf("%d %d %s\n", field->nFieldSize, field->nFieldType, field->szFieldName);

		list_del(pos);
		free(field);
	}

	printf("ssy\n");

	list_for_each(pos, root)
	{
		field = list_entry(pos, FIELD_ATTR, List);
		printf("%d %d %s\n", field->nFieldSize, field->nFieldType, field->szFieldName);
	}

	list_del(root);

	return 0;
}
