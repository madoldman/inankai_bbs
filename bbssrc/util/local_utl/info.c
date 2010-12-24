// info.c  --  ͳ��ÿ����ϸ��Ϣ
// Powered by bluetent 2002.12.10 Version 1.0
char 	datestring[30];

#include <time.h>
#include <stdio.h>
#include "bbs.h"

void getdatestring( time_t now)
{
        struct tm *tm;
        char weeknum[7][3]={"��","һ","��","��","��","��","��"};

        tm = localtime(&now);
        sprintf(datestring,"%4d��%02d��%02d��%02d:%02d:%02d ����%2s",
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

//	int		newaccount;//�����ʺ���
//	int		newaccountweb;//WWW��ʽ�������ʺ���
	int		post=0;//��������
	int		talk=0;//�������
	int		cross=0;//ת�ش���
//	int		del=0;//dɾ�����´���
//	int		range=0;//Dɾ�����´���
	int		mail=0;//�����ż�
	int		bbsnet=0;//���紩�����
	int		newacct=0;//�����ʺ���
	int		deadacct=0;//�����ʺ���
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

	fprintf(fp, "\n\n     << %s %s �ۺ�ͳ����Ϣ >>\n\n",BBSNAME,date);
/*
	int		newaccount;//�����ʺ���
	int		newaccountweb;//WWW��ʽ�������ʺ���
	int		post=0;//��������
	int		talk=0;//�������
	int		cross=0;//ת�ش���
	int		del=0;//ɾ�����´���
	int		mail=0;//�����ż�
	int		bbsnet=0;//���紩�����
*/
//	fprintf(fp, "ע���ʺ�����%d ������WWW��ʽ��ע���ʺ�����%d��\n", newaccount, newaccountweb);
	fprintf(fp, "          �������� %5d ƪ\n", post);
	fprintf(fp, "          ת������ %5d ƪ\n", cross);
//	fprintf(fp, "          ɾ������ %d ƪ��ƽ��ÿСʱ %d ƪ\n", del, del / 24);
	fprintf(fp, "          ��    �� %5d ��\n", talk);
	fprintf(fp, "          �����ż� %5d ��\n", mail);
	fprintf(fp, "          ���紩�� %5d ��\n", bbsnet);
	fprintf(fp, "          �����ʺ� %5d ��\n", newacct);
	fprintf(fp, "          �����ʺ� %5d ��\n", deadacct);
	fclose(fp);
}


