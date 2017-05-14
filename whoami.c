/** whoami.c
 ** ------------------------------------------------------------
	The whoami command prints the name associated with the current user id.
	The who am i command prints the name associated with the 
	current terminal.
	A solution to whoami is: sol02.10b.c .

 ** ------------------------------------------------------------
 *
 * a version of whoami 
 * method: call geteuid() for current user id, then call getpwuid() for name
 * 
 *	
 */
#include        <stdio.h>
#include        <unistd.h>
#include	<pwd.h>
#include	<sys/types.h>

int main()
{
	uid_t	id ;		/* effective user id of process */
	struct passwd *p;	/* will hold pwd data for user	*/

	id = geteuid();		/* manpage says geteuid always succeeds */
	p  = getpwuid(id);	/* get userdb info about this uid	*/

	if ( p == NULL )	/* bizarre! an unknown user id		*/
		printf("I don't know who you are.\n");
	else
		printf("%s\n", p->pw_name);
}
