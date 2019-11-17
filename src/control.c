#include <unistd.h>
#include <curses.h>

#include "control.h"
#include "utils.h"

void control_main(int pos_in, int ghost_out)
{
    Entity pacman = {PACMAN_ID, 27, 23, RIGHT};
    Entity tmp_pkg;

    Position pos;
    int i,j;

    while(1)
    {
        read(pos_in, &tmp_pkg, sizeof(tmp_pkg));    //leggo posizione di pacman
        
        for(i=-1; i<=1; i++)    //cancella pacman
        {
            pos.x=pacman.p.x+i;
            pos.y=pacman.p.y;
            pos = get_pac_eff_pos(pos);
            
            mvaddch(pos.y, pos.x, MAP[pos.y][pos.x]);
        }
        
        pacman = tmp_pkg;
        
        mvprintw(0,60, "x:%d,y:%d,d:%d", pacman.p.x, pacman.p.y, pacman.dir);
        print_pacman(pacman);
        refresh();
    }
}