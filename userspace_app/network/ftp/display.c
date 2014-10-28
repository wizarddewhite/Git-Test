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
#include <stdarg.h>

int nrows, ncols;
int r = 0, c = 0;
WINDOW *win, *dbg_win;
int dbg = 0;

void prompt(char *s)
{
	c = 0;
	attron(A_BOLD | A_BLINK | A_REVERSE);
	mvwprintw(win, r, c, "%s>", s);
	attroff(A_BOLD | A_BLINK | A_REVERSE);
	c = c + strlen(s) + 2;
	move(r, c);
	refresh();
}

static void dbg_dump(char *comm, char d)
{
	if (!dbg)
		return;

	mvwprintw(dbg_win, 0, 0, "command: %s\n", comm);
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

static void move_cursor_v(int step)
{
	if (step == 1) {
		if (r == (nrows - 1)) {
			wscrl(win, 1);
			r--;
		}
	}
}

static void move_cursor_h(int step)
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
		move_cursor_v(1);
		move(r, ++c);
	}
}

int get_command(char *prompt_message, char *command, int n)
{
	int d;
	int i = 0;
	memset(command, n, 0);
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
			move_cursor_h(-1);
			/* delete the char under the cursor */
			delch();
			/* clear the content in command */
			if (i < n - 1)
				command[i] = '\0';
			i--;
		} else {
			insch(d);
			move_cursor_h(+1);
			/* we just store n# of chars */
			if (i < n - 1)
				command[i] = d;
			i++;
		}
		dbg_dump(command, d);

	} while (d != '\n');
	r++;
	move_cursor_v(1);
	prompt(prompt_message);
	dbg_dump(command, '\n');
	wrefresh(win);
	return 0;
}

/* move cursor according to the message format */
static inline void adjust_cursor(char *message)
{
	char *limit = message + strlen(message);

	while ((message = strchr(message,'\n'))) {
		if (message >= limit)
			break;
		message++;
		r++;
		move_cursor_v(1);
	}
}

void print_message(char *message)
{
	mvwprintw(win, r, 0, message);
	adjust_cursor(message);
	refresh();
}

void print_vmessage(char *message, ...)
{
	va_list args;
	
	move(r, 0);
	va_start(args, message);
	vwprintw(win, message, args);
	va_end(args);
	adjust_cursor(message);
	refresh();
}

void print_help()
{
	char help[] = "No such command\n";
	print_message(help);
	prompt("ftp");
}
