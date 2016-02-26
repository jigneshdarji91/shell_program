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
#include <string.h>


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
