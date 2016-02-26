/******************************************************************************
 *
 *  File Name........: main.c
 *
 *  Description......: Simple driver program for ush's parser
 *
 *  Author...........: Vincent W. Freeh
 *
 *****************************************************************************/

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

static pid_t shell_pid;
static pid_t shell_pgid;

static void disable_signal()
{
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
    signal (SIGTTOU, SIG_IGN);
    signal (SIGCHLD, SIG_IGN);
}

static void enable_signal()
{
    signal (SIGINT, SIG_DFL);
    signal (SIGQUIT, SIG_DFL);
    signal (SIGTSTP, SIG_DFL);
    signal (SIGTTIN, SIG_DFL);
    signal (SIGTTOU, SIG_DFL);
    signal (SIGCHLD, SIG_DFL);
}

static void init_shell()
{
    shell_pid = getpid();
    shell_pgid  = shell_pid;
    setpgid(shell_pid, shell_pgid);
    tcsetpgrp(shell_pid, shell_pgid);
    disable_signal();
}

static void exec_pipe(Pipe *p)
{
    log_inf("begin pgid: %d", (*p)->pgid);
    Cmd     c;
    pid_t   pid;
    int     pipes[2];
    int     fd_in = 0, fd_out, fd_err = 2;


    for(c = (*p)->head; c; c = c->next)
    {
        if(Tin == c->in)
            fd_in = open(c->infile, O_RDONLY);
        if(c->next)
        {
            log_dbg("setting up pipe for %s", c->args[0]);
            if(0 > pipe(pipes))
            {
                perror("pipe creation failed");
                exit(1);
            }
            fd_out = pipes[1];
            if(TpipeErr == c->out)
                fd_err = pipes[1];
        }
        else
        {
            fd_out = 1;
            fd_err = 2;
            if(Tout == c->out)
            {
                log_dbg("write");
                fd_out = open(c->outfile, O_WRONLY | O_CREAT, 0666);
                if(fd_out < 0)
                    log_err("could not open fd_out: %s", c->outfile);
            }
            if(Tapp == c->out)
            {
                log_dbg("append to %s", c->outfile);
                fd_out = open(c->outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
                if(fd_out < 0)
                    log_err("could not open fd_out: %s", c->outfile);
            }
            if(ToutErr == c->out)
            {
                log_dbg("out+err write");
                fd_err = open(c->outfile, O_WRONLY | O_CREAT, 0666);
                if(fd_err < 0)
                    log_err("could not open fd_err: %s", c->outfile);
            }
            if(TappErr == c->out)
            {
                log_dbg("out+err: append to %s", c->outfile);
                fd_err = open(c->outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
                if(fd_err < 0)
                    log_err("could not open fd_err: %s", c->outfile);
            }
        }

        pid = fork();
        if(0 > pid)
        {
            log_dbg("error");
            perror("fork failed");
            exit(1);
        }
        else if(0 < pid)
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
        else
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

            execvp(c->args[0], c->args);
            perror("execvp");
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

static void prCmd(Cmd c)
{
    int i;

    if ( c ) {
        printf("%s%s ", c->exec == Tamp ? "BG " : "", c->args[0]);
        if ( c->in == Tin )
            printf("<(%s) ", c->infile);
        if ( c->out != Tnil )
            switch ( c->out ) {
                case Tout:
                    printf(">(%s) ", c->outfile);
                    break;
                case Tapp:
                    printf(">>(%s) ", c->outfile);
                    break;
                case ToutErr:
                    printf(">&(%s) ", c->outfile);
                    break;
                case TappErr:
                    printf(">>&(%s) ", c->outfile);
                    break;
                case Tpipe:
                    printf("| ");
                    break;
                case TpipeErr:
                    printf("|& ");
                    break;
                default:
                    fprintf(stderr, "Shouldn't get here\n");
                    exit(-1);
            }

        if ( c->nargs > 1 ) {
            printf("[");
            for ( i = 1; c->args[i] != NULL; i++ )
                printf("%d:%s,", i, c->args[i]);
            printf("\b]");
        }
        putchar('\n');
        // this driver understands one command
        if ( !strcmp(c->args[0], "end") )
            exit(0);
    }
}

static void prPipe(Pipe p)
{
    int i = 0;
    Cmd c;

    if ( p == NULL )
        return;

    printf("Begin pipe%s\n", p->type == Pout ? "" : " Error");
    for ( c = p->head; c != NULL; c = c->next ) {
        printf("  Cmd #%d: ", ++i);
        prCmd(c);
    }
    printf("End pipe\n");
    prPipe(p->next);
}

int main(int argc, char *argv[])
{
    Pipe p;
    char host[128];
    host[127] = '\0';
    getlogin_r(host, 127);

    init_shell();

    while ( 1 ) {
        printf("%s%% ", host);
        p = parse();
        prPipe(p);
        Pipe p_exec = p;
        while(p_exec != NULL)
        {
            exec_pipe(&p_exec);
            p_exec = p_exec->next;
        }
        freePipe(p);
    }
}

/*........................ end of main.c ....................................*/
