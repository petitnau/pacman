#include <unistd.h>
#include "ghost.h"
#include "utils.h"
#include "entity.h"
#include "drawings.h"
#include "ai.h"

void ghost_main(int pipe_in, int pos_out, int log_out)
{
    CharGhost ghost = {{GHOST_ID, {GHOST_START_X, GHOST_START_Y}, GHOST_START_DIR}, M_CHASE};

    GhostInfo ghost_info = {{PACMAN_ID, {PAC_START_X, PAC_START_Y}, PAC_START_DIR}, false, false};

    int i = 0;
    long fright_timer=0;
    int movepause;

    while(1)
    {       
        while(read(pipe_in, &ghost_info, sizeof(ghost_info)) != -1)
        {                
            if(ghost_info.fright)
            {
                fright_timer = start_timer(6);
                ghost.mode = M_FRIGHT;  
                ghost.e.dir = reverse_direction(ghost.e.dir);
            }
            if(ghost_info.death)
            {
                fright_timer = 0;
                ghost.mode = M_DEAD;
            }
            if(ghost_info.full)
            {
                ghost.e.p.x = GHOST_START_X;
                ghost.e.p.y = GHOST_START_Y;
                ghost.e.dir = UP;
            }
        }
       
        switch(ghost.mode)
        {
            case M_FRIGHT:
                if(check_timer(fright_timer))
                    ghost.e.dir = choose_direction_random(ghost.e);
                    //ghost.e.dir = choose_direction_target(ghost.e, SCATTER[0]);
                else
                {
                    ghost.mode = M_CHASE; //ciclo di chase saltato?
                }
                break;
            case M_DEAD:
                    ghost.e.dir = choose_direction_target(ghost.e, HOME_TARGET);
                break;
            default:
                ghost.e.dir = choose_direction_target(ghost.e, blinky_target(ghost_info.pacman));
                break;
        }

        switch(ghost.e.dir)
        {
            case UP:
                ghost.e.p.y--;
                break;
            case LEFT:
                ghost.e.p.x--;
                break;
            case DOWN:
                ghost.e.p.y++;
                break;
            case RIGHT:
                ghost.e.p.x++;
                break;
        }

        if(ghost.e.p.x == 0 && ghost.e.dir == LEFT)
            ghost.e.p.x = MAP_WIDTH;
        if(ghost.e.p.x == MAP_WIDTH && ghost.e.dir == RIGHT)
            ghost.e.p.x = 0;

        if(ghost.e.p.x == HOME_TARGET.x && ghost.e.p.y == HOME_TARGET.y && ghost.mode == M_DEAD)
        {
            ghost.e.dir = UP;
            ghost.mode = M_CHASE;
        }

        write(pos_out, &ghost, sizeof(ghost)); //invia la posizione a control

        movepause = GHOST_SPEED;
        if(ghost.e.dir == UP || ghost.e.dir == DOWN) //gestisce la velocità
            movepause *= 2;
        if(ghost.mode == M_FRIGHT)
            movepause *= 2;

        usleep(movepause);
    
    }
}