#include <stdbool.h>
#include <unistd.h>

#include "pacman.h"
#include "utils.h"

#define VERTICALCUT 6
#define HORIZONTALCUT 4

_Bool accept_turn(Entity pacman, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            if(pacman.dir == UP || pacman.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(pacman.dir == RIGHT && get_map_at(pacman.p.x+i, pacman.p.y-1)==' ')
                            return true;
                    else if(pacman.dir == LEFT && get_map_at(pacman.p.x-i, pacman.p.y-1)==' ')
                            return true;
                }
            break;
        case DOWN:
            if(pacman.dir == UP || pacman.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(pacman.dir == RIGHT && get_map_at(pacman.p.x+i, pacman.p.y+1)==' ')
                            return true;
                    else if(pacman.dir == LEFT && get_map_at(pacman.p.x-i, pacman.p.y+1)==' ')
                            return true;
                }
            break;
        case RIGHT:
            if(pacman.dir == RIGHT || pacman.dir == LEFT) 
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.dir == UP && get_map_at(pacman.p.x+2, pacman.p.y-i)==' ')
                            return true;
                    else if(pacman.dir == DOWN && get_map_at(pacman.p.x+2, pacman.p.y+i)==' ')
                            return true;
                }
            break;
        case LEFT:
            if(pacman.dir == RIGHT || pacman.dir == LEFT)
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.dir == UP && get_map_at(pacman.p.x-2, pacman.p.y-i)==' ')
                            return true;
                    else if(pacman.dir == DOWN && get_map_at(pacman.p.x-2, pacman.p.y+i)==' ')
                            return true;
                }
            break;
    }

    return false;
}

_Bool can_move(Entity pacman, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=-1; i<=1; i++)
            {
                if(get_map_at(pacman.p.x+i, pacman.p.y-1) != ' ')
                    return false;
            }
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(get_map_at(pacman.p.x+i, pacman.p.y+1) != ' ')
                    return false;
            }
            break;
        case RIGHT:
            if(get_map_at(pacman.p.x+2, pacman.p.y) != ' ')
                return false;
            break;
        case LEFT:
            if(get_map_at(pacman.p.x-2, pacman.p.y) != ' ')
                return false;
            break;
    }

    return true;
}

void pacman_main(int cmd_in, int pos_out)
{
    Entity pacman = {PACMAN_ID, PAC_START_X, PAC_START_Y, PAC_START_DIR};
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
            usleep(165000);
        else
            usleep(100000);
    }
}
 