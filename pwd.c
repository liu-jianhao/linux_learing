#include <sys/stat.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char *__progname;
static char *getcwd_logical(void);
__dead static void usage(void);

int main(int argc, char *argv[])
{
	int ch, lFlag = 0;
	const char *p;

	if (pledge("stdio rpath", NULL) == -1)
		err(1, "pledge");

	while ((ch = getopt(argc, argv, "LP")) != -1) {
		switch (ch) {
		case 'L':
			lFlag = 1;
			break;
		case 'P':
			lFlag = 0;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 0)
		usage();

	if (lFlag)
		p = getcwd_logical();
	else
		p = NULL;
	if (p == NULL)
		p = getcwd(NULL, 0);

	if (p == NULL)
		err(EXIT_FAILURE, NULL);

	puts(p);

	exit(EXIT_SUCCESS);
}

static char *getcwd_logical(void)
{
	char *pwd, *p;
	struct stat s_pwd, s_dot;

	/* Check $PWD -- if it's right, it's fast. */
	/*
		getenv()用来取得参数envvar环境变量的内容。参数envvar为环境变量的名称，
	如果该变量存在则会返回指向该内容的指针。环境变量的格式为envvar=value。
	getenv函数的返回值存储在一个全局二维数组里，当你再次使用getenv函数时
	不用担心会覆盖上次的调用结果。
		返回值: 执行成功则返回指向该内容的指针，找不到符合的环境变量名称则返回NULL。
	如果变量存在但无关联值，它将运行成功并返回一个空字符串，即该字符的第一个字节是null。
	*/ 
	pwd = getenv("PWD"); 	//即得到路径 
	if (pwd == NULL)
		return NULL;
	if (pwd[0] != '/')
		return NULL;

	/* check for . or .. components, including trailing ones */
	for (p = pwd; *p != '\0'; p++)
		if (p[0] == '/' && p[1] == '.') {
			if (p[2] == '.')
				p++;
			if (p[2] == '\0' || p[2] == '/')
				return NULL;
		}

	if (stat(pwd, &s_pwd) == -1 || stat(".", &s_dot) == -1)
		return NULL;
	if (s_pwd.st_dev != s_dot.st_dev || s_pwd.st_ino != s_dot.st_ino)
		return NULL;
	return pwd;
}

static void usage(void)
{
	fprintf(stderr, "usage: %s [-LP]\n", __progname);
	exit(EXIT_FAILURE);
}
