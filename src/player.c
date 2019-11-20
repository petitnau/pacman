#include <curses.h>
#include <unistd.h>

#include "player.h"
#include "utils.h"

void player_main(int cmd_out, int log_out)
{
    char c;
    Direction direction;

    while(1)
    {
        switch(c=getch())
        {
            case K_UP:
                direction = UP;
                break;
            case K_DOWN:
                direction = DOWN;
                break;
            case K_RIGHT:
                direction = RIGHT;
                break;
            case K_LEFT:
                direction = LEFT;
                break;
        }

        write(cmd_out, &direction, sizeof(direction));  //scrivo input a pacman
    }
}