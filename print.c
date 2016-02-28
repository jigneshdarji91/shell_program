/*
 * =====================================================================================
 *
 *       Filename:  print.c
 *
 *    Description:  print cmd and pipes
 *
 *        Version:  1.0
 *        Created:  02/27/2016 08:44:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */


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

void prCmd(Cmd c)
{
    int i;

    if ( c ) {
        printf("%s%s ", c->exec == Tamp ? "BG " : "", c->args[0]);
        if ( c->in == Tin )
            printf("<(%s) ", c->infile);
        if ( c->out != Tnil )
            switch ( c->out ) {
                case Tout:
                    printf(">(%s) ", c->outfile);
                    break;
                case Tapp:
                    printf(">>(%s) ", c->outfile);
                    break;
                case ToutErr:
                    printf(">&(%s) ", c->outfile);
                    break;
                case TappErr:
                    printf(">>&(%s) ", c->outfile);
                    break;
                case Tpipe:
                    printf("| ");
                    break;
                case TpipeErr:
                    printf("|& ");
                    break;
                default:
                    fprintf(stderr, "Shouldn't get here\n");
                    exit(-1);
            }

        if ( c->nargs > 1 ) {
            printf("[");
            for ( i = 1; c->args[i] != NULL; i++ )
                printf("%d:%s,", i, c->args[i]);
            printf("\b]");
        }
        putchar('\n');
        // this driver understands one command
        if ( !strcmp(c->args[0], "end") )
            exit(0);
    }
}

void prPipe(Pipe p)
{
    int i = 0;
    Cmd c;

    if ( p == NULL )
        return;

    printf("Begin pipe%s\n", p->type == Pout ? "" : " Error");
    for ( c = p->head; c != NULL; c = c->next ) {
        printf("  Cmd #%d: ", ++i);
        prCmd(c);
    }
    printf("End pipe\n");
    prPipe(p->next);
}

