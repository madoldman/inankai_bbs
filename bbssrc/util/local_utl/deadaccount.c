char 	datestring[30];


#include <time.h>
#include <stdio.h>
#include <bbs.h>

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
	FILE   *fp, *fp2;
	char    buf[256];
	char bufdead[75];//tdhlshx 031016
	int     logins, posts;
	char	killdate[30], userid[28];//, lastlogin[15];
	int		total=0;//�����ʺ�����
	sprintf(buf,"%s/0Announce/bbslist/deadaccount", BBSHOME);
	if ((fp2 = fopen(buf, "w")) == NULL) {
		return 1;
	}
	sprintf(buf,"%s/usies.old", BBSHOME);
	if ((fp = fopen(buf, "r")) == NULL) {
		printf("can't open usies\n");
		return 1;
	}
	else
	{
		fprintf(fp2, "                              ���������ʺ��б�\n");
		getdatestring(time(0));
		fprintf(fp2, "                    (ͳ�����ڣ�%s)\n\n", datestring);
		fprintf(fp2, "[1;36;44m��� [1;37m�ʺ�         [0;44m ����¼����  [1;44m      ��������                 [1;32m��վ�� [1;32m������[0m\n");
		while (fgets(buf, 256, fp))
		{
			if(strstr(buf, "KILL")&&strstr(buf, "#"))
			{
				total++;
			//	sscanf(buf, "%29s KILL  #%d %-12s %14.14s %5d %5d %5d",
		//			killdate, pid, userid, lastlogin, logins, posts, avl);
				strncpy(killdate, buf, 30);
				strncpy(userid, buf+42,28);
				//strncpy(lastlogin, buf+55, 15);//lastlogin��useridһ���ȡ��.
				logins=atoi(buf+70);
				posts=atoi(buf+76);
				//printf("%s %s %s \n%s\n",userid, lastlogin, killdate,logins, buf);
				sprintf(bufdead,"[1;36m%4d [1;37m%28s [0m[1;33m%30s[1;32m%6d %6d[0m\n",total,userid,killdate,logins,posts);
			       //		fprintf(fp2, "[1;36m%4d[1;37m%12s %30s[1m %14s[1;32m %6d[1;32m %6d[0m\n", 
			//		total, userid, killdate, lastlogin, logins, posts);
			       fprintf(fp2,bufdead);
//2003��02��14��13:32:11 ������ KILL  #1402 gaoxiong     2003��01��29�� 1 0 -1
			}
		}
		fclose(fp);
		fclose(fp2);
	}
}

