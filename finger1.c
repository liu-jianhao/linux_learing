/** sol11.14.c
 ** ------------------------------------------------------------
	A simple user information server is
	sol11.14.c.
	This program accepts a single line of input from the client
	then searches the getpwent database for any entries that
	contain the specified string in the pw_name or pw_gecos
	fields.

	This exercise provides more practice with basic server design,
	a chance to learn about/review getpwent, and a chance to decide
	whether the server should use fork to handle multiple connections
	or just handle them in sequence.  The solution provided here is
	simple, it does not use fork.  

 ** ------------------------------------------------------------
 **
 **
 *   A version of fingerd.  This program accepts a single name from the
 *   client and uses getpwent to search the userlist for a matching entry.
 *   This server does not use fork; the server does the work itself.
 *
 *   You can test this one by using telnet to connect to port 7979 and
 *   then typing any part of any username on the system.

 *	build: cc sol11.14.c -o sol11.14
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <pwd.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <netdb.h>
#include  <time.h>
#include  <strings.h>
#include  <string.h>

#define   PORTNUM  7979   /* regular finger is 79 */
#define   HOSTLEN  256
#define   oops(msg)      { perror(msg) ; exit(1) ; }

void trimcrnl(char *);
void get_userinfo(char *, FILE *);

int main(int ac, char *av[])
{
	struct  sockaddr_in   saddr;   /* build our address here */
	struct	hostent		*hp;   /* this is part of our    */
	char	hostname[HOSTLEN];     /* address 	         */
	int	sock_id,sock_fd;       /* line id, file desc     */
	FILE	*sock_fpi, *sock_fpo;  /* streams for in and out */
	char    request[BUFSIZ];       /* from client            */

      /** Step 1: ask kernel for a socket **/

	sock_id = socket( PF_INET, SOCK_STREAM, 0 );    /* get a socket */
	if ( sock_id == -1 ) 
		oops( "socket" );

      /** Step 2: bind address to socket.  Address is host,port **/

	bzero( (void *)&saddr, sizeof(saddr) ); /* clear out struct     */
	gethostname( hostname, HOSTLEN );       /* where am I ?         */
	hp = gethostbyname( hostname );         /* get info about host  */
	bcopy( (void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(PORTNUM);        /* fill in socket port  */
	saddr.sin_family = AF_INET ;            /* fill in addr family  */
	if ( bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0 )
	       oops( "bind" );

      /** Step 3: allow incoming calls with Qsize=1 on socket **/

	if ( listen(sock_id, 1) != 0 ) 
		oops( "listen" );

      /*
       * main loop: accept(), write(), close()
       */

	while ( 1 ){
	       sock_fd = accept(sock_id, NULL, NULL); /* wait for call */
	       if ( sock_fd == -1 )
		       oops("accept");       

	       /* convert fd into two buffered streams */
	        if( (sock_fpi = fdopen(sock_fd,"r")) == NULL )
		       oops("fdopen reading");
			if ( (sock_fpo = fdopen(sock_fd,"w")) == NULL )
			oops("fdopen writing");

	       if ( fgets(request, BUFSIZ-5, sock_fpi) == NULL )
		       oops("reading request");
	       trimcrnl(request);

		get_userinfo(request, sock_fpo);
	       
	       fclose(sock_fpo);	/* flush, free(), and close */
	       fclose(sock_fpi);	/* this only has to free()  */
       }
       return 0;
}

/*
 * This is important.  The request might arrive with trailing newlines
 * and carriage returns.  This function trims them all off
 */
void trimcrnl(char *str)
{
	int	c;

	while ( ( c = str[strlen(str)-1]) == '\n' || c == '\r' )
		str[strlen(str)-1] = '\0';
}

/*
 * get user info by searching what getpwent has to say
 */
void get_userinfo(char *rq, FILE *fp)
{
	struct passwd *infop;

	if  ( fprintf(fp, "%-10s %s\n", "LOGNAME", "REALNAME") == EOF )
		perror("fprintf");

	/* search */
	while( (infop = getpwent()) != NULL ){
		if ( strstr(infop->pw_name, rq) || strstr(infop->pw_gecos, rq) ){
			fprintf(fp, "%-10s %s\n", infop->pw_name, infop->pw_gecos);
		}
	}

	/* rewind for next time */
	setpwent();					//setpwent()用来将getpwent()的读写地址指回密码文件开头 
}
