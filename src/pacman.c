#include <stdbool.h>
#include <unistd.h>

#include "pacman.h"
#include "entity.h"
#include "utils.h"
#include "drawings.h"

void pacman_main(int cmd_in, int pos_out, int log_out)
{
    CharPacman pacman = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, PAC_START_LIVES};
    Direction nextDir = PAC_START_DIR;

    Direction tmp_dir;
    int i;

    while(1)
    {
        //Legge solo l'ultimo inserito nella pipe e controlla se è una mossa valida
        while(read(cmd_in, &tmp_dir, sizeof(tmp_dir)) != -1)
        {            
            if(accept_turn(pacman.e, tmp_dir))
            {
                nextDir = tmp_dir;
            }
        }

        if(can_move(pacman.e, nextDir))   //controlla se può fare movimento nella nuova direzione
            pacman.e.dir = nextDir;
                  
        if(can_move(pacman.e, pacman.e.dir))
        {        
            switch(pacman.e.dir)
            {
                case UP:
                    pacman.e.p.y--;
                    break;
                case DOWN:
                    pacman.e.p.y++;
                    break;
                case RIGHT:
                    pacman.e.p.x++;
                    break;
                case LEFT:
                    pacman.e.p.x--;
                    break;
            }
        }

        if(pacman.e.p.x == 0 && pacman.e.dir == LEFT)
            pacman.e.p.x = MAP_WIDTH;
        if(pacman.e.p.x == MAP_WIDTH && pacman.e.dir == RIGHT)
            pacman.e.p.x = 0;

        write(pos_out, &pacman, sizeof(pacman)); //invia la posizione a control

        if(pacman.e.dir == UP || pacman.e.dir == DOWN) //gestisce la velocità di pacman
            usleep(PAC_SPEED*2);
        else
            usleep(PAC_SPEED);
    }
}
