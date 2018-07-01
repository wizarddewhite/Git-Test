#include <stdio.h>
#include <stdarg.h>

double average(char m, int num,...)
{
   va_list valist;
   double sum = 0.0;
   int i;

   /* initialize valist for num number of arguments */
   va_start(valist, num);

   /* access all the arguments assigned to valist */
   for (i = 0; i < num; i++) {
      sum += va_arg(valist, int);
   }
	
   /* clean memory reserved for valist */
   va_end(valist);

   return sum/num;
}

// macro with variable argument
#define AVG(...) (average('b', __VA_ARGS__))

int main() {
   printf("Average of 2, 3, 4, 5 = %f\n", average('l', 4, 2,3,4,5));
   printf("Average of 5, 10, 15 = %f\n", average('l', 3, 5,10,15));
   printf("Average of 5, 10, 15 = %f\n", AVG(3, 5,10,15));
}
