/*
 * 	This file is used to print process's signals which is set or proc .
 *  Just print the prev 32 signals .
 */

#ifndef 	_GNU_SOURCE
#define 	_GNU_SOURCE

#include "apue_signal.h"

const char *signame[] = {"","SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGABRT","SIGBUS","SIGFPE",\
							"SIGKILL","SIGUSR1","SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM","SIGTERM","SIGSTKFLT",\
							"SIGCHLD","SIGCONT","SIGSTOP","SIGTSTP","SIGTTIN","SIGTTOU","SIGURG","SIGXCPU",\
							"SIGXFSZ","SIGVTALRM","SIGPROF","SIGWINCH","SIGIO","SIGPWR","SIGSYS","SIGRTMIN"} ;

// Print sig_set passed in .
static inline void print_sigset ( sigset_t sigset ) {
	
	unsigned long set = (long)sigset ;

	unsigned long i = 0 ; 

	for ( ; i < 32 ; i ++ ) {
		if ( set & (1UL << i )) {
			printf ("%s\n" , signame[i]) ;
		}
	}
}

// Print process sigset .
void show_sig_set () {
	
	sigset_t 	old_set ;

	sigemptyset (&old_set ) ;

	if ( 0 > sigprocmask ( SIG_SETMASK , NULL , &old_set )) {
		fprintf (stderr , "sigprocmask error LINE %lu , in func %s " , __LINE__ , __func__) , perror ("") ;
		exit (1) ;
	}
 
	print_sigset ( old_set ) ;
}

// Print proc sigset .
void show_sig_proc () {
	sigset_t 	old_set ;

	sigemptyset (&old_set ) ;

	if ( 0 > sigprocmask ( SIG_SETMASK , NULL , &old_set )) {
		fprintf (stderr , "sigprocmask error LINE %lu , in func %s " , __LINE__ , __func__) , perror ("") ;
		exit (1) ;
	}
 
	print_sigset ( old_set ) ;
}

// Print pending sigset .
void show_sig_pending () {
	sigset_t 	pending_sig ;

	if ( 0 > sigpending (&pending_sig) ) {
		fprintf (stderr , "sigpending error LINE %lu , in func %s " , __LINE__ , __func__) , perror ("") ;
		exit (1) ;
	}

	print_sigset ( pending_sig ) ;
}

#endif
