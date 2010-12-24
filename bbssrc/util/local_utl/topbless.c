/*-------------------------------------------------------*/
/* util/topbless.c    */
/*-------------------------------------------------------*/
/* target : Í³¼Æ½ñÈÕÊ®´ó×£¸£                */
/* create : 06/04/15                                     */
/*                             */
/*-------------------------------------------------------*/

#include <stdio.h>
#include <time.h>
#include "bbs.h"

char *myfile[] = {"day", "week", "month", "year"};
int mycount[4] = {7, 4, 12};
int mytop[] = {10, 50, 100, 150};
char *mytitle[] = {"ÈÕÊ®", "ÖÜÎåÊ®", "ÔÂÒ»°Ù", "Äê¶È°ÙÎå"};
int extbless=0;
char extitle[10][STRLEN];

#define HASHSIZE 1024
#define TOPCOUNT 200

int top10;
struct postrec
{
  char author[13];              /* author name */
  char board[18];               /* board name */
  char title[66];               /* title name */
  time_t date;                  /* last post's date */
  int number;                   /* post number */
  struct postrec *next;         /* next rec */
}      *bucket[HASHSIZE];


/* 105 bytes */
struct posttop
{
  char author[13];              /* author name */
  char board[18];               /* board name */
  char title[66];               /* title name */
  time_t date;                  /* last post's date */
  int number;                   /* post number */
}       top[TOPCOUNT], postlog;


int checkext()
{
	char * genbuf =NULL;
	FILE * fp;
	int i=STRLEN;
	int t=0;
	fp=fopen("etc/bless/ext","r");
	if(fp==NULL) return 0;
	while(!feof(fp))
	{
		getline(&genbuf,&i,fp);
		if(genbuf[0]=='#') continue;
		strtok(genbuf,"\n");
		strcpy(extitle[t],genbuf);
		t++;
		if(t>9) break;
		//printf("%s",genbuf);
	}
	t--;
	fclose(fp);
	free(genbuf);
	return t;
	
}




void strtolower(char *dst,char * src)
{
    for (; *src; src++)
        *dst++ = tolower(*src);
    *dst = '\0';
}

int searchinfile(char *fname,char *str,int mode)
{
	FILE   *fp;
	char    buf[STRLEN], ptr2[200],*ptr;
	ptr = str;
	if ((fp = fopen(fname, "r")) != NULL)
	{
		if(mode==1) strtolower(ptr2, str);
			else strcpy(ptr2,str);
		while (fgets(buf, STRLEN, fp) != NULL)
		{
                   if(!strcmp(buf,"\n")) continue; 
			ptr = strtok(buf, " \n\t\r");
               	if(mode==1)	strtolower(ptr,ptr); 
			if (ptr != NULL && *ptr != '#')
			{
				if(ptr[0] == '*' && strstr(ptr2, &ptr[1]) != NULL) 
				{
					fclose(fp);
					return 1;
				}
				if(ptr[0] != '*' && !strcmp(ptr2,ptr))
				{
						fclose(fp);
						return 1;
				}
			}
		}
		fclose(fp);
	}
	return 0;
}/*loveni:Ê®´óºÚÃûµ¥*/







int
hash(key)
  char *key;
{
  int i, value = 0;

  for (i = 0; key[i] && i < 80; i++)
    value += key[i] < 0 ? -key[i] : key[i];

  value = value % HASHSIZE;
  return value;
}


/* ---------------------------------- */
/* hash structure : array + link list */
/* ---------------------------------- */


void
search(t)
  struct posttop *t;
{
  struct postrec *p, *q, *s;
  int i, found = 0;

  i = hash(t->title);
  q = NULL;
  p = bucket[i];
  while (p && (!found))
  {
    if (!strncmp(p->title, t->title, 40) && !strcmp(p->board, t->board))
      found = 1;
    else
    {
      q = p;
      p = p->next;
    }
  }
  if (found)
  {
    p->number += t->number;
    if (p->date < t->date)      /* È¡½Ï½üÈÕÆÚ */
      p->date = t->date;
  }
  else
  {
    s = (struct postrec *) malloc(sizeof(struct postrec));
    memcpy(s, t, sizeof(struct posttop));
    s->next = NULL;
    if (q == NULL)
      bucket[i] = s;
    else
      q->next = s;
  }
}


int
sort(pp, count)
  struct postrec *pp;
{
  int i, j;

  for (i = 0; i <= count; i++)
  {
    if (pp->number > top[i].number)
    {
      if (count < TOPCOUNT - 1)
        count++;
      for (j = count - 1; j >= i; j--)
        memcpy(&top[j + 1], &top[j], sizeof(struct posttop));

      memcpy(&top[i], pp, sizeof(struct posttop));
      break;
    }
  }
  return count;
}


void
load_stat(fname)
  char *fname;
{
  FILE *fp;

  if (fp = fopen(fname, "r"))
  {
    int count = fread(top, sizeof(struct posttop), TOPCOUNT, fp);
    fclose(fp);
    while (count)
      search(&top[--count]);
  }
}


void
poststat(mytype)
  int mytype;
{
  static char *logfile = ".bless";
  static char *oldfile = ".bless.old";

  FILE *fp;
  char buf[40], curfile[40] = "etc/bless/day.0", *p;
  struct postrec *pp;
  int i, j;

  if (mytype < 0)
  {
    /* --------------------------------------- */
    /* load .post and statictic processing     */
    /* --------------------------------------- */

    remove(oldfile);
    rename(logfile, oldfile);
    if ((fp = fopen(oldfile, "r")) == NULL)
      return;
    mytype = 0;
    load_stat(curfile);

    while (fread(top, sizeof(struct posttop), 1, fp))
      search(top);
    fclose(fp);
  }
  else
  {
    /* ---------------------------------------------- */
    /* load previous results and statictic processing */
    /* ---------------------------------------------- */

    i = mycount[mytype];
    p = myfile[mytype];
    while (i)
    {
      sprintf(buf, "etc/bless/%s.%d", p, i);
      sprintf(curfile, "etc/bless/%s.%d", p, --i);
      load_stat(curfile);
      rename(curfile, buf);
    }
    mytype++;
  }

  /* ---------------------------------------------- */
  /* sort top 100 issue and save results            */
  /* ---------------------------------------------- */

  memset(top, 0, sizeof(top));
  for (i = j = 0; i < HASHSIZE; i++)
  {
    for (pp = bucket[i]; pp; pp = pp->next)
    {

#ifdef  DEBUG
      printf("Title : %s, Board: %s\nPostNo : %d, Author: %s\n"
        ,pp->title
        ,pp->board
        ,pp->number
        ,pp->author);
#endif

      j = sort(pp, j);
    }
  }

  p = myfile[mytype];
  sprintf(curfile, "etc/bless/%s.0", p);
  if (fp = fopen(curfile, "w"))
  {
    fwrite(top, sizeof(struct posttop), j, fp);
    fclose(fp);
  }

  sprintf(curfile, "etc/bless/%s", p);
  if (fp = fopen(curfile, "w"))
  {
    fprintf(fp, "              \x1b[1;33m©¤©¤ \x1b[31m¡î\x1b[33m¡î\x1b[32m¡î \x1b[41;32m  \x1b[33m±¾ÈÕÊ®´óÖÔÐÄ×£¸£  \x1b[m\x1b[1;32m ¡î\x1b[31m¡î\x1b[33m¡î ©¤©¤\x1b[m\n\n");

    i = mytop[mytype];


   if(i==10) top10=1;
   else top10=0;

    int tmp=0,n=0;
	int exbless=checkext();
	for(;tmp<exbless;)
	{
		time_t tmptime=time(NULL);
		strcpy(buf, ctime(&tmptime));
	      buf[20] = '\0';
      		p = buf + 4;
		fprintf(fp,
        "[1;37mµÚ[1;31m%3d[37m Ãû [37m    [33m%-20s[37m¡¾[32m%s[37m¡¿[36m%6s [37m[35m%+14s\n"
        "     [37m±êÌâ : [1;41;37m%-60.60s[40m\n"
        ,tmp+1 , "   ", p,"  ", "SYSOPs", extitle[tmp]);
		tmp++;
    }
	
	tmp=exbless;		
    for (n = 0; tmp<i&&n<j; n++)
    {
        if(strstr(top[n].title,"¡¾ºÏ¼¯¡¿")) continue;
	if(searchinfile("etc/posts/topblackt",top[n].title,0)) continue;
	if(searchinfile("etc/posts/ext",top[n].title,0)) continue;
	tmp++;
	
      strcpy(buf, ctime(&top[n].date));

      buf[20] = '\0';
      p = buf + 4;
      fprintf(fp,
        "[1;37mµÚ[1;31m%3d[37m Ãû [37m    [33m%-20s[37m¡¾[32m%s[37m¡¿[36m%4d [37mÈË[35m%+14s\n"
        "     [37m±êÌâ : [1;41;37m%-60.60s[40m\n"
        ,tmp , "   ", p, top[n].number, top[n].author, top[n].title);
    }
    fclose(fp);
  }

  /* free statistics */
  for (i = 0; i < HASHSIZE; i++)
  {
    struct postrec *pp0;

    pp = bucket[i];
    while (pp)
    {
       pp0 = pp;
       pp = pp->next;
       free(pp0);
    }
    bucket[i] = NULL;
  }
}


main(argc, argv)
  char *argv[];
{
  time_t now;
  struct tm *ptime;

  if (argc < 2)
  {
    printf("Usage:\t%s bbshome [day]\n", argv[0]);
    return (-1);
  }

  chdir(argv[1]);

  if (argc == 3)
  {
    poststat(atoi(argv[2]));
    return (0);
  }

  time(&now);
  ptime = localtime(&now);
  if (ptime->tm_hour == 8)
  {
    poststat(0);
  }
  poststat(-1);

}
