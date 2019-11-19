#include <unistd.h>
#include "ghost.h"
#include "utils.h"
#include "entity.h"
#include "drawings.h"
#include "ai.h"

void ghost_main(int pipe_in, int pos_out, int log_out)
{
    CharGhost ghost = {{GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR}, M_CHASE};

    GhostInfo ghost_info = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, false};

    int i = 0;

    while(1)
    {       
        while(read(pipe_in, &ghost_info, sizeof(ghost_info)) != -1);

        if(!check_timer(ghost_info.fright))
        {
            ghost.e.dir = choose_direction_target(ghost.e, blinky_target(ghost_info.pacman));
            ghost.mode = M_CHASE;
        }
        else
        {
            ghost.e.dir = choose_direction_target(ghost.e, SCATTER[0]);
            ghost.mode = M_FRIGHT;
        }
        switch(ghost.e.dir)
        {
            case UP:
                ghost.e.p.y--;
                break;
            case DOWN:
                ghost.e.p.y++;
                break;
            case RIGHT:
                ghost.e.p.x++;
                break;
            case LEFT:
                ghost.e.p.x--;
                break;
        }

        if(ghost.e.p.x == 0 && ghost.e.dir == LEFT)
            ghost.e.p.x = MAP_WIDTH;
        if(ghost.e.p.x == MAP_WIDTH && ghost.e.dir == RIGHT)
            ghost.e.p.x = 0;

        write(pos_out, &ghost, sizeof(ghost)); //invia la posizione a control

        if(ghost.e.dir == UP || ghost.e.dir == DOWN) //gestisce la velocità di pacman
            usleep(GHOST_SPEED*2);
        else
            usleep(GHOST_SPEED);
    
    }
}