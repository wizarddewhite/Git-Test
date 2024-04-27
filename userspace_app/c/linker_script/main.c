#include <stdio.h>
int global_data = 4;
int global_data_2;

void global_data_default()
{
	printf("global_data = %d\n", global_data);
	printf("global_data_2 = %d\n", global_data_2);
}

int main(int argc, char **argv)
{
	int local_data = 3;
	global_data_default();
	printf("local_data = %d\n", local_data);
	return (0);
}
