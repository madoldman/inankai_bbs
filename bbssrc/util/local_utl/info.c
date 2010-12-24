// info.c  --  统计每日详细信息
// Powered by bluetent 2002.12.10 Version 1.0
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

//	int		newaccount;//新增帐号数
//	int		newaccountweb;//WWW方式下新增帐号数
	int		post=0;//发文章数
	int		talk=0;//聊天次数
	int		cross=0;//转载次数
//	int		del=0;//d删除文章次数
//	int		range=0;//D删除文章次数
	int		mail=0;//发送信件
	int		bbsnet=0;//网络穿梭次数
	int		newacct=0;//新增帐号数
	int		deadacct=0;//死亡帐号数
	sprintf(buf,"%s/tracetoday", BBSHOME);
	if ((fp = fopen(buf, "r")) == NULL) {
		printf("can't open trace\n");
		return 1;
	}
	now = time(0);
	getdatestring(now);
	sprintf(date, "%14.14s", datestring);
	while (fgets(buf, 256, fp))
	{
		if(strstr(buf, date)){
		if(strstr(buf, " cross_posted "))
		{
			cross++;
			continue;
		}
		if(strstr(buf, " posted "))
		{
			post++;
			continue;
		}
/*		if(strstr(buf, " new account"))
		{
			newaccount++;
			if(strstr(buf, "WWW"))
			{
				newaccountweb++;
			}
			continue;
		}*/
		if(strstr(buf, " TALK to "))
		{
			talk++;
			continue;
		}
/*		if(strstr(buf, " Del "))
		{
			del++;
			continue;
		}
		if(strstr(buf, " Range delete "))
		{
			range++;
		}*/
		if(strstr(buf, " mailed "))
		{
			mail++;
			continue;
		}
		if(strstr(buf, " bin/bbsnet "))
		{
			bbsnet++;
			continue;
		}
	}
	}
	fclose(fp);
	sprintf(buf,"%s/usies", BBSHOME);
	if ((fp = fopen(buf, "r")) == NULL) {
		printf("can't open trace\n");
		return 1;
	}
	while (fgets(buf, 256, fp))
	{
		if(strstr(buf, "APPLY")&&strstr(buf, "uid")&&strstr(buf, "from"))
		{
			newacct++;
			continue;
		}
		if(strstr(buf, "KILL  ")&&strstr(buf, "#"))
		{
			deadacct++;
			continue;
		}
	}
	fclose(fp);
	sprintf(buf,"%s/0Announce/bbslist/info", BBSHOME);
	if ((fp = fopen(buf, "w")) == NULL) {
		printf("Cann't open info\n");
		return 1;
	}

	fprintf(fp, "\n\n     << %s %s 综合统计信息 >>\n\n",BBSNAME,date);
/*
	int		newaccount;//新增帐号数
	int		newaccountweb;//WWW方式下新增帐号数
	int		post=0;//发文章数
	int		talk=0;//聊天次数
	int		cross=0;//转载次数
	int		del=0;//删除文章次数
	int		mail=0;//发送信件
	int		bbsnet=0;//网络穿梭次数
*/
//	fprintf(fp, "注册帐号数：%d （其中WWW方式下注册帐号数：%d）\n", newaccount, newaccountweb);
	fprintf(fp, "          发表文章 %5d 篇\n", post);
	fprintf(fp, "          转载文章 %5d 篇\n", cross);
//	fprintf(fp, "          删除文章 %d 篇，平均每小时 %d 篇\n", del, del / 24);
	fprintf(fp, "          聊    天 %5d 次\n", talk);
	fprintf(fp, "          发送信件 %5d 封\n", mail);
	fprintf(fp, "          网络穿梭 %5d 次\n", bbsnet);
	fprintf(fp, "          新增帐号 %5d 个\n", newacct);
	fprintf(fp, "          死亡帐号 %5d 个\n", deadacct);
	fclose(fp);
}


