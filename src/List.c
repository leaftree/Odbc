#include <stdio.h>
#include <stdlib.h>
#include "main.h"

struct fox{
    unsigned int weight;
    int is_cute;
    struct list_head list;
};

//int main()
int testsuit()
{
#if 0
    struct fox *write_fox;
    write_fox=(struct fox*)malloc(sizeof(struct fox));
    write_fox->weight=7;
    write_fox->is_cute=-1;
    INIT_LIST_HEAD(&write_fox->list);
    printf("write_fox:\t%p\t%p\t%p\n",&write_fox->list,
            (write_fox->list).next,
            (write_fox->list).prev);

    printf("\t\t%p\t%d\t%d\n",&write_fox->list, write_fox->weight, write_fox->is_cute);
    //链表头
    LIST_HEAD(fox_list);

    //添加
    list_add(&(write_fox->list),&fox_list);

    struct fox *pos;
    //遍历
    list_for_each_entry(pos,&fox_list,list){
        printf("%p\t%d\t%d\n",&pos->list, pos->weight, pos->is_cute);
    }

    //删除
    list_del(&(write_fox->list));
    free(write_fox);

    list_for_each_entry(pos,&fox_list,list){
        printf("%p\t%d\t%d\n",&pos->list, pos->weight, pos->is_cute);
    }
#endif

    LIST_HEAD(list_root);

    NEW_DB_QUERY_RESULT(dbqr, list_root);

    int i=0;
    for(i=0; i< 10; i++)
    {
        DBRow *row = malloc(sizeof(DBRow));
        row->value = malloc(1000);
        sprintf(row->value, "abcdefghij --- %03d.", i+1);
        INIT_LIST_HEAD(&row->list);

        list_add_tail(&(row->list), &list_root);
    }

    i=0;
    DB_INIT_POS(pos);
    list_for_each_entry(pos, dbqr.root, list)
    //list_for_each_entry(pos, &list_root, list)
    {
        printf("%03d %s\n", ++i, (char*)pos->value);
    }

    return 0;
}


/*
for(pos=({ const typeof(((typeof(*pos)*)0)->list) *__mptr=((dbqr.root)->next); (typeof(*pos)*)((char*)__mptr - __builtin_offsetof (typeof(*pos), list));});
&pos->list!=(dbqr.root);
pos=({ const typeof(((typeof(*pos)*)0)->list) *__mptr=(pos->list.next); (typeof(*pos)*)((char*)__mptr - __builtin_offsetof (typeof(*pos), list));}))
*/

