/*
 * =====================================================================================
 *
 *       Filename:  builtin.c
 *
 *    Description:  builtin functions for the shell
 *
 *        Version:  1.0
 *        Created:  02/25/2016 09:47:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#include "builtin.h"
#include "debug.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "job_control.h"
#include <sys/types.h>
#include <sys/wait.h>

int is_builtin(char* cmd)
{
    if(!(strcmp(cmd, "cd")
        && strcmp(cmd, "echo")
        && strcmp(cmd, "logout")
        && strcmp(cmd, "nice")
        && strcmp(cmd, "pwd")
        && strcmp(cmd, "setenv")
        && strcmp(cmd, "unsetenv")
        && strcmp(cmd, "where")
        && strcmp(cmd, "kill")
        && strcmp(cmd, "bg")
        && strcmp(cmd, "fg")
        ))
    {
        return 1;
    }
    return 0;
}

int exec_builtin(Cmd* c)
{
    log_dbg("begin cmd: %s", (*c)->args[0]);
    if(!strcmp((*c)->args[0], "logout"))
    {
        return builtin_logout(c);
    }
    if(!strcmp((*c)->args[0], "cd"))
    {
        return builtin_cd(c);
    }
    if(!strcmp((*c)->args[0], "echo"))
    {
        return builtin_echo(c);
    }
    if(!strcmp((*c)->args[0], "nice"))
    {
        return builtin_nice(c);
    }
    if(!strcmp((*c)->args[0], "pwd"))
    {
        return builtin_pwd(c);
    }
    if(!strcmp((*c)->args[0], "setenv"))
    {
        return builtin_setenv(c);
    }
    if(!strcmp((*c)->args[0], "unsetenv"))
    {
        return builtin_unsetenv(c);
    }
    if(!strcmp((*c)->args[0], "where"))
    {
        return builtin_where(c);
    }
    if(!strcmp((*c)->args[0], "kill"))
    {
        return builtin_kill(c);
    }
    if(!strcmp((*c)->args[0], "fg"))
    {
        return builtin_fg(c);
    }
    if(!strcmp((*c)->args[0], "bg"))
    {
        return builtin_bg(c);
    }

}

int builtin_logout(Cmd* c)
{
    log_dbg();
    exit(0);  //FIXME: need for graceful exit?
}

int builtin_cd(Cmd* c)
{
    int retVal = chdir((*c)->args[1]);
    char cwd[1024];
    getcwd(cwd, 1024);
    log_dbg("CWD: %s", cwd);
    return retVal; //FIXME: validations and error messages
}

int builtin_echo(Cmd* c)
{
    int i;
    for(i = 1; i < (*c)->nargs; i++)
    {
        //TODO: handle env variable
        fprintf(stdout, "%s ", (*c)->args[i]);
    }
    fprintf(stdout, "\n");
    return 1; //FIXME: validations
}

int builtin_pwd(Cmd* c)
{
    char cwd[1024];
    getcwd(cwd, 1024);
    log_inf("CWD: %s", cwd);
    printf("%s\n", cwd);
    return 1; //FIXME: validations and error messages
}

int builtin_setenv(Cmd* c)
{
    log_dbg();
    //FIXME: validations
    if((*c)->nargs == 2)
        return setenv((*c)->args[1], (*c)->args[2], 1);
    else
        return setenv((*c)->args[1], (char *)" ", 1); //FIXME: set the string to blank
}

int builtin_unsetenv(Cmd* c)
{
    log_dbg();
    return unsetenv((*c)->args[1]); //FIXME: validations
}

int builtin_where(Cmd* c)
{
    if((*c)->nargs > 1
        && is_builtin((*c)->args[1]))
    {
        printf("%s is a shell built-in\n", (*c)->args[1]);
    }
}

int builtin_kill(Cmd* c)
{

}

int builtin_fg(Cmd* c)
{
    //steps:
    //1. find child who's in stopped state
    //2. send continue
    //3. wait

    log_dbg();
    pid_t   pid     = run_stopped();
    if(pid > 0)
    {
        int     status = 0;
        pid_t   pgid    = getpgid(pid);
        tcsetpgrp(pid, pgid);
        waitpid(pid, &status, WUNTRACED);
        if(0 != status)
        {
            log_err("process terminated abnormally status: %d", status);
        }
    }
}

int builtin_bg(Cmd* c)
{

}

int builtin_nice(Cmd* c)
{

}

