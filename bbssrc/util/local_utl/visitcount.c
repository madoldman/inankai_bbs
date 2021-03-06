// visitcount.c  --  统计上站分布情况
// Powered by bluetent 2003.01.10 Version 2.0

////////////////////////////
// history:
// 2002.12.11 Version 1.0
//
////////////////////////////

char 	datestring[30];

#include <time.h>
#include <stdio.h>
#include "bbs.h"

void getdatestring( time_t now)
{
        struct tm *tm;
        char weeknum[7][3]={"天","一","二","三","四","五","六"};

        tm = localtime(&now);
        sprintf(datestring,"%4d年%02d月%02d日%02d:%02d:%02d 星期%2s",
                tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
                tm->tm_hour,tm->tm_min,tm->tm_sec,
                weeknum[tm->tm_wday]);
}

int main()
{
	FILE   *fp;
	char    buf[256];
	char    date[80];
	int     now;
	int		c[5][2];
	int i;
	for(i=0;i<5;i++)
	{
		c[i][0]=0;
		c[i][1]=0;
	}
	sprintf(buf,"%s/usies", BBSHOME);
	if ((fp = fopen(buf, "r")) == NULL) {
		printf("can't open usies\n");
		return 1;
	}
	now = time(0);
	getdatestring(now);
	sprintf(date, "%14.14s", datestring);
	while (fgets(buf, 256, fp))
	{
		int iswww;
		if(strstr(buf, "ENTER")&&strstr(buf, date))
		{
			iswww=0;
			//全部
			c[0][0]++;
			if(strstr(buf, "[www]"))
			{
				c[0][1]++;
				iswww=1;
			}
			//宿舍网
			if(strstr(buf, "@10."))
			{
				c[1][0]++;
				if(iswww)c[1][1]++;
				continue;
			}

			//南开大学教育网
			int n1 = atoi(buf + 59 - iswww);
			if((!strncmp(buf + 51 - iswww, "202.113.", 8)
			&&((n1 >=16 && n1 <= 31) || (n1 >= 224 && n1 <= 239)))
			|| !strncmp(buf + 51 - iswww, "222.30.", 7))
			{
				if(iswww==1)c[2][1]++;
				c[2][0]++;
				continue;
			}
		}
	}
		fclose(fp);
	
	sprintf(buf,"%s/0Announce/bbslist/visitcount", BBSHOME);
	if ((fp = fopen(buf, "w")) == NULL) {
		printf("Cann't open visitcount\n");
		return 1;
	}

	fprintf(fp, "      我爱南开BBS站上站IP分布统计表 (%s)\n\n",datestring);

	fprintf(fp,"[1;36m●[1;36m数值(人次)[1;37m\n");	
	fprintf(fp,"        ┏━━━━━━━┯━━━━━┯━━━━━┯━━━━━┓\n");
	fprintf(fp,"        ┃   来    源   │Telnet方式│ WWW 方式 │  合  计  ┃\n");
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃南开大学宿舍网│   %5d  │   %5d  │   %5d  ┃\n", c[1][0]-c[1][1],c[1][1],c[1][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃南开大学教育网│   %5d  │   %5d  │   %5d  ┃\n",c[2][0]-c[2][1],c[2][1],c[2][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃校  外  网  段│   %5d  │   %5d  │   %5d  ┃\n",(c[0][0]-c[1][0]-c[2][0])-(c[0][1]-c[1][1]-c[2][1]),c[0][1]-c[1][1]-c[2][1],c[0][0]-c[1][0]-c[2][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃   总    计   │   %5d  │   %5d  │   %5d  ┃\n",c[0][0]-c[0][1],c[0][1],c[0][0]);
	fprintf(fp,"        ┗━━━━━━━┷━━━━━┷━━━━━┷━━━━━┛\n");

	
	fprintf(fp,"[1;36m●[1;36m百分比(横向)[1;37m\n");
	fprintf(fp,"        ┏━━━━━━━┯━━━━━┯━━━━━┯━━━━━┓\n");
	fprintf(fp,"        ┃   来    源   │Telnet方式│ WWW 方式 │  合  计  ┃\n");
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃南开大学宿舍网│  %5.2f%%  │  %5.2f%%  │ 100.00%%  ┃\n", (c[1][0]-c[1][1])*100.0/c[1][0],c[1][1]*100.0/c[1][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃南开大学教育网│  %5.2f%%  │  %5.2f%%  │ 100.00%%  ┃\n",(c[2][0]-c[2][1])*100.0/c[2][0],c[2][1]*100.0/c[2][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃校  外  网  段│  %5.2f%%  │  %5.2f%%  │ 100.00%%  ┃\n",((c[0][0]-c[1][0]-c[2][0])-(c[0][1]-c[1][1]-c[2][1]))*100.0/(c[0][0]-c[1][0]-c[2][0]),(c[0][1]-c[1][1]-c[2][1])*100.0/(c[0][0]-c[1][0]-c[2][0]));
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃   总    计   │  %5.2f%%  │  %5.2f%%  │ 100.00%%  ┃\n",(c[0][0]-c[0][1])*100.0/c[0][0],c[0][1]*100.0/c[0][0]);
	fprintf(fp,"        ┗━━━━━━━┷━━━━━┷━━━━━┷━━━━━┛\n");


	
	fprintf(fp,"[1;36m●[1;36m百分比(纵向)[1;37m\n");
	fprintf(fp,"        ┏━━━━━━━┯━━━━━┯━━━━━┯━━━━━┓\n");
	fprintf(fp,"        ┃   来    源   │Telnet方式│ WWW 方式 │  合  计  ┃\n");
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃南开大学宿舍网│  %5.2f%%  │  %5.2f%%  │  %5.2f%%  ┃\n", (c[1][0]-c[1][1])*100.0/(c[0][0]-c[0][1]),c[1][1]*100.0/c[0][1],c[1][0]*100.0/c[0][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃南开大学教育网│  %5.2f%%  │  %5.2f%%  │  %5.2f%%  ┃\n",(c[2][0]-c[2][1])*100.0/(c[0][0]-c[0][1]),c[2][1]*100.0/c[0][1],c[2][0]*100.0/c[0][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃校  外  网  段│  %5.2f%%  │  %5.2f%%  │  %5.2f%%  ┃\n",((c[0][0]-c[1][0]-c[2][0])-(c[0][1]-c[1][1]-c[2][1]))*100.0/(c[0][0]-c[0][1]),(c[0][1]-c[1][1]-c[2][1])*100.0/c[0][1],(c[0][0]-c[1][0]-c[2][0])*100.0/c[0][0]);
	fprintf(fp,"        ┠───────┼─────┼─────┼─────┨\n");
	fprintf(fp,"        ┃   总    计   │ 100.00%%  │ 100.00%%  │ 100.00%%  ┃\n");
	fprintf(fp,"        ┗━━━━━━━┷━━━━━┷━━━━━┷━━━━━┛\n");

	
	
	fclose(fp);
}


