/*
 * =====================================================================================
 *
 *       Filename:  key_usage.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/03/2011 10:27:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <ncurses.h>

#define WIDTH 30
#define HEIGHT 10 

int startx = 0;
int starty = 0;

char *choices[] = { 
			"Choice 1",
			"Choice 2",
			"Choice 3",
			"Choice 4",
			"Exit",
		  };

int n_choices = sizeof(choices) / sizeof(char *);

void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;	

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i) {
		/* High light the present choice */
		if(highlight == i + 1) {
			wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		} else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

int main()
{	
	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	initscr();
	clear();
	noecho();
	cbreak();	/* Line buffering disabled. pass on everything */
	startx = (80 - WIDTH) / 2;
	starty = (24 - HEIGHT) / 2;
		
	/* create a window with HEIGHT and WIDTH
	 * start at positioin starty and startx
	 */
	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	print_menu(menu_win, highlight);
	while(1) {
		c = wgetch(menu_win);
		switch(c) {
		case KEY_UP:
			if(highlight == 1)
				highlight = n_choices;
			else
				--highlight;
			break;
		case KEY_DOWN:
			if(highlight == n_choices)
				highlight = 1;
			else 
				++highlight;
			break;
		case 10:// Enter
		/* User did a choice come out of the infinite loop */
			choice = highlight;
			mvprintw(23, 0, "You chose choice %d with choice string \"%s\"\n", choice, choices[choice - 1]);
			break;
		default:
			mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
			break;
		}
		refresh();
		print_menu(menu_win, highlight);
		if (choice == 5)
			break;
	}	
	delwin(menu_win);
	clrtoeol();
	refresh();
	endwin();
	return 0;
}


