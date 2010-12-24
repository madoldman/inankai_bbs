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

#define VOTE_YN         (1)
#define VOTE_SINGLE     (2)
#define VOTE_MULTI      (3)
#define VOTE_VALUE      (4)
#define VOTE_ASKING     (5)

//Add for vote log & anonymous tdhlshx

//#define VOTE_FLAG_OPENED 0x1
//#define VOTE_FLAG_LIMITED 0x2
//#define VOTE_FLAG_ANONY 0x1

/*  LISTMASK	ͶƱ��ͶƱ�������� */
#define LISTMASK	PERM_POSTMASK
/*  VOTEMASK	ͶƱ����վʱ����������� */
#define VOTEMASK	PERM_NOZAP
/* ���Ʊ��� level ���Ի���������Ȩ�����ƣ��������� */

struct ballot
{
	char    uid[IDLEN+1];                   /* ͶƱ��       */
	unsigned int voted;                  /* ͶƱ������   */
	char    msg[3][STRLEN];         /* ��������     **/
};
//Add for vote log tdhlshx
struct votelog
{
	char uid[IDLEN+1];
	char ip[16];
	time_t votetime;
	unsigned int voted;
};
//Add end
struct votebal
{
	char            userid[16];
	char            title[STRLEN];
	char            type;
	char            items[32][40];
	int             maxdays;
	int             maxtkt;
	int             totalitems;
	time_t          opendate;
	// 	short int       flag;           //vote log tdhlshx
	//        short int       anony;          //anonymous vote
	unsigned int	level;
	unsigned int	x_logins; 	//��վ����
	unsigned int	x_posts;  	//����������
	unsigned int	x_stay; 	//��վ��ʵ��Сʱ��
	unsigned int	x_live; 	//ע�������ʱ��
};

//��¼ͶƱ�û�ID,IP,ͶƱʱ�� tdhlshx
/*struct votelog
{
	char uid[IDLEN+1];
	char ip[16];
	time_t votetime;
	unsigned int voted;
}
*/
