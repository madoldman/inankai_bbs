/*
 * bigfile              -- 'big file' simple compress replacement
 * 
 * A part of BBS Express Project/Firebird BBS 3.0 Project
 * 
 * Copyright (c) 1999, Edward Ping-Da Chuang <edwardc@firebird.dhs.org> 
 * of BBS Express Project, All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * CVS: $Id: bigfile.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
 */

char            bigfile_c[] =
	"$Id: bigfile.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $";

#include <stdio.h>
#include <sys/stat.h>

#define VERSION         "0.1"
#define LINECTRL        31

int
pushin(char *fname, char *dir, char *output)
{
	struct stat     st;
	FILE           *fp, *fp2;
	char            buf[1025];

	if (!(stat(fname, &st) == 0 && S_ISREG(st.st_mode)))
		return 2;

	fp = fopen(output, "a");
	fp2 = fopen(fname, "r");
	if (fp == NULL || fp2 == NULL)
		return 3;

	fprintf(fp, ">%s:%s:<%c\n", dir, fname, LINECTRL);
	while (fgets(buf, 1024, fp2) != NULL)
		fputs(buf, fp);
	if (buf[strlen(buf) - 1] != '\n')
		fputs("\n", fp);

	fclose(fp);
	fclose(fp2);
}

char           *
stringtoken(char *string, char tag, int *log)
{
	int             i, j;
	char           *result;

	result = (char *) malloc(256);

	j = 0;
	for (i = *log;; i++) {
		if (i == strlen(string))
			break;
		if (string[i] == tag)
			break;
		result[j] = string[i];
		j++;
	}

	*log = i + 1;
	return ((char *) result);
}

void
getit(char *buf, char **x, char **y)
{
	int             i;

	buf++;
	i = 0;
	*x = stringtoken(buf, ':', &i);
	*y = stringtoken(buf, ':', &i);
}

int
dump(char *bfile)
{
	FILE           *fin, *fout;
	char            buf[1025], xx[80], *filename, *dir;
	int             locked = 0, size;

	fin = fopen(bfile, "r");
	if (fin == NULL)
		return 1;

	locked = 1;
	while (fgets(buf, 1024, fin) != NULL) {
		if (buf[0] == '>' && buf[strlen(buf) - 3] == '<' && buf[strlen(buf) - 2] == '\37') {
			if (locked == 1) {
				locked = 2;
				getit(buf, &dir, &filename);
				sprintf(xx, "mkdir -p %s", dir);
				system(xx);
				sprintf(xx, "%s/%s", dir, filename);
				size = 0;
				fout = fopen(xx, "w");
			} else {
				fclose(fout);
#ifdef _DEBUG
				printf("%s dumped, size: %d bytes\n", filename, size);
#endif				
				locked = 2;
				getit(buf, &dir, &filename);
				sprintf(xx, "mkdir -p %s", dir);
				system(xx);
				sprintf(xx, "%s/%s", dir, filename);
				size = 0;
				fout = fopen(xx, "w");
			}
			continue;
		}
		fputs(buf, fout);
		size += (int) strlen(buf);
	}

	fclose(fin);
	if (fout != NULL) {
		fclose(fout);
#ifdef _DEBUG
		printf("%s dumped, size: %d bytes\n", filename, size);
#endif		
	}
	printf("all file dumped.\n");
}

#ifdef  _HAVE_MAIN
int
main(int argc, char **argv)
{
	if (!strcmp(argv[1], "put")) {
		pushin("bbsnnrp.txt", "test", argv[2]);
		pushin("rfc977.txt", "test", argv[2]);
		pushin("tv.txt", "test", argv[2]);
		pushin("DEADJOE", "test", argv[2]);
	} else {
		dump(argv[2]);
	}
}
#endif
