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
#include "print.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "job_control.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define DEFAULT_NICE 4

int is_builtin(char* cmd)
{
    if(!(strcmp(cmd, "cd")
        && strcmp(cmd, "echo")
        && strcmp(cmd, "logout")
        && strcmp(cmd, "end")
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
    int retVal = 0;
    if(!strcmp((*c)->args[0], "logout"))
    {
        retVal = builtin_logout(c);
    }
    if(!strcmp((*c)->args[0], "end"))
    {
        retVal = 0;
        //exit(0);  
    }
    if(!strcmp((*c)->args[0], "cd"))
    {
        retVal = builtin_cd(c);
    }
    if(!strcmp((*c)->args[0], "echo"))
    {
        retVal = builtin_echo(c);
    }
    if(!strcmp((*c)->args[0], "pwd"))
    {
        retVal = builtin_pwd(c);
    }
    if(!strcmp((*c)->args[0], "setenv"))
    {
        retVal = builtin_setenv(c);
    }
    if(!strcmp((*c)->args[0], "unsetenv"))
    {
        retVal = builtin_unsetenv(c);
    }
    if(!strcmp((*c)->args[0], "where"))
    {
        retVal = builtin_where(c);
    }
    if(!strcmp((*c)->args[0], "kill"))
    {
        retVal = builtin_kill(c);
    }
    if(!strcmp((*c)->args[0], "fg"))
    {
        retVal = builtin_fg(c);
    }
    if(!strcmp((*c)->args[0], "bg"))
    {
        retVal = builtin_bg(c);
    }

    return retVal;
}

int builtin_logout(Cmd* c)
{
    log_dbg();
    exit(0);  
}

int builtin_cd(Cmd* c)
{
    int retVal = 0;
    log_dbg("begin nargs: %d", (*c)->nargs);
    if((*c)->nargs == 2)
    {
        retVal = chdir((*c)->args[1]);
        char cwd[1024];
        getcwd(cwd, 1024);
        log_dbg("CWD: %s", cwd);
    }
    else if((*c)->nargs == 1)
    {
        char *home;
        home = getenv("HOME");
        retVal = chdir(home);
        char cwd[1024];
        getcwd(cwd, 1024);
        log_dbg("CWD: %s HOME: %s", cwd, home);
    }

    return retVal;
}

int builtin_echo(Cmd* c)
{
    int i;
    for(i = 1; i < (*c)->nargs; i++)
    {
        if((*c)->args[i][0] == '$')
        {
            char cmd_str[1024];
            char *env_value;
            if(strlen((*c)->args[i]) <= 1)
            {
                continue;
            }
            strncpy(cmd_str, (*c)->args[i] + 1, strlen((*c)->args[i]) - 1);
            env_value = getenv(cmd_str);
            if(env_value)
                fprintf(stdout, "%s ", env_value);
        }
        else
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
    if((*c)->nargs == 2)
        return setenv((*c)->args[1], (*c)->args[2], 1);
    else
        return setenv((*c)->args[1], (char *)" ", 1); 
}

int builtin_unsetenv(Cmd* c)
{
    log_dbg();
    return unsetenv((*c)->args[1]); //FIXME: validations
}

int builtin_where(Cmd* c)
{
    if((*c)->nargs > 1)
    {
        if(is_builtin((*c)->args[1]))
        {
            printf("%s is a shell built-in\n", (*c)->args[1]);
        }

        char path[1024];
        struct stat st;
        strcpy(path, getenv("PATH"));
        char* path_split    = strtok(path, ":");
        char cmd_path[1024];
        while(path_split != NULL)
        {
            strcpy(cmd_path, path_split);
            strcat(cmd_path, "/");
            strcat(cmd_path, (*c)->args[1]);
            path_split = strtok(NULL, ":");
            if(stat(cmd_path, &st) == 0)
            {
                printf("%s\n", cmd_path);
            }
        }
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

int builtin_nice(Cmd* c, long int* niceval)
{
    int start_pos = 2;
    char* endptr;
    if((*c)->nargs > 0)
    {
        *niceval = strtol((*c)->args[1], &endptr, 10);
        if (*endptr != '\0')
        {
            *niceval = DEFAULT_NICE;
        }
    }
    if(*niceval == DEFAULT_NICE)
    {
        start_pos = 1;       
    }

    log_dbg("nice val: %ld start_pos: %d nargs: %d", *niceval, start_pos, (*c)->nargs);

    int i = 0;
    for(i = 0; i < (*c)->nargs - start_pos; i++)
    {
        strcpy((*c)->args[i], (*c)->args[i + start_pos]);
    }

    for(i = 0; i <= start_pos; i++)
    {
        (*c)->args[(*c)->nargs] = '\0';
        free((*c)->args[(*c)->nargs]);
        ((*c)->nargs)--;
    }
    log_dbg("nice val: %ld start_pos: %d nargs: %d", *niceval, start_pos, (*c)->nargs);
    prCmd(*c);

}

