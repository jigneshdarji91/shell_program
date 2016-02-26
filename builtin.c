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

int is_builtin(char* cmd)
{
    if(!(strcmp(cmd, "cd")
        && strcmp(cmd, "echo")
        && strcmp(cmd, "logout")
        && strcmp(cmd, "nice")
        && strcmp(cmd, "pwd")
        && strcmp(cmd, "setenv")
        && strcmp(cmd, "unsetenv")
        && strcmp(cmd, "where")))
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
    log_inf("CWD: %s", cwd);
    return retVal; //FIXME: validations and error messages
}
int builtin_echo(Cmd* c)
{
    int i;
    for(i = 1; i < (*c)->nargs; i++)
    {
        fprintf(stdout, "%s ", (*c)->args[i]);
    }
    fprintf(stdout, "\n");
    return 1; //FIXME: validations
}
int builtin_nice(Cmd* c)
{

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
    return setenv((*c)->args[1], (*c)->args[2], 1); //FIXME: validations
}
int builtin_unsetenv(Cmd* c)
{
    log_dbg();
    return unsetenv((*c)->args[1]); //FIXME: validations
}
int builtin_where(Cmd* c)
{

}
