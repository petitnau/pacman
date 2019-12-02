#include <curses.h>
#include <unistd.h>
#include "menu.h"
#include "control.h"

int main_menu()
{
    int c_selection = 0;
    int n_selection = 0;
    int num_choices = 2;
    char c;

    WINDOW* win = newwin(MAP_HEIGHT, MAP_WIDTH, GUI_HEIGHT, 0);

    keypad(win, true);

    box(win,0,0);

    wattron(win, COLOR_PACMAN);
    mvwprintw(win, 5, MAP_WIDTH/2 - 6, "(*<");
    wattroff(win, COLOR_PACMAN);
    mvwprintw(win, 5, MAP_WIDTH/2 - 2, "PacMan");

    mvwprintw(win, 7, MAP_WIDTH/2 - 2, "GunMan");
    wrefresh(win);

    do
    {
        c=getchar();
        switch(c)
        {
            case K_UP: 
                if(c_selection > 0)
                {
                    n_selection = c_selection-1;
                }
                break;
            case K_DOWN:
                if(c_selection < num_choices - 1)
                {
                    n_selection = c_selection+1;
                }
                break;
            case ' ':
            case '\r':
                //animazione
                return c_selection;
        }

        mvwprintw(win, 5+2*c_selection, MAP_WIDTH/2 - 6, "   ");
        wattron(win, COLOR_PACMAN);
        mvwprintw(win, 5+2*n_selection, MAP_WIDTH/2 - 6, "(*<");
        wattroff(win, COLOR_PACMAN);
        wrefresh(win);
        c_selection = n_selection;

    }
    while(c!='\r');
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window
    wrefresh(win);
    delwin(win);

    getchar();
}