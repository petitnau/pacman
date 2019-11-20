#include <stdbool.h>
#include <unistd.h>

#include "pacman.h"
#include "entity.h"
#include "utils.h"
#include "interface.h"

void manage_cmd_in(int, CharPacman*, Direction*);
void switch_direction(CharPacman*);
void pac_wait(CharPacman);

void pacman_main(int cmd_in, int pos_out, int log_out)
{
    CharPacman pacman = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, PAC_START_DIR, PAC_START_LIVES};

    Direction cmd_pkg;
    int i;

    while(1)
    {
        //Legge solo l'ultimo inserito nella pipe e controlla se è una mossa valida
        manage_cmd_in(cmd_in, &pacman, &cmd_pkg);
        switch_direction(&pacman);
        e_move(&pacman.e);
        write(pos_out, &pacman, sizeof(pacman)); //invia la posizione a control
        pac_wait(pacman);
    }
}

void manage_cmd_in(int cmd_in, CharPacman* pacman, Direction* cmd_pkg)
{
    while(read(cmd_in, cmd_pkg, sizeof(*cmd_pkg)) != -1)
    {            
        if(accept_turn(pacman->e, *cmd_pkg))
        {
            pacman->next_dir = *cmd_pkg;
        }
    }
}

void switch_direction(CharPacman* pacman)
{
    if(can_move(pacman->e, pacman->next_dir))   //controlla se può fare movimento nella nuova direzione
        pacman->e.dir = pacman->next_dir;
}

void pac_wait(CharPacman pacman)
{
    int movepause = PAC_SPEED;

    if(pacman.e.dir == UP || pacman.e.dir == DOWN) //gestisce la velocità di pacman
        movepause*=2;

    usleep(movepause);
}