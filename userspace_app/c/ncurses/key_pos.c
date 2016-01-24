/*
 * =====================================================================================
 *
 *       Filename:  key_pos.c
 *
 *    Description:  This is a small example for curse movement 
 *
 *        Version:  1.0
 *        Created:  02/02/2011 12:12:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <ncurses.h>
#include <signal.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>

#define INFO COLOR_PAIR(1)
#define CUR  COLOR_PAIR(2)
#define CLR  COLOR_PAIR(3)

int row, col;
int y, x;

static void sig_winch(int signo);

static void onsig(int n GCC_UNUSED)
{
	curs_set(1);
	endwin();
	exit(EXIT_FAILURE);
}

static int rand_pos(int max)
{
	return max * (rand() / (RAND_MAX + 1.0));
}

static void redraw_cursor(int oy, int ox, int y, int x)
{
	/* clean the last movement 
	 * move to a new pos */
	attrset(CLR);
	mvaddstr(oy, ox, " ");
	attrset(CUR);
	mvaddstr(y, x, " ");
	refresh();
}

static void update_info(int y, int x)
{
	attrset(INFO);
	mvprintw(0, 0, "%3d %3d", row, col);
	mvprintw(1, 0, "%3d %3d", y, x);
	refresh();
}

int init()
{
	int j;
	setlocale(LC_ALL, "");

	for (j = SIGHUP; j <= SIGTERM; j++)
		if (signal(j, SIG_IGN) != SIG_IGN)
			signal(j, onsig);

	signal(SIGWINCH, sig_winch);
	/* initialize the screen */
	initscr();
	/* turn on the color */
	if (has_colors()) {
		int bg = COLOR_BLACK;
		start_color();
#if HAVE_USE_DEFAULT_COLORS
		if (use_default_colors() == OK)
			bg = -1;
#endif
		init_pair(1, COLOR_GREEN, COLOR_WHITE);
		init_pair(2, COLOR_BLUE, COLOR_WHITE);
		init_pair(3, COLOR_BLUE, bg);
	}

	nl();
	noecho(); /*no echo*/
	cbreak(); /* Line buffering disabled. pass on everything */
	keypad(stdscr, TRUE);/* enable the Functioins key */
	curs_set(0);/* not show the curse */
	getmaxyx(stdscr, row, col);/* get the height and width of the window */
	printf("test %d, %d\n", row, col);
	srand((unsigned)time(NULL));/* init rand seed */
	update_info(row, col);

	return 0;
}

static void sig_winch(int signo)
{
	endwin();
	init();
	refresh();
}

int main( int argc , char *argv[]GCC_UNUSED)
{
	int x1, y1;
	int ch;

	init();
	y = rand_pos(row);
	x = rand_pos(col);

	while((ch = getch()) != 'q') {
		switch (ch)
		{
		case KEY_LEFT:
			x1 = (x-1);
			break;
		case KEY_RIGHT:
			x1 = (x+1);
			break;
		case KEY_UP:
			y1 = (y-1);
			break;
		case KEY_DOWN:
			y1 = (y+1);
			break;
		default:
			y1 = rand_pos(row);
			x1 = rand_pos(col);
		}

		/* wrap the cursor */
		if (x1>=col)
			x1 = 0;
		if (x1<0)
			x1 = col;
		if (y1>=row)
			y1 = 0;
		if (y1<0)
			y1 = row;

		redraw_cursor(y, x, y1, x1);
		update_info(y1, x1);
		y = y1, x=x1;
		//napms(50);
	} 

	endwin();
	return 0;
}
