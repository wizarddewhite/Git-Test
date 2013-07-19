/*
 * =====================================================================================
 *
 *       Filename:  asm_string.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/19/2013 10:57:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

char* asm_strcpy(char *dest, char* src)
{
	int d0, d1, d2;

	__asm__ __volatile__("    cld\n"
	                     "1:\tlodsb\n"
			     "    stosb\n"
			     "    testb %%al, %%al\n"
			     "    jne 1b\n"
			     /* S: %esi, D: %edi, a: %eax.
			      * & means early clobber 
			      */
			     :"=&S"(d0), "=&D"(d1), "=&a"(d2)
			     :"0"(src), "1"(dest)
			     :"memory"
			    );

	return dest;
}

