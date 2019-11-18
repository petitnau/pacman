#include <unistd.h>
#include "ghost.h"
#include "utils.h"
#include "entity.h"
#include "drawings.h"
#include "ai.h"

void ghost_main(int id, int pipe_in, int pos_out)
{
    Entity ghost = {GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR};

    Ghost_Info ghost_info = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, false};

    int i = 0;

    while(1)
    {       
        while(read(pipe_in, &ghost_info, sizeof(ghost_info)) != -1);
        
        if(!can_move(ghost, ghost.dir))
        {          
            ghost.dir = choose_direction_target(ghost, blinky_target(ghost_info.pacman));
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