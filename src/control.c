#include <unistd.h>
#include <curses.h>

#include "control.h"
#include "utils.h"

void control_main(int pos_in, int ghost_out)
{
    Entity pacman = {PACMAN_ID, 27, 23, RIGHT};
    Entity tmp_pkg;

    mvaddch(0,0, MAP[25][21]);
    refresh();

    int i;

    while(1)
    {
        read(pos_in, &tmp_pkg, sizeof(tmp_pkg));
        
        for(i=-1; i<=1; i++)
        {
            mvaddch(pacman.y, pacman.x+i, MAP[pacman.y][pacman.x+i]);
        }
        
        pacman = tmp_pkg;
        
        mvprintw(0,60, "x:%d,y:%d,d:%d", pacman.x, pacman.y, pacman.dir);

        mvaddstr(pacman.y, pacman.x-1, "(*<");
        refresh();
    }
}