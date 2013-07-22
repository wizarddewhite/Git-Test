/*
 * =====================================================================================
 *
 *       Filename:  debuger.c
 *
 *    Description:  copied from http://www.alexonlinux.com/how-debugger-works
 *
 *        Version:  1.0
 *        Created:  07/22/2013 10:58:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#include <time.h>

void do_debugger( pid_t child )
{
	int status = 0;
	long data;
	long orig_data;
	unsigned long addr;

	struct user_regs_struct regs;

	printf("In debugger process %ld\n", (long)getpid());


	printf("In debugger process %ld, child %ld is terminated\n",
			(long)getpid(), wait(&status));
}

void do_debuggie( void )
{
	char* argv[] = { NULL };
	char* envp[] = { NULL };
	
	printf("In debuggie process %ld\n", (long)getpid());

	execve("sleeper", argv, envp);
}

int main()
{
	pid_t child;

	// Creating child process. It will execute do_debuggie().
	// Parent process will continue to do_debugger().
	child = fork();
	if (child == 0)
		do_debuggie();
	else if (child > 0)
		do_debugger(child); 
	else
	{
		perror( "fork" );
		return -1;
	}

	return 0;
}
