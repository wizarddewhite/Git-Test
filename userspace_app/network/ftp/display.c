/*
 * =====================================================================================
 *
 *       Filename:  cli.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/22/2014 08:27:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <ncurses.h>
#include <string.h>

int nrows, ncols;
int r = 0, c = 0;
WINDOW *win;

static void prompt(char *s)
{
	c = 0;
	mvwprintw(win, r, c, "%s>", s);
	c = c + strlen(s) + 1;
	move(r, c);
	refresh();
}

void screen_init()
{
	static int inited = 0;

	if (inited)
		return;

	win = initscr();
	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */

	getmaxyx(win, nrows, ncols);
	clear();
	prompt("ftp");

	inited = 1;
}

void screen_dest()
{
	delwin(win);
	clrtoeol();
	refresh();
	endwin();
}

int get_command(char *command, int n)
{
	int d;
	int i = 0;
	do  {
		/* we could display as many as possible */
		d = getch();
		if (d == KEY_BACKSPACE || d == KEY_DC || d == 127) {
			/* we are at the beginning and the command is empty */
			if (i == 0)
				continue;
			/* move cursor to the point */
			move(r, --c);
			/* delete the char under the cursor */
			delch();
			/* clear the content in command */
			if (i > 0)
				command[--i] = '\0';
		} else {
			insch(d);
			/* when more than one line */
			if (c == (ncols - 1)) {
				c = -1;
				r++;
			}
			move(r, ++c);
			/* we just store n# of chars */
			if (i < n)
				command[i++] = d;
		}
	} while (d != '\n');
	r++;
	prompt("ftp");
	wrefresh(win);
	return 0;
}
