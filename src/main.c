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
#include "utils.h"
#include "player.h"
#include "ghost.h"

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

    refresh();
}

int main()
{
    int i;
    int mode;
    pid_t p_player, p_pacman, p_ghosts;
    int pacman_ch_pipe[2], pacman_info_pipe[2], ghost_ch_pipe[2], ghost_info_pipe[2], pacman_cmd_pipe[2], cmd_pipe[2], log_pipe[2];

    init();

    mode = main_menu();

    print_map();
    print_pellets();
    refresh();
    
    if(pipe(pacman_ch_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(pacman_info_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(ghost_ch_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(ghost_info_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(pacman_cmd_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(cmd_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(log_pipe) == -1)
        _exit(PIPE_ERROR);

    fcntl(pacman_ch_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(pacman_ch_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(pacman_info_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(pacman_info_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(ghost_ch_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(ghost_ch_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(pacman_cmd_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(pacman_cmd_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(cmd_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(cmd_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(ghost_info_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(ghost_info_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(log_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(log_pipe[1], F_SETFL, O_NONBLOCK);

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
            close(pacman_cmd_pipe[P_WR]);
            close(cmd_pipe[P_RD]);
            close(cmd_pipe[P_WR]);
            close(log_pipe[P_RD]);
            pacman_main(pacman_cmd_pipe[P_RD], pacman_info_pipe[P_RD], pacman_ch_pipe[P_WR], log_pipe[P_WR]);
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
            close(pacman_cmd_pipe[P_RD]);
            close(pacman_cmd_pipe[P_WR]);
            close(cmd_pipe[P_RD]);
            close(cmd_pipe[P_WR]);
            close(log_pipe[P_RD]);
            ghost_main(ghost_info_pipe[P_RD], ghost_ch_pipe[P_WR], log_pipe[P_WR]);
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
            close(pacman_cmd_pipe[P_RD]);
            close(pacman_cmd_pipe[P_WR]);
            close(cmd_pipe[P_RD]);
            close(log_pipe[P_RD]);
            player_main(cmd_pipe[P_WR], log_pipe[P_WR]);
    }

    close(pacman_ch_pipe[P_WR]);
    close(pacman_info_pipe[P_RD]); 
    close(ghost_ch_pipe[P_WR]);
    close(ghost_info_pipe[P_RD]);  
    close(pacman_cmd_pipe[P_RD]);
    close(cmd_pipe[P_WR]);
    close(log_pipe[P_WR]);
    ControlPipes pipes = {pacman_ch_pipe[P_RD], pacman_info_pipe[P_WR], ghost_ch_pipe[P_RD], ghost_info_pipe[P_WR], cmd_pipe[P_RD], pacman_cmd_pipe[P_WR], log_pipe[P_RD]};
    control_main(pipes);
    
    getchar();
    kill(p_pacman, 1);
    kill(p_ghosts, 1);
    kill(p_player, 1);
    endwin();
    return 0;
}