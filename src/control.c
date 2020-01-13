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
void food_setup(char[MAP_HEIGHT][MAP_WIDTH+1]);
void eat_pause(ControlData*, int);
void create_fruit(ControlData*);

void init_control_data(ControlData* cd, ControlPipes* pipes, Options options)
{
    int i;

    cd->characters.pacman = init_pacman_char(options);
    cd->pacman_info = init_pacman_info();
    cd->ghost_info = init_ghost_info();
    cd->options = options;
    cd->characters.bullets.head = NULL;
    cd->characters.bullets.tail = NULL;
    cd->characters.bullets.count = 0;
    cd->characters.ghosts = malloc(sizeof(CharGhost)*options.num_ghosts);
    
    for(i=0; i < options.num_ghosts; i++)
    {
        cd->characters.ghosts[i].mode = M_INACTIVE;
    }

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


/**
 * Funzione di gioco principale dove vengono gestisti i dati
 * e utilizzati da altre funzioni.
 * 
 * @param pipes Pipe utilizzate per la comunicazione tra i vari processi
 * @param options Opzioni della partita con cui stabilisco la modalità di gioco
 */
void control_main(ControlPipes pipes, Options options)
{
    ControlData cd; 
    MessageList log_list;
    WINDOW *win_map = newwin(MAP_HEIGHT, MAP_WIDTH, GUI_HEIGHT, 0);

    init_control_data(&cd, &pipes, options);
    
    m_list_init(&log_list);
    b_list_init(&cd.characters.bullets);

    while(1)
    {  
        //Funzioni di gestione dati in arrivo a control ed elaborazione
        manage_cmd_in(&cd);
        manage_pacman_in(&cd);
        manage_ghost_in(&cd);
        manage_bullet_in(&cd);
        manage_timers(&cd);
        collision_handler(&cd);
        //Funzioni di invio dati alle entità
        send_pacman_info(&cd);
        send_ghost_info(&cd);
        //Stampa scena
        print_ui(win_map, &cd);

        manage_logs(pipes.log_in, &log_list);
        if(cd.eaten_dots == 240)
        {
            mvprintw(23, 18, "GAME OVER! VITTORIA");
            refresh();
            break;
        }
        if(cd.characters.pacman.lives < 0){
            attron(COLOR_REDTEXT);
            mvprintw(23, 18, "G A M E   O V E R !");
            attroff(COLOR_REDTEXT);
            refresh();
            break;
        }
    }
    delwin(win_map);
}
/**
 * Inserisce il cibo nella mappa attiva di gioco
 */
void food_setup(char food[MAP_HEIGHT][MAP_WIDTH+1])
{
    int i;
    for (i = 0; i < MAP_HEIGHT; i++)
    {
        strcpy(food[i], PELLETS[i]);
    } 
}

/**
 * Gestisce gli input da tastiera
 * 
 * @param cd dati di controllo della partita
 */
void manage_cmd_in(ControlData* cd)
{
    _Bool new_pkg = false;
    int c_in;

    while(read(cd->pipes->cmd_in, &c_in, sizeof(c_in)) != -1)
    {
        switch(c_in)
        {
            case KEY_UP:
                new_pkg = true;
                cd->pacman_info.direction = UP;
                break;
            case KEY_LEFT:
                new_pkg = true;
                cd->pacman_info.direction = LEFT;
                break;
            case KEY_DOWN:
                new_pkg = true;
                cd->pacman_info.direction = DOWN;
                break;
            case KEY_RIGHT:
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
        cd->characters.ghosts[ghost_pkg.e.id] = ghost_pkg;
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
    //Timer per il testo temporaneo allo scadere viene rimosso
    if(cd->temp_text.timer != 0)
        if(!check_timer(cd->temp_text.timer))
        {
            cd->temp_text.timer = 0;
        }
    //Tiimer per lo spawn della frutta allo scadere viene rimossa
    if(cd->timers.fruit_timer != 0)
        if(!check_timer(cd->timers.fruit_timer))
        {
            cd->game_food[cd->fruit.y][cd->fruit.x-1] = ' ';
            cd->game_food[cd->fruit.y][cd->fruit.x] = ' ';
            cd->game_food[cd->fruit.y][cd->fruit.x+1] = ' ';
            cd->timers.fruit_timer = 0;
        }
    //Tiimer per fantasmi spaventati allo scadere tornano normali
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
    if(cd->ghost_info.new)  //Se vi è un nuovo pacchetto per un ghost viene inviato
    {
        write(cd->pipes->ghost_out, &cd->ghost_info, sizeof(cd->ghost_info));
        cd->ghost_info = init_ghost_info();
    }
}

void send_pacman_info(ControlData* cd)
{       
    if(cd->pacman_info.new) //Se vi è un nuovo pacchetto per pacman viene inviato
    {
        write(cd->pipes->pacman_out, &cd->pacman_info, sizeof(cd->pacman_info));
        cd->pacman_info = init_pacman_info();
    }
}

/**
 * Gestore del cibo e punteggi, quando pacman passa sopra un pallino
 * questo viene considerato come mangiato e vengono aggiunti dei punti,
 * inotre dopo un prestabilito numero di pallini viene spawnato un frutto
 * 
 * @param cd dati di controllo della partita
 */
void food_handler(ControlData* cd)
{  
    int i,j ;
    Position pe_pos;

    for(i=-1; i<=1; i++) //Per tutta la hitbox di pacaman 
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
                    //Spawna un frutto va tutto in funzione col controllo
                    create_fruit(cd);
                }
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
                cd->score += 200;

                create_temp_text(&cd->temp_text, pe_pos.x-1, pe_pos.y, "200", 2e3, 13);
                for(j=-1; j<=1; j++)
                    cd->game_food[pe_pos.y][pe_pos.x+j] = ' '; 
                
                break;
        }

    }
}

/**
 * Gestore delle collisioni fra entità
 * 
 * @param cd dati di controllo della partita
 */
void collision_handler(ControlData* cd)
{
    _Bool flag = false;
    int i;
    CharGhost *ghost;

    for(i = 0; i < cd->options.num_ghosts; i++)
    {       
        ghost = &cd->characters.ghosts[i];

        if(ghost->mode != M_INACTIVE)
        {
            if(cd->characters.pacman.e.p.x == ghost->e.p.x && cd->characters.pacman.e.p.y == ghost->e.p.y)
            {
                if(ghost->mode == M_FRIGHT)
                {
                    cd->ghost_streak++;
                    cd->score += pow(2,cd->ghost_streak)*100;
                    kill_ghost(cd, i);

                    eat_pause(cd, pow(2,(cd->ghost_streak))*100);
                }
                else if((ghost->mode == M_CHASE || ghost->mode == M_FRIGHT) && !cd->characters.pacman.dead)
                {
                    cd->pacman_info.new = true;
                    cd->pacman_info.collide = true;
                    cd->characters.pacman.dead = true;
                }
            }
        }
    }
    
    BulletNode *aux = cd->characters.bullets.head, *aux2 = cd->characters.bullets.head;
    while(aux != NULL) //Ciclo la lista degli spari 
    {
        if(!aux->bullet.dead)
        {
            if(!aux->bullet.enemy)
            {
                for(i = 0; i < cd->options.num_ghosts; i++)
                {
                    if(cd->characters.ghosts[i].mode != M_DEAD && cd->characters.ghosts[i].mode != M_INACTIVE && cd->characters.ghosts[i].mode != M_IDLE 
                    && aux->bullet.p.x == cd->characters.ghosts[i].e.p.x && aux->bullet.p.y == cd->characters.ghosts[i].e.p.y)
                    {
                        kill_bullet(cd, aux);
                        kill_ghost(cd, i);
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
    create_temp_text(&cd->temp_text, cd->characters.pacman.e.p.x-1, cd->characters.pacman.e.p.y, points_string, 0.6e3, 12);
    cd->ghost_info.sleeptime = 0.6e6;    
    cd->ghost_info.new = true;
    cd->pacman_info.sleeptime = 0.6e6;    
    cd->pacman_info.new = true;
}

void create_fruit(ControlData* cd)
{
    char x_matrix[MAP_HEIGHT][MAP_WIDTH+1];
    int n_pos;

    if(cd->options.options_fruit.fixed)
        cd->fruit = cd->options.options_fruit.pos;
    else
    {
        diff_matrix(MAP_HEIGHT, MAP_WIDTH, PELLETS, cd->game_food, x_matrix);
        n_pos = count_mat_occ(MAP_HEIGHT, MAP_WIDTH, x_matrix, 'X');
        cd->fruit = get_i_ch_pos(MAP_HEIGHT, MAP_WIDTH, x_matrix, 'X', rand_between(0, n_pos));
    }
    
    cd->game_food[cd->fruit.y][cd->fruit.x-1] = S_FRUIT[0][0];
    cd->game_food[cd->fruit.y][cd->fruit.x] = S_FRUIT[0][1];
    cd->game_food[cd->fruit.y][cd->fruit.x+1] = S_FRUIT[0][2];

    cd->timers.fruit_timer = start_timer(10e3);
}

