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
#include "execute.h"
#include "print.h"

int main(int argc, char *argv[])
{
    Pipe        p;
    Pipe        stack;
    int         first_run = 1;
    char        host[128];

    host[127] = '\0';
    gethostname(host, 127);

    init_shell();

    while ( 1 ) {
        printf("%s%% ", host);
        if(first_run)
        {
            exec_file(".ushrc");
            first_run = 0;
        }
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
