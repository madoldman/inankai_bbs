/*
         [ 配合BBSD方式使用的uptime    by Eric ]
请修改 cron.bbs 中的：
0,5,10,15,20,25,30,35,40,45,50,55 * * * * /usr/bin/uptime >> /home/HOPE/bbs/reclog/uptime.log
为：
0,5,10,15,20,25,30,35,40,45,50,55 * * * * /home/bbs/bin/bbsuptime >> /home/HOPE/bbs/reclog/uptime.log
并重新用 bbs 的身份执行： crontab cron.bbs
*/	 

#include "bbs.h"
#define AVEFLE  BBSHOME"/reclog/ave.src"
#define AVEPIC  BBSHOME"/0Announce/bbslist/today"
#define MAX_LINE (15)

char	datestring[30];
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


int max=0;
int search_shmkey(char *keyname)
{
   int     i = 0, found = 0;
   while (shmkeys[i].key != NULL) {
      if (strcmp(shmkeys[i].key, keyname) == 0) {
         found = shmkeys[i].value;
	 break;
      }
      i++;
   } 
   return found;
}

int     draw_pic()
{
        char    *blk[10] =
        {
                "＿", "＿", "▁", "▂", "▃",
                "▄", "▅", "▆", "▇", "█"
        };
        FILE    *fp;
        int     cr = 0,
                tm,
                i, item,
                j, aver=0;
        int     pic[24];
        char    buf[80];
        time_t  now;
	for (i=0;i<24 ;i++ )
	{
		pic[i]=0;
	}
	now = time(0);
	getdatestring(now);
        if ((fp = fopen(AVEFLE, "r")) == NULL)
                return -1;
        else
        {
                bzero(&pic, sizeof(pic));
				i=0;
                while (fgets(buf, 50, fp) != NULL)
                {
						cr = atoi(index(buf, ':')+1);
                        tm = atoi(buf);
                        pic[tm] = (cr>pic[tm]) ? cr : pic[tm];
                        aver+=cr;
                        i++;
                        max = (max > cr) ? max : cr;
                }
                aver=aver/i+1;
                fclose(fp);			
        }

        if ((fp = fopen(AVEPIC, "w")) == NULL)
                return -1;

        item=(max/MAX_LINE)+1;

        fprintf(fp,"\n[1;36m    ┌——\241\252———————— 超过 1000 将不显示千位数字 ———————————┐\n");

        for (i = MAX_LINE; i >= 0; i--)
        {
                fprintf(fp, "[1;37m%4d[36m│[32m", (i+1)*item);
                for (j = 0; j < 24; j++)
                {
                  if ((item * i > pic[j]) && (item * (i-1) <= pic[j])&& pic[j])
                  {
                    if(pic[j]<1000)
						fprintf(fp, "[35m%-3d[32m", (pic[j]));
					else
					fprintf(fp, "[33m%-3d[32m", (pic[j]-1000));

                    continue;
                  }
                  if(pic[j]-item*i<item && item*i<pic[j])
                          fprintf(fp,"%s ", blk[((pic[j]-item * i)*10)/item]);
                  else if(pic[j]-item * i>=item)
                          fprintf(fp,"%s ",blk[9]);
                  else
                           fprintf(fp,"   ");
                }
                fprintf(fp,"[1;36m│[m");
                fprintf(fp, "\n");
        }
        time(&now);
        fprintf(fp, "[1;37m   0[36m└——[37m%-12.12s平均负载人数统计[36m —— [37m%s[36m ——┘[m\n", BBSNAME,datestring);
        fprintf(fp, "[1;36m      00 01 02 03 04 05 06 07 08 09 10 11 [31m12 13 14");
        fprintf(fp, " 15 16 17 18 19 20 21 22 23[m\n\n");
        fprintf(fp, "   [1;36m    1 [32m■[36m = [37m%3d     [36m 平均上站人数：[37m%4d[36m   最高在线人数：[37m%4d[m\n",item,aver,max);
        fclose(fp);
}


int main()
{
	FILE *fp;
   void   *shmptr;
   struct UTMPFILE *utmpshm;
   struct tm *now;
   int shmid, shmkey;
   int i, max, count;
   time_t  nowtime;

   shmkey = search_shmkey("UTMP_SHMKEY");
   if (shmkey < 1024) shmkey = 30020;
   if ((shmid = shmget( shmkey, 0, 0))==-1) exit(1);
   shmptr = (void *) shmat(shmid, NULL, 0);
   utmpshm = shmptr;

    max = USHM_SIZE - 1;
    while (max > 0 && utmpshm->uinfo[max].active == 0) max--;
    count =0;
    for (i = 0; i <= max; i++) {
        if (utmpshm->uinfo[i].active&&utmpshm->uinfo[i].pid) count++;
    }
    shmdt(shmptr);
    nowtime=time(0);
    now = localtime(&nowtime);
/*   printf("%d:%d%s ,  %d user, \n",
         (now->tm_hour%12==0)?12:(now->tm_hour%12),
         now->tm_min,(now->tm_hour>11)?"pm":"am",count);
*/
//	printf("%2d %2d %4d", now->tm_hour, now->tm_min, count);
        if ((fp = fopen(AVEFLE, "a+")) == NULL)
                return -1;
        fprintf(fp, "%d:%d\n", now->tm_hour, count);
        fclose(fp);
		draw_pic(time);

}
