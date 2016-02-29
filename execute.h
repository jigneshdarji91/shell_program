/*
 * =====================================================================================
 *
 *       Filename:  execute.h
 *
 *    Description:  execute pipes
 *
 *        Version:  1.0
 *        Created:  02/25/2016 09:02:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */
#include "parse.h"

static pid_t shell_pid;
static pid_t shell_pgid;

void disable_signal();
void enable_signal();
void init_shell();
void exec_pipe(Pipe*);
void exec_file(char*);
void setup_pipes(Cmd*, int*, int*, int*, int*);
int  is_pipe_fg(Pipe*);
