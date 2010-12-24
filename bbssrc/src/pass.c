/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: pass.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"
#include <sys/param.h>
#include <sys/resource.h>
#include <pwd.h>
#include <unistd.h>

char* crypt();
#ifndef MD5
#ifndef DES
/* nor DES, MD5, fatal error!! */
#error "(pass.c) you've not define DES nor MD5, fatal error!!"
#endif
#endif

static unsigned char itoa64[] =	/* 0 ... 63 => ascii - 64 */
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(char *s, long v, int n)
{
	while (--n >= 0)
	{
		*s++ = itoa64[v & 0x3f];
		v >>= 6;
	}
}

char* genpasswd(char *pw)
{
	char    salt[10];
	static char  pwbuf[PASSLEN];
	struct timeval tv;
	if (strlen(pw) == 0)
		return "";

	srand(time(0) % getpid());
	gettimeofday(&tv, 0);

#ifdef MD5			/* use MD5 salt */

	strncpy(&salt[0], "$1$", 3);
	to64(&salt[3], random(), 3);
	to64(&salt[6], tv.tv_usec, 3);
	salt[8] = '\0';
#endif
#ifdef DES			/* use DES salt */

	to64(&salt[0], random(), 3);
	to64(&salt[3], tv.tv_usec, 3);
	to64(&salt[6], tv.tv_sec, 2);
	salt[8] = '\0';
#endif

	strcpy(pwbuf, pw);
	return crypt1(pwbuf, salt);
}

#ifdef CHKPASSWDFORK
int checkpasswd(char *passwd, char *test)
{
	int pfds[2], pid;
	char value = 'f';
	if( pipe(pfds) < 0)
		return checkpasswd0(passwd, test);
	pid = fork();
	if( pid == 0)
	{
		close(pfds[0]);
		if( checkpasswd0(passwd, test) )
			value = 't';
		write(pfds[1], &value, 1);
		close(pfds[1]);
		bbsd_exit(0);
	}
	else if( pid == -1 )
		return checkpasswd0(passwd, test);
	else
	{
		close(pfds[1]);
		read(pfds[0], &value, 1);
		close(pfds[0]);
		return ( value == 't') ;
	}
}

int checkpasswd0(char *passwd, char *test)
#else
int checkpasswd(char *passwd, char *test)
#endif
{
	char   *pw;
	static char  pwbuf[PASSLEN];

	strncpy(pwbuf, test, PASSLEN);
	pw = crypt1(pwbuf, passwd);
	return (!strcmp(pw, passwd));
}
