#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "pacman.h" 
#include "control.h"
#include "utils.h"
#include "player.h"
#include "ghost.h"

void init()
{
    initscr();
    noecho(); 
    srand(time(NULL));
    curs_set(0);
    start_color(); 

    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_BLUE, 0, 0, 1000);
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_color(COLOR_RED, 1000, 0, 0);
    init_color(COLOR_CYAN, 0, 1000, 1000);
    init_color(COLOR_MAGENTA, 1000, 700, 1000);
    init_color(COLOR_GREEN, 1000, 700, 300);
    init_color(8, 1000, 800, 800);

    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, 8, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    init_pair(5, COLOR_BLACK, COLOR_RED);
    init_pair(6, COLOR_BLACK, COLOR_CYAN);
    init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(8, COLOR_BLACK, COLOR_GREEN);
    init_pair(9, COLOR_WHITE, COLOR_BLUE);

    refresh();
    print_map();
    print_pellets();
    refresh();
}

int main()
{
    int i;
    pid_t p_player, p_pacman, p_ghosts;
    int pacman_ch_pipe[2], ghost_ch_pipe[2], ghost_info_pipe[2], cmd_pipe[2], log_pipe[2];

    init();

    if(pipe(pacman_ch_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(ghost_ch_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(ghost_info_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(cmd_pipe) == -1)
        _exit(PIPE_ERROR);
    if(pipe(log_pipe) == -1)
        _exit(PIPE_ERROR);

    fcntl(pacman_ch_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(pacman_ch_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(ghost_ch_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(ghost_ch_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(cmd_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(cmd_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(ghost_info_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(ghost_info_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(log_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(log_pipe[1], F_SETFL, O_NONBLOCK);

    switch(p_pacman = fork())
    {
        case -1:
            _exit(FORK_ERROR | PACMAN_ID);
        case 0: //pacman
            close(pacman_ch_pipe[P_RD]);      
            close(ghost_ch_pipe[P_RD]);      
            close(ghost_ch_pipe[P_WR]);      
            close(ghost_info_pipe[P_RD]);
            close(ghost_info_pipe[P_WR]);
            close(cmd_pipe[P_WR]);
            close(log_pipe[P_RD]);
            pacman_main(cmd_pipe[P_RD], pacman_ch_pipe[P_WR], log_pipe[P_WR]);
    }   

    switch(p_ghosts = fork())
    {
        case -1:
            _exit(FORK_ERROR | GHOST_ID);
        case 0:
            close(pacman_ch_pipe[P_RD]);      
            close(pacman_ch_pipe[P_WR]);      
            close(ghost_ch_pipe[P_RD]);      
            close(ghost_info_pipe[P_WR]);
            close(cmd_pipe[P_RD]);
            close(cmd_pipe[P_WR]);
            close(log_pipe[P_RD]);
            ghost_main(ghost_info_pipe[P_RD], ghost_ch_pipe[P_WR], log_pipe[P_WR]);
    }

    switch(p_player = fork())
    {
        case -1:
            _exit(FORK_ERROR | PLAYER_ID);
        case 0:
            close(pacman_ch_pipe[P_RD]);      
            close(pacman_ch_pipe[P_WR]);  
            close(ghost_ch_pipe[P_RD]);      
            close(ghost_ch_pipe[P_WR]);      
            close(ghost_info_pipe[P_RD]);
            close(ghost_info_pipe[P_WR]);
            close(cmd_pipe[P_RD]);
            close(log_pipe[P_RD]);
            player_main(cmd_pipe[P_WR], log_pipe[P_WR]);
    }

    close(pacman_ch_pipe[P_WR]);  
    close(ghost_ch_pipe[P_WR]);
    close(ghost_info_pipe[P_RD]);  
    close(cmd_pipe[P_RD]);
    close(cmd_pipe[P_WR]);
    close(log_pipe[P_WR]);
    control_main(pacman_ch_pipe[P_RD], ghost_ch_pipe[P_RD], ghost_info_pipe[P_WR], log_pipe[P_RD]);
    
    kill(p_pacman, 1);
    kill(p_ghosts, 1);
    kill(p_player, 1);
    endwin();
    return 0;
}