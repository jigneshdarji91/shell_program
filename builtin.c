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
        builtin_logout(c);
    }
    if(!strcmp((*c)->args[0], "cd"))
    {
        builtin_cd(c);
    }
    if(!strcmp((*c)->args[0], "echo"))
    {
        builtin_echo(c);
    }
    if(!strcmp((*c)->args[0], "nice"))
    {
        builtin_nice(c);
    }
    if(!strcmp((*c)->args[0], "pwd"))
    {
        builtin_pwd(c);
    }
    if(!strcmp((*c)->args[0], "setenv"))
    {
        builtin_setenv(c);
    }
    if(!strcmp((*c)->args[0], "unsetenv"))
    {
        builtin_unsetenv(c);
    }
    if(!strcmp((*c)->args[0], "where"))
    {
        builtin_where(c);
    }

}

int builtin_logout(Cmd* c)
{
    log_dbg();
    exit(0);  //FIXME: need for graceful exit?
}

int builtin_cd(Cmd* c)
{

}
int builtin_echo(Cmd* c)
{

}
int builtin_nice(Cmd* c)
{

}
int builtin_pwd(Cmd* c)
{

}
int builtin_setenv(Cmd* c)
{

}
int builtin_unsetenv(Cmd* c)
{

}
int builtin_where(Cmd* c)
{

}
