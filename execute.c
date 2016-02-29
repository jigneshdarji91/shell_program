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
#include "job_control.h"
#include "print.h"
#include <sys/resource.h>
#include <termios.h>

pid_t           shell_pid  = 0;;
pid_t           shell_pgid = 0;;
int             shell_fd   = 0;;
struct termios  shell_terminal_modes;

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
    shell_fd    = STDIN_FILENO;
    disable_signal();
    setpgid(shell_pid, shell_pgid);
    log_inf("tcsetpgrp pgid: %d", shell_pgid);
    tcsetpgrp(shell_fd, shell_pgid);
    tcgetattr(shell_fd, &shell_terminal_modes);
}

void setup_pipes(Cmd* c, int* pipes, int* fd_in, int* fd_out, int* fd_err)
{
    fflush(stdin);
    fflush(stdout);
    fflush(stderr);
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
            *fd_out = open((*c)->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
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
            *fd_err = open((*c)->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
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
    log_inf("end fd_in: %d fd_out:%d fd_err: %d", *fd_in, *fd_out, *fd_err);
}


void exec_pipe(Pipe *p)
{
    log_inf("begin pgid: %d", (*p)->pgid);
    Cmd     c;
    pid_t   pid;
    int     pipes[2];
    int     fd_in = 0, fd_out = 1, fd_err = 2;
    int     old_fd_in = dup(0);
    int     old_fd_out = dup(1);
    int     old_fd_err = dup(2);
    int     nice_flag = 0;
    long int niceval    = 0;
    long int niceold    = 0;

    (*p)->fg = is_pipe_fg(p);

    for(c = (*p)->head; c; c = c->next)
    {
        setup_pipes(&c, pipes, &fd_in, &fd_out, &fd_err);
        if(!strcmp(c->args[0], "nice"))
        {
            builtin_nice(&c, &niceval);
            nice_flag = 1;
        }

        if(is_builtin(c->args[0]) && c->next == NULL)
        {
            //builtin to be executed in the shell
            //if it's last in the pipeline(or alone)

            if(nice_flag)
            {
                //set priority
                niceold     =  getpriority(PRIO_PROCESS, 0);
                setpriority(PRIO_PROCESS, 0, niceval);
            }

            if(fd_in != 0) 
            {
                fflush(stdin);
                dup2(fd_in, 0);
                close(fd_in);
            }
            if(fd_out != 1) 
            {
                fflush(stdout);
                dup2(fd_out, 1);
                close(fd_out);
            }
            if(fd_err != 2)
            {
                fflush(stderr);
                dup2(fd_err, 2);
                close(fd_err);
            }

            int retVal = exec_builtin(&c);
            log_inf("cmd: %s retVal: %d", c->args[0], retVal);

            if(fd_in != 0)
            {
                fflush(stdin);
                close(fd_in);
                dup2(old_fd_in, 0);
                close(old_fd_in);
            }
            if(fd_out != 1)
            {
                fflush(stdout);
                close(fd_out);
                dup2(old_fd_out, 1);
                close(old_fd_out);
            }
            if(fd_err != 2)
            {
                fflush(stderr);
                close(fd_err);
                dup2(old_fd_err, 2);
                close(old_fd_err);
            }
            //fd_in = pipes[0];
            log_inf("fd_in: %d fd_out:%d fd_err: %d", fd_in, fd_out, fd_err);
            if(nice_flag)
            {
                //reset priority
                setpriority(PRIO_PROCESS, 0, niceold);
            }
            break;
        }

        pid = fork();
        if(pid > 0)
        {
            log_dbg("parent");
            c->pid = pid;
            if((*p)->pgid == 0)
                (*p)->pgid = pid;
            setpgid(pid, (*p)->pgid);
        }
        else if(pid == 0)
        {
            log_dbg("child begin cmd: %s pid: %d", c->args[0], getpid());
            if(nice_flag)
            {
                //set priority
                setpriority(PRIO_PROCESS, 0, niceval);
            }
            pid_t cpid = getpid();
            pid_t ppid = getppid();
            if((*p)->pgid == 0)
                (*p)->pgid = cpid;
            setpgid(cpid, (*p)->pgid);
            if((*p)->fg)
            {
                log_inf("tcsetpgrp pgid: %d", (*p)->pgid);
                tcsetpgrp(shell_fd, (*p)->pgid);
            }
            else
            {
                log_inf("tcsetpgrp pgid: %d", shell_pgid);
                tcsetpgrp(shell_fd, shell_pgid);
            }
            enable_signal();

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
                exit(0);
            }
            else 
            {
                log_inf("not a builtin");
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

    if((*p)->fg)
        run_in_fg(p, 0);
    else    
        run_in_bg(p, 0);
    log_inf("end");
}

void exec_file(char* filename)
{
    log_dbg("begin filename: %s", filename);

    Pipe    p;
    int     saved_fd_in  = dup(STDIN_FILENO);
    int     saved_fd_out = dup(STDOUT_FILENO);

    FILE*   f = fopen(filename, "r");
    if(f == NULL)
    {
        log_err("failed to open file: %s", filename);
        return;
    }

    int     line_count = 0, ch;
    while ( (ch=fgetc(f)) != EOF ) {
        if ( ch == '\n' )
            line_count++;
    }
    fseek(f, 0, SEEK_SET);

    log_dbg("ushrc lines: %d", line_count);
    dup2(fileno(f), STDIN_FILENO);
    close(fileno(f));

    while(line_count > 0)
    {
        p = parse();
        Pipe p_exec = p;
        while(p_exec != NULL)
        {
            exec_pipe(&p_exec);
            p_exec = p_exec->next;
        }
        freePipe(p);
        line_count--;
    }

    dup2(saved_fd_in, STDIN_FILENO);
    close(saved_fd_in);
    fclose(f);
    log_dbg("end");
}

int is_pipe_fg(Pipe *p)
{
    if(p == NULL)
        return 0;
    int fg = 0;
    Cmd c = (*p)->head;
    if(c == NULL)
        return 0;
    while(c->next) 
        c = c->next;
    if(c != NULL && c->exec != Tamp)
        fg = 1;
    return fg;
}

