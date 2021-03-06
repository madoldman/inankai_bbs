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
$Id: bbs.c,v 1.20 2010-07-05 04:58:44 madoldman Exp $
*/
#include "bbs.h"
#include <time.h>
#if defined(BSD44)
#include <stdlib.h>

#elif defined(LINUX)
/* include nothing :-) */
#else

#include <rpcsvc/rstat.h>
#endif

#define BBS_PAGESIZE    (t_lines - 4)

#ifndef DLM
#undef	ALLOWGAME
#endif

int     mot;
struct postheader header;
//static int quiting;
extern int numofsig;
int     continue_flag;
int     readpost;
int     digestmode;
int	local_article;
struct userec currentuser;
int     usernum=0;
char    currboard[STRLEN - BM_LEN];
char    currBM[BM_LEN - 1];
int     selboard = 0;
char	someoneID[31];
char    someoneIDs[31];
char    ReadPost[STRLEN] = "";
char    ReplyPost[STRLEN] = "";
int     FFLL = 0;
int		curr_ent=0;//同主题返回的文章号 add by bluetent 2003.4.19

int 	getlist(char *,char **,int, char **, int, char **, int, char **, int);
char   * filemargin();

char	*getname();
int     mkcompact();
void    report();
void    board_usage();
void    cancelpost();
void    add_crossinfo();
int     thesis_mode();
/*For read.c*/
int     compactread();
int		ban_detail();
int		show_detail();
int     auth_search_down();
int     auth_search_up();
int     t_search_down();
int     t_search_up();
int     post_search_down();
int     post_search_up();
int     thread_up();
int     thread_down();
int     deny_user();
int deny_from_article();
#ifdef BOARD_CONTROL
int     control_user();
#endif

int     sendmsgtoauthor();//add by yiyo
int     show_author();
int     SR_first_new();
int     SR_last();
int     SR_first();
int     SR_read();
int     SR_read();
int     SR_author();
int     SR_BMfunc();
int     Q_Goodbye();
int     t_friends();
int	BM_ontop ();
int     s_msg();
int     b_notes_passwd();
//int	read_letter();
int showpattern(char *);
void do_acction(int );

extern int numboards;
extern time_t login_start_time;
extern char BoardName[];
extern int cmpbnames();
extern int toggle1, toggle2;
extern char fromhost[];
//extern int numboards;
extern struct shortfile *bcache;
extern int badwords; //add for 'FILTER'
extern int temphotinfonumber;

int noreply = 0;
int pattern= 0;
#ifdef MARK_X_FLAG
int markXflag = 0;
#endif
int mailtoauther = 0;

char    genbuf[1024];
char    quote_title[120], quote_board[120];
char    quote_file[120], quote_user[120];

#ifndef NOREPLY
char    replytitle[STRLEN];
#endif

struct shortfile *getbcache();
int     totalusers, usercounter;

char *rtrim(char *s)
{
        static char t[1024], *t2;
        if(s[0]==0)
                return s;
        strncpy(t, s, 1024);
        t2=t+strlen(s)-1;
        while(strchr(" \t\r\n", t2[0]) && t2>t)
                t2--;
        t2[1]=0;
        return t;
}

char *ltrim(char *s) {
        char *s2=s;
        if(s[0]==0) return s;
        while(s2[0] && strchr(" \t\r\n", s2[0])) s2++;
        return s2;
}




int check_stuffmode()
{
        if (uinfo.mode == RMAIL || (uinfo.mode == READING && junkboard()))
                return YEA;
        else
                return NA;
}

#ifndef BBSD
void get_load(double load[])
{
#if defined(LINUX)
        FILE   *fp;
        fp = fopen("/proc/loadavg", "r");
        if (!fp)
                load[0] = load[1] = load[2] = 0;
        else {
                float   av[3];
                fscanf(fp, "%g %g %g", av, av + 1, av + 2);
                fclose(fp);
                load[0] = av[0];
                load[1] = av[1];
                load[2] = av[2];
        }
#elif defined(BSD44)
        getloadavg(load, 3);
#else

        struct statstime rs;
        rstat("localhost", &rs);
        load[0] = rs.avenrun[0] / (double) (1 << 8);
        load[1] = rs.avenrun[1] / (double) (1 << 8);
        load[2] = rs.avenrun[2] / (double) (1 << 8);
#endif
}
#endif


int mytruncate(char * path , size_t size, int num)
{

  char tmppath[STRLEN];
  sprintf(tmppath,"%s.iptmp",path);
  FILE *fp=fopen(tmppath,"w+");
  FILE *src=fopen(path,"r");
  if(src==NULL || fp==NULL) return -1;

  fseek(src,-size*num,SEEK_END);
  void *buf=(void *)malloc(size);
  while(1){
      bzero(buf,size);
      if(fread(buf,size,1,src)<=0) break;
      fwrite(buf,size,1,fp);
  }
  fclose(src);
  fclose(fp);
  rename(tmppath,path);
  return 0;
}


void report_user_enter()
{
        FILE *fp;
        time_t now;
        char ipfile[256];
        struct user_enter rduser;
        now=time(0);
        bzero(&rduser,sizeof(rduser));
        snprintf(rduser.loginfrom,18,"%s",uinfo.from);
        snprintf(rduser.logintime,28,"%s",ctime(&now));
        
        sprintf(ipfile,"home/%c/%s/ipfile",toupper(currentuser.userid[0]),currentuser.userid);
        fp=fopen(ipfile,"a+");
        if(fp==NULL)  return;
        fwrite(&rduser,sizeof(rduser),1,fp);
        fclose(fp) ;
        mytruncate(ipfile,sizeof(rduser),120); 
}


int show_user_record()
{
        FILE *fp;
        int  count;
        int  total;
        char ipfile[256];
        struct user_enter duser;
        struct stat st;
        modify_user_mode(QUERYIP);
        snprintf(ipfile,255,"home/%c/%s/ipfile",toupper(currentuser.userid[0]),currentuser.userid);
        if(NULL == (fp=fopen(ipfile,"r"))) {
                return -1;
        }
        stat(ipfile,&st);
        total=st.st_size/sizeof(duser);
        if(total>100)
                total=100;
        fseek(fp,-1 * sizeof(duser),SEEK_END);

        clear();
        prints("\n                              [36;1m您的上站记录[m\n");
        prints("\n                      [35;1m(超过一百次则显示最近一百次)[m\n");
        prints("            [1;35m-------------------------------------------------[m\n");
        for(count=0;count<total;count++) {
                fread(&duser,sizeof(duser),1,fp);
                prints("               [32;1m%-15s[m--   [1;34m%s[m",duser.loginfrom,duser.logintime);
                fseek(fp,-2 * sizeof(duser),SEEK_CUR);
                if(!((count+1)%15)) {
                        pressanykey();
                        clear();
                }
        }
        pressanykey();
        fclose(fp);
        return 1;
}

int search_all_id()
{
        FILE *fp;
        struct fileheader xfh;
        struct boardheader abo;
        char uident[STRLEN],date[20],buf[STRLEN],choice[2];
        static char flist[10000][80];
        char buf1[STRLEN],buf2[STRLEN];
        int i,j,n,cnum,cpage,num,ch,just=1,z;
        int snum=0;
        //  char showtime[20];
        time_t now;
        time_t filetime;

        struct boardheader seekidbodr;
        int seekidbopos;
        int suicide=0;  //add by tdhlshx for searching suicided ID's articles
        struct userec user_uident;
        time_t userfirstl;
        struct art {
                char owner[15],name[30],title[STRLEN],board[30],btitle[STRLEN];
        }
        art[10000],artbuf;
        /*	if(Heavy_load())
        	{
        		clear();
        		move(10, 3);
        		prints("目前本站系统负载过高，无法使用此功能，请进入WWW方式搜索。");
        		pressanykey();
        		return 0;
        	}*/


        clear();
        move(1,0);
        prints("                        %s用户文章全站搜索功能[m              ",BBSNAME);
        modify_user_mode( SAID );
        //Flame modify it 2001.4.29
        if(askyn("您要查询包含帐号已经不存在作者的文章么",NA,YEA) == YEA ) {
                move(1,0);
                clrtobot();
                getdata(1,0,"请输入要搜索的用户帐号(区分大小写)：",uident,40,DOECHO,YEA);
                if(uident[0] == '\0')
                        return;
		if(check_user_exist(uident))
			return;
        } else {
                move(1,0);
                clrtobot();
                prints("请输入要搜索的用户帐号(自动搜索)：");
                move(1,34);
                usercomplete(NULL,uident);
                if(uident[0] == '\0')
                        return;
		if(check_user_exist(uident))
			return;
                if( !getuser(uident) ) {
                        move(4,0) ;
                        clrtoeol();
                        prints("\n◆ 不正确的使用者代号 ◆\n") ;
                        pressanykey() ;
                        return;
                }

        }
        //end
        //add by tdhlshx

        if(getuser(uident)) {
                memcpy(&user_uident,&lookupuser,sizeof(uinfo));
                userfirstl=user_uident.firstlogin;
        }

        clear();
        move(1,0);
        prints("                         %s用户文章全站搜索功能              ",BBSNAME);
        move(3,0);
        prints("正在搜索网友 %s 的所有文章，请稍候...",uident);
        refresh();
        for( n = 0; n < numboards; n++ ) {
                if( get_record(".BOARDS", &abo, sizeof(abo), n) )
                        continue;
                seekidbopos = search_record(BOARDS, &seekidbodr, sizeof(seekidbodr), cmpbnames, abo.filename);
                /*	   if( !strcmp("deleted",abo.filename) || !strcmp("junk",abo.filename)
                           || !strcmp("newcomers",abo.filename) 
                           || !strcmp("notepad",abo.filename) 
                           || !strncmp("syssecurity",abo.filename, 11) 
                           || !strcmp("bbslists",abo.filename) 
                           || !strcmp("Announce",abo.filename) 
                	       || !strcmp("Headquarters",abo.filename)
                           || !strcmp("vote",abo.filename)
                           || (seekidbopos&&((seekidbodr.level & ~PERM_POSTMASK) != 0))
                           ) */
                if(seekidbopos&&((seekidbodr.level & ~PERM_POSTMASK) != 0))
                        continue;

	
 if(!HAS_PERM(PERM_SYSOP) || deny_s_board(abo.filename)) {
         setbfile(genbuf, abo.filename, "board.allow");
         if(dashf(genbuf)&&!seek_in_file(genbuf,currentuser.userid))
              continue;
 }

//	if(!strcmp(abo.filename,"PracticeTeam")) continue;

                move(5,0);
                clrtoeol();
                prints("搜索进度：%2d%%",(n*100)/(numboards-1));
                move(6,0);
                clrtoeol();
                /*	for( bulb=0;bulb<n;bulb++){
                			prints("#");
                	}*/
                move(8,0);
                clrtoeol();
                prints("正在搜索 %s 讨论区...",abo.filename);
                refresh();
                setbdir(buf,abo.filename);
                i = 1;
                fp=fopen(buf, "r");
                if(fp==0)
                        continue;
                //        while(!get_record(buf, &xfh, sizeof(xfh), i)) {
                while(1) {
                        if(fread(&xfh, sizeof(xfh), 1, fp)<=0)
                                break;
                        if( !strcasecmp(xfh.owner,uident) ) {
                                snum++;
                                filetime = atoi( xfh.filename + 2 );
                                //to judge whether the article belongs to the author s last life.
                                if(filetime <userfirstl)
                                        suicide=1;
                                if( filetime > 740000000 ) {
                                        ptime(date,&filetime,1);
                                } else {
                                        date[0] = '\0';
                                }
                                if (!strncmp("Re:",xfh.title,3)
                                                || !strncmp("RE:",xfh.title, 3)) {
                                        if(userfirstl>filetime)
                                                sprintf(flist[snum-1],"%-16s %14s * %-50s",
                                                        abo.filename, date, xfh.title);
                                        else
                                                sprintf(flist[snum-1],"%-16s %14s %-50s",
                                                        abo.filename, date, xfh.title);
                                        flist[snum-1][80] = '\0';
                                } else {
                                        if(userfirstl>filetime)
                                                sprintf(flist[snum-1],"%-16s %14s *● %-47s",
                                                        abo.filename, date, xfh.title);
                                        else

                                                sprintf(flist[snum-1],"%-16s %14s ● %-47s",
                                                        abo.filename, date, xfh.title);
                                        flist[snum-1][80] = '\0';
                                }
                                z=0;
                                buf1[0]='\0';
                                while(abo.title[z]) {
                                        buf1[z] = abo.title[z+11];
                                        z++;
                                }
                                buf1[z]='\0';
                                z=0;
                                strcpy(art[snum-1].name,xfh.filename);
                                strcpy(art[snum-1].title,xfh.title);
                                strcpy(art[snum-1].board,abo.filename);
                                strcpy(art[snum-1].btitle,buf1);
                                strcpy(art[snum-1].owner,xfh.owner);
                        }
                        i++;
                }
                fclose(fp);

        }
        clear();
        move(1,0);
        prints("                         %s用户文章全站搜索功能             ",BBSNAME);
        if( snum == 0 ) {
                move(3,0);
                prints("                       ◆ 没有找到网友“%s”的文章 ◆",uident);
                pressanykey();
                return;
        }
        move(3,0);
        /*add by tdhlshx 2003.4.29 自杀后又被注册的ID的文章显示问题*/
        if(suicide==1)
                prints("        该ID死亡过,他/她前生的文章将以*标记...\n\n");
        //add end

        prints("               ◆ 全程搜索完毕，共找到 %s 的文章 %d 篇 ◆\n",uident,snum);
        prints("\n请选择文章的排列方式\n\n");
        prints("(0) 离 开\n");
        prints("(1) 按讨论区顺序\n");
        prints("(2) 按时间顺序(较慢)");
        getdata(12,0,"选择 [1]：", choice, 2, DOECHO, NULL, YEA);
lab:
        if( z == 1 )
                choice[0]='2';
        if( choice[0] == '2' ) {
                for(i=0;i<snum-2;i++) {

                        for(j=0;j<snum-1-i;j++) {
                                gtime(buf1,flist[j]);
                                gtime(buf2,flist[j+1]);
                                buf1[14]='\0';
                                buf2[14]='\0';
                                if( timecmp(buf1,buf2) > 0 ) {
                                        buf[0] = '\0';
                                        strncpy(buf,flist[j],80);
                                        flist[j][0]='\0';
                                        strncpy(flist[j],flist[j+1],80);
                                        flist[j+1][0]='\0';
                                        strncpy(flist[j+1],buf,80);
                                        artbuf=art[j];
                                        art[j]=art[j+1];
                                        art[j+1]=artbuf;
                                }
                        }
                        if(z==0) {
                                move(13,0);
                                prints("完成进度：%2d%%",i*100/snum);
                                refresh();
                        } else {
                                move(23,0);
                                clrtoeol();
                                prints("正在按时间进行排序，请稍后...                                     [ 进度：%2d%% ]",i*100/snum);
                                refresh();
                        }
                }
        } else if( choice[0] == '0' )
                return;

        i=0;
        j=0;
        z=0;
        buf[0]='\0';
        num = snum;
        just = 1;
        clear();
        while( 1 ) {
                now = time(NULL);
                //    ptime(showtime,&now,0);
                move(0,0);
                prints("[44;1m[33m[文章搜索]                     [36;1m%s全站文章列表                                     [33;1m[m\n",uident);
                move(1,0);
                prints("[m 看文章[[32;1mEnter,→[m]  回文章[[32;1mr,R[m]  离开[[32;1mq,←[m]                                                         ");
                move(2,0);
                prints("[44;37;1m 编 号 讨 论 区          发 表 日 期     文 章 标 题                             [m");
                move(23,0);
                clrtoeol();
                prints("[44;37;1m标题：%-44s 讨论区：%-20s[m",art[num-1].title,art[num-1].btitle);
                if( num < cnum || num > cnum+BBS_PAGESIZE-1 || just == 1) {
                        cpage = num / BBS_PAGESIZE;
                        if( cpage*BBS_PAGESIZE != num  )
                                cpage++;
                        cnum = (cpage-1) * BBS_PAGESIZE + 1;
                        for(i=cnum;i<BBS_PAGESIZE+cnum;i++) {
                                move(3+i-cnum,0);
                                if( i > snum ) {
                                        clrtoeol();
                                        move(3+num-cnum,1);
                                        continue;
                                }
                                if( i == num ) {
                                        clrtoeol();
                                        prints(">%5d %-80s",i,flist[i-1]);
                                } else if( i <= snum ) {
                                        clrtoeol();
                                        prints(" %5d %-80s",i,flist[i-1]);
                                }
                                move(3+num-cnum,1);
                                just = 0;
                        }
                } else {
                        move(3+num-cnum,0);
                        prints(">");
                }
                ch = egetch();
                move(3+num-cnum,0);
                prints(" ");
                switch( ch ) {
                case KEY_UP:
                        if( --num <= 0 ) {
                                num = snum;
                                just=1;
                        }
                        break;
                case KEY_DOWN:
                        if( ++num > snum ) {
                                num = 1;
                                just=1;
                        }
                        break;
                case KEY_RIGHT:
                case '\r':
                case '\n':
                        sprintf(buf,"boards/%s/%s",art[num-1].board,art[num-1].name);
                        ansimore(buf,YEA);
                        just = 1;
                        break;
                case 'r':
                case 'R':
                        strcpy(currboard, art[num-1].board);
                        strcpy(replytitle, art[num-1].title);
                        setbfile( quote_file, currboard, art[num-1].name);
                        strcpy( quote_user, art[num-1].owner );
                        post_article();
                        replytitle[0] = '\0';
                        just = 1;
                        break;
                case KEY_LEFT:
                        //	move(2,0);
                        //	clrtobot();
                        move(t_lines-1,0);
                        clrtoeol();
                        if(askyn("确定离开吗?",YEA,YEA)==YEA) {
                                return;
                        }
                case Ctrl('B'):
                                        case KEY_PGUP:
                                                        num-=BBS_PAGESIZE;
                        while(num<=0)
                                num+=snum;
                        break;
                case ' ':
                case Ctrl('F'):
                                        case KEY_PGDN:
                                                        num += BBS_PAGESIZE;
                        if( num > snum )
                                num = snum;
                        break;
                case Ctrl('S'):
                                                z = 1;
                        goto lab;
                        break;
                }
        }
}

int search_all_tit()
{
        FILE *fp;
        struct fileheader xfh;
        struct boardheader abo;
        char uident[50],date[20],buf[STRLEN],choice[2];
        static char flist[10000][80];
        char buf1[STRLEN],buf2[STRLEN];
        int i,j,n,cnum,cpage,num,ch,just=1,z;
        int snum=0,listtype=0;
        time_t filetime;
        time_t now;
        //  char showtime[20];
        //  int bulb;

        struct boardheader seekbodr;
        int seekbopos;


        struct art {
                char owner[15],name[30],title[STRLEN],board[30],btitle[STRLEN],etitle[20];
        }
        art[10000],artbuf;
        /*	if(Heavy_load())
        	{
        		clear();
        		move(10, 3);
        		prints("目前本站系统负载过高，无法使用此功能，请进入WWW方式搜索。");
        		pressanykey();
        		return 0;
        	}*/
        clear();
        move(1,0);
        prints("                           %s文章标题搜索功能",BBSNAME);
        modify_user_mode( SAAR );
        error:
        getdata(3,0,"请输入要搜索的文章标题：",uident,40,DOECHO,YEA);
        if( uident[0] == '\0' )
                return;
        if( strlen(uident) < 4 ) {
                prints("\n\n◆ 对不起，文章标题必须大于等于四个字符 ◆");
                pressanykey();
                move(6,0);
                clrtoeol();
                goto error;
        }
        clear();
        move(1,0);
        prints("                           %s文章标题搜索功能 ",BBSNAME);
        move(3,0);
        prints("正在搜索标题含有 %s 的所有文章，请稍候...",uident) ;
        refresh();
        for( n = 0; n < numboards; n++ ) {
                if( get_record(".BOARDS", &abo, sizeof(abo), n) )
                        continue;
                seekbopos = search_record(BOARDS, &seekbodr, sizeof(seekbodr), cmpbnames, abo.filename);
                /*    if( !strcmp("deleted",abo.filename) || !strcmp("junk",abo.filename)
                           || !strcmp("newcomers",abo.filename) 
                           || !strcmp("notepad",abo.filename) 
                           || !strncmp("syssecurity",abo.filename, 11) 
                           || !strcmp("bbslists",abo.filename) 
                           || !strcmp("Announce",abo.filename) 
                           || !strcmp("BoardMaster",abo.filename)
                           || !strcmp("vote",abo.filename)
                	       || !strcmp("Headquarters",abo.filename)
                		   || (seekbopos&&((seekbodr.level & ~PERM_POSTMASK) != 0)))	*/
                if(seekbopos&&((seekbodr.level & ~PERM_POSTMASK) != 0))
                        continue;

//                if(!strcmp(abo.filename,"PracticeTeam")) continue;
if(!HAS_PERM(PERM_SYSOP) || deny_s_board(abo.filename)) {
        setbfile(genbuf, abo.filename, "board.allow");
        if(dashf(genbuf)&&!seek_in_file(genbuf,currentuser.userid))
             continue;
}


                move(5,0);
                clrtoeol();
                prints("搜索进度：%2d%%",(n*100)/(numboards-1));
                move(6,0);
                clrtoeol();
                /*	for( bulb=0;bulb<n;bulb++){
                		prints("#");
                	}*/
                move(8,0);
                clrtoeol();
                prints("正在搜索 %s 讨论区...",abo.filename);
                refresh();
                setbdir(buf,abo.filename);
                i = 1;
                fp=fopen(buf, "r");
                if(fp==0)
                        continue;
                //        while(!get_record(buf, &xfh, sizeof(xfh), i)) {
                while(1) {
                        if(fread(&xfh, sizeof(xfh), 1, fp)<=0)
                                break;
                        if( strstr(xfh.title,uident) ) {
                                snum++;
                                filetime = atoi( xfh.filename + 2 );
                                if( filetime > 740000000 ) {
                                        ptime(date,&filetime,1);
                                } else {
                                        date[0] = '\0';
                                }
                                if (!strncmp("Re:",xfh.title,3) || !strncmp("RE:",xfh.title,
                                                3)) {
                                        sprintf(flist[snum-1],"%14s %-47s\n",date, xfh.title);
                                        flist[snum-1][80] = '\0';
                                } else {
                                        sprintf(flist[snum-1],"%14s ● %-44s\n",date, xfh.title);
                                        flist[snum-1][80] = '\0';
                                }
                                z=0;
                                buf1[0]='\0';
                                while(abo.title[z]) {
                                        buf1[z] = abo.title[z+11];
                                        z++;
                                }
                                buf1[z]='\0';
                                z=0;
                                strcpy(art[snum-1].name,xfh.filename);
                                strcpy(art[snum-1].title,xfh.title);
                                strcpy(art[snum-1].board,abo.filename);
                                strcpy(art[snum-1].btitle,buf1);
                                strcpy(art[snum-1].owner,xfh.owner);
                                strcpy(art[snum-1].etitle,abo.filename);
                        }
                        i++;
                }
                fclose(fp);
        }
        clear();
        move(1,0);
        prints("                           %s文章标题搜索功能 ",BBSNAME);
        if( snum == 0 ) {
                prints("\n\n\n◆ 没有找到标题含“%s”的文章 ◆",uident);
                pressanykey();
                return;
        }
        move(3,0);
        prints("                ◆ 全程搜索完毕，共找到标题为 %s 的文章 %d 篇 ◆\n",uident,snum);
        prints("\n请选择文章的排列方式\n\n");
        prints("(0) 离 开\n");
        prints("(1) 按讨论区顺序\n");
        prints("(2) 按时间顺序(较慢)");
        getdata(13,0,"选择 [1]：", choice, 2, DOECHO, NULL, YEA);
lab:
        if( z == 1 )
                choice[0]='2';
        if( choice[0] == '2' ) {
                for(i=0;i<snum-2;i++) {
                        for(j=0;j<snum-i-1;j++) {
                                gtime(buf1,flist[j]);
                                gtime(buf2,flist[j+1]);
                                buf1[14]='\0';
                                buf2[14]='\0';
                                if( timecmp(buf1,buf2) > 0 ) {
                                        buf[0] = '\0';
                                        strncpy(buf,flist[j],80);
                                        flist[j][0]='\0';
                                        strncpy(flist[j],flist[j+1],80);
                                        flist[j+1][0]='\0';
                                        strncpy(flist[j+1],buf,80);
                                        artbuf=art[j];
                                        art[j]=art[j+1];
                                        art[j+1]=artbuf;
                                }
                        }
                        if(z==0) {
                                move(13,0);
                                prints("完成进度：%2d%%",i*100/snum);
                                refresh();
                        } else {
                                move(23,0);
                                clrtoeol();
                                prints("正在按时间进行排序，请稍候...                                     [ 进度：%2d%% ]",i*100/snum);
                                refresh();
                        }
                }
        } else if( choice[0] == '0' )
                return;
        i=0;
        j=0;
        z=0;
        buf[0]='\0';
        num = snum;
        just = 1;
        //  uident[0] = '\0';
        listtype=2;
        /* listtype=1 讨论区列表 listtype=2 用户列表 */
        clear();
        while( 1 ) {
                now = time(NULL);
                //     ptime(showtime,&now,0);
                move(0,0);
                prints("[44;1m[33m[标题搜索]                        [36;1m标题搜索结果                                 [33;1m [m\n");

                move(1,0);
                prints("[m 看文章[[32;1mEnter,→[m]  回文章[[32;1mr,R[m]  离开[[32;1mq,←[m]                                      ");
                move(2,0);
                clrtoeol();
                if( listtype == 1 )
                        prints("[44;37;1m 编 号 讨 论 区          发 表 日 期     文 章 标 题                             [m");
                else
                        prints("[44;37;1m 编 号 刊 登 者          发 表 日 期     文 章 标 题                             [m");
                move(23,0);
                clrtoeol();
                prints("[44;37;1m标题：%-44s 讨论区：%-20s[m",art[num-1].title,art[num-1].btitle);

                if( num < cnum || num > cnum+BBS_PAGESIZE-1 || just == 1) {
                        cpage = num / BBS_PAGESIZE;
                        if( cpage*BBS_PAGESIZE != num  )
                                cpage++;
                        cnum = (cpage-1) * BBS_PAGESIZE + 1;
                        for(i=cnum;i<BBS_PAGESIZE+cnum;i++) {
                                move(3+i-cnum,0);
                                if( i > snum ) {
                                        clrtoeol();
                                        move(3+num-cnum,1);
                                        continue;
                                }
                                if( i == num ) {
                                        clrtoeol();
                                        if( listtype == 1 )
                                                prints(">%5d %-16s %-55s",i,art[i-1].etitle,flist[i-1]);
                                        else
                                                prints(">%5d %-16s %-55s",i,art[i-1].owner,flist[i-1]);
                                } else if( i <= snum ) {
                                        clrtoeol();
                                        if( listtype == 1 )
                                                prints(" %5d %-16s %-55s",i,art[i-1].etitle,flist[i-1]);
                                        else
                                                prints(" %5d %-16s %-55s",i,art[i-1].owner,flist[i-1]);
                                }
                                move(3+num-cnum,1);
                                just = 0;
                        }
                } else {
                        move(3+num-cnum,0);
                        prints(">");
                }
                ch = egetch();
                move(3+num-cnum,0);
                prints(" ");
                switch( ch ) {
                case KEY_UP:
                        if( --num <= 0 ) {
                                num = snum;
                                just=1;
                        }
                        break;
                case KEY_DOWN:
                        if( ++num > snum ) {
                                num = 1;
                                just=1;
                        }
                        break;
                case KEY_RIGHT:
                case '\r':
			if(check_user_exist(art[num-1].owner))
				break;
                        sprintf(buf,"boards/%s/%s",art[num-1].board,art[num-1].name);
                        ansimore(buf,YEA);
                        just = 1;
                        break;

                case 'r':
                case 'R':
                        strcpy(currboard, art[num-1].board);
                        strcpy(replytitle, art[num-1].title);
                        setbfile( quote_file, currboard, art[num-1].name);
                        strcpy( quote_user, art[num-1].owner );
                        post_article();
                        replytitle[0] = '\0';
                        just = 1;
                        break;

                case KEY_LEFT:
                        move(t_lines-1,0);
                        clrtoeol();
                        if((askyn("确定离开吗?",YEA,YEA)==YEA)) {
                                return;
                        }

                case KEY_PGUP:
                case Ctrl('B'):
                                                num-= BBS_PAGESIZE;
                        if(num <0)
                                num=snum+num;
                        break;

                case KEY_PGDN:
                case ' ':
                case Ctrl('F'):
                                                num += BBS_PAGESIZE;
                        if( num > snum )
                                num = snum;
                        break;
                case Ctrl('S'):
                                                z = 1;
                        goto lab;
                        break;
                case Ctrl('T'):
                                                listtype = (listtype==1)? 2:1;
                        just = 1;
                        break;
                }
        }
}

int ptime(char *cti,time_t *clock,int mo)
{
        struct tm *nt;
        char weeknum[7][3]={"天","一","二","三","四","五","六"};
        nt = localtime(clock);
        switch(mo) {
        case 0:
                        sprintf(cti,"%02d:%02d",nt->tm_hour,nt->tm_min);
                break;
        case 1:
                sprintf(cti," %4d年%02d月%02d日 ",nt->tm_year+1900,
                        nt->tm_mon+1,nt->tm_mday);
                break;
        case 2:
                sprintf(cti,"星期%2s  %02d:%02d ",weeknum[nt->tm_wday],
                        nt->tm_hour,nt->tm_min);
                break;
        case 3:
                sprintf(cti,"%4d年%02d月%02d日 星期%2s",nt->tm_year+1900,
                        nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday]);
                break;
        case 4:
                sprintf(cti,"%4d年%02d月%02d日 星期%2s %02d:%02d",nt->tm_year+1900,
                        nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday],nt->tm_hour,
                        nt->tm_min);
                break;
        case 5:
                sprintf(cti,"%4d年%02d月%02d日 星期%2s %02d:%02d:%02d",nt->tm_year+1900,
                        nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday],nt->tm_hour,
                        nt->tm_min,nt->tm_sec);
                break;
        case 6:
                sprintf(cti,"%4d年%02d月%02d日 %02d:%02d",nt->tm_year+1900,
                        nt->tm_mon+1,nt->tm_mday,nt->tm_hour,nt->tm_min);
                break;
        case 7:
                sprintf(cti,"%02d月%02d日 星期%2s %02d:%02d:%02d",
                        nt->tm_mon+1,nt->tm_mday,weeknum[nt->tm_wday],nt->tm_hour,
                        nt->tm_min,nt->tm_sec);
                break;
        default:
                sprintf(cti,"函数使用错误");
        }
        //return cti;
}

int gtime(char *ret,char* flist)
{
        int i;
        for(i=18;i<32;i++)
                ret[i-18] = flist[i];
        return;
}

int timecmp(char *one, char* two)
{
        char a[5],b[5];
        int i;
        for(i=0;i<4;i++) {
                a[i] = one[i];
                b[i] = two[i];
        }
        a[4]='\0';
        b[4]='\0'; //so ft tdhlshx 2003.5.21.4:00
        if( atoi(a) > atoi(b) )
                return 1;
        else if( atoi(a) < atoi(b) )
                return -1;
        for(i=0;i<2;i++) {
                a[i] = one[i+6];
                b[i] = two[i+6];
        }
        a[2]='\0';
        b[2]='\0';
        if( atoi(a) > atoi(b) )
                return 1;
        else if( atoi(a) < atoi(b) )
                return -1;
        for(i=0;i<2;i++) {
                a[i] = one[i+10];
                b[i] = two[i+10];
        }
        a[2]='\0';
        b[2]='\0';
        if( atoi(a) > atoi(b) )
                return 1;
        else if( atoi(a) < atoi(b) )
                return -1;
        else
                return 0;
}

int set_safe_record()
{
        struct userec tmp;
        extern int ERROR_READ_SYSTEM_FILE;
        if (get_record(PASSFILE, &tmp, sizeof(currentuser), usernum) == -1) {
                char    buf[STRLEN];
                sprintf(buf, "Error:Read Passfile %4d %12.12s", usernum, currentuser.userid);
                report(buf);
                ERROR_READ_SYSTEM_FILE = YEA;
                abort_bbs();
                return -1;
        }
        currentuser.numposts = tmp.numposts;
        currentuser.userlevel = tmp.userlevel;
        currentuser.numlogins = tmp.numlogins;
#ifdef ALLOW_MEDALS

        currentuser.nummedals = tmp.nummedals;
#endif

#ifdef ALLOWGAME

        currentuser.nummedals = tmp.nummedals;
        currentuser.money = tmp.money;
        currentuser.bet = tmp.bet;
#endif

        currentuser.stay = tmp.stay;
        return;
}

char* sethomepath(char* buf, char* userid)
{
        sprintf(buf, "home/%c/%s", toupper(userid[0]), userid);
        return buf;
}
/*Add by SmallPig*/
void setqtitle(char* stitle)
{
        FFLL = 1;
        if (strncmp(stitle, "Re: ", 4) != 0 && strncmp(stitle, "RE: ", 4) != 0) {
                sprintf(ReplyPost, "Re: %s", stitle);
                strcpy(ReadPost, stitle);
        } else {
                strcpy(ReplyPost, stitle);
                strcpy(ReadPost, ReplyPost + 4);
        }
}

int chk_currBM(char BMstr[STRLEN-1])
{
        char   *ptr;
        char    BMstrbuf[STRLEN - 1];
        if (HAS_PERM(PERM_BLEVELS))
        {
		return YEA;
	}
        //   if (HAS_PERM(PERM_BLEVELS|PERM_LOOKADMIN)) return YEA;
        /* modified by yiyo 允许校方管理帐号删文章 */
        if ((!strcmp(currboard,"ASCIIArt")||!strcmp(currboard,"cnASCIIArt"))&&HAS_PERM(PERM_ACBOARD)&&HAS_PERM(PERM_WELCOME))
	{
		return YEA;//add by yiyo
	}
    if(!strncmp(currboard,"BMTest",6)) return YEA;
	if(!strcmp(currboard,"BMZone")) return YEA;
	if(!strcmp(currboard,"Arbitrators")) return YEA;

	 
        if (HAS_PERM(PERM_ACHATROOM) && !strncmp (BMstr, "SYSOPs", 6) )
                return YEA;

        if (!HAS_PERM(PERM_BOARDS|PERM_PERSONAL))
                return NA;
	
	if(chk_currZM())
	{
		return YEA;
	}
        strcpy(BMstrbuf, BMstr);
        ptr = strtok(BMstrbuf, ",: ;|&()\0\n");
        while(1) 
	{
                if (ptr == NULL)
                {
			return NA;
		}
                if (!strcasecmp(ptr,currentuser.userid))
                {
			return YEA;
		}
                ptr = strtok(NULL, ",: ;|&()\0\n");
        }
}

int chk_currZM()
{
	struct shortfile *b_children;
	struct shortfile *b_father;
	char temp_BM[BM_LEN - 1];
	char *ptr;
        b_children = getbcache(currboard);
	if(b_children->flag & CHILDBOARD_FLAG)
	{
		b_father=getbcache(b_children->owner);
		memcpy(temp_BM,b_father->BM,BM_LEN - 1);
        	ptr = strtok(temp_BM, ",: ;|&()\0\n");
        	while(1) 
		{
        	        if (ptr == NULL)
        	        {
				return NA;
			}
        	        if (!strcasecmp(ptr,currentuser.userid))
        	        {
				return YEA;
			}
                	ptr = strtok(NULL, ",: ;|&()\0\n");
        	}
	}	
	return NA;
}

/* 精华区管理的chk_currBM，不检查PERM_BOARDS和PERM_PERSONAL */
/* 实现精华区“居民”功能 */
int chk_currBM2(char BMstr[STRLEN - 1])
{
        char   *ptr;
        char    BMstrbuf[STRLEN - 1];

        if (HAS_PERM(PERM_BLEVELS))
                return YEA;
        //   if (HAS_PERM(PERM_BLEVELS|PERM_LOOKADMIN)) return YEA;
        /* modified by yiyo 允许校方管理帐号删文章 */
        if ((!strcmp(currboard,"ASCIIArt")||!strcmp(currboard,"cnASCIIArt"))&&HAS_PERM(PERM_ACBOARD)&&HAS_PERM(PERM_WELCOME))
                return YEA;//add by yiyo
        if (HAS_PERM(PERM_ACHATROOM) && !strncmp (BMstr, "SYSOPs", 6) )
                return YEA;

        //     if (!HAS_PERM(PERM_BOARDS|PERM_PERSONAL)) return NA;
	if(chk_currZM())
	{
		return YEA;
	}
        strcpy(BMstrbuf, BMstr);
        ptr = strtok(BMstrbuf, ",: ;|&()\0\n");
        while (1) {
                if (ptr == NULL)
                        return NA;
                if (!strcmp(ptr, currentuser.userid))
                        return YEA;
                if (!strcmp(ptr, "BMcoach"))
                        return YEA;

               ptr = strtok(NULL, ",: ;|&()\0\n");
        }
}

void setquotefile(char filepath[])
{
        strcpy(quote_file, filepath);
}

char* sethomefile(char* buf, char* userid, char* filename)
{
        sprintf(buf, "home/%c/%s/%s", toupper(userid[0]), userid, filename);
        return buf;
}

char* setuserfile(char* buf, char* filename)
{
        sprintf(buf, "home/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, filename);
        return buf;
}

char* setbpath(char* buf, char*  boardname)
{
        strcpy(buf, "boards/");
        strcat(buf, boardname);
        return buf;
}

int setbdir(char* buf, char* boardname)
{
        char    dir[STRLEN];
        switch (digestmode) {
        case NA:
                strncpy(dir, DOT_DIR, STRLEN);
                break;
        case YEA:
                strncpy(dir, DIGEST_DIR, STRLEN);
                break;
        case 2:
                strncpy(dir, THREAD_DIR, STRLEN);
                break;
        case 3:
                strncpy(dir, MARKED_DIR, STRLEN);
                break;
        case 4:
                strncpy(dir, AUTHOR_DIR, STRLEN);
                break;
                /*case 5:  同作者
                case 6:  同作者 
                case 7:  标题关键字
                       default:*/
        }
        dir[STRLEN - 1] = '\0';
        if( digestmode == 5 || digestmode == 6)
                sprintf(buf, "boards/%s/SOMEONE.%s.DIR.%d",boardname,someoneIDs,digestmode-5);
        else if ( digestmode == 7 )
                sprintf(buf, "boards/%s/KEY.%s.DIR",boardname,currentuser.userid);
        else if ( digestmode == 8 )
	        sprintf(buf, "boards/%s/.junk",boardname);
		else
                sprintf(buf, "boards/%s/%s", boardname, dir);
        //return buf;
}

int setbfile(char* buf, char* boardname, char* filename)
{
        sprintf(buf, "boards/%s/%s", boardname, filename);
        //return buf;
}

void setmfile(char *buf,char *userid, char *filename)
{
        sprintf(buf,"mail/%c/%s/%s",toupper(userid[0]),userid, filename);
}

char* setmdir(char* buf, char* userid)
{
        sprintf(buf,"mail/%c/%s/.DIR",toupper(userid[0]),userid);
        return buf;
}

int deny_me( char * bname)
{
        char    buf[STRLEN];
        setbfile(buf, bname, "deny_users");
        return seek_in_file(buf, currentuser.userid);
}
//add for autoundeny tdhlshx 2003.5.29  自动解封
int  seek_in_denyfile(char filename[STRLEN], char seekstr[STRLEN])
{
        FILE *fp;
        char buf[STRLEN];
        char * namep, * date;
        time_t nowtime;
        if((fp=fopen(filename,"r")) == NULL)
                return 1;

        while(fgets(buf,STRLEN,fp)!=NULL) {
                namep=(char *) strtok(buf,": \n\r\t");
                if(namep!=NULL && ci_strcmp(namep,seekstr)==0) {
                        fclose(fp);
                        nowtime = time(0);
                        getdatestring(nowtime,NA);
                        date = (char *) (buf+54);
                        if(strncmp(date,datestring,14)<=0) {

                                //                            deldeny(seekstr,1);
                                //			       	char msgbuf[STRLEN];
                                //				char repbuf[STRLEN];
                                //				sprintf(repbuf,"恢复%s在%s板的发文权限",seekstr,currboard);
                                //				sprintf(msgbuf,"\n  %s 网友：\n\n"
                                //						"\t因封禁时间已过，现恢复您在 [%s] 板的发文权限.\n\n
                                //						希望您能自觉遵守站规.我爱南开BBS感谢你的支持与合作.\n\n",
                                //						seekstr,currboard);
                                //				autoreport(repbuf,msgbuf,YEA,seekstr);
                                //
                                //				securityreport(repbuf);

                                return 0;
                        }
                        return 1;
                }
        }
        fclose(fp);
        return 1;
}
//add end*/

/*Add by SmallPig*/
void shownotepad()
{
        modify_user_mode(NOTEPAD);
        ansimore("etc/notepad",YEA);
        return;
}

int uleveltochar(char* buf, unsigned int lvl)
{
        if (!(lvl & PERM_BASIC)) {
                strcpy(buf, "--------- ");
                return 0;
        }
        //modify by share
        /*if(!strcmp(lookupuser.userid,"xixuan")||!strcmp(lookupuser.userid,"bridgenk")) {
                strcpy(buf, "          ");
                return 0;
        }*/
        //modify by swen
        buf[10]= '\0';
        buf[0] = (lvl & (PERM_CLOAK)   ) ? 'C':' ';//隐身
        buf[1] = (lvl & (PERM_SEECLOAK)) ? '#':' ';//看隐身
        buf[2] = (lvl & (PERM_BOARDS  )) ? 'B':' ';//板主
        buf[3] = (lvl & (PERM_XEMPT   )) ? 'X':' ';//永久帐号
        buf[4] = (lvl & (PERM_CHATCLOAK))? 'M':' ';//智囊团
        buf[5] = (lvl & (PERM_ACCOUNTS)) ? 'A':' ';//帐号管理
        buf[6] = (lvl & (PERM_OBOARDS )) ? 'O':' ';//讨论区管理
        buf[7] = (lvl & (PERM_DENYPOST)) ? 'D':' ';//修改基本权限
        buf[8] = (lvl & (PERM_SYSOP   )) ? 'S':' ';//系统维护
        buf[9] = (lvl & (PERM_POST    )) ? ' ':'p';//被封禁
        return 1;
}

void printutitle()
{
        move(2, 0);
        prints("[1;44m 编 号  使用者代号   %-19s #%-4s #%-4s %8s    %-12s  [m\n",
#if defined(ACTS_REALNAMES)
               HAS_PERM(PERM_SYSOP) ? "真实姓名" : "使用者昵称",
#else
               "使用者昵称",
#endif
#ifdef ALLOWGAME
               (toggle2 == 0) ? "上站" : (toggle2 == 1) ? "文章" : "存款",
               (toggle2 == 0) ? "时数" : (toggle2 == 1) ? "奖章" : "信件",
#else
               (toggle2 == 0) ? "上站" :  "文章" ,
               (toggle2 == 0) ? "时数" :  "信件" ,
#endif
               HAS_PERM(PERM_SEEULEVELS) ? " 等  级 " : "",
               (toggle1 == 0) ? "最近光临日期" :
               (toggle1 == 1) ? "最近光临地点" : "帐号建立日期");
}


int g_board_names(struct boardheader* fhdrp)
{
        if ((fhdrp->level & PERM_POSTMASK) || HAS_PERM(fhdrp->level)
                        || (fhdrp->level & PERM_NOZAP)) {
			if(fhdrp->filename[0] != 0 )
			{
                AddToNameList(fhdrp->filename);
				AddToNameList(fhdrp->title + 11);
			}
        }
        return 0;
}

void make_blist()
{
        CreateNameList();
        apply_boards(g_board_names);
}

int Select()
{
        modify_user_mode(SELECT);
        do_select(0, NULL, genbuf);
        return 0;
}

int Quick_Select()		// added 2002.3.31
{
        Select();
        Read();
}

int junkboard()
{
        if (strstr(JUNKBOARDS, currboard))
                return 1;
        else
                //return seek_in_file("etc/junkboards",currboard);
		return getbcache(currboard)->flag2&JUNK_FLAG;//by loveni
}

int Post()
{
        if (!selboard) {
                prints("\n\n先用 (S)elect 去选择一个讨论区。\n");
                pressreturn();
                clear();
                return 0;
        }
#ifndef NOREPLY
        *replytitle = '\0';
#endif

        do_post();
        return 0;
}

int Postfile(char* filename, char* nboard, char*  posttitle, int mode)
{
        char    bname[STRLEN];
        char    dbname[STRLEN];
        struct boardheader fh;
        if (search_record(BOARDS, &fh, sizeof(fh), cmpbnames, nboard) <= 0) {
                sprintf(bname, "%s 讨论区找不到", nboard);
                report(bname);
                return -1;
        }
        in_mail = NA;
        strcpy(quote_board, nboard);
        strcpy(dbname, currboard);
        strcpy(currboard, nboard);
        strcpy(quote_file, filename);
        strcpy(quote_title, posttitle);
        post_cross('l', mode);
        strcpy(currboard, dbname);
        return;
}

int get_a_boardname(char* bname, char* prompt)
{
        struct boardheader fh;
        make_blist();
        namecomplete(prompt, bname);
        if (*bname == '\0') {
                return 0;
        }
        if (search_record(BOARDS, &fh, sizeof(fh), cmpbnames, bname) <= 0) {
                move(1, 0);
                prints("错误的讨论区名称\n");
                pressreturn();
                move(1, 0);
                return 0;
        }
        return 1;
}
/* Add by SmallPig */
int do_cross(int ent,struct fileheader *fileinfo,char *direct)
{
        char    bname[STRLEN];
        char    dbname[STRLEN];
        char    ispost[10];
        set_safe_record();
        if (!HAS_PERM(PERM_POST))
                return DONOTHING;
	if(check_user_exist(fileinfo->owner))
		return FULLUPDATE;
        if (uinfo.mode != RMAIL)
                sprintf(genbuf, "boards/%s/%s", currboard, fileinfo->filename);
        else
                sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fileinfo->filename);
        strncpy(quote_file, genbuf, sizeof(quote_file));
        quote_file[sizeof(quote_file) - 1] = '\0';
        strcpy(quote_title, fileinfo->title);
	sprintf(genbuf,"%s",fileinfo->filename);//added by smalldog 

        clear();

        if(check_post_limit()) return FULLUPDATE; 

        if((!strcmp(currboard, "deleted"))&&uinfo.mode != RMAIL) {
                prints("\n\n          不允许从deleted板转出文章，如需恢复请按Y。");
                pressreturn();
                clear();
                return FULLUPDATE;
        }
        if(!strcmp(currboard, "Announce")&&!HAS_PERM(PERM_ACHATROOM)&&uinfo.mode != RMAIL)
	{
                clear();
                move(5,6);
                prints("\n\n                     不允许非站务人员从Announce板转出文章。");
                pressreturn();
                return FULLUPDATE;
        }
        //2003.4.26  added  by thunder
/*        if(!strcmp(currboard, "Headquarters")&&uinfo.mode != RMAIL) {

                clear();
                move(5,6);
                prints("\n\n                       本板文章不许转载,请遵守站规.");
                pressreturn();
                return FULLUPDATE;
        }

	if(!strcmp(currboard, "GrowingPains")&&uinfo.mode != RMAIL){
		clear();
		move(5,6);
		prints("\n\n                          本板文章不许转载,谢谢合作.");
		pressreturn();
		return FULLUPDATE;
	}
*/
        if(!strcmp(currboard, "TEMP")&&uinfo.mode != RMAIL) {
                prints("\n\n          本板禁用转载功能，请用Y键审核并恢复文章。");
                pressreturn();
                clear();
                return FULLUPDATE;
        }

        prints("[1;33m请珍惜本站资源，请在转载后删除不必要的文章，谢谢！ \n[1;32m并且，你不能将本文转载到本板，如果你觉得不方便的话，请与站长联系。[m\n\n");
        prints("您选择转载的文章是: [[1;33m%s[m]\n",quote_title);
        if(!get_a_boardname(bname, "您如果确定要转载的话，请输入要转贴的讨论区名称(取消转载请按回车): ")) {
                return FULLUPDATE;
        }
        if(!strcmp(bname,currboard)&&uinfo.mode != RMAIL) {
                prints("\n\n             很抱歉，你不能把文章转到同一个板上。");
                pressreturn();
                clear();
                return FULLUPDATE;
        }
	if(is_zone(BOARDS,bname))
        {
                clear();
                move(5,6);
                prints("\n\n                     不可以向子分区转载文章。");
                pressreturn();
                return FULLUPDATE;
        }
        move(3, 0);
        clrtoeol();
        prints("转载 ' %s ' 到 %s 板 ", quote_title, bname);
        move(4, 0);
        getdata(4, 0, "(S)转信 (L)本站 (A)取消? [A]: ", ispost, 9, DOECHO, YEA);
        if (ispost[0] == 's' || ispost[0] == 'S' || ispost[0] == 'L' || ispost[0] == 'l') {
                int ret;
                strcpy(quote_board, currboard);
                strcpy(dbname, currboard);
                strcpy(currboard, bname);
                if ((ret = post_cross(ispost[0], 0)) < 0) {
                        if (ret != -2)
                                pressreturn();
                        move(2, 0);
                        strcpy(currboard, dbname);
                        return FULLUPDATE;
                }
                strcpy(currboard, dbname);
                if(!badwords)
                        prints("\n已把文章 \'%s\' 转贴到 %s 板\n", quote_title, bname);
                else
                        prints("取消");
        } else {
                prints("取消");
        }
        move(2, 0);
        pressreturn();
        return FULLUPDATE;
}

void readtitle()
{
        struct shortfile *bp;
        int i,j,bnum,tuid;
        struct user_info uin;
        char tmp[40],bmlists[4][16];
        extern int t_cmpuids();

        char    header[STRLEN], title[STRLEN];
        char    readmode[11];
        bp = getbcache(currboard);
        memcpy(currBM, bp->BM, BM_LEN - 1);
        bnum = 0;
        for (i =0,j =0 ;currBM[i] != '\0';i++) {
                if( currBM[i] == ' ' ) {
                        bmlists[bnum][j] = '\0';
                        bnum ++;
                        j = 0;
                } else
                        bmlists[bnum][j++] = currBM[i];
        }
        bmlists[bnum++][j] = '\0';
        if (currBM[0] == '\0' || currBM[0] == ' ') {
#ifdef AddWaterONLY
                strcpy(header, "多谢关注本板");
#else

                strcpy(header, "诚征板主中");
#endif

        } else {
                strcpy(header, "板主: ");
                for(i = 0 ; i< bnum; i++) {
                        tuid=getuser(bmlists[i]);
                        search_ulist(&uin,t_cmpuids,tuid);
                        if (uin.active && uin.pid && !uin.invisible ) {
                                sprintf(tmp,"[32m%s[33m ",bmlists[i]);
                        } else if(uin.active && uin.pid && uin.invisible && (HAS_PERM(PERM_SEECLOAK)||usernum == uin.uid||canseeme(&uin))) {
                                sprintf(tmp,"[36m%s[33m ",bmlists[i]);
                        } else
                                sprintf(tmp,"%s ",bmlists[i]);
                        strcat(header,tmp);
                }
                /*if(bnum==4)
                        strcpy(header, header+6);add by bluetent*/
        }
        if (chkmail()&&(mail_unread()>0)) //tdhlshx
                sprintf(title, "[%d封新信, 按 M 查看]", mail_unread());
        else if ((bp->flag & VOTE_FLAG))
                sprintf(title, "※投票中,按 v 进入投票※");
        else if (!strcmp(currboard,"ASCIIArt"))
                sprintf(title, "○ 美丽的图案(翻屏作品推荐Ctrl+o发表)");//modified by yiyo
        else {
                if(bnum==4)
                        strcpy(title, bp->title + 11);
                else
                        strcpy(title, bp->title + 8);
        }

        showtitle(header, title);
        prints("离开[[1;32m←[m,[1;32me[m] 选择[[1;32m↑[m,[1;32m↓[m] 阅读[[1;32m→[m,[1;32mRtn[m] 发表文章[[1;32mCtrl-P[m] 砍信[[1;32md[m] 备忘录[[1;32mTAB[m] 求助[[1;32mh[m]\n");
        if (digestmode == 0) {
                if (DEFINE(DEF_THESIS))	/* youzi 1997.7.8 */
                        strcpy(readmode, "主题");
                else
                        strcpy(readmode, "一般");
        } else if (digestmode == 1)
                strcpy(readmode, "文摘");
        else if (digestmode == 2)
                strcpy(readmode, "主题");
        else if (digestmode == 3)
                strcpy(readmode,"MARK");
        else if (digestmode == 4)
                strcpy(readmode,"原作");
        else if (digestmode == 7)
                strcpy(readmode,"标题关键字");
	else if ( digestmode == 8)
		strcpy(readmode,"回收站");
			
        if (DEFINE(DEF_THESIS) && digestmode == 0)
                prints("[1;37;44m 编号   %-12s %6s %-12s(板内在线: %3d)           [%4s式看板] [m\n", "刊 登 者", "日  期", " 标  题", ShowBoardOnline(currboard), readmode);
        else if (digestmode == 5 || digestmode == 6)
                prints("[1;37;44m 编号   %-12s %6s %-10s (关键字: [32m%-12s[37m) [[33m%s[37m同作者阅读] [m\n","刊 登 者", "日  期", " 标  题", someoneIDs,(digestmode==5)?"模糊":"精确");
        else if (digestmode == 7 )
                prints("[1;37;44m 编号   %-12s %6s %-12s(板内在线: %3d)     [%10s式看板] [m\n", "刊 登 者", "日  期", " 标  题", ShowBoardOnline(currboard), readmode);
        else
                prints("[1;37;44m 编号   %-12s %6s %-12s(板内在线: %3d)             [%4s模式] [m\n", "刊 登 者", "日  期", " 标  题", ShowBoardOnline(currboard), readmode);
        clrtobot();
}

char* readdoent(int num, struct fileheader* ent)    // 文章列表
{

        int iscompact=0;
        if(uinfo.mode==COMPACTREADING)
                iscompact=1;

        static char buf[128];
        struct shortfile *bp;
        time_t  filetime;
        char   *date;
        char   *TITLE, color[10],ctype[10],cnum[10], color2[10],color3[10];
        int     type;
        int		attach;
#ifdef COLOR_POST_DATE
	struct user_info uin;
	int    tuid;
	extern int t_cmpuids();
        struct tm *mytm;
#endif

        type = brc_unread(ent->filename) ?
               (!DEFINE(DEF_NOT_N_MASK)?'N':'+') : ' ';

        if(iscompact==1)
                type=' ';

        if ((ent->accessed[0] & FILE_DIGEST) /* && HAS_PERM(PERM_MARKPOST) */ ) {
                if (type == ' ')
                        type = 'g';
                else
                        type = 'G';
        }
        if (ent->accessed[0] & FILE_MARKED) {
                switch (type) {
                case ' ':
                        type = 'm';
                        break;
                case 'N':
                case '+':
                        type = 'M';
                        break;
                case 'g':
                        type = 'b';
                        break;
                case 'G':
                        type = 'B';
                        break;
                }
        }
        //	if(ent->accessed[0] & FILE_DELETED  && chk_currBM(currBM)) {
        //		if(brc_unread(ent->filename))type = 'X'; // deardragon 0729
        //		else type = 'x';
        //	}
        //彩色文章标记
        if (DEFINE(DEF_COLORMARK)) {
                if (type=='+'||type=='N')
                        strcpy(ctype,"[32m");
                else if (type=='m'||type=='M')
                        strcpy(ctype,"[33m");
                else if (type=='g'||type=='G')
                        strcpy(ctype,"[36m");
                else if (type=='b'||type=='B')
                        strcpy(ctype,"[35m");
                //        else if (type=='x'||type=='X') strcpy(ctype,"[34m");
                else
                        strcpy(ctype,"[37m");
        } else
                strcpy(ctype,"[37m");

        strcpy (cnum, "");
        if(ent->accessed[0] & FILE_VISIT && chk_currBM(currBM))
                strcpy(cnum,"\033[33m"); //add by yiyo 收入精华编号变色

        if (ent->accessed[1] & FILE_TIP && chk_currBM(currBM))
                strcpy (cnum, "\33[35m");
        bp = getbcache(currboard);
        if ( ent->accessed[0] & FILE_NOREPLY || bp->flag & NOREPLY_FLAG)
                noreply = 1;
        else
                noreply = 0;
        if (ent->filename[STRLEN-3]=='U')
                attach=1;
        else
                attach=0;

        /* add by bluetent TEMP板面已恢复文章标记借用一下attach */
        /*
        	if(!strcmp(currboard, "TEMP"))
        	{
        		//flaghere
        		if ((ent->accessed[0] & FILE_FORWARDED))
        			attach=1;
        		else
        			attach=0;
        	}
        */
        /* add end */

        if (ent->accessed[1] & FILE_ONTOP) {
                time_t now = time (0);
                sprintf (genbuf, "%d", now);
                filetime = atoi (genbuf);
        } else
                filetime = atoi(ent->filename + 2);
        if (filetime > 740000000) {
                date = ctime(&filetime) + 4;
        } else {
                date = "";
        }

#ifdef COLOR_POST_DATE
        mytm = localtime(&filetime);
        strftime(buf, 5, "%w", mytm);
        sprintf(color, "[1;%dm", 30 + atoi(buf) + 1);
	if(!DEFINE(DEF_USEGB))
	{
		tuid=getuser(ent->owner);
		search_ulist(&uin,t_cmpuids,tuid);
		if(!strcasecmp(ent->owner,BBSID)){
			strcpy(color2,"\33[33m");
			strcpy(color3,"\33[m");
		}
		else if (uin.active && uin.pid && !uin.invisible) 
		{
			strcpy(color2,"\33[32m");
			strcpy(color3,"\33[m");
		} 
		else if(uin.active && uin.pid && uin.invisible && 
			(HAS_PERM(PERM_SEECLOAK)||usernum == uin.uid||canseeme(&uin))) 
		{
			strcpy(color2,"\33[36m");
			strcpy(color3,"\33[m");
		}
		else
		{
        		strcpy(color2, "");
        		strcpy(color3, "");
		} 
	}
	else
	{
        	strcpy(color2, "");
        	strcpy(color3, "");
	}

#else

        strcpy(color, "");
        strcpy(color2, "");
        strcpy(color3, "");
#endif

        /* Re-Write By Excellent */

        /* 990604.edwardc 顺便重排一下缩排 .. */

        TITLE = ent->title;

        if ( TITLE[47] )
                strcpy(TITLE + 44, "...");  /* 把多馀的 string 砍掉 */

      /*  if (ent->accessed[1] & FILE_ONTOP)
	 {
                sprintf (buf, " \33[33m[提示] \33[m%-12.12s %s%6.6s\33[m  ◆ %-.45s",
                         getname(ent->owner), color, date, ent->title);
        }*/
	
	if (ent->accessed[1] & FILE_ONTOP) 
	{
		if (ent->accessed[1] & FILE_STYLE1)
		{
	    sprintf (buf," \33[1;31m[公告] \33[m%s%-12.12s%s %s%6.6s\33[m  ◆ %-.45s",		    
			    color2,getname (ent->owner),color3, color, date, ent->title);
        	}
	else if (ent->accessed[1] & FILE_STYLE2)
		 {
	    sprintf (buf, " \33[1;35m[板规] \33[m%s%-12.12s%s %s%6.6s\33[m  ◆ %-.45s",
		     color2,getname (ent->owner),color3, color, date, ent->title);
		}
	else if (ent->accessed[1] & FILE_STYLE3)
	{
	    sprintf (buf,
		     " \33[1;32m[推荐] \33[m%s%-12.12s%s %s%6.6s\33[m  ◆ %-.45s",
		     color2,getname (ent->owner),color3, color, date, ent->title);
	}
	else if (ent->accessed[1] & FILE_STYLE4)
	{
	    sprintf (buf,
		     " \33[1;34m[精华] \33[m%s%-12.12s%s %s%6.6s\33[m  ◆ %-.45s",
		     color2,getname (ent->owner),color3, color, date, ent->title);
	}
	else if (ent->accessed[1] & FILE_STYLE5)
	{
	    sprintf (buf,
		     " \33[1;36m[其他] \33[m%s%-12.12s%s %s%6.6s\33[m  ◆ %-.45s",
		     color2,getname (ent->owner),color3, color, date, ent->title);
	}
	else
	    sprintf (buf,
		     " \33[1;33m[提示] \33[m%s%-12.12s%s %s%6.6s\33[m  ◆ %-.45s",
		     color2,getname (ent->owner),color3, color, date, ent->title);

                
        }   /*  loveni: 多种置底提示*/

        else  if (FFLL == 0) {
                if (!strncmp("Re:", ent->title, 3) || !strncmp("RE:", ent->title, 3)) {
                        sprintf(buf, " %s%4d\033[m %s%c[m %s%-12.12s%s %s%6.6s[m %s%s[m%-.48s ", cnum,num, ctype,type,
                                color2,getname(ent->owner),color3, color, date, attach?"[45m":"", noreply?"[1;4;33mx[m":" ",TITLE);
                } else {
                        sprintf(buf, " %s%4d\033[m %s%c[m %s%-12.12s%s %s%6.6s[m %s%s[m● %-.45s ", cnum, num, ctype,type,
                                color2,getname(ent->owner),color3, color, date, attach?"[45m":"", noreply?"[1;4;33mx[m":" ",TITLE);
                }
        } else {
                if (!strncmp("Re:", ent->title, 3) || !strncmp("RE:", ent->title, 3)) {
                        if (strncmp(ReplyPost, ent->title, 40) == 0) {
                                sprintf(buf,
                                        " [1;36m%s%4d[m %s%c[m %s%-12.12s%s %s%6.6s.%s%s[m[1;36m%-.48s[m ",
                                        cnum,num, ctype,type, color2,getname(ent->owner),color3, color, date, attach?"[45m":"",
                                        noreply?"[1;4;33mx[m":" ",TITLE);
                        } else {
                                sprintf(buf, " %s%4d\033[m %s%c[m %s%-12.12s%s %s%6.6s[m %s%s[m%-.48s ", cnum,num,ctype, type,
                                        color2,getname(ent->owner),color3, color, date, attach?"[45m":"", noreply?"[1;4;33mx[m":" ",TITLE);
                        }
                } else {
                        if (strncmp(ReadPost, ent->title, 40) == 0) {
                                sprintf(buf,
                                        " [1;32m%s%4d[m %s%c[m %s%-12.12s%s %s%6.6s.%s%s[m[1;32m● %-.45s[m "
                                        ,cnum,num, ctype,type, color2,getname(ent->owner),color3, color, date, attach?"[45m":"",
                                        noreply?"[1;4;33mx[m":" ",TITLE);
                        } else {
                                sprintf(buf, " %s%4d\033[m %s%c[m %s%-12.12s%s %s%6.6s[m %s%s[m● %-.45s ",cnum,num,ctype,type,
                                        color2,getname(ent->owner),color3, color, date, attach?"[45m":"", noreply?"[1;4;33mx[m":" ",TITLE);
                        }
                }
        }
        noreply = 0;
        return buf;
}

/* add by yiyo 把bbs.bbs@bbs.*显示成 bbs. */
char* getname(char * name)
{
        static char buf[30];
        char *ptr;
        strcpy(buf,name);
        if(ptr=strstr(buf, "..bbs@"))
                ptr[1]=0;
        if(ptr=strstr(buf, ".bbs@"))
                ptr[1]=0;
        return buf;
}
/* add end */

char    currfile[STRLEN];

int cmpfilename(struct fileheader* fhdr)
{
        if (!strncmp(fhdr->filename, currfile, STRLEN))
                return 1;
        return 0;
}

int cmpdigestfilename(char* digest_name, struct fileheader* fhdr)
{
        if(!strcmp(fhdr->filename,digest_name))
                return 1 ;
        return 0 ;
}     /* comapare file names for dele_digest function. Luzi 99.3.30 */

int cpyfilename( struct fileheader *fhdr )
{
        char    buf[STRLEN];
        time_t  tnow;
        struct tm *now;

        tnow = time(0);
        now = localtime(&tnow);
        sprintf(buf, "-%s", fhdr->owner);
        strncpy(fhdr->owner, buf, IDLEN + 2);
        sprintf(buf, "<< 本文被 %s 于 %d/%d %d:%02d:%02d 删除 >>",
                currentuser.userid, now->tm_mon + 1, now->tm_mday,
                now->tm_hour, now->tm_min, now->tm_sec);
        strncpy(fhdr->title, buf, STRLEN);
        fhdr->filename[STRLEN - 1] = 'L';
        fhdr->filename[STRLEN - 2] = 'L';
        return 0;
}

int *nums1=0;
int read_post(int ent, struct fileheader* fileinfo, char* direct)
{
        char   *t;
        char    buf[512];
        int     ch;
        int     cou;

        int iscompact=0;
        if(uinfo.mode==COMPACTREADING)
                iscompact=1;
        if(!iscompact)
                brc_addlist(fileinfo->filename);
        /*	{
        //        int *nums;
                char buf2[256], id1[100], accessed1[20], title1[STRLEN];
                sprintf(buf2, "boards/%s/%s", currboard, fileinfo->filename);
        //        nums=(int*) (fileinfo->title+72);
                if(fileinfo->title[71]!=0) {
                    (*nums1)= 0;
                    fileinfo->title[71]=0;
                } 
                getfile(direct, ent, id1, accessed1, title1);
                if(!strcmp(id1, fileinfo->filename)&&!strcmp(accessed1, fileinfo->accessed)&&(strncmp(title1, fileinfo->title, 50)==0))
        		{
        //	        *nums=*nums1+1;
        //			strcpy(fileinfo->title, title1);
        			*(int*)(fileinfo->title+72)=(*nums1)+1;
        			substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        		}
            } 
        */

        if (fileinfo->owner[0] == '-')
                return FULLUPDATE;
	if(check_user_exist(fileinfo->owner))
		return FULLUPDATE;
        clear();
        strcpy(buf, direct);
        if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
        sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
        if(!dashf(genbuf)) {
        	snprintf(genbuf, sizeof(genbuf), "boards/%s/%s", ((struct fileheader_v *)fileinfo)->board,
				fileinfo->filename);
			if(!dashf(genbuf)){
                clear();
                move(10,30);
                prints("对不起，本文内容丢失！");
                pressanykey();
                return FULLUPDATE; //deardragon 0729
			}
        }
        strncpy(quote_file, genbuf, sizeof(quote_file));
        strcpy(quote_board, currboard);
        strcpy(quote_title, fileinfo->title);
        quote_file[255] = fileinfo->filename[STRLEN - 2];
        strcpy(quote_user, fileinfo->owner);
#ifndef NOREPLY

        ch = ansimore(genbuf, NA);
#else
        ch = ansimore(genbuf, YEA);
#endif
/*        move(t_lines - 2, 0); 
	clrtoeol();
	prints("[1;31mhttp://nkbbs.org/cgi-bin/bbs/newcon?board=%s&file=%s&mode=3",currboard,fileinfo->filename); 
*/
#ifndef NOREPLY

        move(t_lines - 1, 0);
        clrtoeol();
        if (haspostperm(currboard)) {
                prints("[1;44;31m[阅读文章]  [33m回信 R │ 结束 Q,← │上一封 ↑│下一封 <Space>,↓│主题阅读 ^X或p [m");
        } else {
                prints("[1;44;31m[阅读文章]  [33m结束 Q,← │上一封 ↑│下一封 <Space>,<Enter>,↓│主题阅读 ^X 或 p [m");
        }

        /* Re-Write By Excellent */

        FFLL = 1;
        if (strncmp(fileinfo->title, "Re:", 3) != 0) {
                strcpy(ReplyPost, "Re: ");
                strncat(ReplyPost, fileinfo->title, STRLEN - 4);
                strcpy(ReadPost, fileinfo->title);
        } else {
                strcpy(ReplyPost, fileinfo->title);
                for (cou = 0; cou < STRLEN; cou++)
                        ReadPost[cou] = ReplyPost[cou + 4];
        }

        refresh();
#ifndef BBSD

        sleep(1);
#endif

        if (!(ch == KEY_RIGHT || ch == KEY_UP || ch == KEY_PGUP))
                ch = egetch();

        switch (ch) {
        case 'N':
        case 'Q':
        case 'n':
        case 'q':
        case KEY_LEFT:
                break;
        case ' ':
        case 'j':
        case KEY_RIGHT:
                if (DEFINE(DEF_THESIS)) {	/* youzi */
                        /*			curr_ent=ent;//bluetent
                         */
                        /* Efan: 同主题阅读时，如果是用右键进入，则退出时光标停留在当前帖子前	*/
                        curr_ent = 0;
                        sread(0, 0, fileinfo);
                        break;
                } else
                        return READ_NEXT;
        case KEY_DOWN:
        case KEY_PGDN:
                return READ_NEXT;
        case KEY_UP:
        case KEY_PGUP:
                return READ_PREV;
        case 'Y':
        case 'R':
        case 'y':
        case 'r':
                if(!iscompact)     {
                        struct shortfile *bp;
                        bp = getbcache(currboard);
                        noreply = fileinfo->accessed[0] & FILE_NOREPLY
                                  ||bp->flag & NOREPLY_FLAG ;
                        local_article=(fileinfo->filename[STRLEN-2]=='L');
                        if(!noreply || HAS_PERM(PERM_SYSOP) ||chk_currBM(currBM))
                                do_reply(fileinfo->title,fileinfo->owner);
                        else {
                                clear();
                                prints("\n\n    对不起, 该文章有不可 RE 属性, 你不能回复(RE) 这篇文章.    ");
                                pressreturn();
                                clear();
                        }
                }
                break;
        case Ctrl('R'):
                                        if(!iscompact)
                                                post_reply(ent, fileinfo, direct);
                break;
        case 'g':
                if(!iscompact)
                        digest_post(ent, fileinfo, direct);
                break;
        case Ctrl('U'):
                                        sread(1, 1, fileinfo);
                break;
        case Ctrl('N'):
                                        locate_the_post(fileinfo, fileinfo->title,5,0,1);
                sread(1, 0, fileinfo);
                break;
        case Ctrl('S'):
        case 'p':
                                                /* Efan: p进入同主题阅读时，光标停留在当前篇	*/
                curr_ent = 0;
                sread (0, 0, fileinfo);
                break;
        case Ctrl('X'):		/* Add by SmallPig */
                                        curr_ent=ent;
                sread(0, 0, fileinfo);
                //		if (cursor_pos(locmem, current, 10))
                //			return PARTUPDATE;
                //		cursor_pos(locmem, curr_ent, 10);
                break;
        case Ctrl('A'):	/* Add by SmallPig */
                                        clear();
                show_author(0, fileinfo, '\0');
                return READ_NEXT;
                break;
        case Ctrl('h'):
                                        clear();
                deny_from_article(0,fileinfo,'\0');
                break;
        case 'S':		/* by youzi */
                if (!HAS_PERM(PERM_MESSAGE))
                        break;
                clear();
                s_msg();
                break;
        case 'Z':
                if (!HAS_PERM(PERM_MESSAGE))
                        break;
                sendmsgtoauthor(0,fileinfo,'\0');
                break;  //add by yiyo 直接给作者发消息
        case 'd':
        case 'D':
                x_dict();
                return DONOTHING;
                break;//add by yiyo 阅读文章时查字典
        default:
                break;
        }
#endif
        return FULLUPDATE;
}

int skip_post(int ent, struct fileheader* fileinfo,char* direct)
{
        brc_addlist(fileinfo->filename);
        return GOTO_NEXT;
}

int action_rel=0;

int do_select(int ent,struct fileheader  *fileinfo,char *direct)
{
        char    bname[STRLEN], bpath[STRLEN];
        struct stat st;
        move(0, 0);
        clrtoeol();
        prints("选择一个讨论区 (英文字母大小写皆可)\n");
        prints("输入讨论区名 (按空白键自动搜寻): ");
        clrtoeol();
        make_blist();
        namecomplete((char *) NULL, bname);
        if(*bname == '\0'){
			if (digestmode == 8){
				digestmode = NA;
				do_acction(digestmode);
			}
            return FULLUPDATE;
		}
        setbpath(bpath, bname);		
		if(stat(bpath,&st) == -1){
			bzero(bpath, STRLEN);
			getboardbytitle(bname);
			setbpath(bpath, bname);
		}
        if (/* (*bname == '\0') || */(stat(bpath, &st) == -1)) {
                move(2, 0);
                prints("不正确的讨论区.\n");
                pressreturn();
                return FULLUPDATE;
        }
        if (!(st.st_mode & S_IFDIR)) {
                move(2, 0);
                prints("不正确的讨论区.\n");
                pressreturn();
                return FULLUPDATE;
        }
        //added by silencer板面外网屏蔽//
       /* if(!strcmp("SARS",bname) && !isAllowedIp(fromhost)) {
                move(2, 0);
                prints("板面系统维护中\n");
                pressreturn();
                move(2, 0);
                clrtoeol();
                return -1;
        }*/
#ifdef BOARD_CONTROL
        if(!HAS_PERM(PERM_SYSOP)||deny_s_board(bname)) {
                setbfile(genbuf, bname, "board.allow");
                if(dashf(genbuf)&&!seek_in_file(genbuf,currentuser.userid)) {
                        clear();
                        move(5,10);
                        prints("很抱歉, 本板面属于团体板面，而您不是该板成员");
                        pressanykey();
                        return FULLUPDATE;
                }
        }
#endif
#ifdef BOARD_READDENY
        if(!HAS_PERM(PERM_SYSOP)) {
                setbfile(genbuf, bname, "board.deny");
                if(dashf(genbuf)&&seek_in_file(genbuf,currentuser.userid)) {
                        clear();
                        move(5,20);
                        prints("对不起，您目前被停止进入本板的权利。");
                        pressanykey();
                        return FULLUPDATE;
                }
        }
#endif
        selboard = 1;
        brc_initial(bname);
        move(0, 0);
        clrtoeol();
        move(1, 0);
        clrtoeol();
	//add by brew
	if(digestmode!=NA)
	{
	    if (digestmode == 8)
		digestmode = NA;
            if( digestmode != 1 )
                    do_acction(digestmode);
            if (action_rel==NA||!dashf(direct)) {
                    digestmode = NA;
            }
	}
	setbdir(direct, currboard);
        if(uinfo.board[0]!='-' && uinfo.board[0]!='\n' && getbnum(uinfo.board) != 0)
	{
                if((*(int*) (bcache[getbnum(uinfo.board)-1].filename+72))>0)
		{
                        (*(int*) (bcache[getbnum(uinfo.board)-1].filename+72))--;
		}
	}
        strcpy(uinfo.board, currboard);
        update_ulist(&uinfo, utmpent);
        (*(int*) (bcache[getbnum(uinfo.board)-1].filename+72))++;
        return NEWDIRECT;
}


int do_select_topten(char *bname,char *id,char *title)
{
        char    bpath[STRLEN];
	extern	char currdirect[STRLEN];
        struct stat st;
	char	tempdir[STRLEN];
	struct fileheader x;
	int	total;
	int	start;
	FILE	*fp;
        if(*bname == '\0')
                return FULLUPDATE;
        setbpath(bpath, bname);
        if (stat(bpath, &st) == -1) 
	{
                move(2, 0);
                prints("不正确的讨论区.\n");
                pressreturn();
                return FULLUPDATE;
        }
        if (!(st.st_mode & S_IFDIR)) {
                move(2, 0);
                prints("不正确的讨论区.\n");
                pressreturn();
                return FULLUPDATE;
        }
#ifdef BOARD_CONTROL
        if(!HAS_PERM(PERM_SYSOP) || deny_s_board(bname)) {
                setbfile(genbuf, bname, "board.allow");
                if(dashf(genbuf)&&!seek_in_file(genbuf,currentuser.userid)) {
                        clear();
                        move(5,10);
                        prints("很抱歉, 本板面属于团体板面，而您不是该板成员");
                        pressanykey();
                        return FULLUPDATE;
                }
        }
#endif
#ifdef BOARD_READDENY
        if(!HAS_PERM(PERM_SYSOP)) {
                setbfile(genbuf, bname, "board.deny");
                if(dashf(genbuf)&&seek_in_file(genbuf,currentuser.userid)) {
                        clear();
                        move(5,20);
                        prints("对不起，您目前被停止进入本板的权利。");
                        pressanykey();
                        return FULLUPDATE;
                }
        }
#endif
        selboard = 1;
        brc_initial(bname);
        move(0, 0);
        clrtoeol();
        move(1, 0);
        clrtoeol();
	//add by brew
	if(digestmode!=NA)
	{
	    if (digestmode == 8)
		digestmode = NA;
            if( digestmode != 1 )
                    do_acction(digestmode);
            if (action_rel==NA||!dashf(currdirect)) {
                    digestmode = NA;
            }
	}
	setbdir(currdirect, currboard);
        if(uinfo.board[0]!='-' && uinfo.board[0]!='\n' && getbnum(uinfo.board) != 0)
	{
                if((*(int*) (bcache[getbnum(uinfo.board)-1].filename+72))>0)
		{
                        (*(int*) (bcache[getbnum(uinfo.board)-1].filename+72))--;
		}
	}
        strcpy(uinfo.board, currboard);
        update_ulist(&uinfo, utmpent);
        (*(int*) (bcache[getbnum(uinfo.board)-1].filename+72))++;
	sprintf(tempdir, "boards/%s/.DIR", bname);
	total=file_size(tempdir)/sizeof(struct fileheader);
	fp=fopen(tempdir, "r");
	if(fp!=NULL)
	{
		for(start=1;start<=total;start++) 
		{
			if(fread(&x, sizeof(x), 1, fp)<=0)
			{
         			break;
     			}
     			if(!strncmp(x.title,title,strlen(title))&&strlen(x.title)==strlen(title)) 
			{
				temphotinfonumber=start;
          			break;
     			}
		}	
		fclose(fp);
	}
        return NEWDIRECT;
}

/*
 
int
read_letter(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
	setmdir(direct,currentuser.userid);
	return NEWDIRECT;
}
*/
void do_acction(int type)
{
	int rel_=0;
        char    buf[STRLEN];
        move(t_lines - 1, 0);
        clrtoeol();
        prints("[1;5m系统处理标题中, 请稍候...[m\n");
        refresh();
        switch(type) {
        case 2:
                sprintf(buf, "bin/thread %s", currboard);
                system(buf);
		action_rel=type;
                break;
	case 8: /* junk模式 */
		action_rel=type;
		break;
        case 3:
        case 4:
        case 5: /* 同作者 */
        case 6: /* 同作者  精确 */
        case 7: /* 标题关键字 */
//	case 8: /* junk模式 */
               if( (rel_=marked_all(type-3)) <= 0 )
		{
			action_rel=NA;
		//	digestmode = NA;
		}
		else
		{
			action_rel=type;
		}
		move(0,0);
	//d	prints("k: %d, %d",digestmode,rel_);
	//d	pressanykey();
        //d        break;
        }
}

int acction_mode()
{
        int type;
        extern char currdirect[STRLEN];
        char ch[4];

/*        if (digestmode != NA) {
                if(digestmode == 5 || digestmode == 6 ) {
                        sprintf(genbuf,"boards/%s/SOMEONE.%s.DIR.%d",currboard,someoneID,digestmode-5);
                        unlink(genbuf);
                } else if (digestmode == 7 ) {
                        sprintf(genbuf,"boards/%s/KEY.%s.DIR",currboard,currentuser.userid);
                        unlink(genbuf);
                }
                digestmode = NA;
                setbdir(currdirect, currboard);
        } else
*/ {
  
              saveline(t_lines - 1, 0);
                move(t_lines - 2, 0);//livebird 2005-02
                clrtoeol();
                ch[0] = '\0';
		if (chk_currBM(currBM)||HAS_PERM(PERM_FORCEPAGE))
                	getdata(t_lines - 2, 0, "切换到: 1)文摘 2)同主题 3)被 m 文章 4)原作 5)同作者 6)标题关键字 7)回收站[1]: ", ch, 3, DOECHO, YEA);
                else 
			getdata(t_lines - 2, 0, "切换到: 1)文摘 2)同主题 3)被 m 文章 4)原作 5)同作者 6)标题关键字 [1]: ", ch, 3, DOECHO, YEA);
		if ( ch[0] == '\0' )
                        ch[0] = '1';
                type = atoi(ch);
		if (!chk_currBM(currBM)&&!HAS_PERM(PERM_FORCEPAGE)&&(type == 7))
		{
			saveline(t_lines - 1, 1);
			return PARTUPDATE;
		}
                if (type < 1 || type > 7) {
                        saveline(t_lines - 1, 1);
                        return PARTUPDATE;
                } else if ( type == 6 ) {
                        getdata(t_lines-1,0,"您想查找的文章标题关键字: ",
                                someoneID,30,DOECHO,YEA);
                        if(someoneID[0] == '\0') {
                                saveline(t_lines-1,1);
                                return PARTUPDATE;
                        }
                        type = 7;
                } else if ( type == 5 ) {
                        getdata(t_lines-1,0,"您想查找哪位网友的文章? ",someoneID,13,DOECHO,YEA);
                        if(someoneID[0] == '\0' ) {
                                saveline(t_lines -1 , 1);
                                return PARTUPDATE;
                        }
                        getdata(t_lines-1,37, "精确查找按 Y， 模糊查找请回车[Enter]",ch,2,DOECHO,YEA);
                        if( ch[0] == 'y' || ch[0] == 'Y')
                                type = 6;
                } else if ( type == 7)
	 	        type = 8;
			      
                //digestmode = type;
                setbdir(currdirect, currboard);
                if( type != 1 )
                {
			 do_acction(type);
			 strcpy(someoneIDs,someoneID);
			 digestmode = action_rel;
		}
		else
		{
			digestmode=1;
		}
		setbdir(currdirect, currboard);
                if (digestmode==NA||!dashf(currdirect)) {
                        digestmode = NA;
                        setbdir(currdirect, currboard);
                        return PARTUPDATE;
                }
        }
        return NEWDIRECT;
}

int dele_digest(char* dname, char* direc)
{
        char    digest_name[STRLEN];
        char    new_dir[STRLEN];
        char    buf[STRLEN];
        char   *ptr;
        struct fileheader fh;
        int     pos;
        strncpy(digest_name, dname,STRLEN);
        strcpy(new_dir, direc);
        digest_name[0] = 'G';
        ptr = strrchr(new_dir, '/') + 1;
        strcpy(ptr, DIGEST_DIR);
        strncpy(buf, currfile,STRLEN);
        strncpy(currfile, digest_name,STRLEN);
        pos = search_record(new_dir, &fh, sizeof(fh), cmpdigestfilename, digest_name);
        if (pos <= 0) {
                return;
        }
        delete_file(new_dir, sizeof(struct fileheader), pos, cmpfilename);
        strncpy(currfile, buf,STRLEN);
        *ptr = '\0';
        sprintf(buf, "%s%s", new_dir, digest_name);
        unlink(buf);
        return;
}

int digest_post(int ent,struct fileheader* fhdr, char* direct)
{
        /*
        	if (!chk_currBM(currBM)) {
        		return DONOTHING;
        	}
        */
        /* Efan: 站务组成员可在SYSOPs版使用mgb功能	*/
        if (!chk_currBM (currBM) && (strcmp (currBM, "SYSOPs") || ! HAS_PERM (PERM_ACHATROOM) ) ) {
                return DONOTHING;
        }

        if (fhdr->accessed [1] & FILE_ONTOP)
                return DONOTHING;
        if (fhdr->accessed[0] & FILE_DIGEST) {
                fhdr->accessed[0] = (fhdr->accessed[0] & ~FILE_DIGEST);
                dele_digest(fhdr->filename, direct);
        } else {
                struct fileheader digest;
                char   *ptr, buf[64];
                memcpy(&digest, fhdr, sizeof(digest));
                digest.filename[0] = 'G';
                strcpy(buf, direct);
                ptr = strrchr(buf, '/') + 1;
                ptr[0] = '\0';
                sprintf(genbuf, "%s%s", buf, digest.filename);
                if (dashf(genbuf)) {
                        fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
                        substitute_record(direct, fhdr, sizeof(*fhdr), ent);
                        return PARTUPDATE;
                }
                digest.accessed[0] = 0;
                sprintf(&genbuf[512], "%s%s", buf, fhdr->filename);
                link(&genbuf[512], genbuf);
                strcpy(ptr, DIGEST_DIR);
                /* REMOVED by bluetent 2003.02.26 */
                /*		if (get_num_records(buf, sizeof(digest)) > MAX_DIGEST) {
                			move(3, 0);
                			clrtobot();
                			move(4, 10);
                			prints("抱歉，你的文摘文章已经超过 %d 篇，无法再加入...\n", MAX_DIGEST);
                			pressanykey();
                			return PARTUPDATE;
                		}*/
                append_record(buf, &digest, sizeof(digest));
                fhdr->accessed[0] = fhdr->accessed[0] | FILE_DIGEST;
                fhdr->accessed[0] &= ~FILE_DELETED;
        }
        substitute_record(direct, fhdr, sizeof(*fhdr), ent);
        return PARTUPDATE;
}
#ifndef NOREPLY
int do_reply(char *title,char* id)
{
        strcpy(replytitle, title);
        post_article(currboard,id);
        replytitle[0] = '\0';
        return FULLUPDATE;
}
#endif

int garbage_line(char* str)
{
        int     qlevel = 0;
        while (*str == ':' || *str == '>') {
                str++;
                if (*str == ' ')
                        str++;
                if (qlevel++ >= 1)
                        return 1;
        }
        while (*str == ' ' || *str == '\t')
                str++;
        if (qlevel >= 1)
                if (strstr(str, "提到:\n") || strstr(str, ": 】\n") || strncmp(str, "==>", 3) == 0 || strstr(str, "的文章 □"))
                        return 1;
        return (*str == '\n');
}
/* this is a 陷阱 for bad people to cover my program to his */

int Origin2(char text[256])
{
        char    tmp[STRLEN];
        sprintf(tmp, ":·%s %s·[FROM:", BoardName, BBSHOST);
        if (strstr(text, tmp))
                return 1;
        else
                return 0;
}

/* When there is an old article that can be included -jjyang */
void do_quote(char *filepath, char quote_mode)
{
        FILE   *inf, *outf;
        char   *qfile, *quser;
        char    buf[256], *ptr;
        char    op;
        int     bflag, i;

        qfile = quote_file;
        quser = quote_user;
        bflag = strncmp(qfile, "mail", 4);
        outf = fopen(filepath, "w");
        if (quote_mode!='\0'&&*qfile!='\0'&&(inf=fopen(qfile, "r"))!=NULL) {
                op = quote_mode;
                if (op != 'N') {
                        fgets(buf, 256, inf);
                        if ((ptr = strrchr(buf, ')')) != NULL) {
                                ptr[1] = '\0';
                                if ((ptr = strchr(buf, ':')) != NULL) {
                                        quser = ptr + 1;
                                        while (*quser == ' ')
                                                quser++;
                                }
                        }
                        if (bflag)
                                fprintf(outf, "\n【 在 %-.55s 的大作中提到: 】\n", quser);
                        else
                                fprintf(outf, "\n【 在 %-.55s 的来信中提到: 】\n", quser);
                        if (op == 'A') {
                                while (fgets(buf, 256, inf) != NULL) {
                                        fprintf(outf, ": %s", buf);
                                }
                        } else if (op == 'R') {
                                while (fgets(buf, 256, inf) != NULL) {
                                        if (buf[0] == '\n')
                                                break;
                                }
                                while (fgets(buf, 256, inf) != NULL) {
                                        if (Origin2(buf))
                                                continue;
                                        fprintf(outf, "%s", buf);
                                }
                        } else {
                                while (fgets(buf, 256, inf) != NULL) {
                                        if (buf[0] == '\n')
                                                break;
                                }
                                i = 0;
                                while (fgets(buf, 256, inf) != NULL) {
                                        if (strcmp(buf, "--\n") == 0)
                                                break;
                                        if (buf[250] != '\0')
                                                strcpy(buf + 250, "\n");
                                        if (!garbage_line(buf)) {
                                                if(op == 'S' && i >= 10 ) {
                                                        fprintf(outf,": .................（以下省略）");
                                                        break;
                                                }
                                                i ++;
                                                fprintf(outf, ": %s", buf);
                                        }
                                }
                        }
                }
                fprintf(outf, "\n");
                fclose(inf);
        }
        *quote_file = '\0';
        *quote_user = '\0';
        if (!(currentuser.signature == 0 || header.chk_anony == 1)) {
                addsignature(outf, 1);
        }
        fclose(outf);
}
/* Add by SmallPig */
void getcross(char *filepath, int mode)
{
        FILE	*inf, *of;
        char    buf[256];
	char	itemp[STRLEN];
      /*  char    owner[248];
        int     count, owner_found = 0;*/
        time_t  now;

        now = time(0);
        inf = fopen(quote_file, "r");
        of = fopen(filepath, "w");
	/* test if announce mail*/ 
	if(inf==NULL&&strstr(quote_file,"mail")!=NULL)
        {
                sprintf(itemp,"%s/%s",ANNOUNCE_MAIL_DIR,genbuf);
                strcpy(genbuf,quote_file);
                strncpy(quote_file, itemp, sizeof(quote_file));
                quote_file[sizeof(quote_file) - 1] = '\0';
                inf=fopen(quote_file,"r");
        }
	strcpy(genbuf,quote_file);

        if (inf == NULL || of == NULL) {
                if( inf ) /* add fclose here and below by yiyo */
                {
                        fclose(inf);
                }
                if( of ) {
                        fclose(of);
                }           //add end
                report("Cross Post error");
                return;
        }
        if (mode == 0) {
                if (in_mail == YEA) {
                        in_mail = NA;
                        write_header(of, 1 /* 不写入 .posts */ );
                        in_mail = YEA;
                } else
                        write_header(of, 1 /* 不写入 .posts */ );
              /*  if (fgets(buf, 256, inf) != NULL) {
                        if(strncmp(&buf[2],"信人: ",6)) {
                                owner_found = 0;
                                strcpy(owner, "Unkown User");
                        } else {
                                for(count=8;buf[count]!=' '&&buf[count]!='\n'&&buf[count]!='\0';count++)
                                        owner[count - 8] = buf[count];
                                owner[count - 8] = '\0';
                                owner_found = 1;
                        }
                }*/
                if (in_mail == YEA)
                        fprintf(of, "[1;37m【 以下文字转载自 [32m%s [37m的信箱 】\n",
                                currentuser.userid);
                else
                        fprintf(of, "[1;37m【 以下文字转载自 [32m%s [37m讨论区 】\n",
                                quote_board);
             /*   if(owner_found) {  
                        if(fgets(buf, 256, inf) != NULL)
                                if (buf[0] != '\n')
                                        fgets(buf, 256, inf);
                        fprintf(of, "【 原文由[32m %s[37m 所发表 】[m\n", owner);
                } else
                        fseek( inf, (long) 0, SEEK_SET);*/
        } else if (mode == 1) {
                fprintf(of,"[1;41;33m发信人: %s (自动发信系统), 信区: %s%*s[m\n",
                        BBSID,quote_board,48-strlen(BBSID)-strlen(quote_board)," ");
                /* add for 'FILTER' */
                if(!(getbcache(currboard)->flag2&NOFILTER_FLAG))
		//loveni
		if(bad_words(quote_title)) {
                        badwords=YEA;
                }
                /* add end */
                fprintf(of, "标  题: %s\n", quote_title);
                getdatestring(now,NA);
                fprintf(of, "发信站: %s[33m自动发信系统[m(%s)\n\n",
                        BoardName, datestring);
        } else if (mode == 2) {
                write_header(of, 0 /* 写入 .posts */ );
        }
        while (fgets(buf, 256, inf) != NULL) {
                /* add for 'FILTER' */
                if(!(getbcache(currboard)->flag2&NOFILTER_FLAG))//loveni
		if(bad_words(buf)) {
                        badwords=YEA;
                }
                /* add end */
                if ((strstr(buf,"【 以下文字转载自 ")&&strstr(buf, "讨论区 】"))
                                || (strstr(buf, "【 原文由") && strstr(buf, "所发表 】")))
                        continue;
                else {
                        fprintf(of, "%s", buf);
                }
        }
        fclose(inf);
        fclose(of);
        *quote_file = '\0';
}


int do_post()
{
        *quote_file = '\0';
        *quote_user = '\0';
        local_article = YEA;
        return post_article(currboard,(char *)NULL);
}
int do_nhpost()
{
        if (strcmp(currboard,"ASCIIArt"))
                return -1;
        *quote_file = '\0';
        *quote_user = '\0';
        local_article = YEA;
        return post_article(currboard,"bbs");
} //add by yiyo ASCIIArt无文章头发文

/*ARGSUSED*/
int post_reply(int ent, struct fileheader* fileinfo,char* direct)
{
        char    uid[STRLEN];
        char    title[STRLEN];
        char   *t;
        FILE   *fp;
        if (!strcmp(currentuser.userid, "guest"))
                return DONOTHING;
	if (!HAS_PERM(PERM_POST))
        {
                return DONOTHING;
        }

        clear();
        if (check_maxmail()) {
                pressreturn();
                return FULLUPDATE;
        }
        modify_user_mode(SMAIL);

        /* indicate the quote file/user */
        setbfile(quote_file, currboard, fileinfo->filename);
        strcpy(quote_user, fileinfo->owner);

        /* find the author */
        if (!getuser(quote_user)) {
                genbuf[0] = '\0';
                fp = fopen(quote_file, "r");
                if (fp != NULL) {
                        fgets(genbuf, 255, fp);
                        fclose(fp);
                }
                t = strtok(genbuf, ":");
                if (strncmp(t, "发信人", 6) == 0 ||
                                strncmp(t, "寄信人", 6) == 0 ||
                                strncmp(t, "Posted By", 9) == 0 ||
                                strncmp(t, "作  者", 6) == 0) {
                        while (t != NULL) {
                                t = (char *) strtok(NULL, " \r\t\n<>");
                                if (t == NULL)
                                        break;
                                if (!invalidaddr(t))
                                        break;
                        }
                        if (t != NULL)
                                strncpy(uid, t, STRLEN);
                } else {
                        prints("对不起，该帐号已经不存在。\n");
                        pressreturn();
                }
        } else
                strcpy(uid, quote_user);
        /* make the title */
        if (toupper(fileinfo->title[0]) != 'R' || fileinfo->title[1] != 'e' ||
                        fileinfo->title[2] != ':')
                strcpy(title, "Re: ");
        else
                title[0] = '\0';
        strncat(title, fileinfo->title, STRLEN - 5);

        /* edit, then send the mail */
        switch (do_send(uid, title)) {
        case -1:
                prints("系统无法送信\n");
                break;
        case -2:
                prints("送信动作已经中止\n");
                break;
        case -3:
                prints("使用者 '%s' 无法收信\n", uid);
                break;
        case -4:
                prints("使用者 '%s' 无法收信，信箱已满\n",uid);
                break;
        default:
                prints("信件已成功地寄给原作者 %s\n", uid);
        }
        pressreturn();
        in_mail = NA;
        return FULLUPDATE;
}
/* Add by SmallPig */
int post_cross(int islocal, int mode)
{
        struct fileheader postfile;
        struct shortfile *bp;
        char    filepath[STRLEN], fname[STRLEN];
        char    buf[256], buf4[STRLEN], whopost[IDLEN + 2];
        int     fp, count;
        time_t now;
        if (!haspostperm(currboard) && !mode) {
                prints("\n\n 目前您尚无权限在 %s 板发表文章.\n", currboard);
                return -1;
        }

        /* efan: 检查是否团体板面	*/
#ifdef BOARD_CONTROL
        // if(!HAS_PERM(PERM_SYSOP)&&!chk_currBM(currBM)){
        if( (!HAS_PERM(PERM_SYSOP) || deny_s_board(currboard)) && !mode ) {
                setbfile(buf, currboard, "board.allow");
                if(dashf(buf)&&!seek_in_file(buf,currentuser.userid)) {
                        clear();
                        move(5,10);
                        prints("很抱歉, 本板面属于团体板面，而您不是该板成员");
                        return -1;
                }
        }

#endif

        /* efan: 检查是否有进入本板的权力	*/
	if( !HAS_PERM(PERM_SYSOP) && !mode) {
                setbfile(buf, currboard, "board.write");
                if(dashf(buf)&&!seek_in_file(buf,currentuser.userid)) {
                        clear();
                        move(5,10);
                        prints("很抱歉, 本板面属于俱乐部板面，而您不是该板成员");
                        return -1;
                }
        }
#ifdef BOARD_READDENY
        if(!HAS_PERM(PERM_SYSOP) && !mode) {
                setbfile(buf, currboard, "board.deny");
                if(dashf(buf)&&seek_in_file(buf,currentuser.userid)) {
                        clear();
                        move(5,20);
                        prints("对不起，您目前被停止进入本板的权利。");
                        return -1;
                }
        }
#endif
        /*	if (!strcmp(currboard, "Punishment")&&!HAS_PERM(PERM_OBOARDS)&&!HAS_PERM(PERM_SYSOP))
        	{
        		prints("\n\n 本板为系统记录，不可发表文章.\n", currboard);
        		return FULLUPDATE;
        	}
        */

        /* Efan: 如果 mode == 1，则表示该文为BBS系统自动发出	*/
        if ( mode !=1&&uinfo.mode != RMAIL)
			if(rdcheck())
  		       return -2;
		                      
        memset(&postfile, 0, sizeof(postfile));
        strncpy(save_filename, fname, 4096);

        now = time(0);
        sprintf(fname, "M.%d.A", now);
        if (!mode) {
                if (!strstr(quote_title, "(转载)"))
                        sprintf(buf4, "%.70s(转载)", quote_title);
                else
                        strcpy(buf4, quote_title);
        } else
                strcpy(buf4, quote_title);
        strncpy(save_title, buf4, STRLEN);
        setbfile(filepath, currboard, fname);
        count = 0;

        while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1) {
                now++;
                sprintf(fname, "M.%d.A", now);
                setbfile(filepath, currboard, fname);
                if (count++ > MAX_POSTRETRY) {
                        return -1;
                }
        }
        close(fp);
        strcpy(postfile.filename, fname);
        if (mode == 1)
                strcpy(whopost, BBSID);
        else
                strcpy(whopost, currentuser.userid);

        strncpy(postfile.owner, whopost, STRLEN);
        setbfile(filepath, currboard, postfile.filename);

        bp = getbcache(currboard);
        local_article = YEA;
        if ((islocal == 'S' || islocal == 's') && (bp->flag & OUT_FLAG))
                local_article = NA;

        modify_user_mode(POSTING);
        badwords=NA;
        getcross(filepath, mode);
        if(badwords) {
                move (3, 0);
                clrtobot ();
                move (5, 0);
                prints ("        对不起，该文包含敏感字，请注意用语");
                return FULLUPDATE;
        }
        strncpy(postfile.title, save_title, STRLEN);
        if (local_article == YEA || !(bp->flag & OUT_FLAG)) {
                postfile.filename[STRLEN - 1] = 'L';
                postfile.filename[STRLEN - 2] = 'L';
        } else {
                postfile.filename[STRLEN - 1] = 'S';
                postfile.filename[STRLEN - 2] = 'S';
                outgo_post(&postfile, currboard);
        }
        setbdir(buf, currboard);
        if (append_record(buf, &postfile, sizeof(postfile)) == -1) {
                if (!mode) {
                        sprintf(buf, "cross_posting '%s' on '%s': append_record failed!",
                                postfile.title, quote_board);
                } else {
                        sprintf(buf, "Posting '%s' on '%s': append_record failed!",
                                postfile.title, quote_board);
                }
                report(buf);
                pressreturn();
                clear();
                return 1;
        }
        /* brc_addlist( postfile.filename ) ; */
        if (!mode) {
                add_crossinfo(filepath, 1);
                sprintf(buf, "cross_posted '%s' on '%s'", postfile.title, currboard);
                report(buf);
        }
        /*    else
                sprintf(buf,"自动发表系统 POST '%s' on '%s'", postfile.title, currboard) ;
            report(buf) ;*/
        return 1;
}

void add_crossinfo(char* filepath, int mode)
{
        FILE   *fp;
        int     color;
        char	ipshow[60];//add by yiyo
        color = (currentuser.numlogins % 7) + 31;
        if ((fp = fopen(filepath, "a")) == NULL)
                return;
        //******************  modified by silencer  ip隐藏  02。5。31 ******************
        /* Efan: 用宏来控制是否隐藏IP	*/
#ifdef DEF_HIDE_IP_ENABLE

        if(isdormip(fromhost))
                strcpy(ipshow,"南开宿舍内网");
        else
                strcpy(ipshow,fromhost);
#else

        strcpy (ipshow, fromhost);
#endif

        fprintf(fp, "--\n[m[1;%2dm※ 转%s:·%s %s·[FROM: %-.20s][m\n"
                ,color, (mode == 1) ? "载" : "寄", BoardName, BBSHOST, ipshow);
        /*
        	fprintf(fp, "--\n[m[1;%2dm※ 转%s:·%s %s·[FROM:     *     ][m\n"
        		,color, (mode == 1) ? "载" : "寄", BoardName, BBSHOST);
        */
        //*****************************************************************************
        fclose(fp);
        return;
}

int show_board_notes(char bname[30], int command)
{
        char    buf[256];
        sprintf(buf, "vote/%s/notes", bname);
        if (dashf(buf)) {
                if (command == 1) {
                        ansimore2(buf, NA, 0, 19);
                } else {
                        ansimore(buf);
                }
                return 1;
        } else if (dashf("vote/notes")) {
                if (command == 1) {
                        ansimore2("vote/notes", NA, 0, 19);
                } else {
                        ansimore("vote/notes", NA, 0, 19);
                }
                return 1;
        }
        return -1;
}

int show_user_notes()
{
        char    buf[256];
        setuserfile(buf, "notes");
        if (dashf(buf)) {
                ansimore(buf);
                return FULLUPDATE;
        }
        clear();
        move(10, 15);
        prints("您尚未在 InfoEdit->WriteFile 编辑个人备忘录。\n");
        pressanykey();
        return FULLUPDATE;
}


int outgo_post(struct fileheader* fh, char* board)
{
        char    buf[256];
        sprintf(buf, "%s\t%s\t%s\t%s\t%s\n", board,
                fh->filename, header.chk_anony ? board : currentuser.userid,
                header.chk_anony ? "我是匿名天使" : currentuser.username, save_title);
        file_append("innd/out.bntp", buf);

}

/*
  check the readonly setting
  return 1 indicating that it's time to read only
*/
int rdcheck(){
/*
        if ( //只读后系统维护板面可以发文.23.5.26
                (!(haspostperm(currboard))||strcmp(currboard, "Maintenance"))
                && !HAS_PERM(PERM_OBOARDS)&&dashf("NOPOST")&&!(HAS_PERM(PERM_CHATCLOAK)&&( !strcmp(currboard, "PreparatoryWork")
                                // Efan: 使站务组在只读时能在Headquarters版发文	--2003.4.25 
                                || ! strcmp (currboard, "Headquarters") || ! strcmp (currboard, "notepad")) )
                && insec (0,currboard) 
                && insec (6,currboard)
                && insec (2,currboard)
                && insec (13,currboard) 
                && insec (3,currboard)
                && insec (4,currboard)
                && insec (5,currboard)
                && insec (8,currboard)
                //&& strcmp (currboard, "newcomers")
                //&& strcmp (currboard, "syssecurity")
		        //&& strcmp (currboard, "syssecurity2")
                //&& strcmp (currboard, "BMTest")
                && strcmp (currboard, "KingKiller")
                //&& strcmp (currboard, "Editorial_Dep")
                //&& strcmp (currboard, "BoardMaster")
                //&& strcmp (currboard, "Advice")
                //&& strcmp (currboard, "sysop")
                //&& strcmp (currboard, "Advisors")
                //&& strcmp (currboard, "Editorial_Dep")
                //&& strcmp (currboard, "notepad")
                //&& strcmp (currboard, "Arbitration")
		        && strcmp (currboard, "SeventhZone")
                //&& (strcmp (currboard, "Intercommunion") || !HAS_PERM(PERM_MAILINFINITY))
        ) 
	    
*/        	
		if(!(HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_BOARDS))&&!IsInternalUser(fromhost)) goto out3;
		if(!HAS_PERM(PERM_CHATCLOAK)&&dashf("NOPOST")) goto out2;/*全站只读*/

		if(!HAS_PERM(PERM_OBOARDS)&&dashf("NOPOST.1")&&!insec(1,currboard)) 
	    goto out;/*1区只读*/

		
        /* efan: very faint	*/
        if (dashf ("NOPOST.2") &&
                        !HAS_PERM (PERM_OBOARDS) && !HAS_PERM (PERM_SYSOP) && (
                                !strcmp (currboard, "VoiceofNankai") ||
                                !strcmp (currboard, "TaiWan") ||
                                !strcmp (currboard, "Salon") ||
                                !strcmp (currboard, "Military"))
        )
	      goto out;			 
		return 0;
   out: 
        if(chk_currBM (currBM)) return 0;
        move(3, 0);
        clrtobot();
        move(5,0);
        prints("          真对不起，从2003.2.27日，本站施行夜间只读制度.您先四处看看吧.\n");
        pressreturn();
        return 1;
    out2: /*loveni:全站只读*/
        move(3, 0);
        clrtobot();
        move(5,0);
        prints("          真对不起，目前全站只读.您先四处看看吧.\n");
        pressreturn();
        return 1;
	out3:
		move(3, 0);
        clrtobot();
        move(5,0);
        prints("          真对不起，目前外网只读.您先四处看看吧.\n");
        pressreturn();
        return 1;

		
}


int post_article(char* post_board,char* mailid)
{
        struct fileheader postfile;
        struct shortfile *bp;
        char    filepath[STRLEN], fname[STRLEN], buf[STRLEN], filepath2[STRLEN], postboard[STRLEN];
        char	buf2[256];
        int     fp, aborted, count;
        time_t  now = time(0);
        static  time_t  lastposttime = 0;
        static  int  failure = 0;
		FILE *vbfp;
        badwords=0; //add for 'FILTER'
        modify_user_mode(POSTING);
		
		setbfile(buf, post_board, ".vbconfig");
		if(!dashf(buf))
			strcpy(postboard, post_board);
		else{
			if (strchr(quote_file, '/'))
			{
				strncpy(postboard, quote_file + 7, sizeof(postboard));
				char * p = strchr(postboard, '/');
				if (p)
				{
					*p = 0;
				}
			}
			else
			{
				vbfp = fopen(buf, "r");
				memset(buf, 0, STRLEN);
				fgets(buf, STRLEN, vbfp);
				fclose(vbfp);
				strcpy(postboard, ltrim(rtrim(buf)));
			}
		}

        //added by soff 防止灌水机 2000.10.08
        if((abs(now-lastposttime)<3 || failure >= 9999)&&
                        (!HAS_PERM(PERM_SYSOP))&&strcmp(postboard, "Water")) {
                clear();
                move(5,10);
                failure++;
                if(failure > 9999) {
                        if( failure >= 10020 )
                                abort_bbs();
                        prints("对不起，您在被劝阻多次的情况下，仍不断试图发文。");
                        move(6,10);
                        prints("您目前被系统认定为灌水机，请退出后重新登陆！[%d/20]",
                               failure - 9999);
                } else {
                        prints("您太辛苦了，先喝杯咖啡歇会儿，3 秒钟后再发表文章。\n");
                        if( failure > 5 ) {
                                move(6,10);
                                prints("您在被劝阻的情况下仍旧试图发表文章。[%d/20]",
                                       failure-5);
                                if (failure >= 25) {
                                        securityreport("多次试图发表文章，被系统判定为灌水机");
                                        failure = 9999;
                                }
                        }
                        lastposttime = now;
                }
                pressreturn();
                clear();
                return FULLUPDATE;
        }

        if(check_post_limit()) return FULLUPDATE;

        //add by tdhlshx 2003.5.30. for 自动解封
        if(!haspostperm(postboard)&&(strcmp(postboard,"deleted"))&&(strcmp(postboard,"Announce"))&&(strcmp(postboard,"Punishment"))&&(strcmp(postboard,"bbslists"))&&(strcmp(postboard,"notepad"))&&(strcmp(postboard,"Reference"))&&(strcmp(postboard,"cnAnnounce"))&&(strcmp(postboard,"cnAdmin"))&&(strcmp(postboard,"vote"))&&(strcmp(postboard,"Amusement"))) {
                int deny;
                char buf[STRLEN];
                setbfile(buf,postboard,"deny_users");//ok
                deny=seek_in_denyfile(buf,currentuser.userid);
                if(deny==0) {
                        deldeny(currentuser.userid,1);
                        move(3,0);
                        clrtobot();
                        move(5,3);
                        prints("解封时间到...");
                        pressanykey();
                        char msgbuf[STRLEN];
                        char repbuf[STRLEN];
                        sprintf(repbuf,"恢复%s在%s板的发文权限",currentuser.userid,currboard);
                        sprintf(msgbuf,"\n  %s 网友：\n\n"
                                "\t因封禁时间已过，现恢复您在 [%s] 板的发文权限.\n\n"
                                "\t希望您今后能自觉遵守站规,%s感谢您的支持与合作.\n\n",
                                currentuser.userid,currboard,BBSNAME);
                        autoreport(repbuf,msgbuf,YEA,currentuser.userid, BBSNAME);
                        securityreport4(repbuf);
                }
        }//add end
//外网用户屏蔽. by brew 05.7.5
	if(!isdormip(fromhost) &&	(!isAllowedIp(fromhost)||!haspostperm(postboard)&&!HAS_PERM(PERM_BLEVELS) &&(!strcmp(postboard,"deleted"))) && !HAS_PERM (PERM_BOARDS) && (strcmp(postboard,"Account")))
		{
		move(3, 0);
		clear();
		move(5, 0);
		prints("          对不起，您暂时无法发文，请先进行邮箱绑定。\n          如果您需要获得发文权限，请点击以下网址：\n             http://bbs.nankai.edu.cn/auth/");
		pressreturn();
		clear();
		return FULLUPDATE;
	}
	if(!HAS_PERM(PERM_SYSOP) )
	 {
              	char boardwrite[STRLEN];
		   setbfile(boardwrite, currboard, "board.write");
                if(dashf(boardwrite)&&!seek_in_file(boardwrite,currentuser.userid))
		 {
                        clear();
                        move(5,0);
                        prints("          很抱歉，本板面属于俱乐部板面，而您不是该板会员\n          如果你想在本板发表文章，请与板务联系，申请成为会员");
                        pressanykey();
                        clear();
		return FULLUPDATE;
                }
	}  /* loveni:俱乐部版面,可读不可写*/
        /* modified by yiyo deleted版不可发文 */
        if (!haspostperm(postboard)||!HAS_PERM(PERM_BLEVELS)&&(!strcmp(postboard,"deleted"))) {//||!strcmp(postboard,"Punishment"))) {
                move(3, 0);
                clrtobot();
                move(5,0);
                if (digestmode == NA || !isdormip(fromhost) && !isAllowedIp(fromhost) && !HAS_PERM(PERM_BOARDS)) {
                        /* prints("          此讨论区目前是唯读的, 或是您尚无权限在此发表文章。");*/
                        prints("          此讨论区是唯读的, 或是您尚无权限在此发表文章.\n          如果您尚未注册，请在个人工具箱内详细注册身份 \n          未通过身份注册认证的用户，没有发表文章的权限。\n                  谢谢合作！ :-) ");
                } else {
                        prints("目前是文摘或主题模式, 不能发表文章 (按 ENTER 后再按左键可离开此模式)。");
                }
                pressreturn();
                clear();
                return FULLUPDATE;
        }
        
		if(rdcheck())
  		       return FULLUPDATE;

		memset(&postfile, 0, sizeof(postfile));
        clear();
        show_board_notes(postboard, 1);
        bp = getbcache(postboard);
        if(bp->flag&OUT_FLAG && replytitle[0] == '\0')
                local_article = NA;
#ifndef NOREPLY

        if (replytitle[0] != '\0') {
                if (ci_strncmp(replytitle, "Re:", 3) == 0)
                        strcpy(header.title, replytitle);
                else
                        sprintf(header.title, "Re: %s", replytitle);
                header.reply_mode = 1;
        } else
#endif
        {
                header.title[0] = '\0';
                header.reply_mode = 0;

        }
        strcpy(header.ds, postboard);
        header.postboard = YEA;
        if (post_header(&header) == YEA) {
                strcpy(postfile.title, header.title);
                strncpy(save_title, postfile.title, STRLEN);
                strncpy(save_filename, fname, 4096);
        } else
                return FULLUPDATE;
        now = time(0);
        lastposttime = now;//added by soff 防止灌水机 2000.10.08
        failure = 0;
        sprintf(fname, "M.%d.A", now);
        setbfile(filepath, postboard, fname);
        count = 0;
        while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1) {
                now++;
                sprintf(fname, "M.%d.A", now);
                setbfile(filepath, postboard, fname);
                if (count++ > MAX_POSTRETRY) {
                        return -1;
                }
        }
        fchmod(fp, 0644);	/* youzi 1999.1.8 */
        close(fp);
        strcpy(postfile.filename, fname);
        in_mail = NA;
        strncpy(postfile.owner, (header.chk_anony) ?
                postboard : currentuser.userid, STRLEN);
        setbfile(filepath, postboard, postfile.filename);

        modify_user_mode(POSTING);
        do_quote(filepath, header.include_mode);
        if(mailid==NULL) {
        //pattern=0;
                if(pattern) {
                        clear(); 
                        setbfile(genbuf,currboard,"pattern");
                        int n=get_num_records(genbuf,sizeof(struct pattern));
                        if (n==0){
					move(10,3330);
					prints("本板尚没有任何发文模板存在!");
			                pressreturn();
					aborted=-1;
					goto E;
                        }
                        move(0,0);
                        prints("[1;33m%s[1;37m模板列表,请选择发文所用模板\n\n",currboard);
			            showpattern(genbuf);
                        int sel=0;
                  Q:
                        getdata(t_lines-2,0, "你要使用那一个模板?( Enter 取消):", genbuf, 3, DOECHO, YEA);
                        sel=atoi(genbuf);
                        if(sel==0) {
                                aborted=-1;
                                goto E;
                        }
                        if(sel<0||sel>n)
                                goto  Q;
                        aborted=patternpost(filepath,save_title,sel);
                } else
                        aborted = vedit(filepath, YEA, YEA);
        } else if(!strcmp("bbs",mailid))
                aborted = vedit(filepath, NA, YEA);
        else
                aborted = vedit(filepath, YEA, YEA);
        //add by yiyo ASCIIArt无文章头发文
        /* add for 'FILTER' */

E:
        pattern=0;
        if(badwords) {
                strcpy(filepath2, filepath);
                setbfile(filepath, "TEMP", fname);
                sprintf(buf2, "/bin/mv %s/%s %s/%s", BBSHOME, filepath2, BBSHOME, filepath);
                system(buf2);
        } else
                setbfile(filepath, postboard, fname);
        /* modified end */
        strncpy(postfile.title, save_title, STRLEN);
        if ((local_article == YEA) || !(bp->flag & OUT_FLAG)) {
                postfile.filename[STRLEN - 1] = 'L';
                postfile.filename[STRLEN - 2] = 'L';
        } else {
                postfile.filename[STRLEN - 1] = 'S';
                postfile.filename[STRLEN - 2] = 'S';
                if(!badwords)
                        outgo_post(&postfile, postboard);
                //modified for 'FILTER'
        }
        /* Anony=0; *//* Inital For ShowOut Signature */
        if (aborted == -1) {
                unlink(filepath);
                clear();
                return FULLUPDATE;
        }
        /* modified for 'FILTER' */
        if(badwords)
                setbdir(buf, "TEMP");
        else
                setbdir(buf, postboard);
        /* modified end */
        if ( noreply ) {
                postfile.accessed[0] |= FILE_NOREPLY;
                noreply = 0;
        }
#ifdef MARK_X_FLAG
        if( markXflag ) {
                postfile.accessed[0] |= FILE_DELETED;
                markXflag = 0;
        } else
                postfile.accessed[0] &= ~FILE_DELETED;
#endif

        if( mailtoauther ) {
                if(header.chk_anony)
                        prints("对不起，您不能在匿名板使用寄信给原作者功能。");
                else if(!mail_file(filepath,mailid,postfile.title))
                        prints("信件已成功地寄给原作者 %s",mailid);
                else
                        prints("信件邮寄失败，%s 无法收信。",mailid);
                mailtoauther = 0;
                pressanykey();
        }
        if (append_record(buf, &postfile, sizeof(postfile)) == -1) {
                sprintf(buf, "posting '%s' on '%s': append_record failed!",
                        postfile.title, currboard);
                report(buf);
                pressreturn();
                clear();
                return FULLUPDATE;
        }
        brc_addlist(postfile.filename);

        sprintf(buf, "posted '%s' %s on '%s'", postfile.title, badwords?"WithBadWords":"", currboard);
        report(buf);
        if(badwords) {
                clear ();
                move(8,5);
                prints("您的文章可能包含不便显示的内容，需经站务审核后才会在板面上发表，请耐心等待");
                pressanykey();
        }

        badwords=NA; //add for 'FILTER'
        if (!junkboard()) {
                set_safe_record();
                currentuser.numposts++;
                substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
        }
        return FULLUPDATE;
}


int patternpost(char *file,char *oldtitle,int num)
{
        struct pattern p;
		char fpath[256];
        setbfile(genbuf,currboard,"pattern");
        get_record(genbuf,&p,sizeof(p),num);
        setbfile(fpath,currboard,p.file);
	sprintf(genbuf,"%s.question",fpath);
        FILE *fq=fopen(genbuf,"r");
        sprintf(genbuf,"%s.bin",fpath);
 	FILE *fbin=fopen(genbuf,"r");
        if(fq==0||fbin==0){
		      prints("该模板目前不可用.");
			  egetch();
			  return -1;
	}
	FILE *out=fopen(file,"w+");
        if(out==0){
             prints("不能建立文件,系统资源不足!");  
             egetch(); 
             return -1;
        }
         
	struct question q;
        if(fread(&q,sizeof(struct question),1,fq)<=0){
                 prints("模板读取错误.");
                 egetch();
                 return -1;
        }
        char *ptr[MQ+1];
	ptr[0]=oldtitle; 
        sprintf(genbuf,"%s%s%s",q.title1,oldtitle,q.title2);
        strcpy(oldtitle,genbuf);
        write_header(out,0);

	int size=0,i;
        char c;
        for(i=1;i<=p.question ;i++ ){
			clear();
			move(0,0);
            prints("讨论区[%s] 模板发文\n使用标题:%s    问题总数:%d\n\n",currboard,oldtitle,p.question);
            prints("  ");
	    prints("\n\n%2d.%s\n\n",i,q.q[i-1].con);
   	    size=q.q[i-1].size;
           
	    prints("回答 [限%d字]:\n",size);
   	    ptr[i]=(char *)malloc(size);
	    bzero(ptr[i],size);
            if(size<80)   
                getdata(10,0,0,(char*) ptr[i],size,DOECHO, NA);                       
	    else{
                prints("\n 按 J 跳过该问,其他键做答...");
                c=0;
                c=egetch();
		if(c=='J'||c=='j'){
			  strcpy(ptr[i]," [对于这个问题我保持沉默] ");
                          continue;
                }			
		sprintf(genbuf,"tmp/%s.%d.pattern",currentuser.userid,getpid());
	        int aborted = vedit(genbuf, NA, YEA);
		if(aborted==-1){
                     prints("该问跳过....");
	  	     strcpy(ptr[i]," [对于这个问题我保持沉默] ");
		     pressanykey();
		}else{
 		     FILE *tmp=fopen(genbuf,"r");
		     fread(ptr[i],size,1,tmp);
		     fclose(tmp);
               
		}
                unlink(genbuf);
 	    }
        }

         move(t_lines-3,0);
         prints("使用标题:%s\n",oldtitle);
	 if (askyn("确认发表吗?", YEA,YEA) == NA) {
 		 for(i=1;i<=p.question;i++)
	        	free(ptr[i]);
                 fcloseall();
                 return -1;
	 }


        i=0;
	char tmpb[AS];
        while(1){
		  bzero(tmpb,AS);
		  if(!readbin(tmpb,&i,fbin)) break;
		  if(i==8888) fprintf(out,"%s",tmpb);
                  else   fprintf(out,"%s%s",tmpb,ptr[i]);
	}
	for(i=1;i<=p.question;i++)
		free(ptr[i]);

        addsignature(out, 1);
        int   color = (currentuser.numlogins % 7) + 31;

#ifdef DEF_HIDE_IP_ENABLE

        if(isdormip(fromhost))
                strcpy(genbuf,"南开宿舍内网");
        else
                strcpy(genbuf,fromhost);
#else

        strcpy (genbuf, fromhost);
#endif

        fprintf(out, "[m[1;%2dm※ 来源:·%s %s·[FROM: %-.20s][m ",
                color, BoardName, BBSHOST, genbuf);

        fcloseall();
        return 0;

}

int IsTheFileOwner(struct fileheader *fileinfo)
{
        char    buf[512];
        int	posttime;

        if(fileinfo->owner[0] == '-'||strstr(fileinfo->owner, "."))
                return 0;
        if(strcmp(currentuser.userid, fileinfo->owner))
                return 0;
        strcpy(buf,&(fileinfo->filename[2]));
        buf[strlen(buf)-2] = '\0';
        posttime = atoi(buf);
        if( posttime < currentuser.firstlogin )
                return 0;
        return 1;
}

int change_title(char *fname, char *title)
{
        FILE   *fp, *out;
        char    buf[256], outname[STRLEN];
        int newtitle = 0 ;

        if ((fp = fopen(fname, "r")) == NULL)
                return 0;

        sprintf(outname, "tmp/editpost.%s.%05d", currentuser.userid, uinfo.pid);
        //   if ((out = fopen(outname, "w")) == NULL) return 0;
        if ((out = fopen(outname, "w")) == NULL) {
                fclose(fp);
                return 0;
        }   //modified by yiyo

        while ((fgets(buf, 256, fp)) != NULL) {
                if (!strncmp(buf, "标  题: ", 8) && newtitle ==0) {
                        fprintf(out, "标  题: %s\n", title);
                        newtitle = 1;
                        continue;
                }
                fputs(buf, out);
        }
        fclose(fp);
        fclose(out);
        rename(outname, fname);
        chmod(fname, 0644);
        return;
}

int edit_post(int ent, struct fileheader *fileinfo, char *direct)
{
        char    buf[512];
        char   *t;
        extern char currmaildir[STRLEN];
        badwords=NA; //add for 'FILTER'

        if(  !strncmp (currboard, "syssecurity", 11) )
        {
                clear();
                move(5,6);
                prints("              系统安全重地,你不能更改文章内容\n");
                pressreturn();
                return DIRCHANGED;
        }

        if (!in_mail)
        {
                if (!chk_currBM(currBM)) {
                        struct shortfile *bp;
                        //if (strcmp(fileinfo->owner, currentuser.userid)) return DONOTHING;
                        if(!IsTheFileOwner(fileinfo))
                                return DONOTHING;
                        bp = getbcache(currboard);
                        if ((bp->flag & ANONY_FLAG) && !strcmp(fileinfo->owner, currboard))
                                return DONOTHING;
                }
        }
        modify_user_mode(EDIT);
        clear();
        if (in_mail)
                strcpy(buf, currmaildir);
        else
                strcpy(buf, direct);
        if ((t = strrchr(buf, '/')) != NULL)
                *t = '\0';
        sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
        if (vedit(genbuf, NA, NA) == -1)
                return FULLUPDATE;
        if(badwords)
                return FULLUPDATE; //add for 'FILTER'
#ifdef MARK_X_FLAG

        if( markXflag )
        {
                fileinfo->accessed[0] |= FILE_DELETED;
                markXflag = 0;
        } else
                fileinfo->accessed[0] &= ~FILE_DELETED;
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
#endif

        if (!in_mail)
        {
                sprintf(genbuf, "edited post '%s' on %s", fileinfo->title, currboard);
                report(genbuf);
        }
        return FULLUPDATE;
}

int getnam(char* direct, int num, char* id)
{
        FILE *fp;
        int size;
        struct fileheader ff;
        size = sizeof(struct fileheader);
        strcpy(id,"none."); /* move this line up by yiyo */
        fp=fopen(direct,"r");
        if( !fp ) {
                return;
        }
        //  fp=fopen(direct,"r");
        fseek(fp,(num-1)*size,SEEK_SET);
        //  strcpy(id,"none.");
        if (fread(&ff,size,1,fp))
                strcpy(id,ff.filename);
        fclose(fp);
}


/* add by bluetent for counting the visits of the articles */
int getfile(char* direct, int num, char* id, char *accessed, char *title)
{
        FILE *fp;
        int size;
        struct fileheader ff;
        size = sizeof(struct fileheader);
        strcpy(id,"none.");
        strcpy(title,"none.");
        accessed[0]=0;
        fp=fopen(direct,"r");
        if( !fp ) {
                return;
        }
        fseek(fp,(num-1)*size,SEEK_SET);
        if (fread(&ff,size,1,fp)) {
                strcpy(id,ff.filename);
                strcpy(accessed,ff.accessed);
                strcpy(title,ff.title);
                nums1=(int*) (ff.title+72);
        }
        fclose(fp);
}

int edit_title(int ent, struct fileheader* fileinfo, char* direct)
{
        char    buf[STRLEN];
	char	logbuf[STRLEN*2];
        char id1[20], id2[20]; //added by zhch, 12.19, to fix T d T bug.
        getnam(direct,ent,id1);

        if( !strncmp (currboard, "syssecurity", 11) ) {
                clear();
                move(5,6);
                prints("              系统安全重地,你不能更改文章标题\n");
                pressreturn();
                return DIRCHANGED;
        }

        if (!chk_currBM(currBM)) {
                struct shortfile *bp;
                //	if(strcmp(fileinfo->owner,currentuser.userid))return DONOTHING;
                if(!IsTheFileOwner(fileinfo))
                        return DONOTHING;
                bp = getbcache(currboard);
                if ((bp->flag & ANONY_FLAG) && !strcmp(fileinfo->owner, currboard))
                        return DONOTHING;
        }
        strcpy(buf,fileinfo->title);
        getdata(t_lines - 1, 0, "新文章标题: ", buf, 50, DOECHO, NA);
        if(!strcmp(buf,fileinfo->title))
                return PARTUPDATE;
        if(!(getbcache(currboard)->flag2&NOFILTER_FLAG))//loveni
	if(bad_words(buf))
                return PARTUPDATE; //add for 'FILTER'
        check_title(buf);
        if (buf[0] != '\0') {
                if (fileinfo->accessed[1] & FILE_ONTOP) {
                        int fd;
                        int nfh;
                        int size = sizeof (struct fileheader);
                        struct fileheader fh;
                        strcpy (fileinfo->title, buf);
                        setbfile (genbuf, currboard, ".top");
                        fd = open (genbuf, O_RDWR);
                        if (fd == -1)
                                return PARTUPDATE;
                        while ((nfh = read (fd, &fh, size)) == size)
                                if (!strcmp (fileinfo->filename, fh.filename))
                                        break;
                        if (nfh == size) {
                                lseek (fd, -size, SEEK_CUR);
                                write (fd, fileinfo, size);
                        }
                        close (fd);
                        return PARTUPDATE;
                }
                char    tmp[STRLEN * 2], *t;
                getnam(direct, ent,id2);
                if(strcmp(id1,id2))
                        return PARTUPDATE;
		sprintf(logbuf,"EditTitle:%s -> %s", fileinfo->title, buf);
                strcpy(fileinfo->title, buf);
                strcpy(tmp, direct);
                if ((t = strrchr(tmp, '/')) != NULL)
                        *t = '\0';
                sprintf(genbuf, "%s/%s", tmp, fileinfo->filename);
                change_title(genbuf, buf);
                substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
		if(!IsTheFileOwner(fileinfo))
		    securityreport3(logbuf,-1);
		else
		    report(logbuf);
        }
        return PARTUPDATE;
}

int underline_post(int ent, struct fileheader* fileinfo, char* direct)
{
        if(!chk_currBM(currBM)&&!IsTheFileOwner(fileinfo)) {
                return DONOTHING;
        }
        if (fileinfo->accessed[0] & FILE_NOREPLY) {
                fileinfo->accessed[0] &= ~FILE_NOREPLY;
        } else {
                fileinfo->accessed[0] |= FILE_NOREPLY;
        }
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        return PARTUPDATE;
}

/*
int
makeDELETEDflag(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
        if (!(chk_currBM(currBM))||
		fileinfo->accessed[0] & (FILE_MARKED|FILE_DIGEST))
 	{
                return DONOTHING;
        }
        if (fileinfo->accessed[0] & FILE_DELETED)
        {
                fileinfo->accessed[0] &= ~FILE_DELETED;
        } else {
                fileinfo->accessed[0] |= FILE_DELETED;
        }
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        return PARTUPDATE;
}
*/ //modified by yiyo 取消'x'标记

int makeVISITflag(int ent, struct fileheader* fileinfo, char* direct)
{
        if (!(chk_currBM(currBM))) {
                return DONOTHING;
        }
        if (fileinfo->accessed[0] & FILE_VISIT) {
                fileinfo->accessed[0] &= ~FILE_VISIT;
        } else {
                fileinfo->accessed[0] |= FILE_VISIT;
        }
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        return PARTUPDATE;
}

int mark_post(int ent, struct fileheader* fileinfo, char* direct)
{
        /*
        	if (!chk_currBM(currBM)) {
        		return DONOTHING;
        	}
        */
        /* Efan: 站务组成员可在任何SYSOPs版使用mgb功能	*/
        if (!chk_currBM (currBM) && (strcmp (currBM, "SYSOPs") || ! HAS_PERM (PERM_ACHATROOM) ) ) {
                return DONOTHING;
        }

        if (fileinfo->accessed[1] & FILE_ONTOP)
                return DONOTHING;
        if (fileinfo->accessed[0] & FILE_MARKED)
                fileinfo->accessed[0] &= ~FILE_MARKED;
        else {
                fileinfo->accessed[0] |= FILE_MARKED;
                fileinfo->accessed[0] &= ~FILE_DELETED;
        }
        substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
        return PARTUPDATE;
}

int del_range(int ent, struct fileheader *fileinfo, char *direct)
{
        struct fileheader *x;
        char    num[8];
        int     inum1, inum2;
        FILE		*fp;
        char		buf[256];
        int		total=0;
        int		delg=0;
	int             modenow=0;
        /* 修复多人同时区段删除造成的错误 */
        int result;
        char fullpath[STRLEN];
        /* add by yiyo */
        if (uinfo.mode == READING)
        {
                if (!chk_currBM(currBM)) {
                        return DONOTHING;
                }
        }
        if (digestmode > 1||(!strncmp(currboard,"deleted", 7)&&!HAS_PERM(PERM_SYSOP))||(!strncmp(currboard,"syssecurity", 11)&&!HAS_PERM(PERM_SYSOP))||(!strcmp(currboard,"Punishment")&&!HAS_PERM(PERM_OBOARDS)&&!HAS_PERM(PERM_SYSOP)))
                return DONOTHING;
        //modified by yiyo syssecurity版不可删文章
        getdata(t_lines - 1, 0, "首篇文章编号: ", num, 6, DOECHO, YEA);
        inum1 = atoi(num);
        if (inum1 <= 0)
        {
                move(t_lines - 1, 50);
                prints("错误编号...");
                egetch();
                return PARTUPDATE;
        }
        getdata(t_lines - 1, 25, "末篇文章编号: ", num, 6, DOECHO, YEA);
        inum2 = atoi(num);
	if(uinfo.mode==RMAIL) sprintf(buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, DOT_DIR);
	else if(uinfo.mode==R_RMAIL)
	sprintf(buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");
	else sprintf(buf,"boards/%s/.DIR",currboard);
        if(fp=fopen(buf,"r"))
        {
                while(1) {
                        fseek(fp, total*sizeof(struct fileheader), SEEK_SET);
                        if(fread(&x, sizeof(x), 1, fp)<=0)
                                break;
                        total++;
                }
                fclose(fp);
        }
        if (inum2 < inum1 + 1 || inum2 > total)
        {
                move(t_lines - 1, 50);
                prints("错误区间...");
                egetch();
                return PARTUPDATE;
        }

        if(inum2>inum1+1000)
        {
                move(t_lines - 1, 0);
                sprintf(genbuf,"您将要同时删除多达%d篇文章,请确认",inum2-inum1);
                if (askyn(genbuf, NA, NA) == NA) {
                        move(t_lines - 1, 50);
                        prints("放弃...");
                        egetch();
                        return PARTUPDATE;
                }
        }
        move(t_lines - 1, 50);
        if (uinfo.mode==READING)
        {
                if (askyn("同时删除文摘", NA, NA) == YEA)
                        delg=1;
                else
                        delg=0;
                move (t_lines -1, 48);
                if (askyn ("不保留置顶文章", NA, NA) == YEA)
                        delg |= 2;
        } else
                delg=0;

        move(t_lines - 1, 48);
        if (askyn("  确定删除", NA, NA) == YEA)
        {
                //      delete_range(direct, inum1, inum2);
		if(uinfo.mode == READING)     //modenow ==0   in reading mode
                	result = delete_range(direct,inum1,inum2,delg,modenow);
		else {
			modenow = 1; // in mailbox
			result = delete_range(direct,inum1,inum2,delg,modenow);
		}
                fixkeep(direct, inum1, inum2);
                if (uinfo.mode == READING) {
                        sprintf(genbuf, "Range delete %d-%d on %s", inum1, inum2, currboard);
                        securityreport3(genbuf, 0);
                } else {
                        sprintf(genbuf, "Range delete %d-%d in mailbox", inum1, inum2);
                }
                report(genbuf);
                /* add by yiyo */
                if (result) {
                        *num = 0;
                        clear();
                        move(6,0);
                        prints("本板的上一次区段删除操作发生了错误． \n如果想消除此错误，[1;31m请务必确定目前没有其他人(板主或站长)在本板执行区段删操作．[m");
                        getdata(8,0,"[1;31m确定之后进行错误消除才安全，否则将丢失文章．[5m是否现在修复错误?[m(Y/N)?[N]:",num,6,DOECHO,YEA);
                        if (*num == 'Y' ||*num == 'y') {
                                sprintf(fullpath,"boards/%s/.tmpfile",currboard);
                                unlink(fullpath);
                                sprintf(fullpath,"boards/%s/.deleted",currboard);
                                unlink(fullpath);
                                prints("\n错误已经消除,请重新执行区段删除!");
                        } else
                                prints("\n不能确认，错误没有被消除．");
                        pressreturn();
                }
                /* add by yiyo 恢复区段删除造成的错误 */
                return DIRCHANGED;
        }
        move(t_lines - 1, 50);
        clrtoeol();
        prints("放弃删除...");
        egetch();
        return PARTUPDATE;
}
int UndeleteArticle(int ent,struct fileheader* fileinfo, char* direct)  /* undelete 一篇文章 Leeward 98.05.18 */
{
        char *p, buf[1024];
        char UBoard[48] = "deleted";
        char UTitle[128];
        struct fileheader UFile;
        int i;
#ifdef KEEP_DELETED_HEADER

        int kind = 0;
#endif

        FILE *fp;
        extern int SR_BMDELFLAG;

	if(strcmp(currboard, "TEMP")&&(digestmode!=8))
		return DONOTHING;
        if (!chk_currBM(currBM)&&!HAS_PERM(PERM_BLEVELS)) {
#ifdef KEEP_DELETED_HEADER
                if( fileinfo->owner[0] != '-')
                        return DONOTHING;
                kind = 1;
#else

                return DONOTHING;
#endif

        }
        sprintf(buf, "boards/%s/%s", currboard, fileinfo->filename);
        fp = fopen(buf, "r");
        if (!fp)
                return DONOTHING;

        strcpy(UTitle, fileinfo->title);
        if (p = strrchr(UTitle, '-')) { /* create default article title */
                *p = 0;
                for (i = strlen(UTitle) - 1; i >= 0; i --) {
                        if (UTitle[i] != ' ')
                                break;
                        else
                                UTitle[i] = 0;
                }
        }

        i = 0;
        while (!feof(fp) && i < 2) {
                fgets(buf, 1024, fp);
                if (feof(fp))
                        break;
                if (strstr(buf, "发信人: ") && (p = strstr(buf, "), 信区: "))) {
                        i ++;
                        strcpy(UBoard, p + 9);
                        if (p = strchr(UBoard, ' '))
                                *p = 0;
                        if (p = strchr(UBoard, '\n'))
                                *p = 0;
			     if(p = strchr(UBoard, '\r'))
                                *p = 0;
                } else if (strstr(buf, "标  题: ")) {
                        i ++;
                        strcpy(UTitle, buf + 8);
                        if (p = strchr(UTitle, '\n'))
                                *p = 0;
                }
        }
        fclose(fp);

#ifdef KEEP_DELETED_HEADER

        if(kind) {
                strncpy(fileinfo->owner,&(fileinfo->owner[1]),IDLEN + 2 );
                strncpy(fileinfo->title,UTitle,STRLEN);
                fileinfo->filename[STRLEN - 1] = 'L';
                fileinfo->filename[STRLEN - 2] = 'L';
                substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
                if (!strcmp(fileinfo->owner,currentuser.userid)) {
                        set_safe_record();
                        if (!junkboard())
                                currentuser.numposts++;
                        substitute_record(PASSFILE, &currentuser,
                                          sizeof(currentuser), usernum);
                } else if (!strstr(fileinfo->owner, ".") && BMDEL_DECREASE) {
                        int     id;
                        if (id = getuser(fileinfo->owner))
                                if (!junkboard()) {
                                        lookupuser.numposts++;
                                        substitute_record(PASSFILE, &lookupuser,
                                                          sizeof(struct userec), id);
                                }
                }
                return DIRCHANGED;
        }
#endif

        /*解决板主恢复文章问题 added by swen */
        if (!check_bBM(UBoard)&&!HAS_PERM(PERM_LOOKADMIN)) {
                clear();
                move(5,10);
                prints("对不起，您不是 %s 板板主!\n", UBoard);
                pressanykey();
                clear();
                return FULLUPDATE;
        }
        /* end of change*/

        if ((fileinfo->accessed[0] & FILE_FORWARDED)) {
                clear();
                move(2,0);
                prints("本文已恢复过了\n");
                pressreturn();
                return FULLUPDATE;
        }

        UFile.accessed[0] = 0;
        UFile.accessed[1] = 0;
        strcpy(UFile.owner, fileinfo->owner);
        strcpy(UFile.title, UTitle);
        strcpy(UFile.filename, fileinfo->filename);

        sprintf(buf, "/bin/cp -f boards/%s/%s boards/%s/%s",
                currboard, fileinfo->filename, UBoard, UFile.filename);
        system(buf);

        sprintf(buf, "boards/%s/.DIR", UBoard);
        append_record(buf, &UFile, sizeof(UFile));

        fileinfo->accessed[0] |= FILE_FORWARDED;
        substitute_record(direct, fileinfo, sizeof(*fileinfo),ent) ;
        sprintf(buf,"%s %s's “%s” on %s", (strcmp(currboard, "TEMP"))?"undeleted":"ADMINchecked", UFile.owner, UFile.title, UBoard);
        report(buf);

        clear();
        move(2,0);
        if(strcmp(currboard, "TEMP")) {
                prints("'%s' 已恢复到 %s 板 \n", UFile.title, UBoard);
		//fixed by iStef @ 6/15/2005
		sprintf(buf, "boards/%s/.junk", UBoard);
		delete_record(buf,sizeof(*fileinfo),ent);
		
        } else {
                sprintf(buf, "审核文章：%s", UFile.title);
                securityreport3(buf, -1);
                prints("'%s' 已审核并恢复到 %s 板 \n", UFile.title, UBoard);
                SR_BMDELFLAG=YEA;
                del_post(ent,fileinfo, direct,0);
                SR_BMDELFLAG=NA;
                //		delete_file(direct, sizeof(struct fileheader), ent, cmpfilename);

        }
        pressreturn();

        return FULLUPDATE;
}

//******************  modified by swen 02.5.31  文章顺序移动***********
/* modified again by yiyo 邮件顺序移动 */
int move_post(int ent,struct fileheader* fileinfo, char* direct)
{
        int         fail;
        int         new_pos;
        char        num[8];
        struct      shortfile *bp;
        extern char currmaildir[STRLEN];

        if (!in_mail) {
                if(digestmode==2||!strcmp(currboard,"deleted")||!strcmp(currboard,"junk")||!strncmp(currboard,"syssecurity", 11))
                        return DONOTHING;

                bp = getbcache(currboard);
                if((bp->flag&ANONY_FLAG) && !strcmp(fileinfo->owner,currboard))
                        return DONOTHING;

                if( !chk_currBM(currBM) ) {
                        return DONOTHING ;
                }
                if (fileinfo->accessed[1] & FILE_ONTOP)
                        return DONOTHING;
        }
        //检查结束
        move(t_lines-2,0);
        clrtoeol();
        move(t_lines-3,0);
        clrtoeol();
        getdata(t_lines-3,0,"移动至编号: ",num,6,DOECHO,YEA) ;
        new_pos = atoi(num);
        if(new_pos <= 0) {
                move(t_lines-2,0);
                clrtoeol();
                prints("错误编号!") ;
                pressreturn() ;
                return FULLUPDATE ;
        }
        if(new_pos == ent) {
                move(t_lines-2,0);
                clrtoeol();
                prints("保持次序不变!");
                pressreturn();
                return FULLUPDATE ;
        }
        move(t_lines-2,0);
        clrtoeol();
        if(askyn("确定移动?",NA,NA) == YEA) {
                if(!in_mail)
                        fail = move_file(direct,sizeof(struct fileheader),ent,new_pos);
                else
                        fail = move_file(currmaildir,sizeof(struct fileheader),ent,new_pos);
        }
        if(fail == -2) {
                move(t_lines-2,0);
                clrtoeol();
                prints("错误编号!") ;
                pressreturn() ;
                return FULLUPDATE ;
        }
        if(fail == -1) {
                move(t_lines-2,0);
                clrtoeol();
                prints("移动失败!") ;
                pressreturn();
                clear();
        }
        return DIRCHANGED;
}
//*******************************************************************

int del_post(int ent, struct fileheader *fileinfo, char *direct,int dflag)
{
		//struct fileheader postfile;
        char    buf[512];
        char    usrid[STRLEN];
        char   *t;
        int     owned=0, fail, IScurrent=0;
        int 	posttime;
        extern int SR_BMDELFLAG;

        /*   if ((askyn("删除文摘", NA, NA) == YEA)&&(fileinfo->accessed[0] & (FILE_DIGEST)))
                return DONOTHING;
        */
        /* Efan: 旧代码	*/
        /*
           if ((fileinfo->accessed[0] & (FILE_MARKED|FILE_DIGEST))&&!HAS_PERM(PERM_LOOKADMIN)) return DONOTHING;
        */
        if ( ( (dflag & 4) && (fileinfo->accessed [0] & FILE_DIGEST) ) ||
                        (fileinfo->accessed [0] & (FILE_MARKED) ) && ! HAS_PERM (PERM_LOOKADMIN) )
                return DONOTHING;
        if (dflag & 4)
                dflag -= 4;
        if (digestmode>1||!strcmp(currboard,"deleted")||!strcmp(currboard,"junk")||!strncmp(currboard,"syssecurity", 11)||(!strcmp(currboard,"Punishment")&&!HAS_PERM(PERM_OBOARDS)&&!HAS_PERM(PERM_SYSOP)))
                return DONOTHING;
    /*    if (fileinfo->owner[0] == '-')
                return DONOTHING;*/
        strcpy(usrid, fileinfo->owner);
        if ( !strstr(usrid, ".") )
                IScurrent = !strcmp(usrid, currentuser.userid);

        strcpy(buf,&(fileinfo->filename[2]));
        buf[strlen(buf)-2] = '\0';
        posttime = atoi(buf);
        if( !IScurrent )
        {
                owned = getuser(usrid);
                if( owned!=0 ) {
                        if(posttime < lookupuser.firstlogin)
                                owned = 0;
                }
        } else
                owned =  posttime > currentuser.firstlogin;

        if (!chk_currBM(currBM)&&!HAS_PERM(PERM_LOOKADMIN))
        {
                struct shortfile *bp;
                if (! (owned && IScurrent))
                        return DONOTHING;
                bp = getbcache(currboard);
                if ((bp->flag & ANONY_FLAG) && !strcmp(usrid, currboard))
                        return DONOTHING;
        }

        /* Efan: 取消置顶文章	*/
        if (fileinfo->accessed[1] & FILE_ONTOP)
        {
                int fd;
                int nfh;
                struct fileheader fh;
                int n;
                int size = sizeof (struct fileheader);
                if (!chk_currBM (currBM))
                        return DONOTHING;
                fd = open (direct, O_RDONLY);
                n = 0;
                if (fd != -1) {
                        while ((nfh = read (fd, & fh, size)) == size) {
                                ++n;
                                if (!strcmp (fileinfo->filename, fh.filename) && (fh.accessed[1] & FILE_TIP))
                                        break;
                        }
                        close (fd);
                        if (nfh == size)
                                return BM_ontop (n, & fh, direct);



                        sprintf (genbuf, "文章取消置顶：●%-.55s", fileinfo->title);
                        if (askyn(genbuf, NA, YEA) == NA) {
                                move(t_lines - 1, 0);
                                prints("放弃...");
                                clrtoeol();
                                egetch();
                                return PARTUPDATE;
                        }

Deltop:

                        setbfile (genbuf, currboard, ".top");
                        n = get_num_records (genbuf, size);
                        fd = open (genbuf, O_RDWR);
                        if (fd == -1)
                                return DONOTHING;
                        while ((nfh = read (fd, &fh, size)) == size)
                                if (!strcmp (fileinfo->filename, fh.filename))
                                        break;
                        if (nfh == size) {
                                while (read (fd, &fh, size) == size) {
                                        lseek (fd, -2 * size, SEEK_CUR);
                                        write (fd, &fh, size);
                                        lseek (fd, size, SEEK_CUR);
                                }
                                close (fd);
                                truncate (genbuf, (n -1) * size);
                                return FULLUPDATE;
                        } else
                                close (fd);
                        return FULLUPDATE;
                }
        }
        if (!SR_BMDELFLAG)
        {
                if (fileinfo->accessed[1] & FILE_TIP) {

                        if (!chk_currBM (currBM)) {
                                move (t_lines -1, 0);
                                prints ("此文章已被置顶，只有板主才能删除");
                                clrtoeol ();
                                egetch ();
                                return PARTUPDATE;
                        }
                        sprintf (genbuf, "此文章 [%-.55s] 已置顶，确认删除？", fileinfo->title);
                        if (askyn(genbuf, NA, YEA) == NA) {
                                move (t_lines -1, 0);
                                prints ("放弃删除文章...");
                                clrtoeol ();
                                egetch ();
                                return PARTUPDATE;
                        }
                } else {
                        sprintf(genbuf, "删除文章 [%-.55s]", fileinfo->title);
                        if (askyn(genbuf, NA, YEA) == NA) {
                                move(t_lines - 1, 0);
                                prints("放弃删除文章...");
                                clrtoeol();
                                egetch();
                                return PARTUPDATE;
                        }
                }
        }
        strncpy(currfile, fileinfo->filename, STRLEN);
#ifndef KEEP_DELETED_HEADER

        fail = delete_file(direct, sizeof(struct fileheader), ent, cmpfilename);
#else

        fail = update_file(direct, sizeof(struct fileheader), ent, cmpfilename,
                           cpyfilename);
#endif

        if (!fail)
        {

                strcpy(buf, direct);
                if ((t = strrchr(buf, '/')) != NULL)
                        *t = '\0';
                if(HAS_PERM(PERM_LOOKADMIN)&&!(owned && IScurrent)) {
                        sprintf(genbuf, "ADMINDel '%s' on '%s' mode:%d", fileinfo->title, currboard, dflag);
                } else {
                        sprintf(genbuf, "Del '%s' on '%s' mode:%d", fileinfo->title, currboard, dflag);
                }
                report(genbuf);
				/*modified by livebird */
		/*	memset(&postfile, 0, sizeof(postfile));
			sprintf(genbuf, "%-32.32s - %s", fileinfo->title, currentuser.userid);
			strncpy(postfile.title, genbuf, STRLEN);
			strcpy(postfile.filename,fileinfo->filename);
			strncpy(postfile.owner, fileinfo->owner, IDLEN + 2);
			postfile.filename[STRLEN - 1] = 'D';
			postfile.filename[STRLEN - 2] = 'D';
			sprintf(genbuf,"boards/%s/.junk",currboard);
		    append_record(genbuf,&postfile, sizeof(postfile));*/ 
				
           /*   if (!NotBackupBoard ())
                       if(owned && IScurrent)
                                cancelpost(currboard, currentuser.userid, fileinfo, 1);
                        else  
				*/		cancelpost(currboard, currentuser.userid, fileinfo, 0);
               /* else if (strcmp (currboard, "deleted") && strcmp (currboard, "junk"))
                        cancelpost (currboard, currentuser.userid, fileinfo, 2);*/

                sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
#ifndef KEEP_DELETED_HEADER
                //unlink(genbuf);
#endif

                if ( !junkboard() && !digestmode) {
                        if (owned && IScurrent && dflag!=2) {
                                set_safe_record();
                                if (currentuser.numposts > 0 )
                                        currentuser.numposts--;
                                substitute_record(PASSFILE,&currentuser,sizeof(currentuser),usernum);
                        }
                        //		else if ( owned && BMDEL_DECREASE && dflag!=1)
                        else if ( owned && BMDEL_DECREASE && dflag==0) {
                                if (lookupuser.numposts > 0)
                                        lookupuser.numposts--;
                                substitute_record(PASSFILE,&lookupuser,sizeof(struct userec),owned);
                        } else if ( owned && BMDEL_DECREASE && dflag==1) {
                                if (lookupuser.numposts >1)
                                        lookupuser.numposts-=2;
                                substitute_record(PASSFILE,&lookupuser,sizeof(struct userec),owned);
                        }
                }

                if (fileinfo->accessed[1] & FILE_TIP)
                        goto Deltop;

                return DIRCHANGED;
        } else if(SR_BMDELFLAG)
        {
                return -1;
        }
        move(t_lines - 1, 0);
        prints("删除失败...");
        clrtoeol();
        egetch();
        return PARTUPDATE;
}

int normal_del_post(int ent, struct fileheader *fileinfo, char *direct)
{
        return del_post(ent,fileinfo, direct,0);
}
int kill_del_post(int ent, struct fileheader *fileinfo, char *direct)
{
        return del_post(ent, fileinfo, direct,1);
}
//add by yiyo '-'删文

//#ifdef QCLEARNEWFLAG
//#define BRC_MAXNUM 120
int flag_clearto(int ent, char *direct, int clearall)
{
        int  fd, posttime, size;
        //char filename[20];
        struct fileheader f_info;
        if(uinfo.mode != READING)
                return DONOTHING;
        if( (fd = open(direct, O_RDONLY, 0)) == -1 )
                return DONOTHING;
        size = sizeof(struct fileheader);
        if(clearall) {

                clear_new_flag_quick(time(0));
                return PARTUPDATE;
        } else
                lseek(fd, (off_t)((ent-1)*size), SEEK_SET);
        read(fd, &f_info, size);
        close(fd);
        posttime = atoi(&(f_info.filename[2]));
        clear_new_flag_quick(posttime);

        return PARTUPDATE;
}
//#else
int flag_clearto2(int ent, char *direct, int clearall)
{
        int fd, i;
        struct fileheader f_info;
        if(uinfo.mode != READING)
                return DONOTHING;
        if( (fd = open(direct, O_RDONLY, 0)) == -1 )
                return DONOTHING;
        for(i = 0; clearall || i < ent; i ++) {
                if(read(fd, &f_info, sizeof(f_info)) != sizeof(f_info))
                        break;
                brc_addlist(f_info.filename);
        }
        close(fd);
        return PARTUPDATE;
}
//#endif
int clear_new_flag(int ent, struct fileheader* fileinfo, char* direct)
{

        clear_new_flag_quick(time(0));
        return PARTUPDATE;
}

int new_flag_clearto(int ent, struct fileheader *fileinfo, char *direct)
{
        if (fileinfo->accessed[1] & FILE_ONTOP)
                return clear_new_flag (ent, direct, YEA);
        else
                return flag_clearto(ent, direct, NA);
}

int new_flag_clear(int ent, struct fileheader *fileinfo, char *direct)
{
        //	return flag_clearto(ent, direct, YEA);
        return clear_new_flag(ent, direct, YEA);
}

/* Added by netty to handle post saving into (0)Announce */
int Save_post(int ent, struct fileheader* fileinfo, char* direct)
{
	if(check_user_exist(fileinfo->owner))
		return FULLUPDATE;
        if (!chk_currBM(currBM))
                return DONOTHING;
        return (a_Save("0Announce", currboard, fileinfo, NA));
}
/*
int full_Save_post(int ent, struct fileheader *fileinfo, char *direct)
{
   return Save_post(ent, fileinfo, direct,1);
}
 
int part_Save_post(int ent, struct fileheader *fileinfo, char *direct)
{
   return Save_post(ent, fileinfo, direct,0);
}
 
int Save_post(int ent, struct fileheader *fileinfo, char *direct, int full)
{
   if (!chk_currBM(currBM)) return DONOTHING;
   return (a_Save("0Announce", currboard, fileinfo, NA, full));
}
*/
//modified by yiyo

/* Added by netty to handle post saving into (0)Announce */
/*
int
Import_post(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
	//if (!chk_currBM(currBM))
	if (!HAS_PERM(PERM_BOARDS|PERM_PERSONAL))
		return DONOTHING;
	return (a_Import("0Announce", currboard, ent, fileinfo, direct, NA));
}
*/
int Import_post(int ent, struct fileheader *fileinfo, char *direct)
{
        FILE *fn;
        char annpath[512];
	if(check_user_exist(fileinfo->owner))
		return FULLUPDATE;
        if (!HAS_PERM(PERM_BOARDS|PERM_PERSONAL))
                return DONOTHING;
        //   if (!chk_currBM(currBM)) return DONOTHING; /*add by wzk for all BM can do*/

        sethomefile(annpath, currentuser.userid,".announcepath");
        if((fn = fopen(annpath, "r")) == NULL )
        {
                presskeyfor("对不起, 你没有设定丝路. 请先用 f 设定丝路.",t_lines-1);
                return 1;
        }
        fscanf(fn,"%s",annpath);
        fclose(fn);
        if (!dashd(annpath))
        {
                presskeyfor("你设定的丝路已丢失, 请重新用 f 设定.",t_lines-1);
                return 1;
        }


    if(fileinfo->accessed[0] & FILE_VISIT)
    {
            if(askyn("文章曾经放入了精华区, 现在还要再放入吗",YEA,YEA)==NA)
                    return 1;
    } else
    {
           if(!strstr(annpath,"0Announce/PersonalCorpus")){
            fileinfo->accessed[0] |= FILE_VISIT;  /* 将标志置位 */
            substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
           }
     }
            
           
        return (a_Import("0Announce", currboard, fileinfo, annpath, NA));
}
//add by yiyo

int check_notespasswd()
{
        FILE   *pass;
        char    passbuf[20], prepass[STRLEN];
        char    buf[STRLEN];
        setvfile(buf, currboard, "notespasswd");
        if ((pass = fopen(buf, "r")) != NULL) {
                fgets(prepass, STRLEN, pass);
                fclose(pass);
                prepass[strlen(prepass) - 1] = '\0';
                getdata(2, 0, "请输入秘密备忘录密码: ", passbuf, 19, NOECHO, YEA);
                if (passbuf[0] == '\0' || passbuf[0] == '\n')
                        return NA;
                if (!checkpasswd(prepass, passbuf)) {
                        move(3, 0);
                        prints("错误的秘密备忘录密码...");
                        pressanykey();
                        return NA;
                }
        }
        return YEA;
}

int show_b_secnote()
{
        char    buf[256];
        clear();
        setvfile(buf, currboard, "secnotes");
        if (dashf(buf)) {
                if (!check_notespasswd())
                        return FULLUPDATE;
                clear();
                ansimore(buf, NA);
        } else {
                move(3, 25);
                prints("此讨论区尚无「秘密备忘录」。");
        }
        pressanykey();
        return FULLUPDATE;
}

int show_b_note()
{
        clear();
        if (show_board_notes(currboard, 2) == -1) {
                move(3, 30);
                prints("此讨论区尚无「备忘录」。");
                pressanykey();
        }
        return FULLUPDATE;
}

int into_announce()
{
        char	found[STRLEN];
	char	tmpboard[STRLEN - BM_LEN];
	strcpy(tmpboard, currboard);
        /*
        if (!strcmp(currboard,"PersonalCorpus"))
        {
        sprintf(found,"0Announce/PersonalCorpus");
        a_menu("",found,((HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP)
                    || HAS_PERM(PERM_OBOARDS)) ? PERM_BOARDS : 0),0);
        return FULLUPDATE;
        }//modified by yiyo 该版精华区和个人文集目录统一
        */
        if (a_menusearch("0Announce", currboard, found)) {
                a_menu("",found,((HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_OBOARDS) || chk_currZM()) ? PERM_BOARDS : 0),0);
		strcpy(currboard, tmpboard);
                return FULLUPDATE;
                //return MODECHANGED;
        }
        return DONOTHING;
}
int into_myAnnounce()
{
        Personal("*");
        return FULLUPDATE;
}

int into_PAnnounce()
{
        Personal(NULL);
        return FULLUPDATE;
}

int Personal(char *userid)
{
        char    found[256], lookid[IDLEN+6];
        //  char user[IDLEN];
        int     id;

        if(!userid) {
                clear();
                move(2, 0);
                usercomplete( "您想看谁的个人文集: " , lookid);
                if (lookid[0] == '\0') {
                        clear();
                        return 1;
                }
        } else
                strcpy(lookid, userid);
        if(lookid[0] == '*') {
                sprintf(lookid,"%c/%s",
                        toupper(currentuser.userid[0]),currentuser.userid);
                sprintf(found,"0Announce/PersonalCorpus/%s",lookid);
                if(!dashd(found)) {
                        move(9,0);
                        clrtobot();
                        prints("没有找到您的个人文集.\n");
                        char ysapply[4];
                        if(HAS_PERM(PERM_POST)) {
                                getdata(11,0,"请问您要现在申请一个吗?(整理文集可是件辛苦的事情啊)(y/n)",ysapply,3,DOECHO,YEA);
                                if(ysapply[0]=='y'||ysapply[0]=='Y') {
                                        AddPCorpusself(currentuser.userid);
                                }
                        }
                }


        } else {
                if (!(id = getuser(lookid))) {
                        /*
                        			lookid[1] = toupper(lookid[0]);
                        			if(lookid[1] < 'A' || lookid[1] > 'Z')
                        				lookid[0] = '\0';
                        			else
                        			{
                        				lookid[0] = '/';
                        				lookid[2] = '\0';
                        			}
                        */
                        if (lookid [0] < 'A' || lookid [1] > 'Z')
                                lookid [0] = 0;
                        else
                                lookid [1] = 0;
                } else {
                        sprintf(lookid, "%c/%s",
                                toupper(lookupuser.userid[0]),lookupuser.userid);
                }
        }
        //   sprintf(found,"0Announce/groups/GROUP_0/PersonalCorpus/%s",lookid);
        sprintf(found,"0Announce/PersonalCorpus/%s",lookid);
        if(!dashd(found))
                sprintf(found,"0Announce/PersonalCorpus");
        a_menu("",found,((HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP)) ? PERM_BOARDS : 0),0);
        return 1;
}
/*add by tdhlshx 用户自己创建个人文集的函数*/
int AddPCorpusself(char * userid)
{
        FILE *fn;
        char    digestpath[80] = "0Announce/PersonalCorpus";
        char    personalpath[80], Log[200], title[200], ftitle[80];
        time_t  now;
        int     id;
        if(id = getuser(currentuser.userid)) {
                move(13,0);
                prints("创建个人文集");
                sprintf(personalpath,"%s/%c/%s",digestpath,toupper(currentuser.userid[0]),currentuser.userid);
                mkdir(personalpath, 0755);
                chmod(personalpath, 0755);
                move(15,0);
                prints("[直接按ENTER键,则标题缺省为:[32m%s 的个人文集[m]",currentuser.userid);
                getdata(14,0,"请输入您的文集标题(文集建立后,您将无权修改此标题):",title,39,DOECHO,YEA);
                if(title[0] == '\0')
                        sprintf(title,"%s 的个人文集",currentuser.userid);
                sprintf(title, "%-38.38s(BM: %s)", title, currentuser.userid);
                sprintf(digestpath,"%s/%c",digestpath,toupper(currentuser.userid[0]));
                sprintf(ftitle,"个人文集 －－ (首字母 %c)", toupper(currentuser.userid[0]));
                linkto(digestpath, currentuser.userid, title);
                sprintf(personalpath, "%s/.Names", personalpath);
                if ((fn = fopen(personalpath, "w")) == NULL) {
                        return -1;
                }
                fprintf(fn, "#\n");
                fprintf(fn, "# Title=%s\n", title);
                fprintf(fn, "#\n");
                fclose(fn);
                if(!(currentuser.userlevel & PERM_PERSONAL)) {
                        char    secu[STRLEN];
                        currentuser.userlevel |= PERM_PERSONAL;
                        substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
                        sprintf(secu, "创建个人文集, 给予 %s 文集管理权限", currentuser.userid);

                        securityreport(secu);
                        move(16,0);
                        prints(secu);
                        sethomefile(genbuf,currentuser.userid,".announcepath");
                        report(genbuf);
                        if(fn = fopen(genbuf,"w+")) {
                                fprintf(fn,"%s/%s",digestpath,currentuser.userid);
                                fclose(fn);
                        }

                }
                now = time(NULL);
                sprintf(genbuf,"%-12.12s    %14.14s    %.38s",
                        currentuser.userid, datestring,title);
                add_to_file(Log,genbuf);
                prints("\n恭喜!从现在开始,您有个人文集啦!Go & enjoy~ :)\nBTW:本站www方式提供文集打包下载功能.\n    请珍惜本站资源,在您不需要此文集的时候可以向站务提出删除\n");
                pressanykey();
                return 1;
        }
}

#ifdef INTERNET_EMAIL
int forward_post(int ent, struct fileheader* fileinfo, char* direct)
{
        if (strcmp("guest", currentuser.userid) == 0)
                return DONOTHING;
	if(check_user_exist(fileinfo->owner))
		return FULLUPDATE;
        return (mail_forward(ent, fileinfo, direct));
}

int forward_u_post(int ent, struct fileheader* fileinfo, char* direct)
{
        if (strcmp("guest", currentuser.userid) == 0)
                return DONOTHING;
	if(check_user_exist(fileinfo->owner))
		return FULLUPDATE;
        return (mail_u_forward(ent, fileinfo, direct));
}
#endif

int Import_post_range(int ent, struct fileheader *fileinfo, char *direct)
{

    char num[8];
	getdata(t_lines - 1, 0, "首篇文章编号: ", num, 6, DOECHO, YEA);
    int inum1 = atoi(num);
    if (inum1 <= 0)
    {
            move(t_lines - 1, 50);
            prints("错误编号...");
            egetch();
            return PARTUPDATE;
    }
    getdata(t_lines - 1, 25, "末篇文章编号: ", num, 6, DOECHO, YEA);
    int inum2 = atoi(num);

	char buf[STRLEN];
    sprintf(buf,"boards/%s/.DIR",currboard);
	int total=get_num_records(buf,sizeof(struct fileheader));
    if (inum2 < inum1 + 1 || inum2 > total)
    {
               move(t_lines - 1, 50);
               prints("错误区间...");
               egetch();
               return PARTUPDATE;
    }
    move(t_lines - 1, 48);
    if (askyn("确定区段收入", NA, NA) == NA)
    {  
		 move(t_lines - 1, 50);
	     clrtoeol();
	     prints("放弃删除...");
		 egetch();
         return PARTUPDATE;
    }

 FILE *fn;
 char annpath[512];
 sethomefile(annpath, currentuser.userid,".announcepath");
 if((fn = fopen(annpath, "r")) == NULL )
 {
         presskeyfor("对不起, 你没有设定丝路. 请先用 f 设定丝路.",t_lines-1);
         return PARTUPDATE;
 }
 fscanf(fn,"%s",annpath);
 fclose(fn);
 if (!dashd(annpath))
 {
         presskeyfor("你设定的丝路已丢失, 请重新用 f 设定.",t_lines-1);
         return PARTUPDATE;
 }


//	int fd;
//	if((fd=open(".dellock",O_RDWR|O_CREAT|O_APPEND, 0644)) == -1)
  //        return -1;
   // flock(fd, LOCK_EX);
    FILE *fp;
 struct fileheader x;
    if(fp=fopen(buf,"r"))
    {
		    fseek(fp, (inum1-1)*sizeof(struct fileheader), SEEK_SET);
			int i=inum1;
            while(1){
                    if(fread(&x, sizeof(x), 1, fp)<=0) break;
                    if(++i>inum2) break;

 // x.accessed[0] |= FILE_VISIT;  /* 将标志置位 */
 // substitute_record(direct, &x, sizeof(x), ent);

					a_Import("0Announce", currboard, &x, annpath, YEA);
            }
            fclose(fp);
    }

//    flock(fd, LOCK_UN);
//    close(fd);
    return PARTUPDATE;
}   




extern int mainreadhelp();
extern int b_vote();
extern int b_results();
extern int b_vote_maintain();
extern int b_notes_edit();
extern int Rangefunc();//add by yiyo 区段操作
extern int y_lockscreen();//add by yiyo
extern int showbadwords();
extern int rmd_thesis(); //设置web推荐
struct one_key read_comms[] =
        {
                '_', underline_post,
                'w', makeVISITflag,
                'Y', UndeleteArticle,
                'r', read_post,
                'K', skip_post,
                'G', BM_ontop,
                //	'G', range_do,		 Efan: 这段代码不用了吧？我用"G"来作置顶喽~~~
                //	'u', skip_post, * // * Key 'u' use to query user's info */
                'd', normal_del_post,
                '-', kill_del_post,//add by yiyo '-'删文减2文章数
                'D', del_range,
                'm', mark_post,
                'E', edit_post,
                '>', move_post,   //swen modified 02.5.31
                '<', ban_detail,	//bluetent 2003.01.13 显示详细封禁情况
                '.', show_detail,	//bluetent 2003.01.15 显示文章详细信息
               // ',', compactread,
                Ctrl('G'), acction_mode,
                Ctrl('H'),deny_from_article,//tdhlshx add 2003.12.6 板面文章封禁ID
                '`', acction_mode,
                'g', digest_post,
                'T', edit_title,
                's', do_select,
                Ctrl('C'), do_cross,
                Ctrl('P'), do_post,
                Ctrl('O'), do_nhpost,//add by yiyo ASCIIArt无文章头发文
                'C', new_flag_clearto,	/* 清除未读标记到当前位置 */
                'c', new_flag_clear,  /* 清全部未读标记 */
#ifdef INTERNET_EMAIL
                'F', forward_post,
                'U', forward_u_post,
                Ctrl('R'), post_reply,
#endif
                'i', Save_post,
                'I', Import_post,
               // Ctrl('I'),Import_post_range,
                'R', b_results,
                'v', b_vote,
                'V', b_vote_maintain,
                //	'M', b_vote_maintain,
                'W', b_notes_edit,
                Ctrl('W'), b_notes_passwd,
                'h', mainreadhelp,
                Ctrl('J'), mainreadhelp,
                KEY_TAB, show_b_note,
                'z', show_b_secnote,
                'Z', sendmsgtoauthor,//add by yiyo
                'x', into_announce,
                'X', into_myAnnounce,
                Ctrl('X'), into_PAnnounce,
                'a', auth_search_down,
                'A', auth_search_up,
                '/', t_search_down,
                '?', t_search_up,
                '\'', post_search_down,
                '\"', post_search_up,
                ']', thread_down,
                '[', thread_up,
                Ctrl('D'), deny_user,
#ifdef BOARD_CONTROL
                Ctrl('Y'), control_user,
#endif
                Ctrl('K'), y_lockscreen,//add by yiyo 快速屏幕锁定
                Ctrl('A'), show_author,
                Ctrl('N'), SR_first_new,
                'n', SR_first_new,
                '\\', SR_last,
                '=', SR_first,
                Ctrl('S'), SR_read,
                'p', SR_read,
                Ctrl('U'), SR_author,
                'b', SR_BMfunc,
                'B', Rangefunc,//modified by yiyo
                Ctrl('T'), acction_mode,
                't', thesis_mode,	/* youzi 1997.7.8 */
                '!', Q_Goodbye,
                'S', s_msg,
                'f', t_friends,
                '#', showbadwords,
		'J', rmd_thesis,
                '\0', NULL
        };

int Read()
{
        char    buf[STRLEN];
        char    notename[STRLEN];
        time_t  usetime;
        struct stat st;
        if (!selboard) {
                move(2, 0);
                prints("请先选择讨论区\n");
                pressreturn();
                move(2, 0);
                clrtoeol();
                return -1;
        }

        in_mail = NA;
        brc_initial(currboard);
        setbdir(buf, currboard);
        //added by silencer板面外网屏蔽//
        if(!strcmp("SARS",currboard) && !isAllowedIp(fromhost)) {
                move(2, 0);
                prints("板面系统维护中\n");
                pressreturn();
                move(2, 0);
                clrtoeol();
                return -1;
        }


#ifdef BOARD_CONTROL
        // if(!HAS_PERM(PERM_SYSOP)&&!chk_currBM(currBM)){
        if( !HAS_PERM(PERM_SYSOP) || deny_s_board(currboard)) {
                setbfile(notename, currboard, "board.allow");
                if(dashf(notename)&&!seek_in_file(notename,currentuser.userid)) {
                        clear();
                        move(5,10);
                        prints("很抱歉, 本板面属于团体板面，而您不是该板成员");
                        pressanykey();
                        return 0;
                }
        }
#endif

#ifdef BOARD_READDENY
        if(!HAS_PERM(PERM_SYSOP)&&!chk_currBM(currBM)) {
                setbfile(notename, currboard, "board.deny");
                if(dashf(notename)&&seek_in_file(notename,currentuser.userid)) {
                        clear();
                        move(5,20);
                        prints("对不起，您目前被停止进入本板的权利。");
                        pressanykey();
                        return 0;
                }
        }
#endif

        setvfile(notename, currboard, "notes");
        if (stat(notename, &st) != -1) {
                if (st.st_mtime < (time(NULL) - 7 * 86400)) {
                        utimes(notename, NULL);
                        setvfile(genbuf, currboard, "noterec");
                        unlink(genbuf);
                }
        }
#ifdef ALWAYS_SHOW_BRDNOTE
        if (dashf(notename))
                ansimore3(notename, YEA);
#else

        if (vote_flag(currboard, '\0', 1 /* 检查读过新的备忘录没 */ ) == 0) {
                if (dashf(notename)) {
                        ansimore3(notename, YEA);
                        vote_flag(currboard, 'R', 1 /* 写入读过新的备忘录 */ );
                }
        }
#endif
        usetime = time(0);
        strcpy(uinfo.board, currboard);
        update_ulist(&uinfo, utmpent);
        (*(int*) (bcache[getbnum(uinfo.board)-1].filename+72))++;
        i_read(READING, buf, readtitle, readdoent, &read_comms[0], sizeof(struct fileheader));
        if((*(int*) (bcache[getbnum(currboard)-1].filename+72))>0)
                (*(int*) (bcache[getbnum(currboard)-1].filename+72))--;
        strcpy(uinfo.board, "-");
        update_ulist(&uinfo, utmpent);
        board_usage(currboard, time(0) - usetime);

        brc_update();
        return 0;
}
/*Add by SmallPig*/
void notepad()
{
        char    tmpname[STRLEN], note1[4];
        char    note[3][STRLEN - 4];
        char    tmp[STRLEN];
        FILE   *in;
        int     i, n;
        time_t  thetime = time(0);
        extern int talkrequest;


        clear();
        move(0, 0);
        prints("开始你的留言吧！大家正拭目以待....\n");
        modify_user_mode(WNOTEPAD);
        sprintf(tmpname, "tmp/notepad.%s.%05d", currentuser.userid, uinfo.pid);
        if ((in = fopen(tmpname, "w")) != NULL) {
                for (i = 0; i < 3; i++)
                        memset(note[i], 0, STRLEN - 4);
                while (1) {
                        for (i = 0; i < 3; i++) {
                                getdata(1 + i, 0, ": ", note[i], STRLEN - 5, DOECHO, NA);
                                if (note[i][0] == '\0')
                                        break;
                        }
                        if (i == 0) {
                                fclose(in);
                                unlink(tmpname);
                                return;
                        }
                        getdata(5, 0, "是否把你的大作放入留言板 (Y)是的 (N)不要 (E)再编辑 [Y]: ", note1, 3, DOECHO, YEA);
                        if (note1[0] == 'e' || note1[0] == 'E')
                                continue;
                        else
                                break;
                }
                if (note1[0] != 'N' && note1[0] != 'n') {
                        sprintf(tmp, "[1;32m%s[37m（%.18s）", currentuser.userid, currentuser.username);

                        fprintf(in, "[1;34m▕[44m▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔[36m酸[32m甜[33m苦[31m辣[37m板[34m▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔[44m◣[m\n");
                        getdatestring(thetime,NA);
                        fprintf(in, "[1;34m▕[32;44m %-44s[32m在 [36m%23.23s[32m 离开时留下的话  [m\n", tmp, datestring+6);
                        for (n = 0; n < i; n++) {
                                if (note[n][0] == '\0')
                                        break;
                                fprintf(in, "[1;34m▕[33;44m %-75.75s[1;34m[m \n", note[n]);
                        }
                        fprintf(in,  "[1;34m▕[44m ───────────────────────────────────── [m \n");
                        catnotepad(in, "etc/notepad");
                        fclose(in);
                        rename(tmpname, "etc/notepad");
                } else {
                        fclose(in);
                        unlink(tmpname);
                }
        }
        if (talkrequest) {
                talkreply();
        }
        clear();
        return;
}
/* youzi quick goodbye */
int Q_Goodbye()
{
        extern int started;
        extern int *zapbuf;
        char    fname[STRLEN];
        int     logouts;

        free(zapbuf);

        setuserfile(fname, "msgfile");

#ifdef LOG_MY_MESG

        unlink(fname);
        setuserfile(fname, "msgfile.me");
#endif


        //modified by bluetent Oct.7, 2002
        if (!DEFINE(DEF_NOTSHOWMSG)) {
                if (dashf(fname)) {
                        clear();
                        mesgmore(fname, NA, 0, 9999);
                }
        }
        clear();
        //	prints("\n\n\n\n");

        setuserfile(fname, "notes");
        if (dashf(fname))
                ansimore(fname, YEA);

        setuserfile(fname, "logout");
        if (dashf(fname)) {
                logouts = countlogouts(fname);
                if (logouts >= 1) {
                        user_display(fname,
                                     (logouts == 1) ? 1 : (currentuser.numlogins % (logouts)) + 1, YEA);
                }
        } else {
                if (fill_shmfile(2, "etc/logout", "GOODBYE_SHMKEY"))
                        show_goodbyeshm();
        }
        pressreturn(); // sunner 的建议
        report("exit");
        if (started) {
                time_t  stay;
                stay = time(0) - login_start_time;
                sprintf(genbuf, "Stay:%3ld (%s)", stay / 60, currentuser.username);
                log_usies("EXIT ", genbuf);
                u_exit();
        }
        /*	{ char buf[200],path[80],*t;
         	setuserfile(path, ".lastread");
        	t = strstr(path,".");
        	*t = '\0';
        	sprintf(buf,"cp %s/.lastread %s/.lastread.backup",path,path);
        	system(buf);
                sprintf(buf,"cp %s/.boardrc %s/.boardrc.backup",path,path);
                system(buf);
        	} 
        */
        //pressreturn();
#ifdef CHK_FRIEND_BOOK
        if (num_user_logins(currentuser.userid) == 0 &&
                        strcmp(currentuser.userid, "guest")) {
                FILE   *fp;
                char    buf[STRLEN], *ptr;
                if ((fp = fopen("friendbook", "r")) != NULL) {
                        while (fgets(buf, sizeof(buf), fp) != NULL) {
                                char    uid[14];
                                ptr = strstr(buf, "@");
                                if (ptr == NULL) {
                                        del_from_file("friendbook", buf);
                                        continue;
                                }
                                ptr++;
                                strcpy(uid, ptr);
                                ptr = strstr(uid, "\n");
                                *ptr = '\0';
                                if (!strcmp(uid, currentuser.userid))
                                        del_from_file("friendbook", buf);
                        }
                        fclose(fp);
                }
        }
#endif
        sleep(1);
#ifndef BBSD

        reset_tty();
#endif

        bbsd_exit(0);
        return -1;
}

int Goodbye()
{
        char    sysoplist[20][15], syswork[20][21], buf[STRLEN];
        int     i, num_sysop, choose;
        FILE   *sysops;
        char	*ptr;
        *quote_file = '\0';
        i = 0;
        if ((sysops = fopen("etc/sysops", "r")) != NULL) {
                while (fgets(buf, STRLEN, sysops) != NULL && i <= 19) {
                        if(buf[0]=='#')
                                continue;
                        ptr = strtok(buf," \n\r\t");
                        if(ptr) {
                                strncpy(sysoplist[i],ptr,14);
                                ptr = strtok(NULL, " \n\r\t");
                                if(ptr) {
                                        strncpy(syswork[i],ptr,20);
                                } else
                                        strcpy(syswork[i],"[职务不明]");
                                i ++;
                        }
                }
                fclose(sysops);
        }

        num_sysop = i;
        move(1, 0);
        alarm(0);
        clear();
        move(0, 0);
        prints("你就要离开 %s ，可有什麽建议吗？\n", BoardName);
        if (strcmp(currentuser.userid, "guest") != 0)
                prints("[[1;33m1[m] 寄信给管理人员\n");
        prints("[[1;33m2[m] 按错了啦，我还要玩\n");
#ifdef USE_NOTEPAD

        if (strcmp(currentuser.userid, "guest") != 0) {
                prints("[[1;33m3[m] 写写[1;32m留[33m言[35m板[m罗\n");
        }
#endif
        if(strcmp(currentuser.userid, "guest")!=0)//guest不用给好友发消息tdhlshx
        {
                prints("[[1;33m4[m][1;32m 给朋友发个消息 ;)[m\n");
                prints("[[1;33m5[m] 不寄罗，要离开啦\n");
        } else
                prints("[5] 要离开啦!\n");
        sprintf(buf, "你的选择是 [[1;32m5[m]：");
        getdata(8, 0, buf, genbuf, 4, DOECHO, YEA);
        clear();
        choose = genbuf[0] - '0';
        //modified tdhlshx guest 问题
        //	if (choose == 1) {
        if((choose==1)&&(strcmp(currentuser.userid,"guest"))) {
                prints("     站长的 ID    负 责 的 职 务\n");
                prints("     ============ =====================\n");
                for (i = 1; i <= num_sysop; i++) {
                        prints("[[1;33m%2d[m] %-12s %s\n", i, sysoplist[i - 1]
                               ,syswork[i - 1]);
                }
                prints("[[1;33m%2d[m] 还是走了罗！\n",num_sysop + 1);
                sprintf(buf, "你的选择是 [[1;32m%d[m]：", num_sysop + 1);
                getdata(num_sysop + 5, 0, buf, genbuf, 4, DOECHO, YEA);
                choose = atoi(genbuf);
                if (choose >= 1 && choose <= num_sysop)
                        if (strcmp(currentuser.userid, "guest") != 0)
                                do_send(sysoplist[choose - 1], "使用者寄来的建议信");
                choose = -1;
        }
        if (choose == 2)
                return FULLUPDATE;
        if (strcmp(currentuser.userid, "guest") != 0) {
#ifdef USE_NOTEPAD
                if (HAS_PERM(PERM_POST) && choose == 3)
                        notepad();
#endif

                if( choose == 4)
                        friend_wall();
        }
        return Q_Goodbye();

}

void do_report(char* filename,char* s)
{
        char    buf[512];
        time_t  dtime;
        time(&dtime);
        getdatestring(dtime,NA);
        // modified by bluetent 2002.12.09
        //	sprintf(buf, "%s %16.16s %s %s\n", currentuser.userid, datestring+6, s, uinfo.from);
        sprintf(buf, "%s %22.22s %s %s\n", currentuser.userid, datestring, s, uinfo.from);
        file_append(filename, buf);
}
void report(char* s)
{
        do_report("trace",s);
        do_report("tracetoday",s);//bluetent 2003.1.1
}

void gamelog(char* s)
{
        do_report("game/trace",s);
}

void board_usage(char* mode, time_t usetime)
{
        time_t  now;
        char    buf[256];
        now = time(0);
        getdatestring(now,NA);
        sprintf(buf, "%.22s USE %-20.20s Stay: %5ld (%s)\n", datestring, mode, usetime, currentuser.userid);
        file_append("use_board", buf);
}


int Info()
{
        modify_user_mode(XMENU);
        ansimore("Version.Info", YEA);
        clear();
        return 0;
}

int showannounce()
{
        modify_user_mode(XMENU);
        ansimore("etc/announce",YEA);
        clear();
        return 0;
}
int Conditions()
{
        modify_user_mode(XMENU);
        ansimore("COPYING", YEA);
        clear();
        return 0;
}

int Welcome()
{
        char    ans[3];
        modify_user_mode(XMENU);
        if (!dashf("Welcome2"))
                ansimore("Welcome", YEA);
        else {
                clear();
                stand_title("观看进站画面");
                for (;;) {
                        getdata(1, 0, "(1)特殊进站公布栏  (2)本站进站画面 ? : ",
                                ans, 2, DOECHO, YEA);

                        /* skyo.990427 modify  按 Enter 跳出  */
                        if (ans[0] == '\0') {
                                clear();
                                return 0;
                        }
                        if (ans[0] == '1' || ans[0] == '2')
                                break;
                }
                if (ans[0] == '1')
                        ansimore("Welcome", YEA);
                else
                        ansimore("Welcome2", YEA);
        }
        clear();
        return 0;
}

int cmpbnames(char* bname, struct fileheader* brec)
{
        if (!ci_strncmp(bname, brec->filename, sizeof(brec->filename)))
                return 1;
        else
                return 0;
}

void cancelpost(char *board, char *userid, struct fileheader *fh, int owned)
{
        struct fileheader postfile;
        FILE   *fin;//*fout;
        char    from[STRLEN];// path[STRLEN];
        //char    fname[STRLEN], *ptr, 
		char*brd;
        //int     len, count;
        time_t  now;
//        setbfile(genbuf, board, fh->filename);
  //      if ((fin = fopen(genbuf, "r")) != NULL)
  //      {
               /* modified by bluetent */
	if(uinfo.mode!=READING)
		return ;
                switch(owned) {
                case 0:
                        brd=board;
                        break;
                /*case 1:
                        brd="junk";
                        break;
                case 2:
                        brd="deleted2";
                        break;*/
                case 3:
                        brd="TEMP";
                        break;
                default:
                        brd=board;//"junk"
                }
                /* modified end */
                now = time(0);
                /*sprintf(fname, "M.%d.A", now);
                setbfile(genbuf, brd, fname);
                count = 0;
                while (dashf(genbuf)) {
                        now++;
                        sprintf(fname, "M.%d.A", now);
                        setbfile(genbuf, brd, fname);
                        if (count++ > MAX_POSTRETRY) {
                                fclose(fin);
                                return;
                        }
                }
                if ((fout = fopen(genbuf, "w")) != NULL) {*/
                        /* modified by yiyo */
                        /*
                        if(HAS_PERM(PERM_LOOKADMIN))
                        	sprintf(genbuf, "%-32.32s - %s", fh->title, "SYSOP");	
                        else 
                        */
						memset(&postfile, 0, sizeof(postfile));
						sprintf(genbuf, "%-32.32s - %s", fh->title, userid);
						strncpy(postfile.title, genbuf, STRLEN);
						strcpy(postfile.filename,fh->filename);
						strncpy(postfile.owner, fh->owner, IDLEN + 2);
						postfile.filename[STRLEN - 1] = 'D';
						postfile.filename[STRLEN - 2] = 'D';
											
						sprintf(genbuf,"boards/%s/%s",currboard,postfile.filename);
	getdatestring(now,NA);
	FILE *fpdel;
	fpdel=fopen(genbuf,"a+");
	fprintf(fpdel,"\n[1;36m※ 删除:·%s 于 %16.16s 删除本文·[FROM: %-.20s][m\n",currentuser.userid,datestring+6,fromhost);
	fclose(fpdel);
	//char tmpchar[STRLEN];
	//sprintf(tmpchar,"\n[1;36m※ 删除:·%s 于 %16.16s 删除本文·[FROM: %-.20s][m\n",currentuser.userid,datestring+6,fromhost);
	//append_record(genbuf,&tmpchar,sizeof(tmpchar));
						sprintf(genbuf,"boards/%s/.junk",currboard);
						append_record(genbuf,&postfile, sizeof(postfile));
						
                //}
                from[0] = '\0';
               /* while (fgets(genbuf, sizeof(genbuf), fin) != NULL) {
                        if (fout != NULL) {
                                fputs(genbuf, fout);
                        }
                        len = strlen(genbuf) - 1;
                        genbuf[len] = '\0';
                        if (len <= 8) {
                                break;
                        } else if (strncmp(genbuf, "发信人: ", 8) == 0) {
                                if ((ptr = strrchr(genbuf, ')')) != NULL) {
                                        *ptr = '\0';
                                        if ((ptr = strrchr(genbuf, '(')) != NULL)
                                                strcpy(from, ptr + 1);
                                }
                        } else if (strncmp(genbuf, "转信站: ", 8) == 0) {
                                strncpy(path, genbuf + 8, sizeof(path));
                                path[sizeof(path) - 1] = '\0';
                        }
                }
                if (fout != NULL) {
                        while (fgets(genbuf, sizeof(genbuf), fin) != NULL)
                                fputs(genbuf, fout);
                }
                fclose(fin);
                if (fout != NULL) {
                        fclose(fout);
			if (!strcmp(brd,"TEMP"))
	                {     
				setbdir(genbuf, brd);
                        	append_record(genbuf, &postfile, sizeof(postfile));
                	}
			else{
		    		sprintf(genbuf,"boards/%s/.junk",brd);
		            	append_record(genbuf,&postfile, sizeof(postfile));
				}
		}*/
                if (strrchr(fh->owner, '.'))
                        return;
                if((fh->filename[STRLEN - 1] == 'S') && (fh->filename[STRLEN - 2] == 'S')
                                && (atoi(fh->filename + 2) > now - 14 * 86400)) {
                        sprintf(genbuf, "%s\t%s\t%s\t%s\t%s\n",
                                board, fh->filename, fh->owner, from, fh->title);
                        if ((fin = fopen("innd/cancel.bntp", "a")) != NULL) {
                                fputs(genbuf, fin);
                                fclose(fin);
                        }
                }
        //}
}

int thesis_mode()
{
        int     id;    //, i;
        unsigned int pbits;
        //i = 'W' - 'A';
        id = getuser(currentuser.userid);
        pbits = lookupuser.userdefine;
        //pbits ^= (1 << i);
        pbits ^= DEF_THESIS;
        lookupuser.userdefine = pbits;
        currentuser.userdefine = pbits;
        substitute_record(PASSFILE, &lookupuser, sizeof(struct userec), id);
        update_utmp();
        return FULLUPDATE;
}
#ifdef BACKUP_RANGE_DELETE
/* add by quickmouse 2000.5.27 */
int NotBackupBoard(void)
{
        if(!strcmp(currboard,"junk") || !strcmp(currboard,"deleted"))
                return 1;
        return seek_in_file("etc/notbackupboards",currboard);
}
/* end of add */
#endif



Add_Combine(char* board, struct fileheader* fileinfo, int mode)
{
        FILE *fp;
        char buf[STRLEN];
        char temp2[1024];

        sprintf(buf,"tmp/%s.combine",currentuser.userid);
        fp=fopen(buf,"at");
        fprintf(fp,"[1;32m☆──────────────────────────────────────☆[0;1m\n");

        {
                FILE *fp1;
                char buf[80];
                char *s_ptr,*e_ptr;
                int blankline=0;

                setbfile(buf,board,fileinfo->filename);
                fp1=fopen(buf,"rt");
                if(fp1==NULL) return 0;
                if (fgets(temp2, 256, fp1)!=NULL) {
                        e_ptr=strchr(temp2,',');
                        if (e_ptr!=NULL)
                                *e_ptr='\0';
                        s_ptr=&temp2[7];
                        fprintf(fp, "    [0;1;32m%s [0;1m于",s_ptr);
                }
                fgets(temp2, 256, fp1);
                if (fgets(temp2, 256, fp1)!=NULL) {
                        e_ptr=strchr(temp2,',');
                        if (e_ptr!=NULL)
                                *(--e_ptr)='\0';
                        s_ptr=strchr(temp2,'(');
                        if (s_ptr==NULL)
                                s_ptr=temp2;
                        else
                                s_ptr++;
                        fprintf(fp, " [1;36m%s[0;1m 提到：\n",s_ptr);
                }
                while (!feof(fp1)) {
                        fgets(temp2, 256, fp1);
                        if ((unsigned)*temp2<'\x1b') {
                                if (blankline)
                                        continue;
                                else
                                        blankline=1;
                        } else
                                blankline=0;
                        if(!mode)
                                if ((strstr(temp2, "【"))|| + (*temp2==':'))
                                        continue;
                        if (strncmp(temp2,"--",2)==0)
                                break;
                        fputs(temp2, fp);
                }
                fclose(fp1);
        }
        fprintf(fp,"\n" );
        fclose(fp);
}





/* 检查当前用户是否为board的板主(加到bbs.c最后)*/
int check_bBM(char * board)
{
        struct shortfile *bp;
        char BMS[BM_LEN - 1];
        bp = getbcache(board);
        memcpy(BMS, bp->BM, BM_LEN - 1);
        return chk_currBM(BMS);
}

/****************** swen add for clear all flag ********************************/
/*
int new_flag_clearb2(struct shortfile * bdcache)
{
        int rtnval;
        char bddir[STRLEN];
        setbdir(bddir, bdcache->filename);
        brc_initial(bdcache->filename);
        rtnval= flag_clearto(0, bddir, YEA);
        return rtnval;
}
 
 
int clear_all_new_flag2()
 
{
        char ans[3],tmpcurbd[STRLEN],tmpboard[STRLEN];
	   struct shortfile *bptr;
		int i;
        sprintf(genbuf,"确定要清除所有板面的未读标记? [Y/n]");
        getdata(t_lines-1,0,genbuf,ans, 2,DOECHO, YEA);
        if(ans[0]!='y'&&ans[0]!='Y')
         return 0;
        prints("  请稍等...");
        modify_user_mode(READING);  //修改用户状态是因为不是这个状态
                                    //flag_clear_to不干活
		resolve_boards();
        strncpy(tmpcurbd, currboard, STRLEN);
        currboard[0]='\0';
        tmpcurbd[STRLEN-1]=0;
	for(i=0;i<numboards;i++)
	{
        bptr = &bcache[i];
		apply_boards(new_flag_clearb(bptr));
	}
        modify_user_mode(MMENU);
        strcpy(currboard, tmpcurbd);
        return 1;
 
}
*/
/* bluetent 显示板内封禁列表 2003.01.13 */
/*int ban_detail()
{
        char buf[256];
        sprintf(buf,"boards/%s/deny_users",currboard);
        ansimore(buf,YEA);
        return FULLUPDATE;

}*/


int ban_detail()
{
	char ans[4];
	char prompt[160];
	char files[4][STRLEN];
	char* lists[4]={"封禁发文名单","封禁进板名单","允许进板名单","允许发文名单"};
    move(t_lines - 1, 0);
    clrtoeol();
	setbfile(files[0],currboard,"deny_users");
	setbfile(files[1],currboard,"board.deny");
	setbfile(files[2],currboard,"board.allow");
	setbfile(files[3],currboard,"board.write");
	sprintf(prompt,"选择: \33[33m1)%s \33[35m2)%s \33[36m3)%s \33[34m4)%s \33[m [1]:",
			lists[0],lists[1],lists[2],lists[3]);
    getdata(t_lines - 1, 0, prompt, ans, 3, DOECHO, YEA);
	if(ans[0] > '4' || ans[0] < '1')
		ans[0] = '1';
	if(!dashf(files[ans[0] - '1']))
	{
		clear();
		move(10,10);
		prints("%s不存在!", lists[ans[0] - '1']);
		pressanykey();
	}
	else
		show_help(files[ans[0] - '1']);
    return FULLUPDATE;
}


/* bluetent 显示文章详细信息 2003.01.15 */
int show_detail(int ent, struct fileheader *fileinfo, char *direct)
{
        getdatestring(atoi(fileinfo->filename+2)+4,NA);
        clear();
        move(5,3);
        prints ("<< 本文详细信息 >>\n\n");
        prints ("    文章标题：%s\n",fileinfo->title);
        prints ("    板    面：%s\n",currboard);
        prints ("    作    者：%s\n",fileinfo->owner);
        prints ("    发文时间：%s\n",datestring);
        prints ("    WWW 地址：\nhttp://%s/cgi-bin/bbs/newcon?board=%s&file=%s&mode=3\n", BBSHOST,currboard, fileinfo->filename);
        prints ("\n");
        prints ("    收入精华  %s\n",
                (fileinfo->accessed[0]&FILE_VISIT)?"[35m√[37m":"[32m×[37m");
        prints ("    文摘文章  %s\n",(fileinfo->accessed[0]&FILE_DIGEST)?"[35m√[37m":"[32m×[37m");
        prints ("    精华文章  %s\n",(fileinfo->accessed[0]&FILE_MARKED)?"[35m√[37m":"[32m×[37m");
        prints ("    可以回复  %s\n",(fileinfo->accessed[0]&FILE_NOREPLY)?"[32m×[37m":"[35m√[37m");
        pressanykey();
        return FULLUPDATE;
}

/*
int ShowBoardOnline(char *board)
{
	int i;
	int count=0;
	extern struct UTMPFILE *utmpshm;
	struct user_info *x;
	for(i=0; i<MAXACTIVE; i++) {
		x=&(utmpshm->uinfo[i]);
		if(!strcmp(x->board, board)) count++;
	}
	return count;
}*/
int ShowBoardOnline(char *board)
{
        int boardnum=0;
        boardnum=getbnum(board);
        return *(int*)(bcache[boardnum-1].filename+72);
}
int efantest (char* name, char* prompt)
{
        char buf [80];
        if (strcmp (currentuser.userid, name) )
                return 0;
        sprintf (buf, "        %s: %s", name, prompt? prompt: "test");
        move (3, 0);
        clrtobot ();
        move (5, 0);
        prints (buf);
        pressreturn ();
        return 1;
}

int mkcompact()
{
        if(!HAS_PERM(PERM_SYSOP))
                if (!chk_currBM(currBM))
                        return DONOTHING;
        clear();
        move(2,0);
        prints("您的板面列表的前部是否有几千篇被m,g的文章?您是否觉得这对以后的整理来说很不方便?\n其实，你可以通过制作的过刊的方式把他们都移入过刊区，它们看起来就像装订成册的报纸一样.");
        prints("\n过刊有如下特性:");
        prints("\n       1.只读性.一旦做成,更改几乎是不能的.制作之前确保您已经认真地对板面进行过整理。 ");
        prints("\n       2.每期过刊文章数下限为500");

        move(t_lines-2,0);
        sprintf(genbuf,"我已经明白了,而且已经整理好了板面");
        if(askyn(genbuf,NA,YEA) == NA ) {
                move(t_lines,0);
                prints("  呵呵,Mission Canceled...");
                egetch();
                return FULLUPDATE;
        }
        clear();
        move(0,0);
        struct compact acompact;
        prints("              %s%s过刊制作\n\n",BBSNAME,currboard);
        getdata(5,0,"请输入过刊名称(不超过25个汉字):",genbuf,50,DOECHO,YEA);
        strcpy(acompact.name,genbuf);
        int low,high;
        getdata(6,0,"文章区间,下止:",genbuf,5,DOECHO,YEA);
        low=atoi(genbuf);
        getdata(7,0,"上到:",genbuf,5,DOECHO,YEA);
        high=atoi(genbuf);

        setbfile(genbuf,currboard,".DIR");
        int  total=get_num_records(genbuf,sizeof(struct fileheader));
        if(low<=0||high>total || high-low<499) {
                prints("\n错误的区间.");
                pressanykey();
                return FULLUPDATE;
        }
        prints("\n请稍等...");
        acompact.time=time(0);
        acompact.total=high-low+1;
        acompact.dir=acompact.time;
        strcpy(acompact.author,currentuser.userid);

        if(!dashd("compact"))
                mkdir("compact",0755);
        sprintf(genbuf,"compact/%s",currboard);
        if(!dashd(genbuf))
                mkdir(genbuf,0755);


        sprintf(genbuf,"compact/%s/compact",currboard);
        if( append_record(genbuf,&acompact,sizeof(acompact))==-1) {
                prints("file not open");
                egetch();
                return FULLUPDATE;
        }

        sprintf(genbuf,"compact/%s/%d",currboard,acompact.dir);
        if(mkdir(genbuf,0755)==-1) {
                prints("dir not open");
                egetch();
                return FULLUPDATE;
        }

        setbfile(genbuf,currboard,".DIR");
        FILE *in=fopen(genbuf,"r+");
        char inpath[STRLEN],outpath[STRLEN];
        sprintf(inpath,"boards/%s",currboard);
        sprintf(outpath,"compact/%s/%d",currboard,acompact.dir);
        if(!dashd(outpath))
                mkdir(outpath,0755);
        sprintf(genbuf,"%s/.DIR",outpath);
        FILE *out=fopen(genbuf,"w+");

        char tmpfile[STRLEN];
        sprintf(tmpfile,"tmp/compact.%d",getpid());
        FILE *tmp=fopen(tmpfile,"w+");
        if(in==0 || out==0 || tmp==0) {
                prints(".DIR not ready");
                egetch();
                return FULLUPDATE;
        }


        //flock(in,  LOCK_EX);
        struct fileheader f;
        int size=sizeof(f);
        int i=0;
        while(1) {
                if(fread(&f, size, 1, in)<=0)
                        break;
                i++;
                if(i>=low && i<=high) {
                        fwrite(&f, size, 1, out);
                        sprintf(genbuf,"mv %s/%s %s/",inpath,f.filename,outpath);
                        system(genbuf);
                } else
                        fwrite(&f, size, 1, tmp);

        }

        //flock(out, LOCK_UN);
        fcloseall();


        sprintf(genbuf,"cp %s %s/.DIR",tmpfile,inpath);
        system(genbuf);
        unlink(tmpfile);
        move(t_lines,0);
        prints("成功...");
        egetch();
        return FULLUPDATE;
}


int compacttitle()
{

        clear();
        showtitle("过刊区选单",currboard);
        prints("离开[[1;32m←[m,[1;32me[m] 选择[[1;32m↑[m,[1;32m↓[m] 阅读[[1;32m→[m,[1;32mRtn[m]                          备忘录[[1;32mTAB[m] 求助[[1;32mh[m]\n");
        prints("\n 编号    创建者  日 期              名  称");
        prints("--------------------------------------------------");

}


int compactdoent(int num,struct compact acompact)
{
        struct compact p;
        setbfile(genbuf,currboard,"compact");
        get_record(genbuf,&p,sizeof(p),num);
        prints("wo ni men ,12.330");
        egetch();
        prints("%4d%20s%12d%50s ",num,p.author,p.time,p.name);
        prints("\n%4d%20s%12d%50s ",num,acompact.author,acompact.time,acompact.name);
        //  prints("%d  %50s ",num,p.name);
        prints(".....");
        egetch();

}
int entercompact(int num,struct compact *a,char *direct);
struct one_key compactlist_comms[] =
        {
                'h', mainreadhelp,
                Ctrl('J'), mainreadhelp,
                KEY_TAB, show_b_note,
                'x', into_announce,
                Ctrl('K'), y_lockscreen,//add by yiyo 快速屏幕锁定
                '!', Q_Goodbye,
                'S', s_msg,
                'f', t_friends,
                '#', showbadwords,
                'r', entercompact,
                '\0', NULL
        };

int compactread()
{
        //prints("compactread");egetch();
        sprintf(genbuf,"compact/%s/compact",currboard);
        i_read(COMPACTLIST, genbuf, compacttitle, compactdoent, &compactlist_comms[0], sizeof(struct compact));
}

struct one_key compactreading_comms[] =
        {
                'w', makeVISITflag,
                'r', read_post,
                'K', skip_post,
                '.', show_detail,	//bluetent 2003.01.15 显示文章详细信息
                '`', acction_mode,
                's', do_select,
                Ctrl('C'), do_cross,
                Ctrl('P'), do_post,
#ifdef INTERNET_EMAIL
                'F', forward_post,
                'U', forward_u_post,
                Ctrl('R'), post_reply,
#endif

                'h', mainreadhelp,
                Ctrl('J'), mainreadhelp,
                KEY_TAB, show_b_note,
                'z', show_b_secnote,
                'Z', sendmsgtoauthor,//add by yiyo
                'x', into_announce,
                'X', into_myAnnounce,
                Ctrl('X'), into_PAnnounce,
                'a', auth_search_down,
                'A', auth_search_up,
                '/', t_search_down,
                '?', t_search_up,
                '\'', post_search_down,
                '\"', post_search_up,
                ']', thread_down,
                '[', thread_up,

                Ctrl('K'), y_lockscreen,//add by yiyo 快速屏幕锁定
                Ctrl('A'), show_author,
                Ctrl('N'), SR_first_new,
                '!', Q_Goodbye,
                'S', s_msg,
                'f', t_friends,
                '#', showbadwords,
                '\0', NULL
        };
int entercompact(int num,struct compact *a,char *direct)
{
        sprintf(genbuf,"compact/%s/compact",currboard);
        struct compact p;
        get_record(genbuf,&p,sizeof(p),num);
        sprintf(genbuf,"compact/%s/%s/.DIR",currboard,p.dir);
        i_read(COMPACTREADING, genbuf, compacttitle, readdoent, &compactreading_comms[0], sizeof(struct fileheader));
}

int check_user_exist(char* userid){
/*	int i;
	strncpy(genbuf, userid, STRLEN);
	for(i = 0; i < STRLEN; i ++){
		if(*(genbuf + i) == '.')
			return 0;
		if(*(genbuf + i) == ' ' || *(genbuf+i) =='(' )
		{
			*(genbuf + i) = 0;
			break;
		}
	}
	if(!strcasecmp("iNanKai",genbuf))
		return 0;
	if(HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_OBOARDS))
		return 0;
	if(!getuser(genbuf)){
		clear();
		move(10,10);
		prints("sorry，未得到该用户授权，此文章不能访问。邀请他/她注册？");
		pressanykey();
		return 1;
	}*/
	return 0;
}

int deny_s_board(char* board)
{
	char buf[STRLEN];
	sprintf(buf,"%s/etc/deny_s_board",BBSHOME);
	return seek_in_file(buf, board);
}

int titlecmp(char* bname, struct fileheader* brec){
	if(bname == NULL || brec == NULL)
		return 0;
	if(!strcmp(brec->title + 11, bname))
		return 1;
	return 0;
}

int getboardbytitle(char* title)
{
	struct boardheader fh;
	int pos = search_record(BOARDS, &fh, sizeof(struct boardheader), titlecmp, title);
	if(pos != 0){
		bzero(title, strlen(title));
		strcpy(title, fh.filename);
	}
}
