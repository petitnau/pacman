#include <stdbool.h>
#include <unistd.h>

#include "pacman.h"
#include "entity.h"
#include "utils.h"
#include "drawings.h"

void pacman_main(int cmd_in, int pos_out)
{
    Entity pacman = {PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR};
    Direction nextDir = UP;

    Direction tmp_dir;
    int i;

    while(1)
    {
        //Legge solo l'ultimo inserito nella pipe e controlla se è una mossa valida
        while(read(cmd_in, &tmp_dir, sizeof(tmp_dir)) != -1)
        {            
            if(accept_turn(pacman, tmp_dir))
            {
                nextDir = tmp_dir;
            }
        }

        if(can_move(pacman, nextDir))   //controlla se può fare movimento nella nuova direzione
            pacman.dir = nextDir;
                  
        if(can_move(pacman, pacman.dir))
        {        
            switch(pacman.dir)
            {
                case UP:
                    pacman.p.y--;
                    break;
                case DOWN:
                    pacman.p.y++;
                    break;
                case RIGHT:
                    pacman.p.x++;
                    break;
                case LEFT:
                    pacman.p.x--;
                    break;
            }
        }

        if(pacman.p.x == 0 && pacman.dir == LEFT)
            pacman.p.x = MAP_WIDTH;
        if(pacman.p.x == MAP_WIDTH && pacman.dir == RIGHT)
            pacman.p.x = 0;

        write(pos_out, &pacman, sizeof(pacman)); //invia la posizione a control

        if(pacman.dir == UP || pacman.dir == DOWN) //gestisce la velocità di pacman
            usleep(PAC_SPEED*2);
        else
            usleep(PAC_SPEED);
    }
}
