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
    char        ushrc_file[128];

    sprintf(ushrc_file, "%s/.ushrc", getenv("HOME"));
    log_dbg("file: %s", ushrc_file);

    host[127] = '\0';
    gethostname(host, 127);

    init_shell();

    while ( 1 ) {
        if(first_run == 1)
        {
            exec_file(ushrc_file); //FIXME: exec ushrc
            first_run = 0;
        }
        else
        {
            printf("%s%% ", host);
            fflush(stdout);
            p = parse();
            Pipe p_exec = p;
            while(p_exec != NULL)
            {
                exec_pipe(&p_exec);
                p_exec = p_exec->next;
            }
            freePipe(p);
        }
    }
}

/*........................ end of main.c ....................................*/
