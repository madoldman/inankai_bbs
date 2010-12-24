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
$Id: struct.h,v 1.3 2010-07-05 04:58:44 madoldman Exp $
*/

/* Note the protocol field is not inside an #ifdef FILES...
   this is a waste but allows you to add/remove UL/DL support without
   rebuilding the PASSWDS file (and it's only a lil ole int anyway).
*/
typedef unsigned char uschar;
struct userec
{                  /* Structure used to hold information in */
	char            userid[IDLEN+2];   /* PASSFILE */
	time_t          firstlogin;
	char            lasthost[16];
	unsigned int    numlogins;
	unsigned int    numposts;
	unsigned int    nummedals; /* 奖章数 */
#ifdef ALLOWGAME

	unsigned int    money;   /* 存款 */
	unsigned int    bet;     /* 贷款 */
	time_t		dateforbet;
#endif

	char            flags[2];
#ifdef ENCPASSLEN

	char            passwd[ENCPASSLEN];
#else

	char            passwd[PASSLEN];
#endif

	char            username[NAMELEN];
	char            ident[NAMELEN];
	char            termtype[16];
	char            reginfo[STRLEN-16];
	unsigned int    userlevel;
	time_t          lastlogin;
	time_t          lastlogout;/* 最近离线时间 */
	time_t          stay;
	char            realname[NAMELEN];
	char            address[STRLEN];
	char            email[STRLEN-12];
	unsigned int    nummails;
	time_t          lastjustify;
	char            gender;
	unsigned char   birthyear;
	unsigned char   birthmonth;
	unsigned char   birthday;
	int             signature;
	unsigned int    userdefine;
	time_t          notedate;
	int             noteline;
};

struct user_info
{               /* Structure used in UTMP file */
	int     active;          /* When allocated this field is true */
	int     uid;             /* Used to find user name in passwd file */
	int     pid;             /* kill() to notify user of talk request */
	int     invisible;       /* Used by cloaking function in Xyz menu */
	int     sockactive;      /* Used to coordinate talk requests */
	int     sockaddr;        /* ... */
	int     destuid;         /* talk uses this to identify who called */
	int     mode;            /* UL/DL, Talk Mode, Chat Mode, ... */
	int     pager;           /* pager toggle, YEA, or NA */
	int     in_chat;         /* for in_chat commands   */
	int     fnum;            /* number of friends */
	int     ext_idle;        /* has extended idle time, YEA or NA */
	char    chatid[ 10 ];    /* chat id, if in chat mode */
	char    from[ 60 ];      /* machine name the user called in from */
	int	lastrecord;	/* used to record the last time the user online time is calculate */
	int utmpkey;             /* for njuwebbs */
	char	board[ 80 ];	 /* for counting users in a certain board */
#ifndef BBSD

	char    tty[ 20 ];       /* tty port */
#else

	time_t	idle_time;			 /* to keep idle time */
#endif

	char    userid[ 20 ];
	char    realname[ 20 ];
	char    username[NAMELEN];
	unsigned short  friend[MAXFRIENDS];
	unsigned short  reject[MAXREJECTS];
};

struct user_enter
{
	char   loginfrom[20];
	char   logintime[30];
};

struct override
{
	char id[13];
	char exp[40];
};


#define BM_LEN 60

struct boardheader
{             /* This structure is used to hold data in */
	char filename[STRLEN];   /* the BOARDS files */
	char owner[STRLEN - BM_LEN];
	char BM[ BM_LEN - 1];
	char flag;
	char title[STRLEN ];
	unsigned level;
	unsigned char flag2;//loveni:辅助属性
	unsigned char accessed[ 11 ];
};

struct fileheader
{             /* This structure is used to hold data in */
	char filename[STRLEN];     /* the DIR files */
	char owner[STRLEN];
	char title[STRLEN];
	unsigned level;
	unsigned char accessed[ 12 ];   /* struct size = 256 bytes */
};

struct fileheader_v //same as fileheader
{             /* This structure is used to hold data in */
	char filename[STRLEN / 2];     /* the DIR files */
	char board[STRLEN - STRLEN / 2]; //vb
	char owner[STRLEN];
	char title[STRLEN];
	unsigned level;
	unsigned char accessed[ 12 ];   /* struct size = 256 bytes */
};

struct xfileheader //512B
{             /* This structure is used to hold data in */
	char filename[STRLEN];     /* the DIR files */
	char owner[STRLEN];
	char title[STRLEN];
	unsigned level;
	unsigned char accessed[ 12 ];   /* struct size = 256 bytes */
	char board[STRLEN];
	char preserved[STRLEN];
	unsigned char flag [STRLEN];
	unsigned int flag2 [4];
};

struct shortfile
{               /* used for caching files and boards */
	char filename[STRLEN];      /* also will do for mail directories */
	char owner[STRLEN - BM_LEN];
	char BM[ BM_LEN - 1];
	char flag;
	char title[STRLEN];
	unsigned level;
	unsigned char flag2;//loveni:辅助属性
};

struct one_key
{                  /* Used to pass commands to the readmenu */
	int key ;
	int (*fptr)() ;
} ;


#define USHM_SIZE       (MAXACTIVE + 10)
struct UTMPFILE
{
	struct user_info    uinfo[ USHM_SIZE ];
	time_t              uptime;
	unsigned short      usersum;
	int                 max_login_num;
	//long int		visit_total;
};

struct BCACHE
{
	struct shortfile    bcache[ MAXBOARD ];
	int         number;
	time_t      uptime;
	time_t      pollvote;
	time_t		fresh_date;
	char		date[60];
	time_t		friendbook;		/* friendbook mtime */
};

struct UCACHE
{
	char        userid[ MAXUSERS ][ IDLEN + 1 ];
	int         number;
	time_t      uptime;
	//    int		board;//add by bluetent for nju0.95
};

struct postheader
{
	char title[STRLEN];
	char ds[40];
	int reply_mode;
	char include_mode;
	int chk_anony;
	int postboard;
};

struct favdigest
{
	char path[256];
	char title[80];
};
struct pattern
{
        char file[16];
        char name[STRLEN];
        char author[IDLEN+1];
        time_t time;
        int question;
        char buf[11];      //sizeof  128
};
struct compact
{
        int total;
        char name[STRLEN];
        char author[IDLEN+1];
        time_t time;
        int dir;
        char buf[3];
};
struct onequestion
{
     char con[STRLEN];
     int size;
};
struct question
{
        char title1[STRLEN];
        char title2[STRLEN];
        struct onequestion q[MQ];
};

//add for telnet下设置web推荐
struct rmd
{
    time_t  rmdtime;
    time_t  psttime;
    char en[STRLEN];
    char cn[STRLEN];
    char owner[16];
    char rmder[16];
    char board[STRLEN];
    int sign;
    char buf[16];
};

struct uuid_st
{
	char uuid[33];
	char owner[IDLEN + 1];
	char reger[IDLEN + 1];
}

