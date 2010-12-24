/* account.c  -- count for no. of logins */
/* $Id: account.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $ */

#include <time.h>
#include <stdio.h>
#include "bbs.h"
#define MAX_LINE        (15)
char 	datestring[30];
int www=0;//bluetent
struct {
	int     no[24];		/* ´ÎÊý */
	int     sum[24];	/* ×ÜºÏ */
}       st;

/* Added by deardragon 1999.12.2 */
void getdatestring( time_t now)
{
        struct tm *tm;
        char weeknum[7][3]={"Ìì","Ò»","¶þ","Èý","ËÄ","Îå","Áù"};

        tm = localtime(&now);
        sprintf(datestring,"%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ÐÇÆÚ%2s",
                tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
                tm->tm_hour,tm->tm_min,tm->tm_sec,
                weeknum[tm->tm_wday]);
}
/* Added End. */

int
main(argc, argv)
char   *argv[];
{
	FILE   *fp;
	char    buf[256], *p;
	char    date[80];
	int     now;
	int     hour, max = 0, item, total = 0;
	int     totaltime = 0;
	int     i, j;
	char   *blk[10] =
	{
                "£ß", "£ß", "¨x", "¨y", "¨z",
                "¨{", "¨|", "¨}", "¨~", "¨€",
	};
	sprintf(buf,"%s/usies", BBSHOME);
	if ((fp = fopen(buf, "r")) == NULL) {
		printf("can't open usies\n");
		return 1;
	}
	now = time(0);
	getdatestring(now);
	sprintf(date, "%14.14s", datestring);
	while (fgets(buf, 256, fp)) {
		hour = atoi(buf + 14);
		if (hour < 0 || hour > 23) {
			printf("%s", buf);
			continue;
		}
		if (strncmp(buf, date, 14))
			continue;
		if (!strncmp(buf + 30, "ENTER", 5)) {
			if(strstr(buf, "[www]"))
				www++;
			st.no[hour]++;
			continue;
		}
		if (p = (char *) strstr(buf + 49, "Stay:")) {
			st.sum[hour] += atoi(p + 6);
			continue;
		}
	}
	fclose(fp);

	for (i = 0; i < 24; i++) {
		total += st.no[i];
		totaltime += st.sum[i];
		if (st.no[i] > max)
			max = st.no[i];
	}

	if ( max != 0 )
		item = max / MAX_LINE + 1;
	else {
/*		printf("max is 0.\n"); */
		exit(0);
	}

	sprintf(buf,"%s/0Announce/bbslist/countusr", BBSHOME);
	if ((fp = fopen(buf, "w")) == NULL) {
		printf("Cann't open countusr\n");
		return 1;
	}
	fprintf(fp, "\n[1;36m    ©°¡ª¡ª\241\252¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª ³¬¹ý 1000 ½«²»ÏÔÊ¾Ç§Î»Êý×Ö ¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©´\n");
	for (i = MAX_LINE; i >= 0; i--) {
		fprintf(fp, "[1;37m%4d[36m©¦[33m", (i + 1) * item);
		for (j = 0; j < 24; j++) {
			if ((item * (i) > st.no[j]) && (item * (i - 1) <= st.no[j]) && st.no[j]) {
					fprintf(fp, "[35m%-3d[33m", st.no[j]%1000);
				continue;
			}
			if (st.no[j] - item * i < item && item * i < st.no[j])
				fprintf(fp, "%s ", blk[((st.no[j] - item * i) * 10) / item]);
			else if (st.no[j] - item * i >= item)
				fprintf(fp, "%s ", blk[9]);
			else
				fprintf(fp, "   ");
		}
		fprintf(fp, "[1;36m©¦\n");
	}
	
	fprintf(fp,"   [37m0[36m©¸¡ª¡ª [37m%-12.12sÃ¿Ð¡Ê±µ½·ÃÈË´ÎÍ³¼Æ[36m¡ª¡ª[37m%s[36m¡ª¡ª©¼\n"
		"    [;36m  00 01 02 03 04 05 06 07 08 09 10 11 [1;31m12 13 14 15 16 17 18 19 20 21 22 23\n\n"
		"   [32m1 [33m¨~ [32m= [37m%-5d [32m×Ü¹²ÉÏÕ¾ÈË´Î£º[37m%-6d [32mÆäÖÐWWW·½Ê½£º[37m%-6d [32mÆ½¾ùÊ¹ÓÃÊ±¼ä£º[37m%d[m\n"
		,BBSNAME, datestring, item, total, www,( totaltime == 0 ) ? 0 : totaltime / total + 1);
	fclose(fp);
}
