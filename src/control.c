#include <unistd.h>
#include <curses.h>

#include "control.h"
#include "utils.h"


void score_handler(int * score, Entity tmp_pkg);

void control_main(int pos_in, int ghost_out)
{
    Entity pacman = {PACMAN_ID, 27, 23, UP};
    Entity tmp_pkg;

    int x,y;
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

void score_handler(int * score, Entity tmp_pkg)
{
    char next_ch;
    switch (tmp_pkg.dir)
        {
            case UP:
                next_ch = mvinch(tmp_pkg.y, tmp_pkg.x); //prende carattere a schermo nella pos y,x
                if(next_ch == '~')
                    *score += 100;
                break;
            case DOWN:
                next_ch = mvinch(tmp_pkg.y, tmp_pkg.x);
                if(next_ch == '~')
                    *score += 100;
            break;
            case RIGHT:
                next_ch = mvinch(tmp_pkg.y, tmp_pkg.x+1);
                if(next_ch == '~')
                    *score += 100;
            break;
            case LEFT:
                next_ch = mvinch(tmp_pkg.y, tmp_pkg.x-1);
                if(next_ch == '~')
                    *score += 100;
            break;
        }
}
