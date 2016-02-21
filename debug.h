/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  debug utilities based on http://c.learncodethehardway.org/book/ex20.html
 *
 *        Version:  1.0
 *        Created:  01/19/2016 03:33:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jignesh Darji (jignesh), jndarji@ncsu.edu
 *   Organization:  North Carolina State University
 *
 * =====================================================================================
 */


#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define DEBUGGING_MODE 1 
#define log_err(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen("logs.txt", "a"); do{ fprintf(_log, "[ERR] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
#define log_inf(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen("logs.txt", "a"); do{ fprintf(_log, "[INF] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
#define log_wrn(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen("logs.txt", "a"); do{ fprintf(_log, "[WRN] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
#define log_dbg(M, ...) if(DEBUGGING_MODE){ FILE* _log = fopen("logs.txt", "a"); do{ fprintf(_log, "[DBG] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__); } while(0); fclose(_log);}
//#define log_wrn(M, ...) if(DEBUGGING_MODE){ int _log = open("logs.txt", O_WRONLY | O_APPEND | O_CREAT); do { fprintf(stdout, "[WRN] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__);} while(0)}
//#define log_inf(M, ...) if(DEBUGGING_MODE){ int _log = open("logs.txt", O_WRONLY | O_APPEND | O_CREAT); do { fprintf(stdout, "[INF] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__);} while(0)}
//#define log_dbg(M, ...) if(DEBUGGING_MODE){ int _log = open("logs.txt", O_WRONLY | O_APPEND | O_CREAT); do { fprintf(stdout, "[DBG] %s | %s | %-15s | " M "\n", __DATE__, __TIME__, __FUNCTION__, ##__VA_ARGS__);} while(0)}


#endif
