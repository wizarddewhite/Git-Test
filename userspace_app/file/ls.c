/*
 * =====================================================================================
 *
 *       Filename:  ls.c
 *
 *    Description:  http://www.linuxidc.com/Linux/2011-03/33584.htm
 *
 *        Version:  1.0
 *        Created:  07/23/2014 04:09:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
char filename[256][256];
int len = 0;

int trave_dir(char* path, int depth)
{
	DIR *d;
	struct dirent *file;
	struct stat sb;  

	if(!(d = opendir(path)))
	{
		printf("error opendir %s!!!\n",path);
		return -1;
	}
	while((file = readdir(d)) != NULL)
	{
		// ignore current dir(.), parent dir(..), hidden fil
		if(strncmp(file->d_name, ".", 1) == 0)
			continue;
		// same file name
		strcpy(filename[len++], file->d_name);
		// if it is a directory, go into it
		if(stat(file->d_name, &sb) >= 0 && S_ISDIR(sb.st_mode) && depth <= 3)
		{
			trave_dir(file->d_name, depth + 1);
		}
	}
	closedir(d);
	return 0;
}

int main()
{
	int depth = 1;
	int i;
	trave_dir(".", depth);
	for(i = 0; i < len; i++)
	{
		printf("%s\t", filename[i]);
	}
	printf("\n");
	return 0;
}
