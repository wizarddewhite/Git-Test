#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dna {
	char *string;
	int inversion;
};

struct dna *dnas;
int num, len;

void calc_inversion(struct dna *dp)
{
	int i, j;

	for (i = 0; i < len; i++) {
		for (j = i; j < len; j++)
			if (dp->string[i] > dp->string[j])
				dp->inversion++;
	}
}

void calc_inversion2(struct dna *dp)
{
	int A, C, G, T;
	A = C = G = T = 0;
	char *p;
	for (p = dp->string; *p; p++) {
		switch (*p) {
			case 'A':
				dp->inversion += C + G + T;
				A++;
				break;
			case 'C':
				dp->inversion += G + T;
				C++;
				break;
			case 'G':
				dp->inversion += T;
				G++;
				break;
			case 'T':
				T++;
				break;

		}
	}
}

int dna_comp(const void *d1, const void *d2)
{
	int inv1, inv2;
	inv1 = ((struct dna *)d1)->inversion; 
	inv2 = ((struct dna *)d2)->inversion; 

	return inv1 - inv2;
}

int main()
{
	char line[1024];
	struct dna *dp;
	int i;

	fgets(line, 1024, stdin);
	sscanf(line, "%d %d\n", &len, &num);
	//printf("DNA len:%d Number:%d\n", len, num);
	
	dnas = calloc(sizeof(struct dna), num);

	for (i = 0; i < num; i++) {
		fgets(line, 1024, stdin);
		dp = dnas + i;
		dp->string = strdup(line);
		calc_inversion2(dp);
	}

	qsort(dnas, num, sizeof(struct dna), dna_comp);

	for (i = 0; i < num; i++)
		//printf("%d %s", dnas[i].inversion, dnas[i].string);
		printf("%s", dnas[i].string);

	return 0;
}
