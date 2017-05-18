/** mkdir.c
 ** ------------------------------------------------------------
	A version of mkdir that supports the -p option is:
	This program allows multiple names on the command line.

 ** ------------------------------------------------------------
 *  A version of mkdir that supports the -p option
 *  exits with code 1 if any error
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h> 
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#define	MODE	0755	/* will be modified by umask */
int
main(int ac, char *av[])
{
	int	p_flag = 0;
	int	rv = 0;

	p_flag = check_args(ac,av);
	while(rv == 0 && --ac)
		if ( **++av != '-' )
			if ( do_mkdir(*av, p_flag) == -1 )
				rv = 1;
	return rv;
}

/*
 * check_args(int ac, char *av[])
 * returns a boolean value indicating if a -p has been seen
 */
int check_args(int ac, char *av[])
{
	int	i;

	for(i=1; i<ac; i++)
		if ( strcmp(av[i],"-p") == 0 )
			return 1;
	return 0;
}

/*
 * does the work of the program.  If the p_flag is set, 
 * then try to make all subdirectories if needed
 */

do_mkdir(char *dirname, int p_flag)
{
	extern int errno;
	char       *dir;
	char	   *cp;
	int	   rv = 0;

	/*
	 * try the easy case first. If it works, return
	 */

	if ( mkdir(dirname, MODE) != -1 )
		return 0;

	/*
	 * if any error besides ENOENT or no p flag
	 * report the error and return
	 */
	if( errno != ENOENT || !p_flag ){			//(errno.h)#define ENOENT           2       No such file or directory 
		perror(dirname);						
		return -1;
	}

	/*
	 * Last case, problem is missing components and
	 * -p was set.  Here, we build the path downwards
	 * quitting if any component is not already there as
	 * a directory or cannot be made.
	 */
	dir = (char *)malloc(strlen(dirname)+1);
	if ( dir == NULL ){
		perror("out of memory");
		return -1;
	}
	strcpy(dir,dirname);

	/** skip over leading /'s **/
	for(cp = dir; *cp == '/' ; cp++ )		
		;

	/** start building dirs from there **/
	while( rv == 0 && (cp = strchr(cp,'/')) )	{
		*cp = '\0';
		if ( !isadir(dir) && mkdir(dir, MODE) == -1 ){
			perror(dir);
			rv = -1;
		}
		*cp++ = '/';		
	}
	free(dir);

	/* then try again without the -p option */
	return ( rv == -1 ? rv : do_mkdir(dirname,0)) ;
}

isadir(char *d)
{
	struct stat	info;

	return ( stat(d,&info) != -1 && S_ISDIR(info.st_mode) );
}
