#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "resv_map.h"

static bool has_same_uncharge_info(struct file_region *rg,
				   struct file_region *org)
{
#ifdef CONFIG_CGROUP_HUGETLB
	return rg && org &&
	       rg->reservation_counter == org->reservation_counter &&
	       rg->css == org->css;

#else
	return true;
#endif
}

void coalesce_file_region(struct resv_map *resv, struct file_region *rg)
{
	struct file_region *nrg = NULL, *prg = NULL;

	prg = list_prev_entry(rg, link);
	if (&prg->link != &resv->regions && prg->to == rg->from &&
	    has_same_uncharge_info(prg, rg)) {
		prg->to = rg->to;

		list_del(&rg->link);
		// kfree(rg);

		coalesce_file_region(resv, prg);
		return;
	}

	nrg = list_next_entry(rg, struct file_region, link);
	if (&nrg->link != &resv->regions && nrg->from == rg->to &&
	    has_same_uncharge_info(nrg, rg)) {
		nrg->from = rg->from;

		list_del(&rg->link);
		// kfree(rg);

		coalesce_file_region(resv, nrg);
		return;
	}
}

void coalesce_file_region2(struct resv_map *resv, struct file_region *rg)
{
	struct file_region *nrg = NULL, *prg = NULL;

	prg = list_prev_entry(rg, link);
	if (&prg->link != &resv->regions && prg->to == rg->from &&
	    has_same_uncharge_info(prg, rg)) {
		prg->to = rg->to;

		list_del(&rg->link);
		// kfree(rg);

		rg = prg;
	}

	nrg = list_next_entry(rg, struct file_region, link);
	if (&nrg->link != &resv->regions && nrg->from == rg->to &&
	    has_same_uncharge_info(nrg, rg)) {
		nrg->from = rg->from;

		list_del(&rg->link);
		// kfree(rg);
	}
}
void dump_resv_map(struct resv_map *resv)
{
	struct file_region *rg;

	if (!resv || list_empty(&resv->regions))
		return;

	list_for_each_entry(rg, &resv->regions, link) {
		printf("\t[%ld - %ld]\n", rg->from, rg->to);
	}

	return;
}

struct resv_map map;
struct file_region regions[4];

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
void init_resv_map(struct resv_map *resv)
{
	int i;

	if (!resv)
		return;

	INIT_LIST_HEAD(&resv->regions);
	resv->adds_in_progress = 0;
	INIT_LIST_HEAD(&resv->region_cache);

	for (i = 0; i < ARRAY_SIZE(regions); i++) {
		INIT_LIST_HEAD(&regions[i].link);
		regions[i].from = i * 100;
		regions[i].to = (i + 1) * 100;
		// printf("regions[%d]: [%ld-%ld]\n", i,
		// 		regions[i].from, regions[i].to);
	}

	list_add(&regions[2].link, &resv->regions);
	list_add(&regions[0].link, &resv->regions);
}

void test_coalesce(int adjust_from, int adjust_to)
{
	struct file_region *rg;

	if (adjust_from < 0 || adjust_to > 0) {
		printf("Invalid adjustment\n");
		return;
	}

	init_resv_map(&map);

	printf("### resv_map start with:\n");
	dump_resv_map(&map);
	printf("\n");

	regions[1].from += adjust_from;
	regions[1].to   += adjust_to;
	printf("### resv_map insert [%ld-%ld]:\n",
			regions[1].from, regions[1].to);
	list_add(&regions[1].link, &regions[0].link);
	dump_resv_map(&map);
	printf("\n");

	printf("### resv_map coalesce [%ld-%ld]:\n",
			regions[1].from, regions[1].to);
	coalesce_file_region(&map, &regions[1]);
	dump_resv_map(&map);
	printf("\n");
	
	return;
}

void test_coalesce2(int adjust_from, int adjust_to)
{
	struct file_region *rg;

	if (adjust_from < 0 || adjust_to > 0) {
		printf("Invalid adjustment\n");
		return;
	}

	init_resv_map(&map);

	printf("### resv_map start with:\n");
	dump_resv_map(&map);
	printf("\n");

	regions[1].from += adjust_from;
	regions[1].to   += adjust_to;
	printf("### resv_map insert [%ld-%ld]:\n",
			regions[1].from, regions[1].to);
	list_add(&regions[1].link, &regions[0].link);
	dump_resv_map(&map);
	printf("\n");

	printf("### resv_map coalesce [%ld-%ld]:\n",
			regions[1].from, regions[1].to);
	coalesce_file_region2(&map, &regions[1]);
	dump_resv_map(&map);
	printf("\n");
	
	return;
}
int main()
{
	test_coalesce(0, -2);
	test_coalesce2(0, -2);
	return 0;
}
