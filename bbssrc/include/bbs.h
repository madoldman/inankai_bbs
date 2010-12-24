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
	Firebird Bulletin Board System  -- Version 3.0 series
	Copyright (C) 1999, Peng Piaw Foong, Harimau.bbs@MSIA.pine.ncu.edu.tw
	Edward Ping-Da Chuang, edwardc.bbs@bbs.seed.net.tw
						
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
$Id: bbs.h,v 1.3 2010-07-05 04:58:43 madoldman Exp $
*/

#ifndef  _BBS_H_
#define _BBS_H_

#ifndef BBSIRC

/* Global includes, needed in most every source file... */
#ifdef FCGI
#include <fcgi_stdio.h>
#else
#include <stdio.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#ifndef BSD44
  #include <stdlib.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <uuid/uuid.h>
#include <dirent.h>
#include "config.h"             /* User-configurable stuff */

#include "glossary.h"
#include "functions.h"	/* you can enable functions that you want */
#include "permissions.h"

#ifndef LOCK_EX
  #define LOCK_EX         2       /* exclusive lock */
  #define LOCK_UN         8       /* unlock */
#endif

#ifdef XINU
extern int errno ;
#endif

#define randomize() srand((unsigned)time(NULL))

#define YEA (1)        /* Booleans  (Yep, for true and false) */
#define NA  (0)

#define DOECHO (1)     /* Flags to getdata input function */
#define NOECHO (0)

//#define MAX_MSG_SIZE 1024   //add for msg
char *bfile() ;

extern FILE *ufp ;     /* External variable declarations */
extern long ti ;

#endif /* BBSIRC */
#define ci_strncmp strncasecmp
#define ci_strcmp strcasecmp

#define MAXFRIENDS (300)
#define MAXREJECTS (32)
#define NUMPERMS   (31)
//#define REG_EXPIRED         360    /* �������ȷ������ */
#define MAXGUEST			1000	   /* ��� guest �ʺ���վ���� */
#define MAXGUESTPERIP		  	10	   /* ÿ�� IP ͬʱ��վ id �� */
#define FILE_BUFSIZE        200    /* max. length of a file in SHM*/
#define FILE_MAXLINE         25    /* max. line of a file in SHM */
#define MAX_WELCOME          15    /* ��ӭ������ */
#define MAX_GOODBYE          15    /* ��վ������ */
#define MAX_ISSUE            15    /* ����վ������ */
#define MAX_ENDLINE			 60 /* ������ҳ���� bluetent 2002.1.2*/
#define MAX_DIGEST         3000    /* �����ժ�� */
#define MAX_POSTRETRY       100
#define PMAXRMD		    4	    /*����Ƽ���*/

#define MORE_BUFSIZE       4096

#ifdef BIGGER_MOVIE
  #define MAXMOVIE		8
#else
  #define MAXMOVIE		6    /* lines of  activity board  */
#endif

#define ACBOARD_BUFSIZE     250 /* max. length of each line for activity board*/
#define ACBOARD_MAXLINE     300 /* max. lines of  activity board  */
#define STRLEN               80    /* Length of most string data */
#define NAMELEN              40    /* Length of username/realname */
#define IDLEN                12    /* Length of userids */
#define INDEXNAME_VIRTUAL   ".vbindex"
#define BOARDCFG_VIRTUAL    ".vbconfig"
#define FETCHCACHE_VIRTUAL    ".vbcache"

#ifdef MD5		  	   /* MD5 cryptographic support */
  #define ENCPASSLEN         35
#else
  #define ENCPASSLEN         14  /* Length of encrypted passwd field */
#endif

#define PASSLEN		 	14    /* User's password length (13 chars) */

#ifdef CODE_VALID
  #define RNDPASSLEN             10        /* ������֤�İ��볤�� (���˷�Χ 4~10)*/
#endif

#define MAXGOPHERITEMS     9999    /* max of gopher items */
#define PASSFILE     ".PASSWDS"    /* Name of file User records stored in */
#define ULIST_BASE   ".UTMP"       /* Names of users currently on line */
extern  char ULIST[];

#ifndef BBSIRC

#define FLUSH       ".PASSFLUSH"   /* Stores date for user cache flushing */
#define BOARDS      ".BOARDS"      /* File containing list of boards */
#define DOT_DIR     ".DIR"         /* Name of Directory file info */
#define THREAD_DIR  ".THREAD"      /* Name of Thread file info */
#define DIGEST_DIR  ".DIGEST"      /* Name of Digest file info */
#define MARKED_DIR  ".MARKEDDIR"
#define AUTHOR_DIR  ".AUTHORDIR"
#define KEY_DIR	    ".KEYDIR"
#define ANNOUNCE_MAIL_DIR "mail/announce"
#define UUIDS       ".UUIDS"

#define QUIT 0x666               /* Return value to abort recursive functions */

#define FILE_READ  0x1        /* Ownership flags used in fileheader structure */
#define FILE_OWND  0x2        /* accessed array */
#define FILE_VISIT 0x4
#define FILE_MARKED 0x8
#define FILE_DIGEST 0x10      /* Digest Mode*/
#define FILE_FORWARDED 0x20  /* Undelete file */
#define MAIL_REPLY 0x20 	/* Mail Reply */
#define FILE_NOREPLY 0x40      /* No Allow Replay */
#define FILE_DELETED 0X80
#define FILE_UPLOAD 0X100	/* If the article has attach, add by bluetent 2002.12.23 */
#define FILE_ONTOP 0x2		/* Efan: ���õ����µ�����λ	*/
#define FILE_TIP 0X4		/* Efan: �õ�����(��ʾΪ[�õ�])	*/
#define FILE_STYLE1 0X8
#define FILE_STYLE2 0X10 
#define FILE_STYLE3 0X20
#define FILE_STYLE4 0X40
#define FILE_STYLE5 0X80      /*loveni:�����õ���ʾ*/

#define VOTE_FLAG    0x1
#define NOZAP_FLAG   0x2
#define OUT_FLAG     0x4
#define ANONY_FLAG   0x8
#define NOREPLY_FLAG 0x10 /* No reply board */
#define ZONE_FLAG    0x40
#define CHILDBOARD_FLAG	0x80
#define ZAPPED  0x1  
         /* For boards...tells if board is Zapped */
/* for flag2	 by loveni */
#define HIDE_FLAG 0x1   		/*���ֲ������Ƿ����� */
#define JUNK_FLAG 0x2  			/* �Ƿ��������� */
#define NOTOPTEN_FLAG 0x4			/* �Ƿ����ʮ��ͳ�� */
#define NOFILTER_FLAG 0x8		/* �Ƿ����������	 */  
#define TEMPLATE_FLAG 0x10		/* �Ƿ�ǿ��ģ�巢��	  */		
#define VBOARD_FLAG 0x20	/* ������� */

/* these are flags in userec.flags[0] */
#define PAGER_FLAG   0x1   /* true if pager was OFF last session */
#define CLOAK_FLAG   0x2   /* true if cloak was ON last session */
#define SIG_FLAG     0x8   /* true if sig was turned OFF last session */
#define BRDSORT_FLAG 0x20  /* true if the boards sorted alphabetical */
#define CURSOR_FLAG  0x80  /* true if the cursor mode open *//* ���ڲ����� */
#define ACTIVE_BOARD 0x200 /* true if user toggled active movie board on */

#define MSG_SEPERATOR   "\
������������������������������������������������������������������������������"
#define MULTI_LOGINS	3	/* ͬʱ����վ ID �� */
#define DEF_VALUE		"��� ��ӭ�����Ͽ��˲���վ! ���"
#define DEF_FILE		"etc/whatdate"
#define MAX_EMAIL_AUTH_SUPPORT 3 /*email auth max count */
#define ALL_PAGER       0x1
#define FRIEND_PAGER    0x2
#define ALLMSG_PAGER    0x4
#define FRIENDMSG_PAGER 0x8

#define SHIFTMODE(usernum,mode) ((usernum<MAXUSERS)?mode:mode<<4)

#define SETFILEMODE(array,usernum,mode) \
     (array[usernum%MAXUSERS] |= ((usernum<MAXUSERS)?mode:mode<<4))

#define CLRFILEMODE(array,usernum,mode) \
          (array[usernum%MAXUSERS] &= ((usernum<MAXUSERS)?~mode:~(mode<<4)))

#define CHECKFILEMODE(array,usernum,mode) \
       (array[usernum%MAXUSERS] & ((usernum<MAXUSERS)?mode:mode<<4))
#define USERIDSIZE (16)
#define USERNAMESZ (24)
#define TERMTYPESZ (10)
/* END */

#endif /* BBSIRC */

#ifndef NO_STRUCT_H

/* ģ�����*/
#define MQ  20
#define MP  12
#define AS  2048
#include "struct.h"

#endif

#include "modes.h"              /* The list of valid user modes */

#define DONOTHING       0       /* Read menu command return states */
#define FULLUPDATE      1       /* Entire screen was destroyed in this oper*/
#define PARTUPDATE      2       /* Only the top three lines were destroyed */
#define DOQUIT          3       /* Exit read menu was executed */
#define NEWDIRECT       4       /* Directory has changed, re-read files */
#define READ_NEXT       5       /* Direct read next file */
#define READ_PREV       6       /* Direct read prev file */
#define GOTO_NEXT       7       /* Move cursor to next */
#define DIRCHANGED      8       /* Index file was changed */
#define MODECHANGED     9       /* ... */

#define I_TIMEOUT   (-2)         /* Used for the getchar routine select call */
#define I_OTHERDATA (-333)       /* interface, (-3) will conflict with chinese */

#define SCREEN_SIZE (23)         /* Used by read menu  */

extern  scrint;               /* Set when screen has been initialized */
/* Used by namecomplete *only* */

extern int digestmode;            /*To control Digestmode*/

#ifndef NO_STRUCT_H		/* NO_STRUCT_H */

extern struct userec currentuser ;/*  user structure is loaded from passwd */
/*  file at logon, and remains for the   */
/*  entire session */

extern struct user_info uinfo ;   /* Ditto above...utmp entry is stored here
                                     and written back to the utmp file when
                                     necessary (pretty darn often). */
extern int usernum ;      /* Index into passwds file user record */
extern int utmpent ;      /* Index into this users utmp file entry */
extern int count_friends,count_users; /*Add by SmallPig for count users and friends*/

extern int t_lines, t_columns;    /* Screen size / width */
extern struct userec lookupuser ; /* Used when searching for other user info */

#endif					/* NO_STRUCT_H */
extern char someoneDIR[];
extern int         nettyNN;
extern char netty_path[] ;
extern char netty_board[] ;
extern char currboard[] ;       /* name of currently selected board */
extern char currBM[] ;          /* BM of currently selected board */

extern int selboard ;           /* THis flag is true if above is active */

extern char genbuf[1024] ;      /* generally used global buffer */

extern jmp_buf byebye ;        /* Used for exception condition like I/O error*/

//commands�ṹδ�ҵ��������⼸������Ҳδʹ�ã�
//ע�͵��⼸�����gcc-4.0�±���ͨ��
//extern struct commands cmdlist[] ; /* main menu command list */
//extern struct commands xyzlist[] ;   /* These are command lists for all the */
//extern struct commands talklist[] ;  /* sub-menus */
//extern struct commands maillist[] ;
//extern struct commands dellist[] ;
//extern struct commands maintlist[] ;

extern char save_title[] ;    /* These are used by the editor when inserting */
extern char save_filename[] ; /* header information */
extern int in_mail ;
extern int dumb_term ;
extern int showansi;
extern time_t uptime;		  /* save user last key-in time, up every 1min */

#define Min(a,b) ((a<b)?a:b)
#define Max(a,b) ((a>b)?a:b)

/*SREAD Define*/
#define SR_BMBASE       (10)
#define SR_BMDEL	(11)
#define SR_BMMARK       (12)
#define SR_BMDIGEST     (13)
#define SR_BMIMPORT     (14)
#define SR_BMTMP        (15)
#define SR_BMUNDERLINE (16)
/*SREAD Define*/

#ifndef EXTEND_KEY
#define EXTEND_KEY
#define KEY_TAB         9
#define KEY_ESC         27
#define KEY_UP          0x0101
#define KEY_DOWN        0x0102
#define KEY_RIGHT       0x0103
#define KEY_LEFT        0x0104
#define KEY_HOME        0x0201
#define KEY_INS         0x0202
#define KEY_DEL         0x0203
#define KEY_END         0x0204
#define KEY_PGUP        0x0205
#define KEY_PGDN        0x0206
#endif

/* edwardc.990706 move shmkeys from sysconf.ini */

struct _shmkey
{
	char key[16];
	int value;
};
#ifdef AddWaterONLY
const static struct _shmkey shmkeys[] =
    {
	    { "BCACHE_SHMKEY",	50000
	    },
	    { "UCACHE_SHMKEY",	50010 },
	    { "UTMP_SHMKEY",	50020 },
	    { "ACBOARD_SHMKEY",	50030 },
	    { "ISSUE_SHMKEY",	50040 },
	    { "GOODBYE_SHMKEY",	50050 },
	    { "WELCOME_SHMKEY", 	50060 },
	    { "STAT_SHMKEY",	50070 },
	    { "CONV_SHMKEY",	50080 },
	    {    0,   0 }
    };
#else
const static struct _shmkey shmkeys[] =
    {
	    { "BCACHE_SHMKEY",      30000
	    },
	    { "UCACHE_SHMKEY",      30010 },
	    { "UTMP_SHMKEY",        30020 },
	    { "ACBOARD_SHMKEY",     30030 },
	    { "ISSUE_SHMKEY",       30040 },
	    { "GOODBYE_SHMKEY",     30050 },
	    { "WELCOME_SHMKEY",     30060 },
	    { "STAT_SHMKEY",        30070 },
	    { "CONV_SHMKEY",        30080 },
	    { "ENDLINE1_SHMKEY",	30090 },
	    { "KILLER_SHMKEY",      30100 },
	    { "GAMEROOM_SHMKEY",    30110 },
	    {    0,   0 }
    };
#endif
#define Ctrl(c)         ( c & 037 )
#define isprint2(c)     ( (c & 0x80) || isprint(c) )

#ifdef  SYSV
#define bzero(tgt, len)         memset( tgt, 0, len )
#define bcopy(src, tgt, len)    memcpy( tgt, src, len)

#define usleep(usec)            {               \
    struct timeval t;                           \
    t.tv_sec = usec / 1000000;                  \
    t.tv_usec = usec % 1000000;                 \
    select( 0, NULL, NULL, NULL, &t);           \
}

#endif  /* SYSV */

/* =============== ANSI EDIT ================== */
#define   ANSI_RESET    "\033[0m"
#define   ANSI_REVERSE  "\033[7m\033[4m"
extern    int  editansi;
extern    int  KEY_ESC_arg;
/* ============================================ */

/* Added by deardragon 1999.11.2 */
extern char	datestring[];
/* Added End. */

/* Efan: �����õ� */
#define	MAX_BRD_ONTOP	6
#define GOOD_BRC_NUM  100
#endif /* of _BBS_H_ */

