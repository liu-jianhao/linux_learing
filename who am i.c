/** who am i.c 
 ** ------------------------------------------------------------
	The whoami command prints the name associated with the current user id.
	The who am i command prints the name associated with the 
	current terminal.
	A solution to whoami is: sol02.10b.c .

 ** ------------------------------------------------------------
 *
 * modified version of who2.c that supports "who am i"
 * method: call ttyname to find the path to our terminal, then
 *         only print the entry in utmp for that terminal
 * see:    ttyname() 
 *
 * who2.c  - read /etc/utmp and list info therein
 *         - suppresses empty records
 *         - formats time nicely
 * 
 *	
 */
#include        <stdio.h>
#include        <unistd.h>
#include        <utmp.h>
#include        <fcntl.h>
#include        <time.h>

/* #define      SHOWHOST */

void showtime(long);
void show_info(struct utmp *);
char *whats_my_line(int);

int main(int ac, char *av[])
{
        struct utmp     utbuf;          /* read info into here */
        int             utmpfd;         /* read from this descriptor */
		char		*myline = NULL;		/* used for who am i logic   */

	if ( ac == 3 )
		myline = whats_my_line(0);

        if ( (utmpfd = open(UTMP_FILE, O_RDONLY)) == -1 ){
                perror(UTMP_FILE);
                exit(1);
        }

        while( read(utmpfd, &utbuf, sizeof(utbuf)) == sizeof(utbuf) )
		if ( myline == NULL || strcmp(utbuf.ut_line,myline) == 0 )
			show_info( &utbuf );
        close(utmpfd);
        return 0;
}

/*
 * calls ttyname and truncates the /dev/ part if any
 */
char *
whats_my_line(int fd)			
{
	char	*rv;
								//ttyname()函数返回终端的名字 
	if ( rv = ttyname(0) )		/* if no null trim off /dev prefix */
		if ( strncmp(rv,"/dev/",5) == 0 )		//比较前五个字符并向后移动得到终端名 
			rv += 5;
	return rv;
}
/*
 *      show info()
 *                      displays the contents of the utmp struct
 *                      in human readable form
 *                      * displays nothing if record has no user name
 */
void show_info( struct utmp *utbufp )
{
        if ( utbufp->ut_type != USER_PROCESS )
                return;

        printf("%-8.8s", utbufp->ut_name);      /* the logname  */
        printf(" ");                            /* a space      */
        printf("%-8.8s", utbufp->ut_line);      /* the tty      */
        printf(" ");                            /* a space      */
        showtime( utbufp->ut_time );            /* display time */
#ifdef SHOWHOST
        if ( utbufp->ut_host[0] != '\0' )
                printf(" (%s)", utbufp->ut_host);/* the host    */
#endif
        printf("\n");                          /* newline      */
}

void showtime( long timeval )
/*
 *      displays time in a format fit for human consumption
 *      uses ctime to build a string then picks parts out of it
 *      Note: %12.12s prints a string 12 chars wide and LIMITS
 *      it to 12chars.
 */
{
        char    *cp;                    /* to hold address of time      */

        cp = ctime(&timeval);           /* convert time to string       */
                                        /* string looks like            */
                                        /* Mon Feb  4 00:46:40 EST 1991 */
                                        /* 0123456789012345.            */
        printf("%12.12s", cp+4 );       /* pick 12 chars from pos 4     */
}
