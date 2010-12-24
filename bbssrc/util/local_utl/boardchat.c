/* $Id: boardchat.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $ */

#include <time.h>
#include <stdio.h>
#include "bbs.h"
#include "config.h"

#define BBSBOARDS BBSHOME"/.BOARDS"
//#define MAXBUF	3000
char    datestring[30];
//extern int flock(int,int);
//char	buf2[MAXBUF][14];
#define BUFSIZE (8192)
 
struct binfo
{
  char  boardname[18];
  char  expname[28];
  int times;
  int sum;
/*
	int num;  //�������
	char boardname[18]; //��������
	char  expname[28];
	int times;  //�˴�
	int times2;  //����
	int sum;  //��ͣ��ʱ��
	int chg1;  //���������β�ֵ������ʾ����
	int chg2;  //��ǰ�����β�ֵ������ʾ����
	int chg3;  //���ǰ�������β�ֵ������ʾ����
*/
} st[MAXBOARD];

int numboards=0;

int 
brd_cmp(b, a)
struct binfo *a, *b;
{
    if(a->times!=b->times)
            return (a->times - b->times);
    return a->sum-b->sum;
}

int report()
{
        return ;
}
/* Added by deardragon 1999.12.2 */
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
/* Added End. */

int
record_data(board,sec)
char *board;
int sec;
{
        int i;
        for(i=0;i<numboards;i++)
        {
                if(!strcmp(st[i].boardname,board))
                {
                        st[i].times++;
                        st[i].sum+=sec;
                        return;
                }
        }
        return ;
}

int
fillbcache(fptr)
struct boardheader *fptr ;
{

    if( numboards >= MAXBOARD )
        return 0;
    if(((fptr->level!=0)&&!(fptr->level&PERM_NOZAP||fptr->level&PERM_POSTMASK))||strlen(fptr->filename)==0)
        return;
    strcpy(st[numboards].boardname,fptr->filename);
    strcpy(st[numboards].expname,fptr->title+8);
/*    printf("%s %s\n",st[numboards].boardname,st[numboards].expname); */
    st[numboards].times=0;
    st[numboards].sum=0;
    numboards++;
    return 0 ;
}

int 
fillboard()
{
        apply_record( BBSBOARDS, fillbcache, sizeof(struct boardheader) );
}

char *
timetostr(i)
int i;
{
        static char str[30];
        int minute,sec,hour;

        minute=(i/60);
        hour=minute/60;
        minute=minute%60;
        sec=i&60;
        sprintf(str,"%2d:%2d:%2d",hour,minute,sec);
        return str;
}

main(argc, argv)
char *argv[];
{
  FILE *fp;
  FILE *op;
  char buf[256], *p,bname[20];
  char date[80];
  int mode=1;
  int c[3];
  int max[3];
  unsigned int ave[3];
  int now,sec;
  int i, j,k;
  char    *blk[10] =
  {
                "��", "��", "�x", "�y", "�z",
                "�{", "�|", "�}", "�~", "��",
  };

  mode=atoi(argv[1]);

  sprintf(buf,"%s/use_board", BBSHOME);
  
  if ((fp = fopen(buf, "r")) == NULL)
  {
    printf("cann't open use_board\n");
    return 1;
  }

  sprintf(buf,"%s/0Announce/bbslist/boardchart", BBSHOME);

  if ((op = fopen(buf, "w")) == NULL)
  {
    printf("Can't Write file\n");
    return 1;
  }

  fillboard();
  now=time(0);
  getdatestring(now);
  sprintf(date,"%14.14s",datestring);
  while (fgets(buf, 256, fp))
  {
    if(strlen(buf)<57)
        continue;
    if(strstr(buf,date)!=buf) continue;
    if ( !strncmp(buf+23, "USE", 3))
    {
      p=strstr(buf,"USE");
      p+=4;
      p=strtok(p," ");
      strcpy(bname,p);
    if ( p = (char *)strstr(buf+48, "Stay: "))
    {
      sec=atoi( p + 6);
    }
    else
        sec=0;
    record_data(bname,sec);
    }
   }
   fclose(fp);
   qsort(st, numboards, sizeof( st[0] ), brd_cmp);
   ave[0]=0;
   ave[1]=0;
   ave[2]=0;
   max[1]=0;
   max[0]=0;
   max[2]=0;
   for(i=0;i<numboards;i++)
   {
        ave[0]+=st[i].times;
        ave[1]+=st[i].sum;
        ave[2]+=st[i].times==0?0:st[i].sum/st[i].times;
        if(max[0]<st[i].times)
        {
                max[0]=st[i].times;
        }
        if(max[1]<st[i].sum)
        {
                max[1]=st[i].sum;
        }
        if(max[2]<(st[i].times==0?0:st[i].sum/st[i].times))
        {
                max[2]=(st[i].times==0?0:st[i].sum/st[i].times);
        }
   }
   c[0]=max[0]/30+1;
   c[1]=max[1]/30+1;
   c[2]=max[2]/30+1;
   numboards++;
   st[numboards-1].times=ave[0]/numboards;
   st[numboards-1].sum=ave[1]/numboards;
   strcpy(st[numboards-1].boardname,"Average");
   strcpy(st[numboards-1].expname,"��ƽ��");
   if(mode==1)
   {
        fprintf(op,"[1;37m���� %-15.15s%-28.28s %5s %8s %10s[m\n","����������","��������","�˴�","�ۻ�ʱ��","ƽ��ʱ��");
   }else
   {
        fprintf(op,"      [1;37m1 [m[34m%2s[1;37m= %d (���˴�) [1;37m1 [m[32m%2s[1;37m= %s (�ۻ���ʱ��) [1;37m1 [m[31m%2s[1;37m= %d ��(ƽ��ʱ��)\n\n",
                blk[9],c[0],blk[9],timetostr(c[1]),blk[9],c[2]);
   }

   for(i=0;i<numboards;i++)
   {
      if(mode==1)
        fprintf(op,"[1m%4d[m %-15.15s%-28.28s %5d %-.8s %10d\n",i+1,st[i].boardname,st[i].expname,st[i].times,timetostr(st[i].sum),st[i].times==0?0:st[i].sum/st[i].times);
      else
      {
        fprintf(op,"      [1;37m��[31m%3d [37m�� ���������ƣ�[31m%s [35m%s[m\n",i+1,st[i].boardname,st[i].expname);
        fprintf(op,"[1;37m    ��������������������������������������������������������������������������\n");
        fprintf(op,"[1;37m�˴Ω�[m[34m");
        for(j=0;j<st[i].times/c[0];j++)
        {
                fprintf(op,"%2s",blk[9]);
        }
                fprintf(op,"%2s [1;37m%d[m\n",blk[(st[i].times%c[0])*10/c[0]],st[i].times);
        fprintf(op,"[1;37mʱ�䩦[m[32m");
        for(j=0;j<st[i].sum/c[1];j++)
        {
                fprintf(op,"%2s",blk[9]);
        }
                fprintf(op,"%2s [1;37m%s[m\n",blk[(st[i].sum%c[1])*10/c[1]],timetostr(st[i].sum));
        j=st[i].times==0?0:st[i].sum/st[i].times;
        fprintf(op,"[1;37mƽ����[m[31m");
        for(k=0;k<j/c[2];k++)
        {
                fprintf(op,"%2s",blk[9]);
        }
                fprintf(op,"%2s [1;37m%s[m\n",blk[(j%c[2])*10/c[2]],timetostr(j));
        fprintf(op,"[1;37m    ��������������������������������������������������������������������������[m\n\n");
      }
   }
   fclose(op);
}
