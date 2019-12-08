#include <curses.h>
#include <pthread.h>
#include <unistd.h>
#include "menu.h"
#include "control.h"
#include "interface.h"

#define OFFSET_OPTIONS 10
#define YELLOW_MENU COLOR_PAIR(14) 
#define WHITE_MENU COLOR_PAIR(15) 
#define BLACK_MENU COLOR_PAIR(16) 

pthread_mutex_t menu_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    WINDOW* win;
    int y;
} Pacrun_Par;

void menu_print_ent(WINDOW*, int, int, int, int, char*, int);
void* delete_menu(void*);
void* pacrun_menu(void*);

void* delete_menu(void* parameters)
{
    int i,j;
    WINDOW* win = (WINDOW*) parameters;

    for(i=-100; i < 100; i++)
    {        
        pthread_mutex_lock(&menu_mutex);
        wattron(win, BLACK_MENU);
        for(j=0;j<i-2;j++)
        {
            mvwaddch(win, MAP_HEIGHT-((i-2)-j), j, ' ');
        }
        wattroff(win, BLACK_MENU);
        wattron(win, WHITE_MENU);
        for(j=0;j<i;j++)
        {
            if(mvwinch(win, MAP_HEIGHT-(i-j), j) != ' ')
                mvwaddch(win, MAP_HEIGHT-(i-j), j, ' ');
        }
        
        wattroff(win, WHITE_MENU);
        wrefresh(win);
        pthread_mutex_unlock(&menu_mutex);
        usleep(9000);
    }
}

void* pacrun_menu(void* parameters)
{
    int i,j;
    Pacrun_Par* par = (Pacrun_Par*) parameters;
    WINDOW* win = par->win;
    int y = par->y;

    for(i=0; i < 90; i++)
    {
        pthread_mutex_lock(&menu_mutex);

        for(j=0; j < 4; j++)
        {        
            menu_print_ent(win, OFFSET_OPTIONS+2*y, -21+i+j*4, OFFSET_OPTIONS+2*y, -20+i+j*4, S_GHOST[0], 8-j);
        }

        menu_print_ent(win, OFFSET_OPTIONS+2*y, MAP_WIDTH/2 - 7+i, OFFSET_OPTIONS+2*y, MAP_WIDTH/2 - 6+i, S_PACMAN[3], 4);
        wattroff(win, COLOR_PACMAN);
        wrefresh(win);
        pthread_mutex_unlock(&menu_mutex);

        usleep(35000);
    }

}

void menu_print_ent(WINDOW* win, int oy, int ox, int ny, int nx, char* str, int color)
{
    int i;

    for(i=0; i < strlen(str); i++)
        if(ox+i>0 && ox+i<MAP_WIDTH)
            mvwaddch(win, oy, ox+i, ' ');
    wattron(win, COLOR_PAIR(color));
    for(i=0; i < strlen(str); i++)
        if(nx+i>0 && nx+1+i<MAP_WIDTH)
            mvwaddch(win, ny, nx+i, str[i]);
    wattroff(win, COLOR_PAIR(color));
}

int main_menu()
{
    int i,j,k;

    int c_selection = 0;
    int n_selection = 0;
    int num_choices = 2;
    char c;
/*
    char* title[9] = {
    " _____________________________________________________ ",
    "/ ___________________________________________________ \\",
    "|/ ____             ____                          __ \\|",
    "|||    \\    /\\     /    \\    |\\   /|    /\\    |\\ |  |||",
    "|||  o  |  /  \\   /    /     | \\_/ |   /  \\   | \\|  |||",
    "|||  __/  / [] \\  \\    \\     |     |  / [] \\  |     |||",
    "|||_|    /______\\  \\____/    |_____| /______\\ |_____|||",
    "|\\___________________________________________________/|",
    "\\_____________________________________________________/"};
*/
    char title[8][MAP_WIDTH+1] = {
    "                                                       ",
    "                                                       ",
    "  XXXXXX    XX     XXXXXX     XX   XX    XX    X  XXXX ",
    "  XX  XXX  XXXX   XXXXXX      XXX XXX   XXXX   XX XXXX ",
    "  XXXXXX  XX  XX  XXXXXX      XXXXXXX  XX  XX  XXXXXXX ",
    "  XXX    XXXXXXXX  XXXXXX     XXXXXXX XXXXXXXX XXXXXXX ",
    "                                                       ",
    "                                                       "};



    WINDOW* win = newwin(MAP_HEIGHT, MAP_WIDTH, GUI_HEIGHT, 0);

    keypad(win, true);

    box(win,0,0);
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window

    wattron(win, COLOR_PACMAN);
    mvwprintw(win, OFFSET_OPTIONS+0, MAP_WIDTH/2 - 6, "(*<");
    wattroff(win, COLOR_PACMAN);
    mvwprintw(win, OFFSET_OPTIONS+0, MAP_WIDTH/2 - 2, "PacMan");

    mvwprintw(win, OFFSET_OPTIONS+2, MAP_WIDTH/2 - 2, "GunMan");
    wrefresh(win);

    for(i=0;i<8;i++)
        for(j=0;j<MAP_WIDTH;j++)
        {
            if(title[i][j] != ' ')
            {
                wattron(win, YELLOW_MENU);
                mvwaddch(win, i, j, title[i][j]);
                wattroff(win, YELLOW_MENU);
            }
        }

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
        }

        mvwprintw(win, OFFSET_OPTIONS+2*c_selection, MAP_WIDTH/2 - 6, "   ");
        wattron(win, COLOR_PACMAN);
        mvwprintw(win, OFFSET_OPTIONS+2*n_selection, MAP_WIDTH/2 - 6, "(*<");
        wattroff(win, COLOR_PACMAN);
        wrefresh(win);
        c_selection = n_selection;

    }
    while(c!='\r');

    pthread_t pacrun_v, delete_v;
    Pacrun_Par pacrun_par = {win, c_selection};
    pthread_create(&pacrun_v, NULL, &pacrun_menu, &pacrun_par);
    pthread_create(&delete_v, NULL, &delete_menu, win);

    pthread_join(pacrun_v, NULL);
    pthread_join(delete_v, NULL);

    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '); // Erase frame around the window
    wrefresh(win);
    delwin(win);

    return c_selection;
}
