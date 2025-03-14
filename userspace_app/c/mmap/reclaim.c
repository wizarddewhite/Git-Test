/*
 * Copied from:
 * commit ea5273c381ab2e3ac2f350ffb765357c031548d8
 * Author: Barry Song <baohua@kernel.org>
 * Date:   Fri Feb 14 22:30:15 2025 +1300
 * 
 *     mm: avoid splitting pmd for lazyfree pmd-mapped THP in try_to_unmap
 */
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

#define SIZE 128*1024*1024  // 128 MB

int main(int argc, char *argv[])
{
       while(1) {
               volatile int *p = mmap(0, SIZE, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

               memset((void *)p, 1, SIZE);
               madvise((void *)p, SIZE, MADV_FREE);
               /* redirty after MADV_FREE */
               memset((void *)p, 1, SIZE);

               clock_t start_time = clock();
               madvise((void *)p, SIZE, MADV_PAGEOUT);
               clock_t end_time = clock();

               double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
               printf("Time taken by reclamation: %f seconds\n", elapsed_time);

               munmap((void *)p, SIZE);
       }
       return 0;
}
