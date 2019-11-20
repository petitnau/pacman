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

void manage_pacman_in(int, CharPacman*, GhostInfo*, char[MAP_HEIGHT][MAP_WIDTH], int*);
void manage_ghost_in(int, CharGhost*, char[MAP_HEIGHT][MAP_WIDTH]);
void send_ghost_info(int, GhostInfo*);

void collision_handler(CharPacman* pacman, CharGhost* ghost, GhostInfo* ghost_info);
void food_handler(int* score, Position pos, int rows, int col, char game_food[rows][col], GhostInfo *ghost_info);
void food_setup();

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

void control_main(int pacman_in, int ghost_in, int ghost_out, int log_in)
{
    CharPacman pacman = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, PAC_START_LIVES};
    CharGhost ghost = {{GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR}, M_CHASE};
    GhostInfo ghost_info = {pacman.e, false, false, false};

    int score = 0;
    char game_food[MAP_HEIGHT][MAP_WIDTH];
    int eaten_dots;

    food_setup(game_food);

    MessageList log_list;
    list_init(&log_list);

    while(1)
    {  
        manage_pacman_in(pacman_in, &pacman, &ghost_info, game_food, &score);
        manage_ghost_in(ghost_in, &ghost, game_food);
        collision_handler(&pacman, &ghost, &ghost_info);    
        send_ghost_info(ghost_out, &ghost_info);
        print_ui(score, pacman, ghost);
        manage_logs(log_in, &log_list);
        refresh();
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

void manage_pacman_in(int pacman_in, CharPacman* pacman, GhostInfo* info_pkg, char game_food[MAP_HEIGHT][MAP_WIDTH], int* score)
{
    CharPacman pacman_pkg;

    int i;

    while(read(pacman_in, &pacman_pkg, sizeof(pacman_pkg)) != -1)
    {
        unprint_entity(pacman->e, game_food);
        *pacman = pacman_pkg;
        food_handler(&score, pacman->e.p, MAP_HEIGHT, MAP_WIDTH, game_food, info_pkg);
        
        info_pkg->pacman = pacman->e;
        info_pkg->new = true;
    }
}

void manage_ghost_in(int ghost_in, CharGhost* ghost, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    CharGhost ghost_pkg;

    int i;

    while(read(ghost_in, &ghost_pkg, sizeof(ghost_pkg)) != -1)
    {
        unprint_entity(ghost->e, game_food);
        *ghost = ghost_pkg;
    }
}

void send_ghost_info(int ghost_out, GhostInfo* ghost_info)
{    
    if(ghost_info->new)
    {
        write(ghost_out, ghost_info, sizeof(*ghost_info));
        ghost_info->fright = false;
        ghost_info->death = false;
        ghost_info->full = false;
        ghost_info->new = false;
    }
}

void food_handler(int* score, Position pos, int rows, int col, char game_food[rows][col], GhostInfo *ghost_info)
{  
    int i;
    Position pe_pos;

    for(i=-1; i<=1; i++)
    {
        pe_pos.x = pos.x+i;
        pe_pos.y = pos.y;
        pe_pos = get_pac_eff_pos(pe_pos);

        switch(game_food[pe_pos.y][pe_pos.x])
        {
            case '~':
                *score += 10;
                beep();
                break;
            case '`': 
                *score += 50;
                ghost_info->fright = true;
                ghost_info->new = true;
                break;
        }
        game_food[pe_pos.y][pe_pos.x] = ' ';
    }
}

void collision_handler(CharPacman *pacman, CharGhost *ghost, GhostInfo* ghost_info)
{
    if(pacman->e.p.x == ghost->e.p.x && pacman->e.p.y == ghost->e.p.y)
    {
        if(ghost->mode == M_FRIGHT)
        {
            ghost->mode = M_DEAD;
            ghost_info->death = true;
            ghost_info->new = true;
            // morto il fantasma
        }
        else if(ghost->mode != M_DEAD)
        {
            //perdi una vita
            pacman->lives--;
            ghost_info->full = true;
            ghost_info->new = true;
            //pacman viene riportato alla pos. inziiale idem
        }
    }
}