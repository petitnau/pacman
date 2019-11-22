#include <unistd.h>
#include <curses.h>
#include <string.h>

#include "control.h"
#include "utils.h"
#include "pacman.h"
#include "ghost.h"
#include "entity.h"
#include "interface.h"
#include "list.h"

void manage_cmd_in(int, int, GhostInfo*, PacManInfo*);
void manage_pacman_in(int, int* , CharPacman*, CharGhost* ghost, GhostInfo*, char[MAP_HEIGHT][MAP_WIDTH], int*, TempText*);
void manage_ghost_in(int, CharGhost*, char[MAP_HEIGHT][MAP_WIDTH]);

void manage_timers(TempText, CharPacman, CharGhost, char[MAP_HEIGHT][MAP_WIDTH]);

void send_ghost_info(int, GhostInfo*);
void send_pacman_info(int, PacManInfo*);

void collision_handler(CharPacman*, PacManInfo*, CharGhost*, GhostInfo*, int*, TempText*, char[MAP_HEIGHT][MAP_WIDTH]);
void food_handler(int*, int*, CharPacman, CharGhost, char[MAP_HEIGHT][MAP_WIDTH], GhostInfo*, TempText*);
void food_setup();
void eat_pause(CharPacman, PacManInfo*, CharGhost, GhostInfo*, TempText*, char[MAP_HEIGHT][MAP_WIDTH]);

void manage_logs(int log_in, MessageList* log_list)
{
    static int p=0;
    int i;
    char msg_pkg[50];

    while(read(log_in, &msg_pkg, sizeof(char)*50) != -1)
    {
        list_push(log_list, msg_pkg);
    }    

    while(log_list->count > MAP_HEIGHT-2)
        list_pop(log_list);

    i = 0;
    MessageNode* aux = log_list->tail;
    while(aux != NULL)
    {
        mvprintw(MAP_HEIGHT+GUI_HEIGHT-(2+i), 60, "%*s",50,"");
        mvprintw(MAP_HEIGHT+GUI_HEIGHT-(2+i), 60, "%s", aux->msg);
        aux = aux->prev;
        i++;
    }
    refresh();
}

void control_main(int pacman_in, int pacman_out, int ghost_in, int ghost_out, int cmd_in, int p_cmd_out, int log_in)
{
    CharPacman pacman = init_pacman_char();
    CharGhost ghost = init_ghost_char();
    PacManInfo pacman_info = init_pacman_info();
    GhostInfo ghost_info = init_ghost_info();

    TempText temp_text = {};

    int score = 0;
    char game_food[MAP_HEIGHT][MAP_WIDTH];
    int eaten_dots = 0;

    food_setup(game_food);

    MessageList log_list;
    list_init(&log_list);

    while(1)
    {  
        manage_cmd_in(cmd_in, p_cmd_out, &ghost_info, &pacman_info);
        manage_pacman_in(pacman_in, &eaten_dots, &pacman, &ghost, &ghost_info, game_food, &score, &temp_text);
        manage_ghost_in(ghost_in, &ghost, game_food);
        manage_timers(temp_text, pacman, ghost, game_food);
        collision_handler(&pacman, &pacman_info, &ghost, &ghost_info, &score, &temp_text, game_food);
        send_pacman_info(pacman_out, &pacman_info);
        send_ghost_info(ghost_out, &ghost_info);
        print_ui(score, pacman, ghost);
        print_temp_text(temp_text);

        manage_logs(log_in, &log_list);
        if(eaten_dots == 240)
        {
            mvprintw(23, 23, "GAME OVER! VITTORIA");
            refresh();
            return;
        }
        if(pacman.lives < 0){
            attron(COLOR_PAIR(5));
            mvprintw(23, 23, "G A M E   O V E R !");
            attroff(COLOR_PAIR(5));
            refresh();
            return;
        }
    }
}

void food_setup(char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    int i;
    for (i = 0; i < MAP_HEIGHT; i++)
    {
        strcpy(game_food[i], PELLETS[i]);
    } 
}

void manage_cmd_in(int cmd_in, int p_cmd_out, GhostInfo* ghost_info, PacManInfo* pacman_info)
{
    Direction direction;
    char c_in;

    while(read(cmd_in, &c_in, sizeof(c_in)) != -1)
    {
        switch(c_in)
        {
            case K_UP:
                direction = UP;
                break;
            case K_LEFT:
                direction = LEFT;
                break;
            case K_DOWN:
                direction = DOWN;
                break;
            case K_RIGHT:
                direction = RIGHT;
                break;
        }
        if(c_in == K_UP || c_in == K_DOWN || c_in == K_RIGHT || c_in == K_LEFT)
        {
            ghost_info->resume = true;
            ghost_info->new = true;
            pacman_info->resume = true;
            pacman_info->new = true;
            write(p_cmd_out, &direction, sizeof(direction));
        }

    }
}

void manage_pacman_in(int pacman_in, int* eaten_dots, CharPacman* pacman, CharGhost* ghost, GhostInfo* info_pkg, char game_food[MAP_HEIGHT][MAP_WIDTH], int* score, TempText* temp_text)
{
    CharPacman pacman_pkg;

    while(read(pacman_in, &pacman_pkg, sizeof(pacman_pkg)) != -1)
    {
        unprint_area(pacman->e.p.y, pacman->e.p.x-1, 3, game_food);
        *pacman = pacman_pkg;
        food_handler(score, eaten_dots, *pacman, *ghost, game_food, info_pkg, temp_text);
        
        info_pkg->pacman = pacman->e;
        info_pkg->new = true;
    }
}

void manage_ghost_in(int ghost_in, CharGhost* ghost, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    CharGhost ghost_pkg;

    while(read(ghost_in, &ghost_pkg, sizeof(ghost_pkg)) != -1)
    {
        unprint_area(ghost->e.p.y, ghost->e.p.x-1, 3, game_food);
        *ghost = ghost_pkg;
    }
}

void manage_timers(TempText temp_text, CharPacman pacman, CharGhost ghost, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    if(temp_text.timer != 0)
        if(!check_timer(temp_text.timer))
        {
            sunprint_area(temp_text.p.y, temp_text.p.x, strlen(temp_text.text), game_food, pacman, ghost);
            temp_text.timer = 0;
        }
}

void send_ghost_info(int ghost_out, GhostInfo* ghost_info)
{    
    if(ghost_info->new)
    {
        write(ghost_out, ghost_info, sizeof(*ghost_info));
        *ghost_info = init_ghost_info();
    }
}

void send_pacman_info(int pacman_out, PacManInfo* pacman_info)
{    
    if(pacman_info->new)
    {
        write(pacman_out, pacman_info, sizeof(*pacman_info));
        *pacman_info = init_pacman_info();
    }
}

void food_handler(int* score, int* eaten_dots, CharPacman pacman, CharGhost ghost, char game_food[MAP_HEIGHT][MAP_WIDTH], GhostInfo *ghost_info, TempText *temp_text)
{  
    int i,j ;
    Position pe_pos;

    for(i=-1; i<=1; i++)
    {
        pe_pos.x = pacman.e.p.x+i;
        pe_pos.y = pacman.e.p.y;
        pe_pos = get_pac_eff_pos(pe_pos);

        switch(game_food[pe_pos.y][pe_pos.x])
        {
            case '~':
                *score += 10;
                *eaten_dots +=1;
                game_food[pe_pos.y][pe_pos.x] = ' ';
                if(*eaten_dots == 70 || *eaten_dots == 170){
                    //spawna un frutto va tutto in funzione col controllo
                    print_fruit();
                    game_food[FRUIT_POS_Y][FRUIT_POS_X-1] = FRUIT[0][0];
                    game_food[FRUIT_POS_Y][FRUIT_POS_X] = FRUIT[0][1];
                    game_food[FRUIT_POS_Y][FRUIT_POS_X+1] = FRUIT[0][2];
                }
                beep();
                break;
            case '`': 
                *score += 50;
                
                ghost_info->fright = true;
                ghost_info->new = true;    
                game_food[pe_pos.y][pe_pos.x] = ' ';
                break;
            case '^':
                *score += 100; //*lvl?

                sunprint_area(temp_text->p.y, temp_text->p.x, strlen(temp_text->text), game_food, pacman, ghost);
                create_temp_text(temp_text, pe_pos.x-1, pe_pos.y+GUI_HEIGHT, "200", 2e3);
                for(j=-1; j<=1; j++)
                    game_food[pe_pos.y][pe_pos.x+j] = ' '; 
                
                mvaddch(pe_pos.y+GUI_HEIGHT, pe_pos.x+i, ' ');
                refresh();
                break;
        }

    }
}

void collision_handler(CharPacman *pacman, PacManInfo *pacman_info, CharGhost *ghost, GhostInfo* ghost_info, int* score, TempText* temp_text, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    if(pacman->e.p.x == ghost->e.p.x && pacman->e.p.y == ghost->e.p.y)
    {
        if(ghost->mode == M_FRIGHT)
        {
            *score += 200; //200 400 800 1600
            ghost->mode = M_DEAD;
            ghost_info->death = true;
            ghost_info->new = true;
            eat_pause(*pacman, pacman_info, *ghost, ghost_info, temp_text, game_food);
            // morto il fantasma
        }
        else if(ghost->mode != M_DEAD && !pacman->dead)
        {
            //perdi una vita
            pacman->dead = true;
            pacman_info->death = true;
            pacman_info->new = true;
            ghost_info->full = true;
            ghost_info->new = true;
            //pacman viene riportato alla pos. inziiale idem
        }
    }
}

void eat_pause(CharPacman pacman, PacManInfo* pacman_info, CharGhost ghost, GhostInfo* ghost_info, TempText* temp_text, char game_food[MAP_HEIGHT][MAP_WIDTH])
{        
    sunprint_area(temp_text->p.y, temp_text->p.x, strlen(temp_text->text), game_food, pacman, ghost);
    create_temp_text(temp_text, pacman.e.p.x-1, pacman.e.p.y+GUI_HEIGHT, "200", 1e3);
    ghost_info->sleeptime = 1e6;    
    ghost_info->new = true;
    pacman_info->sleeptime = 1e6;    
    pacman_info->new = true;
}