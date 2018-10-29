#include <stdio.h>
#include <stdbool.h>

struct double_list {
	struct double_list *prev;
	struct double_list *next;
	int val;
};

struct double_list_head {
	struct double_list head;
};

#define list_for_each(pos, head)	\
	for(pos = (head)->next; pos != (head); pos = pos->next)

void init_list(struct double_list *list)
{
	list->prev = list->next = list;
}

bool is_empty(struct double_list_head *head)
{
	return head->head.next == &head->head;
}

void dump(struct double_list_head *head)
{
	struct double_list *tmp = head->head.next;
	int idx = 0;

	list_for_each(tmp, &head->head)
		printf("[%02d]: %08d\n", idx++, tmp->val);
}

void insert_after(struct double_list *head, struct double_list *elem)
{
	struct double_list *prev, *next;

	prev = head;
	next = head->next;

	elem->next = next;
	next->prev = elem;
	prev->next = elem;
	elem->prev = prev;
}

void insert_before(struct double_list *head, struct double_list *elem)
{
	struct double_list *prev, *next;

	prev = head->prev;
	next = head;

	elem->next = next;
	next->prev = elem;
	prev->next = elem;
	elem->prev = prev;
}

void delete(struct double_list *elem)
{
	struct double_list *prev, *next;

	prev = elem->prev;
	next = elem->next;

	prev->next = next;
	next->prev = prev;
}

struct double_list* search(struct double_list_head* head, int target)
{
	struct double_list *tmp;

	list_for_each(tmp, &head->head) {
		if (tmp->val > target)
			break;
		if (tmp->val == target)
			return tmp;
	}

	return NULL;
}

void insert_after_test()
{
	struct double_list_head head = {{&head.head, &head.head}};
	struct double_list lists[10];
	struct double_list *tmp;
	int idx;

	for (idx = 0; idx < 10; idx++) {
		init_list(&lists[idx]);
		lists[idx].val = idx;
	}

	if (is_empty(&head))
		printf("list is empty\n");

	insert_after(&head.head, &lists[6]);
	insert_after(&head.head, &lists[5]);
	insert_after(&head.head, &lists[4]);
	insert_after(&head.head, &lists[1]);
	insert_after(&head.head, &lists[0]);

	printf("=== insert 0, 1, 4, 5, 6\n");
	dump(&head);

	tmp = search(&head, 6);
	if (tmp)
		printf("find 6 in list\n");
	else
		printf("6 not in list\n");

	tmp = search(&head, 2);
	if (tmp)
		printf("find 2 in list\n");
	else
		printf("2 not in list\n");

}

void insert_before_test()
{
	struct double_list_head head = {{&head.head, &head.head}};
	struct double_list lists[10];
	int idx;

	for (idx = 0; idx < 10; idx++) {
		init_list(&lists[idx]);
		lists[idx].val = idx;
	}

	if (is_empty(&head))
		printf("list is empty\n");

	insert_before(&head.head, &lists[6]);
	insert_before(&head.head, &lists[5]);
	insert_before(&head.head, &lists[4]);
	insert_before(&head.head, &lists[1]);
	insert_before(&head.head, &lists[0]);

	printf("=== insert 0, 1, 4, 5, 6\n");
	dump(&head);

	printf("=== delete 6\n");
	delete(&lists[6]);
	dump(&head);
}

int main()
{
	insert_after_test();
	//insert_before_test();
	return 0;
}

