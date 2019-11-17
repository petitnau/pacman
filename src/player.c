#include <curses.h>
#include <unistd.h>

#include "player.h"
#include "utils.h"

void player_main(int cmd_out)
{
    char c;
    Direction direction;

    while(1)
    {
        switch(c=getch())
        {
            case K_UP:
            case K_DOWN:
            case K_RIGHT:
            case K_LEFT:
                direction = c-65;
                break;
        }

        write(cmd_out, &direction, sizeof(direction));
    }
}