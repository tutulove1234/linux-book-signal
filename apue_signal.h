#ifndef 	APUE_SIGNAL_H
#define 	APUE_SIGNAL_H

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <stat.h>

/*
 * Declare some functions.
 *
 */

// 显示信号集
void show_sig_set () ;
// 显示被屏蔽的信号
void show_sig_proc () ;
// 显示未决信号集
void show_sig_pending () ;

#endif

