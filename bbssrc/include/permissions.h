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
$Id: permissions.h,v 1.2 2008-12-04 12:32:47 madoldman Exp $
*/

/* These are all the permissions available in the BBS */

#define PERM_BASIC      000001
#define PERM_CHAT       000002
#define PERM_PAGE       000004
#define PERM_POST       000010
#define PERM_LOGINOK    000020
#define PERM_DENYPOST   000040
#define PERM_CLOAK      000100
#define PERM_SEECLOAK   000200
#define PERM_XEMPT      000400
#define PERM_WELCOME    001000
#define PERM_BOARDS     002000
#define PERM_ACCOUNTS   004000
#define PERM_CHATCLOAK  010000
#define PERM_OVOTE      020000
#define PERM_SYSOP      040000   /* ϵͳά��Ȩ��*/
#define PERM_POSTMASK  0100000
#define PERM_ANNOUNCE  0200000
#define PERM_OBOARDS   0400000
#define PERM_ACBOARD   01000000
#define PERM_NOZAP     02000000
#define PERM_FORCEPAGE 04000000
#define PERM_EXT_IDLE  010000000
#define PERM_MESSAGE   020000000
#define PERM_ACHATROOM 040000000
#define PERM_LARGEMAIL 0100000000
#define PERM_MAILINFINITY 0200000000
#define PERM_SEEUSERIP 0400000000
#define PERM_PERSONAL  01000000000
#define PERM_SUICIDE    02000000000
#define PERM_RECLUSION  04000000000
#define PERM_LOOKADMIN  010000000000  //add by bluetent 2002.11.1 У����Ȩ�ʺ�

/* This is the default permission granted to all new accounts. */
#define PERM_DEFAULT    (PERM_BASIC | PERM_CHAT | PERM_PAGE | \
                         PERM_POST | PERM_LOGINOK|PERM_MESSAGE)

/* These permissions are bitwise ORs of the basic bits. They work that way
   too. For example, anyone with PERM_SYSOP or PERM_OBOARDS or both has
   PERM_BLEVELS. */

#define PERM_ADMINMENU  (PERM_ACCOUNTS|PERM_SYSOP|PERM_OVOTE|PERM_OBOARDS|PERM_WELCOME|PERM_ACBOARD|PERM_DENYPOST|PERM_LOOKADMIN)
#define PERM_MULTILOG   PERM_SYSOP
#define PERM_ESYSFILE   (PERM_SYSOP | PERM_WELCOME | PERM_ACBOARD | PERM_ACCOUNTS|PERM_OVOTE)
#define PERM_LOGINCLOAK (PERM_SYSOP | PERM_ACCOUNTS | PERM_WELCOME | PERM_CLOAK)
#define PERM_SEEULEVELS (PERM_SYSOP | PERM_CHATCLOAK | PERM_SEECLOAK)
#define PERM_BLEVELS    (PERM_SYSOP | PERM_OBOARDS)
#define PERM_MARKPOST   (PERM_SYSOP | PERM_BOARDS)
#define PERM_UCLEAN     (PERM_SYSOP | PERM_ACCOUNTS)
#define PERM_NOTIMEOUT  PERM_SYSOP

#define PERM_READMAIL   PERM_BASIC
#define PERM_VOTE       PERM_BASIC

#define PERM_CHANGEINFO (PERM_SYSOP | PERM_OBOARDS | PERM_ACCOUNTS)
#define PERM_LOOKINFO   (PERM_SYSOP | PERM_OBOARDS | PERM_ACCOUNTS)  //add by yiyo for У�������ʺ�
#define PERM_EDITWORDS  (PERM_SYSOP | PERM_OBOARDS | PERM_ACCOUNTS | PERM_LOOKADMIN)   //�ؼ��ֱ༭��������У�������ѯ�û�����
/* These are used only in Internet Mail Forwarding */
/* You may want to be more restrictive than the default, especially for an
   open access BBS. */

#define PERM_SETADDR    PERM_POST      /* to set address for forwarding */
#define PERM_FORWARD    PERM_BASIC     /* to do the forwarding */

/* Don't mess with this. */
#define HAS_PERM(x)     ((x)?currentuser.userlevel&(x):1)
#define DEFINE(x)     ((x)?currentuser.userdefine&(x):1)
#define HAS_DEFINE(x,y)	((y)?x&(y):1)
/* HAS_DEFINE(userdefine, flag) */

#ifndef EXTERN
extern char *permstrings[];
#else

/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
depending on how/if you use them. */
/* skyo.0507 modify ��������� PERM ����� menu.ini ���գ� */
char *permstrings[] = {
                          "����Ȩ��       (PERM_BASIC)",  /* PERM_BASIC */
                          "����������     (CHAT)",        /* PERM_CHAT */
                          "������������   (PAGE)",        /* PERM_PAGE */
                          "��������       (POST)",        /* PERM_POST */
                          "ʹ����������ȷ (LOGINOK)",     /* PERM_LOGINOK */
                          "�޸Ļ���Ȩ��   (DENYPOST)",    /* PERM_DENYPOST */
                          "������         (CLOAK)",	/* PERM_CLOAK */
                          "����������     (SEECLOAK)",    /* PERM_SEECLOAK */
                          "�ʺ����ñ���   (XEMPT)",       /* PERM_XEMPT */
                          "�༭��վ����   (WELCOME)",     /* PERM_WELCOME */
                          "����           (BOARDS)",      /* PERM_BOARDS */
                          "�ʺŹ���Ա     (ACCOUNTS)",    /* PERM_ACCOUNTS */
                          "��վ������     (CHATCLOAK)",   /* PERM_CHATCLOAK */
                          "ͶƱ����Ա     (OVOTE)",       /* PERM_OVOTE */
                          "ϵͳά������Ա (SYSOP)",       /* PERM_SYSOP */
                          "Read/Post ���� (POSTMASK)",    /* PERM_POSTMASK */
                          "�������ܹ�     (ANNOUNCE)",    /* PERM_ANNOUNCE*/
                          "�������ܹ�     (OBOARDS)",     /* PERM_OBOARDS*/
                          "������ܹ�   (ACBOARD)",     /* PERM_ACBOARD*/
                          "���� ZAP ������(NOZAP)", 	/* PERM_NOZAP*/
                          "ǿ�ƺ���       (FORCEPAGE)",   /* PERM_FORCEPAGE*/
                          "�ӳ�����ʱ��   (EXT_IDLE)",    /* PERM_EXT_IDLE*/
                          "�����˷���Ϣ   (MESSAGE)",     /* PERM_MESSAGE*/
                          "�����ҹ���Ա   (ACHATROOM)",   /* PERM_ACHATROOM*/
                          "������         (LARGEMAIL)",   /* PERM_LARGEMAIL*/
                          "����������     (MAILINFINITY)",/* PERM_MAILINFINITY*/
                          "���û�IP       (SEEUSERIP)",   /* PERM_SEEUSERIP*/
                          "��������ļ�   (PERSONAL)",    /* PERM_PERSONAL*/
                          "��ɱ������     (SUICIDE)",     /* PERM_SUICIDE*/
                          "������԰       (RECLUSION)",   /* PERM_RECLUSION*/
                          "У������       (LOOKADMIN)",	/* PERM_LOOKADMIN *///add by bluetent 2002.11.1 У����Ȩ�ʺ�
                          NULL
                      };
#endif

#define DEF_FRIENDCALL   0x00000001
#define DEF_ALLMSG       0x00000002
#define DEF_FRIENDMSG    0x00000004
#define DEF_SOUNDMSG     0x00000008
//#define DEF_COLOR        0x00000010
#define DEF_NOTSHOWMSG   0x00000010
#define DEF_ACBOARD      0x00000020
#define DEF_ENDLINE      0x00000040
#define DEF_EDITMSG      0x00000080
#define DEF_NOTMSGFRIEND 0x00000100
#define DEF_NORMALSCR    0x00000200
#define DEF_NEWPOST      0x00000400
#define DEF_CIRCLE       0x00000800
#define DEF_FIRSTNEW     0x00001000
#define DEF_LOGFRIEND    0x00002000
#define DEF_LOGINFROM    0x00004000
#define DEF_NOTEPAD      0x00008000
#define DEF_NOLOGINSEND  0x00010000
#define DEF_THESIS	 0x00020000	/* youzi */
#define DEF_MSGGETKEY    0x00040000
#define DEF_GRAPH        0x00080000
#define DEF_TOP10        0x00100000
#define DEF_RANDSIGN	 0x00200000
#define DEF_S_HOROSCOPE  0x00400000
#define DEF_FRIEND_INFO	 0x00800000
#define DEF_NOT_N_MASK   0x01000000
#define DEF_DELDBLCHAR	 0x02000000
#define DEF_AUTOWRAP	 0x04000000
#define DEF_USEGB        0x08000000     /*  KCN  */
#define DEF_NOTHIDEIP	 0x10000000
#define DEF_CAMPUS       0x10000000   
#define DEF_COLORMARK	 0x20000000
#define DEF_SYSMSG	 	 0x40000000
#define DEF_POSTNOMSG	 0x80000000
#define NUMDEFINES 32

#ifndef EXTERN
extern char *user_definestr[];
#else
/* You might want to put more descriptive strings for SPECIAL1 and SPECIAL2
depending on how/if you use them. */
char *user_definestr[] = {
                             "�������ر�ʱ���ú��Ѻ���",     /* DEF_FRIENDCALL */
                             "���������˵�ѶϢ",             /* DEF_ALLMSG */
                             "���ܺ��ѵ�ѶϢ",               /* DEF_FRIENDMSG */
                             "�յ�ѶϢ��������",             /* DEF_SOUNDMSG */
                             "��վ����ʾ�����¼",		/* DEF_NOTSHOWMSG */
                             "��ʾ�����",             	/* DEF_ACBOARD */
                             "��ʾѡ����ѶϢ��",             /* DEF_ENDLINE */
                             "�༭ʱ��ʾ״̬��",     	/* DEF_EDITMSG */
                             "ѶϢ������һ��/����ģʽ",	/* DEF_NOTMSGFRIEND */
                             "ѡ������һ��/����ģʽ",	/* DEF_NORMALSCR */
                             "������������ New ��ʾ",	/* DEF_NEWPOST */
                             "�Ķ������Ƿ�ʹ���ƾ�ѡ��",   	/* DEF_CIRCLE */
                             "�Ķ������α�ͣ춵�һƪδ��",	/* DEF_FIRSTNEW */
                             "��վʱ��ʾ��������",   	/* DEF_LOGFRIEND */
                             "������վ֪ͨ",                 /* DEF_LOGINFROM */
                             "�ۿ����԰�",                   /* DEF_NOTEPAD*/
                             "��Ҫ�ͳ���վ֪ͨ������",       /* DEF_NOLOGINSEND */
                             "����ʽ����",                   /* DEF_THESIS */
                             "�յ�ѶϢ�Ⱥ��Ӧ�����",       /* DEF_MSGGETKEY */
                             "��վʱ�ۿ���վ�˴�ͼ",         /* DEF_GRAPH */
                             "��վʱ�ۿ�ʮ�����а�",         /* DEF_TOP10 */
                             "ʹ������ǩ����",		/* DEF_RANDSIGN */
                             "��ʾ����",			/* DEF_S_HOROSCOPE */
                             "������������",			/* DEF_FRIEND_INFO */
                             "ʹ��\'+\'���δ������",	/* DEF_NOT_N_MASK */
                             "��������ɾ��",                 /* DEF_DELDBLCHAR */
                             "�Զ��Ű���Ԥ��Ϊ 78 ��",	/* DEF_AUTOWRAP */
                             "���治��ʾ�û��Ƿ�����",                 /* DEF_USEGB KCN 99.09.03 */
                             "��վʱ�ۿ�����",         /*DEF_CAMPUS */   
			//"�������Լ��� IP",		/* DEF_NOTHIDEIP */
                             "���±��ʹ�ò�ɫ",		/* DEF_COLORMARK */
                             "��ʾ��̬����",		/* DEF_SYSMSG*/
                             "��������ʱ��ʱ����ѶϢ",       /* DEF_POSTNOMSG */
                             NULL
                         };
#endif
