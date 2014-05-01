/*
 * =====================================================================================
 *
 *       Filename:  snake2.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/04/2011 09:30:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef SNAKE2_H
#define SNAKE2_H
#include <ncurses.h>
enum TDirection {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};
class Food
{
public:
    int y;
    int x;
};
class TSnakeNode {
public:
    int y;
    int x;
    TSnakeNode *prev;
    TSnakeNode(int y, int x, TSnakeNode *p = NULL)
    {
        this->y = y;
        this->x = x;
        prev = p;
    }
    virtual ~TSnakeNode()
    {}
};
class TSnake {
public:
    WINDOW *win;
    TDirection  dir;
    int row, col;
    TSnakeNode *tail;
    TSnakeNode *head;
    TSnake(WINDOW *win, int y, int x ): dir(DIR_LEFT), row(y), col(x),
        tail(NULL),head(NULL)
    { 
        this->win = win;
    }
    virtual ~TSnake()
    {
        TSnakeNode *tmp = NULL;
        TSnakeNode *tmp2 = NULL;

        for (tmp = tail; tmp != NULL; )
        {
            tmp2 = tmp;
            tmp=tmp->prev;
            delete tmp2;
        }

    }
    void add_head(int y, int x)
    {
        TSnakeNode* h = new TSnakeNode(y,x); 
        if (head != NULL)
        {
            head->prev = h;
        }
        else
        {
            tail = h;
        }
        head = h;
        mvwprintw(stdscr, 0, 0, "%3d %3d", h->y, h->x);
        mvwprintw(win, h->y, h->x, "@");
        wrefresh(win);
    }
    void del_tail()
    {
        TSnakeNode *tmp = NULL;
        if (tail != NULL)
        {
            tmp = tail;
            tail = tail->prev;
        }

        mvwprintw(win, tmp->y, tmp->x, " ");
        wrefresh(win);

        if (tmp != NULL)
        {
            delete tmp;
        }
    }
    void show()
    {
        TSnakeNode *tmp = NULL;

        for (tmp = tail; tmp != NULL; tmp=tmp->prev)
        {
            mvwprintw(win, tmp->y, tmp->x, "@");
        }
        wrefresh(win);
    }
    void unshow()
    {
        TSnakeNode *tmp = NULL;

        for (tmp = tail; tmp != NULL; tmp=tmp->prev)
        {
            mvwprintw(win, tmp->y, tmp->x, " ");
        }
        wrefresh(win);
    }

    void next_position(int& y, int& x, TDirection new_dir)
    {
        /*calculate the next position*/
        if (new_dir == DIR_LEFT)
        {
            y = head->y;
            x = head->x-1;
        }
        else if (new_dir == DIR_RIGHT)
        {
            y = head->y;
            x = head->x+1;
        }
        else if (new_dir == DIR_UP)
        {
            y = head->y-1;
            x = head->x;
        }
        else if (new_dir == DIR_DOWN)
        {
            y = head->y+1;
            x = head->x;
        }

        /* adjust the pos */
        if (y>row-2)
        {
            y = 1;
        }
        else if (y<1)
        {
            y = row-2;
        }

        if (x>col-2)
        {
            x = 1;
        }
        else if (x<1)
        {
            x = col-2;
        }

    }
    void set_dir(int ch)
    {
        int y,x;
        TDirection new_dir = dir;

        if (ch == KEY_LEFT)
            new_dir = DIR_LEFT;
        if (ch == KEY_RIGHT)
            new_dir = DIR_RIGHT; 
        if (ch == KEY_DOWN)
            new_dir = DIR_DOWN; 
        if (ch == KEY_UP)
            new_dir = DIR_UP;

        next_position(y, x, new_dir);
        
        if (traverse(y, x))
            return;

        dir = new_dir;
        return;
    }
    
    int eat(Food f)
    {
        int y, x;

        next_position(y, x, dir);

        if (traverse(y,x))
            return -1;

        add_head(y,x);

        if(y != f.y || x != f.x)
        {
            del_tail();
            return 0;
        }
        else
            return 1;
    }

    int traverse(const Food &f)
    {
        return traverse(f.y, f.x);
    }

    /*return 1 if (y,x) is in the snake body*/
    int traverse(const int y, const int x)
    {
        TSnakeNode *tmp = NULL;

        for (tmp = tail; tmp != NULL; tmp=tmp->prev)
        {
            if (tmp->y == y && tmp->x == x)
                return 1;
        }
        return 0;
    }
};

#endif
