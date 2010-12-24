/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: boards.c,v 1.2 2009-01-04 08:45:06 madoldman Exp $
*/

#include "bbs.h"

#define BBS_PAGESIZE    (t_lines - 4)

/*
#define BRC_MAXSIZE     80000
exceed when boardsize>310
*/

#define BRC_MAXNUM      60
#define BRC_STRLEN      20
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))
//#define GOOD_BRC_NUM	40

#define BRC_MAXSIZE     (MAXBOARD+2)*BRC_ITEMSIZE

char    brc_buf[BRC_MAXSIZE];
int     brc_size, brc_changed = 0;
char    brc_name[BRC_STRLEN];
int     brc_list[BRC_MAXNUM], brc_num;

int sign=0;
/*thunder 2003.7.16
  0 Õý³£
  1 ÈÈÃÅÌÖÂÛÇø
  2 ÍÆ¼öÌÖÂÛÇø
  3 ÐÂ¿ªÌÖÂÛÇø
    */

char   *sysconf_str();
extern time_t login_start_time;
extern int numboards;
extern struct shortfile *bcache;
struct newpostdata
{
	char   *name, *title, *BM;
	char    flag;
	int     pos, total;
	char    unread, zap;
	char    status;
}
*nbrd;

struct goodboard
{
	char ID[GOOD_BRC_NUM][BRC_STRLEN];
	int	num;
}
GoodBrd;

int    *zapbuf;
int     brdnum, yank_flag = 0;
char   *boardprefix;
int     allboards=0;//Read All Board±êÖ¾ Added by smalldog

void EGroup(char* cmd)
{
	sign=0;
	char    buf[STRLEN];
	sprintf(buf, "EGROUP%c", *cmd);
	GoodBrd.num = 0;
	boardprefix = sysconf_str(buf);
	choose_board(DEFINE(DEF_NEWPOST) ? 1 : 0 ,NULL);
}

/*
 if board in this sec,return 0 else 1
*/
int insec(int n,char *board)
{
    char    buf[STRLEN],*p,c;
    switch(n) {
       case 13:
              c=' ';
              break;
       case 12:
              c='N';
              break;
       case 11:
              c='G';
              break;
       case 10:
              c='C';
              break;
       default:
              c=n+'0';
   }

    sprintf(buf, "EGROUP%c", c);
    p=sysconf_str(buf);
    struct shortfile *bp=getbcache(board);
    if(strchr(p,bp->title[0])) 
        return 0;
    else
        return 1;
} 

void Boards()
{
	sign=0;
	boardprefix = NULL;
	GoodBrd.num = 0;
	allboards=1;
	choose_board(0,NULL);
	allboards=0;
}

void GoodBrds()
{
	sign=0;
	if(!strcmp(currentuser.userid,"guest"))
		return;
	GoodBrd.num = 9999;
	boardprefix = NULL;
	choose_board(1,NULL);
}




/*thunder 2003.7.16*/
void  Hotboards()
{
	prints("hello");
	sign=1;
	GoodBrd.num =0;
	boardprefix = NULL;
	choose_board(1,NULL);
}

void  Newboards()
{
	sign=3;
	GoodBrd.num =0;
	boardprefix = NULL;
	choose_board(1,NULL);
}

void  Cmdboards()
{
	sign=2;
	GoodBrd.num =0;
	boardprefix = NULL;
	choose_board(1,NULL);
}

/*
  Ó¦¿¼ÂÇ½«ÈÈÃÅ,ÍÆ¼ö,ÐÂ¿ª×°Èë¹²ÏíÄÚ´æ
  ÏÖÔÚÊÇÃ¿´Î¶¼¶ÁÎÄ¼þ,Ð§ÂÊµÍ
  */
int  brdinfile(char *bname)
{

	int i;
	FILE *fp;
	char buf[STRLEN],buf2[STRLEN];
	char path[STRLEN];
	char *file[3]={
	                  "hotboards",
	                  "boards",
	                  "newboards"
	              };
	sprintf(path,"/home/www/html/news/%s",file[sign-1]);
	if( ( fp = fopen(path,"r"))!=NULL)
	{
		while(1)
		{
			if(fscanf(fp, "%s %s \n", buf,buf2) != EOF)
			{
				if ( !strcmp(bname, buf))
					return i+1;
				else
					i--;
			}
			else
				break;
			i++;
		}
		fclose(fp);
	}
	return 0;
}



void New()
{
	if (heavyload())
	{
		clear();
		prints("±§Ç¸£¬Ä¿Ç°ÏµÍ³¸ººÉ¹ýÖØ£¬Çë¸ÄÓÃ Boards Ö¸ÁîÔÄÀÀÌÖÂÛÇø...");
		pressanykey();
		return;
	}
	sign = 0;
	boardprefix = NULL;
	GoodBrd.num = 0;
	allboards=1;
	choose_board(1,NULL);
	allboards=0;
}

int inGoodBrds(char *bname)
{
	int i;
	for ( i = 0; i < GoodBrd.num && i< GOOD_BRC_NUM; i ++ )
		if ( !strcmp(bname, GoodBrd.ID[i]))
			return i+1;
	return 0;
}

void load_zapbuf()
{
	char    fname[STRLEN];
	int     fd, size, n;

	size = MAXBOARD * sizeof(int);
	zapbuf = (int *) malloc(size);
	for (n = 0; n < MAXBOARD; n++)
		zapbuf[n] = 1;
	setuserfile(fname, ".lastread");
	if ((fd = open(fname, O_RDONLY, 0600)) != -1)
	{
		size = numboards * sizeof(int);
		read(fd, zapbuf, size);
		close(fd);
	}
}
void loadbrdfromfile(char *s)
{
        int i=0;
  	FILE *fp;
        GoodBrd.num = 0;
        if((fp = fopen(s,"r"))){
          for ( i = 0; i< GOOD_BRC_NUM ; i++)
                  if(fscanf(fp, "%s\n", GoodBrd.ID[i]) != EOF){
                          if( getbnum(GoodBrd.ID[i]) )
                                  GoodBrd.num ++;
                          else
                                  i--;//add by yiyo
                  }
                  else
                          break;
          fclose(fp);
        }
}

void load_GoodBrd()
{
	char fname[STRLEN];
	GoodBrd.num = 0;
	setuserfile(fname,".goodbrd");
	loadbrdfromfile(fname);
	/*********************************************************************
	  Do you kown  what work  this code does?
	if (GoodBrd.num&&!HAS_PERM(PERM_BOARDS)&&inGoodBrds("Boardmaster"))
	{
		pos=inGoodBrds("Boardmaster");
		for(i = pos-1; i< GoodBrd.num-1; i++)
			strcpy(GoodBrd.ID[i],"Boardmaster");
		GoodBrd.num --;
		save_GoodBrd();
		//         GoodBrd.num = 9999;
	}
	**********************************************************************/
       
        if(GoodBrd.num==1&&inGoodBrds("sysop"))
           GoodBrd.num=0;

	if (GoodBrd.num == 0){
		strcpy(fname,"etc/fav");
                loadbrdfromfile(fname);
	}

        if (GoodBrd.num == 0){
      	        GoodBrd.num = 1;
                if(getbcache(DEFAULTBOARD))
                   strcpy(GoodBrd.ID[0],DEFAULTBOARD);
                else
                   strcpy(GoodBrd.ID[0],currboard);
        }
}

int save_GoodBrd()
{
	int i;
	FILE *fp;
	char fname[STRLEN];
        
	if( GoodBrd.num <= 0 ) return 0;
	/*{
		GoodBrd.num = 1;
		if(getbcache(DEFAULTBOARD))
			strcpy(GoodBrd.ID[0],DEFAULTBOARD);
		else
			strcpy(GoodBrd.ID[0],currboard);
	}
        */
        
	setuserfile(fname,".goodbrd");
	if( (fp = fopen(fname,"wb+")) != NULL)
	{
		for (i=0; i< GoodBrd.num; i++)
			fprintf(fp, "%s\n", GoodBrd.ID[i]);
		fclose(fp);
	}
}

void save_zapbuf()
{
	char    fname[STRLEN];
	int     fd, size;
	setuserfile(fname, ".lastread");
	if ((fd = open(fname, O_WRONLY | O_CREAT, 0600)) != -1)
	{
		size = numboards * sizeof(int);
		write(fd, zapbuf, size);
		close(fd);
	}
}

int count_zone(char *filename,char *zonename)
{
        int num=0;
        FILE *fp;
        char tempbuf[STRLEN];
        struct boardheader myheader;
        if((fp=fopen(filename,"r"))==NULL)
        {
                return 0;
        }
        while(fread(&myheader,sizeof(struct boardheader),1,fp)>0)
        {
                if((myheader.flag & CHILDBOARD_FLAG) && strcmp(myheader.owner,zonename)==0)
                {
                        if(!(myheader.filename[0]))
                                continue;
			if (!(myheader.level & PERM_POSTMASK) && !HAS_PERM(myheader.level) && !(myheader.level & PERM_NOZAP))
                                continue;
#ifdef BOARD_CONTROL
                        //if( !HAS_PERM(PERM_SYSOP))
			if( !HAS_PERM(PERM_SYSOP) && (myheader.flag2 & HIDE_FLAG ))
                        {//loveni:·â±Õ¾ãÀÖ²¿°åÃæ¿É¼û¿ØÖÆ
                                setbfile(tempbuf, myheader.filename, "board.allow");
                                if(dashf(tempbuf)&&!seek_in_file(tempbuf,currentuser.userid))
                                continue;
                        }
#endif
                        num++;
                }
		if((myheader.flag & ZONE_FLAG) && strcmp(myheader.owner,zonename)==0)
		{
			if(count_zone(BOARDS,myheader.filename)!=0 || HAS_PERM(PERM_SYSOP))
				num++;
		} 
        }
	fclose(fp);
        return num;
}

int load_boards(char *zonename)
{
	struct shortfile *bptr;
	struct newpostdata *ptr;
	int     n, addto = 0, goodbrd = 0;
	resolve_boards();
	if (zapbuf == NULL)
	{
		load_zapbuf();
	}

	brdnum = 0;
	if( GoodBrd.num == 9999 )
	{
		load_GoodBrd();
		goodbrd = 1;
	}

	for (n = 0; n < numboards; n++)
	{
		bptr = &bcache[n];
		if(!(bptr->filename[0]))
			continue; /* Òþ²Ø±»É¾³ýµÄ°åÃæ */
		if ( goodbrd == 0)
		{
			if(sign == 0)
			{
                        if(zonename!=NULL)
                        {
                                if(!(bptr->flag & CHILDBOARD_FLAG) && strcmp(bptr->owner,zonename)!=0)
                                        continue;
                                if(strcmp(bptr->owner,zonename)!=0)
                                        continue;
				if((bptr->flag & ZONE_FLAG) && (count_zone(BOARDS,bptr->filename)==0) && !HAS_PERM(PERM_SYSOP))
                                        continue;
                        }
                        else if(allboards==1)
                        {
                                if(bptr->flag & ZONE_FLAG)
                                        continue;
                        }
                        else
                        {
                                if(bptr->flag & CHILDBOARD_FLAG)
                                        continue;
				if((bptr->flag & ZONE_FLAG) && (count_zone(BOARDS,bptr->filename)==0) && !HAS_PERM(PERM_SYSOP))
					continue;
				if((bptr->flag & ZONE_FLAG) &&  strcmp(bptr->owner,"")!=0)
					continue;
                        }
			if (!(bptr->level & PERM_POSTMASK) && !HAS_PERM(bptr->level) && !(bptr->level & PERM_NOZAP))
				continue;
			if (boardprefix != NULL && strchr(boardprefix, bptr->title[0]) == NULL && boardprefix[0] != '*')
				continue;
			if (boardprefix != NULL && boardprefix[0] == '*')
			{
				if (!strstr(bptr->title, "¡ñ") && !strstr(bptr->title, "¡Ñ") && bptr->title[0] != '*')
					continue;
			}
			if (boardprefix == NULL && bptr->title[0] == '*')
				continue;
#ifdef BOARD_CONTROL
        // if(!HAS_PERM(PERM_SYSOP)&&!chk_currBM(currBM)){
	//		if( !HAS_PERM(PERM_SYSOP) && !(bptr->flag & ZONE_FLAG))
        		if( !HAS_PERM(PERM_SYSOP) && !(bptr->flag & ZONE_FLAG)
&&(bptr->flag2 & HIDE_FLAG)) //loveni:·â±Õ¾ãÀÖ²¿°åÃæ¿É¼û¿ØÖÆ
			{
                		setbfile(genbuf, bptr->filename, "board.allow");
                		if(dashf(genbuf)&&!seek_in_file(genbuf,currentuser.userid))
                      			continue; 
        		}
#endif
			/*	thunder */
			/* addto = yank_flag || zapbuf[n] != 0 || (bptr->level & PERM_NOZAP);*/
			//if(sign==0)
				addto = yank_flag || zapbuf[n] != 0 || (bptr->level & PERM_NOZAP);
			}
			else
				addto=  brdinfile(bptr->filename);


		}
		else
			addto = inGoodBrds(bptr->filename);
		if ( addto )
		{
			ptr = &nbrd[brdnum++];
			ptr->name = bptr->filename;
			ptr->title = bptr->title;
			ptr->BM = bptr->BM;
			ptr->flag = bptr->flag | ((bptr->level & PERM_NOZAP) ? NOZAP_FLAG : 0);
			ptr->pos = n;
			ptr->total = -1;
			ptr->zap = (zapbuf[n] == 0);
			if (bptr->level & PERM_POSTMASK)
				ptr->status = 'p';
			else if (bptr->level & PERM_NOZAP)
				ptr->status = 'z';
			else if (ptr->flag & NOREPLY_FLAG)
				ptr->status = 'x';
			else if ((bptr->level & ~PERM_POSTMASK) != 0)
				ptr->status = 'r';
			else
				ptr->status = ' ';
		}
	}
	if (brdnum == 0 && !yank_flag && boardprefix == NULL)
	{
		if(goodbrd)
		{
			GoodBrd.num = 0;
			save_GoodBrd();
			GoodBrd.num = 9999;
		}
		brdnum = -1;
		yank_flag = 1;
		return -1;
	}


	return 0;
}

int search_board(int* num)
{
	static int i = 0, find = YEA;
	static char bname[STRLEN];
	int     n, ch, tmpn = NA;
	if (find == YEA)
	{
		bzero(bname, sizeof(bname));
		find = NA;
		i = 0;
	}
	while (1)
	{
		move(t_lines - 1, 0);
		clrtoeol();
		prints("ÇëÊäÈëÒªÕÒÑ°µÄ board Ãû³Æ£º%s", bname);
		ch = egetch();

		if (isprint2(ch))
		{
			bname[i++] = ch;
			for (n = 0; n < brdnum; n++)
			{
				if (!ci_strncmp(nbrd[n].name, bname, i))
				{
					tmpn = YEA;
					*num = n;
					if (!strcmp(nbrd[n].name, bname))
						return 1	/* ÕÒµ½ÀàËÆµÄ°å£¬»­ÃæÖØ»­
						              							    */ ;
				}
			}
			if (tmpn)
				return 1;
			if (find == NA)
			{
				bname[--i] = '\0';
			}
			continue;
		}
		else if (ch == Ctrl('H') || ch == KEY_LEFT || ch == KEY_DEL ||
		         ch == '\177')
		{
			i--;
			if (i < 0)
			{
				find = YEA;
				break;
			}
			else
			{
				bname[i] = '\0';
				continue;
			}
		}
		else if (ch == '\t')
		{
			find = YEA;
			break;
		}
		else if (ch == '\n' || ch == '\r' || ch == KEY_RIGHT)
		{
			find = YEA;
			break;
		}
		bell(1);
	}
	if (find)
	{
		move(t_lines - 1, 0);
		clrtoeol();
		return 2 /* ½áÊøÁË */ ;
	}
	return 1;
}

int check_newpost(struct newpostdata* ptr)
{
	struct fileheader fh;
	struct stat st;
	char    filename[STRLEN];
	int     fd, offset, total;
	ptr->total = ptr->unread = 0;
	setbdir(genbuf, ptr->name);
	/*
	while(1){
	if ((fd = open(genbuf, O_RDWR)) == -1){
	            usleep(250);
	            continue;
	        }
	break;
	}
	*/

	if ((fd = open(genbuf, O_RDONLY)) == -1)
		return 0;

	if( fstat(fd, &st) == -1)
	{
		return 0;
	}

	total = st.st_size / sizeof(fh);
	if (total <= 0)
	{
		close(fd);
		return 0;
	}
	ptr->total = total;
	if (!brc_initial(ptr->name))
	{
		ptr->unread = 1;
	}
	else
	{
		offset = (int) ((char *) &(fh.filename[0]) - (char *) &(fh));
		lseek(fd, (off_t) (offset + (total - 1) * sizeof(fh)), SEEK_SET);
		if (read(fd, filename, STRLEN) > 0 && brc_unread(filename))
		{
			ptr->unread = 1;
		}
	}
	close(fd);
	return 1;
}

int unread_position(char* dirfile, struct newpostdata* ptr)
{
	struct fileheader fh;
	char    filename[STRLEN];
	int     fd, offset, step, num;
	num = ptr->total + 1;
	if (ptr->unread && (fd = open(dirfile, O_RDWR)) > 0)
	{
		if (!brc_initial(ptr->name))
		{
			num = 1;
		}
		else
		{
			offset = (int) ((char *) &(fh.filename[0]) - (char *) &(fh));
			num = ptr->total - 1;
			step = 4;
			while (num > 0)
			{
				lseek(fd, (off_t) (offset + num * sizeof(fh)), SEEK_SET);
				if (read(fd, filename, STRLEN) <= 0 ||
				        !brc_unread(filename))
					break;
				num -= step;
				if (step < 32)
					step += step / 2;
			}
			if (num < 0)
				num = 0;
			while (num < ptr->total)
			{
				lseek(fd, (off_t) (offset + num * sizeof(fh)), SEEK_SET);
				if (read(fd, filename, STRLEN) <= 0 ||
				        brc_unread(filename))
					break;
				num++;
			}
		}
		close(fd);
	}
	if (num < 0)
		num = 0;
	return num;
}

void show_brdlist(int page, int clsflag, int newflag)
{
	struct newpostdata *ptr;
	int     n;
	char    tmpBM[BM_LEN - 1];
	if (clsflag)
	{
		clear();
		docmdtitle("[ÌÖÂÛÇøÁÐ±í]", " [mÖ÷Ñ¡µ¥[[1;32m¡û[m,[1;32me[m] ÔÄ¶Á[[1;32m¡ú[m,[1;32mRtn[m] Ñ¡Ôñ[[1;32m¡ü[m,[1;32m¡ý[m] ÁÐ³ö[[1;32my[m] ÅÅÐò[[1;32ms[m] ËÑÑ°[[1;32m/[m] ÇÐ»»[[1;32mc[m] ÇóÖú[[1;32mh[m]\n");
		prints("[1;44;37m %s Î´ ÌÖÂÛÇøÃû³Æ       V  Àà±ð  ×ª %-21s S °å  Ö÷      ÔÚÏß[m\n",
		       newflag ? " È«²¿" : " ±àºÅ", "ÖÐ  ÎÄ  Ðð  Êö");
	}
	move(3, 0);
	for (n = page; n < page + BBS_PAGESIZE; n++)
	{
		if (n >= brdnum)
		{
			prints("\n");
			continue;
		}
		ptr = &nbrd[n];
		if (ptr->total == -1)
		{
			//				refresh();
			check_newpost(ptr);
		}
		//if(ptr->total!=0)
		if(ptr->flag & ZONE_FLAG)
                {
                        int tempi;
                        tempi=count_zone(BOARDS,ptr->name);
                        prints(" %5d %s",newflag?tempi:n+1,"£«");
                        prints(" %-16s   %-31s %c\n",ptr->name,ptr->title+1,HAS_PERM(PERM_POST)?ptr->status:' ');
                }
		else
		{
			prints(" %5d %s", newflag ? ptr->total:n+1,ptr->unread ? "¡ô" : "¡ó");
		//else
		//   prints(" %7s", "¡ó");
			strcpy(tmpBM, ptr->BM);
			prints("%c%-16s %s %-31s %c %-12s %3d\n",(ptr->zap&&!(ptr->flag&NOZAP_FLAG))?'*':' ', ptr->name, (ptr->flag & VOTE_FLAG) ? "[1;31mV[m" : " ",
		       	ptr->title + 1, HAS_PERM(PERM_POST) ? ptr->status : ' ',
#ifdef AddWaterONLY
		       	ptr->BM[0] <= ' ' ? "»¶Ó­¹âÁÙ" : strtok(tmpBM, " "),
#else
			ptr->BM[0] <= ' ' ? "³ÏÕ÷°åÖ÷ÖÐ" : strtok(tmpBM, " "),
#endif
		       	ShowBoardOnline(ptr->name));
		}
	}
	refresh();
}

int cmpboard(struct newpostdata *brd,struct newpostdata *tmp)
{
	register int type = 0;
	if (!(currentuser.flags[0] & BRDSORT_FLAG))
	{
		type = brd->title[0] - tmp->title[0];
		if((brd->flag & ZONE_FLAG) && !(tmp->flag & ZONE_FLAG))
                        type = -1;
                if(!(brd->flag & ZONE_FLAG) && (tmp->flag & ZONE_FLAG))
                        type = 1;
		if (type == 0)
			type = ci_strncmp(brd->title + 1, tmp->title + 1, 6);

	}
	if (type == 0)
	{
		if((brd->flag & ZONE_FLAG) && !(tmp->flag & ZONE_FLAG))
                {
                        type = -1;
                }
                if(!(brd->flag & ZONE_FLAG) && (tmp->flag & ZONE_FLAG))
                {
                        type = 1;
                }
		if(type==0)
		{
			type = ci_strcmp(brd->name, tmp->name);
		}
	}
	return type;
}

int choose_board(int newflag,char *boardname)
{
	static int num;
	struct newpostdata newpost_buffer[MAXBOARD];
	struct newpostdata *ptr;
	int     page, ch, tmp, number, tmpnum;
	int     loop_mode = 0;
        int     zoneout=0;
        int     zoneoutpage,zoneoutnum;
start:
	if (!strcmp(currentuser.userid, "guest"))
		yank_flag = 1;
	nbrd = newpost_buffer;
	modify_user_mode(newflag ? READNEW : READBRD);
	brdnum = number = 0;
	clear();
	//show_brdlist(0, 1, newflag);
	while (1)
	{
		if (brdnum <= 0)
		{
			if (load_boards(boardname) == -1)
			{
				continue;
			}
			qsort(nbrd, brdnum, sizeof(nbrd[0]), cmpboard);
			page = -1;
			if (brdnum <= 0)
				break;
		}
		if (num < 0)
			num = 0;
		if (num >= brdnum)
			num = brdnum - 1;
		if (page < 0)
		{
			if (newflag)
			{
				tmp = num;
				while (num < brdnum)
				{
					ptr = &nbrd[num];
					if (ptr->total == -1)
						check_newpost(ptr);
					if (ptr->unread)
						break;
					num++;
				}
				if (num >= brdnum)
					num = tmp;
			}
			page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
			if(zoneout==1)
                        {
                                num=zoneoutnum;
                                page=zoneoutpage;
                                zoneout=0;
                        }
			show_brdlist(page, 1, newflag);
			update_endline();
		}
		if (num < page || num >= page + BBS_PAGESIZE)
		{
			page = (num / BBS_PAGESIZE) * BBS_PAGESIZE;
			if(zoneout==1)
                        {
                                num=zoneoutnum;
                                page=zoneoutpage;
                                zoneout=0;
                        }
			show_brdlist(page, 0, newflag);
			update_endline();
		}
		move(3 + num - page, 0);
		prints(">", number);
		if (loop_mode == 0)
		{
			ch = egetch();
		}
		move(3 + num - page, 0);
		prints(" ");
		if (ch == 'q' || ch == 'e' || ch == KEY_LEFT || ch == EOF)
			break;
		switch (ch)
		{
		case 'P':
		case 'b':
		case KEY_PGUP:
		case Ctrl('B'):
						if (num == 0)
							num = brdnum - 1;
				else
					num -= BBS_PAGESIZE;
			break;
		case Ctrl('K'):
						y_lockscreen();
			break;//add by yiyo ¿ìËÙÆÁÄ»Ëø¶¨
		case 'C':
		case 'c':
			if (newflag == 1)
				newflag = 0;
			else
				newflag = 1;
			show_brdlist(page, 1, newflag);
			break;
		case 'L':
			m_read();
			page = -1;
			break;
		case 'M':
			m_new();
			page = -1;
			break;
		case 'u':
			modify_user_mode(QUERY);
			t_query();
			page = -1 ;
			break;
		case 'H':
			{
				read_hot_info();//loveni
				Read();
				//show_help("0Announce/bbslist/day");
				page = -1;
				break;
			}
			/*		case 'R': {
			        		char buf[200],path[80],ans[4],*t;
				        	sprintf(buf, "[1;5;31mÁ¢¼´¶ÏÏß[m: [1;33mÒÔ±ã»Ö¸´ÉÏ´ÎÕý³£Àë¿ª±¾Õ¾Ê±µÄÎ´¶Á±ê¼Ç (Y/N) ? [[1;32mN[m[1;33m]£º[m");
					        getdata(22, 0, buf, ans, 3, DOECHO, YEA);
						if (ans[0] == 'Y' || ans[0] == 'y' ) {
			        			setuserfile(path, ".lastread");
						        t = strstr(path,".");
							*t = '\0';
						        sprintf(buf,"cp %s/.lastread.backup %s/.lastread",path,path);
						        system(buf);
						        sprintf(buf,"cp %s/.boardrc.backup %s/.boardrc",path,path);
						        system(buf);
							move(23,0);clrtoeol();
							move(22,0);clrtoeol();
							prints("[1;33mÒÑ¾­»Ö¸´ÉÏ´ÎÕý³£Àë¿ª±¾Õ¾Ê±µÄÎ´¶Á±ê¼Ç[m\n[1;32mÇë°´ Enter ¼ü[1;31mÁ¢¼´¶ÏÏß[m[1;32m, È»ºóÖØÐÂµÇÂ½±¾Õ¾ [m");
							egetch();
							exit(0);
						}
						page = -1;
						break;	
				        }
			*/
		case 'l':
			show_allmsgs();
			page = -1;
			break;
		case 'N':
		case ' ':
		case KEY_PGDN:
		case Ctrl('F'):
						if (num == brdnum - 1)
							num = 0;
				else
					num += BBS_PAGESIZE;
			break;
		case 'p':
		case 'k':
		case KEY_UP:
			if (num-- <= 0)
				num = brdnum - 1;
			break;
		case 'n':
		case 'j':
		case KEY_DOWN:
			if (++num >= brdnum)
				num = 0;
			break;
		case '$':
			num = brdnum - 1;
			break;
		case '!':	/* youzi leave */
			return Goodbye();
			break;
		case 'h':
			show_help("help/boardreadhelp");
			page = -1;
			break;
		case '/':
			move(3 + num - page, 0);
			prints(">", number);
			tmpnum = num;
			tmp = search_board(&num);
			move(3 + tmpnum - page, 0);
			prints(" ", number);
			if (tmp == 1)
				loop_mode = 1;
			else
			{
				loop_mode = 0;
				update_endline();
			}
			break;
		case 's':	/* sort/unsort -mfchen */
			currentuser.flags[0] ^= BRDSORT_FLAG;
			qsort(nbrd, brdnum, sizeof(nbrd[0]), cmpboard);
			page = 999;
			break;
		case 'y':
			if(GoodBrd.num)
				break;
			yank_flag = !yank_flag;
			brdnum = -1;
			break;
		case 'z':
			if(GoodBrd.num)
				break;
			if (HAS_PERM(PERM_BASIC) && !(nbrd[num].flag & NOZAP_FLAG))
			{
				ptr = &nbrd[num];
				ptr->zap = !ptr->zap;
				ptr->total = -1;
				zapbuf[ptr->pos] = (ptr->zap ? 0 : login_start_time);
				page = 999;
			}
			break;
		case 'a':
                        if (!HAS_PERM(PERM_LOGINOK)) 
			 	break;                
			if (GoodBrd.num)
			{
				if( GoodBrd.num >= GOOD_BRC_NUM)
				{
					move(2, 0);
					clrtoeol();
					prints("¸öÈËÈÈÃÅ°æÊýÒÑ¾­´ïÉÏÏÞ(%d)£¡", GOOD_BRC_NUM);
					pressreturn();
				}
				else
				{
					int pos;
					char  bname[STRLEN];
					struct boardheader fh;
					if(gettheboardname(1,"ÊäÈëÌÖÂÛÇøÃû (°´¿Õ°×¼ü×Ô¶¯ËÑÑ°): ", &pos, &fh, bname))
					{
						if(!inGoodBrds(bname))// && is_zone(BOARDS,bname)==0)
						{
							strcpy(GoodBrd.ID[GoodBrd.num ++],bname);
							save_GoodBrd();
							GoodBrd.num = 9999;
							brdnum = -1;
							break;
						}
					}
					//	 page = -1;
				}
				page = -1;
			}
			else
			{
				load_GoodBrd();
				ptr = &nbrd[num];
				if (GoodBrd.num >= GOOD_BRC_NUM)
				{
					move(2, 0);
					clrtoeol();
					prints("¸öÈËÈÈÃÅ°æÊýÒÑ¾­´ïÉÏÏÞ(%d)£¡", GOOD_BRC_NUM);
					GoodBrd.num = 0;
					pressreturn();
				}
				else
				{
					if (!inGoodBrds(ptr->name) && is_zone(BOARDS,ptr->name)==0)
					{
						strcpy(GoodBrd.ID[GoodBrd.num++],ptr->name);
						save_GoodBrd();
						GoodBrd.num = 0;
						move(2, 0);
						clrtoeol();
						prints("°åÃæ%sÒÑ±»¼ÓÈëÊÕ²Ø¼Ð",ptr->name);
						pressreturn();
						page =-1;
						break;
					}
					GoodBrd.num = 0;
				}
				page =-1;
			}
			break;
		case 'd':
			if ( GoodBrd.num )
			{
				int i, pos;
				char ans[5];
				sprintf(genbuf,"Òª°Ñ %s ´ÓÊÕ²Ø¼ÐÖÐÈ¥µô£¿(Y/N) [Y]:",nbrd[num].name);
				getdata(t_lines-1,0,genbuf,ans, 2,DOECHO, YEA);
				if(ans[0]=='n'||ans[0]=='N')
				{//modified by bluetent 2003.02.23
					page = -1;
					break;
				}
				pos = inGoodBrds(nbrd[num].name);
				for(i = pos-1; i< GoodBrd.num-1; i++)
					strcpy(GoodBrd.ID[i],GoodBrd.ID[i+1]);
				GoodBrd.num --;
				save_GoodBrd();
				GoodBrd.num = 9999;
				brdnum = -1;
			}
			break;
		case KEY_HOME:
			num = 0;
			break;
		case KEY_END:
			num = brdnum - 1;
			break;
		case '\n':
		case '\r':
			if (number > 0)
			{
				num = number - 1;
				break;
			}
			/* fall through */
		case KEY_RIGHT:
			{
				char    buf[STRLEN];
				ptr = &nbrd[num];
				if(ptr->flag & ZONE_FLAG)
                                {
                                        zoneoutnum=num;
                                        zoneoutpage=page;
                                        choose_board(DEFINE(DEF_NEWPOST) ? 1 : 0,ptr->name);
                                        zoneout=1;
                                        goto start;
                                }
				brc_initial(ptr->name);
				memcpy(currBM, ptr->BM, BM_LEN - 1);
				if (DEFINE(DEF_FIRSTNEW))
				{
					setbdir(buf, currboard);
					tmp = unread_position(buf, ptr);
					page = tmp - t_lines / 2;
					getkeep(buf, page > 1 ? page : 1, tmp + 1);
				}
				Read();
				if (zapbuf[ptr->pos] > 0 && brc_num > 0)
				{
					zapbuf[ptr->pos] = brc_list[0];
				}
				ptr->total = page = -1;
				break;
			}
		case 'S':	/* sendmsg ... youzi */
			if (!HAS_PERM(PERM_MESSAGE))
				break;
			s_msg();
			page = -1;
			break;
		case 'f':	/* show friends ... youzi */
			if (!HAS_PERM(PERM_BASIC))
				break;
			t_friends();
			page = -1;
			break;
		default:
			;
		}
		modify_user_mode(newflag ? READNEW : READBRD);
		if (ch >= '0' && ch <= '9')
		{
			number = number * 10 + (ch - '0');
			ch = '\0';
		}
		else
		{
			number = 0;
		}
	}
	clear();
	save_zapbuf();
	return -1;
}

char* brc_getrecord(char* ptr, char* name, int* pnum, int* list)
{
	int     num;
	char   *tmp;
	strncpy(name, ptr, BRC_STRLEN);
	ptr += BRC_STRLEN;
	num = (*ptr++) & 0xff;
	if (num > BRC_MAXNUM)
	{
		num = BRC_MAXNUM;
	}
	tmp = ptr + num * sizeof(int);
	*pnum = num;
	memcpy(list, ptr, num * sizeof(int));
	return tmp;
}

char* brc_putrecord(char* ptr, char* name, int num, int* list)
{
	if (num > 0 )
	{
		if (num > BRC_MAXNUM)
		{
			num = BRC_MAXNUM;
		}
		strcpy(ptr, name);
		ptr += BRC_STRLEN;
		*ptr++ = num;
		memcpy(ptr, list, num * sizeof(int));
		ptr += num * sizeof(int);
	}
	return ptr;
}

void brc_update()
{
	char    dirfile[STRLEN], *ptr;
	char    tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
	char    tmp_name[BRC_STRLEN];
	int     tmp_list[BRC_MAXNUM], tmp_num;
	int     fd, tmp_size;
	if (brc_changed == 0)
	{
		return;
	}
	ptr = brc_buf;
	if (brc_num > 0)
	{
		ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);
	}
	if (1)
	{
		setuserfile(dirfile, ".boardrc");
		if ((fd = open(dirfile, O_RDONLY)) != -1)
		{
			tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
			close(fd);
		}
		else
		{
			tmp_size = 0;
		}
	}
	tmp = tmp_buf;
	while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z'))
	{
		tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
		if (strncmp(tmp_name, currboard, BRC_STRLEN) != 0)
		{
			ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
		}
	}
	brc_size = (int) (ptr - brc_buf);

	if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
	{
		ftruncate(fd, 0);
		write(fd, brc_buf, brc_size);
		close(fd);
	}
	brc_changed = 0;
}

int brc_initial(char* boardname)  //deardragon0912
{
	char    dirfile[STRLEN], *ptr;
	int     fd;
	if (strcmp(currboard, boardname) == 0)
	{
		return brc_num;
	}
	brc_update();
	strcpy(currboard, boardname);
	brc_changed = 0;
	if (brc_buf[0] == '\0')
	{
		setuserfile(dirfile, ".boardrc");
		if ((fd = open(dirfile, O_RDONLY)) != -1)
		{
			brc_size = read(fd, brc_buf, sizeof(brc_buf));
			close(fd);
		}
		else
		{
			brc_size = 0;
		}
	}
	ptr = brc_buf;
	while (ptr < &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z'))
	{
		ptr = brc_getrecord(ptr, brc_name, &brc_num, brc_list);
		if (strncmp(brc_name, currboard,BRC_STRLEN) == 0)
		{
			return brc_num;
		}
	}
	strncpy(brc_name, boardname, BRC_STRLEN);
	brc_list[0] = 1;
	brc_num = 1;
	brc_changed=1;
	return 0;
}

void brc_addlist(char* filename)
{

	int     ftime, n, i;
	if (!strcmp(currentuser.userid, "guest"))
		return;
	ftime = atoi(&filename[2]);
	if ((filename[0]!='M'&&filename[0]!='G')||filename[1]!='.')
	{
		return;
	}
	if (brc_num <= 0)
	{
		brc_list[brc_num++] = ftime;
		brc_changed = 1;
		return;
	}

	if(brc_num>BRC_MAXNUM)
		brc_num=BRC_MAXNUM;
	for (n = 0; n < brc_num; n++)
	{
		if (ftime == brc_list[n])
		{
			return;
		}
		else if (ftime > brc_list[n])
		{
			if (brc_num < BRC_MAXNUM)
				brc_num++;
			for (i = brc_num - 1; i > n; i--)
			{
				brc_list[i] = brc_list[i - 1];
			}
			brc_list[n] = ftime;
			brc_changed = 1;
			return;
		}
	}

	if (brc_num < BRC_MAXNUM)
	{
		brc_list[brc_num++] = ftime;
		brc_changed = 1;
	}

}

int brc_unread(char* filename)
{
	int     ftime, n;
	ftime = atoi(&filename[2]);
	if ((filename[0] != 'M' && filename[0] != 'G') || filename[1] != '.')
	{
		return 0;
	}
	if (brc_num <= 0)
		return 1;
	for (n = 0; n < brc_num; n++)
	{
		if (ftime > brc_list[n])
		{
			return 1;
		}
		else if (ftime == brc_list[n])
		{
			return 0;
		}
	}
	return 0;
}

int clear_new_flag_quick(int time)
{
	strncpy(brc_name,currboard,BRC_STRLEN);
	for(brc_num=0;brc_num < BRC_MAXNUM;++brc_num)
		brc_list[brc_num] = time;
	brc_changed=1;
	brc_update();
}


int clear_all_new_flag()
{
	int i;
	char ans[3];
	struct newpostdata newpost_buffer[ MAXBOARD ];
	nbrd=newpost_buffer;
	getdata(t_lines-2,0,"È·¶¨ÒªÇå³ýËùÓÐ°åÃæµÄÎ´¶Á±ê¼Ç£¿(Y/N) [N]: ",ans, 2,DOECHO,YEA);
	if(ans[0]!='y'&&ans[0]!='Y')
		return;

	boardprefix = NULL;
	GoodBrd.num = 0;
	if(load_boards(NULL)<0)
		return;
	move(t_lines - 1, 0);
	clrtoeol();
	prints("  ÇëÉÔµÈ...");
	brc_update();
	int mytime=time(0);
	char *ptr=brc_buf;

	for(brc_num=0;brc_num < BRC_MAXNUM;++brc_num)
		brc_list[brc_num] = mytime;

	for(i=0;i< brdnum;i++)
		ptr = brc_putrecord(ptr, nbrd[i].name, brc_num, brc_list);
	char dirfile[STRLEN];
	setuserfile(dirfile, ".boardrc");
	int fd;
	if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1)
	{
		ftruncate(fd, 0);
		write(fd, brc_buf, (int)(ptr-brc_buf));
		close(fd);
	}
	return FULLUPDATE;
}
