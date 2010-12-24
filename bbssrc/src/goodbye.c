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
$Id: goodbye.c,v 1.2 2010-07-05 04:58:44 madoldman Exp $
*/

#include "bbs.h"

#ifndef DLM
#undef  ALLOWGAME
#endif

extern char BoardName[];
typedef struct
{
	char   *match;
	char   *replace;
}
logout;

int countlogouts(char filename[STRLEN])
{
	FILE   *fp;
	char    buf[256];
	int     count = 0;
	if ((fp = fopen(filename, "r")) == NULL)
		return 0;

	while (fgets(buf, 255, fp) != NULL)
	{
		if (strstr(buf, "@logout@") || strstr(buf, "@login@"))
			count++;
	}
	fclose(fp); /* add by yiyo */
	return count + 1;
}

void showstuff(char *buf);

user_display(char* filename, int number, int mode)
{
	FILE   *fp;
	char    buf[256];
	int     count = 1;
	clear();
	move(1, 0);
	if ((fp = fopen(filename, "r")) == NULL)
		return;

	while (fgets(buf, 255, fp) != NULL)
	{
		if (strstr(buf, "@logout@") || strstr(buf, "@login@"))
		{
			count++;
			continue;
		}
		if (count == number)
		{
			if (mode == YEA)
				showstuff(buf/*, 0*/);
			else
			{
				prints("%s", buf);
			}
		}
		else if (count > number)
			break;
		else
			continue;
	}
	refresh();
	fclose(fp);
	return;
}

char* cexp(int exp)
{
	if (exp == -9999)
		return GLY_CEXP0;
	if (exp <= 100 )
		return GLY_CEXP1;
	if (exp > 100  && exp <= 450 )
		return GLY_CEXP2;
	if (exp > 450 && exp <= 850 )
		return GLY_CEXP3;
	if (exp > 850 && exp <= 1500 )
		return GLY_CEXP4;
	if (exp > 1500 && exp <= 2500 )
		return GLY_CEXP5;
	if (exp > 2500 && exp <= 3000 )
		return GLY_CEXP6;
	if (exp > 3000 && exp <= 5000 )
		return GLY_CEXP7;
	if (exp > 5000 && exp <= 10000 )
		return GLY_CEXP8;
	if (exp > 10000 && exp <= 20000 )
		return GLY_CEXP9;
	if (exp > 20000)
		return GLY_CEXP10;
}

#ifdef	ALLOW_MEDALS

char* cmedal (unsigned int num)
{
	switch (num) {
	case 1: return GLY_NEW_MEDAL1;
	case 2: return GLY_NEW_MEDAL2;
	case 3: return GLY_NEW_MEDAL3;
	case 4: return GLY_NEW_MEDAL4;
	case 5: return GLY_NEW_MEDAL5;
	case 6: return GLY_NEW_MEDAL6;
	case 7: return GLY_NEW_MEDAL7;
	}
	return GLY_NEW_MEDAL0;
}

#endif

char* cnumposts(int num)
{

	if(num== 0 )
		return GLY_CPOST0;
	if(num>0&&num<=500)
		return GLY_CPOST1;
	if(num>500&&num<=1500)
		return GLY_CPOST2;
	if(num>1500&&num<=4000)
		return GLY_CPOST3;
	if(num>4000&&num<=8000)
		return GLY_CPOST4;
	if(num>8000&&num<=15000)
		return GLY_CPOST5;
	if(num>15000)
		return GLY_CPOST6;
}

#ifdef ALLOWGAME
char* cnummedals(int num)
{

	if(num== 0 )
		return  GLY_MEDAL0;
	if(num<=300)
		return  GLY_MEDAL1;
	if(num>300&&num<=1000)
		return  GLY_MEDAL2;
	if(num>1000&&num<=3000)
		return  GLY_MEDAL3;
	if(num>3000)
		return  GLY_MEDAL4;

}

char* cmoney(int num)
{
	if(num<= 100 )
		return GLY_MONEY0;
	if(num>100&&num<=3000)
		return GLY_MONEY1;
	if(num>3000&&num<=10000)
		return GLY_MONEY2;
	if(num>10000&&num<=50000)
		return GLY_MONEY3;
	if(num>50000&&num<=150000)
		return GLY_MONEY4;
	if(num>150000&&num<=300000)
		return GLY_MONEY5;
	if(num>300000&&num<=500000)
		return GLY_MONEY6;
	if(num>500000)
		return GLY_MONEY7;
}

#endif

char* cperf(int perf)
{
	if (perf == -9999)
		return GLY_CPERF0;
	if (perf <= 5)
		return GLY_CPERF1;
	if (perf > 5 && perf <= 12)
		return GLY_CPERF2;
	if (perf > 12 && perf <= 35)
		return GLY_CPERF3;
	if (perf > 35 && perf <= 50)
		return GLY_CPERF4;
	if (perf > 50 && perf <= 90)
		return GLY_CPERF5;
	if (perf > 90 && perf <= 140)
		return GLY_CPERF6;
	if (perf > 140 && perf <= 200)
		return GLY_CPERF7;
	if (perf > 200)
		return GLY_CPERF8;
}

int countexp(struct userec* udata)
{
	int     exp;
	if (!strcmp(udata->userid, "guest"))
		return -9999;
	exp = udata->numposts +
	      udata->numlogins / 5 +
	      (time(0) - udata->firstlogin) / 86400 +
	      udata->stay / 3600;
	return exp > 0 ? exp : 0;
}

int countperf(struct userec* udata)
{
	int     perf;
	int     reg_days;
	if (!strcmp(udata->userid, "guest"))
		return -9999;
	reg_days = (time(0) - udata->firstlogin) / 86400 + 1;
	/*
	 * 990530.edwardc 注册没成功或还在注册的人的人会导致 reg_days = 0,
	 * 然後在下面会产生 SIGFPE, 除数为零的错误 ..
	 */
	if (reg_days <= 0)
		return 0;
	if (udata->numlogins<=0)
		return 0;//modified by yiyo上站次数为0表现值为0
	perf = ((float) udata->numposts / (float) udata->numlogins +
	        (float) udata->numlogins / (float) reg_days) * 10;
	return perf > 0 ? perf : 0;
}

void countdays(int* year,int* month,int* day,time_t now)
{
	struct tm *GoodTime;
	time_t tmptime;
	GoodTime = localtime(&now);
	GoodTime->tm_year = *year - 1900;
	GoodTime->tm_mon = *month-1;
	GoodTime->tm_mday = *day;
	GoodTime->tm_hour = 0;
	GoodTime->tm_min = 0;
	tmptime = mktime(GoodTime);
	*year = (tmptime-now)/86400;
	*month = (tmptime-now-*year*86400)/3600;
	*day = (tmptime-now-*year*86400-*month*3600)/60;
}
/*
void showstuff(buf, limit)
char    buf[256];
int     limit;
*/
#define WEATHERUPDATE 21600
#define DEFAULTCITY "天津"
void showstuff(char *buf)
{
	extern time_t login_start_time;
	int     frg, i, j, matchfrg, strlength, cnt, tmpnum;
	static char numlogins[10], numposts[10], nummails[10], rgtday[30],
	lasttime[30],lastjustify[30], thistime[30], stay[10],
	alltime[20], ccperf[20],perf[10], exp[10], ccexp[20],
	star[5];
	static char ipdata[64], province[16], city[32];
	static char weatherbuf[1024], wcity[32];
	static char* weatherdata[3 * 3];
	static time_t wuptime;
	FILE * fp;
	char buf1[80], *p;
#ifdef ALLOWGAME

	static char moneys[10];
#endif

	char    buf2[STRLEN], *ptr, *ptr2;
	time_t  now;
	if (!weatherdata[0])
	{
		for(i = 0; i < sizeof(weatherdata) / sizeof(*weatherdata); i++)
		{
			weatherdata[i] = weatherbuf + 64 * i;
		}
	}
	static logout loglst[] =
	    {
	        "userid", currentuser.userid,
	        "username", currentuser.username,
	        "realname", currentuser.realname,
	        "address", currentuser.address,
	        "email", currentuser.email,
	        "termtype", currentuser.termtype,
	        "realemail", currentuser.reginfo,
	        "ident", currentuser.ident,
	        "rgtday", rgtday,
	        "login", numlogins,
	        "post", numposts,
	        "mail", nummails,
	        "lastlogin", lasttime,
	        "lasthost", currentuser.lasthost,
	        "lastjustify", lastjustify,
	        "now", thistime,
	        "bbsname", BoardName,
	        "stay", stay,
	        "alltime", alltime,
	        "exp", exp,
#ifdef ALLOWGAME
	        "money",moneys,
#endif
	        "cexp", ccexp,
	        "perf", perf,
	        "cperf", ccperf,
	        "star", star,
	        "pst", numposts,
	        "log", numlogins,
	        "bbsip", BBSIP,
	        "bbshost", BBSHOST,
	        "version",BBSVERSION,
			"ipdata", ipdata,
			"city", city,
			"province", province,
			"wcity", wcity,
			"wea_0_d", weatherbuf + 0 * 64,
			"wea_0_t", weatherbuf + 1 * 64,
			"wea_0_w", weatherbuf + 2 * 64,
			"wea_1_d", weatherbuf + 3 * 64,
			"wea_1_t", weatherbuf + 4 * 64,
			"wea_1_w", weatherbuf + 5 * 64,
			"wea_2_d", weatherbuf + 6 * 64,
			"wea_2_t", weatherbuf + 7 * 64,
			"wea_2_w", weatherbuf + 8 * 64,
	        NULL, NULL,
	    };
	if (!*ipdata || wuptime < time(NULL) - WEATHERUPDATE)
	{
		wuptime = time(NULL);
		strncpy(buf2, currentuser.lasthost, 40);
		for(p = buf2; *p; p++)
		{
			if (!strchr("01234567890.", *p))
			{
				*p = 0;
				break;
			}
		}
		sprintf (buf1, "bin/city.php %s", buf2);
		fp = popen(buf1, "r");
		if (fp)
		{
			fgets(ipdata, 80, fp);
			p = strchr(ipdata, '\n');
			if (p)
			{
				*p = 0;
			}
			fgets(province, sizeof(province), fp);
			p = strchr(province, '\n');
			if (p)
			{
				*p = 0;
			}
			fgets(city, sizeof(city), fp);
			p = strchr(city, '\n');
			if (p)
			{
				*p = 0;
			}
			if (strlen(city) < 4)
			{
				strcpy(wcity, DEFAULTCITY);
			}
			else
			{
				strcpy(wcity, city);
			}
			for (i = 0; i < 9; i++)
			{
				fgets(weatherbuf + i * 64, 60, fp);
				p = strchr(weatherbuf + i * 64, '\n');
				if (p)
				{
					*p = 0;
				}
			}
			fclose(fp);
		}
		else
		{
			strcpy(ipdata, "?");
		}
	}
	if (!strchr(buf, '$'))
	{
		//if (!limit)
		prints("%s", buf);
		//else
		//	prints("%.82s", buf);
		return;
	}
	now = time(0);
	/* for ansimore3() */

	if (currentuser.numlogins > 0)
	{
		tmpnum = countexp(&currentuser);
		sprintf(exp, "%d", tmpnum);
		strcpy(ccexp, cexp(tmpnum));
		tmpnum = countperf(&currentuser);
		sprintf(perf, "%d", tmpnum);
		strcpy(ccperf, cperf(tmpnum));
		sprintf(alltime, "%d小时%d分钟", currentuser.stay / 3600, (currentuser.stay / 60) % 60);
		getdatestring(currentuser.firstlogin,NA);
		sprintf(rgtday, "%s", datestring);
		getdatestring(currentuser.lastlogin,NA);
		sprintf(lasttime, "%s", datestring);
		getdatestring(now,NA);
		sprintf(thistime, "%s", datestring);
		getdatestring(currentuser.lastjustify,NA);
		sprintf(lastjustify, "%24.24s", datestring);
		sprintf(stay, "%d", (time(0) - login_start_time) / 60);
		sprintf(numlogins, "%d", currentuser.numlogins);
		sprintf(numposts, "%d", currentuser.numposts);
#ifdef ALLOWGAME

		sprintf(moneys,"%d",currentuser.money);
#endif

		sprintf(nummails, "%d", currentuser.nummails);
		sprintf(star, "%s", horoscope(currentuser.birthmonth, currentuser.birthday));
	}
	frg = 1;
	ptr2 = buf;
	do
	{
		if (ptr = strchr(ptr2, '$'))
		{
			matchfrg = 0;
			*ptr = '\0';
			prints("%s", ptr2);
			ptr += 1;
			for (i = 0; loglst[i].match != NULL; i++)
			{
				if (strstr(ptr, loglst[i].match) == ptr)
				{
					strlength = strlen(loglst[i].match);
					ptr2 = ptr + strlength;
					for (cnt = 0; *(ptr2 + cnt) == ' '; cnt++)
						;
					sprintf(buf2, "%-*.*s", cnt ? strlength + cnt : strlength + 1, strlength + cnt, loglst[i].replace);
					prints("%s", buf2);
					ptr2 += (cnt ? (cnt - 1) : cnt);
					matchfrg = 1;
					break;
				}
			}
			if (!matchfrg)
			{
				prints("$");
				ptr2 = ptr;
			}
		}
		else
		{
			//if (!limit)
			prints("%s", ptr2);
			//else
			//	prints("%.82s", ptr2);
			frg = 0;
		}
	}
	while (frg);
	return;
}
