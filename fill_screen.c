/** sol06.12.c
 ** ------------------------------------------------------------
	A program that waits for keystrokes and also responds to
	window size change signals is
	sol06.12.c.
	This program appears to be a visual toy, but is in fact
	an introduction to the ideas and skills presented in
	the chapter on writing a video game.  Those ideas and
	skills are important to for understanding event-driven
	programming, operating system internals, and multi-threaded
	programming.

 ** ------------------------------------------------------------
 **
 **
 *   visual toy that combines signal handling and char by char keyboard
 *   input. This program fills the window by printing one character a lot.
 *   At each SIGWINCH, the screen refills with a new character.  When the
 *   user presses any key but 'Q' the filling letter is reset to A.
 *
	build: cc sol06.12.c -o sol06.12
 */

#include	<stdio.h>
#include	<signal.h>
#include	<termios.h>
#define oops(s) { perror(s); exit(1); }

char	filler = 'A';

void fill_screen(int);
void getout(int s); 
void settings(int how);
int screen_size(int *rowp, int *colp);

int main()
{
	int	c;

	settings(0);
	fill_screen(0);
	while( (c = getchar()) != EOF && c != 'Q' ){
		filler = 'A';
		fill_screen(0);
	}
	settings(1);
}

/*
 * fill_screen()
 *   - putchar() enough filler chars to fill the screen
 *   - assume the screen wraps
 */
void fill_screen(int s)
{
	int	i, n;
	int	rows, cols;
	
	if ( screen_size(&rows, &cols) == 0 ){
		n = rows * cols ;
		for(i = 0 ; i<n ; i++)
			putchar(filler);
		fflush(stdout);
	}
	filler++;
}

/*
 * called for SIGINT and SIGQUIT
 */
void getout(int s)
{
	settings(1);
	exit(0);
}

/*
 * program settings are turned on and off here
 */
void settings(int how)
{
	int	rv;

	static struct termios	term_orig;

	/* read settings */
	if ( how == 0 )
	{
		struct termios		term_new;

		rv = tcgetattr(0, &term_orig);	/* get settings */
		if ( rv == -1 )
			oops("getting settings");
		term_new = term_orig;
		term_new.c_lflag   &= ~ICANON;	/* noncanonical	*/ //ICANON：允许正规输入处理
		term_new.c_lflag   &= ~ECHO;	/* no echo 	*/
		term_new.c_cc[VMIN] = 1;	/* chr-by-chr	*/

		rv = tcsetattr(0, TCSANOW, &term_new);
		if ( rv == -1 )
			oops("changing settings");

		signal(SIGINT,getout);
		signal(SIGQUIT,getout);
		signal(SIGWINCH,fill_screen);
	}
	else 
	{
		tcsetattr(0, TCSANOW, &term_orig);
	}
}

#include <sys/ioctl.h>

/*
 *  puts the number of rows and cols in the ints pointed to by
 *  rowp and colp.  Returns 0 on ok, -1 on error
 */
int screen_size(int *rowp, int *colp)
{
	struct winsize wbuf;

	if ( ioctl(1, TIOCGWINSZ, &wbuf) != -1 ){
		*rowp = wbuf.ws_row;
		*colp = wbuf.ws_col;
		return 0;
	}
	return -1;
}
