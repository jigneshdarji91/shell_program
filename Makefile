# Makefile for ush
#
# Vincent W. Freeh
# 

CC=gcc
CFLAGS=-g
SRC=main.c parse.c parse.h execute.c execute.h builtin.c builtin.h job_control.c job_control.h
OBJ=main.o parse.o builtin.o execute.o job_control.o 

ush:	$(OBJ)
	$(CC) -o $@ $(OBJ)

tar:
	tar czvf ush.tar.gz $(SRC) Makefile README

clean:
	\rm $(OBJ) ush

ush.1.ps:	ush.1
	groff -man -T ps ush.1 > ush.1.ps
