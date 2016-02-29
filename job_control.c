/*
 * =====================================================================================
 *
 *       Filename:  job_control.c
 *
 *    Description:  Job control book keeping 
 *
 *        Version:  1.0
 *        Created:  02/26/2016 04:45:17 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/wait.h>
#include "job_control.h"
#include "debug.h"
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "execute.h"

//extern struct termios shell_terminal_modes;
//extern pid_t shell_pgid;
//extern pid_t shell_pid;
//extern int   shell_fd;

void poll_pipe_status(Pipe *p)
{
    int     status      =   0;
    pid_t   cmd_pid     =   0;

    do {
        cmd_pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    } while(!update_pipe_status(p, cmd_pid, status));
}

int update_pipe_status(Pipe *first_pipe, pid_t pid, int status)
{
    Cmd c;
    Pipe p;
    if(pid <= 0)
    {
        log_dbg("nobody seems to be waiting");
        return -1;
    }
    for(p = *first_pipe; p; p = p->next)
    {
        for(c = p->head; c; c = c->next)
        {
            if(c->pid != pid)
            {
                continue;
            }
            
            if(WIFSTOPPED(status))
                c->paused = 1;
            else 
                c->completed = 1;
            return 0;
        }
    }
}

pid_t run_stopped()
{
    int     status  =   0;
    pid_t   pid     =   0;
    pid_t   pgid    = 0;
    do {
        pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
        log_inf("starting pid: %d", pid);
        if(pid > 0 
                && !WIFSTOPPED(status))
        {
            pgid = getpgid(pid);
            continue_pgrp(pid);
            break;
        }
    } while(pid > 0);
    return pgid;
}

void continue_pgrp(pid_t pgid)
{
    log_dbg();
    if (kill (- pgid, SIGCONT) < 0)
        perror ("kill (SIGCONT)");
}

void run_in_fg(Pipe *p, int cont)
{
    log_dbg("begin");
    tcgetattr(shell_fd, &shell_terminal_modes);
    log_inf("tcsetpgrp pgid: %d", (*p)->pgid);
    tcsetpgrp(shell_fd, (*p)->pgid);

    if(cont)
    {
        tcsetattr(shell_fd, TCSADRAIN, &(*p)->terminal_mode);
        if(kill(- (*p)->pgid, SIGCONT) < 0)
            perror("kill (SIGCONT)");
    }
    wait_for_pipe(p);

    log_inf("tcsetpgrp pgid: %d", shell_pgid);
    tcsetpgrp(shell_fd, shell_pgid);
    tcgetattr(shell_fd, &(*p)->terminal_mode);
    tcsetattr(shell_fd, TCSADRAIN, &shell_terminal_modes);
    log_dbg("end");
}

void run_in_bg(Pipe *p, int cont)
{
    if(cont)
        if(kill(- (*p)->pgid, SIGCONT) < 0)
            perror("kill (SIGCONT)");
}

void wait_for_pipe(Pipe *p)
{
    int     status = 0;
    pid_t   cpid;
    waitpid(- (*p)->pgid, &status, WUNTRACED);
    if(0 != status)
    {
        log_err("pipe terminated abnormally status: %d", status);
    }
    else
    {
        log_inf("pipe terminated normally status: %d", status);
    }
    fflush(stdout);
}

