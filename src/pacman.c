#include <stdbool.h>
#include <unistd.h>

#include "pacman.h"
#include "utils.h"

_Bool can_cut_turn(Entity pacman, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=0; i<4; i++)
            {
                if(pacman.dir == UP || pacman.dir == DOWN) 
                    return true;
                else if(pacman.dir == RIGHT)
                {
                    if(MAP[pacman.y-1][pacman.x+i]==' ')
                        return true;
                }
                else
                    if(MAP[pacman.y-1][pacman.x-i]==' ')
                        return true;
            }
            break;
        case DOWN:
            for(i=0; i<4; i++)
            {
                if(pacman.dir == UP || pacman.dir == DOWN) 
                    return true;
                else if(pacman.dir == RIGHT)    
                {
                    if(MAP[pacman.y+1][pacman.x+i]==' ')
                        return true;
                }
                else                    
                    if(MAP[pacman.y+1][pacman.x-i]==' ')
                        return true;
            }
            break;
        case RIGHT:
            for(i=0; i<3; i++)
            {
                if(pacman.dir == RIGHT || pacman.dir == LEFT) 
                    return true;
                else if(pacman.dir == UP)
                {
                    if(MAP[pacman.y-i][pacman.x+2]==' ')
                        return true;
                }
                else
                    if(MAP[pacman.y+i][pacman.x+2]==' ')
                        return true;
            }
            break;
        case LEFT:
            for(i=0; i<3; i++)
            {
                if(pacman.dir == RIGHT || pacman.dir == LEFT)
                    return true;
                else if(pacman.dir == UP)
                {
                    if(MAP[pacman.y-i][pacman.x-2]==' ')
                        return true;
                }
                else
                    if(MAP[pacman.y+i][pacman.x-2]==' ')
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
                if(MAP[pacman.y-1][pacman.x+i] != ' ')
                    return false;
            }
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(MAP[pacman.y+1][pacman.x+i] != ' ')
                    return false;
            }
            break;
        case RIGHT:
            if(MAP[pacman.y][pacman.x+2] != ' ')
                return false;
            break;
        case LEFT:
            if(MAP[pacman.y][pacman.x-2] != ' ')
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
        if(read(cmd_in, &tmp_dir, sizeof(tmp_dir)) != -1)
        {            
            if(can_cut_turn(pacman, tmp_dir))
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

        write(pos_out, &pacman, sizeof(pacman)); //invia la posizione a control

        usleep(100000);
    }
}
