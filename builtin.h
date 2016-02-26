/*
 * =====================================================================================
 *
 *       Filename:  builtin.h
 *
 *    Description:  builtin functions for the shell
 *
 *        Version:  1.0
 *        Created:  02/25/2016 09:43:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#include "parse.h"

int is_builtin(char* cmd);
int exec_builtin(Cmd *p);
int builtin_logout(Cmd* c);
int builtin_cd(Cmd* c);
int builtin_echo(Cmd* c);
int builtin_nice(Cmd* c);
int builtin_pwd(Cmd* c);
int builtin_setenv(Cmd* c);
int builtin_unsetenv(Cmd* c);
int builtin_where(Cmd* c);
