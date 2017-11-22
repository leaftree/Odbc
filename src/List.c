#include <stdio.h>
#include <stdlib.h>
#include "main.h"

DBRow * new_list_node(void *data, size_t len)
{
    DBRow *row = malloc(sizeof(DBRow));
    row->value = calloc(1, len);
    memcpy(row->value, data, len);
    INIT_LIST_HEAD(&row->list);

    return row;
}

int main()
{
    LIST_HEAD(root);
    INIT_LIST_HEAD(&root);

    DBQueryResult dbqr = {
        .TableSize = 0,
        .ResultCounter = 0,
        .root = &root,
    };

    DBRow *row = NULL;
    row = new_list_node("abc", 4);

    list_add(&row->list, dbqr.root);

    row = new_list_node("abcx", 4);

    list_add(&row->list, dbqr.root);

    struct list_head *pos;
    DBRow *tmp;

    list_for_each(pos, dbqr.root)
    {
        tmp = list_entry(pos, DBRow, list);
        printf("%s\n", (char*)tmp->value);
    }

    return 0;
}