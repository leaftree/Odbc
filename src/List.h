
#ifndef __LIST_H__
#define __LIST_H__

/**
 * Double link list.
 *
 * Plagiarize from <linux/list.h>
 */

__BEGIN_DECLS

/**
 * offsetof - Get the offset of @member in structure @type
 *
 * @type   : Type of structure
 * @member : A member of @type
 *
 * In /usr/include/string.h, stddef.h is included,
 * and in stddef.h, offsetof was defined, it's build macro for gcc.
 *
 */
#ifndef offsetof
# define offsetof(type, member) ((size_t)&((type*)0)->member)
#endif

/**
 * container_of - Get the structure start address by @member
 *
 * @ptr    : Readl memory address of @member
 * @type   : Type of structure
 * @member : Member variable of structure @type
 */
#ifndef container_of
# define container_of(ptr, type, member) ({         \
		const typeof(((type*)0)->member) *__mptr=(ptr); \
		(type*)((char*)__mptr - offsetof(type, member));})
#endif

struct list_head {
	struct list_head *prev, *next;
};

#define LIST_HEAD_INIT(name) {&(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
		struct list_head *prev,
		struct list_head *next)
{
	next->prev = new;
	new->next  = next;
	new->prev  = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = 0x00000000;
	entry->prev = 0x00000000;
}

static inline void list_replace(struct list_head *old, struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void list_replace_init(struct list_head *old,
		struct list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

static inline void list_del_init(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

static inline void list_move(struct list_head *list, struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add(list, head);
}

static inline void list_move_tail(struct list_head *list,
		struct list_head *head)
{
	__list_del(list->prev, list->next);
	list_add_tail(list, head);
}

static inline int list_is_last(const struct list_head *list,
		const struct list_head *head)
{
	return list->next == head;
}

static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

static inline void list_rotate_left(struct list_head *head)
{
	struct list_head *first;

	if(!list_empty(head))
	{
		first = head->next;
		list_move_tail(first, head);
	}
}

static inline int list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_for_each(pos, head) \
	for(pos=(head)->next; pos!=(head); pos=pos->next)

#define __list_for_each(pos, head) \
	for(pos=(head)->next; pos!=(head); pos=pos->next)

#define list_for_each_prev(pos, head) \
	for(pos=(head)->prev; pos!=(head); pos=pos->prev)

#define list_for_each_safe(pos, n, head) \
	for(pos=(head)->next, n=pos->next; pos!=(head);\
			pos=n, n=pos->next)

#define list_for_each_prev_safe(pos, n, head) \
	for(pos=(head)->prev, n=pos->prev;\
			pos!=(head);\
			pos=n; n=pos->prev)

#define list_for_each_entry(pos, head, member) \
	for(pos=list_entry((head)->next, typeof(*pos), member);\
			&pos->member!=(head);\
			pos=list_entry(pos->member.next, typeof(*pos), member))

__END_DECLS

#endif /*__LIST_H__*/
