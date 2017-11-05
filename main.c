#include <stdio.h>
#include <string.h>

static int strlength(const unsigned char *str, int leading_space_only)
{
	const unsigned char *p = str;
	int len = 0;
	int in_leading_space = 1;

	while (*p != 0) {
		if ((*p & 0xC0) != 0x80) {
			/* https://stackoverflow.com/questions/7298059/how-to-count-characters-in-a-unicode-string-in-c */
			switch (*p) {
			case '\t':
				len = ((len + 8) / 8) * 8;
				break;
			case ' ':
				++len;
				break;
			case '\r':
			case '\n':
				if (leading_space_only != 0 && in_leading_space == 1) {
					len = 0;
				}
				break;
			default:
				if (leading_space_only != 0) {
					return len;
				} else {
					in_leading_space = 0;
				}
				++len;
			}
		}
		++p;
	}

	return len;
}

static void dumpstr(const unsigned char *str)
{
	const unsigned char *p = str;

	while (*p != 0) {
		printf("%02X ", *p++);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	const int strlen_limit = 1024 * 1024;
	char strcur[strlen_limit];
	char strmax[strlen_limit];
	FILE *fp;
	int strlen_max = 0;
	int Flag_debug = 0;

	if (argc == 2) {
		fp = fopen(argv[1], "r");
	} else if (argc == 3) {
		if (strncmp(argv[1], "-d", 2) == 0)
			Flag_debug = 1;
		fp = fopen(argv[2], "r");
	} else {
		printf("%s file\n", argv[0]);
		return 0;
	}
	if (fp == NULL) {
		fprintf(stderr, "%s open failed", argv[1]);
		return -1;
	}

	while (!feof(fp)) {
		int strlen_cur;

		if (fgets(strcur, strlen_limit, fp) == NULL) {
			break;
		}
		strlen_cur = strlength(strcur, (argc == 3));
		if (strlen_cur > strlen_max) {
			strncpy(strmax, strcur, strlen_cur);
			strlen_max = strlen_cur;
		}
	}

	if (Flag_debug != 0) {
		printf("%d %s\n", strlen_max, strmax);
		dumpstr(strmax);
	}

	printf("%d %s\n", strlen_max, argv[1]);

	return strlen_max;
}
