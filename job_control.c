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

void poll_pipe_status(Pipe *p)
{
    int status      =   0;
    pid_t   cmd_pid  =   0;

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

