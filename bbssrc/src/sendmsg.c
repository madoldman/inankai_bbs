/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@@rock.b11.ingr.com
                        Guy Vega, gtvega@@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@@csie.ncu.edu.tw
 
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
$Id: sendmsg.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"
#ifdef lint
#include <sys/uio.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

extern int RMSG;
extern int msg_num;
char    buf2[STRLEN];
struct user_info *t_search();
extern struct UTMPFILE *utmpshm;

int msg_blocked=0;//add by yiyo ������ʱ��ʱ������Ϣ

void count_msg();
void r_msg();

int get_msg(char* uid, char* msg, int line)
{
	char    genbuf[3];
	/*int i;*///add for msgs
	move(line, 0);
	clrtoeol();
	prints("�����Ÿ���%s", uid);
	//	prints("�����Ÿ�:%-13s  ��������������,Ctrl+Q����",uid); //add for msgs
	memset(msg, 0, sizeof(msg));
	while (1)
	{
		getdata(line + 1, 0, "���� : ", msg, 55, DOECHO, NA);

		/* SMTH
		 i=multi_getdata(line+1,0,79,NULL,msg,MAX_MSG_SIZE,11,NA);*///MAX_MSG_SIZE=1024,defined in bbs.h add for msgs
		if (msg[0] == '\0')
			return NA;
		getdata(line + 2, 0, "ȷ��Ҫ�ͳ���(Y)�ǵ� (N)��Ҫ (E)�ٱ༭? [Y]: ",
		        genbuf, 2, DOECHO, YEA);
		if (genbuf[0] == 'e' || genbuf[0] == 'E')
			continue;
		if (genbuf[0] == 'n' || genbuf[0] == 'N')
			return NA;
		else
			return YEA;
	}
}

char msgchar(struct user_info* uin)
{
	if(uin->mode==TYPEN)
		return '#';
	if(uin->mode==FIVE||uin->mode==BBSNET)
		return '@@';
	if (isreject(uin))
		return '*';
	if ((uin->pager & ALLMSG_PAGER))
		return ' ';
	if (hisfriend(uin))
	{
		if ((uin->pager & FRIENDMSG_PAGER))
			return 'O';
		else
			return '#';
	}
	return '*';
}

int canmsg(struct user_info* uin)
{
	if(uin->mode==TYPEN)
		return NA;
	if (isreject(uin))
		return NA;
	if ((uin->pager & ALLMSG_PAGER) || HAS_PERM(PERM_SYSOP | PERM_FORCEPAGE))
		return YEA;
	if ((uin->pager & FRIENDMSG_PAGER) && hisfriend(uin))
		return YEA;
	return NA;
}
s_msg()
{
	do_sendmsg(NULL, NULL, 0, 0);
}

int show_allmsgs()
{
	char    fname[STRLEN];
	if(!strcmp(currentuser.userid,"guest"))
		return;
#ifdef LOG_MY_MESG

	setuserfile(fname, "msgfile.me");
#else

	setuserfile(fname, "msgfile");
#endif

	clear();
	modify_user_mode(LOOKMSGS);
	if (dashf(fname))
	{
		mesgmore(fname, YEA, 0, 9999, YEA);
		clear();
	}
	else
	{
		move(5, 30);
		prints("û���κε�ѶϢ���ڣ���");
		pressanykey();
		clear();
	}
}

int show_clearmsgs()
{
        char    fname[STRLEN];
        if(!strcmp(currentuser.userid,"guest"))
                return;
#ifdef LOG_MY_MESG

        setuserfile(fname, "msgfile.me.old");
#else

        setuserfile(fname, "msgfile.old");
#endif

        clear();
        modify_user_mode(LOOKMSGS);
        if (dashf(fname))
        {
                mesgmore(fname, YEA, 0, 9999, NA);
                clear();
        }
        else
        {
                move(5, 30);
                prints("û���κ������ѶϢ���ڣ���");
                pressanykey();
                clear();
        }
}

int do_sendmsg(struct user_info* uentp, char msgstr[256], int mode, int userpid)
{
	char    uident[STRLEN], ret_str[20];
	time_t  now;
	struct user_info *uin;
	char    buf[80], *msgbuf, *timestr;
#ifdef LOG_MY_MESG

	char   *mymsg, buf2[80];
	int     ishelo = 0;	/* �ǲ��Ǻ�����վ֪ͨѶϢ */
	mymsg = (char *) malloc(256);
#endif

	msgbuf = (char *) malloc(256);

	if (mode == 0)
	{
		move(2, 0);
		clrtobot();
		/*
			if (uinfo.invisible && !HAS_PERM(PERM_SYSOP)) {
					move(2, 0);
					prints("��Ǹ, �˹���������״̬�²���ִ��...\n");
					pressreturn();
					return 0;
				}
		*/
		modify_user_mode(MSG);
	}
	if (uentp == NULL)
	{
		prints("<����ʹ���ߴ���>\n");
		move(1, 0);
		clrtoeol();
		prints("��ѶϢ��: ");
		creat_list();
		namecomplete(NULL, uident);
		if (uident[0] == '\0')
		{
			clear();
			return 0;
		}
		uin = t_search(uident, NA);
		if (uin == NULL)
		{
			move(2, 0);
			prints("�Է�Ŀǰ��������...\n");
			pressreturn();
			move(2, 0);
			clrtoeol();
			return -1;
		}
		if (uin->mode == IRCCHAT || uin->mode == BBSNET
		        ||uin->mode == LOCKSCREEN || uin->mode == HYTELNET
		        ||uin->mode == GAME || uin->mode == PAGE
		        ||uin->mode == TETRIS || uin->mode == TT
		        ||uin->mode == FIVE || uin->mode ==WORKER
		        ||!canmsg(uin))
		{
			move(2, 0);
			prints("Ŀǰ�޷�����ѶϢ���Է�.\n");
			pressreturn();
			move(2, 0);
			clrtoeol();
			return -1;
		}
	}
	else
	{
		if (uentp->uid == usernum)
			return 0;
		uin = uentp;
		if (uin->mode == IRCCHAT || uin->mode == BBSNET
		        || uin->mode == HYTELNET || uin->mode == GAME
		        || uin->mode == PAGE || uin->mode == LOCKSCREEN
		        || uin->mode == FIVE || uin->mode == TETRIS
		        || uin->mode == TT || uin->mode ==WORKER
		        ||!canmsg(uin))
			return 0;
		strcpy(uident, uin->userid);
	}
	if (msgstr == NULL)
	{
		if (!get_msg(uident, buf, 1))
		{
			move(1, 0);
			clrtoeol();
			move(2, 0);
			clrtoeol();
			return 0;
		}
	}
	// add by bluetent 2003.02.23
	if (uin->mode == IRCCHAT || uin->mode == BBSNET
	        || uin->mode == HYTELNET || uin->mode == GAME
	        || uin->mode == PAGE || uin->mode == LOCKSCREEN
	        || uin->mode == FIVE || uin->mode == TETRIS
	        || uin->mode == TT || uin->mode ==WORKER
	        ||!canmsg(uin))
		return 0;
	//          sprintf(msgbuf, "[1;32mѶϢ�޷��ͳ�.[m");
	// add end
	now = time(0);
	timestr = ctime(&now) + 11;
	*(timestr + 8) = '\0';
	strcpy(ret_str, "^Z��");
	if (msgstr == NULL || mode == 2)
	{
		sprintf(msgbuf, "[0;1;44;36m%-12.12s[33m([36m%-5.5s[33m):[37m%-54.54s[31m(%s)[m[%05dm\n",
		        currentuser.userid, timestr, (msgstr == NULL) ? buf : msgstr, ret_str, uinfo.pid);
#ifdef LOG_MY_MESG

		sprintf(mymsg, "[1;32;40mTo [1;33;40m%-12.12s[m (%-5.5s):%-55.55s\n",
		        uin->userid, timestr, (msgstr == NULL) ? buf : msgstr);

		sprintf(buf2, "��ĺ����� %s �Ѿ���վ�ޣ�", currentuser.userid);

		if (msgstr != NULL)
			if (strcmp(msgstr, buf2) == 0)
				ishelo = 1;
			else if (strcmp(buf, buf2) == 0)
				ishelo = 1;
#endif

	}
	else if (mode == 0)
	{
		sprintf(msgbuf, "[0;1;5;44;33mվ�� ��[36m %8.8s [33m�㲥��[m[1;37;44m%-57.57s[m[%05dm\n",
		        timestr, msgstr, uinfo.pid);
	}
	else if (mode == 1)
	{
		sprintf(msgbuf, "[0;1;44;36m%-12.12s[37m([36m%-5.5s[37m) ������[37m%-48.48s[31m(%s)[m[%05dm\n",
		        currentuser.userid, timestr, msgstr, ret_str, uinfo.pid);
	}
	else if (mode == 3)
	{
		sprintf(msgbuf, "[0;1;45;36m%-12.12s[33m([36m%-5.5s[33m):[37m%-54.54s[31m(%s)[m[%05dm\n",
		        currentuser.userid, timestr, (msgstr == NULL) ? buf : msgstr, ret_str, uinfo.pid);
	}
	if (userpid)
	{
		if (userpid != uin->pid)
		{
			saveline(0, 0);	/* Save line */
			move(0, 0);
			clrtoeol();
			prints("[1m�Է��Ѿ�����...[m\n");
			sleep(1);
			saveline(0, 1);	/* restore line */
			return -1;
		}
	}

/*
        int tmpchenz=0;
        if( uin->mode>=20000 && !uin->active )
            tmpchenz=1;

        if( uin->mode<20000 && (!uin->active || kill(uin->pid, 0) == -1))
            tmpchenz=1;
        if(tmpchenz){

            if (msgstr == NULL) {
     	      prints("\n�Է��Ѿ�����...\n");
              pressreturn();
              clear();
            }
            return -1;

        } 
*/

	if (!uin->active)
	{
		if (msgstr == NULL)
		{
			prints("\n�Է��Ѿ�����...\n");
			pressreturn();
			clear();
		}
		return -1;
	}


	/* add by yiyo ˫���Զ�Ӧ�����ѶϢ���������һ�ֽ������ */
	if(flood_msg(msgbuf, userpid))
	{
		move(1, 0);
		clrtoeol();
		prints("����! ���ղ��Ѿ����������Ϣ��...\n");
		refresh();
		sleep(1);
		return -1;
	}
	/* add end by yiyo */
	sethomefile(buf, uident, "msgfile");
	file_append(buf, msgbuf);

#ifdef LOG_MY_MESG
	/*
	 * 990610.edwardc ������ֱ����ѶϢ��ĳ����, ������㲦��վ��
	 * ������һ�ɲ���¼ .. :)
	 */

	if (mode == 2 || (mode == 0 && msgstr == NULL))
	{
		if (ishelo == 0)
		{
			sethomefile(buf, currentuser.userid, "msgfile.me");
			file_append(buf, mymsg);
		}
	}
	sethomefile(buf, uident, "msgfile.me");
	file_append(buf, msgbuf);
	free(mymsg);

#endif

	free(msgbuf);
	if(uin->pid)
	{
		kill(uin->pid, SIGUSR2);
	}
	if (msgstr == NULL)
	{
		prints("\n���ͳ�ѶϢ...\n");
		pressreturn();
		clear();
	}
	return 1;
}

int dowall(struct user_info* uin)
{
	if (!uin->active || !uin->pid)
		return -1;
	move(1, 0);
	clrtoeol();
	prints("[1;32m���� %s �㲥.... Ctrl-D ֹͣ�Դ�λ User �㲥��[m", uin->userid);
	refresh();
	do_sendmsg(uin, buf2, 0, uin->pid);
}

int myfriend_wall(struct user_info* uin)
{
	if ((uin->pid - uinfo.pid == 0) || !uin->active || !uin->pid || isreject(uin))
		return -1;
	if (myfriend(uin->uid))
	{
		move(1, 0);
		clrtoeol();
		prints("[1;32m������ѶϢ�� %s...  [m", uin->userid);
		refresh();
		do_sendmsg(uin, buf2, 3, uin->pid);
	}
}

int hisfriend_wall(struct user_info* uin)
{
	if ((uin->pid - uinfo.pid == 0) || !uin->active || !uin->pid || isreject(uin))
		return -1;
	if (hisfriend(uin))
	{
		move(1, 0);
		clrtoeol();
		prints("[1;32m������ѶϢ�� %s...  [m", uin->userid);
		refresh();
		do_sendmsg(uin, buf2, 3, uin->pid);
	}
}

int friend_wall()
{
	char    buf[3];
	char    msgbuf[256], filename[80];
	time_t  now;
	char   *timestr;
	now = time(0);
	timestr = ctime(&now) + 11;
	*(timestr + 8) = '\0';

	/*
		if (uinfo.invisible) {
			move(2, 0);
			prints("��Ǹ, �˹���������״̬�²���ִ��...\n");
			pressreturn();
			return 0;
		}
	*/
	modify_user_mode(MSG);
	move(2, 0);
	clrtobot();
	getdata(1, 0, "��ѶϢ�� [1] �ҵĺ����ѣ�[2] ����Ϊ����: ",
	        buf, 2, DOECHO, YEA);
	switch (buf[0])
	{
	case '1':
		if (!get_msg("�ҵĺ�����", buf2, 1))
			return 0;
		if (apply_ulist(myfriend_wall) == -1)
		{
			move(2, 0);
			prints("���Ͽ���һ��\n");
			pressanykey();
		}
		else
		{
			/* ��¼��ѶϢ������ */
			sprintf(msgbuf, "[0;1;45;36m��ѶϢ������[33m([36m%-5.5s[33m):[37m%-54.54s[31m(^Z��)[m[%05dm\n",
			        timestr, buf2, uinfo.pid);
			setuserfile(filename, "msgfile.me");
			file_append(filename, msgbuf);
		}
		break;
	case '2':
		if (!get_msg("����Ϊ����", buf2, 1))
			return 0;
		if (apply_ulist(hisfriend_wall) == -1)
		{
			move(2, 0);
			prints("���Ͽ���һ��\n");
			pressanykey();
		}
		else
		{
			/* ��¼��ѶϢ������Ϊ���� */
			sprintf(msgbuf, "[0;1;45;36m�͸�����Ϊ��[33m([36m%-5.5s[33m):[37m%-54.54s[31m(^Z��)[m[%05dm\n",
			        timestr, buf2, uinfo.pid);
			setuserfile(filename, "msgfile.me");
			file_append(filename, msgbuf);

		}
		break;
	default:
		return 0;
	}
	move(6, 0);
	prints("ѶϢ�������...");
	pressanykey();
	return 1;
}

void r_msg2()
{
	FILE   *fp;
	char    buf[256];
	char    msg[256];
	char    fname[STRLEN];
	int     line, tmpansi;
	int     y, x, ch, i, j;
	int     MsgNum;
	struct sigaction act;

	if(!strcmp(currentuser.userid,"guest"))
		return ;
	getyx(&y, &x);
	if (uinfo.mode == TALK)
		line = t_lines / 2 - 1;
	else
		line = 0;
	setuserfile(fname, "msgfile");
	i = get_num_records(fname, 129);
	if (i == 0)
		return;
	signal(SIGUSR2, count_msg);
	tmpansi = showansi;
	showansi = 1;
	oflush();
	if (RMSG == NA)
	{
		saveline(line, 0);
		saveline(line + 1, 2);
	}
	MsgNum = 0;
	RMSG = YEA;
	while (1)
	{
		MsgNum = (MsgNum % i);
		if ((fp = fopen(fname, "r")) == NULL)
		{
			RMSG = NA;
			if( msg_num )
				r_msg();
			else
			{
				sigemptyset(&act.sa_mask);
				act.sa_flags = SA_NODEFER;
				act.sa_handler = r_msg;
				sigaction(SIGUSR2, &act, NULL);
			}
			return;
		}
		for (j = 0; j < (i - MsgNum); j++)
		{
			if (fgets(buf, 256, fp) == NULL)
				break;
			else
				strcpy(msg, buf);
		}
		fclose(fp);
		move(line, 0);
		clrtoeol();
		prints("%s", msg);
		refresh();
		{
			struct user_info *uin;
			char    msgbuf[STRLEN];
			int     good_id, send_pid;
			char   *ptr, usid[STRLEN];
			ptr = strrchr(msg, '[');
			send_pid = atoi(ptr + 1);
			ptr = strtok(msg + 12, " [");
			if (ptr == NULL)
				good_id = NA;
			else if (!strcmp(ptr, currentuser.userid)/*||uinfo.mode==LOCKSCREEN*/)
				good_id = NA;
			//tdhlshx lock���ܻظ�ѶϢ//������.
			else
			{
				strcpy(usid, ptr);
				uin = t_search(usid, send_pid);
				if (uin == NULL)
					good_id = NA;
				else
					good_id = YEA;
			}
			if (good_id == YEA && canmsg(uin))
			{
				int     userpid;
				userpid = uin->pid;
				move(line + 1, 0);
				clrtoeol();
				sprintf(msgbuf, "��ѶϢ�� %s: ", usid);
				getdata(line + 1, 0, msgbuf, buf, 55, DOECHO, YEA);
				if (buf[0] == Ctrl('Z'))
				{
					MsgNum++;
					continue;
				}
				else if (buf[0] == Ctrl('A'))
				{
					MsgNum--;
					if (MsgNum < 0)
						MsgNum = i - 1;
					continue;
				}
				if (buf[0] != '\0')
				{
					if (do_sendmsg(uin, buf, 2, userpid) == 1)
						sprintf(msgbuf, "[1;32m�����ͳ�ѶϢ�� %s ��![m", usid);
					else
						sprintf(msgbuf, "[1;32mѶϢ�޷��ͳ�.[m");
				}
				else
					sprintf(msgbuf, "[1;33m��ѶϢ, ���Բ��ͳ�.[m");
				move(line + 1, 0);
				clrtoeol();
				refresh();
				prints("%s", msgbuf);
				refresh();
				if (!strstr(msgbuf, "����"))
					sleep(1);
			}
			else /*if(uinfo.mode!=LOCKSCREEN){//locking can not reply message
				    		 sprintf(msgbuf,"[1;32m���Ƚ��өĻ�����ٻظ�ѶϢ.:P[m");
				    	 }
				
				else*/
				{
					sprintf(msgbuf, "[1;32m�Ҳ�����ѶϢ�� %s! �밴��:[^Z ��] "
					        "����:[^A ��] ���������뿪.[m", usid);
					move(line + 1, 0);
					clrtoeol();
					refresh();
					prints("%s", msgbuf);
					refresh();
					if ((ch = igetkey()) == Ctrl('Z') || ch == KEY_UP)
					{
						MsgNum++;
						continue;
					}
					if (ch == Ctrl('A') || ch == KEY_DOWN)
					{
						MsgNum--;
						if (MsgNum < 0)
							MsgNum = i - 1;
						continue;
					}
				}
		}
		break;
	}
	saveline(line, 1);
	saveline(line + 1, 3);
	showansi = tmpansi;
	move(y, x);
	refresh();
	RMSG = NA;
	if( msg_num )
	{
		msg_num --;
		r_msg();
	}
	else
	{
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_NODEFER;
		act.sa_handler = r_msg;
		sigaction(SIGUSR2, &act, NULL);
	}
	return;
}

void count_msg()
{
	signal(SIGUSR2,count_msg);
	msg_num++;
}

void r_msg()
{
	FILE   *fp;
	char    buf[256];
	char    msg[256];
	char    fname[STRLEN];
	int     line, tmpansi;
	int     y, x, i, j, premsg;
	char    ch;
	struct sigaction act;

	signal(SIGUSR2, count_msg);
	msg_num++;
	getyx(&y, &x);
	tmpansi = showansi;
	showansi = 1;
	if (uinfo.mode == TALK)
		line = t_lines / 2 - 1;
	else
		line = 0;
	if (DEFINE(DEF_MSGGETKEY))
	{
		oflush();
		saveline(line, 0);
		premsg = RMSG;
	}
	while (msg_num)
	{
		if (DEFINE(DEF_SOUNDMSG))
		{
			bell();
		}
		setuserfile(fname, "msgfile");
		i = get_num_records(fname, 129);
		if ((fp = fopen(fname, "r")) == NULL)
		{
			sigemptyset(&act.sa_mask);
			act.sa_flags = SA_NODEFER;
			act.sa_handler = r_msg;
			sigaction(SIGUSR2, &act, NULL);
			return;
		}
		for (j = 0; j <= (i - msg_num); j++)
		{
			if (fgets(buf, 256, fp) == NULL)
				break;
			else
				strcpy(msg, buf);
		}
		fclose(fp);
		move(line, 0);
		clrtoeol();
		prints("%s", msg);
		refresh();
		msg_num--;
		if (DEFINE(DEF_MSGGETKEY))
		{
			RMSG = YEA;
			ch = 0;
#ifdef MSG_CANCLE_BY_CTRL_C

			while (ch != Ctrl('C'))
#else

			while (ch != '\r' && ch != '\n')
#endif

			{
				ch = igetkey();
#ifdef MSG_CANCLE_BY_CTRL_C

				if (ch == Ctrl('C'))
					break;
#else

				if (ch == '\r' || ch == '\n')
					break;
#endif

				else if(ch==Ctrl('R')||ch=='R'||ch=='r'||ch==Ctrl('Z'))
				{
					struct user_info *uin;
					char    msgbuf[STRLEN];
					int     good_id, send_pid;
					char   *ptr, usid[STRLEN];

					ptr = strrchr(msg, '[');
					send_pid = atoi(ptr + 1);
					ptr = strtok(msg + 12, " [");
					if (ptr == NULL)
						good_id = NA;
					else if (!strcmp(ptr, currentuser.userid)/*||uinfo.mode==LOCKSCREEN*/)
						good_id = NA;
					/*modified by tdhlshx ����״̬���ܻظ�ѶϢ03.11.8*/
					else
					{
						strcpy(usid, ptr);
						uin = t_search(usid, send_pid);
						if (uin == NULL)
							good_id = NA;
						else
							good_id = YEA;
					}
					oflush();
					saveline(line + 1, 2);
					if (good_id == YEA)
					{
						int     userpid;
						userpid = uin->pid;
						move(line + 1, 0);
						clrtoeol();
						sprintf(msgbuf, "������ѶϢ�� %s: ", usid);
						getdata(line + 1, 0, msgbuf, buf, 55, DOECHO, YEA);
						if(buf[0]!='\0'&&buf[0]!=Ctrl('Z')&&buf[0]!=Ctrl('A'))
						{
							if (do_sendmsg(uin, buf, 2, userpid))
								sprintf(msgbuf,"[1;32m�����ͳ�ѶϢ�� %s ��![m",usid);
							else
								sprintf(msgbuf, "[1;32mѶϢ�޷��ͳ�.[m");
						}
						else
							sprintf(msgbuf, "[1;33m��ѶϢ, ���Բ��ͳ�. [m");
					}
					else/* if(uinfo.mode!=LOCKSCREEN){
						    		       sprintf(msgbuf,"[1;32m���Ƚ��өĻ�����ٻظ�ѶϢ.:P[m");
						    	       }
						;
						else*/
						{
							sprintf(msgbuf, "[1;32m�Ҳ�����ѶϢ�� %s.[m", usid);
						}
					move(line + 1, 0);
					clrtoeol();
					refresh();
					prints("%s", msgbuf);
					refresh();
					if (!strstr(msgbuf, "����"))
						sleep(1);
					saveline(line + 1, 3);
					refresh();
					break;
				}	/* if */
			}	/* while */
		}		/* if */
	}		/* while */
	/*
	   sigemptyset(&act.sa_mask);
	   act.sa_flags = SA_NODEFER;
	   act.sa_handler = r_msg;
	   sigaction(SIGUSR2, &act, NULL); 
	*/
	if (!msg_blocked)
	{
		struct sigaction act;
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_NODEFER;
		act.sa_handler = r_msg;
		sigaction(SIGUSR2, &act, NULL);
	}

	if (DEFINE(DEF_MSGGETKEY))
	{
		RMSG = premsg;
		saveline(line, 1);
	}
	showansi = tmpansi;
	move(y, x);
	refresh();
	return;
}

/* add by yiyo ����������ʱ������Ϣ */
void block_msg()
{
	signal(SIGUSR2, count_msg);
	msg_blocked=1;
}

void unblock_msg()
{
	struct sigaction act;
	msg_blocked=0;
	if (msg_num)
		r_msg();
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NODEFER;
	act.sa_handler = r_msg;
	sigaction(SIGUSR2, &act, NULL);
}
/* add end */

/* add by yiyo ˫���Զ�Ӧ�����ѶϢ���������һ�ֽ������ */
int flood_msg(char *msgbuf, int pid)
{
	static char last_buf[256]="";
	static int last_pid=0;
	static int last_time=0;
	if(abs(time(0)-last_time)>10 || pid!=last_pid || strcmp(msgbuf, last_buf))
	{
		last_time=time(0);
		last_pid=pid;
		strncpy(last_buf, msgbuf, 256);
		return 0;
	}
	else
	{
		return 1;
	}
}
/* add end */

int friend_login_wall(struct user_info* pageinfo)
{
	char    msg[STRLEN];
	int     x, y;
	if (!pageinfo->active || !pageinfo->pid || isreject(pageinfo))
		return 0;
	if (hisfriend(pageinfo))
	{
		if (getuser(pageinfo->userid) <= 0)
			return 0;
		if (!(lookupuser.userdefine & DEF_LOGINFROM))
			return 0;
		if (pageinfo->uid ==usernum)
			return 0;
		/* edwardc.990427 ��������Ͳ���ʾ�ͳ���վ֪ͨ */
		if (pageinfo->invisible)
			return 0;
		getyx(&y, &x);
		if (y > 22)
		{
			pressanykey();
			move(7, 0);
			clrtobot();
		}
		prints("�ͳ�������վ֪ͨ�� %s\n", pageinfo->userid);
		sprintf(msg, "��ĺ����� %s �Ѿ���վ�ޣ�", currentuser.userid);
		do_sendmsg(pageinfo, msg, 2, pageinfo->pid);
	}
	return 0;
}


/* add by bluetent ��ȡѶϢ 2003.01.18 */
int AbstractMsg()
{
	FILE *fp, *fp2;
	char buf[256],buf2[256],buf3[256];
	char abstract[100][14];
	int i;
	clear();
	modify_user_mode(LOOKMSGS);
	setuserfile(buf, "msgfile.me");
	if(!dashf(buf))
	{
		move (10,10);
		prints("��û���κ�ѶϢ����");
		pressanykey();
		return FULLUPDATE;
	}
	sethomefile(buf, currentuser.userid, "abstractmsg");
	ListFileEdit(buf,"������ȡ��Щ�û���ѶϢ����༭���б�(Ŀǰ֧��100���û�)");
	if(fp2=fopen(buf,"r"))
	{
		i=0;
		while(fgets(buf, 20, fp2))
		{
			strtok(buf, " \n\r\t");
			strncpy(abstract[i], buf, 20);
			i++;
		}
		fclose(fp2);
	}
	else
		return 1;
	abstract[i+1][0] = '\0';
	sprintf(buf2,"tmp/%d.abstract",getpid());
	setuserfile(buf, "msgfile.me");
	i=0;
	while(abstract[i][0]!='\0')
	{
		if((fp=fopen(buf2, "a"))&&(fp2=fopen(buf, "r")))
		{
			while(1)
			{
				//if(fread(buf3, 256, 1, fp2))
				if(fgets(buf3, 256, fp2)!=NULL)
				{
					//if(strncmp(buf3, abstract[i], 3)==0)
					//if(strstr(buf3, abstract[i]))
					if(!strncmp(buf3+12, abstract[i], strlen(abstract[i]))&&(!strncmp(buf3+12+strlen(abstract[i])," ",1)||!strncmp(buf3+12+strlen(abstract[i]),"(",1)||(strlen (abstract[i]) == 12))
					        ||(!strncmp(buf3+23, abstract[i], strlen(abstract[i]))&&(!strncmp(buf3+23+strlen(abstract[i])," ",1)||!strncmp(buf3+23+strlen(abstract[i]),"(",1)||(strlen (abstract[i]) == 12))))
						fprintf(fp, "%s", buf3);
				}
				else
					break;
			}
			fclose(fp);
			fclose(fp2);
		}
		//sprintf(buf, "grep %s %s/home/%c/%s/msgfile.me > %s/%s", abstract[i], BBSHOME, toupper(currentuser.userid[0]), currentuser.userid, BBSHOME, buf2);
		//		system(buf);
		sprintf(buf3, "����ȡ��ѶϢ{%s}", abstract[i]);
		mail_file(buf2, currentuser.userid, buf3);
		unlink(buf2);
		i++;
	}
	clear();
	move(10,10);
	prints("�Ѿ�����ȡ����ѶϢ��������������");
	pressanykey();
	return FULLUPDATE;
}

/* add by bluetent ѶϢ���� */
int SearchMsg()
{
	FILE *fp, *fp2;
	char key[80], buf[256], buf2[256], genbuf[256];
	clear();
	modify_user_mode(LOOKMSGS);
	setuserfile(buf, "msgfile.me");
	if(!dashf(buf))
	{
		move (10,10);
		prints("��û���κ�ѶϢ����");
		pressanykey();
		return FULLUPDATE;
	}
	sprintf(buf2,"tmp/%d.searchmsg",getpid());
	getdata(3,0,"������Ҫ�����Ĺؼ��֣�",key,40,DOECHO,YEA);
	if( key[0] == '\0' )
		return 0;
	if((fp=fopen(buf, "r"))==NULL)
		return 0;
	if((fp2=fopen(buf2, "a"))==NULL)
		return 0;
	while(1)
	{
		if(fgets(genbuf, 256, fp)==NULL)
			break;
		if(strstr(genbuf, key)!=NULL)
		{
			fprintf(fp2, "%s", genbuf);
		}
	}
	fclose(fp);
	fclose(fp2);
	sprintf(genbuf, "����ȡ��ѶϢ{�ؼ���:%s}", key);
	mail_file(buf2, currentuser.userid, genbuf);
	unlink(buf2);
	prints("�Ѿ�����ȡ����ѶϢ��������������");
	pressanykey();
	return 1;
}
