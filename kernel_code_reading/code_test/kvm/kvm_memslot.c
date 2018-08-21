#include <string.h>
#include <stdio.h>

enum kvm_mr_change {
	KVM_MR_CREATE,
	KVM_MR_DELETE,
	KVM_MR_MOVE,
	KVM_MR_FLAGS_ONLY,
};

struct kvm_memory_slot {
	unsigned long base_gfn;
	unsigned long npages;
	short id;
};

struct kvm_memslots {
	struct kvm_memory_slot memslots[10];
	short slots_order[10];
	short id_to_index[10];
	int used_slots;
	int lru_slot;
};

void dump_memslot(struct kvm_memslots *slots)
{
	int i;
	for (i = 0; i < 10; i++) {
		printf("[%d]%lx, ",
			slots->memslots[i].id,
			slots->memslots[i].base_gfn);
	}
	printf("\n");
	for (i = 0; i < 10; i++) {
		printf("[%d], ",
			slots->id_to_index[i]);
	}
	printf("\n");
}

static inline struct kvm_memory_slot *
search_memslots(struct kvm_memslots *slots, unsigned long gfn)
{
	int start = 0, end = slots->used_slots;
	int slot = slots->lru_slot;
	struct kvm_memory_slot *memslots = slots->memslots;

	if (gfn >= memslots[slot].base_gfn &&
	    gfn < memslots[slot].base_gfn + memslots[slot].npages)
		return &memslots[slot];

	while (start < end) {
		slot = start + (end - start) / 2;

		if (gfn >= memslots[slot].base_gfn)
			end = slot;
		else
			start = slot + 1;
	}

	if (gfn >= memslots[start].base_gfn &&
	    gfn < memslots[start].base_gfn + memslots[start].npages) {
		//atomic_set(&slots->lru_slot, start);
		return &memslots[start];
	}

	return NULL;
}

static void update_memslots(struct kvm_memslots *slots,
			    struct kvm_memory_slot *new)
{
	int id = new->id;
	int i = slots->id_to_index[id];
	struct kvm_memory_slot *mslots = slots->memslots;

	if (!new->npages) {
		if (mslots[i].npages)
			slots->used_slots--;
	} else {
		if (!mslots[i].npages)
			slots->used_slots++;
	}

	while (i < 10 - 1 &&
	       new->base_gfn <= mslots[i + 1].base_gfn) {
		if (!mslots[i + 1].npages)
			break;
		mslots[i] = mslots[i + 1];
		slots->id_to_index[mslots[i].id] = i;
		i++;
	}

	/*
	 * The ">=" is needed when creating a slot with base_gfn == 0,
	 * so that it moves before all those with base_gfn == npages == 0.
	 *
	 * On the other hand, if new->npages is zero, the above loop has
	 * already left i pointing to the beginning of the empty part of
	 * mslots, and the ">=" would move the hole backwards in this
	 * case---which is wrong.  So skip the loop when deleting a slot.
	 */
	if (new->npages) {
		while (i > 0 &&
		       new->base_gfn >= mslots[i - 1].base_gfn) {
			mslots[i] = mslots[i - 1];
			slots->id_to_index[mslots[i].id] = i;
			i--;
		}
	} 

	mslots[i] = *new;
	slots->id_to_index[mslots[i].id] = i;
}

/* This case shows why it is necessary to check ">=" */
void test1()
{
	int i;
	struct kvm_memslots mem_slot;
	struct kvm_memory_slot slot, *ps;
	mem_slot.used_slots = 5;
	mem_slot.lru_slot = 1;

	memset(&mem_slot, 0, sizeof(struct kvm_memslots));
	for (i=0; i<10;i++) {
		mem_slot.id_to_index[i] = mem_slot.memslots[i].id = i;
	}
	printf("--- initial state --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x1000;
	slot.npages   = 0x1000;
	slot.id   = 0;
	update_memslots(&mem_slot, &slot);
	printf("--- set slot 0 --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x3000;
	slot.npages   = 0x1000;
	slot.id   = 1;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x5000;
	slot.npages   = 0x1000;
	slot.id   = 2;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x7000;
	slot.npages   = 0x1000;
	slot.id   = 3;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x8000;
	slot.npages   = 0x1000;
	slot.id   = 4;
	update_memslots(&mem_slot, &slot);
	printf("--- set slot 1-4 --- \n");
	dump_memslot(&mem_slot);

	// move slot [0x1000-] to [0x6000-]
	slot.base_gfn = 0x6000;
	slot.npages   = 0x1000;
	slot.id   = 0;
	update_memslots(&mem_slot, &slot);
	printf("--- rearrange slot 0 to 0x6000 --- \n");
	dump_memslot(&mem_slot);
	
	// create slot 6 with range [0x0-] 
	// this is a corner case, since there are only 5 valid slot
	slot.base_gfn = 0x0;
	slot.npages   = 0x1000;
	slot.id   = 6;
	update_memslots(&mem_slot, &slot);
	printf("--- set slot 6 to 0x0 --- \n");
	dump_memslot(&mem_slot);
}

/* This case shows why it is necessary to check new->pages
 * when removing a slot.
 */
void remove_test()
{
	int i;
	struct kvm_memslots mem_slot;
	struct kvm_memory_slot slot, *ps;
	mem_slot.used_slots = 5;
	mem_slot.lru_slot = 1;

	memset(&mem_slot, 0, sizeof(struct kvm_memslots));
	for (i=0; i<10;i++) {
		mem_slot.id_to_index[i] = mem_slot.memslots[i].id = i;
	}
	printf("--- initial state --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x0;
	slot.npages   = 0x1000;
	slot.id   = 5;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x2000;
	slot.npages   = 0x1000;
	slot.id   = 4;
	update_memslots(&mem_slot, &slot);
	printf("--- set slot 4,5 --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x0;
	slot.npages   = 0x0;
	slot.id   = 4;
	update_memslots(&mem_slot, &slot);
	printf("--- remove slot 4 --- \n");
	dump_memslot(&mem_slot);
}

void search_test()
{
	int i;
	struct kvm_memslots mem_slot;
	struct kvm_memory_slot slot, *ps;
	mem_slot.used_slots = 5;
	mem_slot.lru_slot = 1;

	memset(&mem_slot, 0, sizeof(struct kvm_memslots));
	for (i=0; i<10;i++) {
		mem_slot.id_to_index[i] = mem_slot.memslots[i].id = i;
	}
	printf("--- initial state --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x1000;
	slot.npages   = 0x1000;
	slot.id   = 0;
	update_memslots(&mem_slot, &slot);
	printf("--- set slot 0 --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x3000;
	slot.npages   = 0x1000;
	slot.id   = 1;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x5000;
	slot.npages   = 0x1000;
	slot.id   = 2;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x7000;
	slot.npages   = 0x1000;
	slot.id   = 3;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x8000;
	slot.npages   = 0x1000;
	slot.id   = 4;
	update_memslots(&mem_slot, &slot);
	printf("--- set slot 1-4 --- \n");
	dump_memslot(&mem_slot);

	ps = search_memslots(&mem_slot, 0x1010);
	printf("%lx-%lx\n", ps->base_gfn, ps->npages);

}

void insert_test()
{
	int i;
	struct kvm_memslots mem_slot;
	struct kvm_memory_slot slot, *ps;
	mem_slot.used_slots = 5;
	mem_slot.lru_slot = 1;

	memset(&mem_slot, 0, sizeof(struct kvm_memslots));
	for (i=0; i<10;i++) {
		mem_slot.id_to_index[i] = mem_slot.memslots[i].id = i;
	}
	printf("--- initial state --- \n");
	dump_memslot(&mem_slot);

	// insert 9 slot first
	slot.base_gfn = 0x1000;
	slot.npages   = 0x1000;
	slot.id   = 0;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x3000;
	slot.npages   = 0x1000;
	slot.id   = 1;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x5000;
	slot.npages   = 0x1000;
	slot.id   = 2;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x7000;
	slot.npages   = 0x1000;
	slot.id   = 3;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x8000;
	slot.npages   = 0x1000;
	slot.id   = 4;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x11000;
	slot.npages   = 0x1000;
	slot.id   = 5;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x13000;
	slot.npages   = 0x1000;
	slot.id   = 6;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x15000;
	slot.npages   = 0x1000;
	slot.id   = 7;
	update_memslots(&mem_slot, &slot);

	slot.base_gfn = 0x17000;
	slot.npages   = 0x1000;
	slot.id   = 9;
	update_memslots(&mem_slot, &slot);

	printf("--- set slot 0-8 --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x0;
	slot.npages   = 0x1000;
	slot.id   = 8;
	update_memslots(&mem_slot, &slot);

	printf("--- set slot 9 to [0x0-] --- \n");
	dump_memslot(&mem_slot);

}

static inline struct kvm_memory_slot *
search_memslots2(struct kvm_memslots *slots, unsigned long gfn, int *pos)
{
	int start = 0, end = slots->used_slots;
	int slot = slots->lru_slot;
	struct kvm_memory_slot *memslots = slots->memslots;

	if (gfn >= memslots[slot].base_gfn &&
	    gfn < memslots[slot].base_gfn + memslots[slot].npages) {
		if (!pos)
			return &memslots[slot];
	}

	while (start < end) {
		slot = (start + end) / 2;

		if (gfn >= memslots[slots->id_to_index[slot]].base_gfn)
			end = slot;
		else
			start = slot + 1;
	}

	short id = slots->id_to_index[start];
	if (pos)
		*pos = start;
	if (gfn >= memslots[id].base_gfn &&
	    gfn < memslots[id].base_gfn + memslots[id].npages) {
		//atomic_set(&slots->lru_slot, start);
		return &memslots[id];
	}

	return NULL;
}

static void update_memslots2(struct kvm_memslots *slots,
			    struct kvm_memory_slot *new,
			    int id)
{
	int i = slots->id_to_index[id];
	struct kvm_memory_slot *mslots = slots->memslots;

	if (!new->npages) {
		if (mslots[i].npages)
			slots->used_slots--;
	} else {
		if (!mslots[i].npages)
			slots->used_slots++;
	}

	while (i < 10 - 1 &&
	       new->base_gfn <= mslots[slots->slots_order[i+1]].base_gfn) {
		if (!mslots[slots->slots_order[i+1]].npages)
			break;
		slots->slots_order[i] = slots->slots_order[i+1];
		slots->id_to_index[slots->slots_order[i]] = i;
		i++;
	}

	if (new->npages) {
		while (i > 0 &&
		       new->base_gfn >= mslots[slots->slots_order[i-1]].base_gfn) {
			slots->slots_order[i] = slots->slots_order[i-1];
			slots->id_to_index[slots->slots_order[i]] = i;
			i--;
		}
	}

	mslots[id] = *new;
	slots->slots_order[i] = id;
	slots->id_to_index[slots->slots_order[i]] = i;
}

void new_search_test()
{
	int i;
	struct kvm_memslots mem_slot;
	struct kvm_memory_slot slot, *ps;
	
	mem_slot.used_slots = 5;
	mem_slot.lru_slot = 1;

	memset(&mem_slot, 0, sizeof(struct kvm_memslots));
	for (i=0; i<10;i++) {
		mem_slot.id_to_index[i] = mem_slot.memslots[i].id = i;
	}

	mem_slot.used_slots = 3;

	mem_slot.memslots[1].base_gfn = 0x8000;
	mem_slot.memslots[1].npages = 0x1000;
	mem_slot.memslots[1].id = 0;
	mem_slot.id_to_index[0] = 1;

	mem_slot.memslots[2].base_gfn = 0x4000;
	mem_slot.memslots[2].npages = 0x1000;
	mem_slot.memslots[2].id = 1;
	mem_slot.id_to_index[1] = 2;

	mem_slot.memslots[0].base_gfn = 0x1000;
	mem_slot.memslots[0].npages = 0x1000;
	mem_slot.memslots[0].id = 2;
	mem_slot.id_to_index[2] = 0;

	printf("--- initial state --- \n");
	dump_memslot(&mem_slot);

	ps = search_memslots2(&mem_slot, 0x1010, &i);
	if (ps)
		printf("%lx-%lx at %d\n", ps->base_gfn, ps->npages, i);
	else
		printf("0x1010 pos is %d\n", i);

	ps = search_memslots2(&mem_slot, 0x8010, &i);
	if (ps)
		printf("%lx-%lx at %d\n", ps->base_gfn, ps->npages, i);
	else
		printf("0x8010 pos is %d\n", i);

	ps = search_memslots2(&mem_slot, 0x6010, &i);
	if (ps)
		printf("%lx-%lx\n", ps->base_gfn, ps->npages);
	else
		printf("0x6010 pos is %d\n", i);

	ps = search_memslots2(&mem_slot, 0x9010, &i);
	if (ps)
		printf("%lx-%lx\n", ps->base_gfn, ps->npages);
	else
		printf("0x9010 pos is %d\n", i);

	ps = search_memslots2(&mem_slot, 0x0010, &i);
	if (ps)
		printf("%lx-%lx\n", ps->base_gfn, ps->npages);
	else
		printf("0x0010 pos is %d\n", i);

}

void new_update_slot_test()
{
	int i;
	struct kvm_memslots mem_slot;
	struct kvm_memory_slot slot, *ps;
	memset(&mem_slot, 0, sizeof(struct kvm_memslots));
	mem_slot.lru_slot = 1;
	for (i=0; i<10;i++) {
		mem_slot.id_to_index[i] = mem_slot.slots_order[i] = mem_slot.memslots[i].id = i;
	}
	printf("--- initial state --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x1000;
	slot.npages   = 0x1000;
	update_memslots2(&mem_slot, &slot, 0);
	printf("--- set slot 0 --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x3000;
	slot.npages   = 0x1000;
	update_memslots2(&mem_slot, &slot, 1);
	printf("--- set slot 1 --- \n");
	dump_memslot(&mem_slot);

	slot.base_gfn = 0x5000;
	slot.npages   = 0x1000;
	update_memslots2(&mem_slot, &slot, 2);

	slot.base_gfn = 0x7000;
	slot.npages   = 0x1000;
	update_memslots2(&mem_slot, &slot, 3);

	slot.base_gfn = 0x8000;
	slot.npages   = 0x1000;
	update_memslots2(&mem_slot, &slot, 4);
	printf("--- set slot 1-4 --- \n");
	dump_memslot(&mem_slot);
}

int main()
{
	//test1();
	//remove_test();
	// insert_test();
	// new_search_test();
	new_update_slot_test();
	return 0;
}
