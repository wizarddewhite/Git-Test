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
WINDOW *win, *dbg_win;
int dbg = 0;

static void prompt(char *s)
{
	c = 0;
	mvwprintw(win, r, c, "%s>", s);
	c = c + strlen(s) + 1;
	move(r, c);
	refresh();
}

static void dbg_dump(char *comm, char d)
{
	if (!dbg)
		return;

	mvwprintw(dbg_win, 0, 0, "command: %s", comm);
	mvwprintw(dbg_win, 1, 0,
		"window size: (%03d, %03d) current pos: (%03d, %03d), char: %03d",
		nrows, ncols, r, c, d);
	wrefresh(dbg_win);
}

void screen_init(int debug)
{
	static int inited = 0;

	if (inited)
		return;

	dbg = debug;

	win = initscr();
	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */
	keypad(win, TRUE);
	/* enable win scrolling */
	scrollok(win, TRUE);

	getmaxyx(win, nrows, ncols);
	if (dbg) {
		nrows -= 2;
		dbg_win = newwin(2, ncols, nrows, 0);
	}

	clear();
	prompt("ftp");

	inited = 1;
}

void screen_dest()
{
	if (dbg)
		delwin(dbg_win);
	delwin(win);
	clrtoeol();
	refresh();
	endwin();
}

void move_cursor(int step)
{
	if (step == -1) {
		if (c == 0) {
			r--;
			c = ncols;
		}
		move(r, --c);
	} else if (step == 1) {
		/* we are at the end of this line */
		if (c == (ncols - 1)) {
			c = -1;
			r++;
		}
		/* end of the window */
		if (r == (nrows - 1)) {
			wscrl(win, 1);
			r--;
		}
		move(r, ++c);
	}
}

int get_command(char *command, int n)
{
	int d;
	int i = 0;
	do  {
		/* we could display as many as possible */
		d = getch();
		if (d == KEY_LEFT || d == KEY_RIGHT
			  || d == KEY_UP || d == KEY_DOWN) {
			continue;
		} else if (d == KEY_BACKSPACE || d == KEY_DC || d == 127) {
			/* we are at the beginning and the command is empty */
			if (i == 0)
				continue;
			move_cursor(-1);
			/* delete the char under the cursor */
			delch();
			/* clear the content in command */
			command[--i] = '\0';
		} else {
			insch(d);
			move_cursor(+1);
			/* we just store n# of chars */
			if (i < n)
				command[i++] = d;
		}
		dbg_dump(command, d);

	} while (d != '\n');
	r++;
	prompt("ftp");
	dbg_dump(command, '\n');
	wrefresh(win);
	return 0;
}
