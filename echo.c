#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

/* ARGSUSED */
int
main(int argc, char *argv[])
{
	int nflag;

	if (pledge("stdio", NULL) == -1)
		err(1, "pledge");

	/* This utility may NOT do getopt(3) option parsing. */
	if (*++argv && !strcmp(*argv, "-n")) {
		++argv;			//Ö¸Ïò×Ö·û´® 
		nflag = 1;
	}
	else
		nflag = 0;

	while (*argv) {
		(void)fputs(*argv, stdout);
		if (*++argv)
			putchar(' ');
	}
	if (!nflag)
		putchar('\n');

	return 0;
}
