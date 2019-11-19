#include <curses.h>
#include <string.h>
#include "entity.h"
#include "drawings.h"

_Bool accept_turn(Entity entity, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            if(entity.dir == UP || entity.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(entity.dir == RIGHT && get_map_at(entity.p.x+i, entity.p.y-1)==' ')
                            return true;
                    else if(entity.dir == LEFT && get_map_at(entity.p.x-i, entity.p.y-1)==' ')
                            return true;
                }
            break;
        case DOWN:
            if(entity.dir == UP || entity.dir == DOWN) 
                return true;
            else
                for(i=0; i<VERTICALCUT; i++)
                {
                    if(entity.dir == RIGHT && get_map_at(entity.p.x+i, entity.p.y+1)==' ')
                            return true;
                    else if(entity.dir == LEFT && get_map_at(entity.p.x-i, entity.p.y+1)==' ')
                            return true;
                }
            break;
        case RIGHT:
            if(entity.dir == RIGHT || entity.dir == LEFT) 
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(entity.dir == UP && get_map_at(entity.p.x+2, entity.p.y-i)==' ')
                            return true;
                    else if(entity.dir == DOWN && get_map_at(entity.p.x+2, entity.p.y+i)==' ')
                            return true;
                }
            break;
        case LEFT:
            if(entity.dir == RIGHT || entity.dir == LEFT)
                return true;
            else
                for(i=0; i<HORIZONTALCUT; i++)
                {
                    if(entity.dir == UP && get_map_at(entity.p.x-2, entity.p.y-i)==' ')
                            return true;
                    else if(entity.dir == DOWN && get_map_at(entity.p.x-2, entity.p.y+i)==' ')
                            return true;
                }
            break;
    }

    return false;
}

_Bool can_move(Entity entity, Direction direction)
{
    int i;

    switch(direction)
    {
        case UP:
            for(i=-1; i<=1; i++)
            {
                if(get_map_at(entity.p.x+i, entity.p.y-1) != ' ')
                    return false;
            }
            break;
        case DOWN:
            for(i=-1; i<=1; i++)
            {
                if(get_map_at(entity.p.x+i, entity.p.y+1) != ' ')
                    return false;
            }
            break;
        case RIGHT:
            if(get_map_at(entity.p.x+2, entity.p.y) != ' ')
                return false;
            break;
        case LEFT:
            if(get_map_at(entity.p.x-2, entity.p.y) != ' ')
                return false;
            break;
    }

    return true;
}
