#include <unistd.h>
#include <curses.h>

#include "control.h"
#include "utils.h"

void control_main(int pos_in, int ghost_out)
{
    Entity pacman = {PACMAN_ID, 27, 23, RIGHT};
    Entity tmp_pkg;

    int x,y;
    int i,j;

    while(1)
    {
        read(pos_in, &tmp_pkg, sizeof(tmp_pkg));    //leggo posizione di pacman
        
        for(i=-1; i<=1; i++)    //cancella pacman
        {
            x=pacman.x+i;
            y=pacman.y;
            get_pac_eff_pos(&y, &x);
            
            mvaddch(y, x, MAP[y][x]);
        }
        
        pacman = tmp_pkg;
        
        mvprintw(0,60, "x:%d,y:%d,d:%d", pacman.x, pacman.y, pacman.dir);
        print_pacman(pacman);
        refresh();
    }
}