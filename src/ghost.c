#include "ghost.h"
#include "utils.h"
#include "entity.h"
#include "drawings.h"

void ghost_main(int i, int pipein, int pos_out)
{
    Entity ghost = {GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR};

    while(1)
    {        
        if(!can_move(ghost, ghost.dir))
        {  
            //Scegli nuova direzione  
        }    

        switch(ghost.dir)
        {
            case UP:
                ghost.p.y--;
                break;
            case DOWN:
                ghost.p.y++;
                break;
            case RIGHT:
                ghost.p.x++;
                break;
            case LEFT:
                ghost.p.x--;
                break;
        }

        if(ghost.p.x == 0 && ghost.dir == LEFT)
            ghost.p.x = MAP_WIDTH;
        if(ghost.p.x == MAP_WIDTH && ghost.dir == RIGHT)
            ghost.p.x = 0;

        write(pos_out, &ghost, sizeof(ghost)); //invia la posizione a control

        if(ghost.dir == UP || ghost.dir == DOWN) //gestisce la velocità di pacman
            usleep(GHOST_SPEED*2);
        else
            usleep(GHOST_SPEED);
    
    }
}