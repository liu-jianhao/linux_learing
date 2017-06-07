/** sol07.13.c
 ** ------------------------------------------------------------
	A program for autologout is
	sol07.13.c.
	This program uses sleep and kill.  If a filename argument
	is given, the program waits until that file has not been
	modified for the specified number of seconds before sending
	SIGKILL to the specified process.  That second feature 
	requires using stat() to find the modification time of
	the file and computing the sleep interval based on that
	time and the current time.  

	The program could be done with a polling loop that kept
	checking the modification time of the file, but that would
	waste CPU time.

	The program could be called with the command line:
	timeout $$ 3600 `tty` &
	to pass it the process id of the current shell and the
	name of the terminal file for the current shell.

	By allowing any file name as the last argument, the program
	is more flexible than a simple autologout tool.  If the
	signal were other than SIGKILL, the program could be used
	to notify a process when a file was no longer changing.

 ** ------------------------------------------------------------
 **
 **
 *  timeout.c - sends SIGKILL to a process after a certain inteval
 *              Also, if a terminal name is given as an argument,
 *              does not kill the process until that terminal has
 *              no modifications for the interval specified.
 *
 *              Can be used to logout a terminal line after a period
 *              of inactivity. Also can be used to kill a process
 *              after any file is unchanged for a duration.  There
 *              are probably some useful applications for that.
 *
 *  usage: timeout pid duration [filename]
 *
 *  build: cc sol07.13.c -o sol07.13
 */

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<signal.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<time.h>

#define oops(m,x)  { perror(m); exit(x); }

time_t lastmod(char *fname);

int main(int ac, char *av[])
{
	int	timeout;	/* kill after this duration	   */
	pid_t	pid;		
	char	*file = NULL ;

	/*
	 * setup
	 */
	if ( ac < 3 ){
		fprintf(stderr,"usage: timeout pid duration [file]\n");
		exit(1);
	}
	pid     = (pid_t) atoi(av[1]);
	timeout = atoi(av[2]);
	if ( ac == 4 )
		file = av[3];

	/*
	 * wait until the time elapses.  In the basic case,
	 * just sleep and then proceed to the kill code.
	 * In the case of watching a file (e.g. /dev/ttyXX)
	 * we may need to loop
	 */

	if ( file == NULL ){
		sleep(timeout);
	}
	else {
		time_t	now;		/* to figure out sleeptime */
		int	sleeptime;

		do {
			time(&now);
			sleeptime = (lastmod(file) + timeout) - now;
			if ( sleeptime > 0 )
				sleep(sleeptime);
		} 
		while( sleeptime > 0 );
	}

	/*
	 * now we kill the process
	 */
	kill(pid, SIGKILL);		//超过时间就kill 
	return 0;
}

/*
 * returns last mod time for the named file
 */

time_t
lastmod(char *fname)
{
	struct stat info;

	if ( stat(fname,&info) == -1 )
		oops(fname,2);

	return info.st_mtime;
}
