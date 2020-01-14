#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include "ghost.h"
#include "entity.h"
#include "utils.h"
#include "interface.h"
#include "ai.h"

void manage_g_info_in(int, GhostShared*);
void manage_g_timers(GhostShared*, CharGhost*);
void ghost_choose_dir(CharGhost*, GhostShared*);
void manage_position_events(GhostShared*, CharGhost*);
void ghost_wait(CharGhost, GhostShared*);
void ghost_move(CharGhost*, char[MAP_HEIGHT][MAP_WIDTH+1]);
_Bool is_empty_space_ghost(char); 
void* ghost_thread(void*);
void set_ghost_start(GhostShared*, CharGhost*);
void init_ghost_map(GhostShared*);
void check_ghost_spawn(GhostParameters*);
void manage_respawn(GhostParameters*);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void init_timers(GhostShared* ghost_shared, int id)
{
    CharGhost* ghost = &ghost_shared->ghosts[id];

    ghost->timers.fright = 0;
    ghost->timers.respawn = 0;
    ghost->timers.shoot = 0;
    ghost->timers.load = 0;
}

void init_ghost_char(GhostShared *ghost_shared, int id)
{
    CharGhost* ghost = &ghost_shared->ghosts[id];
    ghost->e.id = id;
    ghost->type = id%4;

    set_ghost_start(ghost_shared, ghost);

    init_timers(ghost_shared, id);

    if(ghost_shared->options.options_shoot.enabled)
        ghost->timers.shoot = start_timer(1e3);

    if(ghost_shared->options.options_spawn.random && ghost->mode != M_RESPAWN)
    {
        ghost->mode = M_IDLE;      
        ghost->timers.load = start_timer(rand_between(1,3)*1e3);
    }
    else
    {
        ghost->mode = M_CHASE;
    }
}

/**
 * Imposta la posizione dove deve apparire un fantasma
 * 
 * @param ghost_shared dati condivisi tra i thread dei fantasmi 
 * @param ghost fantasma preso in considerazione
 */
void set_ghost_start(GhostShared *ghost_shared, CharGhost *ghost)
{
    if(ghost->mode == M_RESPAWN) //se il fantasma deve respawnare va nella monsterpit
    {
        ghost->e.p = HOME_POSITION;
    }
    else if(ghost_shared->options.options_spawn.random) //posizione iniziale per spawnare random
    {
        ghost->e.p = ghost_shared->starting_pos[ghost->e.id];
    }
    else //posizione iniziale fantasma dentro la monster pit
    {
        ghost->e.p = GHOST_START_POS[ghost->type];
    }
    
    ghost->e.dir = UP;
}

GhostInfo init_ghost_info()
{
    GhostInfo info;
    info.pacman.id = PACMAN_ID;
    info.pacman.dir = PACMAN_START_DIR;
    info.pacman.p.x = PACMAN_START_X;
    info.pacman.p.y = PACMAN_START_Y;
    info.new = false;
    info.fright = false;
    info.restart = false;
    info.pause = false;
    info.resume = false;
    info.death = -1;
    info.sleeptime = 0;
    return info;
}

/**
 * Funzione principale del processo dei fantasmi dove arrivano i dati e vengono gestiti
 * 
 * @param options opzioni di gioco
 * @param info_in informazioni/eventi in arrivo da control
 * @param pos_out pipe per inviare posizione a control
 * @param bullet_out pipe per inviare al processo bullet posizione e nuovo poriettile
 * @param log_out pipe per i log
 */
void ghost_main(Options options, int info_in, int pos_out, int bullet_out, int log_out)
{
    int i;
    pthread_t ghost;
    GhostInfo info_pkg = init_ghost_info();
    GhostShared ghost_shared = {};
    GhostParameters *ghost_parameters = malloc(sizeof(GhostParameters)*options.num_ghosts);

    ghost_shared.options = options;
    ghost_shared.num_ghosts = options.num_ghosts;
    ghost_shared.pacman = init_pacman_char(options).e;
    ghost_shared.paused = true;
    ghost_shared.mode = M_CHASE;
    ghost_shared.pos_out = pos_out;
    ghost_shared.bullet_out = bullet_out;
    ghost_shared.log_out = log_out;
    ghost_shared.ghosts = malloc(sizeof(CharGhost)*options.num_ghosts); //alloca puntatori
    ghost_shared.starting_pos = malloc(sizeof(Position)*options.num_ghosts); //alloca puntatori
    
    init_ghost_map(&ghost_shared); //Mette i fantasmi nella mappa 
    
    //Imposto i fantasmi a inattivi inizializzo valori
    for(i = 0; i < options.num_ghosts; i++)
    {
        init_timers(&ghost_shared, i);
        ghost_shared.ghosts[i].mode = M_INACTIVE;

        ghost_parameters[i].ghost_shared = &ghost_shared;
        ghost_parameters[i].id = i;
    }     
    //Se i fantasmi non spawnano random creo subito i loro threads
    if(!options.options_spawn.random)
        for(i = 0; i < options.num_ghosts; i++)
            pthread_create(&ghost, NULL, &ghost_thread, &ghost_parameters[i]);    

    while(1)
    {
        manage_g_info_in(info_in, &ghost_shared);
        if(options.options_spawn.random) check_ghost_spawn(ghost_parameters);
        manage_respawn(ghost_parameters);
    }
}

/**
 * Gestisce il respawn dei fantasmi implementato tramite creazione 
 * di un nuovo thread dopo un tempo prestabilito
 * 
 * @param ghost_parameters Array di ghost e id del ghost
 */
void manage_respawn(GhostParameters* ghost_parameters)
{
    GhostShared* ghost_shared = ghost_parameters[0].ghost_shared;
    int i;
    pthread_t ghost;
    
    for(i=0; i<ghost_shared->num_ghosts; i++)
    {
        if(ghost_shared->ghosts[i].timers.respawn != 0 && ghost_shared->ghosts[i].mode == M_RESPAWN)
        {                
            if(!check_timer(ghost_shared->ghosts[i].timers.respawn))
            {
                ghost_shared->ghosts[i].timers.respawn = 0;
                pthread_create(&ghost, NULL, &ghost_thread, &ghost_parameters[i]); 
            }
        }        
    }
}

/**
 * Thread del ghost quando viene generato, gestisce i movimenti e la lgica del fantasma
 * 
 * @param parameters parametri passati al thread
 */
void* ghost_thread(void* parameters)
{
    _Bool end_flag = false;
    CharGhost* ghost;
    GhostShared* ghost_shared = ((GhostParameters*)parameters)->ghost_shared;
    int id = ((GhostParameters*)parameters)->id;

    int i = 0;  

    //Inizializzo il fantasma del thread
    pthread_mutex_lock(&mutex);
    init_ghost_char(ghost_shared, id); 
    ghost = &ghost_shared->ghosts[id];
    pthread_mutex_unlock(&mutex);

    do
    {       
        pthread_mutex_lock(&mutex);
        manage_g_timers(ghost_shared, ghost);
        ghost_choose_dir(ghost, ghost_shared); 
        //Il fantasma si muove
        if(!ghost_shared->paused && ghost->mode != M_IDLE) ghost_move(ghost, ghost_shared->options.map);
        manage_position_events(ghost_shared, ghost);
        end_flag = (ghost->mode == M_RESPAWN); //Flag per sapere se il thread è terminato o meno
        write(ghost_shared->pos_out, ghost, sizeof(*ghost)); //invia la posizione a control
        pthread_mutex_unlock(&mutex);
        ghost_wait(*ghost, ghost_shared);
    }
    while(!end_flag);
}

/**
 * Gestisco i dati in ingresso al fantasma in arrivo da control
 * 
 * @param info_in file descriptor pipe ingresso da control
 * @param ghost_shared dati condivisi tra i thread dei fantasmi 
 */
void manage_g_info_in(int info_in, GhostShared* ghost_shared)
{
    GhostInfo info;
    static int i=0;
    int k;
        
    pthread_mutex_lock(&mutex);
    //Legge le info da control 
    while(read(info_in, &info, sizeof(info)) != -1)
    {
        if(info.death != -1) //Se è diverso da -1 corrisonde all'id del fantasma morto
        {
            ghost_shared->ghosts[info.death].mode = M_DEAD;
        }
        if(info.fright) //Manda i fantasmi in fright quando pacman mangia un energizer
        {
            for(i=0; i < ghost_shared->num_ghosts; i++)
            {
                if((ghost_shared->ghosts[i].mode == M_SCATTER || ghost_shared->ghosts[i].mode == M_CHASE) && !is_in_pen(ghost_shared->ghosts[i]))
                {
                    ghost_shared->ghosts[i].timers.fright = start_timer(6e3);
                    ghost_shared->ghosts[i].mode = M_FRIGHT;
                    reverse_direction(&(ghost_shared->ghosts[i].e.dir));
                }
            } 
        }
        if(info.restart) //Riavvia i fantasmi
        {
            for(i=0; i < ghost_shared->num_ghosts; i++)
            {
                if(ghost_shared->ghosts[i].mode != M_INACTIVE)
                {
                    init_ghost_char(ghost_shared, i); //esetto il ghost
                    
                    if(ghost_shared->options.options_spawn.random)
                        ghost_shared->ghosts[i].e.p = HOME_POSITION;
                }
            } 
            ghost_shared->paused = true;
            ghost_shared->fright = false;
            ghost_shared->mode = M_CHASE;
        }
        if(info.pause)
        {
            ghost_shared->paused = true;
        }
        if(info.resume) //Riattiva i fantasmi quando viene premuto un tasto
        {
            if(ghost_shared->paused)
            {
                if(ghost_shared->options.options_spawn.random)
                {
                    for(i=0; i < ghost_shared->num_ghosts; i++)
                    {
                        //Se devono respawnare escono dopo qualche secondo dalla monsterpit
                        if(ghost_shared->ghosts[i].mode != M_INACTIVE)
                        {
                            ghost_shared->ghosts[i].mode = M_IDLE;
                            ghost_shared->ghosts[i].timers.load = start_timer(i*1e3);
                        }
                    } 
                }
                ghost_shared->paused = false;
            }
        }
        if(info.sleeptime > 0) //Tempo per cui devono stare fermi
        {
            ghost_shared->paused = true;
            usleep(info.sleeptime);
            ghost_shared->paused = false;
            info.sleeptime = 0;
        }
        ghost_shared->pacman = info.pacman;
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * Gestisce i timers dei fantasmi
 * 
 * @param ghost_shared dati condivisi tra i thread dei fantasmi
 * @param ghost fantasma preso in considerazione
 */
void manage_g_timers(GhostShared* ghost_shared, CharGhost* ghost)
{
    BulletInfo bullet_info = {};
    int i;

    if(ghost_shared->paused) 
        return;

    //Controllo se il frigh è finito o meno
    if(ghost->timers.fright != 0)
    {
        if(!check_timer(ghost->timers.fright))
        {
            if(ghost->mode != M_DEAD)
                ghost->mode = ghost_shared->mode;
            ghost->timers.fright = 0; 
        }
    }
    //Controllo se i fantasmi hanno finito di caricare
    if(ghost->timers.load != 0)
    {
        if(!check_timer(ghost->timers.load))
        {
            ghost->mode = M_CHASE;
            ghost->timers.load = 0;
        }
    }
    //Cooldown degli spari dei fantasmi se sono abilitati, 1s
    if(ghost->timers.shoot != 0 && ghost_shared->options.options_shoot.enabled)
    {
        //Il ghost può sparare solo fuori dalla monster pit e sesi trova in una posizione pari
        if(!is_in_pen(*ghost) && ghost->e.p.x % 2 == 0 && ghost->mode == M_CHASE && !check_timer(ghost->timers.shoot))
        {
            for(i = 0; i < 4; i++) //In quante direzioni sparare 
            {
                bullet_info.create_bullet = true;
                bullet_info.p = ghost->e.p;
                bullet_info.dir = i;
                bullet_info.enemy = true;

                switch(bullet_info.dir) //Controllo la direzione
                {
                    case UP:
                        bullet_info.p.y--;
                        break;
                    case LEFT:
                        bullet_info.p.x-=2;
                        break;
                    case DOWN:
                        bullet_info.p.y++;
                        break;
                    case RIGHT:
                        bullet_info.p.x+=2;
                        break;
                }       
                //Invio la generazione di uno sparo al processo proiettile
                write(ghost_shared->bullet_out, &bullet_info, sizeof(bullet_info));
            }
            ghost->timers.shoot = start_timer(1e3); //Resetto cooldown
        }
    }
}

/**
 * Funzione usata per stabilire il comportamento dell'IA in bse al tipo 
 * di fantasma o alla modalità
 * 
 * @param ghost_shared dati condivisi tra i thread dei fantasmi
 * @param ghost fantasma preso in considerazione
 */
void ghost_choose_dir(CharGhost* ghost, GhostShared* ghost_shared)
{
    switch(ghost->mode) //Controllo la modalità
    {
        case M_FRIGHT:
            ghost->e.dir = choose_direction_random(*ghost, ghost_shared->options.map);
            break;
        case M_DEAD:
            ghost->e.dir = choose_direction_target(*ghost, HOME_POSITION, ghost_shared->options.map);
            break;
        case M_SCATTER:
            ghost->e.dir = choose_direction_target(*ghost, SCATTER[ghost->type], ghost_shared->options.map);
            break;
        case M_CHASE:
            switch(ghost->type) //Se non vi sono modalità particolari controllo il tipo di fantasma
            {
                case BLINKY: 
                    ghost->e.dir = choose_direction_target(*ghost, blinky_target(ghost_shared->pacman), ghost_shared->options.map);
                    break;
                case PINKY:
                    ghost->e.dir = choose_direction_target(*ghost, pinky_target(ghost_shared->pacman), ghost_shared->options.map);
                    break;
                case INKY:
                    ghost->e.dir = choose_direction_target(*ghost, inky_target(ghost_shared->pacman, ghost_shared->ghosts[0].e), ghost_shared->options.map);
                    break;
                case CLYDE:
                    ghost->e.dir = choose_direction_target(*ghost, clyde_target(ghost_shared->pacman, ghost_shared->ghosts[3].e), ghost_shared->options.map);
                    break;
            }
            break;
    }
}

/**
 * Gestisce il comportamento dei fantasmi se sono in una posizione particolare
 * 
* @param ghost_shared dati condivisi tra i thread dei fantasmi
 * @param ghost fantasma preso in considerazione
 */
void manage_position_events(GhostShared* ghost_shared, CharGhost* ghost)
{
    int i;
    //Se sono a casa e morti 
    if(ghost->e.p.x == HOME_POSITION.x && ghost->e.p.y == HOME_POSITION.y && ghost->mode == M_DEAD)
    {
        ghost->mode = M_RESPAWN;
        ghost->timers.respawn = start_timer(ghost_shared->options.time_spawn);

    }
    //Se sono nella stessa posizione di un altro fantasma nella mappa
    if(ghost_shared->options.boing && !is_in_pen(*ghost))
    {
        for(i = 0; i < ghost_shared->num_ghosts; i++)
        {
            if(i != ghost->e.id)
            {            
                //Rimbalzano solo con direzioni di movimento diverse
                if(ghost->e.dir != ghost_shared->ghosts[i].e.dir)
                {
                    if(ghost->e.p.x == ghost_shared->ghosts[i].e.p.x && ghost->e.p.y == ghost_shared->ghosts[i].e.p.y)
                    {
                        reverse_direction(&ghost->e.dir);
                        reverse_direction(&ghost_shared->ghosts[i].e.dir);
                    }
                }
            } 
        
        }
    }
}

void ghost_wait(CharGhost ghost, GhostShared* ghost_shared)
{
    int movepause = ghost_shared->options.options_speed.ghost_speed;

    if(ghost.e.dir == UP || ghost.e.dir == DOWN) //Gestisce la velocità in base alla direzione
        movepause *= 2;
    if(ghost.mode == M_DEAD)
        movepause /= 3;
    else if(ghost.mode == M_FRIGHT)
        movepause *= 2;

    usleep(movepause); //Ferma il processo
}

/**
 * Muove il fantasma
 * 
 * @param ghost fantasma in considerazione
 * @param map mappa di gioco
 */
void ghost_move(CharGhost* ghost, char map[MAP_HEIGHT][MAP_WIDTH+1])
{         
    //Controllo possa muoversi
    if(can_move_ghost(*ghost, ghost->e.dir, map))
    {        
        switch(ghost->e.dir)
        {
            case UP:
                ghost->e.p.y--;
                break;
            case LEFT:
                ghost->e.p.x--;
                break;
            case DOWN:
                ghost->e.p.y++;
                break;
            case RIGHT:
                ghost->e.p.x++;
                break;
        }
    }
    map_loop(&ghost->e.p);
}

_Bool is_empty_space_ghost(char c)
{
    return is_empty_space(c) || c=='^' || c == 'v' || c=='<' || c=='>' || c=='[' || c==']';
}

/**
 * Controllo se il fantasma può muoversi nella nuova direzione 
 * 
 * @param ghost fantasma preso in considerazione
 * @param direction nuova direzione da controllare se vuota/fattibile
 * @param map mappa di gioco
 */
_Bool can_move_ghost(CharGhost ghost, Direction direction, char map[MAP_HEIGHT][MAP_WIDTH+1])
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+i, ghost.e.p.y-1, map)))
                    return false;
                if(ghost.mode != M_DEAD && get_map_at(ghost.e.p.x+i, ghost.e.p.y-1, map) == 'v')
                    return false;
            }
            if(get_map_at(ghost.e.p.x, ghost.e.p.y, map) == '#')
                return false;
            break;
        case LEFT:
            if(!is_empty_space_ghost(get_map_at(ghost.e.p.x-2, ghost.e.p.y, map)))
                return false;
            if(ghost.mode != M_DEAD && (get_map_at(ghost.e.p.x-2, ghost.e.p.y, map) == '>' || get_map_at(ghost.e.p.x-2, ghost.e.p.y, map) == ']'))
                return false;
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+i,ghost.e.p.y+1, map)))
                    return false;
                if(ghost.mode != M_DEAD && get_map_at(ghost.e.p.x+i, ghost.e.p.y+1, map) == '^')
                    return false;
            }
            break;
        case RIGHT:
            if(!is_empty_space_ghost(get_map_at(ghost.e.p.x+2, ghost.e.p.y, map)))
                return false;
            if(ghost.mode != M_DEAD && (get_map_at(ghost.e.p.x+2, ghost.e.p.y, map) == '<' || get_map_at(ghost.e.p.x+2, ghost.e.p.y, map) == '['))
                return false;
            break;
    }

    return true;
}

/**
 * Controllo se il fantasma è nella monster pen
 * 
 * @param ghost fantasma da controllare
 */
_Bool is_in_pen(CharGhost ghost)
{
    return (ghost.e.p.x >= 20 && ghost.e.p.y >= 12 && ghost.e.p.x <= 34 && ghost.e.p.y <= 16);
}

/**
 * In caso i fantasmi spawnino random inserisce i fantasmi nella mappa
 * 
 * @param ghost_shared dati condivisi tra i thread dei fantasmi 
 */
void init_ghost_map(GhostShared* ghost_shared)
{
    int n_pos;
    //Numeri a caso per la posizione dei pellet dove andranno i fantasmi
    int *rand_nums = malloc(sizeof(int)*(ghost_shared->options.num_ghosts));
    int i,j;
    Position pos;

    //Conta quanti pallini ci sono nella mappa
    n_pos = count_mat_occ(MAP_HEIGHT, MAP_WIDTH, PELLETS, '~');
    get_rand_nums(0, n_pos, ghost_shared->options.num_ghosts, rand_nums);
    for (i = 0; i < ghost_shared->options.num_ghosts; i++)
    {   
        //Trova l'iesima posizione di un carattere e ne restituisce x e y
        pos = get_i_ch_pos(MAP_HEIGHT, MAP_WIDTH, PELLETS, '~', rand_nums[i]);   
        ghost_shared->starting_pos[i] = pos;
    }
}

/**
 * Controlla se i fantasmi nascosti debbano spawnare creando un nuovo thread
 * 
 * @param ghost_parameters array di fantasmi e id del fantasma
 */
void check_ghost_spawn(GhostParameters* ghost_parameters)
{
    GhostShared* ghost_shared = ghost_parameters[0].ghost_shared;
    int num_ghosts = ghost_shared->num_ghosts;
    int i,j;
    pthread_t ghost;

    for(i = 0; i < num_ghosts; i++)
    {
        //Controlla se pacman è sopra uno dei fantasmi
        if(ghost_shared->pacman.p.x == ghost_shared->starting_pos[i].x && ghost_shared->pacman.p.y == ghost_shared->starting_pos[i].y && ghost_shared->ghosts[i].mode == M_INACTIVE)
        {        
            pthread_create(&ghost, NULL, &ghost_thread, &ghost_parameters[i]);
            ghost_shared->ghosts[i].mode = M_IDLE;
        }   
    }
}