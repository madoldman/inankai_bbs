/*-------------------------------------------------------*/
/* util/poststat.c      ( NTHU CS MapleBBS Ver 2.36 )    */
/*-------------------------------------------------------*/
/* target : Õ≥º∆ΩÒ»’°¢÷‹°¢‘¬°¢ƒÍ»»√≈ª∞Ã‚                 */
/* create : 95/03/29                                     */
/* update : 95/12/15                                     */
/*-------------------------------------------------------*/

#include <stdio.h>
#include <time.h>
#include <bbs.h>
#include <string.h>
#include <stdlib.h>

char *myfile[] = {"day", "week", "month", "year"};
int mycount[4] = {7, 4, 12};
int mytop[] = {10, 50, 100, 150};
char *mytitle[] = {"»’ Æ", "÷‹ŒÂ Æ", "‘¬“ª∞Ÿ", "ƒÍ∂»∞ŸŒÂ"};


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


struct posttop exttop[10];

int checkext()/*loveni: ÷π§…Ë÷√ Æ¥Û*/
{
	char * genbuf =NULL;
	FILE * fp;
	int i=STRLEN;
	int t=0;
	fp=fopen("etc/posts/ext","r");
	if(fp==NULL) return 0;
	while(!feof(fp))
	{
		getline(&genbuf,&i,fp);
		if(genbuf[0]=='#') continue;
		strtok(genbuf,"\n");
		if(t%2==0) strcpy(exttop[t/2].board,genbuf);
		if(t%2==1) strcpy(exttop[t/2].title,genbuf);
		t++;
		if(t>16) break;
		//printf("%s",genbuf);
	}
	t--;
	fclose(fp);
	free(genbuf);
	return t/2;
	
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
}/*loveni: Æ¥Û∫⁄√˚µ•*/

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
    if (p->date < t->date)      /* »°ΩœΩ¸»’∆⁄ */
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
  static char *logfile = ".post";
  static char *oldfile = ".post.old";

  FILE *fp;
  char buf[40], curfile[40] = "etc/posts/day.0", *p;
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
      sprintf(buf, "etc/posts/%s.%d", p, i);
      sprintf(curfile, "etc/posts/%s.%d", p, --i);
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
  sprintf(curfile, "etc/posts/%s.0", p);
  if (fp = fopen(curfile, "w"))
  {
    fwrite(top, sizeof(struct posttop), j, fp);
    fclose(fp);
  }

  sprintf(curfile, "etc/posts/%s", p);
  if (fp = fopen(curfile, "w"))
  {
    fprintf(fp, "                [1;34m-----[37m=====[41m ±æ%s¥Û»»√≈ª∞Ã‚ [40m=====[34m-----[0m\n\n", mytitle[mytype]);

    i = mytop[mytype];
  /*  if (j > i)
      j = i;
   */


   if(i==10) top10=1;
   else top10=0;

	

    int tmp=0,n=0;
	if(top10==1)
	{
		int extop=checkext();
		for(;tmp<extop;)
		{
		time_t tmptime=time(NULL);
		strcpy(buf, ctime(&tmptime));
	      buf[20] = '\0';
      		p = buf + 4;
		 fprintf(fp,
        "[1;37mµ⁄[1;31m%3d[37m √˚ [37m–≈«¯ : [33m%-20s[37m°æ[32m%s[37m°ø[36m%s [37m[35m%+14s\n"
        "     [37m±ÍÃ‚ : [1;44;37m%-60.60s[40m\n"
        ,tmp+1, exttop[tmp].board, p, "  ", "SYSOPs", exttop[tmp].title);
  		  
		tmp++;
   		}
	
	tmp=extop;
	}	
    for (n = 0; tmp<i&&n<j; n++)
    {
      
      if(notexist(&top[n],tmp)) continue;
      tmp++;
      strcpy(buf, ctime(&top[n].date));
/*      buf[20] = NULL; */
      buf[20] = '\0';
      p = buf + 4;
      fprintf(fp,
        "[1;37mµ⁄[1;31m%3d[37m √˚ [37m–≈«¯ : [33m%-20s[37m°æ[32m%s[37m°ø[36m%4d [37m»À[35m%+14s\n"
        "     [37m±ÍÃ‚ : [1;44;37m%-60.60s[40m\n"
        ,tmp , top[n].board, p, top[n].number, top[n].author, top[n].title);
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
    if (ptime->tm_mday == 1)
      poststat(2);
    if (ptime->tm_wday == 0)
      poststat(1);
    poststat(0);
  }
  poststat(-1);

	//int ext1=checkext();
	//printf("%d",ext1);
}

char board[10][STRLEN];


struct stat *f_stat(char *file) {
        static struct stat buf;
        bzero(&buf, sizeof(buf));
        if(stat(file, &buf)==-1) bzero(&buf, sizeof(buf));
        return &buf;
}
#define file_time(x) f_stat(x)->st_mtime
#define file_exist(x) (file_time(x)!=0)


int notexist(struct postrec* p,int i)
{
  char buf[STRLEN];

  /*≤ªœ‘ æ≥…‘±∞Â√Êµƒª∞Ã‚Õ≥º∆*/ 
   if(searchinfile("etc/posts/topblackb",p->board,1)) return 1;
   if(searchinfile("etc/posts/topblackt",p->title,0)) return 1;    
   if(searchinfile("etc/posts/ext",p->title,0)) return 1;
    if(!strcmp(p->board,"Blessing")) return 1;	
    sprintf(buf,"boards/%s/.DIR",p->board);
    FILE *fp=fopen(buf, "r"); 
    if(fp==0)  return 0;
    struct fileheader x;
    while(1){
        if(fread(&x, sizeof(x), 1, fp)==0)  break;
        if(!strncmp(p->title, x.title, 45)) {
           fclose(fp);
           goto out;
        }
    }
    fclose(fp);
    return 1;

/*make sure that only one post per board can be on the top10 at one time*/
   out:
   if(top10==0) return 0;
   int j=0;
   for(j=0;j<i;j++)
	\
       if(!strcmp(board[j],p->board)) return 1;
   strcpy(board[i],p->board);
   return 0;
} 
