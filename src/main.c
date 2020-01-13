#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "menu.h" 
#include "pacman.h" 
#include "control.h"
#include "player.h"
#include "ghost.h"
#include "options.h"

void init()
{
    initscr();
    cbreak();
    noecho(); 
    srand(time(NULL));
    curs_set(0);
    start_color(); 

    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_BLUE, 0, 0, 1000);
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_color(COLOR_RED, 1000, 0, 0);
    init_color(COLOR_CYAN, 0, 1000, 1000);
    init_color(COLOR_MAGENTA, 1000, 700, 700);
    init_color(COLOR_ORANGE, 1000, 700, 300);
    init_color(COLOR_PINK, 1000, 800, 800);
    init_color(COLOR_WHITE, 1000, 1000, 1000);
    init_color(COLOR_BROWN, 350, 250, 250);
    init_color(COLOR_BLUE_1, 0, 0, 700);
    init_color(COLOR_BLUE_2, 0, 0, 400);
    init_color(COLOR_BLUE_3, 0, 0, 200);

    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_PINK, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);

    init_pair(5, COLOR_BLACK, COLOR_RED);
    init_pair(6, COLOR_BLACK, COLOR_CYAN);
    init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(8, COLOR_BLACK, COLOR_ORANGE);
    
    init_pair(9, COLOR_WHITE, COLOR_BLUE);
    init_pair(10, COLOR_RED, COLOR_BLACK);
    init_pair(11, COLOR_GREEN, COLOR_BLACK);
    init_pair(12, COLOR_CYAN, COLOR_BLACK);
    init_pair(13, COLOR_MAGENTA, COLOR_BLACK);

    init_pair(14, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(15, COLOR_WHITE, COLOR_WHITE);
    init_pair(16, COLOR_BLACK, COLOR_BLACK);
    init_pair(17, COLOR_YELLOW, COLOR_BLACK);
    init_pair(18, COLOR_BLACK, COLOR_BROWN);
    init_pair(19, COLOR_PINK, COLOR_BLACK);
    init_pair(20, COLOR_ORANGE, COLOR_BLACK);
    init_pair(21, COLOR_BLUE_1, COLOR_BLACK);
    init_pair(22, COLOR_BLUE_2, COLOR_BLACK);
    init_pair(23, COLOR_BLUE_3, COLOR_BLACK);

    refresh();
}

int main()
{
    int i;
    int menu_choice;
    pid_t p_player, p_pacman, p_ghosts, p_bullet;
    int pacman_ch_pipe[2], pacman_info_pipe[2], ghost_ch_pipe[2], ghost_info_pipe[2], cmd_pipe[2], log_pipe[2], bullet_info [2], bullet_pos[2];
    init();
    Options options;

    keypad(stdscr, true);

    #define MENU_EXIT 4
    do
    {
        erase();
        main_menu(&options);

        if(menu_choice==MENU_EXIT)
            break;
        //print_map();
        //print_food();
        refresh();
        
        if(pipe(pacman_ch_pipe) == -1)
            _exit(PIPE_ERROR);
        if(pipe(pacman_info_pipe) == -1)
            _exit(PIPE_ERROR);
        if(pipe(ghost_ch_pipe) == -1)
            _exit(PIPE_ERROR);
        if(pipe(ghost_info_pipe) == -1)
            _exit(PIPE_ERROR);
        if(pipe(cmd_pipe) == -1)
            _exit(PIPE_ERROR);
        if(pipe(log_pipe) == -1)
            _exit(PIPE_ERROR);
        if(pipe(bullet_pos) == -1)
            _exit(PIPE_ERROR);
        if(pipe(bullet_info) == -1)
            _exit(PIPE_ERROR);

        fcntl(pacman_ch_pipe[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(pacman_ch_pipe[P_WR], F_SETFL, O_NONBLOCK);
        fcntl(pacman_info_pipe[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(pacman_info_pipe[P_WR], F_SETFL, O_NONBLOCK);
        fcntl(ghost_ch_pipe[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(ghost_ch_pipe[P_WR], F_SETFL, O_NONBLOCK);
        fcntl(cmd_pipe[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(cmd_pipe[P_WR], F_SETFL, O_NONBLOCK);
        fcntl(ghost_info_pipe[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(ghost_info_pipe[P_WR], F_SETFL, O_NONBLOCK);
        fcntl(log_pipe[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(log_pipe[P_WR], F_SETFL, O_NONBLOCK);
        fcntl(bullet_info[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(bullet_info[P_WR], F_SETFL, O_NONBLOCK);
        fcntl(bullet_pos[P_RD], F_SETFL, O_NONBLOCK);
        fcntl(bullet_pos[P_WR], F_SETFL, O_NONBLOCK);

        switch(p_pacman = fork())
        {
            case -1:
                _exit(FORK_ERROR);
            case 0: //pacman
                close(pacman_ch_pipe[P_RD]); 
                close(pacman_info_pipe[P_WR]);   
                close(ghost_ch_pipe[P_RD]);      
                close(ghost_ch_pipe[P_WR]);      
                close(ghost_info_pipe[P_RD]);
                close(ghost_info_pipe[P_WR]);
                close(cmd_pipe[P_RD]);
                close(cmd_pipe[P_WR]);
                close(log_pipe[P_RD]);
                close(bullet_info[P_RD]);
                close(bullet_pos[P_RD]);
                close(bullet_pos[P_WR]);
                pacman_main(options, pacman_info_pipe[P_RD], pacman_ch_pipe[P_WR], bullet_info[P_WR], log_pipe[P_WR]);
        }   

        switch(p_ghosts = fork())
        {
            case -1:
                _exit(FORK_ERROR);
            case 0:
                close(pacman_ch_pipe[P_RD]);      
                close(pacman_ch_pipe[P_WR]);     
                close(pacman_info_pipe[P_WR]);   
                close(pacman_info_pipe[P_RD]);   
                close(ghost_ch_pipe[P_RD]);      
                close(ghost_info_pipe[P_WR]);
                close(cmd_pipe[P_RD]);
                close(cmd_pipe[P_WR]);
                close(log_pipe[P_RD]);
                close(bullet_info[P_RD]);
                close(bullet_pos[P_RD]);
                close(bullet_pos[P_WR]);
                ghost_main(options,ghost_info_pipe[P_RD], ghost_ch_pipe[P_WR], bullet_info[P_WR], log_pipe[P_WR]);
        }

        switch(p_player = fork())
        {
            case -1:
                _exit(FORK_ERROR);
            case 0:
                close(pacman_ch_pipe[P_RD]);      
                close(pacman_ch_pipe[P_WR]);
                close(pacman_info_pipe[P_WR]);   
                close(pacman_info_pipe[P_RD]); 
                close(ghost_ch_pipe[P_RD]);      
                close(ghost_ch_pipe[P_WR]);      
                close(ghost_info_pipe[P_RD]);
                close(ghost_info_pipe[P_WR]);
                close(cmd_pipe[P_RD]);
                close(log_pipe[P_RD]);
                close(bullet_info[P_RD]);
                close(bullet_info[P_WR]);
                close(bullet_pos[P_RD]);
                close(bullet_pos[P_WR]);
                player_main(cmd_pipe[P_WR], log_pipe[P_WR]);
        }
        
        switch(p_bullet = fork())
        {
            case -1:
                _exit(FORK_ERROR);
            case 0:
                close(pacman_ch_pipe[P_RD]);      
                close(pacman_ch_pipe[P_WR]);
                close(pacman_info_pipe[P_WR]);   
                close(pacman_info_pipe[P_RD]); 
                close(ghost_ch_pipe[P_RD]);      
                close(ghost_ch_pipe[P_WR]);      
                close(ghost_info_pipe[P_RD]);
                close(ghost_info_pipe[P_WR]);
                close(cmd_pipe[P_RD]);
                close(cmd_pipe[P_WR]);
                close(log_pipe[P_RD]);
                close(log_pipe[P_WR]);
                close(bullet_info[P_WR]);
                close(bullet_pos[P_RD]);
                bullet_main(bullet_info[P_RD], bullet_pos[P_WR]);
        }

        close(pacman_ch_pipe[P_WR]);
        close(pacman_info_pipe[P_RD]); 
        close(ghost_ch_pipe[P_WR]);
        close(ghost_info_pipe[P_RD]);
        close(cmd_pipe[P_WR]);
        close(log_pipe[P_WR]);
        close(bullet_info[P_RD]);
        close(bullet_pos[P_WR]);
        ControlPipes pipes = {pacman_ch_pipe[P_RD], 
                            pacman_info_pipe[P_WR], 
                            ghost_ch_pipe[P_RD], 
                            ghost_info_pipe[P_WR], 
                            cmd_pipe[P_RD], 
                            log_pipe[P_RD],
                            bullet_pos[P_RD],
                            bullet_info[P_WR]};

        control_main(pipes, options);
        
        close(pacman_ch_pipe[P_RD]);
        close(pacman_info_pipe[P_WR]);
        close(ghost_ch_pipe[P_RD]);
        close(ghost_info_pipe[P_WR]);
        close(cmd_pipe[P_RD]);
        close(log_pipe[P_RD]);
        close(bullet_pos[P_RD]);
        close(bullet_info[P_WR]);

        getchar();
        kill(p_pacman, 1);
        kill(p_ghosts, 1);
        kill(p_player, 1);
        kill(p_bullet, 1);
    }
    while(menu_choice != MENU_EXIT);

    endwin();
    return 0;
}