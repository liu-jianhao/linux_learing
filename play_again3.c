/* play_again3.c
 *	purpose: ask if user wants another transaction
 *	 method: set tty into chr-by-chr, no-echo mode
 *		 set tty into no-delay mode
 *		 read char, return result
 *	returns: 0=>yes, 1=>no, 2=>timeout
 *	 better: reset terminal mode on Interrupt
 */
#include	<stdio.h>
#include	<termios.h>
#include	<fcntl.h>
#include	<string.h>

#define	ASK		"Do you want another transaction"
#define	TRIES	   3	                                   /* max tries */
#define SLEEPTIME  2                                       /* time per try */
#define	BEEP       putchar('\a')                           /* alert user */

main()
{
	int	response;
 * purpose: put file descriptor 0 into no-delay mode
 *  method: use fcntl to set bits
 *   notes: tcsetattr() will do something similar, but it is complicated
 */
{
	int	termflags;

	termflags = fcntl(0, F_GETFL);		/* read curr. settings	*/
	termflags |= O_NDELAY;			/* flip on nodelay bit	*/
	fcntl(0, F_SETFL, termflags);		/* and install 'em	*/
}

/* how == 0 => save current mode,  how == 1 => restore mode */
/* this version handles termios and fcntl flags             */

tty_mode(int how)
{
	static struct termios original_mode;
	static int            original_flags;
	if ( how == 0 ){
		tcgetattr(0, &original_mode);
		original_flags = fcntl(0, F_GETFL);
	}
	else {
		tcsetattr(0, TCSANOW, &original_mode); 
		fcntl( 0, F_SETFL, original_flags);	
	}
}
