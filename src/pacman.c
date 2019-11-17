#include <stdbool.h>
#include <unistd.h>

#include "pacman.h"
#include "utils.h"

#define VERTICALCUT 6
#define HORIZONTALCUT 3

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
                    if(pacman.dir == RIGHT && get_map_at(pacman.y-1, pacman.x+i)==' ')
                            return true;
                    else if(pacman.dir == LEFT && get_map_at(pacman.y-1, pacman.x-i)==' ')
                            return true;
                }
            break;
        case DOWN:
            if(pacman.dir == UP || pacman.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(pacman.dir == RIGHT && get_map_at(pacman.y+1, pacman.x+i)==' ')
                            return true;
                    else if(pacman.dir == LEFT && get_map_at(pacman.y+1, pacman.x-i)==' ')
                            return true;
                }
            break;
        case RIGHT:
            if(pacman.dir == RIGHT || pacman.dir == LEFT) 
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.dir == UP && get_map_at(pacman.y-i, pacman.x+2)==' ')
                            return true;
                    else if(pacman.dir == DOWN && get_map_at(pacman.y+i, pacman.x+2)==' ')
                            return true;
                }
            break;
        case LEFT:
            if(pacman.dir == RIGHT || pacman.dir == LEFT)
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(pacman.dir == UP && get_map_at(pacman.y-i, pacman.x-2)==' ')
                            return true;
                    else if(pacman.dir == DOWN && get_map_at(pacman.y+i, pacman.x-2)==' ')
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
                if(get_map_at(pacman.y-1, pacman.x+i) != ' ')
                    return false;
            }
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(get_map_at(pacman.y+1, pacman.x+i) != ' ')
                    return false;
            }
            break;
        case RIGHT:
            if(get_map_at(pacman.y, pacman.x+2) != ' ')
                return false;
            break;
        case LEFT:
            if(get_map_at(pacman.y, pacman.x-2) != ' ')
                return false;
            break;
    }

    return true;
}

void pacman_main(int cmd_in, int pos_out)
{
    Entity pacman = {PACMAN_ID, 27, 23, UP};
    Direction nextDir = UP;

    Direction tmp_dir;
    int i;

    while(1)
    {
        while(read(cmd_in, &tmp_dir, sizeof(tmp_dir)) != -1)
        {            
            if(accept_turn(pacman, tmp_dir))
            {
                nextDir = tmp_dir;
            }
        }

        if(can_move(pacman, nextDir))
            pacman.dir = nextDir;
                  
        if(can_move(pacman, pacman.dir))
        {        
            switch(pacman.dir)
            {
                case UP:
                    pacman.y--;
                    break;
                case DOWN:
                    pacman.y++;
                    break;
                case RIGHT:
                    pacman.x++;
                    break;
                case LEFT:
                    pacman.x--;
                    break;
            }
        }

        if(pacman.x == MAPXMIN && pacman.dir == LEFT)
            pacman.x = MAPXMAX;
        if(pacman.x == MAPXMAX && pacman.dir == RIGHT)
            pacman.x = MAPXMIN;

        write(pos_out, &pacman, sizeof(pacman)); //invia la posizione a control

        if(pacman.dir == UP || pacman.dir == DOWN)
            usleep(165000);
        else
            usleep(100000);
    }
}
 