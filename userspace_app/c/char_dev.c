/*
 * =====================================================================================
 *
 *       Filename:  char_dev.c
 *
 *    Description:  copied from http://linuxgazette.net/125/mishra.html
 *
 *        Version:  1.0
 *        Created:  08/08/2013 09:06:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main()
{
	int fd=0,ret=0;
	char buff[80]="";
	
	fd=open("/dev/my_device",O_RDONLY);
	
	printf("fd :%d\n",fd);
	
	ret=read(fd,buff,10);
	buff[ret]='\0';
	
	printf("buff: %s ;length: %d bytes\n",buff,ret);
	close(fd);

	return 0;
}
