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
$Id: mail.c,v 1.2 2008-12-22 12:54:56 madoldman Exp $
*/

extern struct postheader header;
#include "bbs.h"

/*For read.c*/
int     auth_search_down();
int     auth_search_up();
int     do_cross();
int     edit_post();
int		move_post();
int		edit_mail_title();
int     Import_post();
int     Save_post();
int     t_search_down();
int     t_search_up();
int     post_search_down();
int     post_search_up();
int     thread_up();
int     thread_down();
int     deny_m_user();
int     into_myAnnounce();
int     show_user_notes();
int     show_allmsgs();
int     show_author();
int     SR_first_new();
int     SR_last();
int     SR_first();
int     SR_read();
int     SR_read();
int     SR_author();
int     Q_Goodbye();
int     t_friends();
int     s_msg();
int     G_SENDMODE = NA;
int		Mail_Add_Combine();//bluetent 2003.02.24 �ʼ��ϼ�����
int	y_lockscreen();
int	Mail_func ();

extern int numofsig;
extern char quote_file[], quote_user[];
char    currmaildir[STRLEN];
int gsendnum;//loveni
#define maxrecp 300


/*����վ,��Ҫ�����û���mailĿ¼���½���һ��recycle��,
  ��ɾ�ĵ��ż��ݴ��ڴ�*/
#ifdef MAILRECYCLE
int recycle_mailreadhelp();
char   r_currmaildir[STRLEN];
#endif





int chkmail()
{
	static long lasttime = 0;
	static  ismail = 0;
	struct fileheader fh;
	struct stat st;
	int     fd, size;
	register int i, offset;
	register long numfiles;
	unsigned char ch;
	extern char currmaildir[STRLEN];
	if (!HAS_PERM(PERM_BASIC))
	{
		return 0;
	}
	/* add by bluetent ����һ��Ķ�ȡ�� */
	/*	if (time(0)%2==1)
		{
			return 0;
		}*/
	/* add end */
	size = sizeof(struct fileheader);
	offset = (int) ((char *) &(fh.accessed[0]) - (char *) &(fh));
	if ((fd = open(currmaildir, O_RDONLY)) < 0)
		return (ismail = 0);
	fstat(fd, &st);
	if (lasttime >= st.st_mtime)
	{
		close(fd);
		return ismail;
	}
	lasttime = st.st_mtime;
	numfiles = st.st_size;
	numfiles = numfiles / size;
	if (numfiles <= 0)
	{
		close(fd);
		return (ismail = 0);
	}
	lseek(fd, (off_t) (st.st_size - (size - offset)), SEEK_SET);
	for (i = 0; i < numfiles; i++)
	{
		read(fd, &ch, 1);
		if (!(ch & FILE_READ))
		{
			close(fd);
			return (ismail = 1);
		}
		lseek(fd, (off_t) (-size - 1), SEEK_CUR);
	}
	close(fd);
	return (ismail = 0);
}

int check_query_mail(char qry_mail_dir[STRLEN])
{
	struct fileheader fh;
	struct stat st;
	int     fd, size;
	register int offset;
	register long numfiles;
	unsigned char ch;
	offset = (int) ((char *) &(fh.accessed[0]) - (char *) &(fh));
	if ((fd = open(qry_mail_dir, O_RDONLY)) < 0)
		return 0;
	fstat(fd, &st);
	numfiles = st.st_size;
	size = sizeof(struct fileheader);
	numfiles = numfiles / size;
	if (numfiles <= 0)
	{
		close(fd);
		return 0;
	}
	lseek(fd, (off_t) (st.st_size - (size - offset)), SEEK_SET);
	/*    for(i = 0 ; i < numfiles ; i++) {
	        read(fd,&ch,1) ;
	        if(!(ch & FILE_READ)) {
	            close(fd) ;
	            return YEA ;
	        }
	        lseek(fd,(off_t)(-size-1),SEEK_CUR);
	    }*/
	/*���߲�ѯ����ֻҪ��ѯ����һ���Ƿ�Ϊ���ţ�����������Ҫ*/
	/*Modify by SmallPig*/
	read(fd, &ch, 1);
	if (!(ch & FILE_READ))
	{
		close(fd);
		return YEA;
	}
	close(fd);
	return NA;
}

int mailall()
{
	char    ans[4], fname[STRLEN], title[STRLEN];
	char    doc[8][STRLEN], buf[STRLEN];
	int     i;
	strcpy(title, "û����");
	modify_user_mode(TYPEN);
	clear();
	move(0, 0);
	sprintf(fname, "tmp/mailall.%s", currentuser.userid);
	prints("��Ҫ�ĸ����еģ�\n");
	prints("(0) ����\n");
	strcpy(doc[0], "(1) ��δͨ�����ȷ�ϵ�ʹ����");
	strcpy(doc[1], "(2) ����ͨ�����ȷ�ϵ�ʹ����");
	strcpy(doc[2], "(3) ���еİ���");
	strcpy(doc[3], "(4) ��վ������");
	strcpy(doc[4], "(5) ����ӵ������Ȩ�޵�ʹ����");
        strcpy(doc[5], "(6) ����ӵ�п�������Ȩ�޵�ʹ����");
        strcpy(doc[6], "(7) ����ӵ�������ʺŵ�ʹ����");
        strcpy(doc[7], "(8) ����ʹ����");

	for (i = 0; i < 8; i++)
		prints("%s\n", doc[i]);
	getdata(11, 0, "������ģʽ (0~8)? [0]: ", ans, 2, DOECHO, YEA);
	if (ans[0] - '0' < 1 || ans[0] - '0' > 8)
	{
		return NA;
	}
	sprintf(buf, "�Ƿ�ȷ���ĸ�%s ", doc[ans[0] - '0' - 1]);
	move(12, 0);
	if (askyn(buf, NA, NA) == NA)
		return NA;
	in_mail = YEA;
	header.reply_mode = NA;
	strcpy(header.title, "û����");
	strcpy(header.ds, doc[ans[0] - '0' - 1]);
	header.postboard = NA;
	i = post_header(&header);
	if( i == -1 )
		return NA;
	if( i == YEA )
		sprintf(save_title, "[Type %c ����] %.60s", ans[0], header.title);
	setquotefile("");
	do_quote(fname, header.include_mode);
	if (vedit(fname, YEA, YEA) == -1)
	{
		in_mail = NA;
		unlink(fname);
		clear();
		return -2;
	}
	move(t_lines - 1, 0);
	clrtoeol();
	prints("[5;1;32;44m���ڼļ��У����Ժ�.....                                                        [m");
	refresh();
	mailtoall(ans[0] - '0');
	move(t_lines - 1);
	clrtoeol();
	unlink(fname);
	in_mail = NA;
	return 0;
}


#ifdef INTERNET_EMAIL

void m_internet()
{
	char    receiver[68];
	modify_user_mode(SMAIL);
	if (check_maxmail())
	{
		pressreturn();
		return;
	}

	getdata(1, 0, "������E-mail��", receiver, 65, DOECHO, YEA);
	strtolower(genbuf, receiver);
	if (strstr(genbuf, "@"BBSHOST)
	        || strstr(genbuf, "@localhost"))
	{
		move(3, 0);
		prints("վ���ż�, ���� (S)end ָ������\n");
		pressreturn();
	}
	else if (!invalidaddr(receiver))
	{
		*quote_file = '\0';
		clear();
		do_send(receiver, NULL);
	}
	else
	{
		move(3, 0);
		prints("�����˲���ȷ, ������ѡȡָ��\n");
		pressreturn();
	}
	clear();
	refresh();
}
#endif

void m_init()
{
	sprintf(currmaildir, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, DOT_DIR);
}


/*thunder  2003.5.2  �ʼ�����վ*/
#ifdef MAILRECYCLE
void r_m_init()
{
	sprintf(r_currmaildir, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");
}
#endif








int do_send(char* userid, char* title)
{
	struct fileheader newmessage;
	struct override fh;
	struct stat st;
	char    filepath[STRLEN], fname[STRLEN], *ip;
	char    save_title2[STRLEN];
	int     fp, count, result;
	int     internet_mail = 0;
	char    tmp_fname[STRLEN];
	extern int cmpfnames();

	/* I hate go to , but I use it again for the noodle code :-) */
	if (strchr(userid, '@'))
	{
		internet_mail = YEA;
		sprintf(tmp_fname, "tmp/imail.%s.%05d", currentuser.userid, uinfo.pid);
		strcpy(filepath, tmp_fname);
		goto edit_mail_file;
	}
	/* end of kludge for internet mail */

	if (!getuser(userid))
		return -1;
	if (!(lookupuser.userlevel & PERM_READMAIL)||(lookupuser.userlevel & PERM_SUICIDE))
		return -3;
	sethomefile(filepath, userid, "rejects");
	if(!(currentuser.userlevel & PERM_SYSOP)&&search_record(filepath, &fh, sizeof(fh), cmpfnames, currentuser.userid))
		return -5; //add by yiyo SYSOP�ɷ���
	if(!(lookupuser.userlevel&PERM_MAILINFINITY) && getmailboxsize(lookupuser)*2<getmailsize(lookupuser.userid))
		return -4;

	/* add by yiyo for deny user's mail */
	if (deny_m_user(userid))
		return -5;
	/* end of change */

	sprintf(filepath, "mail/%c/%s", toupper(userid[0]), userid);
	if (stat(filepath, &st) == -1)
	{
		if (mkdir(filepath, 0755) == -1)
			return -1;


		/*thunder  2003.5.2  �ʼ�����վ,�½�.recycle��*/
#ifdef MAILRECYCLE

		char   buf[STRLEN];
		sprintf(buf, "mail/%c/%s/%s", toupper(userid[0]), userid,".recycle");
		if (open(buf,O_CREAT,S_IRUSR|S_IWUSR|S_IXOTH)==-1)

			// int fd;
			//if((fd =open(buf, O_WRONLY | O_CREAT, 0644)) == -1)
			return -2;
#endif



	}
	else
	{
		if (!(st.st_mode & S_IFDIR))
			return -1;
	}
	memset(&newmessage, 0, sizeof(newmessage));
	sprintf(fname, "M.%d.A", time(NULL));
	sprintf(filepath, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);
	ip = strrchr(fname, 'A');
	count = 0;
	while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
	{
		if (*ip == 'Z')
			ip++, *ip = 'A', *(ip + 1) = '\0';
		else
			(*ip)++;
		sprintf(filepath, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);
		if (count++ > MAX_POSTRETRY)
		{
			return -1;
		}
	}
	close(fp);
	strcpy(newmessage.filename, fname);
#if defined(MAIL_REALNAMES)

	sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.realname);
#else

	sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.username);
#endif

	strncpy(newmessage.owner, currentuser.userid, STRLEN);
	sprintf(filepath, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);

edit_mail_file:
	if (title == NULL)
	{
		header.reply_mode = NA;
		strcpy(header.title, "û����");
	}
	else
	{
		header.reply_mode = YEA;
		strcpy(header.title, title);
	}
	header.postboard = NA;
	in_mail = YEA;

	setuserfile(genbuf, "signatures");
	ansimore2(genbuf, NA, 0, 18);
	strcpy(header.ds, userid);
	result = post_header(&header);
	if( result == -1 )
	{
		clear();
		return -2;
	}
	if( result == YEA)
	{
		strcpy(newmessage.title, header.title);
		strncpy(save_title, newmessage.title, STRLEN);
		sprintf(save_title2, "{%.16s} %.60s", userid, newmessage.title);
		strncpy(save_filename, fname, 4096);
	}
	do_quote(filepath, header.include_mode);

	if (internet_mail)
	{
#ifndef INTERNET_EMAIL
		prints("�Բ��𣬱�վ�ݲ��ṩ InterNet Mail ����");
		pressanykey();
#else

		int     res;
		if (vedit(filepath, YEA, YEA) == -1)
		{
			unlink(filepath);
			clear();
			return -2;
		}
		clear();
		prints("�ż������ĸ� %s \n", userid);
		prints("����Ϊ�� %s \n", header.title);
		if (askyn("ȷ��Ҫ�ĳ���", YEA, NA) == NA)
		{
			prints("\n�ż���ȡ��...\n");
			res = -2;
		}
		else
		{
			int	filter=YEA;
#ifdef SENDMAIL_MIME_AUTOCONVERT

			int     ans;
			ans = askyn("�� MIME ��ʽ����", NA, NA);
			if (askyn("�Ƿ����ANSI���Ʒ�",YEA,NA) == NA)
				filter = NA;
			if (askyn("�Ƿ񱸷ݸ��Լ�", NA, NA) == YEA)
				mail_file(tmp_fname, currentuser.userid, save_title2);
			prints("���Ժ�, �ż�������...\n");
			refresh();
			res = bbs_sendmail(tmp_fname, header.title, userid, filter,ans);
#else

		if (askyn("�Ƿ����ANSI���Ʒ�",YEA,NA) == NA)
			filter = NA;
		if (askyn("�Ƿ񱸷ݸ��Լ�", NA, NA) == YEA)
			mail_file(tmp_fname, currentuser.userid, save_title2);
		prints("���Ժ�, �ż�������...\n");
		refresh();
		res = bbs_sendmail(tmp_fname, header.title, userid, filter);
#endif

		}
		unlink(tmp_fname);
		sprintf(genbuf, "mailed %s", userid);
		report(genbuf);
		return res;
#endif

	}
	else
	{
		if (vedit(filepath, YEA, YEA) == -1)
		{
			unlink(filepath);
			clear();
			return -2;
		}
		clear();
		if (askyn("�Ƿ񱸷ݸ��Լ�", NA, NA) == YEA)
			mail_file(filepath, currentuser.userid, save_title2);
#if 0
		//-----add by yl to calculate the length of a mail -----
		sprintf(genbuf, "mail/%c/%s/%s", toupper(userid[0]), userid, newmessage.filename);
		if (stat(genbuf, &st) == -1)
			file_size = 0;
		else
			file_size=st.st_blksize*st.st_blocks;
		//memcpy(newmessage.filename+STRLEN-5,&file_size,4);
		sizeptr = (int*)(newmessage.filename+STRLEN-5);
		*sizeptr = file_size;
		//------------------------------------------------------
#endif

		sprintf(genbuf, "mail/%c/%s/%s", toupper(userid[0]), userid, DOT_DIR);
		if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
			return -1;
		sprintf(genbuf, "mailed %s", userid);
		report(genbuf);
		return 0;
	}
}

int m_send(char userid[])
{
	char    uident[STRLEN];
	if (check_maxmail())
	{
		pressreturn();
		return 0;
	}
	if (!HAS_PERM(PERM_POST))
	{
		return 0;
	}

	if (uinfo.mode != LUSERS && uinfo.mode != LAUSERS && uinfo.mode != FRIEND
	        && uinfo.mode != GMENU)
	{
		move(1, 0);
		clrtoeol();
		modify_user_mode(SMAIL);
		usercomplete("�����ˣ� ", uident);
		if (uident[0] == '\0')
		{
			return FULLUPDATE;
		}
	}
	else
		strcpy(uident, userid);
	modify_user_mode(SMAIL);
	clear();
	*quote_file = '\0';
	switch (do_send(uident, NULL))
	{
	case -1:
		prints("�����߲���ȷ\n");
		break;
	case -2:
		prints("ȡ��\n");
		break;
	case -3:
		prints("[%s] �޷�����\n", uident);
		break;
	case -4:
		prints("[%s] �����������޷�����\n",uident);
		break;
	case -5:
		prints("[%s] �����յ������ż�\n",uident);
		break;
	default:
		prints("�ż��Ѽĳ�\n");
	}
	pressreturn();
	return FULLUPDATE;
}

int read_mail(struct fileheader* fptr)
{
	FILE *judgefp;
	sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fptr->filename);
	if((judgefp=fopen(genbuf,"r"))!=NULL)
        {
                fclose(judgefp);
                ansimore(genbuf, NA);
        }
        else
        {
                sprintf(genbuf, "%s/%s",ANNOUNCE_MAIL_DIR,fptr->filename);
                ansimore(genbuf, NA);
        }
	fptr->accessed[0] |= FILE_READ;
	return 0;
}

int     mrd;

int     delmsgs[1024];
int     delcnt;

int read_new_mail(struct fileheader *fptr)
{
	static int idc;
	char    done = NA, delete_it;
	FILE	*judgefp;
	/*thunder  2003.5.2  �ʼ�����վ*/
#ifdef MAILRECYCLE

	char recycle_it=NA;
#endif

	char    fname[256];
	if (fptr == NULL)
	{
		delcnt = 0;
		idc = 0;
		return 0;
	}

	idc++;
	if (fptr->accessed[0])
		return 0;
	mrd = 1;
	clear();

	prints("��ȡ %s ������ '%s' ?\n", fptr->owner, fptr->title);
	//prints("(Yes, or No): ");
	getdata(1, 0, "(Y)��ȡ (N)���� (Q)�뿪 [Y]: ", genbuf, 3, DOECHO, YEA);
	if (genbuf[0] == 'q' || genbuf[0] == 'Q')
	{
		clear();
		return QUIT;
	}
	if (genbuf[0] != 'y' && genbuf[0] != 'Y' && genbuf[0] != '\0')
	{
		clear();
		return 0;
	}
	read_mail(fptr);
	strncpy(fname, genbuf, sizeof(fname));
	//mrd = 1;
	if (substitute_record(currmaildir, fptr, sizeof(*fptr), idc))
		return -1;
	delete_it = NA;
	while (!done)
	{
		move(t_lines - 1, 0);

		/*thunder  2003.5.2  �ʼ�����վ*/
#ifdef MAILRECYCLE

		prints("(R)����, (D)����ɾ��, (C)�������վ (G)����? [G]: ");
#else

		prints("(R)����, (D)ɾ��, (G)���� ? [G]: ");
#endif


		switch (egetch())
		{
		case 'R':
		case 'r':
			mail_reply(idc, fptr, currmaildir);
			break;

			/*thunder 2003.5.2 */
#ifdef MAILRECYCLE

		case 'c':
		case 'C':
			done=YEA;
			recycle_it=YEA;
			break;
#endif

		case 'D':
		case 'd':
			delete_it = YEA;

		default:
			done = YEA;
		}
		if (!done)
			ansimore(fname, NA);	/* re-read */
	}

	/*thunder 2003.5.2 */
#ifdef MAILRECYCLE
	if(recycle_it)
	{
		delmsgs[delcnt++] = idc;



		sprintf(genbuf, "�������վ[%-.55s]", fptr->title);
		if (askyn(genbuf, NA, YEA) == NA)
		{
			//move(t_lines - 1, 0);
			return 0;
		}


		sprintf(r_currmaildir, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");


		//int id=search_record(currmaildir, rptr, sizeof(struct fileheader), equal, fileinfo->filename);
		// if (id>0)
		//if (delete_record(currmaildir,sizeof(struct fileheader), id) ==0)
		if (append_record(r_currmaildir,fptr,sizeof(struct fileheader))==0)
		{

			move(t_lines - 1, 0);
			//      prints("ok...");
			return 0;
		}




	}
#endif

	if (delete_it)
	{
		clear();
		sprintf(genbuf, "ɾ���ż� [%-.55s]", fptr->title);
		if (askyn(genbuf, NA, NA) == YEA)
		{
			sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fptr->filename);
			if((judgefp=fopen(genbuf,"r"))!=NULL)
                        {
                                fclose(judgefp);
                                unlink(genbuf);
                        }
			delmsgs[delcnt++] = idc;
		}
	}



	clear();
	return 0;
}

int m_new()
{
	if(!strcmp(currentuser.userid,"guest"))
		return 0;
	clear();
	mrd = 0;
	modify_user_mode(RMAIL);
	read_new_mail(NULL);
	apply_record(currmaildir, read_new_mail, sizeof(struct fileheader));
	while (delcnt--)
		delete_record(currmaildir,sizeof(struct fileheader),delmsgs[delcnt]);
	if(!mrd)
	{
		clear();
		move(10, 30);
		prints("������û�����ż�!");
		pressanykey();
	}
	in_mail = NA;
	return -1;
}

extern char BoardName[];

/*
void
mailtitle()
{
	showtitle("�ʼ�ѡ��    ", BoardName);
	prints("�뿪[[1;32m��[m,[1;32me[m]  ѡ��[[1;32m��[m,[1;32m��[m]  �Ķ��ż�[[1;32m��[m,[1;32mRtn[m]  ����[[1;32mR[m]  ���ţ��������[[1;32md[m,[1;32mD[m]  ����[[1;32mh[m][m\n");
	prints("[1;44m���   %-12s %6s  %-50s[m\n", "������", "��  ��", "��  ��");
	clrtobot();
}
*/
void mailtitle()
{
	int total,used;
	if(!HAS_PERM(PERM_MAILINFINITY))
	{
		total=getmailboxsize(currentuser) ;
		//used=getmailsize(currentuser.userid);
	}
	used=getmailsize(currentuser.userid);
	showtitle( "�ʼ�ѡ��    ", BoardName );
#ifdef MAILRECYCLE

	prints( "�뿪[[1;32m��[m,[1;32me[m] ѡ��[[1;32m��[m, [1;32m��[m] �Ķ��ż�[[1;32m��[m,[1;32mRtn[m] �� ��[[1;32mR[m] ����վ[c] ����[[1;32md[m,[1;32mD[m] ����[[1;32mh[m][m\n" );
#else

	prints( "�뿪[[1;32m��[m,[1;32me[m] ѡ��[[1;32m��[m, [1;32m��[m] �Ķ��ż�[[1;32m��[m,[1;32mRtn[m] �� ��[[1;32mR[m] ����/�������[[1;32md[m,[1;32mD[m] ����[[1;32mh[m][m\n" );

#endif

	if(HAS_PERM(PERM_MAILINFINITY))
		prints( "[1;44m���   ������       �� ��      ����        ([32m��������[������],��ǰ����[%4dK][37m) [m\n",used);
	else
		prints("[1;44m���   ������       �� ��      ����  ([33m������������Ϊ[%4dK]����ǰ����[%4dK][37m) [m\n",total,used);
	clrtobot() ;
}

/*thunder  2003.5.2  �ʼ�����վ*/
#ifdef MAILRECYCLE
void recycle_mailtitle()
{
	int total,used;
	if(!HAS_PERM(PERM_MAILINFINITY))
	{
		total=getmailboxsize(currentuser.userlevel) ;
		//used=getmailsize(currentuser.userid);
	}
	used=getmailsize(currentuser.userid);
	showtitle( "�ʼ�����վ", BoardName );
	prints( "�뿪[[1;32m��[m,[1;32me[m] ѡ��[[1;32m��[m, [1;32m��[m] �Ķ��ż�[[1;32m��[m,[1;32mRtn[m]��ԭ[B] ���[C][m] ����[[1;32mh[m][m\n" );
	prints("[1;44m���   ������       �� ��      ���� [0m\n");
	clrtobot() ;
}
#endif





int getmailboxsize(struct userec user)
{
	if(user.userlevel&(PERM_SYSOP))
		return 30000;
	if(user.userlevel&(PERM_LARGEMAIL))
		return 25000;
	if((user.userlevel&(PERM_BOARDS))&&user.stay>2000000)
		return 16000;
	if((user.userlevel&(PERM_BOARDS))||user.stay>2000000)
		return 12000;
	if(user.stay>1000000)
		return 8000;
	if(user.userlevel&(PERM_LOGINOK))
		return 4000;
	return 100;
}

int getmailsize(char *userid)
{

	struct stat DIRst, SIZEst;
	char sizefile[50],dirfile[256];
	FILE  *fp;
	int mailsize= 9999;

	setmdir(dirfile,userid);
	sprintf(sizefile,"tmp/%s.mailsize",userid);
	if(stat(dirfile, &DIRst)==-1||DIRst.st_size==0)
		mailsize = 0;
	else
		if(stat(sizefile, &SIZEst)!=-1 && SIZEst.st_size!=0
		        && SIZEst.st_ctime >= DIRst.st_ctime)
		{
			fp = fopen(sizefile,"r");
			if(fp)
			{
				fscanf(fp,"%d",&mailsize);
				fclose(fp);
			}
		}

	if( mailsize != 9999 )
		return mailsize;

	mailsize = 0;
	
	int fd, ssize = sizeof(struct fileheader);
	struct fileheader fcache;
	char mailfile[256];
	struct stat st;

	fd = open(dirfile, O_RDONLY);
	if(fd != -1) {
	   while(read(fd, &fcache, ssize) == ssize){
	      sprintf(mailfile,"mail/%c/%s/%s",
	  toupper(userid[0]),userid,fcache.filename);
	      if(stat(mailfile,&st)!=-1) {
           	  mailsize += (st.st_size/1024+1);
         	}   
	   }

          close(fd);
	}



 sprintf(dirfile, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");

        fd = open(dirfile, O_RDONLY);
        if(fd != -1) {
           while(read(fd, &fcache, ssize) == ssize){
              sprintf(mailfile,"mail/%c/%s/%s",
          toupper(userid[0]),userid,fcache.filename);
              if(stat(mailfile,&st)!=-1) {
                  mailsize += (st.st_size/1024+1);
                }
           }

          close(fd);
        }


	fp = fopen(sizefile,"w+");
	if(fp){
	   fprintf(fp,"%d",mailsize);
	   fclose(fp);
	}
/*
	sprintf(genbuf,"du -s mail/%c/%s >%s",toupper(userid[0]),userid,sizefile);
        system(genbuf);
	fp=fopen(sizefile,"r");
	if(fp)
	{
		fscanf(fp,"%d ",&mailsize);
		fclose(fp);
	}*/
	return mailsize;
}

char   * maildoent(int num, struct fileheader *ent)
{
	static char buf[512];
	char    b2[512];
	time_t  filetime;
	char    status[20];
	char    color[10];
	char   *date,*t;
	extern char ReadPost[];
	extern char ReplyPost[];
	char    c1[8];
	char    c2[8];
	int     same = NA;
#ifdef COLOR_POST_DATE

	struct tm *mytm;
#endif

	filetime = atoi(ent->filename + 2);
	if (filetime > 740000000)
	{
		date =  ctime(&filetime) + 4;
	}
	else
	{
		date = "";
	}

#ifdef COLOR_POST_DATE
	mytm = localtime(&filetime);
	strftime(buf, 5, "%w", mytm);
	sprintf(color, "[1;%dm", 30 + atoi(buf) + 1);
#else

	strcpy(color, "");
#endif

	strcpy(c1, "[1;33m");
	strcpy(c2, "[1;36m");
	if (!strcmp(ReadPost, ent->title) || !strcmp(ReplyPost, ent->title))
		same = YEA;
	strncpy(b2, ent->owner, STRLEN);
	if ((b2[strlen(b2) - 1] == '<') && strchr(b2, '>'))
	{
		t = strtok(b2, "<>");
		if (invalidaddr(t))
			t = strtok(NULL, "<>");
		if (t != NULL)
			strcpy(b2, t);
	}
	if ((t = strchr(b2, ' ')) != NULL)
		*t = '\0';
	if (ent->accessed[0] & FILE_READ)
	{
		if ( ( ent->accessed[0] & FILE_MARKED )
		        && ( ent->accessed[0] & MAIL_REPLY))
			strcpy (status, "\33[35mb\33[m");
		else if ( ent->accessed[0] & FILE_MARKED )
			strcpy (status, "\33[33mm\33[m");
		else if ( ent->accessed[0] & MAIL_REPLY )
			strcpy (status, "r");
		else
			strcpy (status, " ");
	}
	else
	{
		if (ent->accessed[0] & FILE_MARKED)
			strcpy (status, "\33[33mM\33[m");
		else
			strcpy (status, "N");
	}
	if (!strncmp("Re:", ent->title, 3))
	{
		sprintf(buf, " %s%3d[m %s %-12.12s %s%6.6s[m  %s%.50s[m", same ? c1 : ""
		        ,num, status, b2, color, date, same ? c1 : "", ent->title);
	}
	else
	{
		sprintf(buf, " %s%3d[m %s %-12.12s %s%6.6s[m  �� %s%.47s[m"
		        ,same ? c2 : "", num, status, b2, color, date, same ? c2 : "", ent->title);
	}
	return buf;
}



/*thunder  2003.5.2  �ʼ�����վ*/
#ifdef MAILRECYCLE
int recycle_mail_read(int ent,struct fileheader *fileinfo,char *direct)
{
	char	notgenbuf[128],buf[STRLEN];
	char	*t;
	int	readnext;
	char	done = NA, del_it, recycle_it,replied;
	FILE	*judgefp;
	clear();
	readnext = NA;
	setqtitle(fileinfo->title);
	strcpy(buf, direct);
	if ((t = strrchr(buf, '/')) != NULL)
		*t = '\0';
	sprintf(notgenbuf, "%s/%s", buf, fileinfo->filename);
	if((judgefp=fopen(notgenbuf,"r"))!=NULL)
        {
                fclose(judgefp);
        }
        else
        {
                sprintf(notgenbuf,"%s/%s",ANNOUNCE_MAIL_DIR,fileinfo->filename);
        }
	del_it = replied =recycle_it= NA;
	while (!done)
	{
		ansimore(notgenbuf, NA);
		move(t_lines - 1, 0);
		prints("(B)��ԭ,(D)����ɾ��,(G)����? [G]: ");
		switch (egetch())
		{
		case ' ':
		case 'j':
		case KEY_RIGHT:
		case KEY_DOWN:
		case KEY_PGDN:
			done = YEA;
			readnext = YEA;
			break;
		case 'b':
		case 'B':
			done=YEA;
			recycle_it=YEA;
			break;
		case 'd':
		case 'D':
			done=YEA;
			del_it=YEA;
			break;

		default:
			done = YEA;
		}
	}
	if (recycle_it)

	{
		sprintf(genbuf, "��ԭ�ż� [%-.55s]", fileinfo->title);
		if (askyn(genbuf, NA, YEA) == NA)
		{
			move(t_lines - 1, 0);
			//	prints("������ԭ�ż�...");
			clrtoeol();
			//	egetch();
			return DIRCHANGED;
		}

		//   fileinfo->accessed[11]=(char)0;
		//		substitute_record(currmaildir, fileinfo, sizeof(*fileinfo), ent);
		if (delete_record(r_currmaildir,sizeof(struct fileheader), ent) ==0)
			if (append_record(currmaildir,fileinfo,sizeof(struct fileheader))==0)
			{

				move(t_lines - 1, 0);
				//          prints("ok...");
				clrtoeol();
				//     egetch();
				check_maxmail();
				return DIRCHANGED;
			}
		move(t_lines - 1, 0);
		clrtoeol();
		prints("ʧ��...");
	}


	if (del_it)
	{
		return    mail_del_from_recycle(ent,fileinfo,"");

	}

	if (readnext == YEA)
		return READ_NEXT;
	return FULLUPDATE;

}
//end
#endif




int mail_read(int ent,struct fileheader *fileinfo,char *direct)
{
	char    buf[512], notgenbuf[128];
	char	*t;
	int     readnext;
	char    done = NA, delete_it, replied;
	FILE	*judgefp;
	/*thunder 2003.5.2*/
#ifdef MAILRECYCLE

	char recycle_it=NA;
#endif

	clear();
	readnext = NA;
	setqtitle(fileinfo->title);
	strcpy(buf, direct);
	if ((t = strrchr(buf, '/')) != NULL)
		*t = '\0';
	sprintf(notgenbuf, "%s/%s", buf, fileinfo->filename);
	if((judgefp=fopen(notgenbuf,"r"))!=NULL)
        {
		fclose(judgefp);
	}
        else
        {
		sprintf(notgenbuf,"%s/%s",ANNOUNCE_MAIL_DIR,fileinfo->filename);
	}
	delete_it = replied = NA;
	while (!done)
	{
		ansimore(notgenbuf,NA);
		move(t_lines - 1, 0);

		/*thunder 2003.5.2*/
#ifdef MAILRECYCLE

		prints("(R)����, (D)����ɾ��, (C)�������վ (G)����? [G]: ");
#else

		prints("(R)����, (D)ɾ��, (G)���� ? [G]: ");
#endif

		switch (egetch())
		{
		case 'R':
		case 'r':
			replied = YEA;
			mail_reply(ent, fileinfo, direct);
			break;
		case ' ':
		case 'j':
		case KEY_RIGHT:
		case KEY_DOWN:
		case KEY_PGDN:
			done = YEA;
			readnext = YEA;
			break;


			/*thunder 2003.5.2*/
#ifdef MAILRECYCLE

		case 'c':
		case 'C':
			done=YEA;
			recycle_it=YEA;
			break;
#endif


		case 'D':
		case 'd':
			delete_it = YEA;
		default:
			done = YEA;
		}
	}
	if (delete_it)
		return mail_del(ent, fileinfo, direct);	/* �޸��ż�֮bug
				 * ����return */
	else

	{
		fileinfo->accessed[0] |= FILE_READ;
		substitute_record(currmaildir, fileinfo, sizeof(*fileinfo), ent);

		/*thunder 2003.5.2*/
#ifdef MAILRECYCLE

		if(recycle_it)
			return  mail_recycle(ent ,fileinfo,direct);
#endif

	}
	if (readnext == YEA)
		return READ_NEXT;
	return FULLUPDATE;
}


/*ARGSUSED*/
int mail_reply(int ent, struct fileheader* fileinfo, char* direct)
{
	char    uid[STRLEN];
	char    title[STRLEN];
	char   *t;
	modify_user_mode(SMAIL);
	sprintf(genbuf, "MAILER-DAEMON@%s", BBSHOST);
	if (strstr(fileinfo->owner, genbuf))
	{
		ansimore("help/mailerror-explain", YEA);
		return FULLUPDATE;
	}
	if (check_maxmail())
	{
		pressreturn();
		return 0;
	}
	clear();
	strncpy(uid, fileinfo->owner, STRLEN);
	if ((uid[strlen(uid) - 1] == '>') && strchr(uid, '<'))
	{
		t = strtok(uid, "<>");
		if (invalidaddr(t))
			t = strtok(NULL, "<>");
		if (t != NULL)
			strcpy(uid, t);
		else
		{
			prints("�޷�Ͷ��\n");
			pressreturn();
			return FULLUPDATE;
		}
	}
	if ((t = strchr(uid, ' ')) != NULL)
		*t = '\0';
	if (toupper(fileinfo->title[0]) != 'R' || toupper(fileinfo->title[1]) != 'E' ||
	        fileinfo->title[2] != ':')
		strcpy(title, "Re: ");
	else
		title[0] = '\0';
	strncat(title, fileinfo->title, STRLEN - 5);

	sprintf(quote_file, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fileinfo->filename);
	strcpy(quote_user, fileinfo->owner);
	switch (do_send(uid, title))
	{
	case -1:
		prints("�޷�Ͷ��\n");
		break;
	case -2:
		prints("ȡ������\n");
		break;
	case -3:
		prints("[%s] �޷�����\n", uid);
		break;
	case -4:
		prints("[%s] �����������޷�����\n", uid);
		break;
	case -5:
		prints("[%s] �����յ������ż�\n",uid);
		break;
	default:
		fileinfo->accessed[0] |= MAIL_REPLY;
		substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
		prints("�ż��Ѽĳ�\n");
	}
	pressreturn();
	return FULLUPDATE;
}





/*thunder 2003.5.2*/
#ifdef MAILRECYCLE

int equal(struct fileheader * rptr,char * s)
{
	if(strcmp(s,rptr->filename))
		return 0;
	else
		return 1;//equal ,strcmp return 0,and we need return 1
}
int mail_recycle(int a , struct fileheader *fileinfo,char * b)
{
	char    rptr[256];
	if(fileinfo->accessed[0] & FILE_MARKED)
	{

		move(t_lines - 1, 0);
		prints("�ż�m����,���ܷ������վ ...");
		//	clrtoeol();
		egetch();
		return DIRCHANGED;
	}

	sprintf(genbuf, "�������վ[%-.55s]", fileinfo->title);
	if (askyn(genbuf, NA, YEA) == NA)
	{
		move(t_lines - 1, 0);
		return DIRCHANGED;
	}


	sprintf(r_currmaildir, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");


	int id=search_record(currmaildir, rptr, sizeof(struct fileheader), equal, fileinfo->filename);
	if (id>0)
		if (delete_record(currmaildir,sizeof(struct fileheader), id) ==0)
			if (append_record(r_currmaildir,fileinfo,sizeof(struct fileheader))==0)
			{

				move(t_lines - 1, 0);
				//      prints("ok...");
				clrtoeol();
				// egetch();
				check_maxmail();
				return DIRCHANGED;
			}




	move(t_lines - 1, 0);
	prints("�������վʧ��...");
	clrtoeol();
	egetch();
	return   DIRCHANGED;
}
#endif

/* Efan: ����ɾ��ʱ����ʾ	*/
int mail_del2(int ent, struct fileheader* fileinfo, char* direct)
{
	char    buf[512];
	char	*t;
	FILE	*judgefp;
	extern int cmpfilename();
	extern char currfile[];
	strcpy(buf, direct);
	if ((t = strrchr(buf, '/')) != NULL)
		*t = '\0';
	strncpy(currfile, fileinfo->filename, STRLEN);
	if (!delete_file(direct, sizeof(*fileinfo), ent, cmpfilename))
	{
		sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
		if((judgefp=fopen(genbuf,"r"))!=NULL)
                {
                        fclose(judgefp);
                        unlink(genbuf);
                }
		check_maxmail();
		return DIRCHANGED;
	}
	move(t_lines - 1, 0);
	prints("ɾ��ʧ��...");
	clrtoeol();
	egetch();
	return PARTUPDATE;
}

int mail_del(int ent,struct fileheader *fileinfo,char *direct)
{
	char    buf[512];
	char	*t;
	FILE	*judgefp;
	extern int cmpfilename();
	extern char currfile[];
	sprintf(genbuf, "ɾ���ż� [%-.55s]", fileinfo->title);
	if (askyn(genbuf, NA, YEA) == NA)
	{
		move(t_lines - 1, 0);
		prints("����ɾ���ż�...");
		clrtoeol();
		egetch();

		//	return PARTUPDATE;
		return DIRCHANGED; //2003.4.26 add by thunder
	}
	strcpy(buf, direct);
	if ((t = strrchr(buf, '/')) != NULL)
		*t = '\0';
	strncpy(currfile, fileinfo->filename, STRLEN);
	if (!delete_file(direct, sizeof(*fileinfo), ent, cmpfilename))
	{
		sprintf(genbuf, "%s/%s", buf, fileinfo->filename);
		if((judgefp=fopen(genbuf,"r"))!=NULL)
                {
                        fclose(judgefp);
                        unlink(genbuf);
                }
		check_maxmail();
		return DIRCHANGED;
	}
	move(t_lines - 1, 0);
	prints("ɾ��ʧ��...");
	clrtoeol();
	egetch();
	return PARTUPDATE;
}
#ifdef INTERNET_EMAIL

int mail_forward(int ent, struct fileheader* fileinfo, char* direct)
{
	char    buf[STRLEN];
	char   *p;
	if (!HAS_PERM(PERM_FORWARD))
	{
		return DONOTHING;
	}
	strncpy(buf, direct, STRLEN);
	buf[STRLEN - 1] = '\0';
	if ((p = strrchr(buf, '/')) != NULL)
		*p = '\0';
	switch (doforward(buf, fileinfo, 0))
	{
	case 0:
		prints("����ת�����!\n");
		break;
	case -1:
		prints("ת��ʧ��: ϵͳ��������.\n");
		break;
	case -2:
		prints("ת��ʧ��: ����ȷ�����ŵ�ַ.\n");
		break;
	case -3:
		prints("�������䳬�ޣ���ʱ�޷�ʹ���ʼ�����.\n");
		break;
	default:
		prints("ȡ��ת��...\n");
	}
	pressreturn();
	clear();
	return FULLUPDATE;
}

int mail_u_forward(int ent, struct fileheader* fileinfo, char* direct)
{
	char    buf[STRLEN];
	char   *p;
	if (!HAS_PERM(PERM_FORWARD))
	{
		return DONOTHING;
	}
	strncpy(buf, direct, STRLEN);
	buf[STRLEN - 1] = '\0';
	if ((p = strrchr(buf, '/')) != NULL)
		*p = '\0';
	switch (doforward(buf, fileinfo, 1))
	{
	case 0:
		prints("����ת�����!\n");
		break;
	case -1:
		prints("ת��ʧ��: ϵͳ��������.\n");
		break;
	case -2:
		prints("ת��ʧ��: ����ȷ�����ŵ�ַ.\n");
		break;
	case -3:
		prints("�������䳬�ޣ���ʱ�޷�ʹ���ʼ�����.\n");
		break;
	default:
		prints("ȡ��ת��...\n");
	}
	pressreturn();
	clear();
	return FULLUPDATE;
}
#endif

int mail_del_range(int ent, struct fileheader* fileinfo, char* direct)
{
	//	return (del_range(ent, fileinfo, direct));
	int a;
	a=del_range(ent, fileinfo, direct);
	check_maxmail();
	return(a); //modified by yiyo ������������ʾ����
}






int mail_mark(int ent, struct fileheader* fileinfo, char* direct)
{
	if (fileinfo->accessed[0] & FILE_MARKED)
		fileinfo->accessed[0] &= ~FILE_MARKED;
	else
		fileinfo->accessed[0] |= FILE_MARKED;
	substitute_record(currmaildir, fileinfo, sizeof(*fileinfo), ent);
	return (PARTUPDATE);
}

extern int mailreadhelp();

#ifdef MAILRECYCLE
//int mail_recycle_del_range(int,struct fileheader *,char *);
#endif



int mail_recycle_del_range(int ent, struct fileheader* fileinfo, char* direct)
{
	clear();
	move(5,6);
	prints("          C ���ν�����վ������ʵ����.");
	pressreturn();
	return DIRCHANGED;
}


struct one_key mail_comms[] =
    {

	    /*thunder 2003.5.2*/
#ifdef MAILRECYCLE
	    'c', mail_recycle,//the quick key for recycling
	    'C', mail_recycle_del_range,
#endif
	    'd', mail_del,
	    'D', mail_del_range,
	    Ctrl('P'), m_send,
	    'E', edit_post,
	    Ctrl('K'), y_lockscreen,
	    '>', move_post,//add by yiyo�����ƶ��ʼ�˳��
	    'r', mail_read,
	    'R', mail_reply,
	    'm', mail_mark,
	    'i', Save_post,
	    'I', Import_post,
	    'x', into_myAnnounce,
	    'T', edit_mail_title,//add by bluetent �޸����±��� 2003.1.5
	    KEY_TAB, show_user_notes,
#ifdef INTERNET_EMAIL
	    'F', mail_forward,
	    'U', mail_u_forward,
#endif
	    'a', auth_search_down,
	    'A', auth_search_up,
	    '/', t_search_down,
	    '?', t_search_up,
	    '\'', post_search_down,
	    '\"', post_search_up,
	    ']', thread_down,
	    '[', thread_up,
	    Ctrl('A'), show_author,
	    Ctrl('N'), SR_first_new,
	    '\\', SR_last,
	    '=', SR_first,
	    'l', show_allmsgs,
	    Ctrl('C'), do_cross,
	    Ctrl('S'), SR_read,
	    'n', SR_first_new,
	    'p', SR_read,
	    Ctrl('X'), SR_read,
	    Ctrl('U'), SR_author,
	    'h', mailreadhelp,
	    Ctrl('J'), mailreadhelp,
	    '!', Q_Goodbye,
	    'S', s_msg,
	    'f', t_friends,
	    'b', Mail_func,
	    //'B', Mail_Add_Combine, //add by bluetent �ʼ��ϼ� 2003.02.24
	    '\0', NULL
    };



/*thunder 2003.5.2*/

#ifdef MAILRECYCLE
/*
int mail_recycle_del_range(ent, fileinfo, direct)
int     ent;
struct fileheader *fileinfo;
char   *direct;
{
   struct fileheader *x;
   char    num[8];
   int     inum1, inum2;
   FILE         *fp;
   char         buf[256],buf2[256];
   int          total=0;
   int          m=0;
 
//   int result;
   char fullpath[STRLEN];
   getdata(t_lines - 1, 0, "��ƪ�ż����: ", num, 6, DOECHO, YEA);
   inum1 = atoi(num);
   if (inum1 <= 0) {
      move(t_lines - 1, 50);
      prints("������...");
      egetch();
      return PARTUPDATE;
   }
   getdata(t_lines - 1, 25, "ĩƪ�ż����: ", num, 6, DOECHO, YEA);
   inum2 = atoi(num);
   
   
   if(uinfo.mode!=R_RMAIL)
	  
	  {
		  strcpy(buf,currmaildir);
          sprintf(buf2, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");
		  }
   else 
   {
	   strcpy(buf,r_currmaildir);
	   sprintf(buf2, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, DOT_DIR);
 
	   }
   
 
 
 
   if(fp=fopen(buf,"r"))
        { while(1)
                {
                        fseek(fp, total*sizeof(struct fileheader), SEEK_SET);
                        if(fread(&x, sizeof(x), 1, fp)<=0) break;
                        total++;
                }
                fclose(fp);
        }
   if (inum2 < inum1 + 1 || inum2 > total) {
      move(t_lines - 1, 50);
      prints("��������...");
      egetch();
      return PARTUPDATE;
   }
               	char result=NA; 
                if(currentuser.mode!=R_RMAIL)
				{
					move(t_lines - 1, 50);
			     	if (askyn("ɾ������m�ż�", NA, NA) == YEA)
                        m=1;
                    else
                        m=0;  
						
					move(t_lines - 1, 50);	
					result=askyn("ȷ��ɾ��", NA, NA);
						}
                  else 
				  { move(t_lines - 1, 50);
                    result=askyn("ȷ����ԭ", NA, NA);
					}
			 	 
   
   if ( result== YEA)
   {
   int i=inum1;
   for(;i<inum2+1;i++)
   {         
 
 
	      if(m==1)
			{    
			
				if ( ent->accessed[0] & FILE_MARKED )
			    status = 'm';
 
                ��ɾ����m���ż�:�д�ʵ��.
            
 
             /  //�ǲ�������Զ�����.
			if (delete_record(buf,sizeof(struct fileheader), inum1) ==0)
            if (append_record(buf2,fileinfo,sizeof(struct fileheader))==0) 
			{
			   //move(t_lines - 1, 0);
        //     prints("ok...");
	           //clrtoeol();
	       //  egetch();
               //check_maxmail();
              // return;
			   }
			else
            break;
   }
 
    if(i==inum2+1)  return  DIRCHANGED;
 
  }
   move(t_lines - 1, 50);
   clrtoeol();
   prints("����...");
   egetch();
   return PARTUPDATE;
}
 
 
*/

//thunder
int mail_back(int ent, struct fileheader* fileinfo, char* direct)
{
	//char    buf[512];
	//char   *t;
	//extern int cmpfilename();
	//extern char currfile[];
	sprintf(genbuf, "��ԭ�ż� [%-.55s]", fileinfo->title);
	if (askyn(genbuf, NA, YEA) == NA)
	{
		move(t_lines - 1, 0);
		//prints("������ԭ�ż�...");
		clrtoeol();
		egetch();
		return DIRCHANGED;
	}
	// fileinfo->accessed[11]=(char)0;
	if (delete_record(r_currmaildir,sizeof(struct fileheader), ent) ==0)
		if (append_record(currmaildir,fileinfo,sizeof(struct fileheader))==0)
		{

			move(t_lines - 1, 0);
			//     prints("ok...");
			clrtoeol();
			//  egetch();
			check_maxmail();
			return DIRCHANGED;
		}

	move(t_lines - 1, 0);
	prints("��ԭʧ��...");
	clrtoeol();
	egetch();
	return PARTUPDATE;
}


int mail_del_from_recycle(int ent, struct fileheader* fileinfo, char* direct)
{
	//char    buf[512];
	//char   *t;
	//extern int cmpfilename();
	//extern char currfile[];
	FILE	*judgefp;
	sprintf(genbuf, "�ӻ���վ����ɾ���ż�[%-.55s]", fileinfo->title);
	if (askyn(genbuf, NA, YEA) == NA)
	{
		move(t_lines - 1, 0);
		//prints("����ɾ��...");
		clrtoeol();
		//egetch();
		return DIRCHANGED;
	}

	sprintf(r_currmaildir, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");
	if (delete_record(r_currmaildir,sizeof(struct fileheader), ent) ==0)
	{
		char path[STRLEN];
		sprintf(path, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid,fileinfo->filename);
		if((judgefp=fopen(path,"r"))!=NULL)
		{
			fclose(judgefp);
			if(unlink(path)==0)
			{
				move(t_lines - 1, 0);
				// prints("ok...");
				clrtoeol();
				//     egetch();
				//check_maxmail();
				return DIRCHANGED;
			}
		}
		else
                {
                        sprintf(path,"%s/%s",ANNOUNCE_MAIL_DIR,fileinfo->filename);
			if((judgefp=fopen(path,"r"))!=NULL)
			{
				fclose(judgefp);
				move(t_lines - 1, 0);
                                // prints("ok...");
                                clrtoeol();
                                //     egetch();
                                //check_maxmail();
                                return DIRCHANGED;
			}
                }
	}
	move(t_lines - 1, 0);
	prints("ɾ��ʧ��..." );
	clrtoeol();
	egetch();
	return DIRCHANGED;
}



int recycle_clear()
{
	if (askyn("������ջ���վ,ȷ����", NA, YEA) == NA)
	{
		move(t_lines - 1, 0);
		prints("����...");
		clrtoeol();
		egetch();
		return PARTUPDATE;
	}
	//int done=1;
	char path[128];
	char rptr[256];// buf[128];
	// sprintf(path, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid,DOT_DIR);
	int num=get_num_records(r_currmaildir,sizeof(struct fileheader));
	int i;
	for(i=0;i<num;i++)
	{
		get_record(r_currmaildir, rptr, sizeof(struct fileheader),i+1);
		sprintf(path, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid,((struct fileheader *)rptr)->filename);
		unlink(path);

	}
	if(unlink(r_currmaildir)==0) 
		if ( open(r_currmaildir,O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH|S_IXOTH)!=-1)
		{

			move(t_lines - 1, 0);
			//  prints("ok...");
			clrtoeol();
			//   egetch();
			check_maxmail();
			return DIRCHANGED;
		}

	move(t_lines - 1, 0);
	prints("���ʧ��...");
	clrtoeol();
	egetch();
	return PARTUPDATE;
}


int mail_recycle_back_range(int ent, struct fileheader* fileinfo, char* direct)
{
	clear();
	move(5,6);
	prints("          B ���λ�ԭ������ʵ����.");
	pressreturn();
	return DIRCHANGED;
}

/*�ż�����:�������ֵ*/ 
int getrange(int *high,int *low)
{  
   getdata(t_lines - 1, 0, "��ƪ�ż����: ", genbuf, 6, DOECHO, YEA);
   *low = atoi(genbuf);
   if (*low <= 0) {
      move(t_lines - 1, 50);
      prints("������...");
	  egetch();
      return 1;
   }
   getdata(t_lines - 1, 25, "ĩƪ�ż����: ", genbuf, 6, DOECHO, YEA);
   *high = atoi(genbuf);
   
   if(uinfo.mode!=R_RMAIL)
		sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");
   else 
		sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, DOT_DIR);
   int total=file_size(genbuf)/sizeof(struct fileheader);    
   if (*high < *low || *high > total) {
      move(t_lines - 1, 50);
      prints("��������...");
      egetch();
      return 1;
   }
   if(askyn("ȷ����?", NA, YEA) == NA){
		move(t_lines - 1, 0);
    	clrtoeol();
		prints("����...");
		egetch();
		return 1;
	}
   return 0;
}

/*���ν�����վ��ں���*/
int mail_rd_range(int ent, struct fileheader *fileinfo, char *direct)
{
	int high ,low;
	if(!getrange(&high,&low))
  	    recycleit(high,low);
	return PARTUPDATE;
}

int recycleit(int high,int low)
{
}

struct one_key recycle_mail_comms[] =
    {

	    'c', recycle_clear,//��ջ���վ
	    'C', recycle_clear,
	    'r', recycle_mail_read,
	    //   'C', mail_del_range,

	    'b', mail_back,//��ԭ�����ż�
	    'B', mail_recycle_back_range,

	    'd', mail_del_from_recycle,
	    'D', mail_del_from_recycle,


	    KEY_TAB, show_user_notes,
	    'a', auth_search_down,
	    'A', auth_search_up,
	    '/', t_search_down,
	    '?', t_search_up,
	    '\'', post_search_down,
	    '\"', post_search_up,
	    ']', thread_down,
	    '[', thread_up,
	    Ctrl('A'), show_author,
	    Ctrl('N'), SR_first_new,
	    '\\', SR_last,
	    '=', SR_first,
	    'l', show_allmsgs,
	    Ctrl('C'), do_cross,
	    Ctrl('S'), SR_read,
	    'n', SR_first_new,
	    'p', SR_read,
	    Ctrl('X'), SR_read,
	    Ctrl('U'), SR_author,
	    'h', recycle_mailreadhelp,
	    Ctrl('J'), recycle_mailreadhelp,
	    '!', Q_Goodbye,
	    'S', s_msg,
	    'f', t_friends,
	    '\0', NULL
    };


#endif










int m_read()
{
	if(!strcmp(currentuser.userid,"guest"))
		return 0;
	in_mail = YEA;
	i_read(RMAIL, currmaildir, mailtitle, maildoent, &mail_comms[0], sizeof(struct fileheader));
	in_mail = NA;
	return 0;
}


#ifdef MAILRECYCLE
//thunder
int Rmail()
{
	if(!strcmp(currentuser.userid,"guest"))
		return 0;
	in_mail = YEA;
	r_m_init();
	i_read(R_RMAIL, r_currmaildir, recycle_mailtitle, maildoent, &recycle_mail_comms[0], sizeof(struct fileheader));
	in_mail = NA;
	return 0;
}
#endif





int invalidaddr(char* addr)
{
	if (*addr == '\0' || !strchr(addr, '@'))
		return 1;
	while (*addr)
	{
		if (!isalnum(*addr) && !strchr(".!@:-_", *addr))
			return 1;
		addr++;
	}
	return 0;
}
#ifdef INTERNET_EMAIL

#ifdef SENDMAIL_MIME_AUTOCONVERT
int bbs_sendmail(char* fname, char* title, char* receiver, int filter, int mime)
#else
int bbs_sendmail(char* fname, char* title, char* receiver, int filter)
#endif
{
	FILE   *fin, *fout;
	sprintf(genbuf, "%s -f %s@%s %s", SENDMAIL,
	        currentuser.userid, BBSHOST, receiver);
	fout = popen(genbuf, "w");
	fin = fopen(fname, "r");
	if (fin == NULL || fout == NULL)
	{
		if( fin ) /* add fclose here and below by yiyo */
		{
			fclose(fin);
		}
		if( fout )
		{
			fclose(fout);
		}
		return -1;
	}

	fprintf(fout, "Return-Path: %s@%s\n", currentuser.userid, BBSHOST);
	fprintf(fout, "Reply-To: %s@%s\n", currentuser.userid, BBSHOST);
	fprintf(fout, "From: %s@%s\n", currentuser.userid, BBSHOST);
	fprintf(fout, "To: %s\n", receiver);
	fprintf(fout, "Subject: %s\n", title);
	fprintf(fout, "X-Forwarded-By: %s (%s)\n",
	        currentuser.userid,
#ifdef MAIL_REALNAMES
	        currentuser.realname);
#else

			currentuser.username);
#endif

	fprintf(fout, "X-Disclaimer: %s �Ա�������ˡ������\n", BoardName);
#ifdef SENDMAIL_MIME_AUTOCONVERT
	if (mime)
	{
		fprintf(fout, "MIME-Version: 1.0\n")
		;
		fprintf(fout, "Content-Type: text/plain; charset=US-ASCII\n");
		fprintf(fout, "Content-Transfer-Encoding: 8bit\n");
	}
#endif
	fprintf(fout, "Precedence: junk\n\n");

	while (fgets(genbuf, 255, fin)
	        != NULL)
	{
		if(filter)
			my_ansi_filter(genbuf);
		if (genbuf[0] == '.' && genbuf[1] == '\n')
			fputs(". \n", fout);
		else
			fputs(genbuf, fout);
	}

	fprintf(fout, ".\n");

	fclose(fin);
	pclose(fout);
	return 0;
}
#endif
int g_send()
{
	char    maillists[STRLEN];
	char mailnum[STRLEN];
	int	cnt;

	modify_user_mode(SMAIL);
	if (check_maxmail())
	{
		pressreturn();
		return 0;
	}
	*quote_file = '\0';
	clear();
	move(0,0);
	prints("����Ⱥ��");
	char buf[4];
	getdata (1, 0, "������ʹ�õڼ���Ⱥ������?(1~9): ? ", buf, 2, DOECHO, YEA);
	int num=atoi(buf);
	if(num<1||num>9) return 0;
	gsendnum=num;
	if(num==1) sprintf(mailnum,"maillist");
	else sprintf(mailnum,"maillist%d",num);
	//sethomefile(maillists, currentuser.userid, "maillist");
	sethomefile(maillists, currentuser.userid, mailnum);
	cnt = ListFileEdit(maillists,"�༭��Ⱥ���ż�������");
	if(cnt == 0)
	{
		prints("\nȺ���ż�����Ϊ��\n");
	}
	else
	{
		if(askyn("��ȷ��Ҫ��Ⱥ���ż���",NA,NA)==NA)
			return 0;
		G_SENDMODE = 2;
		switch (do_gsend(NULL, NULL, cnt))
		{
		case -1:
			prints("�ż�Ŀ¼����\n");
			break;
		case -2:
			prints("ȡ��\n");
			break;
		default:
			prints("�ż��Ѽĳ�\n");
		}
		G_SENDMODE = 0;
	}
	pressreturn();
	return 0;
}

#if 0
int g_send()
{
	char    uident[13], tmp[3];
	int     cnt, i, n, fmode = NA;
	char    maillists[STRLEN];

	modify_user_mode(SMAIL);
	if (check_maxmail())
	{
		pressreturn();
		return 0;
	}
	*quote_file = '\0';
	clear();
	sethomefile(maillists, currentuser.userid, "maillist");
	cnt = listfilecontent(maillists,3);
	while (1)
	{
		if (cnt > maxrecp - 10)
		{
			move(2, 0);
			prints("Ŀǰ���Ƽ��Ÿ� [1m%d[m ��", maxrecp);
		}
		getdata(0,0,"(A)���� (D)ɾ�� (I)������� (C)���Ŀǰ���� (E)���� (S)�ĳ�? [S]�� ",
		        tmp, 2, DOECHO, YEA);
		if (tmp[0] == '\n' || tmp[0] == '\0' || tmp[0] == 's' || tmp[0] == 'S')
		{
			break;
		}
		if (tmp[0] == 'a' || tmp[0] == 'd' || tmp[0] == 'A' || tmp[0] == 'D')
		{
			move(1, 0);
			if (tmp[0] == 'a' || tmp[0] == 'A')
				usercomplete("����������ʹ���ߴ���(ֻ�� ENTER ��������): ", uident);
			else
				namecomplete("����������ʹ���ߴ���(ֻ�� ENTER ��������): ", uident);
			move(1, 0);
			clrtoeol();
			if (uident[0] == '\0')
				continue;
			if (!getuser(uident))
			{
				move(2, 0);
				prints("���ʹ���ߴ����Ǵ����.\n");
			}
		}
		switch (tmp[0])
		{
		case 'A':
		case 'a':
			if (!(lookupuser.userlevel & PERM_READMAIL))
			{
				move(2, 0);
				prints("�޷����Ÿ�: [1m%s[m\n", lookupuser.userid);
				break;
			}
			else if (seek_in_file(maillists, uident))
			{
				move(2, 0);
				prints("�Ѿ���Ϊ�ռ���֮һ \n");
				break;
			}
			add_to_file(maillists, uident);
			cnt++;
			break;
		case 'E':
		case 'e':
		case 'Q':
		case 'q':
			cnt = 0;
			break;
		case 'D':
		case 'd':
			{
				if (seek_in_file(maillists, uident))
				{
					del_from_file(maillists, uident);
					cnt--;
				}
				break;
			}
		case 'I':
		case 'i':
			n = 0;
			clear();
			for (i = cnt; i < maxrecp && n < uinfo.fnum; i++)
			{
				int     key;
				move(2, 0);
				getuserid(uident, uinfo.friend[n]);
				prints("%s\n", uident);
				move(3, 0);
				n++;
				prints("(A)ȫ������ (Y)���� (N)������ (Q)����? [Y]:");
				if (!fmode)
					key = igetkey();
				else
					key = 'Y';
				if (key == 'q' || key == 'Q')
					break;
				if (key == 'A' || key == 'a')
				{
					fmode = YEA;
					key = 'Y';
				}
				if (key == '\0' || key == '\n' || key == 'y' || key == 'Y')
				{
					if (!getuser(uident))
					{
						move(4, 0);
						prints("���ʹ���ߴ����Ǵ����.\n");
						i--;
						continue;
					}
					else if (!(lookupuser.userlevel & PERM_READMAIL))
					{
						move(4, 0);
						prints("�޷����Ÿ�: [1m%s[m\n", lookupuser.userid);
						i--;
						continue;
					}
					else if (seek_in_file(maillists, uident))
					{
						i--;
						continue;
					}
					add_to_file(maillists, uident);
					cnt++;
				}
			}
			fmode = NA;
			clear();
			break;
		case 'C':
		case 'c':
			unlink(maillists);
			cnt = 0;
			break;
		}
		if (strchr("EeQq", tmp[0]))
			break;
		move(5, 0);
		clrtobot();
		if (cnt > maxrecp)
			cnt = maxrecp;
		move(3, 0);
		clrtobot();
		listfilecontent(maillists,3);
	}
	if (cnt > 0)
	{
		G_SENDMODE = 2;
		switch (do_gsend(NULL, NULL, cnt))
		{
		case -1:
			prints("�ż�Ŀ¼����\n");
			break;
		case -2:
			prints("ȡ��\n");
			break;
		default:
			prints("�ż��Ѽĳ�\n");
		}
		G_SENDMODE = 0;
		pressreturn();
	}
	return 0;
}
#endif

/*Add by SmallPig*/

int do_gsend(char *userid[], char* title, int num)
{
	struct stat st;
	char    filepath[STRLEN], tmpfile[STRLEN], fname[STRLEN];
	int     cnt, result;
	FILE   *mp;
	in_mail = YEA;
#if defined(MAIL_REALNAMES)

	sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.realname);
#else

	sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.username);
#endif

	header.reply_mode = NA;
	strcpy(header.title, "û����");
	strcpy(header.ds, "���Ÿ�һȺ��");
	header.postboard = NA;
	sprintf(tmpfile, "tmp/gsend.%s.%05d", currentuser.userid, uinfo.pid);
	result = post_header(&header);
	if( result == -1)
	{
		clear();
		return -2;
	}
	if( result == YEA)
	{
		sprintf(save_title, "[Ⱥ���ż�] %.60s", header.title);
		strncpy(save_filename, fname, 4096);
	}
	setquotefile("");//add by yiyo
	do_quote(tmpfile, header.include_mode);
	if (vedit(tmpfile, YEA, YEA) == -1)
	{
		unlink(tmpfile);
		clear();
		return -2;
	}
	clear();
	prints("[5;1;32m���ڼļ��У����Ժ�...[m");
	if (G_SENDMODE == 2)
	{
		char    maillists[STRLEN];
		char 	mailnum[STRLEN];
		if(gsendnum==1) sprintf(mailnum,"maillist");
	else sprintf(mailnum,"maillist%d",gsendnum);
		//setuserfile(maillists, "maillist");
		setuserfile(maillists, mailnum);
		if ((mp = fopen(maillists, "r")) == NULL)
		{
			return -3;
		}
	}
	for (cnt = 0; cnt < num; cnt++)
	{
		char    uid[13];
		char    buf[STRLEN];
		if (G_SENDMODE == 1)
			getuserid(uid, uinfo.friend[cnt]);
		else if (G_SENDMODE == 2)
		{
			if (fgets(buf, STRLEN, mp) != NULL)
			{
				if (strtok(buf, " \n\r\t") != NULL)
					strcpy(uid, buf);
				else
					continue;
			}
			else
			{
				cnt = num;
				continue;
			}
		}
		else
			strcpy(uid, userid[cnt]);
		sprintf(filepath, "mail/%c/%s", toupper(uid[0]), uid);
		if (stat(filepath, &st) == -1)
		{
			if (mkdir(filepath, 0755) == -1)
			{
				if (G_SENDMODE == 2)
					fclose(mp);
				return -1;
			}
		}
		else
		{
			if (!(st.st_mode & S_IFDIR))
			{
				if (G_SENDMODE == 2)
					fclose(mp);
				return -1;
			}
		}
		//		mail_file(tmpfile, uid, save_title);

		/* change by yiyo for deny user's mail */
		if (!deny_m_user(uid))
			mail_file(tmpfile, uid, save_title);
		else
			prints("\n[%s]�����յ������ż�", uid);
		/* add end */

	}
	unlink(tmpfile);
	//	clear();
	if (G_SENDMODE == 2)
		fclose(mp);
	return 0;
}
int mail_announce(char userid[STRLEN],char title[STRLEN],char fname[STRLEN])
{
        struct fileheader newmessage;
        struct stat st;
        char    filepath[STRLEN];
        memset(&newmessage, 0, sizeof(newmessage));
#if defined(MAIL_REALNAMES)
        sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.realname);
#else
        sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.username);
#endif
        strncpy(newmessage.owner, currentuser.userid, STRLEN);
        strncpy(newmessage.title, title, STRLEN);
        strcpy(newmessage.filename, fname);

        sprintf(filepath, "mail/%c/%s", toupper(userid[0]), userid);
        if (stat(filepath, &st) == -1)
        {
                if (mkdir(filepath, 0755) == -1)
                        return -1;
        }
        else
        {
                if (!(st.st_mode & S_IFDIR))
                        return -1;
        }
        sprintf(genbuf, "mail/%c/%s/%s", toupper(userid[0]), userid, DOT_DIR);
        if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
                return -1;
        sprintf(genbuf, "mailed %s ", userid);
        report(genbuf);
        return 0;
}


int mail_file(char tmpfile[STRLEN], char userid[STRLEN], char title[STRLEN])
{
	struct fileheader newmessage;
	struct stat st;
	char    fname[STRLEN], filepath[STRLEN], *ip;
	int     fp, count;
	memset(&newmessage, 0, sizeof(newmessage));
#if defined(MAIL_REALNAMES)

	sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.realname);
#else

	sprintf(genbuf, "%s (%s)", currentuser.userid, currentuser.username);
#endif

	strncpy(newmessage.owner, currentuser.userid, STRLEN);
	strncpy(newmessage.title, title, STRLEN);
	strncpy(save_title, newmessage.title, STRLEN);

	sprintf(filepath, "mail/%c/%s", toupper(userid[0]), userid);
	if (stat(filepath, &st) == -1)
	{
		if (mkdir(filepath, 0755) == -1)
			return -1;
	}
	else
	{
		if (!(st.st_mode & S_IFDIR))
			return -1;
	}
	sprintf(fname, "M.%d.A", time(NULL));
	sprintf(filepath, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);
	ip = strrchr(fname, 'A');
	count = 0;
	while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
	{
		if (*ip == 'Z')
			ip++, *ip = 'A', *(ip + 1) = '\0';
		else
			(*ip)++;
		sprintf(filepath, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);
		if (count++ > MAX_POSTRETRY)
		{
			return -1;
		}
	}
	close(fp);
	strcpy(newmessage.filename, fname);
	strncpy(save_filename, fname, 4096);
	sprintf(filepath, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);

	f_cp(tmpfile, filepath, O_CREAT);
#if 0
	//-----add by yl to calculate the length of a mail -----
	sprintf(genbuf, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);
	if (stat(genbuf, &st) == -1)
		file_size = 0;
	else
		file_size=st.st_blksize*st.st_blocks;
	//memcpy(newmessage.filename+STRLEN-5,&file_size,4);
	sizeptr = (int*)(newmessage.filename+STRLEN-5);
	*sizeptr = file_size;
	//------------------------------------------------------
#endif

	sprintf(genbuf, "mail/%c/%s/%s", toupper(userid[0]), userid, DOT_DIR);
	if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
		return -1;
	sprintf(genbuf, "mailed %s ", userid);
	report(genbuf);
	return 0;
}
/*Add by SmallPig*/
int ov_send()
{
	int     all, i;
	modify_user_mode(SMAIL);
	if (check_maxmail())
	{
		pressreturn();
		return 0;
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	prints("���Ÿ����������е��ˣ�Ŀǰ��վ���ƽ����Լĸ� [1m%d[m λ��\n", maxrecp);
	if (uinfo.fnum <= 0)
	{
		prints("�㲢û���趨���ѡ�\n");
		pressanykey();
		clear();
		return 0;
	}
	else
	{
		prints("�������£�\n");
	}
	G_SENDMODE = 1;
	all = (uinfo.fnum >= maxrecp) ? maxrecp : uinfo.fnum;
	for (i = 0; i < all; i++)
	{
		char    uid[IDLEN + 2];
		getuserid(uid, uinfo.friend[i]);
		prints("%-12s ", uid);
		if ((i + 1) % 6 == 0)
			outc('\n');
	}
	pressanykey();
	switch (do_gsend(NULL, NULL, all))
	{
	case -1:
		prints("�ż�Ŀ¼����\n");
		break;
	case -2:
		prints("�ż�ȡ��\n");
		break;
	default:
		prints("�ż��Ѽĳ�\n");
	}
	pressreturn();
	G_SENDMODE = 0;
	return 0;
}

int in_group(char uident[maxrecp][STRLEN], int cnt)
{
	int     i;
	for (i = 0; i < cnt; i++)
		if (!strcmp(uident[i], uident[cnt]))
		{
			return i + 1;
		}
	return 0;
}
#ifdef INTERNET_EMAIL

int doforward(char *direct,struct shortfile *fh,int mode)
{
	static char address[STRLEN];
	char    fname[STRLEN], tmpfname[STRLEN];
	char    receiver[STRLEN];
	char    title[STRLEN];
	int	changetitle=0;
	int     return_no, internet_mail=0;
	int	filter=YEA;
	FILE    *judgefp;
	clear();
	if(check_maxmail())
		return -3;
	if (address[0] == '\0')
	{
		//strncpy(address, currentuser.email, STRLEN);
		strncpy(address, currentuser.userid, STRLEN);//modified by yiyoĬ��Ϊվ��ת��
	}
	if (HAS_PERM(PERM_SETADDR))
	{
		prints("��ֱ�Ӱ� Enter ������������ʾ�ĵ�ַ, ��������������ַ(��ʱ���ɶ��⴫��)\n");
		prints("���ż�ת�ĸ� [%s]\n", address);
		getdata(2, 0, "==> ", receiver, 70, DOECHO, YEA);
	}
	else
		strcpy(receiver,currentuser.userid);
	if (receiver[0] != '\0')
	{
		strncpy(address, receiver, STRLEN);
	}
	else
		strncpy(receiver,address,STRLEN);
	sprintf(genbuf, "@%s", BBSHOST);
	if (strstr(receiver, genbuf)
	        || strstr(receiver, "@localhost"))
	{
		char   *pos;
		pos = strchr(address, '.');
		*pos = '\0';
	}
	if( strpbrk(address,"@."))
		internet_mail = YEA;
	if(!internet_mail)
	{
		if (!getuser(address))
			return -1;
		if(!(lookupuser.userlevel&PERM_MAILINFINITY) && getmailboxsize(lookupuser)*2<getmailsize(lookupuser.userid))
		{
			prints("[%s] �����������޷����š�\n",address);
			return -4;
		}
	}
	sprintf(genbuf, "ȷ�������¼ĸ� %s ��", address);
	if (askyn(genbuf, YEA, NA) == 0)
		return 1;
	if (invalidaddr(address))
		if (!getuser(address))
			return -2;
	sprintf(tmpfname, "tmp/forward.%s.%05d", currentuser.userid, uinfo.pid);

	sprintf(genbuf, "%s/%s", direct, fh->filename);
	if(strstr(genbuf,"mail")!=NULL)
        {
                if((judgefp=fopen(genbuf,"r"))!=NULL)
                {
                        fclose(judgefp);
                }
                else
                {
                        sprintf(genbuf,"%s/%s",ANNOUNCE_MAIL_DIR,fh->filename);
                }
        }
	f_cp(genbuf, tmpfname, O_CREAT);

	if (askyn("�Ƿ��޸����±���", NA, NA) == 1)//add by tdhlshx 
	{
		changetitle=1;
		prints("ԭ����: %s \n",fh->title);
		getdata(6,0,"�±���: ",fh->title,41,DOECHO,YEA);
	}
//	else strcpy(title,fh->title);
	if (askyn("�Ƿ��޸���������", NA, NA) == 1)
	{
		if (vedit(tmpfname, NA, NA) == -1)
		{
			if (askyn("�Ƿ�ĳ�δ�޸ĵ�����", YEA, NA) == 0)
			{
				unlink(tmpfname);
				clear();
				return 1;
			}
		}
		clear();
	}
	if(internet_mail)
		if (askyn("�Ƿ����ANSI���Ʒ�",YEA,NA) == NA )
			filter = NA;
	add_crossinfo(tmpfname, 2);
	prints("ת���ż��� %s, ���Ժ�....\n", address);
	refresh();

	if (mode == 0)
		strcpy(fname, tmpfname);
	else if (mode == 1)
	{
		sprintf(fname, "tmp/file.uu%05d", uinfo.pid);
		sprintf(genbuf, "uuencode %s fb-bbs.%05d > %s",
		        tmpfname, uinfo.pid, fname);
		system(genbuf);
	}
//	if( !strstr(fh->title,"(ת��)"))
//		    sprintf(title, "%.70s(ת��)", fh->title);
        if(!strstr(fh->title,"(ת��)"))	
	{
		if(changetitle==1)
		sprintf(title, "(ת�����޸����±���Ϊ:)%.41s(ת��)", fh->title);
		else
			    sprintf(title, "%.70s(ת��)", fh->title);
	}
	else
	//	strcpy(title,fh->title);
	{
	       	if(changetitle==1)
			sprintf(title, "(ת�����޸����±���Ϊ:)%.41s", fh->title);
     		else
			strcpy(title, fh->title);
	}
	if (!internet_mail)
		return_no = mail_file(fname, lookupuser.userid, title);
	else
	{
#ifdef SENDMAIL_MIME_AUTOCONVERT
		if (askyn("�� MIME ��ʽ����", NA, NA) == YEA)
			return_no = bbs_sendmail(fname, title, address, filter,YEA);
		else
			return_no = bbs_sendmail(fname, title, address, filter,NA);
#else

		return_no = bbs_sendmail(fname, title, address, filter);
#endif

	}
	if (mode == 1)
	{
		unlink(fname);
	}
	unlink(tmpfname);
	return (return_no);
}
#endif

/* add by yiyo for deny user's mail */
int deny_m_user( char * user)
{
	char filepath[STRLEN];
	char userid[40], *ptr;
	FILE *fp;
	if (HAS_PERM(PERM_SYSOP))
		return 0;
	sprintf(filepath, "home/%c/%s/deny_user", toupper(user[0]), user);
	if ((fp = fopen(filepath, "r")) == NULL)
		return 0;
	while (fgets(userid, 30, fp))
	{
		ptr = strtok(userid, " \t\n\r");
		if ( ptr != NULL && *ptr != '#' )
		{
			if (!strcasecmp(ptr, currentuser.userid))
			{
				fclose(fp);
				return 1;
			}
		}
	}
	fclose(fp);
	return 0;
}

int edit_deny_user()
{
	char denyfile[70];
	modify_user_mode(USERDEF);
	sethomefile(denyfile, currentuser.userid, "deny_user");
	ListFileEdit(denyfile,"�༭�������ʼ�������");
}
/* add end by yiyo */

/* bluetent 2003.1.5 edit the title of mails*/
int edit_mail_title(int ent, struct fileheader* fileinfo, char* direct)
{
	char    buf[STRLEN];
	char id1[20], id2[20]; //added by zhch, 12.19, to fix T d T bug.
	//	char b2[20];
	getnam(direct,ent,id1);
	//	strncpy(b2, fileinfo->owner, 20);



	strcpy(buf,fileinfo->title);
	getdata(t_lines - 1, 0, "���ż�����: ", buf, 50, DOECHO, NA);
	if(!strcmp(buf,fileinfo->title))
		return PARTUPDATE;
	check_title(buf);
	if (buf[0] != '\0')
	{
		char    tmp[STRLEN * 2], *t;
		getnam(direct, ent,id2);
		if(strcmp(id1,id2))
			return PARTUPDATE;
		//			if(strcmp(currentuser.userid, id2))return PARTUPDATE;
		strcpy(fileinfo->title, buf);
		strcpy(tmp, direct);
		if ((t = strrchr(tmp, '/')) != NULL)
			*t = '\0';
		sprintf(genbuf, "%s/%s", tmp, fileinfo->filename);
		change_title(genbuf, buf);
		substitute_record(direct, fileinfo, sizeof(*fileinfo), ent);
	}
	return PARTUPDATE;
}

int Mail_func (int ent, struct fileheader* fileinfo, char* direct)
{
	/*
	if (strncmp (currentuser.userid, "efan", 4) )
	{
		move (3, 0);
		clrtobot ();
		move (5, 0);
		prints ("                ����δʵ�֣������С���");
		pressreturn ();
	}
	else {
		Mail_func2 (ent, fileinfo, direct);
	}
	*/
	Mail_func2 (ent, fileinfo, direct);
	return FULLUPDATE;
}

int Mail_Add_Combine(char* board,struct fileheader* fileinfo,int mode)
{
	FILE *fp;
	char buf[256], buf2[256];
	char temp2[1024];
	int AddCombineMode=0;
	move(t_lines - 1, 0);
	sprintf(buf,"ȷ��Ҫִ���ʼ��ϼ�������");
	if (askyn(buf, NA, NA) == 0)
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	move(t_lines - 1, 0);
	sprintf(buf, "�Ƿ���¼���� (Y)�� (N)��");
	if(askyn(buf, YEA, NA) == YEA)
		AddCombineMode=1;
	else
		AddCombineMode=0;

	sprintf(buf,"tmp/%s.mailcombine",currentuser.userid);
	fp=fopen(buf,"at");
	fprintf(fp,"[1;32m�����������������������������������������������������������������������������[0;1m\n");

	{
		FILE *fp1;
		char buf[80];
		char *s_ptr,*e_ptr;
		int blankline=0;

		//    setbfile(buf,board,fileinfo->filename);
		sprintf(buf, "mail/%c/%s", toupper(currentuser.userid[0]), currentuser.userid);
		fp1=fopen(buf,"rt");
		if (fgets(temp2, 256, fp1)!=NULL)
		{
			e_ptr=strchr(temp2,',');
			if (e_ptr!=NULL)
				*e_ptr='\0';
			s_ptr=&temp2[7];
			fprintf(fp, "    [0;1;32m%s [0;1m��",s_ptr);
		}
		fgets(temp2, 256, fp1);
		if (fgets(temp2, 256, fp1)!=NULL)
		{
			e_ptr=strchr(temp2,',');
			if (e_ptr!=NULL)
				*(--e_ptr)='\0';
			s_ptr=strchr(temp2,'(');
			if (s_ptr==NULL)
				s_ptr=temp2;
			else
				s_ptr++;
			fprintf(fp, " [1;36m%s[0;1m �ᵽ��\n",s_ptr);
		}
		while (!feof(fp1))
		{
			fgets(temp2, 256, fp1);
			if ((unsigned)*temp2<'\x1b')
			{
				if (blankline)
					continue;
				else
					blankline=1;
			}
			else
				blankline=0;
			if(!mode)
				if ((strstr(temp2, "��"))|| + (*temp2==':'))
					continue;
			if (strncmp(temp2,"--",2)==0)
				break;
			fputs(temp2, fp);
		}
		fclose(fp1);
	}
	fprintf(fp,"\n" );
	fclose(fp);
	sprintf(buf,"tmp/%s.mailcombine",currentuser.userid);
	sprintf(buf2, "���ϼ���%s", fileinfo->title);
	mail_file(buf, currentuser.userid, buf2);
	return FULLUPDATE;
}

/* add by bluetent 2003.4.19 δ���ż���Ŀͳ�� */
int mail_unread()
{
	struct fileheader x;
	char path[80];
	int unread=0;
	FILE *fp;
	sprintf(path, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]), currentuser.userid);
	fp=fopen(path, "r");
	if(fp==0)
		return 0;
	int  total=file_size(path)/sizeof(x);
	int i=0;
	if(total>20)
		i=total-20;
	fseek(fp, sizeof(x)*i, SEEK_SET);
	while(fread(&x, sizeof(x), 1, fp)>0)
		if(!(x.accessed[0] & FILE_READ))
			unread++;
	fclose(fp);
	return unread;
}
int file_size(char *file)
{
	static struct stat buf;
	bzero(&buf, sizeof(buf));
	if(stat(file, &buf)==-1)
		bzero(&buf, sizeof(buf));
	return buf.st_size;
}
/* add end */
int get_mail_back()
{
	clear();
	move(0,0);
        modify_user_mode(SMAIL);  	
	char uident[STRLEN], temp[STRLEN];
	usercomplete("��Ҫ���ظ�˭���ż�: ", uident);
	if (uident[0] == '\0')
		return FULLUPDATE;
	if(!strcmp(uident,currentuser.userid)){
		move(1,0);
		prints("��û�и��,�����Լ����ż�ҲҪ����?");
		pressreturn();
		return;
	}
	strtolower(temp, uident);
	if (strcmp(temp, "sysop") == 0)
	{
		move(1,0);
		prints("�Բ���,������SYSOP���ż����ܳ���!");
		pressreturn();
		return;
	}
	FILE *fp;
	char path[STRLEN];
	sprintf(path, "mail/%c/%s/.DIR", toupper(uident[0]), uident);
	fp=fopen(path, "r");
	if(fp==0){
		move(1,0);
		prints("�ļ��򿪴���!");
		pressreturn();
		return 0;
	}

	int  total=0;
	struct fileheader x;
	int first=0;
	int i=0;
	while(fread(&x, sizeof(x), 1, fp)>0){
		i++;
		if(!(x.accessed[0] & FILE_READ))
    			//if(strstr(x.owner,currentuser.userid)){ 
			if(strcmp(currentuser.userid,strtok(x.owner," ")) == 0){
		    		if(total==0) first=i;
		    	        total++;
			}
	}
	move(1,0);
	if(total==0){
		prints("û���ҵ��ż�.�Ǻ�,�����˼�ȫ��������,�Ǿ�û����----���и�����������?");	
		fclose(fp);
		pressreturn();
		return ;
	}
	prints("���ҵ� %d ��δ���ż�,ȫ��������?\n",total);
    prints("\n(Y)��,ȫ������ (N)��,���ҿ����� (Q)�뿪 [Q]: ");
 	switch (egetch()){
		case 'y':
	    case 'Y':
			get_back(uident,first,1);
		    break;
		case 'N':
		case 'n':
			get_back(uident,first,0);
		    break;
		default:
                     ;
	}
	clear();
	fclose(fp);
    return FULLUPDATE;
}
int get_back(char *id,int first ,int flag)
{
	FILE *fp;
	char path[STRLEN],tmpfile[STRLEN];
	sprintf(path, "mail/%c/%s/.DIR", toupper(id[0]), id);
	fp=fopen(path, "r");
	if(fp==0){
		prints("�ļ��򿪴���!");
		pressreturn();
		return 0;
	}
	
	struct fileheader x;

	int j=first;
	char title[STRLEN];
	char ch=0;
        time_t time;
        fseek(fp, sizeof(struct fileheader)*(first-1), SEEK_SET);
	int i=0;
	while(fread(&x, sizeof(x), 1, fp)>0){
		if(!(x.accessed[0] & FILE_READ))
		if(strstr(x.owner,currentuser.userid)){
			 i++;
			 if(flag==0){
				clear();
				move(0,0);
                time=atoi(x.filename+2);
				prints("�� %d ��\n",i);
				prints("����:%s\n����:%s\nҪ������?",x.title,ctime(&time) + 4);
				prints("(Y)�� (N) NO (Q)�뿪 [N]: ");
				ch=egetch();
			 }
			 else ch='y';
 	        	 switch (ch){
				case 'y':
			        case 'Y':
			             sprintf(tmpfile,"mail/%c/%s/%s",toupper(id[0]),id,x.filename);
	                     
             		     sprintf(title,"[����] %s",x.title);
                             mail_file(tmpfile,currentuser.userid,title);
						 sprintf(genbuf,"rm %s",tmpfile);
						 system(genbuf);
						 delete_record(path,sizeof(struct fileheader),j);
						 j--;
						break;
				
					case 'n':
					case 'N':
   					    break;
					case 'Q':
					case 'q':
						clear();
					        fclose(fp);
						return 0;
					default:;
		  	 }
		}//if
		j++;
	}//while
	fclose(fp);
	return 0;
}

#define INETMAILER	"/home/mailer/"
#define QMAIL		"| /home/bbs/bin/bbsmail $LOCAL"
/*int inetmail()
{
	struct userec uinfo;
	int id, flag;
	char fname[256],uname[16];
	struct stat st;
	FILE *fp;
	if (!HAS_PERM(PERM_SYSOP) && !HAS_PERM(PERM_ESYSFILE))
		return 0;
	modify_user_mode(ADMIN);
	stand_title("�����û����ջ������ʼ�Ȩ��");
	if (!gettheuserid(1, "������ʹ���ߴ���: ", &id))
		return -1;
	memcpy(&uinfo, &lookupuser, sizeof(uinfo));
	strtolower(uname, uinfo.userid);
	sprintf(fname, "%s.qmail-%s", INETMAILER, uname);
	move(4, 0);
	clrtobot();
	
	flag = (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
	if (flag == 0)
	{
		if (askyn("���û���δ��ͨ�������ʼ����չ���,�����Ƿ�ͨ?", NA, NA) == YEA)
		{
			fp = fopen(fname, "w+");
			if (fp)
			{
				fprintf(fp, "%s", QMAIL);
				fclose(fp);
				prints("�Ѿ���ͨ���û��Ļ������ʼ����չ���");
			}
			else
				prints("���������ļ�����");
		}
	}
	else
	{
		if (askyn("���û��Ѿ���ͨ�������ʼ����չ���,�����Ƿ�ر�?", NA, NA) == YEA)
		{
			unlink(fname);
			prints("�Ѿ��رո��û��Ļ������ʼ����չ���");
		}
	}
	pressreturn();
	return 0;
}*/



int inetmail()
{
	int  flag;
	char fname[256],uname[16];
	struct stat st;
	FILE *fp;
	strtolower(uname, currentuser.userid);
	sprintf(fname, "%s.qmail-%s", INETMAILER, uname);
	flag = (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
	move(1, 0);
	clrtoeol();
	if (flag == 0)
	{
			fp = fopen(fname, "w+");
			if (fp)
			{
				fprintf(fp, "%s", QMAIL);
				fclose(fp);
				prints("���Ļ������ʼ����չ����Ѿ���ͨ");
			}
			else
				prints("���������ļ�ʧ��,����ϵͳά����ϵ,лл!");
	}
	else
	{
			unlink(fname);
			prints("���Ļ������ʼ����չ����Ѿ��ر�");
	}
	pressreturn();
	return 0;
}

