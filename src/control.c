#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <math.h>
#include <string.h>

#include "control.h"
#include "utils.h"
#include "pacman.h"
#include "ghost.h"
#include "entity.h"
#include "bullet.h"
#include "list.h"
#include "menu.h"
#include "interface.h"

void init_control_data(ControlData*, ControlPipes*, Options);

void manage_cmd_in(ControlData*);
void manage_pacman_in(ControlData*);
void manage_ghost_in(ControlData*);

void manage_timers(ControlData*);

void send_ghost_info(ControlData*);
void send_pacman_info(ControlData*);

void collision_handler(ControlData*);
void food_handler(ControlData*);
void food_setup(char[MAP_HEIGHT][MAP_WIDTH]);
void eat_pause(ControlData*, int);
void create_fruit(ControlData*);

void init_control_data(ControlData* cd, ControlPipes* pipes, Options options)
{
    cd->characters.pacman = init_pacman_char(options);
    cd->pacman_info = init_pacman_info();
    cd->ghost_info = init_ghost_info();
    cd->options = options;
    cd->characters.bullets.head = NULL;
    cd->characters.bullets.tail = NULL;
    cd->characters.bullets.count = 0;
    cd->characters.ghosts = malloc(sizeof(CharGhost)*options.num_ghosts);
    
    cd->temp_text.timer = 0;
    cd->timers.fright_timer = 0;
    cd->timers.fruit_timer = 0;
    cd->pipes = pipes;

    cd->score = 0;
    cd->eaten_dots = 0;
    cd->ghost_streak = 0;
    
    food_setup(cd->game_food);
}

void manage_logs(int log_in, MessageList* log_list)
{
    static int p=0;
    int i;
    char msg_pkg[50];

    while(read(log_in, &msg_pkg, sizeof(char)*50) != -1)
    {
        m_list_push(log_list, msg_pkg);
    }    

    while(log_list->count > MAP_HEIGHT-2)
        m_list_pop(log_list);

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

void control_main(ControlPipes pipes, Options options)
{
    ControlData cd; 

    init_control_data(&cd, &pipes, options);
    
    MessageList log_list;
    m_list_init(&log_list);
    b_list_init(&cd.characters.bullets);


    while(1)
    {  
        manage_cmd_in(&cd);
        manage_pacman_in(&cd);/* &eaten_dots, &ghost_streak, characters, &ghost_info, game_food, &score, &temp_text, &timers); */
        manage_ghost_in(&cd);
        manage_bullet_in(&cd);
        manage_timers(&cd);
        collision_handler(&cd);
        send_pacman_info(&cd);
        send_ghost_info(&cd);
        print_ui(&cd);
        mvprintw(0,60, "%d          ", cd.characters.pacman.armor);
        refresh();

        manage_logs(pipes.log_in, &log_list);
        if(cd.eaten_dots == 240)
        {
            mvprintw(23, 18, "GAME OVER! VITTORIA");
            refresh();
            return;
        }
        if(cd.characters.pacman.lives < 0){
            attron(COLOR_REDTEXT);
            mvprintw(23, 18, "G A M E   O V E R !");
            attroff(COLOR_REDTEXT);
            refresh();
            return;
        }
    }
}

void food_setup(char food[MAP_HEIGHT][MAP_WIDTH])
{
    int i;
    for (i = 0; i < MAP_HEIGHT; i++)
    {
        strcpy(food[i], PELLETS[i]);
    } 
}

void manage_cmd_in(ControlData* cd)
{
    _Bool new_pkg = false;
    char c_in;

    while(read(cd->pipes->cmd_in, &c_in, sizeof(c_in)) != -1)
    {
        switch(c_in)
        {
            case K_UP:
                new_pkg = true;
                cd->pacman_info.direction = UP;
                break;
            case K_LEFT:
                new_pkg = true;
                cd->pacman_info.direction = LEFT;
                break;
            case K_DOWN:
                new_pkg = true;
                cd->pacman_info.direction = DOWN;
                break;
            case K_RIGHT:
                new_pkg = true;
                cd->pacman_info.direction = RIGHT;
                break;
            case ' ':
                if(cd->options.options_shoot.enabled)
                {
                    new_pkg = true;
                    cd->pacman_info.shoot = true;
                }
                break;
            /*
            case 'p':
            //case k_ESC:
                cd->ghost_info.pause = true;
                cd->ghost_info.new = true;
                cd->pacman_info.pause = true;
                cd->pacman_info.new = true; 
                send_ghost_info(cd);
                send_pacman_info(cd);
                pause_menu(cd);
                cd->ghost_info.resume = true;
                cd->ghost_info.new = true;
                cd->pacman_info.resume = true;
                cd->pacman_info.new = true;
                break;
            */
        }
        if(new_pkg)
        {
            cd->ghost_info.resume = true;
            cd->ghost_info.new = true;
            cd->pacman_info.resume = true;
            cd->pacman_info.new = true;
            new_pkg = false;
        }
    }
}

void manage_pacman_in(ControlData* cd)
{
    CharPacman pacman_pkg;

    while(read(cd->pipes->pacman_in, &pacman_pkg, sizeof(pacman_pkg)) != -1)
    {
        cd->characters.pacman = pacman_pkg;
        food_handler(cd);
        
        cd->ghost_info.pacman = cd->characters.pacman.e;
        cd->ghost_info.new = true;

        if(pacman_pkg.dead)
        {
            reset_game(cd);
        }
    }
}

void manage_ghost_in(ControlData* cd)
{
    CharGhost ghost_pkg;

    while(read(cd->pipes->ghost_in, &ghost_pkg, sizeof(ghost_pkg)) != -1)
    {
        cd->characters.ghosts[ghost_pkg.ghost_id] = ghost_pkg;

        if(ghost_pkg.ghost_id+1 > cd->characters.num_ghosts)
            cd->characters.num_ghosts = ghost_pkg.ghost_id+1;
    }
}

void manage_bullet_in(ControlData* cd)
{
    Bullet bullet_pkg;

    while(read(cd->pipes->bullet_in, &bullet_pkg, sizeof(bullet_pkg)) != -1)
    {
        if(!bullet_pkg.dead)
        {
            b_list_update(&cd->characters.bullets, bullet_pkg);
        }
        else
        {
            b_list_remove(&cd->characters.bullets, b_list_search(cd->characters.bullets, bullet_pkg));
        }
    }
}

void manage_timers(ControlData* cd)
{
    if(cd->temp_text.timer != 0)
        if(!check_timer(cd->temp_text.timer))
        {
            cd->temp_text.timer = 0;
        }
    if(cd->timers.fruit_timer != 0)
        if(!check_timer(cd->timers.fruit_timer))
        {
            cd->game_food[FRUIT_POS_Y][FRUIT_POS_X-1] = ' ';
            cd->game_food[FRUIT_POS_Y][FRUIT_POS_X] = ' ';
            cd->game_food[FRUIT_POS_Y][FRUIT_POS_X+1] = ' ';
            cd->timers.fruit_timer = 0;
        }
    if(cd->timers.fright_timer != 0)
        if(!check_timer(cd->timers.fright_timer))
        {
            cd->ghost_streak = 0;
            cd->timers.fright_timer = 0;;
        }
}

void send_ghost_info(ControlData* cd)
{    
    int i = 0;
    if(cd->ghost_info.new)
    {
        write(cd->pipes->ghost_out, &cd->ghost_info, sizeof(cd->ghost_info));
        cd->ghost_info = init_ghost_info();
    }
}

void send_pacman_info(ControlData* cd)
{    
    if(cd->pacman_info.new)
    {
        write(cd->pipes->pacman_out, &cd->pacman_info, sizeof(cd->pacman_info));
        cd->pacman_info = init_pacman_info();
    }
}

void food_handler(ControlData* cd)
{  
    int i,j ;
    Position pe_pos;

    for(i=-1; i<=1; i++)
    {
        pe_pos.x = cd->characters.pacman.e.p.x+i;
        pe_pos.y = cd->characters.pacman.e.p.y;
        pe_pos = get_pac_eff_pos(pe_pos);

        switch(cd->game_food[pe_pos.y][pe_pos.x])
        {
            case '~':
                cd->score += 10;
                cd->eaten_dots +=1;
                cd->game_food[pe_pos.y][pe_pos.x] = ' ';
                if(cd->eaten_dots == 70 || cd->eaten_dots == 170){
                    //spawna un frutto va tutto in funzione col controllo
                    create_fruit(cd);
                }
                beep();
                break;
            case '`': 
                cd->score += 50;
                cd->ghost_streak = 0;
                cd->ghost_info.fright = true;
                cd->ghost_info.new = true;    
                cd->game_food[pe_pos.y][pe_pos.x] = ' ';
                cd->timers.fright_timer = start_timer(6);
                break;
            case '^':
                cd->score += 100; //*lvl?

                create_temp_text(&cd->temp_text, pe_pos.x-1, pe_pos.y+GUI_HEIGHT, "200", 2e3, 13);
                for(j=-1; j<=1; j++)
                    cd->game_food[pe_pos.y][pe_pos.x+j] = ' '; 
                
                mvaddch(pe_pos.y+GUI_HEIGHT, pe_pos.x+i, ' ');
                refresh();
                break;
        }

    }
}

void collision_handler(ControlData* cd)
{
    _Bool flag = false;
    int i, j;
    CharGhost *ghost;

    for(i = 0; i < cd->characters.num_ghosts; i++)
    {
        ghost = &cd->characters.ghosts[i];

        if(cd->characters.pacman.e.p.x == ghost->e.p.x && cd->characters.pacman.e.p.y == ghost->e.p.y)
        {
            if(ghost->mode == M_FRIGHT)
            {
                kill_ghost(cd, i);

                eat_pause(cd, pow(2,(cd->ghost_streak))*100);
            }
            else if(ghost->mode != M_DEAD && !cd->characters.pacman.dead)
            {
                //cd->pacman_info.new = true;
                //cd->pacman_info.collide = true;
                //cd->characters.pacman.dead = true;
            }
        }
    }
    
    BulletNode *aux = cd->characters.bullets.head, *aux2 = cd->characters.bullets.head;
    while(aux != NULL)
    {
        if(!aux->bullet.dead)
        {
            if(!aux->bullet.enemy)
            {
                for(j = 0; j < cd->characters.num_ghosts; j++)
                {
                    if(cd->characters.ghosts[j].mode != M_DEAD && aux->bullet.p.x == cd->characters.ghosts[j].e.p.x && aux->bullet.p.y == cd->characters.ghosts[j].e.p.y)
                    {
                        kill_bullet(cd, aux);
                        kill_ghost(cd, j);
                        break;
                    }
                }
                while(aux2 != NULL) //Distruzione spari quando collidono di pacman e ghost
                {
                    if(aux2->bullet.enemy && aux->bullet.p.x == aux2->bullet.p.x && aux->bullet.p.y == aux2->bullet.p.y)
                    {
                        kill_bullet(cd, aux);
                        kill_bullet(cd, aux2);
                        break;
                    }

                    aux2 = aux2->next;
                }
            }
            else
            {
                if(aux->bullet.p.x == cd->characters.pacman.e.p.x && aux->bullet.p.y == cd->characters.pacman.e.p.y)
                {               
                    cd->pacman_info.new = true;
                    cd->pacman_info.hit = true;
                    kill_bullet(cd, aux);
                }
            }
        }
        aux = aux->next;
    }
}

void reset_game(ControlData* cd)
{
    BulletNode *aux = cd->characters.bullets.head, *aux2;

    while(aux != NULL)
    {
        kill_bullet(cd, aux);
        aux2 = aux;
        aux = aux->next;
    }

    cd->ghost_info.restart = true;
    cd->ghost_info.new = true;
    cd->pacman_info.reset = true;
    cd->pacman_info.new = true;
}

void kill_ghost(ControlData* cd, int i)
{
    cd->ghost_streak++;
    cd->score += pow(2,cd->ghost_streak)*100;    //200 400 800 1600
    cd->characters.ghosts[i].mode = M_DEAD;
    cd->ghost_info.death = i;
    cd->ghost_info.new = true;
}
void kill_bullet(ControlData* cd, BulletNode* aux)
{
    BulletInfo bullet_info;
    
    bullet_info.create_bullet = false;
    bullet_info.destroy_bullet = true;
    bullet_info.destroy_id = aux->bullet.id;
    aux->bullet.dead = true;

    write(cd->pipes->bullet_out, &bullet_info, sizeof(bullet_info));
}

void eat_pause(ControlData* cd, int points)
{   
    char points_string[6] = {};     
    sprintf(points_string, "%d", points);
    create_temp_text(&cd->temp_text, cd->characters.pacman.e.p.x-1, cd->characters.pacman.e.p.y+GUI_HEIGHT, points_string, 0.6e3, 12);
    cd->ghost_info.sleeptime = 0.6e6;    
    cd->ghost_info.new = true;
    cd->pacman_info.sleeptime = 0.6e6;    
    cd->pacman_info.new = true;
}

void create_fruit(ControlData* cd)
{
    attron(COLOR_REDTEXT);
    mvaddch(23, FRUIT_POS_X-1, S_FRUIT[0][0]);
    mvaddch(17+GUI_HEIGHT, FRUIT_POS_X+1, S_FRUIT[0][2]);
    attroff(COLOR_REDTEXT);
    attron(COLOR_GREENTEXT);
    mvaddch(23, FRUIT_POS_X, S_FRUIT[0][1]);
    attroff(COLOR_GREENTEXT);
    cd->game_food[FRUIT_POS_Y][FRUIT_POS_X-1] = S_FRUIT[0][0];
    cd->game_food[FRUIT_POS_Y][FRUIT_POS_X] = S_FRUIT[0][1];
    cd->game_food[FRUIT_POS_Y][FRUIT_POS_X+1] = S_FRUIT[0][2];

    cd->timers.fruit_timer = start_timer(10e3);
}