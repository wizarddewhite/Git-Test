/*
 * =====================================================================================
 *
 *       Filename:  cli.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/22/2014 08:48:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _CLI_H_
#define _CLI_H_

void screen_init(int debug);
void screen_dest();
int get_command(char *command, int n);
void print_help();
void print_message(char *message);
void print_vmessage(char *message, ...);

#endif //_CLI_H_
