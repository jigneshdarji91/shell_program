/*
 * =====================================================================================
 *
 *       Filename:  execute.c
 *
 *    Description:  executes pipes
 *
 *        Version:  1.0
 *        Created:  02/25/2016 08:59:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "debug.h"
#include <unistd.h>
#include <string.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "execute.h"
#include "builtin.h"

static pid_t shell_pid = 0;
static pid_t shell_pgid = 0;

void disable_signal()
{
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
    signal (SIGTTOU, SIG_IGN);
    signal (SIGCHLD, SIG_IGN);
}

void enable_signal()
{
    signal (SIGINT, SIG_DFL);
    signal (SIGQUIT, SIG_DFL);
    signal (SIGTSTP, SIG_DFL);
    signal (SIGTTIN, SIG_DFL);
    signal (SIGTTOU, SIG_DFL);
    signal (SIGCHLD, SIG_DFL);
}

void init_shell()
{
    shell_pid = getpid();
    shell_pgid  = shell_pid;
    setpgid(shell_pid, shell_pgid);
    tcsetpgrp(shell_pid, shell_pgid);
    disable_signal();
}

void exec_pipe(Pipe *p)
{
    log_inf("begin pgid: %d", (*p)->pgid);
    Cmd     c;
    pid_t   pid;
    int     pipes[2];
    int     fd_in = 0, fd_out, fd_err = 2;


    for(c = (*p)->head; c; c = c->next)
    {
        setup_pipes(&c, pipes, &fd_in, &fd_out, &fd_err);
        if(is_builtin(c->args[0]) && c->next == NULL)
        {
            //builtin to be executed in the shell
            exec_builtin(&c);
            continue;
        }
        pid = fork();
        if(pid > 0)
        {
            log_dbg("parent");
            c->pid = pid;
            pid_t pgid = getppid();
            if(!(*p)->pgid)
                (*p)->pgid = pgid;
            setpgid(pid, (*p)->pgid);
            tcsetpgrp(shell_pid, shell_pgid);

            //if cmd is FG, wait
            if(Tamp != c->exec)
            {
                log_dbg("child is FG");
                int     status;
                pid_t   cpid;
                waitpid(pid, &status, 0);
                if(0 != status)
                {
                    log_err("process terminated abnormally");
                }
                tcsetpgrp(shell_pid, shell_pgid);
            }
        }
        else if(pid == 0)
        {
            log_dbg("child");
            pid_t cpid = getpid();
            pid_t ppid = getppid();
            enable_signal();
            setpgid(cpid, ppid);
            if(Tamp != c->exec)
            {
                tcsetpgrp(shell_pid, shell_pgid);
            }
            else
            {
                tcsetpgrp(cpid, (*p)->pgid);
            }

            if(fd_in != 0) 
            {
                dup2(fd_in, 0);
                close(fd_in);
            }
            if(fd_out != 1) 
            {
                dup2(fd_out, 1);
                close(fd_out);
            }
            if(fd_err != 2)
            {
                dup2(fd_err, 2);
                close(fd_err);
            }

            if(is_builtin(c->args[0]))
            {
                exec_builtin(&c);
            }
            else 
            {
                execvp(c->args[0], c->args);
                perror("execvp");
                exit(1);
            }
        }
        else
        {
            log_dbg("error");
            perror("fork failed");
            exit(1);
        }

        if(fd_in != 0)
            close(fd_in);
        if(fd_out != 1)
            close(fd_out);
        if(fd_err != 2)
            close(fd_err);
        fd_in = pipes[0];
    }
}

void setup_pipes(Cmd* c,int* pipes, int* fd_in, int* fd_out, int* fd_err)
{
    if(Tin == (*c)->in)
        *fd_in = open((*c)->infile, O_RDONLY);
    if((*c)->next)
    {
        log_dbg("setting up pipe for %s", (*c)->args[0]);
        if(0 > pipe(pipes))
        {
            perror("pipe creation failed");
            exit(1);
        }
        *fd_out = pipes[1];
        if(TpipeErr == (*c)->out)
            *fd_err = pipes[1];
    }
    else
    {
        *fd_out = 1;
        *fd_err = 2;
        if(Tout == (*c)->out)
        {
            log_dbg("write");
            *fd_out = open((*c)->outfile, O_WRONLY | O_CREAT, 0666);
            if(*fd_out < 0)
                log_err("could not open fd_out: %s", (*c)->outfile);
        }
        if(Tapp == (*c)->out)
        {
            log_dbg("append to %s", (*c)->outfile);
            *fd_out = open((*c)->outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if(*fd_out < 0)
                log_err("could not open fd_out: %s", (*c)->outfile);
        }
        if(ToutErr == (*c)->out)
        {
            log_dbg("out+err write");
            *fd_err = open((*c)->outfile, O_WRONLY | O_CREAT, 0666);
            if(*fd_err < 0)
                log_err("could not open fd_err: %s", (*c)->outfile);
        }
        if(TappErr == (*c)->out)
        {
            log_dbg("out+err: append to %s", (*c)->outfile);
            *fd_err = open((*c)->outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if(*fd_err < 0)
                log_err("could not open fd_err: %s", (*c)->outfile);
        }
    }
}

void exec_builtin(Cmd* c)
{
    log_dbg("begin cmd: %s", (*c)->args[0]);

}
