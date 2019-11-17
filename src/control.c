#include <unistd.h>
#include <curses.h>

#include "control.h"
#include "utils.h"


void score_handler(int * score, Entity tmp_pkg);

void control_main(int pos_in, int ghost_out)
{
    Entity pacman = {PACMAN_ID, 27, 23, UP};
    Entity tmp_pkg;

    Position pos;
    int i,j;

    int score = 0;
    char next_ch;
    while(1)
    {
        mvprintw(2, 60, "Score:   %d", score);
        read(pos_in, &tmp_pkg, sizeof(tmp_pkg));    //leggo posizione di pacman
        
        score_handler(&score, tmp_pkg);

        for(i=-1; i<=1/* code */; i++)    //cancella pacman
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

void score_handler(int * score, Entity tmp_pkg)
{
    char next_ch;
    switch (tmp_pkg.dir)
        {
            case UP:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x); //prende carattere a schermo nella pos y,x
                if(next_ch == '~')
                    *score += 100;
                break;
            case DOWN:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x);
                if(next_ch == '~')
                    *score += 100;
            break;
            case RIGHT:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x+1);
                if(next_ch == '~')
                    *score += 100;
            break;
            case LEFT:
                next_ch = mvinch(tmp_pkg.p.y, tmp_pkg.p.x-1);
                if(next_ch == '~')
                    *score += 100;
            break;
        }
}
