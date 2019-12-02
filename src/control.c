#include <unistd.h>
#include <stdlib.h>
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
void manage_pacman_in(int, int*, int*, Characters, GhostInfo*, char[MAP_HEIGHT][MAP_WIDTH], int*, TempText*, ControlTimers*);
void manage_ghost_in(int, Characters*, char[MAP_HEIGHT][MAP_WIDTH]);

void manage_timers(TempText, ControlTimers*, Characters, char[MAP_HEIGHT][MAP_WIDTH], int*);

void send_ghost_info(int, GhostInfo*);
void send_pacman_info(int, PacManInfo*);

void collision_handler(Characters*, PacManInfo*, GhostInfo*, int*, TempText*, char[MAP_HEIGHT][MAP_WIDTH], int*);
void food_handler(int*, int*, int*, Characters, char[MAP_HEIGHT][MAP_WIDTH], GhostInfo*, TempText*, ControlTimers*);
void food_setup();
void eat_pause(Characters, PacManInfo*, GhostInfo*, TempText*, char[MAP_HEIGHT][MAP_WIDTH], int);

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
        mvprintw(MAP_HEIGHT+GUI_HEIGHT-(2+i), 60, "%*s",50," ");
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

    Characters characters = {&pacman, &ghost};
    characters.ghosts = malloc(sizeof(CharGhost)*4);
    
    TempText temp_text = {};
    ControlTimers timers = {};

    int score = 0;
    char game_food[MAP_HEIGHT][MAP_WIDTH];
    int eaten_dots = 0;
    int ghost_streak = 0;

    food_setup(game_food);

    MessageList log_list;
    list_init(&log_list);

    while(1)
    {  
        manage_cmd_in(cmd_in, p_cmd_out, &ghost_info, &pacman_info);
        manage_pacman_in(pacman_in, &eaten_dots, &ghost_streak, characters, &ghost_info, game_food, &score, &temp_text, &timers);
        manage_ghost_in(ghost_in, &characters, game_food);
        manage_timers(temp_text, &timers, characters, game_food, &ghost_streak);
        collision_handler(&characters, &pacman_info, &ghost_info, &score, &temp_text, game_food, &ghost_streak);
        send_pacman_info(pacman_out, &pacman_info);
        send_ghost_info(ghost_out, &ghost_info);
        print_ui(score, characters);
        print_temp_text(temp_text);

        manage_logs(log_in, &log_list);
        if(eaten_dots == 240)
        {
            mvprintw(23, 18, "GAME OVER! VITTORIA");
            refresh();
            return;
        }
        if(pacman.lives < 0){
            attron(COLOR_REDTEXT);
            mvprintw(23, 18, "G A M E   O V E R !");
            attroff(COLOR_REDTEXT);
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

void manage_pacman_in(int pacman_in, int* eaten_dots, int* ghost_streak, Characters characters, GhostInfo* info_pkg, char game_food[MAP_HEIGHT][MAP_WIDTH], int* score, TempText* temp_text, ControlTimers* timers)
{
    CharPacman pacman_pkg;

    while(read(pacman_in, &pacman_pkg, sizeof(pacman_pkg)) != -1)
    {
        unprint_area(characters.pacman->e.p.y, characters.pacman->e.p.x-1, 3, game_food);
        *characters.pacman = pacman_pkg;
        food_handler(score, eaten_dots, ghost_streak, characters, game_food, info_pkg, temp_text, timers);
        
        info_pkg->pacman = characters.pacman->e;
        info_pkg->new = true;
    }
}

void manage_ghost_in(int ghost_in, Characters* characters, char game_food[MAP_HEIGHT][MAP_WIDTH])
{
    CharGhost ghost_pkg;

    while(read(ghost_in, &ghost_pkg, sizeof(ghost_pkg)) != -1)
    {
        unprint_area(characters->ghosts[ghost_pkg.ghost_id].e.p.y, characters->ghosts[ghost_pkg.ghost_id].e.p.x-1, 3, game_food);
        characters->ghosts[ghost_pkg.ghost_id] = ghost_pkg;

        if(ghost_pkg.ghost_id+1 > characters->num_ghosts)
            characters->num_ghosts = ghost_pkg.ghost_id+1;
    }
}

void manage_timers(TempText temp_text, ControlTimers* timers, Characters characters, char game_food[MAP_HEIGHT][MAP_WIDTH], int* ghost_streak)
{
    if(temp_text.timer != 0)
        if(!check_timer(temp_text.timer))
        {
            sunprint_area(temp_text.p.y, temp_text.p.x, strlen(temp_text.text), game_food, characters);
            temp_text.timer = 0;
        }
    if(timers->fruit_timer != 0)
        if(!check_timer(timers->fruit_timer))
        {
            game_food[FRUIT_POS_Y][FRUIT_POS_X-1] = ' ';
            game_food[FRUIT_POS_Y][FRUIT_POS_X] = ' ';
            game_food[FRUIT_POS_Y][FRUIT_POS_X+1] = ' ';
            unprint_area(FRUIT_POS_Y, FRUIT_POS_X-1, 3, game_food);
            timers->fruit_timer = 0;
        }
    if(timers->fright_timer != 0)
        if(!check_timer(timers->fright_timer))
        {
            *ghost_streak = 0;
            timers->fright_timer = 0;;
        }
}

void send_ghost_info(int ghost_out, GhostInfo* ghost_info)
{    
    int i = 0;
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

void food_handler(int* score, int* eaten_dots, int* ghost_streak, Characters characters, char game_food[MAP_HEIGHT][MAP_WIDTH], GhostInfo *ghost_info, TempText *temp_text, ControlTimers* timers)
{  
    int i,j ;
    Position pe_pos;

    for(i=-1; i<=1; i++)
    {
        pe_pos.x = characters.pacman->e.p.x+i;
        pe_pos.y = characters.pacman->e.p.y;
        pe_pos = get_pac_eff_pos(pe_pos);

        switch(game_food[pe_pos.y][pe_pos.x])
        {
            case '~':
                *score += 10;
                *eaten_dots +=1;
                game_food[pe_pos.y][pe_pos.x] = ' ';
                if(*eaten_dots == 70 || *eaten_dots == 170){
                    //spawna un frutto va tutto in funzione col controllo
                    create_fruit(game_food, timers);
                }
                beep();
                break;
            case '`': 
                *score += 50;
                *ghost_streak = 0;
                ghost_info->fright = true;
                ghost_info->new = true;    
                game_food[pe_pos.y][pe_pos.x] = ' ';
                timers->fright_timer = start_timer(6);
                break;
            case '^':
                *score += 100; //*lvl?

                sunprint_area(temp_text->p.y, temp_text->p.x, strlen(temp_text->text), game_food, characters);
                create_temp_text(temp_text, pe_pos.x-1, pe_pos.y+GUI_HEIGHT, "200", 2e3, 13);
                for(j=-1; j<=1; j++)
                    game_food[pe_pos.y][pe_pos.x+j] = ' '; 
                
                mvaddch(pe_pos.y+GUI_HEIGHT, pe_pos.x+i, ' ');
                refresh();
                break;
        }

    }
}

void collision_handler(Characters* characters, PacManInfo *pacman_info, GhostInfo* ghost_info, int* score, TempText* temp_text, char game_food[MAP_HEIGHT][MAP_WIDTH], int* ghost_streak)
{
    int i;
    CharGhost* ghost;

    for(i = 0; i < characters->num_ghosts; i++)
    {
        ghost = &characters->ghosts[i];

        if(characters->pacman->e.p.x == ghost->e.p.x && characters->pacman->e.p.y == ghost->e.p.y)
        {
            if(ghost->mode == M_FRIGHT)
            {
                (*ghost_streak)++;
                *score += pow(2,(*ghost_streak))*100;    //200 400 800 1600
                ghost->mode = M_DEAD;
                ghost_info->death = i;
                ghost_info->new = true;

                eat_pause(*characters, pacman_info, ghost_info, temp_text, game_food, pow(2,(*ghost_streak))*100);
                // morto il fantasma
            }
            else if(ghost->mode != M_DEAD && !characters->pacman->dead)
            {
                //perdi una vita
                characters->pacman->dead = true;
                pacman_info->death = true;
                pacman_info->new = true;
                ghost_info->restart = true;
                ghost_info->new = true;
                //pacman viene riportato alla pos. inziiale idem
            }
        }
    }
}

void eat_pause(Characters characters, PacManInfo* pacman_info, GhostInfo* ghost_info, TempText* temp_text, char game_food[MAP_HEIGHT][MAP_WIDTH], int points)
{   
    char points_string[6] = {};     
    sprintf(points_string, "%d", points);
    sunprint_area(temp_text->p.y, temp_text->p.x, strlen(temp_text->text), game_food, characters);
    create_temp_text(temp_text, characters.pacman->e.p.x-1, characters.pacman->e.p.y+GUI_HEIGHT, points_string, 0.6e3, 12);
    ghost_info->sleeptime = 0.6e6;    
    ghost_info->new = true;
    pacman_info->sleeptime = 0.6e6;    
    pacman_info->new = true;
}

void create_fruit(char game_food[MAP_HEIGHT][MAP_WIDTH], ControlTimers* timers)
{
    attron(COLOR_REDTEXT);
    mvaddch(23, FRUIT_POS_X-1, S_FRUIT[0][0]);
    mvaddch(17+GUI_HEIGHT, FRUIT_POS_X+1, S_FRUIT[0][2]);
    attroff(COLOR_REDTEXT);
    attron(COLOR_GREENTEXT);
    mvaddch(23, FRUIT_POS_X, S_FRUIT[0][1]);
    attroff(COLOR_GREENTEXT);
    game_food[FRUIT_POS_Y][FRUIT_POS_X-1] = S_FRUIT[0][0];
    game_food[FRUIT_POS_Y][FRUIT_POS_X] = S_FRUIT[0][1];
    game_food[FRUIT_POS_Y][FRUIT_POS_X+1] = S_FRUIT[0][2];

    timers->fruit_timer = start_timer(10e3);
}