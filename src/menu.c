#include <curses.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "menu.h"
#include "control.h"
#include "interface.h"

#define NUM_OPTIONS 5
#define OPTIONS_OFFSET 3 
#define PREVIEW_OFFSET 2
#define PREVIEW_WIDTH 30
#define PREVIEW_HEIGHT 12
#define TITLE_OFFSET 2
#define TITLE_HEIGHT 4
#define YELLOW_MENU COLOR_PAIR(14) 
#define WHITE_MENU COLOR_PAIR(15) 
#define BLACK_MENU COLOR_PAIR(16) 

#define OPTIONS_POSY OPTIONS_OFFSET+TITLE_HEIGHT+TITLE_OFFSET
#define PREVIEW_POSY OPTIONS_POSY+OPTIONS_OFFSET+2*NUM_OPTIONS

pthread_mutex_t menu_mutex = PTHREAD_MUTEX_INITIALIZER;

char pac_game[(PREVIEW_HEIGHT-2)][(PREVIEW_WIDTH-2)+1] = {
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
char pac_col[(PREVIEW_HEIGHT-2)][(PREVIEW_WIDTH-2)+1] = {
" dyyyd d d d d d d d d d m m",
" d mmmmmmm d mmmmmmmmm d m m",
" d mmmmm m d mmmmmmmmm d mmm",
" d d d m mwww               ",
"mmmm d m m   mmm d mmmmmmmmm",
"mmmm d mmm   m m d mmmmmmm m",
" d d ppp     m m d d d d m m",
" d mmmmmmmmmmm mmmmmmm d m m",
" d mmmmmmmmmmmmmmmmmmm d mmm",
" d d d zzz d d d d d dxxxd d"};
char gun_game[(PREVIEW_HEIGHT-2)][(PREVIEW_WIDTH-2)+1] = {
"      * *                x x",
" ~ lqqqqqk   lqqqqqqqk   x x",
" ~ mqqqk x   mqqqqqqqj   mqj",
" ` ~ ~ x x  (*< + ++ + [\"]  ",
"qqqk ~ x x ~ lqk ~ lqqqqqqqq",
"qqqj ~ mqj ~ x x ~ mqqqqqk l",
" ~ ~ ~ ~ ~ ~ x x ~ ~ ~ ~ x x",
" ~ lqqqqqqqqqj mqqqqqk ~ x x",
" ~ mqqqqqqqqqqqqqqqqqj ~ mqj",
" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~"};
char gun_col[(PREVIEW_HEIGHT-2)][(PREVIEW_WIDTH-2)+1] = {
"                         m m",
" d mmmmmmm   mmmmmmmmm   m m",
" d mmmmm m   mmmmmmmmm   mmm",
" d d d m m  ppp + +- - www  ",
"mmmm d m m d mmm d mmmmmmmmm",
"mmmm d mmm d m m d mmmmmmm m",
" d d d d d d m m d d d d m m",
" d mmmmmmmmmmm mmmmmmm d m m",
" d mmmmmmmmmmmmmmmmmmm d mmm",
" d d d d d d d d d d d d d d"};

char title[TITLE_HEIGHT][MAP_WIDTH+1] = {
    "  XXXXXX    XX     XXXXXX     XX   XX    XX    X  XXXX ",
    "  XX  XXX  XXXX   XXXXXX      XXX XXX   XXXX   XX XXXX ",
    "  XXXXXX  XX  XX  XXXXXX      XXXXXXX  XX  XX  XXXXXXX ",
    "  XXX    XXXXXXXX  XXXXXX     XXXXXXX XXXXXXXX XXXXXXX "};

char* options[NUM_OPTIONS] = {"Play Pacman", "Play Gunman", "", "Options", "Exit"};
char* pointer = "(*<";

void menu_print_ent(int, int, int, int, char*, int);
void* delete_menu(void*);
void* pacrun_menu(void*);

void options_menu(Options*);

void print_title();
void print_options(int, char **);
void print_pointer(int, int, char*, int, char **);
void move_pointer(char , int*, int*);


void* delete_menu(void* parameters)
{
    int start_y = MAP_HEIGHT*2;
    int i,j;

    for(i=-30; i < 120; i++)
    {        
        pthread_mutex_lock(&menu_mutex);
        attron(BLACK_MENU);
        for(j=0;j<i-2;j++)
        {
            mvaddch(start_y-((i-2)-j), j, ' ');
        }
        attroff(BLACK_MENU);
        attron(WHITE_MENU);
        for(j=0;j<i;j++)
        {
            if(mvinch(start_y-(i-j), j) != ' ')
                mvaddch(start_y-(i-j), j, ' ');
        }
        
        attroff(WHITE_MENU);
        refresh();
        pthread_mutex_unlock(&menu_mutex);
        usleep(9000);
    }
}

void* pacrun_menu(void* parameters)
{
    int i,j;
    int n_sel = *((int*) parameters);
    
    const int ghost_offset = 20;
    const int ghost_num = 4;
    const int cicles = 90;

    for(i=0; i < cicles; i++)
    {
        pthread_mutex_lock(&menu_mutex);

        for(j=0; j < ghost_num; j++)
        {        
            menu_print_ent(OPTIONS_POSY+2*n_sel, i+j*4 - ghost_offset, OPTIONS_POSY+2*n_sel, i+j*4 - (ghost_offset-1), S_GHOST[0], 8-j);
        }

        menu_print_ent(OPTIONS_POSY+2*n_sel, i-(strlen(S_PACMAN[3])+2) + (MAP_WIDTH-strlen(options[n_sel]))/2, OPTIONS_POSY+2*n_sel, i-(strlen(S_PACMAN[3])+1) + (MAP_WIDTH-strlen(options[n_sel]))/2, S_PACMAN[3], 4);
        attroff(COLOR_PACMAN);
        refresh();
        pthread_mutex_unlock(&menu_mutex);

        usleep(35000);
    }

}

void menu_print_ent(int oy, int ox, int ny, int nx, char* str, int color)
{
    int i;

    for(i=0; i < strlen(str); i++)
        if(ox+i>0 && ox+i<MAP_WIDTH)
            mvaddch(oy, ox+i, ' ');
    attron(COLOR_PAIR(color));
    for(i=0; i < strlen(str); i++)
        if(nx+i>0 && nx+1+i<MAP_WIDTH)
            mvaddch(ny, nx+i, str[i]);
    attroff(COLOR_PAIR(color));
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

    char c;


    WINDOW* win_preview = newwin(PREVIEW_HEIGHT, PREVIEW_WIDTH, PREVIEW_POSY, (MAP_WIDTH-PREVIEW_WIDTH)/2);

    print_title();
    print_options(NUM_OPTIONS, options);
    print_pointer(0,0, pointer, NUM_OPTIONS, options);
    
    print_preview(win_preview, (PREVIEW_HEIGHT-2), (PREVIEW_WIDTH-2)+1, pac_game, pac_col);

    do
    {
        c = getchar();
        beep();
        n_selection = c_selection;

        move_pointer(c, &c_selection, &n_selection);
        print_pointer(c_selection, n_selection, pointer, NUM_OPTIONS, options);
        //refresh();

        c_selection = n_selection;

        switch(c_selection)
        {
            case 0:
                print_preview(win_preview, (PREVIEW_HEIGHT-2), (PREVIEW_WIDTH-2)+1, pac_game, pac_col);
                break;
            case 1:
                print_preview(win_preview, (PREVIEW_HEIGHT-2), (PREVIEW_WIDTH-2)+1, gun_game, gun_col);
                break;
            default:
                werase(win_preview);
                wrefresh(win_preview);
        }

    }
    while(c!='\r' && c!=' ');

    if(c_selection == 0 || c_selection == 1) //Game 
    {
        pthread_t pacrun_v, delete_v;
        pthread_create(&pacrun_v, NULL, &pacrun_menu, &c_selection);
        pthread_create(&delete_v, NULL, &delete_menu, NULL);

        pthread_join(pacrun_v, NULL);
        pthread_join(delete_v, NULL);
    }
    else if(c_selection == 3) //Otions
    {
        options_menu(options);
    }
    
    erase();
    delwin(win_preview);

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

    for(i = 0; i < 2; i++)
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
            for(i = 0; i < 2; i++)
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

void options_menu(Options *options)
{
    char c;
    erase();
    print_title();
    char* options_menu[2] = {"Pacman", "Gunman"};
    
    do
    {
        print_options(2, options_menu);
        print_pointer(0, 0, pointer, 2, options);
        //refresh();
        c = getchar();
    } while (c != '\r');
    
}

void print_title()
{
    int i,j;
    for(i=0;i<TITLE_HEIGHT;i++)
        for(j=0;j<MAP_WIDTH;j++)
        {
            if(title[i][j] != ' ')
            {
                attron(YELLOW_MENU);
                mvaddch(i+TITLE_OFFSET, j, title[i][j]);
                attroff(YELLOW_MENU);
            }
        }
    refresh();
}

void print_options(int num_options, char *options[num_options])
{
    int i;
    
    for(i=0; i < num_options; i++)
    {
        mvprintw(OPTIONS_POSY+2*i, (MAP_WIDTH-strlen(options[i]))/2, options[i]);
    }
    refresh();
}

void print_pointer(int opos, int npos, char* pointer, int num_options, char *options[num_options])
{
    int i;

    for(i=0; i < strlen(pointer); i++)
        mvaddch(OPTIONS_POSY+2*opos, (MAP_WIDTH-strlen(options[opos]))/2 - (i+2), ' ');
    attron(COLOR_PACMAN);
    mvprintw(OPTIONS_POSY+2*npos, (MAP_WIDTH-strlen(options[npos]))/2 - (strlen(pointer)+1), pointer);
    attroff(COLOR_PACMAN);
    refresh();
}

void move_pointer(char c, int *c_selection, int *n_selection)
{
    switch(c)
    {
        case K_UP: 
            if(*c_selection > 0)
            {
                do
                {
                    (*n_selection)--;
                }
                while(*n_selection > 0 && !strcmp("", options[*n_selection]));
            }
            break;
        case K_DOWN:
            if(*c_selection + 1 < NUM_OPTIONS)
            {
                do
                {
                    (*n_selection)++;
                }
                while(*n_selection > 0 && !strcmp("", options[*n_selection]));
            }
            break;
    }
}