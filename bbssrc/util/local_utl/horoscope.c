/* $Id: horoscope.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $ */

#include <time.h>
#include <stdio.h>
#include <math.h>
#include "bbs.h"
#define MAX_LINE        (15)

/* Added by deardragon 1999.12.2 */
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
/* Added End. */

main()
{
  FILE *fp;
  int now;
  int max = 0,item;
  int i, j;
  int boy=0, girl=0;
  char buf[256];
  char    *blk[10] =
  {
      "＿", "□", "□", "□", "□",
      "□", "□", "□", "□", "□",
  };
  struct userec aman;
//  struct userec allman[ MAXUSERS ];
  int mhoro[12];
  int fhoro[12];
  int bound[12] = {21,19,21,21,21,22,23,23,23,24,23,22};
  int mani[12];
  int fani[12];

  sprintf(buf,"%s/.PASSWDS", BBSHOME);
  if ((fp = fopen(buf, "rb")) == NULL)
  {
    printf("Can't open record data file.\n");
    return 1;
  }
  for (i=0; ; i++) {
    if (fread(&aman, sizeof( aman ), 1, fp ) <= 0) break;
    if (aman.birthmonth == 0) continue;
    if (aman.gender=='M') {
       boy++;
       mani[aman.birthyear % 12]++;
    } else {
       girl++;
       fani[aman.birthyear % 12]++;
    }
    if (aman.birthday < bound[aman.birthmonth-1]) {
      if(aman.gender=='M') mhoro[aman.birthmonth-1]++; else fhoro[aman.birthmonth-1]++;
    } else {
      if(aman.birthmonth == 12) {
        if(aman.gender=='M') mhoro[0]++; else fhoro[0]++;
      } else {
        if(aman.gender=='M') mhoro[aman.birthmonth]++; else fhoro[aman.birthmonth]++;
      }
    }
  }
  fclose(fp);

  now=time(0);
  getdatestring(now);
  for (i = 0; i < 12; i++)
  {
    if (mhoro[i] > max)
      max = mhoro[i];
    if (fhoro[i] > max)
      max = fhoro[i];
  }

  item = max / MAX_LINE + 1;

  sprintf(buf,"%s/0Announce/bbslist/counthoro", BBSHOME);
  if ((fp = fopen(buf, "w")) == NULL)
  {
    printf("Can't open counthoro\n");
    return 1;
  }

  fprintf(fp,"\n[1;37m    ┌————————————————————————————————————┐\n");
  for (i = MAX_LINE ; i >= 0; i--)
  {
    fprintf(fp, "[1;37m%4d│",(i+1)*item);
    for (j = 0; j < 12; j++)
    {
      if ((item * (i) > mhoro[j]) && (item * (i-1) <= mhoro[j]) && mhoro[j])
        fprintf(fp, "[35m%-3d", (mhoro[j]));
      else if(mhoro[j]-item*i<item && item*i<mhoro[j])
              fprintf(fp,"[36m%s ", blk[((mhoro[j]-item * i)*10)/item]);
      else if(mhoro[j]-item * i>=item)
              fprintf(fp,"[36m%s ",blk[9]);
      else
           fprintf(fp,"   ");
      if ((item * (i) > fhoro[j]) && (item * (i-1) <= fhoro[j]) && fhoro[j])
        fprintf(fp, "[35m%-3d", (fhoro[j]));
      else if(fhoro[j]-item*i<item && item*i<fhoro[j])
              fprintf(fp,"[31m%s ", blk[((fhoro[j]-item * i)*10)/item]);
      else if(fhoro[j]-item * i>=item)
              fprintf(fp,"[31m%s ",blk[9]);
      else
           fprintf(fp,"   ");
    }
    fprintf(fp, "[1;37m│\n");
  }
  fprintf(fp,"   [37m0└——— 目前本站注册使用者星座统计———%s——┘\n"
       "    [1;33m  摩羯  水瓶  双鱼  牡羊  金牛  双子  巨蟹  狮子  处女  天秤  天蝎  射手\n\n"
       "            [32m1 [37m□ [32m= [37m%-5d       [36m□ [32m= [37m男生 (%d)    [31m□ [32m= [37m女生 (%d)[m\n"
    ,datestring,item,boy,girl);
  fclose(fp);

  for (i = 0; i < 12; i++)
  {
    if (mani[i] > max)
      max = mani[i];
    if (fani[i] > max)
      max = fani[i];
  }

  item = max / MAX_LINE + 1;

  sprintf(buf,"%s/0Announce/bbslist/countani", BBSHOME);
  if ((fp = fopen(buf, "w")) == NULL)
  {
    printf("Can't open countani\n");
    return 1;
  }

  fprintf(fp,"\n[1;37m    ┌————————————————————————————————————┐\n");
  for (i = MAX_LINE ; i >= 0; i--)
  {
    fprintf(fp, "[1;37m%4d│",(i+1)*item);
    for (j = 0; j < 12; j++)
    {
      if ((item * (i) > mani[j]) && (item * (i-1) <= mani[j]) && mani[j])
        fprintf(fp, "[35m%-3d", (mani[j]));
      else if(mani[j]-item*i<item && item*i<mani[j])
              fprintf(fp,"[36m%s ", blk[((mani[j]-item * i)*10)/item]);
      else if(mani[j]-item * i>=item)
              fprintf(fp,"[36m%s ",blk[9]);
      else
           fprintf(fp,"   ");
      if ((item * (i) > fani[j]) && (item * (i-1) <= fani[j]) && fani[j])
        fprintf(fp, "[35m%-3d", (fani[j]));
      else if(fani[j]-item*i<item && item*i<fani[j])
              fprintf(fp,"[31m%s ", blk[((fani[j]-item * i)*10)/item]);
      else if(fani[j]-item * i>=item)
              fprintf(fp,"[31m%s ",blk[9]);
      else
           fprintf(fp,"   ");
    }
    fprintf(fp, "[1;37m│\n");
  }
  fprintf(fp,"   [37m0└———目前本站注册使用者生肖统计—————%s———┘\n"
       "    [1;33m    鼠    牛    虎    兔    龙    蛇    马    羊    猴    鸡    狗    猪\n\n"
       "            [32m1 [37m□ [32m= [37m%-5d       [36m□ [32m= [37m男生 (%d)    [31m□ [32m= [37m女生 (%d)[m\n"
    ,datestring,item,boy,girl);
  fclose(fp);

}
