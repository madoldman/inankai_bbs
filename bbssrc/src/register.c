//deardrago 2000.09.27  over
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
$Id: register.c,v 1.14 2009-12-24 03:38:36 madoldman Exp $
*/

#include "bbs.h"

#ifndef DLM
#undef  ALLOWGAME
#endif
#define SAVELIVE
char   *sysconf_str();
char   *genpasswd();
extern int	forceresolve;

extern char fromhost[60];
extern time_t login_start_time;

time_t  system_time;

#ifdef ALLOWSWITCHCODE
extern int convcode;
#endif
int CleanAccount(char *userid)
{
	if(userid==NULL)
		return -1;
	if(strlen(userid)>20)
		return -1;

	sprintf(genbuf,"mail/%c/%s",toupper(userid[0]),userid);
	if(!strncmp(genbuf+7,userid,strlen(userid)))
		f_rm(genbuf);
	sprintf(genbuf, "home/%c/%s",toupper(userid[0]),userid);
	if(!strncmp(genbuf+7,userid,strlen(userid)))
		f_rm(genbuf);
	sprintf(genbuf, "0Announce/PersonalCorpus/%c/%s",toupper(userid[0]),userid);
	if(!strncmp(genbuf+27,userid,strlen(userid)))
		f_rm(genbuf);
	sprintf(genbuf, "/home/www/upload/qmd/%c/%s.*", toupper(userid[0]), userid);
	if(!strncmp(genbuf+23,userid,strlen(userid)))
		f_rm(genbuf);

	return 0;

}//loveni:ɾ���ʺ���Ϣ

/*
int killPersonalCorpus(char *userid)
{
	char cmd[160];
	sprintf(cmd, "rm -rf 0Announce/PersonalCorpus/%c/%s",toupper(userid[0]),userid);
	system(cmd);
}//loveni:ɾ�������ļ�

int delqmdpic(char *userid)
{
	char cmd[160];
	sprintf(cmd, "rm -f /home/www/upload/qmd/%c/%s.*", toupper(userid[0]), userid);
	system(cmd);
}//ɾ��qmdͼƬ
*/


int bad_user_id(char *userid)
{
	FILE   *fp;
	char    buf[STRLEN], ptr2[IDLEN + 2],*ptr, ch;

	ptr = userid;
	while ((ch = *ptr++) != '\0')
	{
		if (!isalnum(ch) && ch != '_')
			return 1;
	}
	if( !strcasecmp(userid,BBSID) )
		return 1;
	if ((fp = fopen(".badname", "r")) != NULL)
	{
		strtolower(ptr2, userid);
		while (fgets(buf, STRLEN, fp) != NULL)
		{
                        if(!strcmp(buf,"\n")) continue; 
			ptr = strtok(buf, " \n\t\r");
               		strtolower(ptr,ptr); //add by yiyo ���ò���ע��ID���ִ�Сд
			if (ptr != NULL && *ptr != '#')
			{
				if(  (ptr[0] == '*' && strstr(ptr2, &ptr[1]) != NULL)
				        ||(ptr[0] != '*' && !strcmp(ptr2,ptr)) )
				{
					fclose(fp);
					return 1;
				}
			}
		}
		fclose(fp);
	}
	return 0;
}

int recent_killed_id(char* userid)
{
	char filename[STRLEN],userid_upper[IDLEN + 2];
	int i;
	static struct stat buf;
	size_t length;
	length = strlen(userid);
	for(i = 0;i < length; i ++)
		userid_upper[i] = userid[i];
	userid_upper[i] = '\0';
	sprintf(filename, "%s/recent_killed_id/%s", BBSHOME, userid_upper);
	bzero(&buf, sizeof(buf));
	if(stat(filename, &buf) == -1) 
		return 0;
	if(time(NULL) - buf.st_mtime > 30 * 24 * 60 * 60 )
	{
		unlink(filename);
		return 0;
	}
	return 1;
}
/*
int check_nkbbs_user(char* userid, char* uuid)
{
	FILE* fp;
	int result = 1;
	char buf[STRLEN], filepath[STRLEN];
	if(!userid || !uuid)
		return 2;
	sprintf(buf,"%s/bin/checkuser2.sh '%s' '%s'",BBSHOME, userid,uuid);
	sprintf(filepath, "%s/tmp/result_%s", BBSHOME, userid);
	system(buf);
	fp = fopen(filepath,"r");
	if(!fp) 
		return 2;
	fscanf(fp,"%d",&result);
	fclose(fp);
	unlink(filepath);
	return result;
}
*/

int compute_user_value(struct userec *urec)
{
	int     value;
        /*SȨ�ޣ�guest�������ʺ� 999��������*/
	if (  (urec->userlevel & PERM_XEMPT)
	        || strcmp(urec->userid, "SYSOP") == 0
	        || strcmp(urec->userid, "guest") == 0)
		return 999;
	//add for school_id by swen
	sprintf(genbuf, "home/%c/%s/life666", toupper(urec->userid[0]), urec->userid);
	if(dashf(genbuf)) return 666;//add by loveni
        /*new*/ 
	value = (time(0) - urec->lastlogin) / 60;//����Ϊֹ�û�û�е�½�ķ�����
	/* new user should register in 30 mins */
	if (strcmp(urec->userid, "new") == 0)
	{
		return (30 - value) * 60;
	}


        /*��ɱ���������㣬15���ȥδ��½����  ����Ա������ɱ*/
	/*if (!(urec->userlevel & PERM_SYSOP)&&(urec->userlevel & PERM_SUICIDE))
		return (15 * 1440 - value) / 1440;*/

        /*��½����С�ڵ���1���ʺŴ���*/
	if (urec->numlogins <= 1)
		return (15 * 1440 - value) / 1440;
		//return 100;

        /*û��ͨ��ע����ʺŴ�������һ����*/
	if (!(urec->userlevel & PERM_LOGINOK))
		return (30 * 1440 - value) / 1440;
		//return 100;

	/*  ȥ��ע�����������ʺ�*/
       int exp=countexp(urec);
       int posts=urec->numposts;
       int days=(time(0)-(urec->firstlogin))/86400;
       if(days >= 365*3 && exp > 1500 && posts > 300)
          	      return 666;
        /*��ͨ�ʺŹ�������������, ֻҪ����������������365���������оͲ�����
          ���Ĺ������ʺ�����������
        */
/*	if (!(urec->userlevel & PERM_SYSOP)&&(urec->userlevel & PERM_RECLUSION)){*/
		/*if ((365 * 1440 -value) / 1440 < 100)
			return 100;
		else*/	
/*			return (365 * 1440 - value) / 1440;
	}*/

      int life = 200;
      if (urec->stay > 1000000){
          	life = 365;
    	  	if (exp > 3000 && posts > 300 )     
			  		life=465;
   	  		if (exp > 5000 && posts>800 )     
		  		life = 565;
      }
	  if ((life * 1440 - value) / 1440 < 0)
			return 1;
	  else
			return (life * 1440 - value) / 1440;
     /*return (life * 1440 - value) / 1440; 
   
   
	if (urec->stay > 1000000){
		if ((365 * 1440 - value) / 1440 < 100)
			return 100;
		else
			return (365 * 1440 - value) / 1440;
	}

        
	if ((200 * 1440 -value) / 1440 < 100)
		return 100;
	else
        
	return (200 * 1440 - value) / 1440;
	*/
}

#ifndef SAVELIVE
int compute_user_value2(struct userec *urec)
{
	int     value;
	if (  (urec->userlevel & PERM_XEMPT)|| strcmp(urec->userid, "SYSOP") == 0
	        || strcmp(urec->userid, "guest") == 0)
		return 999;
	
	value = (time(0) - urec->lastlogin) / 60;//����Ϊֹ�û�û�е�½�ķ�����
	/* new user should register in 30 mins */
	if (strcmp(urec->userid, "new") == 0)
	{
		return (30 - value) * 60;
	}


        /*��ɱ���������㣬15���ȥδ��½����  ����Ա������ɱ*/
	if (!(urec->userlevel & PERM_SYSOP)&&(urec->userlevel & PERM_SUICIDE))
		return (15 * 1440 - value) / 1440;

        /*��½����С�ڵ���1���ʺŴ���*/
	if (urec->numlogins <= 1)
		return (15 * 1440 - value) / 1440;
		//return 100;

        /*û��ͨ��ע����ʺŴ�������һ����*/
	if (!(urec->userlevel & PERM_LOGINOK))
		return (30 * 1440 - value) / 1440;
		//return 100;
	
	
	/*  ȥ��ע�����������ʺ�*/
       int exp=countexp(urec);
       int posts=urec->numposts;
       int days=(time(0)-(urec->firstlogin))/86400;
       if(days>=365*3){
   	        if(exp>1500 && posts>300)
          	      return 666;
       }

        /*��ͨ�ʺŹ�������������, ֻҪ����������������365���������оͲ�����
          ���Ĺ������ʺ�����������
        */
/*	if (!(urec->userlevel & PERM_SYSOP)&&(urec->userlevel & PERM_RECLUSION)){*/
		/*if ((365 * 1440 -value) / 1440 < 100)
			return 100;
		else*/	
/*			return (365 * 1440 - value) / 1440;
	}*/

      int life=200;
      if (urec->stay > 1000000){
          life=365;
    	  if (exp > 3000 && posts>300 )     life=465;
   	  if (exp > 5000 && posts>800 )     life=565;
      }
      
      return (life * 1440 - value) / 1440; 
   
   



/*	if (urec->stay > 1000000){
		if ((365 * 1440 -value) / 1440 < 100)
			return 100;
		else
			return (365 * 1440 - value) / 1440;
	}

        
	if ((200 * 1440 -value) / 1440 < 100)
		return 100;
	else
        
	return (200 * 1440 - value) / 1440;*/
}

#endif

int getnewuserid()
{
	struct userec utmp;
#ifndef SAVELIVE

	struct userec zerorec;
	int size;
#endif

	struct stat st;
	int     fd, val, i;

	system_time = time(0);
#ifndef SAVELIVE

	if (stat("tmp/killuser", &st)==-1 || st.st_mtime+3600 < system_time)
	{
		if ((fd = open("tmp/killuser", O_RDWR | O_CREAT, 0600)) == -1)
			return -1;
		getdatestring(system_time,NA);
		write(fd, datestring, 29);
		close(fd);
		log_usies("CLEAN", "dated users.");
		prints("Ѱ�����ʺ���, ���Դ�Ƭ��...\n");
		memset(&zerorec, 0, sizeof(zerorec));
		if ((fd = open(PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
			return -1;
		flock(fd, LOCK_EX);
		size = sizeof(utmp);
		for (i = 0; i < MAXUSERS; i++)
		{
			if (read(fd, &utmp, size) != size)
				break;
			val = compute_user_value2(&utmp);
			if(val<0) val=compute_user_value(&utmp);//loveni:�������666������new��ʱ��
								//����������û�����bug
			if (utmp.userid[0] != '\0' && val < 0)
			{
				getdatestring(utmp.lastlogin,NA);
				sprintf(genbuf, "#%5d %-12s %14.14s %5d %5d %5d", i + 1,
				        utmp.userid, datestring, utmp.numlogins, utmp.numposts, val);
				log_usies("KILL ", genbuf);
				if (!bad_user_id(utmp.userid))
				{
					//ɾ�������û���������֤
			//		sprintf(genbuf, "mail/%c/email.auth",
			//				toupper(GetAuthUserMailF(utmp.userid)));
			//		del_authrow_has_userid(genbuf, utmp.userid);
			//		strcat(genbuf, ".U");
        		//	del_authrow_has_userid(genbuf, utmp.userid);
					
					
				/*	sprintf(genbuf,"mail/%c/%s",
					        toupper(utmp.userid[0]), utmp.userid);
					if(!strncmp(genbuf+7,utmp.userid,strlen(utmp.userid)))
						f_rm(genbuf);
					sprintf(genbuf, "home/%c/%s",
					        toupper(utmp.userid[0]), utmp.userid);
					if(!strncmp(genbuf+7,utmp.userid,strlen(utmp.userid)))
						f_rm(genbuf);
					killPersonalCorpus(utmp.userid);
					delqmdpic(utmp.userid);
					*///loveni:�����ʺŵ�ʱ����ɾ����Ϣ����Ϊע���ʱ��ɾ��
				}
				if(lseek(fd, (off_t)(-size), SEEK_CUR)==-1)
				{
					flock(fd, LOCK_UN);
					close(fd);
					return -1;
				}
				write(fd, &zerorec, sizeof(utmp));
			}
		}
		flock(fd, LOCK_UN);
		close(fd);
		touchnew();
	}
#endif   // of SAVELIVE

	if ((fd = open(PASSFILE, O_RDWR | O_CREAT, 0600)) == -1)
		return -1;
	flock(fd, LOCK_EX);
	i = searchnewuser();
	sprintf(genbuf, "uid %d from %s", i, fromhost);
	log_usies("APPLY", genbuf);
	if (i <= 0 || i > MAXUSERS)
	{
		flock(fd, LOCK_UN);
		close(fd);
		prints("��Ǹ, ʹ�����ʺ��Ѿ�����, �޷�ע���µ��ʺ�.\n\r");
		val = (st.st_mtime - system_time + 3660) / 60;
		prints("��ȴ� %d ����������һ��, ף�����.\n\r", val);
		sleep(2);
		exit(1);
	}
	memset(&utmp, 0, sizeof(utmp));
	strcpy(utmp.userid, "new");
	utmp.lastlogin = time(0);
	if (lseek(fd, (off_t)(sizeof(utmp) * (i - 1)), SEEK_SET) == -1)
	{
		flock(fd, LOCK_UN);
		close(fd);
		return -1;
	}
	write(fd, &utmp, sizeof(utmp));
	setuserid(i, utmp.userid);
	flock(fd, LOCK_UN);
	close(fd);
	return i;
}

int id_with_num(char userid[IDLEN + 1])
{
	char   *s;
	for (s = userid; *s != '\0'; s++)
		if (*s < 1 || !isalpha(*s))
			return 1;
	return 0;
}

void new_register()
{
	struct userec newuser;
	char    passbuf[STRLEN], uuid[33];
	int     allocid, try;
	forceresolve=1;
	resolve_ucache();
	forceresolve=0;

	if (dashf("NOREGISTER") )
	{
		ansimore("NOREGISTER", NA);
		pressreturn();
		exit(1);
	}
	if(!isdormip(fromhost) && !isAllowedIp2(fromhost))
	{
	        prints("�Բ��𣬱�վֹͣ�����ʺ�ע��\n");
	        pressreturn();
	        exit(1);
	}
	ansimore("etc/register", NA);
	try = 0;
/*	generate_uuid2(uuid);
	getdata(0, 0, "������������:",uuid, 33, DOECHO, YEA);
	if(!valid_uuid(uuid)){
		prints("�Բ��������벻��ȷ\n");
		pressreturn();
		exit(1);
	}*/
	while (1)
	{
		if (++try
		        >= 9)
		{
			prints("\n�ݰݣ���̫����  <Enter> ��...\n");
			refresh();
			longjmp(byebye, -1);
		}
		getdata(0, 0, "�������ʺ����� (Enter User ID, \"0\" to abort): ",
		        passbuf, IDLEN + 1, DOECHO, YEA);
		if (passbuf[0] == '0')
		{
			longjmp(byebye, -1);
		}
		if (id_with_num(passbuf))
		{
			prints("�ʺű���ȫΪӢ����ĸ!\n");
		}
		else if (strlen(passbuf) < 2)
		{
			prints("�ʺ�������������Ӣ����ĸ!\n");
		}
		else if ((*passbuf == '\0') || bad_user_id(passbuf) || recent_killed_id(passbuf))
		{
			prints("��Ǹ, ������ʹ���������Ϊ�ʺš� ���������һ����\n");
		}
		else if (dosearchuser(passbuf))
		{
			prints("���ʺ��Ѿ�����ʹ��\n");
		}
		else{
			/*int result = check_nkbbs_user(passbuf,uuid);
			if(result == 2)
			{*/
			//prints("�����bbs.nankai.edu.cn�ϴ�����Ϊ%s��ID\n",passbuf);
			//prints("������Ҫ��¼����վ���Ÿ�news˵�������Ż�ͨ����֤�������½⡣\n");
			//pressreturn();
			/*	prints("\n");
				result = check_nkbbs_user(passbuf, uuid);
				if(result == 2){
					prints("\033[1;33m��֤����\033[0m\n");
					pressreturn();
					longjmp(byebye, -1);
				}else
					break;
			}
			else*/
			break;
		}
	}

	memset(&newuser, 0, sizeof(newuser));
	allocid = getnewuserid();
	if (allocid > MAXUSERS || allocid <= 0)
	{
		prints("No space for new users on the system!\n\r");
		exit(1);
	}
	strcpy(newuser.userid, passbuf);
	strcpy(passbuf, "");
	while (1)
	{
		getdata(0, 0, "���趨�������� (Setup Password, \"0\" to abort): ",
		        passbuf, PASSLEN, NOECHO, YEA);
		
                if (passbuf[0] == '0'&&strlen(passbuf)==1) {
     			    longjmp(byebye, -1);
 		}
                
		if (strlen(passbuf) < 4 || !strcmp(passbuf, newuser.userid))
         	{              

			prints("����̫�̻���ʹ���ߴ�����ͬ, ����������\n");
			continue;
		}
		strncpy(newuser.passwd, passbuf, PASSLEN);
		getdata(0, 0, "��������һ��������� (Reconfirm Password): ",
		        passbuf, PASSLEN, NOECHO, YEA);
		if (strncmp(passbuf, newuser.passwd, PASSLEN) != 0)
		{
			prints("�����������, ��������������.\n");
			continue;
		}
		passbuf[8] = '\0';
#ifdef ENCPASSLEN

		strncpy(newuser.passwd, genpasswd(passbuf), ENCPASSLEN);
#else

		strncpy(newuser.passwd, genpasswd(passbuf), PASSLEN);
#endif

		break;
	}

	strcpy(newuser.termtype, "vt100");
	newuser.gender = 'X';
#ifdef ALLOWGAME

	newuser.money = 1000 ;
#endif

	newuser.userdefine = -1;
	if (!strcmp(newuser.userid, "guest"))
	{
		newuser.userlevel = 0;
		newuser.userdefine &= ~(DEF_FRIENDCALL | DEF_ALLMSG | DEF_FRIENDMSG);
	}
	else
	{
		newuser.userlevel = PERM_BASIC;
		newuser.flags[0] =  PAGER_FLAG;
	}

	newuser.userdefine &= ~(DEF_NOLOGINSEND);
#ifdef ALLOWSWITCHCODE
/*
	if (convcode)
		newuser.userdefine&=~DEF_USEGB;*/
#endif
	newuser.userdefine&=~DEF_USEGB;

	newuser.flags[1] = 0;
	newuser.firstlogin = newuser.lastlogin = time(0);
	if (substitute_record(PASSFILE, &newuser, sizeof(newuser),allocid) == -1)
	{
		prints("too much, good bye!\n");
		oflush();
		sleep(2);
		exit(1);
	}
//	save_uuid(uuid, newuser.userid);
	setuserid(allocid, newuser.userid);
	if (!dosearchuser(newuser.userid))
	{
		prints("User failed to create\n");
		oflush();
		sleep(2);
		exit(1);
	}
	report("new account");
	CleanAccount(newuser.userid);

}

/*
char   *
trim(s)
char   *s;
{
	char   *buf;
	char   *l, *r;
	buf = (char *) malloc(256);
 
	buf[0] = '\0';
	r = s + strlen(s) - 1;
 
	for (l = s; strchr(" \t\r\n", *l) && *l; l++);
 
	// if all space, *l is null here, we just return null 
	if (*l != '\0') {
		for (; strchr(" \t\r\n", *r) && r >= l; r--);
		strncpy(buf, l, r - l + 1);
	}
	return buf;
}
*/

int invalid_email(char *addr)
{
	FILE   *fp;
	char    temp[STRLEN], tmp2[STRLEN];

	if(strlen(addr)<3)
		return 1;

	strtolower(tmp2, addr);
	if ( strstr(tmp2, "bbs") != NULL)
		return 1;

	if ((fp = fopen(".bad_email", "r")) != NULL)
	{
		while (fgets(temp, STRLEN, fp) != NULL)
		{
			strtok(temp, "\n");
			strtolower(genbuf, temp);
			if (strstr(tmp2, genbuf)!=NULL||strstr(genbuf,tmp2)!= NULL)
			{
				fclose(fp);
				return 1;
			}
		}
		fclose(fp);
	}
	return 0;
}

int check_register_ok(void)
{
	FILE   *fn;
	char   fname[STRLEN];

	sethomefile(fname, currentuser.userid, "register");
	if ((fn = fopen(fname, "r")) != NULL)
	{
		fgets(genbuf, STRLEN, fn);
		fclose(fn);
		strtok(genbuf, "\n");
		if (   valid_ident(genbuf) && ((strchr(genbuf, '@') != NULL)
		                               || strstr(genbuf, "usernum")))
		{
			move(21, 0);
			prints("������!! ����˳����ɱ�վ��ʹ����ע������,\n");
			prints("������������ӵ��һ��ʹ���ߵ�Ȩ��������...\n");
			pressanykey();
			return 1;
		}
	}
	return 0;
}
#ifdef MAILCHECK
#ifdef CODE_VALID
char *genrandpwd(int seed)
{
	char panel[]="1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char   *result;
	int     i, rnd;

	result = (char *) malloc(RNDPASSLEN + 1);
	srand((unsigned) (time(0) * seed));
	bzero(result, RNDPASSLEN + 1);
	for (i = 0; i < RNDPASSLEN; i++)
	{
		rnd = rand() % sizeof(panel);
		if ( panel[rnd] == '\0' )
		{
			i--;
			continue;
		}
		result[i] = panel[rnd];
	}
	sethomefile(genbuf, currentuser.userid, ".regpass");
	unlink(genbuf);
	file_append(genbuf, result);
	return ((char *) result);
}
#endif

void send_regmail( struct userec *trec )
{
	time_t  code;
	FILE   *fin, *fout, *dp;
#ifdef CODE_VALID

	char    buf[RNDPASSLEN + 1];
#endif

	sethomefile(genbuf, trec->userid, "mailcheck");
	if ((dp = fopen(genbuf, "w")) == NULL)
		return;
	code = time(0);
	fprintf(dp, "%9.9d:%d\n", code, getpid());
	fclose(dp);

	sprintf(genbuf, "%s -f %s.bbs@%s %s ",
	        SENDMAIL, trec->userid, BBSHOST, trec->email);
	fout = popen(genbuf, "w");
	fin = fopen("etc/mailcheck", "r");
	if ((fin != NULL) && (fout != NULL))
	{
		fprintf(fout, "Reply-To: SYSOP.bbs@%s\n", BBSHOST);
		fprintf(fout, "From: SYSOP.bbs@%s\n", BBSHOST);
		fprintf(fout, "To: %s\n", trec->email);
		fprintf(fout, "Subject: @%s@[-%9.9d:%d-]%s mail check.\n", trec->userid, code, getpid(), BBSID);
		fprintf(fout, "X-Purpose: %s registration mail.\n", BBSNAME);
		fprintf(fout, "\n");
		fprintf(fout, "[����]\n");
		fprintf(fout, "BBS λַ         : %s (%s)\n", BBSHOST, BBSIP);
		fprintf(fout, "��ע��� BBS ID  : %s\n", trec->userid);
		fprintf(fout, "��������         : %s", ctime(&trec->firstlogin));
		fprintf(fout, "������Դ         : %s\n", fromhost);
		fprintf(fout, "������ʵ����/�ǳ�: %s (%s)\n", trec->realname, trec->username);
#ifdef CODE_VALID

		sprintf(buf, "%s", (char *) genrandpwd((int) getpid()));
		fprintf(fout, "��֤����         : %s (��ע���Сд)\n", buf);
#endif

		fprintf(fout, "��֤�ŷ�������   : %s\n", ctime(&code));

		fprintf(fout, "[English]\n");
		fprintf(fout, "BBS LOCATION     : %s (%s)\n", BBSHOST, BBSIP);
		fprintf(fout, "YOUR BBS USER ID : %s\n", trec->userid);
		fprintf(fout, "APPLICATION DATE : %s", ctime(&trec->firstlogin));
		fprintf(fout, "LOGIN HOST       : %s\n", fromhost);
		fprintf(fout, "YOUR NICK NAME   : %s\n", trec->username);
		fprintf(fout, "YOUR NAME        : %s\n", trec->realname);
#ifdef CODE_VALID

		fprintf(fout, "VALID CODE       : %s (case sensitive)\n", buf);
#endif

		fprintf(fout, "THIS MAIL SENT ON: %s\n", ctime(&code));

		while (fgets(genbuf, 255, fin) != NULL)
		{
			if (genbuf[0] == '.' && genbuf[1] == '\n')
				fputs(". \n", fout);
			else
				fputs(genbuf, fout);
		}
		fprintf(fout, ".\n");
		fclose(fin);
		fclose(fout);
	}
}
#endif

void check_register_info()
{
	struct userec *urec = &currentuser;
	FILE   *fout;
	char    buf[192], buf2[STRLEN];
#ifdef MAILCHECK

	char    ans[4];
#ifdef CODE_VALID

	int     i;
#endif
#endif

	if (!(urec->userlevel & PERM_BASIC))
	{
		urec->userlevel = 0;
		return;
	}
#ifdef NEWCOMERREPORT
	if (urec->numlogins == 1)
	{
		clear();
		sprintf(buf, "tmp/newcomer.%s", currentuser.userid);
		if ((fout = fopen(buf, "w")) != NULL)
		{
			fprintf(fout, "��Һ�,\n\n");
			fprintf(fout, "���� %s (%s), ���� %s\n",
			        currentuser.userid, urec->username, fromhost);
			fprintf(fout, "����%s������վ����, ���Ҷ��ָ�̡�\n",
			        (urec->gender == 'M') ? "С��" : "СŮ��");
			move(2, 0);
			prints("�ǳ���ӭ %s ���ٱ�վ��ϣ�������ڱ�վ�ҵ������Լ���һƬ��գ�\n\n", currentuser.userid);
			prints("����������̵ĸ��˼��, ����д�����Ա�վ������\n");
			prints("(�������, д���ֱ�Ӱ� <Enter> ����)....");
			getdata(6, 0, ":", buf2, 75, DOECHO, YEA);
			if (buf2[0] != '\0')
			{
				fprintf(fout, "\n\n���ҽ���:\n\n");
				fprintf(fout, "%s\n", buf2);
				getdata(7, 0, ":", buf2, 75, DOECHO, YEA);
				if (buf2[0] != '\0')
				{
					fprintf(fout, "%s\n", buf2);
					getdata(8, 0, ":", buf2, 75, DOECHO, YEA);
					if (buf2[0] != '\0')
					{
						fprintf(fout, "%s\n", buf2);
					}
				}
			}
			fclose(fout);
			sprintf(buf2, "Telnet: %s@%s",currentuser.userid, urec->username);
			Postfile(buf, "newcomers", buf2, 2);
			unlink(buf);
		}
		pressanykey();
	}
#endif
	///
	///���³�����վ���޷�������������������ʺ�ע�ᣡ
	///����ȥ����������ʱȥ������������ʱ�������
	///
	/*
	#ifdef PASSAFTERTHREEDAYS
	   if (urec->lastlogin - urec->firstlogin < 3 * 86400) {
	      if (!HAS_PERM(PERM_SYSOP)) {
	         set_safe_record();
	         urec->userlevel &= ~(PERM_DEFAULT);
	         urec->userlevel |= PERM_BASIC;
	         substitute_record(PASSFILE, urec, sizeof(struct userec), usernum);
	         ansimore("etc/newregister", YEA);
		 return ;
	      }
	   }
	#endif
	*/
	clear();
	if (HAS_PERM(PERM_LOGINOK))
		return;
#ifndef AUTOGETPERM

	if( check_register_ok())
	{
#endif
		set_safe_record();
		urec->lastjustify = time(0);
		urec->userlevel |= PERM_DEFAULT;
		substitute_record(PASSFILE, urec, sizeof(struct userec), usernum);
		return;
#ifndef AUTOGETPERM

	}
#endif

#ifdef MAILCHECK
#ifdef CODE_VALID
	sethomefile(buf, currentuser.userid, ".regpass");
	if (dashf(buf))
	{
		move(13, 0);
		prints("����δͨ�����ȷ��... \n");
		prints("�����ڱ�������ע��ȷ������, \"��֤����\"����Ϊ���ȷ��\n");
		prints("һ���� %d ���ַ�, ��Сд���в���, ��ע��.\n", RNDPASSLEN);
		prints("����ȡ�������������� [Enter] ��.\n");
		prints("[1;33m��ע��, ����������һ����֤�������������������룡[m\n");
		if ((fout = fopen(buf, "r")) != NULL)
		{
			fscanf(fout, "%s", buf2);
			fclose(fout);
			for (i = 0; i < 3; i++)
			{
				move(18, 0);
				prints("�㻹�� %d �λ���\n", 3 - i);
				getdata(19,0,"��������֤����: ",genbuf,(RNDPASSLEN+1),DOECHO,YEA);
				if (strcmp(genbuf, "") != 0)
				{
					if (strcmp(genbuf, buf2) != 0)
						continue;
					else
						break;
				}
			}
		}
		else
			i = 3;
		if (i == 3)
		{
			prints("������֤ʧ��! ����Ҫ��дע�ᵥ�����ȷ������ȷ���������\n");
			getdata(22,0,"��ѡ��1.��ע�ᵥ 2.����ȷ���� [1]:",ans,2,DOECHO,YEA);
			if(ans[0] == '2')
			{
				send_regmail(&currentuser);
				pressanykey();
			}
			else
				x_fillform();
		}
		else
		{
			set_safe_record();
			urec->userlevel |= PERM_DEFAULT;
			urec->lastjustify = time(0);
			strncpy(urec->reginfo, urec->email, 62);
			substitute_record(PASSFILE, urec,sizeof(struct userec), usernum);
			prints("������!! ����˳����ɱ�վ��ʹ����ע������,\n");
			prints("������������ӵ��һ��ʹ���ߵ�Ȩ��������...\n");
			unlink(buf);
			mail_file("etc/smail", "SYSOP", "��ӭ���뱾վ����");
			pressanykey();
		}
		return ;
	}
#endif
	if ( (!strstr(urec->email, BBSHOST)) && (!invalidaddr(urec->email)) &&
	        (!invalid_email(urec->email)))
	{
		move(13, 0);
		prints("���ĵ������� ����ͨ��������֤...  \n");
		prints("    ��վ�����ϼ�һ����֤�Ÿ���,\n");
		prints("    ��ֻҪ�� %s ����, �Ϳ��Գ�Ϊ��վ�ϸ���.\n\n", urec->email);
		prints("    ��Ϊ��վ�ϸ���, �������и����Ȩ���!\n");
		prints("    ��Ҳ����ֱ����дע�ᵥ��Ȼ��ȴ�վ�����ֹ���֤��\n");
		getdata(21,0,"��ѡ��1.��ע�ᵥ 2.��ȷ���� [1]: ",ans,2,DOECHO,YEA);
		if(ans[0] == '2')
		{
			send_regmail(&currentuser);
			getdata(21,0,"ȷ�����Ѽĳ�, ��������Ŷ!! ",ans, 2, DOECHO, YEA);
			return ;
		}
	}
#endif
	x_fillform();
}
/*
int generate_uuid()
{
	char userid[IDLEN + 1], numbuf[2];
	int num;
	modify_user_mode (ADMIN);
	clear();
	move(5,0);
	bzero(userid,IDLEN + 1);
	bzero(numbuf,2);
	usercomplete("�������û�id��", userid);
	if(userid[0] == '\0')
		return 0;
	getdata(6, 0, "���������ɵ�����:", numbuf,2, DOECHO, NA);
	if(atoi(numbuf) >0)
		num = atoi(numbuf);
	else
		return 0;
	if(create_uuid(userid, num, YEA))
	{
		clear();
		move(10,10);
		prints("����������ʧ�ܣ�");
		pressanykey();
		return 1;
	}
	move(10,10);
	prints("����������ɹ�!\n");
	pressanykey();
	return 0;
}


int create_uuid(char* userid, int num, int flag)
{
	struct uuid_st uu;
	uuid_t uu_t;
	char uuidfile[STRLEN],title[STRLEN],msg[STRLEN],
	     fname[STRLEN],uuids[10][33];
	int fd, i;
	FILE* se;
	if(userid[0] == '\0')
		return 0;
	if(num > 9)
		return 1;
	sprintf(uuidfile,"%s/%s",BBSHOME,UUIDS);
	if ((fd = open(uuidfile, O_WRONLY | O_APPEND)) == -1)
		return 1;
	for(i = 0;i < num; i++){
		bzero(&uu, sizeof(struct uuid_st));
		uuid_generate(uu_t);
		uuid_unparse(uu_t,uu.uuid);
		uu.uuid[32] = 0;
		strcpy(uu.owner, userid);
		uu.owner[strlen(userid)] = 0;
		sprintf(uuidfile,"%s/%s" , BBSHOME, UUIDS);
		write(fd, &uu, sizeof(struct uuid_st));
		flock(fd,LOCK_UN);
		strcpy(uuids[i],uu.uuid);
		uuids[i][32] = 0;
	}
	close(fd);
	if(flag)
		sprintf(title,"%s����%s������",currentuser.userid, userid);
	else
		sprintf(title,"%s����%s������",BBSID, userid);
	bzero(msg, STRLEN);
	sprintf(msg,"%s ���ã�����������Ϊ:\n",userid);
	sprintf(fname,"tmp/%s.%s.%05d",BBSID,currentuser.userid,uinfo.pid);
	if((se=fopen(fname,"w"))!=NULL)
	{
		fprintf(se,"������: %s(%s)\n",currentuser.userid,currentuser.username);
		fprintf(se,"��  ��: ������\n");
		getdatestring(time(0),NA);
		fprintf(se,"����վ: �Ͽ��˲���վ (%s)\n",datestring);
		fprintf(se,"��  Դ: %s\n\n",fromhost);
	 	fprintf(se,"%s",msg);
		for(i = 0; i < num; i ++)
			fprintf(se,"%s\n", uuids[i]);
		fprintf(se, "--\n\33[m\33[1;35m�� ��Դ:��%s %s��[FROM: %-.20s]\33[m ",
				                BBSNAME, BBSHOST, fromhost);
		fclose(se);
		mail_file(fname,userid,title);
		Postfile(fname,"BBSAuthInfo",title,1);
		unlink(fname);
	}
	report(title);
	pressanykey();
	return 0;
}

int generate_uuid2(char* uuid){
	struct uuid_st uu;
	uuid_t uu_t;
	if(uuid == NULL)
		return 0;
	bzero(&uu, sizeof(struct uuid_st));
	uuid_generate(uu_t);
	uuid_unparse(uu_t,uu.uuid);
	uu.uuid[32] = 0;
	strncpy(uuid,uu.uuid,32);
	uuid[32] = 0;
	return 1;
}

int valid_uuid(char* uuid){
	char uuidfile[STRLEN];
	int fd;
	struct uuid_st uu;
	int uu_size = sizeof(struct uuid_st);
	if(!uuid)
		return NA;
	sprintf(uuidfile,"%s/%s" , BBSHOME, UUIDS);
	if ((fd = open(uuidfile, O_RDONLY)) == -1)
		return NA;
	while(read(fd, &uu, uu_size) == uu_size)
	{
		if(!strcasecmp(uu.uuid, uuid) && uu.reger[0] == 0){
			close(fd);
			return YEA;
		}
		bzero(&uu, uu_size);
	}
	close(fd);
	return NA;
}
int save_uuid(char* uuid, char* regid){
	char uuidfile[STRLEN];
	int fd;
	int id = 0;
	struct uuid_st uu;
	int uu_size = sizeof(struct uuid_st);
	if(!uuid || !regid)
		return 1;
	sprintf(uuidfile,"%s/%s" , BBSHOME, UUIDS);
	if ((fd = open(uuidfile, O_RDONLY)) == -1)
		return 1;
	while(read(fd, &uu, uu_size) == uu_size)
	{
		id ++;
		if(!strcasecmp(uu.uuid, uuid)){
			strcpy(uu.reger, regid);
			close(fd);
			substitute_record(uuidfile, &uu,sizeof(struct uuid_st), id);
			break;
		}
		bzero(&uu, uu_size);
	}
	return 0;
}*/
//deardrago 2000.09.27  over
