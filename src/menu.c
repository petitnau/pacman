#include <curses.h>
#include <string.h>
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

char pac_game[10][28+1] = {
" ~[\"]~ ~ ~ ~ ~ ~ ~ ~ ~ ~ x x",
" ~ lqqqqqk ~ lqqqqqqqk ~ x x",
" ~ mqqqk x ~ mqqqqqqqj ~ mqj",
" ` ~ ~ x x[\"]               ",
"qqqk ~ x x   lqk ~ lqqqqqqqq",
"qqqj ~ mqj   x x ~ mqqqqqk l",
" ~ ~ >*)     x x ~ ~ ~ ~ x x",
" ~ lqqqqqqqqqj mqqqqqk ~ x x",
" ~ mqqqqqqqqqqqqqqqqqj ~ mqj",
" ~ ~ ~ [\"] ~ ~ ~ ~ ~ ~[\"]~ ~"};
char pac_col[10][28+1] = {
" dyyyd d d d d d d d d d m m",
" d mmmmmmm d mmmmmmmmm d m m",
" d mmmmm m d mmmmmmmmm d mmm",
" ` d d m mwww              ",
"mmmm d m m   mmm d mmmmmmmmm",
"mmmm d mmm   m m d mmmmmmm m",
" d d ppp     m m d d d d m m",
" d mmmmmmmmmmm mmmmmmm d m m",
" d mmmmmmmmmmmmmmmmmmm d mmm",
" d d d zzz d d d d d dxxxd d"};
char gun_game[10][28+1] = {
"      * *                x x",
" ~ lqqqqqk   lqqqqqqqk   x x",
" ~ mqqqk x   mqqqqqqqj   mqj",
" ` ~ ~ x x (*< + +  + + [\"] ",
"qqqk ~ x x ~ lqk ~ lqqqqqqqq",
"qqqj ~ mqj ~ x x ~ mqqqqqk l",
" ~ ~ ~ ~ ~ ~ x x ~ ~ ~ ~ x x",
" ~ lqqqqqqqqqj mqqqqqk ~ x x",
" ~ mqqqqqqqqqqqqqqqqqj ~ mqj",
" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~"};
char gun_col[10][28+1] = {
"                         m m",
" d mmmmmmm   mmmmmmmmm   m m",
" d mmmmm m   mmmmmmmmm   mmm",
" ` d d m m ppp + +  - - www ",
"mmmm d m m d mmm d mmmmmmmmm",
"mmmm d mmm d m m d mmmmmmm m",
" d d d d d d m m d d d d m m",
" d mmmmmmmmmmm mmmmmmm d m m",
" d mmmmmmmmmmmmmmmmmmm d mmm",
" d d d d d d d d d d d d d d"};

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

void print_preview(WINDOW* win, int r, int c, char preview[r][c], char colors[r][c])
{
    int i,j;

    werase(win);

    wattron(win, COLOR_MAP);
    box(win,0,0);
    wattroff(win, COLOR_MAP);
    for(i=0;i<r;i++)
        for(j=0;j<c-1;j++)
        {
            switch(colors[i][j])
            {
                case 'w':
                    wattron(win, COLOR_BLINKY);
                    break;
                case 'x':
                    wattron(win, COLOR_PINKY);
                    break;
                case 'y':
                    wattron(win, COLOR_INKY);
                    break;
                case 'z':
                    wattron(win, COLOR_CLYDE);
                    break;
                case 'm':
                    wattron(win, COLOR_MAP);
                    break;
                case 'd':
                    wattron(win, COLOR_PELLETS);
                    break;
                case 'p':
                    wattron(win, COLOR_PACMAN);
                    break;
                case '+':
                    wattron(win, COLOR_PAIR(17));
                    break;
                case '-':
                    wattron(win, COLOR_REDTEXT);
                    break;
            }

            if(preview[i][j] == '>' || preview[i][j] == '<' || preview[i][j] == '*' || preview[i][j] == ')' || preview[i][j] == '(' || preview[i][j] == '+' || preview[i][j] == '[' || preview[i][j] == '"' || preview[i][j] == ']')
                mvwaddch(win, i+1, j+1, preview[i][j]);
            else
                mvwaddch(win, i+1, j+1, NCURSES_ACS(preview[i][j]));

            switch(colors[i][j])
            {
                case 'w':
                    wattroff(win, COLOR_BLINKY);
                    break;
                case 'x':
                    wattroff(win, COLOR_PINKY);
                    break;
                case 'y':
                    wattroff(win, COLOR_INKY);
                    break;
                case 'z':
                    wattroff(win, COLOR_CLYDE);
                    break;
                case 'm':
                    wattroff(win, COLOR_MAP);
                    break;
                case 'd':
                    wattroff(win, COLOR_PELLETS);
                    break;
                case 'p':
                    wattroff(win, COLOR_PACMAN);
                    break;
                case '+':
                    wattroff(win, COLOR_PAIR(17));
                    break;
                case '-':
                    wattroff(win, COLOR_REDTEXT);
                    break;
            }
        }
    wrefresh(win); 
}

int main_menu()
{
    int i,j,k;

    int c_selection = 0;
    int n_selection = 0;
    int num_choices = 2;
    char c;

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
    WINDOW* win2 = newwin(12, 30, 22, (MAP_WIDTH-30)/2);

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

    print_preview(win2, 10, 28+1, pac_game, pac_col);

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

        switch(c_selection)
        {
            case 0:
                print_preview(win2, 10, 28+1, pac_game, pac_col);
                break;
            case 1:
                print_preview(win2, 10, 28+1, gun_game, gun_col);
                break;
        }

    }
    while(c!='\r' && c!=' ');

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

int pause_menu(ControlData* cd)
{
    int c_selection = 0;
    int n_selection = 0;
    int num_choices = 2;
    char c;
    int i;

    char *options[2] = { "-Riprendi", "-Esci"};
    
    WINDOW* win = newwin(9, 19, 17, 18); //y x, sty, stx
        
    box(win, 0, 0);

    for( i = 0; i < 2; i++)
            {
                if(i == c_selection)
                    wattron(win, COLOR_BLINKY);
                mvwprintw(win, 3+i, 5, options[i]);
                wattroff(win, COLOR_BLINKY);
            }
            wrefresh(win);
    
    do
    {
        while(read(cd->pipes->cmd_in, &c, sizeof(c)) != -1)
        {
            

            switch(c)
            {
                case K_UP: 
                    if(c_selection > 0)
                    {
                        c_selection--;
                    }
                    break;
                case K_DOWN:
                    if(c_selection < num_choices - 1)
                    {
                       c_selection++;
                    }
                    break;
            }
            for( i = 0; i < 2; i++)
            {
                if(i == c_selection)
                    wattron(win, COLOR_BLINKY);
                mvwprintw(win, 3+i, 5, options[i]);
                wattroff(win, COLOR_BLINKY);
            }
            wrefresh(win);
        }
    }
    while(c != '\n');
    
    return c_selection;
}