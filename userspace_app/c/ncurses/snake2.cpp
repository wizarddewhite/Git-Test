// =====================================================================================
// 
//       Filename:  snake.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  02/03/2011 07:05:38 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  YOUR NAME (), 
//        Company:  
// 
// =====================================================================================

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/select.h>
#include "snake2.h"

/*window size*/
int row, col;
#define LOG_ROW 7

/* setting */
int g_level = 1;

/* two sub win, game window and log window */
WINDOW *gwin;
WINDOW *logwin;

void log(const char *str)
{
    static char logbuf[LOG_ROW][1024] = {{0}};
    static int  index = 0;
    static char EMPTYLINE[1024] = {0};

    strcpy(logbuf[index], str);
    
    int i = 0;
    for (; i < LOG_ROW-3; ++i) {
        memset(EMPTYLINE, ' ', strlen(logbuf[(index+i+1)%LOG_ROW]));
        mvwprintw(logwin, i+1, 1, EMPTYLINE);
        wrefresh(logwin);
        mvwprintw(logwin, i+1, 1, logbuf[(index+i) % LOG_ROW]);
        wrefresh(logwin);
    }
    
    index = (index + LOG_ROW - 1) % LOG_ROW;
}

void init()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    getmaxyx(stdscr, row, col);
    refresh();
    /*init the game window*/
    gwin = newwin(row-LOG_ROW, col-5, 1, 3);
    box(gwin, 0, 0);
    mvwprintw(gwin, 0, 2, " GAME ");
    mvwprintw(gwin, row-LOG_ROW-1, 2, " Level: %d ", g_level);
    mvwprintw(gwin, row-LOG_ROW-1, 30, " Speed: %d ", (int)(g_level/3));
    wrefresh(gwin);
    /*init the log window*/
    logwin = newwin(LOG_ROW-1, col-5, row-LOG_ROW +1, 3);
    box(logwin, 0, 0);
    mvwprintw(logwin, 0, 2, " LOG ");
    wrefresh(logwin);
    log("  Press 'q' or 'Q' to quit.");
    log("  Press arrow to move the snake.");
    log("Info:");
    /* rand seed */
    srand((unsigned)time(NULL));/* init rand seed */
}

void end()
{
    endwin();
}

int main()
{
    int ch;
    Food f;

    init();

    TSnake *snake = new TSnake(gwin, row-LOG_ROW, col-5);

    snake->add_head(5,9);
    snake->add_head(5,8);
    snake->add_head(5,7);
    snake->add_head(5,6);

    f.y = 9;
    f.x = 6;
    mvwprintw(gwin, f.y, f.x, "x");
    wrefresh(gwin);
    

    timeout(1000);
    while((ch = getch()) != 'q')
    {
        int res;

        snake->set_dir(ch);

        res = snake->eat(f);
        if (res == -1) /*you are so greedy*/
        {
            mvwprintw(gwin, (row-LOG_ROW)/2,(col-10)/2, "Game over!");
            wrefresh(gwin);
            timeout(-1);
            getch();
            break;
        }
        else if (res == 1)/*eat one*/
        {
            while(1)
            {
                f.x = (col-5-3)*(rand() / (RAND_MAX + 1.0))+1;
                f.y = (row-LOG_ROW-3)*(rand() / (RAND_MAX + 1.0))+1;
                if ( !snake->traverse(f) )
                    break;
            }

            mvwprintw(stdscr, 0, 10, "%3d %3d", f.y, f.x);
            mvwprintw(gwin, f.y, f.x, "x");
            wrefresh(gwin);
        }
    }

    delete snake;

    end();

    return 0;
}
