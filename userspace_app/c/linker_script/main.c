#include <stdio.h>
int global_data = 4;
int global_data_2;

int main(int argc, char **argv)
{
	int local_data = 3;
	printf("Hello Worldn");
	printf("global_data = %d\n", global_data);
	printf("global_data_2 = %d\n", global_data_2);
	printf("local_data = %d\n", local_data);
	return (0);
}
