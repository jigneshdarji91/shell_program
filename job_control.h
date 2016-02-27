/*
 * =====================================================================================
 *
 *       Filename:  job_control.h
 *
 *    Description:  job control and book keeping
 *
 *        Version:  1.0
 *        Created:  02/26/2016 05:30:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */

#ifndef __JOB_CONTROL__
#define __JOB_CONTROL__

#include "parse.h"

void poll_pipe_status(Pipe *p);
int update_pipe_status(Pipe *p, pid_t pid, int status);
pid_t run_stopped();
void continue_pgrp(pid_t);


#endif /* __JOB_CONTROL__ */
