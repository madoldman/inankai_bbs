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
$Id: delete.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif
int seclusionday=0;
FILE *fp;
char tmp[80];
void mail_info(int mode,char *lastword);


//int offline()
void d_offline(int mode)
{
	int i;
	char    buf[STRLEN], lastword[640];

	modify_user_mode(mode?RECLUSION:OFFLINE);
	clear();
	if (count_user( )>1)
	{/*Added by Tim 1999/10/14. avoid suiciding or fading when multi-login
		            this is a serious bug..vectra is a victim due to the bug.
		            wuwuw....now, vectra is a newcomer...:(*/
		move(1,0);
		prints("��Ŀǰ��login��ֹһ��,���˳�����login������һ��\n");
		pressreturn();
		clear();
		return;
	}

	if (HAS_PERM(PERM_BOARDS) || HAS_PERM(PERM_ADMINMENU) || HAS_PERM(PERM_CHATCLOAK))
	{
		move(1, 0);
		prints("\n\n������������, �������%s��!!\n",mode?"����":"��ɱ");
		pressreturn();
		clear();
		return;
	}
	if (!HAS_PERM(PERM_BASIC)||!HAS_PERM(PERM_CHAT)||!HAS_PERM(PERM_PAGE)||!HAS_PERM(PERM_POST)||!HAS_PERM(PERM_LOGINOK)||!HAS_PERM(PERM_MESSAGE))
	{
		move(1, 0);
		prints("\n\n�Բ���, ����δ���ʸ�ִ�д�����!!\n");
		prints("�� mail �� SYSOP ˵��ԭ��, лл��\n");
		pressreturn();
		clear();
		return;
	}  /*loveni:������ɱ��������*/
	getdata(1, 0, "�������������: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
	{
		prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
		pressreturn();
		clear();
		return;
	}
	getdata(3, 0, "�������ʲô����? ", buf, NAMELEN, DOECHO, YEA);
	if (*buf == '\0' || strcmp(buf, currentuser.realname))
	{
		prints("\n\n�ܱ�Ǹ, �Ҳ�����ʶ�㡣\n");
		pressreturn();
		clear();
		return;
	}
	clear();
	move(1, 0);
	if(mode)
	{
		//add by bluetent 2002.12.05
		int usrexp=0;
		prints("Ŀǰ�涨��\n������·��������3-5�죬һ��ս�ѿ�������3-15�죬\n�м�վ�ѿ�������3-30�죬�߼�վ�ѿ�������3-60�죬\n��վ�ѿ�������3-90�죬���ϼ���������3-120�죬\n��վԪ�ϼ������ϵȼ���������3-180�졣\n�йصȼ�������ȥBBSHelp��鿴��������\n���⣬�������ڼ䣬������ʹ��pop3������ȡվ���ż�.\n");
		//	move(8,0);
		sprintf( genbuf, "��ѡ������ʱ��(��) [%d]: ", seclusionday);
		while(1)
		{
			getdata( 8, 0, genbuf, buf, 5, DOECHO,YEA);
			seclusionday = atoi( buf );
			if(seclusionday==0)
				return;
			else if(seclusionday>2&&seclusionday<181)
				break;
		}
		usrexp = currentuser.numposts + currentuser.numlogins / 5 + (time(0) - currentuser.firstlogin) / 86400 + currentuser.stay / 3600;
		if (usrexp<=100)
			seclusionday=(seclusionday>5?5:seclusionday);
		if (usrexp<=450)
			seclusionday=(seclusionday>15?15:seclusionday);
		if (usrexp<=850)
			seclusionday=(seclusionday>30?30:seclusionday);
		if (usrexp<=1500)
			seclusionday=(seclusionday>60?60:seclusionday);
		if (usrexp<=2500)
			seclusionday=(seclusionday>90?90:seclusionday);
		if (usrexp<=3000)
			seclusionday=(seclusionday>120?120:seclusionday);
		if (usrexp>5000)
			seclusionday=(seclusionday>180?180:seclusionday);

		//add end
		move(10,0);
		prints("[1;5;31m����[0;1;31m�� ������, ����%d���޷����ô��ʺŽ��뱾վ����\n", seclusionday);
		prints("[1;32m���ʺ��� %d ��������Խ������״̬ :( .....[m", seclusionday);
		pressanykey();
	}
	else
	{
		prints("���棺��ɱ��, ���������������ٵ�14�죬14��������ʺ��Զ���ʧ");
		move(2,0);
		prints("����14�������ı�����Ļ��������ͨ����¼��վһ�λָ�ԭ������");
		move(3, 0);
		prints("[1;31m��ɱ�û�����ʧ����[33m����Ȩ��[31m������[m");
	}
	clear();
	move(5,0);
	i = 0;
	if(askyn("�����᲻���㣬����֮ǰ��ʲô����˵ô",NA,NA)==YEA)
	{
		strcpy(lastword,">\n>");
		buf[0] = '\0';
		for(i = 0; i< 8; i++)
		{
			getdata(i+6, 0, ": ", buf, 77, DOECHO, YEA);
			if(buf[0] == '\0')
				break;
			strcat(lastword,buf);
			strcat(lastword,"\n>");
		}
		if(i == 0)
			lastword[0] = '\0';
		else
			strcat(lastword,"\n\n");
		move( i + 8, 0);
		if( i == 0 )
			prints("�����㻹��ʲô����Ը��˵���ǲ��ǻ�����˼δ�ˣ�");
		else if( i <= 4 )
			prints("�������㲵��������Ķ����� ... ");
		else
			prints("�һ�ǵ���ģ����ѣ���Ҳ֪������뿪Ҳ��û�а취���£�������");
	}
	else
	{
		lastword[0] = '\0';
	}
	move( i + 10, 0);
	if (askyn("��ȷ��Ҫ�뿪������ͥ", NA, NA) == 1)
	{
		clear();
		mail_info(mode,lastword);
		if(mode)
		{
			char buf[80];
			sprintf(buf, "home/%c/%s/Reclusion", toupper(currentuser.userid[0]), currentuser.userid);
			fp=fopen(buf,"w+");
			if(fp)
			{
				//	      flock(fileno(fp),LOCK_EX);
				fprintf(fp,"%d", seclusionday);
				//	      flock(fileno(fp),LOCK_UN);
				fclose(fp);
			}
			currentuser.userlevel|=PERM_RECLUSION;
		}
		else
		{
			if((currentuser.userlevel & PERM_PERSONAL) == 0)
				currentuser.userlevel = PERM_DEFAULT|PERM_SUICIDE;
			else
				currentuser.userlevel = PERM_DEFAULT|PERM_SUICIDE|PERM_PERSONAL;
		}   //  by livebird:  ��ɱ�й����ļ�Ȩ�ޱ���
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec), usernum);
		//      mail_info(mode,lastword);
		modify_user_mode(OFFLINE);
		kick_user(&uinfo);
		exit(0);
	}
}

getuinfo(FILE* fn)
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n%s�Ĵ���     : %s\n", (currentuser.gender=='F'||currentuser.gender=='f')?"��":"��",currentuser.userid);
	fprintf(fn, "%s���ǳ�     : %s\n", (currentuser.gender=='F'||currentuser.gender=='f')?"��":"��",currentuser.username);
	fprintf(fn, "��ʵ����     : %s\n", currentuser.realname);
	fprintf(fn, "��ססַ     : %s\n", currentuser.address);
	fprintf(fn, "�����ʼ����� : %s\n", currentuser.email);
	fprintf(fn, "��ʵ E-mail  : %s\n", currentuser.reginfo);
	fprintf(fn, "�ʺ�ע���ַ : %s\n", currentuser.ident);
	getdatestring(currentuser.firstlogin,NA);
	fprintf(fn, "�ʺŽ������� : %s\n", datestring);
	getdatestring(currentuser.lastlogin,NA);
	fprintf(fn, "����������� : %s\n", datestring);
	fprintf(fn, "������ٻ��� : %s\n", currentuser.lasthost);
	fprintf(fn, "��վ����     : %d ��\n", currentuser.numlogins);
	fprintf(fn, "������Ŀ     : %d\n", currentuser.numposts);
	fprintf(fn, "��վ��ʱ��   : %d Сʱ %d ����\n",
	        currentuser.stay / 3600, (currentuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(currentuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, "ʹ����Ȩ��   : %s\n\n", buf);
}

/* add by yiyo ��¼����ѯID������Ϣ */
gethisinfo(FILE* fn)
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n%s�Ĵ���     : %s\n", (lookupuser.gender=='F'||lookupuser.gender=='f')?"��":"��",lookupuser.userid);
	fprintf(fn, "%s���ǳ�     : %s\n", (lookupuser.gender=='F'||lookupuser.gender=='f')?"��":"��",lookupuser.username);
	fprintf(fn, "��ʵ����     : %s\n", lookupuser.realname);
	fprintf(fn, "��ססַ     : %s\n", lookupuser.address);
	fprintf(fn, "�����ʼ����� : %s\n", lookupuser.email);
	fprintf(fn, "��ʵ E-mail  : %s\n", lookupuser.reginfo);
	fprintf(fn, "�ʺ�ע���ַ : %s\n", lookupuser.ident);
	getdatestring(lookupuser.firstlogin,NA);
	fprintf(fn, "�ʺŽ������� : %s\n", datestring);
	getdatestring(lookupuser.lastlogin,NA);
	fprintf(fn, "����������� : %s\n", datestring);
	fprintf(fn, "������ٻ��� : %s\n", lookupuser.lasthost);
	fprintf(fn, "��վ����     : %d ��\n", lookupuser.numlogins);
	fprintf(fn, "������Ŀ     : %d\n", lookupuser.numposts);
	fprintf(fn, "��վ��ʱ��   : %d Сʱ %d ����\n",
	        lookupuser.stay / 3600, (lookupuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(lookupuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, "ʹ����Ȩ��   : %s\n\n", buf);
}
/* add end */
void mail_info(int mode,char *lastword)
{
	FILE   *fn;
#ifndef TIMECOUNTER

	time_t  now;
#endif

	char    filename[STRLEN];

#ifndef TIMECOUNTER

	now = time(0);
	getdatestring(now,NA);
#else

	getnowdatestring();
#endif

	sprintf(filename, "tmp/suicide.%s", currentuser.userid);
	if ((fn = fopen(filename, "w")) != NULL)
	{
		if(mode)
		{
			fprintf(fn, "\033[1m%s\033[m �Ѿ��� \033[1m%s\033[m �Ǽǹ����ˣ������ϣ����뱣��...", currentuser.userid ,datestring);
		}
		else
		{
			fprintf(fn, "[1m%s[m �Ѿ��� [1m%s[m �Ǽ���ɱ�ˣ������ϣ����뱣��...", currentuser.userid ,datestring);
		}
		getuinfo(fn);
		fclose(fn);
		if(mode)
			Postfile(filename,"syssecurity3","����֪ͨ(������Ч)...",2);
		else
			Postfile(filename, "syssecurity3", "�Ǽ���ɱ֪ͨ(15������Ч)...", 2);
		unlink(filename);
	}
	if ((fn = fopen(filename, "w")) != NULL)
	{
		fprintf(fn, "��Һ�,\n\n");
		fprintf(fn,"���� %s (%s)���Ҽ�������%s\n\n",
		        currentuser.userid, currentuser.username,mode?"��ʱ�뿪һ��ʱ���ˡ�":"�� 15 ����뿪�����ˡ�");
		//getdatestring(currentuser.firstlogin,NA);
		/*fprintf(fn, "��ע���������Ѿ����� %d ���ˣ������ܼ� %d ���ӵ����������У�\n",
		        currentuser.numlogins,currentuser.stay/60);
		fprintf(fn, "������λ����������أ������ҵ�����...  ���εΣ������������У�\n\n");;*/
		fprintf(fn,"%s",lastword);
		if (!mode)
			fprintf(fn,"�����ǣ���� %s �����ǵĺ����������õ��ɡ���Ϊ�Ҽ��������뿪������!\n\n", currentuser.userid);
		else
			fprintf(fn,"�����ǣ��벻ҪΪ�����ģ�\n\n");
		sprintf(tmp, "%d��֮��", seclusionday);
		fprintf(fn, "%s�һ�����ġ� ����!! �ټ�!!\n\n\n", mode?tmp:"�����г�һ��");
#ifndef TIMECOUNTER

		getdatestring(now,NA);
#else

		getnowdatestring();
#endif

		fprintf(fn, "%s �� %s ��.\n\n", currentuser.userid, datestring);
		fclose(fn);
		Postfile(filename, "notepad", (mode?"��������...":"�Ǽ���ɱ����..."), 2);
		unlink(filename);
	}
}
offline()
{
	d_offline(0);
}

fade_out()
{
	d_offline(1);
}
