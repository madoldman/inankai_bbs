#ifndef WITHOUT_ADMIN_TOOLS
#ifndef DLM
#include <stdio.h>
#include "bbs.h"

extern int cmpbnames();
extern int numboards;
extern int cleanmail();
extern char *chgrp();
extern int dowall();
extern int cmpuids();
extern int t_cmpuids();

extern int noreply;
int  showperminfo(int, int);
#ifndef NEW_CREATE_BRD
char    cexplain[STRLEN];
#endif
char	buf2[STRLEN];
char    lookgrp[30];
char	bnames[3][20];

struct GroupInfo
{
	char   name[10][16];
	char   items[10][32];
	char   chs[10][8];
	time_t update;
	int	  num;
}
GroupsInfo;

FILE   *cleanlog;

int getbnames(char* userid, char* bname,int* find)
{
	int oldbm = 0;
	FILE 	*bmfp;
	char bmfilename[STRLEN],tmp[20];
	*find = 0;
	sethomefile(bmfilename,userid,".bmfile");
	bmfp = fopen (bmfilename,"r");
	if ( !bmfp )
		return 0;
	for( oldbm =0 ; oldbm < 4;)
	{
		fscanf(bmfp,"%s\n",tmp);
		if(!strcmp(bname,tmp))
			*find = oldbm+1;
		strcpy(bnames[oldbm++],tmp);
		if (feof(bmfp))
			break;
	}
	fclose(bmfp);
	return oldbm;
}

int add_medal ()
{
#ifdef ALLOW_MEDALS
	struct userec uinfo;
	int id;
	char buf [10], show [256];
	static char * medaltype [] = {
					"����վ����",
					"ԭ��д�ֽ���",
					"���������",
					"�������ѽ���",
					"ץ�����ֽ���",
					"�����ٲý���",
					"���⹱�׽���",
					};
	unsigned int num, type;
	int i;
	int nummedal [7];
	modify_user_mode (ADMIN);
	if (!check_systempasswd () )
	{
		return -1;
	}

while (1) {
	clear ();
	stand_title ("��������!");
	if (!gettheuserid (1, "������ʹ���ߴ���(ֱ�ӻس�ȡ������)��", &id) )
		return -1;
	clrtobot ();
	memcpy (& uinfo, & lookupuser, sizeof (uinfo) );
	move (4, 0);
	prints ("�û� %s ӵ�н��������\n", uinfo.userid);
	num = uinfo.nummedals;
	for (i = 7; i > 0; --i) {
		nummedal [7 - i] = (num >> (i << 2)) & 0x0f;
		prints ("%30s %d ö\n", medaltype [7 - i], nummedal [7 - i]);
	}
	move (13, 0);
	prints ("     A.����վ��  B.ԭ��д��  C.�������  D.��������\n");
	prints ("     E.ץ������  F.�����ٲ�  G.���⹱��  H.��������\n");
	getdata (16, 0, "�����뽱������: ", buf, 2, DOECHO, YEA);
	if (buf [0] == 0) {
		move (17, 0);
		prints ("����ȡ��");
		pressreturn ();
		continue;
	}
	type = toupper (buf [0]) - 'A';
	if (type < 0 || type > 7) {
		move (17, 0);
		prints ("�Ƿ�����");
		pressreturn ();
		continue;
	}
	if (type == 7) {
		sprintf (show, "���� \033[1;32m%s\033[m ���ѵ����н��£�ȷ����[y/N]", uinfo.userid);
		getdata (17, 0, show, buf, 10, DOECHO, YEA);
		if (buf [0] == 'y' || buf [0] == 'Y')
		{
			char bufsys [100];
			uinfo.nummedals = 0;
			if (uinfo.nummedals > 0x8000)
				uinfo.nummedals =0;
			substitute_record (PASSFILE, & uinfo, sizeof (uinfo), id);
			sprintf (bufsys, "���� %s ���н���", uinfo.userid);
			securityreport (bufsys);
			prints ("�����ɹ���");
			pressreturn ();
			continue;
		}
		else {
			prints ("����ȡ��");
			pressreturn ();
			continue;
		}
	}
	move (17, 0);
	clrtobot ();
	move (17, 0);
	sprintf (genbuf, "���轱������: \033[1;32m%s\033[m", medaltype [type]);
	prints (genbuf);
	getdata (18, 0, "�������öѫ�£�", buf, 10, DOECHO, YEA);
	if (buf [0] != 0) {
		num = atoi (buf);
		if (num > 15) {
			prints ("����!!������ 0--15 ����!!");
			pressreturn ();
			continue;
		}
	}
	sprintf (show, "���� \033[1;32m%s\033[m ���� \033[1;32m%d\033[m ö \033[1;32m%s\033[m��ȷ����[y/N]", uinfo.userid, num, medaltype [type]);
	getdata (19, 0, show, buf, 10, DOECHO, YEA);
	if (buf [0] == 'y' || buf [0] == 'Y')
	{
		char bufsys [100];

		sprintf (bufsys, "���� %s %s %d ö", uinfo.userid, medaltype [type], num);
		securityreport (bufsys);

		nummedal [type] = num;
		num = 0;
		for (i = 7; i > 0; --i) {
			unsigned int tmp = (nummedal [7 - i] << (i << 2));
			if (tmp > 0) {
				++num;
				num |= tmp;
			}
		}
		uinfo.nummedals = num;
		substitute_record (PASSFILE, & uinfo, sizeof (uinfo), id);
		prints ("�����ɹ���");
		pressreturn ();
		continue;
	}
	prints ("����ȡ��");
	pressreturn ();

}  // Efan: while loop
#endif

	return 0;
}
/* add by livebird ������� */
int un_Reclusion()
{
        int id;
        modify_user_mode(ADMIN);
        if (!check_systempasswd())
        {
                return;
        }
        clear();
        prints("���ʹ���߹���״̬\n");
        clrtoeol();
        move(1, 0);
        usercomplete("���������ĵ�ʹ�����ʺ�: ", genbuf);
        if (genbuf[0] == '\0')
        {
                clear();
                return 0;
        }
        if (!(id = getuser(genbuf)))
        {
                move(3, 0);
                prints("Invalid User Id");
                clrtoeol();
                pressreturn();
                clear();
                return 0;
        }
       // move(1, 0);
        clrtobot();
        move(2, 0);
        if( askyn("��ȷ��Ҫ��������״̬��?",NA,NA)== NA )
        {
                 prints("\nʹ���� '%s' û�б��������״̬��\n",lookupuser.userid);
                 pressreturn();
                 clear();
        }
        else
        {
                if((lookupuser.userlevel & PERM_RECLUSION ) == 0)
                {
                        prints("\nʹ���� '%s' û�й�����\n", lookupuser.userid);
                        pressreturn();
                        clear();
                }
                else
                {       char    secu[STRLEN];
                        sprintf(secu, "��� %s �Ĺ���״̬", lookupuser.userid);
                        //securityreport1(secu);

                        lookupuser.userlevel &= ~PERM_RECLUSION;

                        security_report(secu,1);
                        substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
                        prints("\nʹ���� '%s' �Ѿ����������״̬��\n", lookupuser.userid);
                        pressreturn();
                        clear();
                }
        }
        return 0;
}

int m_info()
{
	struct userec uinfo;
	int     id;
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return -1;
	}
	clear();
	stand_title("�޸�ʹ��������");
	if(!gettheuserid(1,"������ʹ���ߴ���: ",&id))
		return -1;
	memcpy(&uinfo, &lookupuser, sizeof(uinfo));

	move(1, 0);
	clrtobot();
	disply_userinfo(&uinfo);
	uinfo_query(&uinfo, 1, id);
	return 0;
}

/* add by yiyo ����У�������ʺŲ鿴�û����ϣ��������޸� */
int look_m_info()
{
	struct userec uinfo;
	int     id;
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
        if(redhackercheck("��ѯ��������")) return;
	stand_title("��ѯʹ��������");
	if(!gettheuserid(1,"������ʹ���ߴ���: ",&id))
		return -1;
	memcpy(&uinfo, &lookupuser, sizeof(uinfo));

	move(1, 0);
	clrtobot();
	disply_userinfo(&uinfo);
	pressreturn();
	return 0;
}
/* add end by yiyo */

int m_ordainBM()
{
	int     id, pos, oldbm = 0, i,find,bm = 1;
	struct boardheader fh;
	FILE	*bmfp;
	char	bmfilename[STRLEN], bname[256];
	char buf[5][STRLEN];
	char	genbuftemp[STRLEN];

	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;

	clear();
	stand_title("��������\n");
	clrtoeol();
	if (!gettheuserid(2,"������������ʹ�����ʺ�: ",&id))
		return 0;
	if(!gettheboardname(3,"�����ʹ���߽����������������: ",&pos,&fh,bname))
		return -1;
	if (is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("�㲻�������ӷ�������");
                pressanykey();                 
		clear();
                return -1;
        }
	if( fh.BM[0] != '\0' )
	{
		if ( !strncmp(fh.BM,"SYSOPs",6) )
		{
			move(4, 0);
			if(askyn("�������������� SYSOPs, ��ȷ���ð���Ҫ����",NA,NA)==NA)
			{
				clear();
				return -1;
			}
			fh.BM[0] = '\0';
		}
		else
		{
			for (i =0 , oldbm =1 ;fh.BM[i] != '\0';i++)
				if( fh.BM[i] == ' ' )
					oldbm ++;
			//         if ( oldbm == 3 ) {
			if(oldbm==4)
			{
				move(5, 0);
				//	    prints("%s ������������������",bname);
				prints("%s ������������������",bname);
				pressreturn();
				clear();
				return -1;
			}
			bm = 0;
		}
	}
	if (!strcmp(lookupuser.userid,"guest"))
	{
		move ( 5,0);
		prints("�㲻������ guest ������");
		pressanykey();
		clear();
		return -1;
	}
        if(strlen(fh.BM)+strlen(lookupuser.userid)>BM_LEN){
                move(5,0);
		prints("����,����һ��id̫����ϵͳҪ����ˡ�����");
                pressanykey();
                return -1;
	}

	oldbm = getbnames(lookupuser.userid,bname,&find);
	if( find || (oldbm  == 4 && strcmp (lookupuser.userid, "SYSOP")))
	{
		move ( 5,0);
		prints(" %s �Ѿ���%s��İ�����",lookupuser.userid,find?"��":"�ĸ�");
		pressanykey();
		clear();
		return -1;
	}
	prints("\n�㽫���� %s Ϊ %s ���%s.\n",lookupuser.userid,bname,bm?"��":"��");
	if( askyn("��ȷ��Ҫ������?",NA,NA)== NA )
	{
		prints("ȡ����������");
		pressanykey();
		clear();
		return -1;
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(8,0);
	prints("��������������(������У��� Enter ����)");
	for (i=0;i<5;i++)
	{
		getdata(i+9, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
	}
	strcpy(bnames[oldbm],bname);
	if (!oldbm)
	{
		char    secu[STRLEN];

		lookupuser.userlevel |= PERM_BOARDS ;
		substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
		sprintf(secu, "��������, ���� %s �İ���Ȩ��", lookupuser.userid);
		securityreport(secu);
		move(15,0);
		prints(secu);
	}
	if ( fh.BM[0] == '\0' )
		strcpy(genbuf,lookupuser.userid);
	else
		sprintf(genbuf,"%s %s",fh.BM,lookupuser.userid);
	strncpy(fh.BM, genbuf, sizeof(fh.BM));
	sprintf(genbuf, "%-38.38s(BM: %s)", fh.title +8, fh.BM);

	get_grp(fh.filename);
	edit_grp(fh.filename, lookgrp, fh.title + 8, genbuf);
	substitute_record(BOARDS, &fh, sizeof(fh), pos);

	sethomefile(bmfilename,lookupuser.userid,".bmfile");

	bmfp = fopen(bmfilename,"w+");
	for (i = 0 ; i < oldbm+1; i++)
		fprintf(bmfp,"%s\n",bnames[i]);
	fclose(bmfp);
	//   sprintf(bmfilename, "[����]���� %s Ϊ %s ������%s",
	sprintf(bmfilename, "[����]���� %s Ϊ %s ��%s",
	        lookupuser.userid,fh.filename,bm?"����":"�帱");
	securityreport(bmfilename);
	//move(16,0);
	//prints(bmfilename);

	/*  livebird 11.22.2004 */

	move(16,0);
	if( askyn("��Ҫʹ�ð�У��������������?",NA,NA)== NA )
		sprintf(genbuf,"\n\t\t\t�� ���� ��\n\n"
	        	"\t���� %s Ϊ %s ��%s��\n"
	        	"\t�� %s �������ڵ���Ӧ�������屨����\n"
	        	"\t����δ�ܱ����ߣ���ȡ������Ȩ�ޡ�\n"
			"\tʵϰ��һ���£��������Ѽල��\n",
	        	lookupuser.userid,bname,bm?"����":"�帱",lookupuser.userid);
	else
		sprintf(genbuf,"\n\t\t\t�� ���� ��\n\n"
			"\t���� %s Ϊ %s ��%s��\n"
			"\t�� %s ����У��ѵ����ѵ�����ڸð��õס�\n"
			"\t��ѵ��Ϊ���죬����ȡ����ѵ�ʸ�\n",
			lookupuser.userid,bname,bm?"����":"�帱",lookupuser.userid);
	/* end */
	move(18,0);
	prints(bmfilename);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\n\n�������ԣ�\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(currboard,bname);
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	//tdhlshx
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"��ϲ��������Ϊ%s��İ���!",bname);
	sprintf(tmpfile,"tmp/committbm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,������ʱ�ļ�����.����ϵͳά����ϵ���ٴγ���!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);

	mail_file(tmpfile,lookupuser.userid,buftitle);
	prints("\n\n�����鷢��!");
	pressanykey();
	char bmfaqtitle[STRLEN];
	sprintf(bmfaqtitle,"���������Լ���������ֲ�");
	mail_file("/home/bbs/etc/forbm",lookupuser.userid,bmfaqtitle);
#ifdef ORDAINBM_POST_BOARDNAME

	strcpy(currboard,ORDAINBM_POST_BOARDNAME);
	autoreport(bmfilename,genbuf,YEA,NULL);
#endif

	pressanykey();
	unlink(tmpfile);
	return 0;
}
/*smalldog�����ӷ�������*/
int m_ordainZM()
{
	int     id, pos, oldbm = 0, i,find,bm = 1;
	struct boardheader fh;
	FILE	*bmfp;
	char	bmfilename[STRLEN], bname[256];
	char buf[5][STRLEN];
	char	genbuftemp[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;
	clear();
	stand_title("�����ӷ�������\n");
	clrtoeol();
	if (!gettheuserid(2,"������������ʹ�����ʺ�: ",&id))
		return 0;
	if(!gettheboardname(3,"�����ʹ���߽�������ӷ�������: ",&pos,&fh,bname))
		return -1;
	if (!is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("�㲻��������������ʱ��ȴ�����������");
                pressanykey();                 
		clear();
                return -1;
        }
	if( fh.BM[0] != '\0' )
	{
		for (i =0 , oldbm =1 ;fh.BM[i] != '\0';i++)
			if( fh.BM[i] == ' ' )
				oldbm ++;
		if(oldbm==4)
		{
			move(5, 0);
			prints("%s �ӷ���������������",bname);
			pressreturn();
			clear();
			return -1;
		}
		bm = 0;
	}
	if (!strcmp(lookupuser.userid,"guest"))
	{
		move ( 5,0);
		prints("�㲻������ guest ���ӷ�������");
		pressanykey();
		clear();
		return -1;
	}
        if(strlen(fh.BM)+strlen(lookupuser.userid)>BM_LEN)
	{
                move(5,0);
		prints("����,����һ��id̫����ϵͳҪ����ˡ�����");
                pressanykey();
                return -1;
	}
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if( find || (oldbm  == 4 && strcmp (lookupuser.userid, "SYSOP")))
	{
		move ( 5,0);
		prints(" %s �Ѿ���%s��İ�����",lookupuser.userid,find?"��":"�ĸ�");
		pressanykey();
		clear();
		return -1;
	}
	prints("\n�㽫���� %s Ϊ %s �ӷ�������.\n",lookupuser.userid,bname);
	if( askyn("��ȷ��Ҫ������?",NA,NA)== NA )
	{
		prints("ȡ�������ӷ�������");
		pressanykey();
		clear();
		return -1;
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(8,0);
	prints("��������������(������У��� Enter ����)");
	for (i=0;i<5;i++)
	{
		getdata(i+9, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
	}
	strcpy(bnames[oldbm],bname);
	if (!oldbm)
	{
		char    secu[STRLEN];
		lookupuser.userlevel |= PERM_BOARDS ;
		substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
		sprintf(secu, "�ӷ�����������, ���� %s ����ӦȨ��", lookupuser.userid);
		securityreport(secu);
		move(15,0);
		prints(secu);
	}
	if ( fh.BM[0] == '\0' )
		strcpy(genbuf,lookupuser.userid);
	else
		sprintf(genbuf,"%s %s",fh.BM,lookupuser.userid);
	strncpy(fh.BM, genbuf, sizeof(fh.BM));
	sprintf(genbuf, "%-38.38s(BM: %s)", fh.title +8, fh.BM);

	get_grp(fh.filename);
	edit_grp(fh.filename, lookgrp, fh.title + 8, genbuf);
	substitute_record(BOARDS, &fh, sizeof(fh), pos);

	sethomefile(bmfilename,lookupuser.userid,".bmfile");

	bmfp = fopen(bmfilename,"w+");
	for (i = 0 ; i < oldbm+1; i++)
		fprintf(bmfp,"%s\n",bnames[i]);
	fclose(bmfp);
	sprintf(bmfilename, "[����]���� %s Ϊ %s �ӷ�������",lookupuser.userid,fh.filename);
	securityreport(bmfilename);
	sprintf(genbuf,"\n\t\t\t�� ���� ��\n\n"
        	       "\t���� %s Ϊ %s �ӷ���������\n"
        	       "\t�� %s �������ڵ���Ӧ�������屨����\n"
                       "\t����δ�ܱ����ߣ���ȡ������Ȩ��\n"
		       "\tʵϰ��һ���£��������Ѽල��\n",
                       lookupuser.userid,bname,lookupuser.userid);

	move(18,0);
	prints(bmfilename);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\n\n�������ԣ�\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(currboard,bname);
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	//tdhlshx
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"��ϲ��������Ϊ%s�ӷ���������!",bname);
	sprintf(tmpfile,"tmp/committbm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,������ʱ�ļ�����.����ϵͳά����ϵ���ٴγ���!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);

	mail_file(tmpfile,lookupuser.userid,buftitle);
	prints("\n\n�����鷢��!");
	pressanykey();
	char bmfaqtitle[STRLEN];
	sprintf(bmfaqtitle,"���������Լ���������ֲ�");
	mail_file("/home/bbs/etc/forbm",lookupuser.userid,bmfaqtitle);
#ifdef ORDAINBM_POST_BOARDNAME

	strcpy(currboard,ORDAINBM_POST_BOARDNAME);
	autoreport(bmfilename,genbuf,YEA,NULL);
#endif

	pressanykey();
	unlink(tmpfile);
	return 0;
}

/* thunder 2003.5.21 ���;���
   ��ȷ����Ҫ���͵�idд������*/

int refs_mail()
{
	int     id, pos,  i;
	struct boardheader fh;

	char nbuf[8], bname[STRLEN];
	strcpy(bname,"");
	char buf[5][STRLEN];
	char note[5][STRLEN]={"","����","����","�����ļ�","�����ʺ�"};
	modify_user_mode(ADMIN);


	clear();
	stand_title("���;���\n");
	clrtoeol();
	prints("��ȷ����Ҫ���;��ŵ�idд������");
	if (!gettheuserid(2,"�����뱻����id: ",&id))
		return 0;
	prints("��ѡ��������:\n1.ʧ�ܵİ�������\n2.ʧ�ܵİ�������\n3.ʧ�ܵĸ����ļ�����\n4.ʧ�ܵ������ʺ�����\n5.exit");


	getdata(9, 0, "# [5] ", nbuf, 2, DOECHO, YEA);
	int sel=nbuf[0]-'0';
	if(sel<1 || sel>=5 )
		return 0;
	if(sel==1)
	{
		if(!gettheboardname(10,"�ĸ�����İ���? ",&pos,&fh,bname))
			return -1;
	}
	if(sel==2)
	{

		getdata(10, 0, "������İ�����:", bname, STRLEN-5, DOECHO, YEA);
	}


	/*  prints("�ܾ�%s��%s����",lookupuser.userid,note[sel]);
	  if( askyn("  ��ȷ����?",NA,NA)== NA ) {
	      prints("�ܾ�����ȡ��");
	      pressanykey();
	      clear();
	      return -1;
	  }
	  */
	prints("��Ҫ�ܾ�%s��,��ʲô��Ҫ˵��?����������(������У���Enter����)",lookupuser.userid);
	for (i=0;i<5;i++)
	{
		getdata(i+12, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
	}



	prints("�ܾ�%s��%s%s����",lookupuser.userid,bname,note[sel]);
	if( askyn("  ��ȷ����?",NA,NA)== NA )
	{
		prints("\n\n�ܾ�����ȡ��");
		pressanykey();
		clear();
		return -1;
	}

	char    secu[STRLEN];
	sprintf(secu, "�ܾ�����, �ܾ� %s ����%s%s", lookupuser.userid,bname,note[sel]);
	securityreport(secu);

	char tmpfile[STRLEN],title[STRLEN];
	sprintf(tmpfile,"tmp/refsmail.%s", currentuser.userid);
	sprintf(title,"[ͨ��] ��������%s%s����",bname,note[sel]);
	FILE *fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Erroe,���ܴ�����ʱ�ļ�");
		pressanykey();
		clear();
		return -1;
	}

	char msg[STRLEN];
	if(sel==1)
		sprintf(msg,"���ȸ�л������%s�����.",bname);
	else
		sprintf(msg,"����%s%s���������Ѿ�������.",bname,note[sel]);


	struct tm *tm;
	time_t now;
	time(&now);
	char datestring[STRLEN];
	tm = localtime(&now);
	sprintf(datestring,"%4d��%02d��%02d��",
	        tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday
	       );
	sprintf(genbuf,"\n\t[ͨ��]\n\t�װ���%s:\n\t\t����!%s\n\t\t�ź�����,����վ���������,���ǲ�����׼��������.\n\t\t�����������,�뵽sysop�巢��ѯ��.\n\t\t���,ϣ�����ܼ�����ע��֧�ֱ�BBS.\n\t\t\t\t\t\t\t\t\t\t  %s վ����\n\t\t\t\t\t\t\t\t\t\t   %s",lookupuser.userid,msg,BBSNAME,datestring);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\nվ�����ԣ�\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}

	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,title);

	unlink(tmpfile);
	prints("\n\n���ŷ��ͳɹ�!");
	pressanykey();
	return 0;
}

int m_retireBM()
{
	int     id, pos, right = 0 , oldbm = 0, i,j, bmnum;
	int 	find,bm = 1;
	struct boardheader fh;
	FILE    *bmfp;
	char    bmfilename[STRLEN], buf[5][STRLEN];
	char    bname[STRLEN], usernames[4][STRLEN];
	char	genbuftemp[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;

	clear();
	stand_title("������ְ\n");
	clrtoeol();
	if (!gettheuserid(2,"��������ְ�İ����ʺ�: ",&id))
		return -1;
	if (!gettheboardname(3,"������ð���Ҫ��ȥ�İ���: ",&pos,&fh,bname))
		return -1;
	if (is_zone(BOARDS,bname))
	{
        	move ( 5,0);
        	prints("�㲻�������ӷ�������");
        	pressanykey(); 
                clear();
	        return -1;
	}
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if ( !oldbm || !find)
	{
		move ( 5,0);
		prints(" %s %s����������д�����֪ͨ����վ����",lookupuser.userid,(oldbm)?"���Ǹ�":"û�е����κ�");
		pressanykey();
		clear();
		return -1;
	}
	for ( i = find-1; i< oldbm ; i ++ )
	{
		if (i!=oldbm-1)
			strcpy( bnames[i], bnames[i+1]);
	}
	bmnum = 0;
	for (i =0,j =0 ;fh.BM[i] != '\0';i++)
	{
		if( fh.BM[i] == ' ' )
		{
			usernames[bmnum][j] = '\0';
			bmnum ++;
			j = 0;
		}
		else
		{
			usernames[bmnum][j++] = fh.BM[i];
		}
	}
	usernames[bmnum++][j] = '\0';
	for ( i = 0, right = 0; i< bmnum ; i ++ )
	{
		if ( !strcasecmp (usernames[i],lookupuser.userid))
		{
			right = 1;
			if(i)
				bm = 0;
		}
		if (right&&i!=bmnum-1)
			strcpy( usernames[i],usernames[i+1]);
	}
	if ( !right )
	{
		move ( 5,0);
		prints("�Բ��� %s �����������û�� %s �����д�����֪ͨ����վ����",
		       bname, lookupuser.userid);
		pressanykey();
		clear();
		return -1;
	}
	prints("\n�㽫ȡ�� %s �� %s ���%sְ��.\n",
	       lookupuser.userid,bname,bm?"��":"��");
	if( askyn("��ȷ��Ҫȡ�����ĸð����ְ����?",NA,NA)== NA )
	{
		prints("\n�Ǻǣ���ı������ˣ� %s �������� %s �����ְ��",
		       lookupuser.userid, bname);
		pressanykey();
		clear();
		return -1;
	}
	if( bmnum - 1)
	{
		sprintf(genbuf,"%s",usernames[0]);
		for(i=1;i<bmnum-1;i++)
			sprintf(genbuf,"%s %s",genbuf,usernames[i]);
	}
	else
		genbuf[0] = '\0';
	strncpy(fh.BM, genbuf, sizeof(fh.BM));
	if( fh.BM[0] != '\0')
		sprintf(genbuf, "%-38.38s(BM: %s)", fh.title +8, fh.BM);
	else
		sprintf(genbuf, "%-38.38s", fh.title + 8);
	get_grp(fh.filename);
	edit_grp(fh.filename, lookgrp, fh.title + 8, genbuf);
	substitute_record(BOARDS, &fh, sizeof(fh), pos);
	sprintf(genbuf,"ȡ�� %s �� %s ����������ְ��",lookupuser.userid,fh.filename);
	strcpy(genbuftemp,genbuf);
	securityreport(genbuf);
	strcpy(genbuf,genbuftemp);
	move(8,0);
	prints(genbuf);
	sethomefile(bmfilename,lookupuser.userid,".bmfile");
	if ( oldbm -1 )
	{
		bmfp = fopen(bmfilename,"w+");
		for (i = 0 ; i < oldbm-1; i++)
			fprintf(bmfp,"%s\n",bnames[i]);
		fclose(bmfp);
	}
	else
	{
		char    secu[STRLEN];

		unlink(bmfilename);
		if(   !(lookupuser.userlevel&PERM_OBOARDS )
		        && !(lookupuser.userlevel&PERM_SYSOP) )
		{
			lookupuser.userlevel &= ~PERM_BOARDS ;
			substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
			sprintf(secu,"����жְ, ȡ�� %s �İ���Ȩ��", lookupuser.userid);
			securityreport(secu);
			move( 9,0);
			prints(secu);
		}
	}
	prints("\n\n");
	if( askyn("��Ҫ����ذ��淢�͹�����?",YEA,NA)== NA )
	{
		pressanykey();
		return 0;
	}
	prints("\n");
	if(askyn("���������밴 Enter ��ȷ�ϣ���ְ�ͷ��� N ��",YEA,NA)==YEA)
		right=1;
	else
		right = 0;
	if(right)
		sprintf(bmfilename,"%s ��%s %s ���ι���",bname,bm?"����":"�帱",lookupuser.userid);
	else
		sprintf(bmfilename,"[����]���� %s ����� %s ",bname,lookupuser.userid);
	strcpy(currboard,bname);
	if(right)
	{
		sprintf(genbuf,"\n\t\t\t�� ���� ��\n\n"
		        "\t��վ�������ۣ�\n"
		        "\tͬ�� %s ��ȥ %s ���%sְ��\n"
		        "\t�ڴˣ���%s������ %s �������������ʾ��л��\n\n"
		        "\tϣ�����Ҳ��֧�ֱ���Ĺ���.",
		        lookupuser.userid,bname,bm?"����":"�帱",(lookupuser.gender=='F'||lookupuser.gender=='f')?"��":"��",bname);
	}
	else
	{
		sprintf(genbuf,"\n\t\t\t����ְ���桿\n\n"
		        "\t��վ�������۾�����\n"
		        "\t���� %s ����� %s ��\n",
		        bname,lookupuser.userid);
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(14,0);
	prints("������%s����(������У��� Enter ����)",right?"��������":"������ְ");
	for (i=0;i<5;i++)
	{
		getdata(i+15, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,right?"\n\n���θ��ԣ�\n":"\n\n��ְ˵����\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	//   autoreport(bmfilename,genbuf,0,lookupuser.userid);
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"%s",bmfilename);
	sprintf(tmpfile,"tmp/goodbyebm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,������ʱ�ļ�����.����ϵͳά����ϵ���ٴγ���!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,buftitle);

	//�������κ����˿·
	sethomefile(genbuf, lookupuser.userid, ".announcepath");
	if(dashf(genbuf))
		unlink(genbuf);

	prints("\nִ����ϣ�");
	pressanykey();
	return 0;
}
int m_retireZM()
{
	int     id, pos, right = 0 , oldbm = 0, i,j, bmnum;
	int 	find,bm = 1;
	struct boardheader fh;
	FILE    *bmfp;
	char    bmfilename[STRLEN], buf[5][STRLEN];
	char    bname[STRLEN], usernames[4][STRLEN];
	char	genbuftemp[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;
	clear();
	stand_title("�ӷ���������ְ\n");
	clrtoeol();
	if (!gettheuserid(2,"��������ְ���ӷ��������ʺ�: ",&id))
		return -1;
	if (!gettheboardname(3,"������ð���Ҫ��ȥ���ӷ�����: ",&pos,&fh,bname))
		return -1;
        if (!is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("�㲻�����ΰ���");
                pressanykey();
                clear();
                return -1;
        }
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if ( !oldbm || !find)
	{
		move ( 5,0);
		prints(" %s %s�ӷ������������д�����֪ͨϵͳά����",lookupuser.userid,(oldbm)?"���Ǹ�":"û�е����κ�");
		pressanykey();
		clear();
		return -1;
	}
	for ( i = find-1; i< oldbm ; i ++ )
	{
		if (i!=oldbm-1)
			strcpy( bnames[i], bnames[i+1]);
	}
	bmnum = 0;
	for (i =0,j =0 ;fh.BM[i] != '\0';i++)
	{
		if( fh.BM[i] == ' ' )
		{
			usernames[bmnum][j] = '\0';
			bmnum ++;
			j = 0;
		}
		else
		{
			usernames[bmnum][j++] = fh.BM[i];
		}
	}
	usernames[bmnum++][j] = '\0';
	for ( i = 0, right = 0; i< bmnum ; i ++ )
	{
		if ( !strcasecmp (usernames[i],lookupuser.userid))
		{
			right = 1;
			if(i)
				bm = 0;
		}
		if (right&&i!=bmnum-1)
			strcpy( usernames[i],usernames[i+1]);
	}
	if ( !right )
	{
		move ( 5,0);
		prints("�Բ��� %s �ӷ�������������û�� %s �����д�����֪ͨ����վ����",bname, lookupuser.userid);
		pressanykey();
		clear();
		return -1;
	}
	prints("\n�㽫ȡ�� %s �� %s �ӷ�������ְ��.\n",lookupuser.userid,bname);
	if( askyn("��ȷ��Ҫȡ�����ĸ��ӷ�������ְ����?",NA,NA)== NA )
	{
		prints("\n�Ǻǣ���ı������ˣ� %s �������� %s �ӷ�������ְ��",
		       lookupuser.userid, bname);
		pressanykey();
		clear();
		return -1;
	}
	if( bmnum - 1)
	{
		sprintf(genbuf,"%s",usernames[0]);
		for(i=1;i<bmnum-1;i++)
			sprintf(genbuf,"%s %s",genbuf,usernames[i]);
	}
	else
		genbuf[0] = '\0';
	strncpy(fh.BM, genbuf, sizeof(fh.BM));
	if( fh.BM[0] != '\0')
		sprintf(genbuf, "%-38.38s(BM: %s)", fh.title +8, fh.BM);
	else
		sprintf(genbuf, "%-38.38s", fh.title + 8);
	get_grp(fh.filename);
	edit_grp(fh.filename, lookgrp, fh.title + 8, genbuf);
	substitute_record(BOARDS, &fh, sizeof(fh), pos);
	sprintf(genbuf,"ȡ�� %s �� %s �ӷ�������ְ��",lookupuser.userid,fh.filename);
	strcpy(genbuftemp,genbuf);
	securityreport(genbuf);
	strcpy(genbuf,genbuftemp);
	move(8,0);
	prints(genbuf);
	sethomefile(bmfilename,lookupuser.userid,".bmfile");
	if ( oldbm -1 )
	{
		bmfp = fopen(bmfilename,"w+");
		for (i = 0 ; i < oldbm-1; i++)
			fprintf(bmfp,"%s\n",bnames[i]);
		fclose(bmfp);
	}
	else
	{
		char    secu[STRLEN];

		unlink(bmfilename);
		if(   !(lookupuser.userlevel&PERM_OBOARDS )
		        && !(lookupuser.userlevel&PERM_SYSOP) )
		{
			lookupuser.userlevel &= ~PERM_BOARDS ;
			substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
			sprintf(secu,"����жְ, ȡ�� %s ���ӷ�������Ȩ��", lookupuser.userid);
			securityreport(secu);
			move( 9,0);
			prints(secu);
		}
	}
	prints("\n\n");
	if(askyn("���������밴 Enter ��ȷ�ϣ���ְ�ͷ��� N ��",YEA,NA)==YEA)
		right=1;
	else
		right = 0;
	if(right)
		sprintf(bmfilename,"%s �ӷ������� %s ���ι���",bname,lookupuser.userid);
	else
		sprintf(bmfilename,"[����]���� %s �ӷ������� %s ",bname,lookupuser.userid);
	strcpy(currboard,bname);
	if(right)
	{
		sprintf(genbuf,"\n\t\t\t�� ���� ��\n\n"
		        "\t��վ�������ۣ�\n"
		        "\tͬ�� %s ��ȥ %s �ӷ���������ְ��\n"
		        "\t�ڴˣ����������� %s �ӷ���������������ʾ��л��\n\n"
		        "\tϣ�����Ҳ��֧�ֱ��ӷ����Ĺ���.",
		        lookupuser.userid,bname,bname);
	}
	else
	{
		sprintf(genbuf,"\n\t\t\t����ְ���桿\n\n"
		        "\t��վ�������۾�����\n"
		        "\t���� %s �ӷ������� %s ������ְ��\n",
		        bname,lookupuser.userid);
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(14,0);
	prints("������%s����(������У��� Enter ����)",right?"�ӷ�����������":"�ӷ���������ְ");
	for (i=0;i<5;i++)
	{
		getdata(i+15, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,right?"\n\n���θ��ԣ�\n":"\n\n��ְ˵����\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"%s",bmfilename);
	sprintf(tmpfile,"tmp/goodbyebm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,������ʱ�ļ�����.����ϵͳά����ϵ���ٴγ���!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,buftitle);
	
	//�ӷ��������κ����˿·
	sethomefile(genbuf, lookupuser.userid, ".announcepath");
	if(dashf(genbuf))
		unlink(genbuf);
	
	prints("\nִ����ϣ�");
	pressanykey();
	return 0;
}
#ifdef NEW_CREATE_BRD
int getGroupSet(void)
{
	FILE *mfp;
	char  buf[256];
	char *ptr;
	int   i=0, groupNum, j=0,k=0;
	struct stat st;

	if(stat("etc/menu.ini",&st)==-1||st.st_size==0)
	{
		GroupsInfo.update = 0;
		return 0;
	}
	if(GroupsInfo.update >= st.st_mtime)
	{
		return 1;
	}
	GroupsInfo.update = st.st_mtime;
	mfp = fopen("etc/menu.ini","r");
	if(mfp==NULL)
	{
		GroupsInfo.update = 0;
		return 0;
	}
	for(i = 0; i < 10; i++)
	{
		for( j = 0; j < 16; j ++)
			GroupsInfo.name[i][j] = '\0';
		for( j = 0; j < 32; j ++)
			GroupsInfo.items[i][j] = '\0';
		for( j = 0; j < 8; j ++)
			GroupsInfo.chs[i][j] = '\0';
	}
	j = 0;
	while(fgets(buf,256,mfp)!=NULL)
	{
		//      if(j >= 9 && k >= 9 ) break;
		if(j > 9 && k >9 )
			break;        /* nicky changed for add new group */
		my_ansi_filter(buf);
		if(buf[0] == '#')
			continue;
		if(strstr(buf,"EGroups"))
		{
			if(buf[0]!='@')
				continue;
			ptr = strchr(buf,'\"');
			if(ptr==NULL)
				continue;
			groupNum = *(ptr+1)-'0';
			if(groupNum > 9 || groupNum < 0)
				continue;
			ptr = strchr(buf,')');
			if(ptr==NULL)
				continue;
			for(i=1;*(ptr+i)==' '&&*(ptr+i)!='\0';i++)
				;
			ptr += i;
			for(i=0;*(ptr+i)!=' '&&*(ptr+i)!='\0'&&*(ptr+i)!='\"'&&i<16;i++)
			{
				GroupsInfo.name[groupNum][i] = *(ptr+i);
			}
			GroupsInfo.name[groupNum][i] = '\0';
			ptr += i;
			if(*ptr != '\0' && *ptr != '\n')
			{
				for(i=1;*(ptr+i)==' '&&*(ptr+i)!='\0';i++)
					;
				ptr += i;
				if(*ptr != '\0'&& *ptr != '\n')
				{
					for(i=0;*(ptr+i)!='\"'&&*(ptr+i)!=' '&&*(ptr+i)!='\0'&&i<32;i++)
					{
						GroupsInfo.items[groupNum][i] = *(ptr+i);
					}
					GroupsInfo.items[groupNum][i] = '\0';
				}
				else
					GroupsInfo.items[groupNum][0] = '\0';
			}
			else
				GroupsInfo.items[groupNum][0] = '\0';
			j ++;
			continue;
		}
		if(strstr(buf,"EGROUP"))
		{
			if(buf[0]!='E')
				continue;
			groupNum = buf[6]-'0';
			if(groupNum > 9 || groupNum < 0)
				continue;
			ptr = strchr(buf,'\"');
			if(ptr == NULL)
				continue;
			ptr ++;
			for(i=0;*(ptr+i)!='\"'&&*(ptr+i)!=' '&&*(ptr+i)!='\0'&&i<8;i++)
			{
				GroupsInfo.chs[groupNum][i] = *(ptr+i);
			}
			GroupsInfo.chs[groupNum][i] = '\0';
			k ++;
		}
	}
	fclose(mfp);
	if( j != k )
	{
		GroupsInfo.update = 0;
		return 0;
	}
	GroupsInfo.num = j;
	return 1;
}

int chkGroupsInfo()
{
	int haveError = NA,i;
	clear();
	if(getGroupSet() == 0 )
		haveError = YEA;
	else
	{
		for( i =0 ; i < 10; i++)
			if((GroupsInfo.name[i][0]=='\0')^(GroupsInfo.chs[i][0]=='\0'))
				haveError=YEA;
	}
	if(haveError != YEA)
		return 1;
	prints("\n��\033[0;1;4;33mע��\033[m��ϵͳ���� menu.ini ������"
	       "���ܴ������⣬��������ټ������У�\n        ");
	prints("��������׸���ʾ�ĺ��壬�뿽��������Ϣ����ˮվ�������⣡\n\n");
	prints("��վĿǰ������������������£�\n\n");
	prints(" ����   ��������     �����������             ������\n");
	prints("-------------------------------------------------------\n");
	for( i =0 ; i < 10; i++)
	{
		if(GroupsInfo.name[i][0]=='\0'&&GroupsInfo.chs[i][0]=='\0')
			continue;
		prints("GROUP_%d %-12s %-24s \"%s\"\n",i,
		       GroupsInfo.name[i][0]=='\0'?"######":GroupsInfo.name[i],
		       GroupsInfo.items[i][0]=='\0'?"######":GroupsInfo.items[i],
		       GroupsInfo.chs[i][0]=='\0'?"######":GroupsInfo.chs[i]);
	}
	prints("\n\n");
	if(askyn("�������Ϣ���� menu.ini ������������Ȼ�����ִ����",NA,NA)==YEA)
		return 1;
	return 0;
}

void ShowBrdSet(struct boardheader fh);

int ChangeTheBoard(struct boardheader *oldfh, int pos)
{
	struct boardheader newfh;
	char buf[STRLEN], title[STRLEN], vbuf[100],ch;
	char num[2],chs[2],items[9],nntp[3],group[16];
	int  i,j,a_mv;

	if (oldfh)
		memcpy(&newfh, oldfh, sizeof(newfh));
	else
		memset(&newfh, 0, sizeof(newfh));
	move(2,0);
	if(oldfh)
	{
		prints("���������趨 [\033[32m%s\033[m] �İ������� [Enter-->�趨����]",
		       oldfh->filename);
	}
	else
		prints("������ʼ����һ��\033[32m��\033[m�������� [ENTER-->ȡ������]");
	while(1)
	{
		struct boardheader dh;
		getdata(3, 0, "����������(Ӣ����): ", buf, 18, DOECHO, YEA);
		if(buf[0]=='\0')
		{
			if(oldfh)
				strcpy(newfh.filename,oldfh->filename);
			else
				return -1;
			break;
		}
		if(killwordsp(buf)==0)
			continue;
		if (!valid_brdname(buf))
			continue;
		if (search_record(BOARDS,&dh,sizeof(dh),cmpbnames,buf))
		{
			prints("\n����! ���������Ѿ�����!!");
			if(oldfh)
				continue;
			pressanykey();
			return -1;
		}
		strcpy(newfh.filename, buf);
		break;
	}
	prints("\n[������˵��]�������� + �������� + ת�ű�־ + ����������\n");
	if(oldfh)
	{
		prints("��ʼ�趨��%11s\033[32m%s\033[m\n",oldfh->title,oldfh->title+11);
	}
	else
		prints("��ʽ������a[��վ] �� \033[32m����һ��վ��������\033[m\n");
	while(1)
	{
		if(oldfh)
			strcpy(buf,oldfh->title+11);
		else
			buf[0] = '\0';
		getdata(8, 0, "����������(������): ", buf, 41, DOECHO, NA);
		if(buf[0]=='\0')
		{
			if(oldfh == NULL)
				return -1;
			continue;
		}
		if(killwordsp(buf)==0)
			continue;
		strcpy(title,buf);
		break;
	}
	if(askyn("����������Ҫ����ת����",NA,NA)==YEA)
	{
		strcpy(nntp,"��");
		newfh.flag |= OUT_FLAG;
	}
	else
	{
		strcpy(nntp,"��");
		newfh.flag &= ~OUT_FLAG;
	}
	if(oldfh)
	{
		ch = oldfh->title[0];
		for(i=0;i<10;i++)
		{
			if(GroupsInfo.chs[i][0]!='\0'&&strchr(GroupsInfo.chs[i],ch))
				break;
		}
		if(i>=10)
			ch = '0';
		else
			ch = '0'+i;
	}
	else
		ch = '0';
	//   for(i=0,j=3;i<10;i++){
	for(i=0,j=1;i<10;i++)
	{ //modified by yiyo
		if(GroupsInfo.name[i][0] == '\0')
			continue;
		move(j++,60);
		prints("��%s(%d)%-12s\033[m��",
		       (i+'0'==ch)?"\033[32m":"",i,GroupsInfo.name[i]);
	}
	while(1)
	{
		num[0] = ch;
		num[1] = '\0';
		getdata(11, 0, "����������������һ����(�ο��ұ���ʾ���)��: ",
		        num, 2, DOECHO, NA);
		if(num[0]=='\0'||num[0]<'0'||num[0]>'9')
			continue;
		if(GroupsInfo.name[num[0]-'0'][0] == '\0')
			continue;
		break;
	}
	prints("\n�� %c ���ķ�����Ųο���[\033[32m%s\033[m] ���������ο���%s",
	       num[0],GroupsInfo.chs[num[0]-'0'],GroupsInfo.items[num[0]-'0']);
	while(1)
	{
		if(oldfh==NULL||!strchr(GroupsInfo.chs[num[0]-'0'],oldfh->title[0]))
			chs[0] = GroupsInfo.chs[num[0]-'0'][0];
		else
			chs[0] = oldfh->title[0];
		chs[1] = '\0';
		getdata(14,0,"��������������ķ������: ",chs,2,DOECHO,NA);
		if(chs[0]=='\0')
			continue;
		if(strchr(GroupsInfo.chs[num[0]-'0'],chs[0]))
			break;
	}
	move(12,0);
	prints("\n�� %c ���ķ�����Ųο���[%s] ���������ο���\033[32m%s\033[m",
	       num[0],GroupsInfo.chs[num[0]-'0'],GroupsInfo.items[num[0]-'0']);
	while(1)
	{
		if(oldfh)
		{
			strncpy(buf,oldfh->title+1,7);
			buf[6] = '\0';
		}
		else
			buf[0] = '\0';
		getdata(15,0,"��������������ķ�������: ",buf,7,DOECHO,NA);
		if(buf[0] == '\0')
			continue;
		if(killwordsp(buf)==0)
			continue;
		strcpy(items,buf);
		break;
	}
	sprintf(newfh.title,"%c%-6s %s %s",chs[0],items,nntp,title);
	if(oldfh==NULL||oldfh->BM[0] == '\0'||oldfh->BM[0] == ' '
	        ||!strncmp(newfh.BM,"SYSOPs",6))
	{
		prints("\n��������  [ �������Ҫ��������ϵͳ�Զ�����Ϊ�� SYSOPs ����]\n");
		if(askyn("������Ҫ������",YEA,NA)==NA)
			strcpy(newfh.BM, "SYSOPs");
		else
			newfh.BM[0] = '\0';
	}
	else
	{
		prints("\n��������  [ Ŀǰ����İ�����: %s ]\n", oldfh->BM);
		if(askyn("�����ְ������������ʱ�򣬲���Ҫ�޸Ĵ˴�����Ҫ�޸İ���������",
		         NA,NA)==YEA)
		{
			prints("��\033[31m����\033[m���޸İ���������������ʹ�ã�"
			       "������������Ķ��˴���\n");
			strcpy(newfh.BM,oldfh->BM);
			getdata(20,0,"��������: ",newfh.BM,sizeof(newfh.BM),DOECHO,NA);
			if(newfh.BM[0] == ' ')
				newfh.BM[0] = '\0';
		}
		else
			strcpy(newfh.BM,oldfh->BM);
	}
	clear();
	if(askyn("����������Ҫ���� READ �� POST ������",NA,NA)==YEA)
	{
		char ans[4];
		sprintf(buf, "���� (R)�Ķ� �� (P)���� ���� [%c]: ",
		        oldfh?(newfh.level & PERM_POSTMASK ? 'P' : 'R'):'P');
		getdata(3, 0, buf, ans, 2, DOECHO, YEA);
		if(ans[0]=='\0')
		{
			ans[0] = oldfh?(newfh.level & PERM_POSTMASK ? 'P' : 'R'):'P';
			ans[1] = '\0';
		}
		if ((newfh.level & PERM_POSTMASK)&&(*ans == 'R'||*ans == 'r'))
			newfh.level &= ~PERM_POSTMASK;
		else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P'||*ans == 'p'))
			newfh.level |= PERM_POSTMASK;
		clear();
		move(2, 0);
		prints("�趨 %s '%s' ��������Ȩ��\n",
		       newfh.level & PERM_POSTMASK ? "����" : "�Ķ�", newfh.filename);
		newfh.level = setperms(newfh.level, "Ȩ��", NUMPERMS, showperminfo);
		clear();
	}
	move(1,0);
	if(askyn("���ñ���Ϊ����������",newfh.flag & ANONY_FLAG?YEA:NA,NA)==YEA)
		newfh.flag |= ANONY_FLAG;
	else
		newfh.flag &= ~ANONY_FLAG;
	move(2,0);
	if(askyn("���ñ���Ϊ���� Re ģʽ������",
	         (newfh.flag&NOREPLY_FLAG)?YEA:NA,NA)==YEA)
		newfh.flag |= NOREPLY_FLAG;
	else
		newfh.flag &= ~NOREPLY_FLAG;
	move(3,0);
	if(oldfh!=NULL)
	{
		if(askyn("�Ƿ��ƶ���������λ��",NA,NA)==YEA)
			a_mv = 2;
		else
			a_mv = 0;
	}
	else
	{
		if(askyn("������Ҫ������������",YEA,NA)==YEA)
			a_mv = 1;
		else
			a_mv = 0;
	}
	if(a_mv != 0)
	{
		ch = num[0];
		for(i=0,j=1;i<10;i++)
		{
			if(GroupsInfo.name[i][0] == '\0')
				continue;
			move(j++,50);
			prints("��%s(%d)%s GROUP_%d\033[m",
			       (i+'0'==ch)?"\033[32m":"",i, GroupsInfo.name[i], i);
		}
		while(1)
		{
			strcpy(chs,num);
			getdata(j+1,48,"�������ֲ����ϣ���ѡ��: ",chs,2,DOECHO,NA);
			if(chs[0] == '\0'||chs[0] < '0' || chs[0] > '9')
				continue;
			if(GroupsInfo.name[chs[0]-'0'][0] == '\0')
				continue;
			break;
		}
	}
	ShowBrdSet(newfh);
	if(oldfh)
	{
		if(askyn("��ȷ��Ҫ������",NA,NA)==NA)
			return -1;
		i = pos;
	}
	else
		i = getbnum("");
	sprintf(group,"GROUP_%c",chs[0]);
	if(oldfh== NULL)
	{
		strcpy(vbuf, "vote/");
		strcat(vbuf, newfh.filename);
		setbpath(genbuf, newfh.filename);
		if ((!dashd(genbuf)&&mkdir(genbuf,0755)==-1)
		        ||(!dashd(vbuf)&&mkdir(vbuf, 0755)==-1))
		{
			prints("\n��ʼ��������Ŀ¼ʱ����!\n");
			pressreturn();
			clear();
			return -1;
		}
		if(a_mv!=0)
		{
			if (newfh.BM[0] != '\0')
				sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 8, newfh.BM);
			else
				sprintf(vbuf, "%-38.38s", newfh.title + 8);
			if (add_grp(group,GroupsInfo.name[chs[0]-'0'],newfh.filename,vbuf)==-1)
				prints("\n����������ʧ��....\n");
			else
				prints("�Ѿ����뾫����...\n");
		}
	}
	else
	{  // �޸�������
		char tmp_grp[30];
		if (strcmp(oldfh->filename, newfh.filename))
		{
			char    old[256], tar[256];
			a_mv = 1;
			setbpath(old, oldfh->filename);
			setbpath(tar, newfh.filename);
			rename(old, tar);
			sprintf(old, "vote/%s", oldfh->filename);
			sprintf(tar, "vote/%s", newfh.filename);
			rename(old, tar);
		}
		if (newfh.BM[0] != '\0')
			sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 8, newfh.BM);
		else
			sprintf(vbuf, "%-38.38s", newfh.title + 8);
		get_grp(oldfh->filename);
		edit_grp(oldfh->filename, lookgrp, oldfh->title + 8, vbuf);
		if(a_mv)
		{
			get_grp(oldfh->filename);
			strcpy(tmp_grp, lookgrp);
			if(strcmp(tmp_grp,group)||a_mv == 1)
			{
				char tmpbuf[160],oldpath[STRLEN], newpath[STRLEN];
				sprintf(tmpbuf,"%s:",oldfh->filename);
				del_from_file("0Announce/.Search", tmpbuf);
				if (newfh.BM[0] != '\0')
					sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 8, newfh.BM);
				else
					sprintf(vbuf, "%-38.38s", newfh.title + 8);
				if (add_grp(group, GroupsInfo.name[chs[0]-'0'],
				            newfh.filename, vbuf) == -1)
					prints("\n����������ʧ��....\n");
				else
					prints("�Ѿ����뾫����...\n");
				sprintf(newpath,"0Announce/groups/%s/%s",group,newfh.filename);
				sprintf(oldpath,"0Announce/groups/%s/%s",tmp_grp,oldfh->filename);
				if (dashd(oldpath))
				{
					deltree(newpath);
				}
				rename(oldpath, newpath);
				del_grp(tmp_grp, oldfh->filename, oldfh->title + 8);
			}
		}
	}
	if( i > 0)
	{
		substitute_record(BOARDS, &newfh, sizeof(newfh), i);
		if(oldfh)
		{
			sprintf(genbuf, "���������� %s ������ --> %s",
			        oldfh->filename, newfh.filename);
			report(genbuf);
		}
	}
	else if(append_record(BOARDS, &newfh, sizeof(newfh)) == -1)
	{
		prints("\n�����°�ʧ�ܣ�\n");
		pressanykey();
		clear();
		return -1;
	}
	numboards = -1;
	{
		char    secu[STRLEN];
		if(oldfh)
			sprintf(secu, "�޸���������%s(%s)",oldfh->filename,newfh.filename);
		else
			sprintf(secu, "�����°壺%s", newfh.filename);
		securityreport(secu);
	}
	pressanykey();
	clear();
}

int m_newbrd()
{
	modify_user_mode(ADMIN);
	if(!check_systempasswd())
		return -1;
	if(chkGroupsInfo()==0)
		return -1;
	clear();
	stand_title("������������") ;
	ChangeTheBoard(NULL,0);
	return 0 ;
}
void ShowBrdSet(struct boardheader fh)
{
	int i = 4;
	move(i,0);
	prints("������ [%s] ���������",fh.filename);
	i += 2;
	move(i++,0);
	prints("��Ŀ����           ��Ŀ����");
	move(i++,0);
	prints("------------------------------");
	move(i++,0);
	prints("����������       : %s",fh.filename);
	move(i++,0);
	prints("����������Ա     : %s",fh.BM[0]=='\0'?"���ް���":fh.BM);
	move(i++,0);
	prints("�Ƿ�Ϊ���������� : %s",(fh.flag&ANONY_FLAG) ? "����" : "������");
	move(i++,0);
	prints("�����Ƿ���Իظ� : %s",(fh.flag&NOREPLY_FLAG)?"���� Re":"����");
	move(i++,0);
	prints("READ/POST ����   : ");
	if(fh.level & ~PERM_POSTMASK)
	{
		prints("%s", (fh.level & PERM_POSTMASK) ? "POST" :
		       (fh.level & PERM_NOZAP) ? "ZAP" : "READ");
	}
	else
		prints("������");
	move(i++,0);
	prints("��������˵��     : %s\n\n",fh.title);
}
int m_editbrd()
{
	int     pos;
	struct boardheader fh;
	char    bname[STRLEN];

	modify_user_mode(ADMIN);
	if(!check_systempasswd())
		return -1;
	if(chkGroupsInfo()==0)
		return -1;
	clear();
	stand_title("�޸���������������");
	if(!gettheboardname(2,"��������������: ",&pos,&fh,bname))
		return -1;
	ShowBrdSet(fh);
	move(15, 0);
	if (askyn("�Ƿ���Ҫ������������", NA, NA) == YEA)
	{
		clear();
		stand_title("�޸���������������");
		ChangeTheBoard(&fh,pos);
	}
	return 0;
}
#else

int m_newzone()
{
        struct boardheader newboard;
        extern int numboards;
        char    ans[4];
        //char    vbuf[100];
        //char   *group;
        int     bid;
	char	zonename[STRLEN];
        modify_user_mode(ADMIN);
        if (!check_systempasswd())
                return;
        clear();
        stand_title("�������ӷ���");
        memset(&newboard, 0, sizeof(newboard));
        move(2, 0);
        //   ansimore2("etc/boardref", NA, 3, 7);
        ansimore2("etc/zoneref", NA, 2, 9);
        while (1)
        {
                getdata(11, 0, "�ӷ�������:   ", newboard.filename, 18, DOECHO, YEA);
                if (newboard.filename[0] != 0)
                {
                        struct boardheader dh;
                        if (search_record(BOARDS,&dh,sizeof(dh),cmpbnames,newboard.filename))
                        {
                                prints("\n����! ���������Ѿ�����!!");
                                pressanykey();
                                return -1;
                        }
                }
                else
                        return -1;
                if (valid_brdname(newboard.filename))
                        break;
                prints("\n���Ϸ�����!!");
        }
        newboard.flag = 0;
        while (1)
        {
                getdata(12, 0, "�ӷ���˵��:   ", newboard.title, 60, DOECHO, YEA);
                if (newboard.title[0] == '\0')
                        return -1;
                if (strstr(newboard.title, "��"))
                {
                        newboard.flag |= ZONE_FLAG;
                        break;
                }
                else
                        prints("������ӷ�����ʽ!!");
        }
        if (   getbnum(newboard.filename) > 0 )
        {
                prints("\n������ӷ�������!!\n");
                pressreturn();
                clear();
                return -1;
        }
        newboard.BM[0] = '\0';
        move(13,0);
	if (askyn("�÷����Ƿ�Ϊ�༶�ӷ���", NA, NA) == YEA)
        {
                while(1)
                {
                        getdata(15, 0, "���ӷ�������:   ", zonename, 18, DOECHO, YEA);
                        if(search_zone(BOARDS,zonename,newboard.title[0]))
                        {
                                break;
                        }
                }
                strcpy(newboard.owner,zonename);
        }
        if (askyn("�Ƿ����ƴ�ȡȨ��", NA, NA) == YEA)
        {
                getdata(17, 0, "���� Read/Post? [R]: ", ans, 2, DOECHO, YEA);
                if (*ans == 'P' || *ans == 'p')
                        newboard.level = PERM_POSTMASK;
                else
                        newboard.level = 0;
                move(1, 0);
                clrtobot();
                move(2, 0);
                prints("�趨 %s Ȩ��. ������: '%s'\n",
                       (newboard.level & PERM_POSTMASK ? "POST" : "READ"),newboard.filename);
                newboard.level=setperms(newboard.level,"Ȩ��",NUMPERMS,showperminfo);
                clear();
        }
        else
        {
                newboard.level = 0;
        }
        move(15, 0);
        if ((bid = getbnum("")) > 0)
        {
                substitute_record(BOARDS, &newboard, sizeof(newboard), bid);
        }
        else if (append_record(BOARDS, &newboard, sizeof(newboard)) == -1)
        {
                pressreturn();
                clear();
                return -1;
        }
        numboards = -1;
        prints("\n���ӷ�������\n");
        {
                char    secu[STRLEN];
                sprintf(secu, "�������ӷ�����%s", newboard.filename);
                securityreport(secu);
        }
        pressreturn();
        clear();
        return 0;

}

int m_newbrd()
{
	struct boardheader newboard;
	extern int numboards;
	char    ans[4];
	char    vbuf[100];
	char   *group;
	int     bid;
	char    zonename[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;
	clear();
	stand_title("������������");
	memset(&newboard, 0, sizeof(newboard));
	move(2, 0);
	//   ansimore2("etc/boardref", NA, 3, 7);
	ansimore2("etc/boardref", NA, 2, 9);
	while (1)
	{
		getdata(11, 0, "����������:   ", newboard.filename, 18, DOECHO, YEA);
		if (newboard.filename[0] != 0)
		{
			struct boardheader dh;
			if (search_record(BOARDS,&dh,sizeof(dh),cmpbnames,newboard.filename))
			{
				prints("\n����! ���������Ѿ�����!!");
				pressanykey();
				return -1;
			}
		}
		else
			return -1;
		if (valid_brdname(newboard.filename))
			break;
		prints("\n���Ϸ�����!!");
	}
	newboard.flag = 0;
	while (1)
	{
		getdata(12, 0, "������˵��:   ", newboard.title, 60, DOECHO, YEA);
		if (newboard.title[0] == '\0')
			return -1;
		if (strstr(newboard.title, "��") || strstr(newboard.title, "��"))
		{
			newboard.flag |= OUT_FLAG;
			break;
		}
		else if (strstr(newboard.title, "��"))
		{
			newboard.flag &= ~OUT_FLAG;
			break;
		}
		else
			prints("����ĸ�ʽ, �޷��ж��Ƿ�ת��!!");
	}
	strcpy(vbuf, "vote/");
	strcat(vbuf, newboard.filename);
	setbpath(genbuf, newboard.filename);
	if (   getbnum(newboard.filename) > 0 || mkdir(genbuf, 0755) == -1
	        || mkdir(vbuf, 0755) == -1)
	{
		prints("\n���������������!!\n");
		pressreturn();
		clear();
		return -1;
	}
	move(13,0);
	if(askyn("�������������(������SYSOPs����)?",YEA,NA)==NA)
		strcpy(newboard.BM, "SYSOPs");
	else
		newboard.BM[0] = '\0';
	move(14, 0);
	if (askyn("�Ƿ����ƴ�ȡȨ��", NA, NA) == YEA)
	{
		getdata(15, 0, "���� Read/Post? [R]: ", ans, 2, DOECHO, YEA);
		if (*ans == 'P' || *ans == 'p')
			newboard.level = PERM_POSTMASK;
		else
			newboard.level = 0;
		move(1, 0);
		clrtobot();
		move(2, 0);
		prints("�趨 %s Ȩ��. ������: '%s'\n",
		       (newboard.level & PERM_POSTMASK ? "POST" : "READ"),newboard.filename);
		newboard.level=setperms(newboard.level,"Ȩ��",NUMPERMS,showperminfo);
		clear();
	}
	else
		newboard.level = 0;
	move(15, 0);
	if (askyn("�Ƿ����������", NA, NA) == YEA)
		newboard.flag |= ANONY_FLAG;
	else
		newboard.flag &= ~ANONY_FLAG;
	if (askyn("�ð��ȫ�����¾������Իظ�", NA, NA) == YEA)
		newboard.flag |= NOREPLY_FLAG;
	else
		newboard.flag &= ~NOREPLY_FLAG;
	if (askyn("����ð�Ϊ���ʽ���ֲ������Ƿ�Էǻ�Ա���أ�", NA, NA) == YEA)//loveni
		newboard.flag2 |= HIDE_FLAG;
	else
		newboard.flag2 &= ~HIDE_FLAG;
	if (askyn("�����Ƿ�����������", NA, NA) == YEA)//loveni
		newboard.flag2 |= JUNK_FLAG;
	else
		newboard.flag2 &= ~JUNK_FLAG;
	if (askyn("���������Ƿ�ͳ����ʮ��", NA, NA) == YEA)//loveni
		newboard.flag2 |= NOTOPTEN_FLAG;
	else
		newboard.flag2 &= ~NOTOPTEN_FLAG;
	if (askyn("�����Ƿ�\033[1;31m�����������֣�\033[m", NA, NA) == YEA)//loveni
		newboard.flag2 |= NOFILTER_FLAG;
	else
		newboard.flag2 &= ~NOFILTER_FLAG;
	if (askyn("�����Ƿ�ǿ��ģ�巢�ģ�", NA, NA) == YEA)//loveni
		newboard.flag2 |= TEMPLATE_FLAG;
	else
		newboard.flag2 &= ~TEMPLATE_FLAG;
        if (askyn("�ð��Ƿ�Ϊ��������", NA, NA) == YEA)
        {
                newboard.flag |= CHILDBOARD_FLAG;
                while(1)
                {
                        getdata(20, 0, "�ӷ�������:   ", zonename, 18, DOECHO, YEA);
                        if(search_zone(BOARDS,zonename,newboard.title[0]))
                        {
                                break;
                        }
                }
                strcpy(newboard.owner,zonename);
        }
        else
	{
                newboard.flag &= ~CHILDBOARD_FLAG;
	}
	if ((bid = getbnum("")) > 0)
	{
		substitute_record(BOARDS, &newboard, sizeof(newboard), bid);
	}
	else if (append_record(BOARDS, &newboard, sizeof(newboard)) == -1)
	{
		pressreturn();
		clear();
		return -1;
	}
	group = chgrp();
	if (group != NULL)
	{
		if (newboard.BM[0] != '\0')
			sprintf(vbuf, "%-38.38s(BM: %s)", newboard.title + 8, newboard.BM);
		else
			sprintf(vbuf, "%-38.38s", newboard.title + 8);
		if (add_grp(group, cexplain, newboard.filename, vbuf) == -1)
			prints("\n����������ʧ��....\n");
		else
			prints("�Ѿ����뾫����...\n");
	}
	numboards = -1;
	prints("\n������������\n");
	{
		char    secu[STRLEN];
		sprintf(secu, "�����°壺%s", newboard.filename);
		securityreport(secu);
	}
	pressreturn();
	clear();
	return 0;
}

int m_editbrd()
{
	char    bname[STRLEN], buf[STRLEN], oldtitle[STRLEN], vbuf[256], *group;
	char    oldpath[STRLEN], newpath[STRLEN], tmp_grp[30];
	int     pos, noidboard, a_mv;
	struct boardheader fh, newfh;
	int zonefather=0;

	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;
	clear();
	stand_title("�޸���������Ѷ");
	if(!gettheboardname(2,"��������������: ",&pos,&fh,bname))
		return -1;
	if(fh.flag & ZONE_FLAG)
        {
                move(2,0);
                prints("���ﲻ���޸��ӷ�����Ϣ\n");
                pressreturn();
                return -1;
        }
	noidboard = fh.flag & ANONY_FLAG;
	noreply = fh.flag & NOREPLY_FLAG;
	clear();
	move(0, 0);
	memcpy(&newfh, &fh, sizeof(newfh));
	prints("����������:  %s               ����������Ա:  %s\n", fh.filename, fh.BM);
	prints("������˵��:     %s\n", fh.title);
//	prints("����������Ա:   %s\n", fh.BM);
	prints("����������:     %s             ", (noidboard) ? "Yes" : "No");
	prints("���²����Իظ�: %s\n", (noreply) ? "Yes" : "No");
	prints("���ʽ���ֲ������Ƿ�Էǻ�Ա����:%s\n",fh.flag2 & HIDE_FLAG ? "Yes" : "No");
	prints("�����Ƿ���������:%s            ",fh.flag2 & JUNK_FLAG ? "Yes" : "No");
	prints("���������Ƿ�ͳ����ʮ��:%s\n",fh.flag2 & NOTOPTEN_FLAG? "Yes" : "No");
	prints("�����Ƿ�\033[1;31m������������\033[m:%s\n",fh.flag2 & NOFILTER_FLAG ? "Yes" : "No");//loveni
	prints("�����Ƿ�ǿ��ģ�巢��:%s\n",fh.flag2 & TEMPLATE_FLAG ? "Yes" : "No");//loveni
	prints("�Ƿ����������: %s ", fh.flag&CHILDBOARD_FLAG ? "Yes":"No");
	strcpy(oldtitle, fh.title);
	prints("���� %s Ȩ��: %s", (fh.level & PERM_POSTMASK) ? "POST" :
	       (fh.level & PERM_NOZAP) ? "ZAP" : "READ",
	       (fh.level & ~PERM_POSTMASK) == 0 ? "������" : "������");
	move(8, 0);
	if (askyn("�Ƿ����������Ѷ", NA, NA) == NA)
	{
		clear();
		return 0;
	}
	move(9, 0);
	prints("ֱ�Ӱ� <Return> ���޸Ĵ�����Ѷ...");
	while (1)
	{
		getdata(10, 0, "������������: ", genbuf, 18, DOECHO, YEA);
		if (*genbuf != 0)
		{
			struct boardheader dh;
			if (search_record(BOARDS, &dh, sizeof(dh), cmpbnames, genbuf))
			{
				move(2, 0);
				prints("����! ���������Ѿ�����!!");
				move(10, 0);
				clrtoeol();
				continue;
			}
			if (valid_brdname(genbuf))
			{
				strncpy(newfh.filename, genbuf, sizeof(newfh.filename));
				strcpy(bname, genbuf);
				break;
			}
			else
			{
				move(2, 0);
				prints("���Ϸ�������������!");
				move(10, 0);
				clrtoeol();
				continue;
			}
		}
		else
			break;
	}
	//   ansimore2("etc/boardref", NA, 11, 7);
	ansimore2("etc/boardref", NA, 11, 6);
	while (1)
	{
		getdata(17, 0, "��������˵��: ", genbuf, 60, DOECHO, YEA);
		if (*genbuf != 0)
			strncpy(newfh.title, genbuf, sizeof(newfh.title));
		else
			break;
		if (strstr(newfh.title, "��") || strstr(newfh.title, "��"))
		{
			newfh.flag |= OUT_FLAG;
			break;
		}
		else if (strstr(newfh.title, "��"))
		{
			newfh.flag &= ~OUT_FLAG;
			break;
		}
		else
			prints("\n����ĸ�ʽ, �޷��ж��Ƿ�ת��!!");
	}
	move(18, 0);
	//clrtoeol();
	//move(20, 0);
	if( fh.BM[0] != '\0' && strcmp(fh.BM,"SYSOPs"))
	{
		if(askyn("�޸�����������Ա��ע�⣺ȷ�д���ſ��޸ġ����޸İ��س�",
		         NA,NA) == YEA)
		{
			getdata(18,0,"����������Ա(��ע�⡿������������Ķ��˴���): ",
			        newfh.BM,sizeof(newfh.BM),DOECHO,YEA);
			if ( newfh.BM[0] == '\0' )
				strcpy(newfh.BM,fh.BM);
			else if (newfh.BM[0] == ' ')
				newfh.BM[0]='\0';
			else
				;
		}
	}
	else
	{
		if(askyn("�������������(������SYSOPs����)?",YEA,NA)==NA)
			strncpy(newfh.BM, "SYSOPs", sizeof(newfh.BM));
		else
			strncpy(newfh.BM, "\0", sizeof(newfh.BM));
	}
	sprintf(buf, "������ (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
	getdata(19, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag |= ANONY_FLAG;
		else
			newfh.flag &= ~ANONY_FLAG;
	}
	sprintf(buf, "�����Ƿ񲻿��Իظ� (Y/N)? [%c]: ", (noreply) ? 'Y' : 'N');
	getdata(19, 30, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag |= NOREPLY_FLAG;
		else
			newfh.flag &= ~NOREPLY_FLAG;
	}
	
	sprintf(buf, "����ð�Ϊ���ʽ���ֲ������Ƿ�Էǻ�Ա���أ� (Y/N)? [%c]: ", (fh.flag2&HIDE_FLAG) ? 'Y' : 'N');
	getdata(20, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= HIDE_FLAG;
		else
			newfh.flag2 &= ~HIDE_FLAG;
	}

	move(21, 0);
        if(newfh.flag & CHILDBOARD_FLAG)
        {
                if(askyn("�����ӷ������ķ�",NA,NA)==YEA)
                {
                        while(1)
                        {
                                sprintf(buf,"�����ӷ�������Ϊ(.Ϊȡ���ӷ���): ");
                                getdata(21,0,buf,genbuf,18,DOECHO,YEA);
                                if(strcmp(genbuf,".")==0)
                                {
                                        zonefather=0;
                                        break;
                                }
                                if(search_zone(BOARDS,genbuf,newfh.title[0]))
                                {
                                        zonefather=1;
                                        break;
                                }
                        }
                        if(zonefather==0)
                        {
                                newfh.flag &= ~CHILDBOARD_FLAG;
                                strcpy(newfh.owner,"");
                        }
                        else
                        {
                                newfh.flag |= CHILDBOARD_FLAG;
                                strcpy(newfh.owner,genbuf);
                        }
                }
        }
        else
        {
                if(askyn("�������κ��ӷ������ķ�",NA,NA)==YEA)
                {
                        while(1)
                        {
                                sprintf(buf,"�����ӷ�������: ");
                                getdata(21,0,buf,genbuf,18,DOECHO,YEA);
                                if(search_zone(BOARDS,genbuf,newfh.title[0]))
                                {
                                        zonefather=1;
                                        break;
                                }
                        }
                        newfh.flag |= CHILDBOARD_FLAG;
                        strcpy(newfh.owner,genbuf);
                }
        }
	move(22,0);
	if (askyn("�Ƿ��ƶ���������λ��", NA, NA) == YEA)
		a_mv = 2;
	else
		a_mv = 0;
	move(23, 0);
	if (askyn("�Ƿ���Ĵ�ȡȨ��", NA, NA) == YEA)
	{
		char    ans[4];
		sprintf(genbuf, "���� (R)�Ķ� �� (P)���� ���� [%c]: ",
		        (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
		getdata(24, 0, genbuf, ans, 2, DOECHO, YEA);
		if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
			newfh.level &= ~PERM_POSTMASK;
		else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
			newfh.level |= PERM_POSTMASK;
		clear();
		move(2, 0);
		prints("�趨 %s '%s' ��������Ȩ��\n",
		       newfh.level & PERM_POSTMASK ? "����" : "�Ķ�", newfh.filename);
		newfh.level = setperms(newfh.level, "Ȩ��", NUMPERMS, showperminfo);
		//clear();
		//getdata(0, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
	}
	//else
	//{
	clear();
	sprintf(buf, "�����Ƿ����������� (Y/N)? [%c]: ", (fh.flag2&JUNK_FLAG) ? 'Y' : 'N');
	getdata(0, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= JUNK_FLAG;
		else
			newfh.flag2 &= ~JUNK_FLAG;
	}
	sprintf(buf, "���������Ƿ�ͳ����ʮ�� (Y/N)? [%c]: ", (fh.flag2&NOTOPTEN_FLAG) ? 'Y' : 'N');
	getdata(1, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= NOTOPTEN_FLAG;
		else
			newfh.flag2 &= ~NOTOPTEN_FLAG;
	}
	sprintf(buf, "�����Ƿ�\033[1;31m�����������֣�\033[m (Y/N)? [%c]: ", (fh.flag2&NOFILTER_FLAG) ? 'Y' : 'N');
	getdata(2, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= NOFILTER_FLAG;
		else
			newfh.flag2 &= ~NOFILTER_FLAG;
	}
	sprintf(buf, "�����Ƿ�ǿ��ģ�巢�� (Y/N)? [%c]: ", (fh.flag2&TEMPLATE_FLAG) ? 'Y' : 'N');
	getdata(3, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= TEMPLATE_FLAG;
		else
			newfh.flag2 &= ~TEMPLATE_FLAG;
	}
	
		
	getdata(4, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
	//}
	if (*genbuf != 'Y' && *genbuf != 'y')
	{
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "�޸���������%s(%s)", fh.filename, newfh.filename);
		securityreport(secu);
	}
	if (strcmp(fh.filename, newfh.filename))
	{
		char    old[256], tar[256];

		a_mv = 1;
		setbpath(old, fh.filename);
		setbpath(tar, newfh.filename);
		rename(old, tar);
		sprintf(old, "vote/%s", fh.filename);
		sprintf(tar, "vote/%s", newfh.filename);
		rename(old, tar);
	}
	if (newfh.BM[0] != '\0')
		sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 8, newfh.BM);
	else
		sprintf(vbuf, "%-38.38s", newfh.title + 8);
	get_grp(fh.filename);
	edit_grp(fh.filename, lookgrp, oldtitle + 8, vbuf);

	if (a_mv >= 1)
	{
		group = chgrp();
		get_grp(fh.filename);
		strcpy(tmp_grp, lookgrp);
		if (strcmp(tmp_grp, group) || a_mv != 2)
		{
			char tmpbuf[160];
			sprintf(tmpbuf,"%s:",fh.filename);
			del_from_file("0Announce/.Search", tmpbuf);
			if (group != NULL)
			{
				if (newfh.BM[0] != '\0')
					sprintf(vbuf, "%-38.38s(BM: %s)", newfh.title + 8, newfh.BM);
				else
					sprintf(vbuf, "%-38.38s", newfh.title + 8);
				if (add_grp(group, cexplain, newfh.filename, vbuf) == -1)
					prints("\n����������ʧ��....\n");
				else
					prints("�Ѿ����뾫����...\n");
				sprintf(newpath,"0Announce/groups/%s/%s",group,newfh.filename);
				sprintf(oldpath, "0Announce/groups/%s/%s", tmp_grp, fh.filename);
				if (dashd(oldpath))
				{
					deltree(newpath);
				}
				rename(oldpath, newpath);
				del_grp(tmp_grp, fh.filename, fh.title + 8);
			}
		}
	}
	substitute_record(BOARDS, &newfh, sizeof(newfh), pos);
	sprintf(genbuf, "���������� %s ������ --> %s", fh.filename, newfh.filename);
	report(genbuf);
	numboards = -1;	/* force re-caching */
	clear();
	return 0;
}
#endif

int m_mclean()
{
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	stand_title("���˽���ż�");
	move(1, 0);
	prints("��������Ѷ���δ mark ���ż�\n");
	if (askyn("ȷ����", NA, NA) == NA)
	{
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "�������ʹ�����Ѷ��ż���");
		securityreport(secu);
	}

	cleanlog = fopen("mailclean.log", "w");
	move(3, 0);
	prints("�����ĵȺ�.\n");
	refresh();
	if (apply_record(PASSFILE, cleanmail, sizeof(struct userec)) == -1)
	{
		move(4, 0);
		prints("apply PASSFILE err...\n");
		pressreturn();
		clear();
		fclose(cleanlog);
		return -1;
	}
	move(4, 0);
	fclose(cleanlog);
	prints("������! ��¼�� mailclean.log.\n");
	pressreturn();
	clear();
	return 0;
}
/*
int
m_trace()
{
	struct stat ostatb, cstatb;
	int     otflag, ctflag, done = 0;
	char    ans[3];
	char   *msg;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("Set Trace Options");
	while (!done) {
		move(2, 0);
		otflag = stat("trace", &ostatb);
		ctflag = stat("trace.chatd", &cstatb);
		prints("Ŀǰ�趨:\n");
		trace_state(otflag, "һ��", ostatb.st_size);
		trace_state(ctflag, "����", cstatb.st_size);
		move(9, 0);
		prints("<1> �л�һ���¼\n");
		prints("<2> �л������¼\n");
		getdata(12, 0, "��ѡ�� (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);
 
		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "һ���¼ ON";
			} else {
				rename("trace", "trace.old");
				msg = "һ���¼ OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "�����¼ ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "�����¼ OFF";
			}
			break;
		default:
			msg = NULL;
			done = 1;
		}
		move(t_lines - 2, 0);
		if (msg) {
			prints("%s\n", msg);
			report(msg);
		}
	}
	clear();
	return 0;
}
*/
//extern int GetAuthUserDispColor( char * userid);
extern int PrintColorUser( char * userid);
int m_register()
{
	FILE   *fn;
	char    ans[3], *fname;
	int     x, y, wid, len;
	char    uident[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();

	stand_title("�趨ʹ����ע������");
	for (;;)
	{
		getdata(1, 0, "(0)�뿪  (1)�����ע������  (2)��ѯʹ����ע������ ? : ",
		        ans, 2, DOECHO, YEA);
		if (ans[0] == '1' || ans[0] == '2')
			break;
		else
			return 0;
	}
	if (ans[0] == '1')
	{
		fname = "new_register";
		if ((fn = fopen(fname, "r")) == NULL)
		{
			prints("\n\nĿǰ������ע������.");
			pressreturn();
		}
		else
		{
			y = 3, x = wid = 0;
			while (fgets(genbuf, STRLEN, fn) != NULL && x < 65)
			{
				if (strncmp(genbuf, "userid: ", 8) == 0)
				{
					move(y++, x);
					PrintColorUser(genbuf+8);
		//prints("\033[%dm%s \033[37m",GetAuthUserDispColor(genbuf + 8),
		//				genbuf + 8,
		//GetAuthUserDispColor(genbuf + 8));
		//			prints("%s",genbuf + 8);
					len = strlen(genbuf + 8);
					if (len > wid)
						wid = len;
					if (y >= t_lines - 2)
					{
						y = 3;
						x += wid + 6;
					}
				}
			}
			fclose(fn);
			if (askyn("\33[37m�趨������", NA, YEA) == YEA)
			{
				securityreport("�趨ʹ����ע������");
				scan_register_form(fname);
			}
		}
	}
	else
	{
		move(1, 0);
		usercomplete("������Ҫ��ѯ�Ĵ���: ", uident);
		if (uident[0] != '\0')
			if (!getuser(uident))
			{
				move(2, 0);
				prints("�����ʹ���ߴ���...");
			}
			else
			{
				sprintf(genbuf, "home/%c/%s/register", toupper(lookupuser.userid[0]), lookupuser.userid);
				if ((fn = fopen(genbuf, "r")) != NULL)
				{
					prints("\nע����������:\n\n");
					for (x = 1; x <= 15; x++)
					{
						if (fgets(genbuf, STRLEN, fn))
							prints("%s", genbuf);
						else
							break;
					}
					fclose(fn); /* add fclose by yiyo */
				}
				else
					prints("\n\n�Ҳ�����/����ע������!!\n");
			}
		pressanykey();
	}
	clear();
	return 0;
}
int d_zone()
{
        struct boardheader binfo;
        int     bid, ans;
        char    bname[STRLEN];
        //extern char lookgrp[];
        extern int numboards;
        if (!HAS_PERM(PERM_BLEVELS))
        {
                return 0;
        }
        modify_user_mode(ADMIN);
        if (!check_systempasswd())
        {
                return;
        }
        clear();
        stand_title("ɾ���ӷ���");
        make_blist();
        move(1, 0);
        namecomplete("�������ӷ���: ", bname);
        if (bname[0] == '\0')
                return 0;
        bid = getbnum(bname);
        if (get_record(BOARDS, &binfo, sizeof(binfo), bid) == -1 || !(binfo.flag & ZONE_FLAG))
        {
                move(2, 0);
                prints("����ȷ���ӷ���\n");
                pressreturn();
                clear();
                return 0;
        }
        if(count_zone(BOARDS,binfo.filename))
        {
                move(2,0);
                prints("�ӷ�������������������ɾ��\n");
                pressreturn();
                clear();
                return 0;
        }
        ans = askyn("��ȷ��Ҫɾ������ӷ���", NA, NA);
        if (ans != 1)
        {
                move(2, 0);
                prints("ȡ��ɾ���ж�\n");
                pressreturn();
                clear();
                return 0;
        }
        {
                char    secu[STRLEN];
                sprintf(secu, "ɾ���ӷ�����%s", binfo.filename);
                securityreport(secu);
        }
        sprintf(genbuf, " << '%s' �� %s ɾ�� >>",
                binfo.filename, currentuser.userid);
        memset(&binfo, 0, sizeof(binfo));
        strncpy(binfo.title, genbuf, STRLEN);
        binfo.level = PERM_SYSOP;
        substitute_record(BOARDS, &binfo, sizeof(binfo), bid);
        move(4, 0);
        prints("\n���ӷ����Ѿ�ɾ��...\n");
        pressreturn();
        numboards = -1;
        clear();
        return 0;
}
int d_board()
{
	struct boardheader binfo;
	int     bid, ans;
	char    bname[STRLEN];
	extern char lookgrp[];
	extern int numboards;
	if (!HAS_PERM(PERM_BLEVELS))
	{
		return 0;
	}
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	stand_title("ɾ��������");
	make_blist();
	move(1, 0);
	namecomplete("������������: ", bname);
	if (bname[0] == '\0')
		return 0;
	bid = getbnum(bname);
	if (get_record(BOARDS, &binfo, sizeof(binfo), bid) == -1 || binfo.flag & ZONE_FLAG)
	{
		move(2, 0);
		prints("����ȷ��������\n");
		pressreturn();
		clear();
		return 0;
	}
	if(binfo.BM[0]!='\0'&&strcmp(binfo.BM,"SYSOPs"))
	{
		move(5,0);
		prints("�ð廹�а�������ɾ������ǰ������ȡ��������������\n");
		pressanykey();
		clear();
		return 0;
	}
	ans = askyn("��ȷ��Ҫɾ�����������", NA, NA);
	if (ans != 1)
	{
		move(2, 0);
		prints("ȡ��ɾ���ж�\n");
		pressreturn();
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "ɾ����������%s", binfo.filename);
		securityreport(secu);
	}
	if (seek_in_file("0Announce/.Search", bname))
	{
		move(4, 0);
		if (askyn("�Ƴ�������", NA, NA) == YEA)
		{
			get_grp(binfo.filename);
			del_grp(lookgrp, binfo.filename, binfo.title + 8);
		}
	}
	if (seek_in_file("etc/junkboards", bname))
		del_from_file("etc/junkboards", bname);
	if (seek_in_file("0Announce/.Search", bname))
	{
		char tmpbuf[160];
		sprintf(tmpbuf,"%s:",bname);
		del_from_file("0Announce/.Search", tmpbuf);
	}
	if (binfo.filename[0] == '\0')
		return -1;	/* rrr - precaution */
	sprintf(genbuf, "boards/%s", binfo.filename);
	f_rm(genbuf);
	sprintf(genbuf, "vote/%s", binfo.filename);
	f_rm(genbuf);
	sprintf(genbuf, " << '%s' �� %s ɾ�� >>",
	        binfo.filename, currentuser.userid);
	memset(&binfo, 0, sizeof(binfo));
	strncpy(binfo.title, genbuf, STRLEN);
	binfo.level = PERM_SYSOP;
	substitute_record(BOARDS, &binfo, sizeof(binfo), bid);

	move(4, 0);
	prints("\n���������Ѿ�ɾ��...\n");
	pressreturn();
	numboards = -1;
	clear();
	return 0;
}

int redhackercheck(char *s)
{

     return 0;
     if(!HAS_PERM(PERM_LOOKADMIN)) return 0;
     prints("�����ð�!���ȸ�л���������Ͷ�.�������ǲ�����˼,��û�� %s ��Ȩ��.\n���ȷʵ��Ҫ,���ڱ�վ��ع�����Ա��ϵ.\n",s);
     pressanykey();
     return 1; 

} 
extern int del_authrow_has_userid(char *file, char *word);
extern char GetAuthUserMailF(char *userid);
int d_user(char* cid)
{
	int     id,num,i;
	char    secu[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	if(redhackercheck("ɾ���ʺ�"))
           return ;

 	stand_title("ɾ��ʹ�����ʺ�");
	move(2,0);
	if(!gettheuserid(1,"��������ɾ����ʹ���ߴ���: ",&id))
		return 0;
	if (!strcmp(lookupuser.userid, "SYSOP"))
	{
		prints("\n�Բ����㲻����ɾ�� SYSOP �ʺ�!!\n");
		pressreturn();
		clear();
		return 0;
	}
	if ((lookupuser.userlevel&PERM_SYSOP)||(lookupuser.userlevel&PERM_ACCOUNTS)||(lookupuser.userlevel&PERM_OBOARDS)||(lookupuser.userlevel&PERM_CHATCLOAK))
	{
		prints("\n�Բ������������վ��Ȩ��!!\n");
		pressreturn();
		clear();
		return 0;
	}
	if (!strcmp(lookupuser.userid, currentuser.userid))
	{
		prints("\n�Բ����㲻����ɾ���Լ�������ʺ�!!\n");
		pressreturn();
		clear();
		return 0;
	}
	prints("\n\n������ [%s] �Ĳ�������:\n",lookupuser.userid);
	prints("    User ID:  [%s]\n",lookupuser.userid);
	prints("    ��   ��:  [%s]\n",lookupuser.username);
	prints("    ��   ��:  [%s]\n",lookupuser.realname);
	strcpy(secu, "bTCPRD#@XWBA#VS-DOM-F0s2345678");
	for (num = 0; num < strlen(secu); num++)
		if (!(lookupuser.userlevel & (1 << num)))
			secu[num] = '-';
	prints("    Ȩ   ��: %s\n\n", secu);

	num = getbnames(lookupuser.userid,secu,&num);
	if(num)
	{
		prints("[%s] Ŀǰ�е����� %d ����İ���: ",
		       lookupuser.userid, num);
		for(i = 0 ; i< num ; i ++ )
			prints("%s ",bnames[i]);
		prints("\n����ʹ�ð���жְ����ȡ�������ְ�������ò���.");
		pressanykey();
		clear();
		return 0;
	}

	sprintf(genbuf,"��ȷ��Ҫɾ�� [%s] ��� ID ��",lookupuser.userid);
	if (askyn(genbuf, NA, NA) == NA)
	{
		prints("\nȡ��ɾ��ʹ����...\n");
		pressreturn();
		clear();
		return 0;
	}
	sprintf(genbuf, "Ҫ�Ѹ� [%s] ���ID���벻��ע��30����", lookupuser.userid);
	if (askyn(genbuf, NA, NA) == YEA)
	{
		int fileid,i;
		size_t length;
		char userid_upper[IDLEN + 2];
		length = strlen(lookupuser.userid);
		for(i = 0;i < length; i ++)
			userid_upper[i] = lookupuser.userid[i];
		userid_upper[i] = '\0';
		sprintf(genbuf, "%s/recent_killed_id/%s", BBSHOME, userid_upper);
		if ((fileid = creat(genbuf, S_IRUSR)) == -1)
			prints("���벻��ע��id�������ֹ����룡");
		close(fileid);
	}
	sprintf(secu, "ɾ��ʹ���ߣ�%s", lookupuser.userid);

	//delete mail auth info,by brew  05.07.06
	sprintf(genbuf,"mail/%c/email.auth",toupper(GetAuthUserMailF(lookupuser.userid)));
	del_authrow_has_userid(genbuf,lookupuser.userid);
	
        strcat(genbuf,".U");
        del_authrow_has_userid(genbuf,lookupuser.userid);
	/*
	sprintf(genbuf, "mail/%c/%s", toupper(lookupuser.userid[0]), lookupuser.userid);
	f_rm(genbuf);
	sprintf(genbuf, "home/%c/%s", toupper(lookupuser.userid[0]), lookupuser.userid);
	f_rm(genbuf);
	killPersonalCorpus(lookupuser.userid);//lovnei
	delqmdpic(lookupuser.userid);*/
	lookupuser.userlevel = 0;
	strcpy(lookupuser.address, "");
#ifdef ALLOW_MEDALS

	lookupuser.nummedals = 0;
#endif

#ifdef ALLOWGAME

	lookupuser.money = 0;
	lookupuser.nummedals = 0;
	lookupuser.bet = 0;
#endif

	strcpy(lookupuser.username, "");
	strcpy(lookupuser.realname, "");
	strcpy(lookupuser.termtype, "");
	prints("\n%s �Ѿ��������...\n", lookupuser.userid);
	lookupuser.userid[0] = '\0';
	securityreport(secu);
	substitute_record(PASSFILE, &lookupuser, sizeof(lookupuser), id);
	setuserid(id, lookupuser.userid);
	pressreturn();
	clear();
	return 1;
}

int x_level()
{
	int     id;
	unsigned int newlevel;
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(0, 0);
	prints("����ʹ����Ȩ��\n");
	clrtoeol();
	move(1, 0);
	usercomplete("���������ĵ�ʹ�����ʺ�: ", genbuf);
	if (genbuf[0] == '\0')
	{
		clear();
		return 0;
	}
	if (!(id = getuser(genbuf)))
	{
		move(3, 0);
		prints("Invalid User Id");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	prints("�趨ʹ���� '%s' ��Ȩ�� \n", genbuf);
	newlevel = setperms(lookupuser.userlevel, "Ȩ��", NUMPERMS, showperminfo);
	move(2, 0);
	if (newlevel == lookupuser.userlevel)
		prints("ʹ���� '%s' Ȩ��û�б��\n", lookupuser.userid);
	else
	{

		//		lookupuser.userlevel = newlevel;
		{
			char    secu[STRLEN];
			sprintf(secu, "�޸� %s ��Ȩ��", lookupuser.userid);
			securityreport1(secu);
			lookupuser.userlevel = newlevel;
			securityreport1(secu);
		}

		substitute_record(PASSFILE, &lookupuser, sizeof(struct userec), id);
		if(!(lookupuser.userlevel & PERM_LOGINOK))
		{
			char	src[STRLEN], dst[STRLEN];
			sethomefile(dst, lookupuser.userid, "register.old");
			if(dashf(dst))
				unlink(dst);
			sethomefile(src, lookupuser.userid, "register");
			if(dashf(src))
				rename(src, dst);
		}
		 //���˿·
    	sethomefile(genbuf, lookupuser.userid, ".announcepath");
		if(dashf(genbuf))
			unlink(genbuf);
		prints("ʹ���� '%s' Ȩ���Ѿ��������.\n", lookupuser.userid);
	}
	pressreturn();
	clear();
	return 0;
}


int x_denylevel()
{
	int     id;
	char	ans[7];
	int denycase=0;//loveni;
	char fname[STRLEN];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(0, 0);
	prints("����ʹ���߻���Ȩ�޻�����\n");
	clrtoeol();
	move(1, 0);
	usercomplete("���������ĵ�ʹ�����ʺ�: ", genbuf);
	if (genbuf[0] == '\0')
	{
		clear();
		return 0;
	}
	if (!(id = getuser(genbuf)))
	{
		move(3, 0);
		prints("Invalid User Id");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "�޸� %s �Ļ���Ȩ�޻�����", lookupuser.userid);
		securityreport1(secu);
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	prints("�趨ʹ���� '%s' �Ļ���Ȩ�޻����� \n\n", lookupuser.userid);
	prints("(1) �����������Ȩ��       (A) �ָ���������Ȩ��\n");
	prints("(2) ȡ��������վȨ��       (B) �ָ�������վȨ��\n");
	prints("(3) ��ֹ����������         (C) �ָ�����������Ȩ��\n");
	prints("(4) ��ֹ������������       (D) �ָ�������������Ȩ��\n");
	prints("(5) ��ֹ�����˷���Ϣ       (E) �ָ������˷���ϢȨ��\n");
	prints("(6) �޸� %s ��������\n", lookupuser.userid);
	getdata(12, 0, "��������Ĵ���: ", ans, 3, DOECHO, YEA);
	//tdhlshx modified.
	int flag=1;
	char day1[7];
	int day=0;
	char buf[30],str[24];
	while(1)
	{
		getdata(13,0,"������˵��:",buf,29,DOECHO,YEA);
		if(killwordsp(buf)!=0)
			break;
	}
	switch(ans[0])
	{
	case '1':
		denycase=1;//loveni
		lookupuser.userlevel &= ~PERM_POST;
		getdata(14,0,"������ʱ��(����Ϊ��λ): ",day1,6,DOECHO,YEA);
		strcpy(str,"�������������Ȩ��\0");
		break;
	case 'a':
	case 'A':
		denycase=1;//loveni
		lookupuser.userlevel |= PERM_POST;
		flag=2;
		strcpy(str,"���ָ���������Ȩ��\0");
		break;
	case '2':
		denycase=2;//loveni
		lookupuser.userlevel &= ~PERM_BASIC;
		getdata(14,0,"������ʱ��(����Ϊ��λ): ",day1,6,DOECHO,YEA);
		strcpy(str,"��ȡ��������վȨ��\0");
		break;
	case 'b':
	case 'B':
		denycase=2;//loveni
		lookupuser.userlevel |= PERM_BASIC;
		flag=2;
		strcpy(str,"���ָ�������վȨ��\0");
		break;
	case '3':
		denycase=3;//loveni
		lookupuser.userlevel &= ~PERM_CHAT;
		getdata(14,0,"������ʱ��: ",day1,6,DOECHO,YEA);
		strcpy(str,"����ֹ����������\0");
		break;
	case 'c':
	case 'C':
		denycase=3;//loveni
		lookupuser.userlevel |= PERM_CHAT;
		flag=2;
		strcpy(str,"���ָ�����������Ȩ��\0");
		break;
	case '4':
		denycase=4;//loveni
		lookupuser.userlevel &= ~PERM_PAGE;
		getdata(14,0,"������ʱ��: ",day1,6,DOECHO,YEA);
		strcpy(str,"����ֹ������������\0");
		break;
	case 'd':
	case 'D':
		denycase=4;//loveni
		lookupuser.userlevel |= PERM_PAGE;
		flag=2;
		strcpy(str,"���ָ�������������Ȩ��\0");
		break;
	case '5':
		denycase=5;//loveni
		lookupuser.userlevel &= ~PERM_MESSAGE;
		getdata(14,0,"������ʱ��: ",day1,6,DOECHO,YEA);
		strcpy(str,"����ֹ�����˷���Ϣ\0");
		break;
	case 'e':
	case 'E':
		denycase=5;//loveni
		lookupuser.userlevel |= PERM_MESSAGE;
		flag=2;
		strcpy(str,"���ָ������˷���ϢȨ��\0");
		break;
	case '6':
		flag=0;
		uinfo_change1(12,&lookupuser,&lookupuser);
		break;
	default:
		flag=0;
		prints("\n ʹ���� '%s' ����Ȩ��������û�б��\n",
		       lookupuser.userid);
		pressreturn();
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "�޸� %s �Ļ���Ȩ�޻�����", lookupuser.userid);
		securityreport1(secu);
	}
	char uident[14];
	strncpy(uident,lookupuser.userid,sizeof(lookupuser.userid));
	if(flag==1)
	{
		day=atoi(day1);
		if(day<=0)
			return 0;
		char msgbuf[STRLEN];
		char repbuf[STRLEN];
		
		/*	move(18,0);
			prints("\n %s",str);
			pressanykey();*/
		sprintf(repbuf,"%s %s",uident,str);
		sprintf(msgbuf,
		        "\n  %s ����: \n\n"
		        "\t�ܱ�Ǹ�ĸ����������Ѿ�%s \n\n"
		        "\t����ȡ��Ȩ����ԭ����: %s \n\n"
		        "\t��Ϊ������Ϊ������ֹͣ����Ȩ�� %d �죬\n\n"
		        "\t%d ��󣬵�����¼ʱ��ϵͳ��Ϊ���Զ���⡣\n\n",
		        uident,str,buf,day,day,currentuser.userid);
		autoreport(repbuf,msgbuf,0,lookupuser.userid);
		FILE * fp;
		if(denycase==1) sprintf(fname, "home/%c/%s/permpost", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==2) sprintf(fname, "home/%c/%s/permbasic", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==3) sprintf(fname, "home/%c/%s/permchat", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==4) sprintf(fname, "home/%c/%s/permpage", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==5) sprintf(fname, "home/%c/%s/permmessage", toupper(lookupuser.userid[0]), lookupuser.userid);
		if(denycase>0)
			if ((fp = fopen(fname, "w")) != NULL)
			{
				fprintf(fp,"%d\n",day);
				if(denycase==2)	fprintf(fp,"%s\n",buf);
				fclose(fp);
			}//loveni
		
	}
	else if(flag==2)
	{
		char msgbuf[STRLEN];
		char repbuf[STRLEN];
		sprintf(repbuf,"%s %s",uident,str);
		sprintf(msgbuf,
		        "\n  %s ����: \n\n"
		        "\t���Ѿ�%s, \n\n"
		        "\t ����:%s \n\n"
		        "\t��л����֧��.\n\n"
		        "\t\t\t\t\t\t\t By %s   \n\n",
		        uident,str,buf,currentuser.userid);
		autoreport(repbuf,msgbuf,0,lookupuser.userid);
		if(denycase==1) sprintf(fname, "home/%c/%s/permpost", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==2) sprintf(fname, "home/%c/%s/permbasic", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==3) sprintf(fname, "home/%c/%s/permchat", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==4) sprintf(fname, "home/%c/%s/permpage", toupper(lookupuser.userid[0]), lookupuser.userid);
		else if(denycase==5) sprintf(fname, "home/%c/%s/permmessage", toupper(lookupuser.userid[0]), lookupuser.userid);
		if(denycase>0) unlink(fname);
	}//add over

	substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
	prints("\n ʹ���� '%s' ����Ȩ�޻������Ѿ��������.\n", lookupuser.userid);
	pressreturn();
	clear();
	return 0;
}

void a_edits()
{
	int     aborted;
	char    ans[7], buf[STRLEN], buf2[STRLEN];
	int     ch, num, confirm;
	extern struct BCACHE *brdshm;
	/*static char *e_file[] =
	    {"../Welcome", "../Welcome2", "issue", "announce","logout", "../vote/notes",
	     "../.badname", "../.bad_email", "../.bad_host",
	     "junkboards", "sysmsg","menu1.ini", "lock","menu.ini", "autopost", "sysops", "expire.ctl", "whatdate", "../NOLOGIN", "../NOREGISTER", "../.blockmail","s_fill","fornewcomers", "f_fill", "forbm","register", "firstlogin","chatstation","bbsnet.ini","bbsnet1.ini","notbackupboards","wwwannounce","../ftp/FTPFileList","patternpost","fav",NULL};
	static char *explain_file[] =
	    {"�����վ������", "��վ����", "��վ��ӭ��", "��վ��Ҫ����","��վ����"
	     ,"���ñ���¼", "����ע��� ID", "����ȷ��֮E-Mail", "������վ֮λַ"
	     ,"����POST���İ�", "ϵͳ�ײ�������Ϣ", "menu1.ini", "өĻ��������","menu.ini", "ÿ���Զ����ŵ�", "����������", "��ʱ�����趨��",
	     "�������嵥", "��ͣ��½(NOLOGIN)", "��ͣע��(NOREGISTER)", "ת�ź�����", "ע��ɹ��ż�","�����ֵ���","ע��ʧ���ż�","���������Լ���������ֲ�","���û�ע�᷶��","�û���һ�ε�½����","���ʻ������嵥","BBSNET תվ�嵥","BBSNET תվ�嵥II","����ɾ�����豸��֮�嵥","WWW��ʽ��վ����","FTP����վ���б�","ǿ��ģ�淢�İ���","Ĭ���ղؼа���",NULL};*/
static char *e_file[] =
	    {"../Welcome", "../Welcome2", "issue", "announce","logout", 
	"../vote/notes","sysmsg",    "lock","wwwannounce", "campusannounce" ,
	"../.badname", "../.bad_email", "../.bad_host","../../www/html/news/hotboards",
	"../../www/html/news/boards" ,"fav","posts/topblackb",
	"posts/topblackt","posts/ext","bless/ext","posts/day","bless/day",
	"campus","bbsannounce","deny_s_board","../ip.allow","sysops",
	"menu1.ini", "menu.ini","../NOLOGIN", "../NOREGISTER",
	"../NOPOST",  "../../www/html/news/noupload.html", "double_stay",NULL};


/*
 "autopost",  "expire.ctl", "whatdate",  "../.blockmail","s_fill","fornewcomers", "f_fill", "forbm","register", "firstlogin","chatstation","bbsnet.ini","bbsnet1.ini","../ftp/FTPFileList",NULL};*/
	static char *explain_file[] =
	    {"�����վ������", "��վ����", "��վ��ӭ��", "��վ��Ҫ����",
	"��վ����" ,"���ñ���¼", "ϵͳ�ײ�������Ϣ", "өĻ��������",
	"WWW��ʽվ���Ƽ�","WWW��ʽУ���Ƽ�","����ע��� ID","����ȷ��֮E-Mail", 
	"������վ֮λַ" ,"����������", "�Ƽ�������",
	"Ĭ���ղؼа���","ʮ����������","ʮ����������",
	"ָ��ʮ������","ָ��ʮ��ף������","�༭ʮ��","�༭ʮ��ף��","У�ڹ���",
	"վ�ڹ���","SȨ�޲��ɽ������","������ַ�趨","����������","menu1.ini", 
	"menu.ini","��ͣ��½(NOLOGIN)", "��ͣע��(NOREGISTER)",
	"ȫվֻ��(NOPOST)", "��ͣ�����ϴ�", "��վ˫������",NULL};

/*

"menu1.ini", "menu.ini", "ÿ���Զ����ŵ�",  "��ʱ�����趨��",
	     "�������嵥", "��ͣ��½(NOLOGIN)", "��ͣע��(NOREGISTER)", "ת�ź�����", "ע��ɹ��ż�","�����ֵ���","ע��ʧ���ż�","���������Լ���������ֲ�","���û�ע�᷶��","�û���һ�ε�½����","���ʻ������嵥","BBSNET תվ�嵥","BBSNET תվ�嵥II","FTP����վ���б�",NULL};*/
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(1, 0);
	prints("����ϵͳ����\n\n");
	for (num = 0;  e_file[num] != NULL && explain_file[num] != NULL ; num++){
              
  		prints("[[1;32m%2d[m] %s", num + 1, explain_file[num]);
		if(num < 17 )
			move(4+num,0);
		else
			move(num - 14, 50);
	}
	prints("[[1;32m%2d[m] �������\n", num + 1);

	getdata(23, 0, "��Ҫ������һ��ϵͳ����: ", ans, 3, DOECHO, YEA);
	ch = atoi(ans);
	if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
		return;
	ch -= 1;
        

        if((ch>=0&&ch<8)&&!HAS_PERM(PERM_ACBOARD)&&!HAS_PERM(PERM_SYSOP))
	{
             clear();
	     prints("��û��Ȩ���޸ı����,��ȷ������SYSOP��ACBOARDȨ��");
             pressanykey();
	     return;
	}
	if((ch==8||ch==9||ch==27)&&!HAS_PERM(PERM_WELCOME)&&!HAS_PERM(PERM_SYSOP))
	{
             clear();
	     prints("��û��Ȩ���޸ı����,��ȷ������SYSOP��WELCOMEȨ��");
             pressanykey();
	     return;
	}
	

        if((ch==10||ch==11||ch==12|| ch == 33)&&!HAS_PERM(PERM_ACCOUNTS)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("��û��Ȩ���޸ı����,��ȷ������SYSOP��ACCOUNTSȨ��");
             pressanykey();
             return;

        }
	if((ch==13||ch==14)&&!HAS_PERM(PERM_OBOARDS)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("��û��Ȩ���޸ı����,��ȷ������SYSOP��OBOARDSȨ��");
             pressanykey();
             return;

    }
	if((ch>23&&ch<32&&ch!=27)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("��û��Ȩ���޸ı����,��ȷ������SYSOPȨ��");
             pressanykey();
             return;

    }
	if((ch==32)&&!HAS_PERM(PERM_ANNOUNCE)&&!HAS_PERM(PERM_SYSOP))
	{
		clear();
		prints("��û��Ȩ���޸ı����,��ȷ������SYSOP��ANNOUNCEȨ��");
		pressanykey();
		return;
	}

	sprintf(buf2, "etc/%s", e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)�༭ (D)ɾ�� %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"��ȷ��Ҫɾ�� %s ���ϵͳ��",explain_file[ch]);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("ȡ��ɾ���ж�\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "ɾ��ϵͳ������%s", explain_file[ch]);
			securityreport(secu);
		}
		unlink(buf2);
		move(5, 0);
		prints("%s ��ɾ��\n", explain_file[ch]);
		pressreturn();
		clear();
		return;
	}
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf2, NA, YEA); /* ������ļ�ͷ, �����޸�ͷ����Ϣ */
	clear();
	if (aborted != -1)
	{
		prints("%s ���¹�", explain_file[ch]);
		{
			char    secu[STRLEN];
			sprintf(secu, "�޸�ϵͳ������%s", explain_file[ch]);
			securityreport(secu);
		}

		if (!strcmp(e_file[ch], "../Welcome"))
		{
			unlink("Welcome.rec");
			prints("\nWelcome ��¼������");
		}
		else if(!strcmp(e_file[ch],"whatdate"))
		{
			brdshm->fresh_date = time(0);
			prints("\n�������嵥 ����");
		}
	}
	pressreturn();
}

void help_edits()
{
	int     aborted;
	char    ans[7], buf[STRLEN], buf2[STRLEN];
	int     ch, num, confirm;
	//extern struct BCACHE *brdshm;
	/*static char *e_file[] =
	    {"help/announcereadhelp", "help/boardreadhelp", "help/chathelp", "help/chatophelp",
	     "help/edithelp", "help/friendshelp", "help/mailerror-explain", "help/mailreadhelp",
	     "help/mainreadhelp", "help/morehelp", "help/msghelp", "help/rejectshelp",
	     "help/serlisthelp", "help/usershelp", "help/favdigesthelp","etc/birthday","vipuser","ip.allow","etc/posts/topblackb","etc/posts/topblackt","etc/posts/day","etc/campus","etc/bbsannounce", NULL};
	static char *explain_file[] =
	    {"������ʹ��˵��", "������ʹ��˵��", "������ʹ��˵��", "�����ҹ���Աָ��",
	     "�༭����ָ��˵��", "����ѡ�����ܼ�", "�ż�����ʧ����Ϣ", "�ʼ�ѡ��ʹ��˵��",
	     "�๦���Ķ�ѡ��˵��", "�Ķ����¹��ܼ�", "ѶϢ�����ʹ��˵��", "����ѡ�����ܼ�",
	     "ʹ��������ѡ��ʹ��˵��", "ע��ʹ����ѡ��˵��", "�������ղ�ѡ��ʹ��˵��", "���պ���","���������û�","������ַ�趨","ʮ����������","ʮ����������","�༭ʮ��","У�ڹ���","վ�ڹ���",NULL};*/
	static char *e_file[] =
	    {"help/announcereadhelp", "help/boardreadhelp", "help/chathelp", "help/chatophelp",
	     "help/edithelp", "help/friendshelp", "help/mailerror-explain", "help/mailreadhelp",
	     "help/mainreadhelp", "help/morehelp", "help/msghelp", "help/rejectshelp",
	     "help/serlisthelp", "help/usershelp", "help/favdigesthelp","etc/s_fill","etc/fornewcomers", "etc/f_fill", "etc/forbm","etc/register", "etc/firstlogin","ftp/FTPFileList","etc/chatstation","etc/bbsnet.ini","etc/bbsnet1.ini",".blockmail","etc/whatdate","etc/birthday","etc/expire.ctl", "etc/autopost","etc/BMCLOAK","etc/WarningBM","etc/specbm",NULL};

	static char *explain_file[] =
	    {"������ʹ��˵��", "������ʹ��˵��", "������ʹ��˵��", "�����ҹ���Աָ��",
	     "�༭����ָ��˵��", "����ѡ�����ܼ�", "�ż�����ʧ����Ϣ", "�ʼ�ѡ��ʹ��˵��",
	     "�๦���Ķ�ѡ��˵��", "�Ķ����¹��ܼ�", "ѶϢ�����ʹ��˵��", "����ѡ�����ܼ�",
	     "ʹ��������ѡ��ʹ��˵��", "ע��ʹ����ѡ��˵��", "�������ղ�ѡ��ʹ��˵��", "ע��ɹ��ż�","�����ֵ���","ע��ʧ���ż�","���������Լ���������ֲ�","���û�ע�᷶��","�û���һ�ε�½����","FTP����վ���б�","���ʻ������嵥","BBSNET תվ�嵥","BBSNET תվ�嵥II", "ת�ź�����","�������嵥","���պ���","��ʱ�����趨��","ÿ���Զ����ŵ�","����������վ����","ȡ����������Ȩ��ǰ����","������������վ����֮����",NULL};
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(1, 0);
	prints("����ϵͳ����2\n\n");
	/* Efan: �˴���bug����Ϊ�ǰ�����Ϣ�����Բ�����"menu.ini"����
		for (num = 0; HAS_PERM(PERM_SYSOP) ? e_file[num] != NULL && explain_file[num] != NULL : explain_file[num] != "menu.ini"; num++) {
	*/
	for (num =0; e_file [num] != NULL && explain_file [num] != NULL; ++num)
	{
		prints("[[1;32m%2d[m] %s", num + 1, explain_file[num]);
		if(num < 17 )
			move(4+num,0);
		else
			move(num - 14, 50);
	}
	prints("[[1;32m%2d[m] �������\n", num + 1);

	getdata(23, 0, "��Ҫ������һ��ϵͳ����: ", ans, 3, DOECHO, YEA);
	ch = atoi(ans);
	
	if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
		return;
	ch -= 1;
	if((ch==30||ch==31||ch==32)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("��û��Ȩ���޸ı����,��ȷ������SYSOPȨ��");
             pressanykey();
             return;

        }
	sprintf(buf2, "%s", e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)�༭ (D)ɾ�� %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"��ȷ��Ҫɾ�� %s ",explain_file[ch]);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("ȡ��ɾ���ж�\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "ɾ��ϵͳ����2��%s", explain_file[ch]);
			securityreport(secu);
		}
		unlink(buf2);
		move(5, 0);
		prints("%s ��ɾ��\n", explain_file[ch]);
		pressreturn();
		clear();
		return;
	}
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf2, NA, YEA); /* ������ļ�ͷ, �����޸�ͷ����Ϣ */
	clear();
	if (aborted != -1)
	{
		prints("%s ���¹�", explain_file[ch]);
		{
			char    secu[STRLEN];
			sprintf(secu, "����ϵͳ����2��%s", explain_file[ch]);
			securityreport(secu);
		}
	}
	pressreturn();
}


int wall()
{
	if (!HAS_PERM(PERM_SYSOP)&&!HAS_PERM(PERM_OBOARDS))
		return 0;
	modify_user_mode(MSG);
	move(2, 0);
	clrtobot();
	if (!get_msg("����ʹ����", buf2, 1))
	{
		return 0;
	}

        securityreport(buf2);

	if (apply_ulist(dowall) == -1)
	{
		move(2, 0);
		prints("���Ͽ���һ��\n");
		pressanykey();
	}
	prints("\n�Ѿ��㲥���...\n");
	pressanykey();
	return 1;
}

int setsystempasswd()
{
	FILE   *pass;
	char    passbuf[20], prepass[20];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;
	if (strcmp(currentuser.userid, "SYSOP"))
	{
		clear();
		move(10,20);
		prints("�Բ���ϵͳ����ֻ���� SYSOP �޸ģ�");
		pressanykey();
		return;
	}
	getdata(2, 0, "�������µ�ϵͳ����(ֱ�ӻس���ȡ��ϵͳ����): ",
	        passbuf, 19, NOECHO, YEA);
	if(passbuf[0] == '\0')
	{
		if( askyn("��ȷ��Ҫȡ��ϵͳ������?",NA,NA)== YEA )
		{
			unlink("etc/.syspasswd");
			securityreport("[32mȡ��ϵͳ����[37m");
		}
		return ;
	}
	getdata(3, 0, "ȷ���µ�ϵͳ����: ", prepass, 19, NOECHO, YEA);
	if (strcmp(passbuf, prepass))
	{
		move(4,0);
		prints("�������벻��ͬ, ȡ���˴��趨.");
		pressanykey();
		return;
	}
	if ((pass = fopen("etc/.syspasswd", "w")) == NULL)
	{
		move(4, 0);
		prints("ϵͳ�����޷��趨....");
		pressanykey();
		return;
	}
	fprintf(pass, "%s\n", genpasswd(passbuf));
	fclose(pass);
	move(4, 0);
	prints("ϵͳ�����趨���....");
	pressanykey();
	return;
}

int x_csh()
{
	char    buf[PASSLEN];
	int     save_pager;
	int     magic;

	if (!HAS_PERM(PERM_SYSOP))
	{
		return 0;
	}
	if (!check_systempasswd())
	{
		return;
	}
	modify_user_mode(SYSINFO);
	clear();
	getdata(1, 0, "������ͨ�а���: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
	{
		prints("\n\n���Ų���ȷ, ����ִ�С�\n");
		pressreturn();
		clear();
		return;
	}
	randomize();
	magic = rand() % 1000;
	prints("\nMagic Key: %d", magic * 5 - 2);
	getdata(4, 0, "Your Key : ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !(atoi(buf) == magic))
	{
		securityreport("Fail to shell out");
		prints("\n\nKey ����ȷ, ����ִ�С�\n");
		pressreturn();
		clear();
		return;
	}
	securityreport("Shell out");
	modify_user_mode(SYSINFO);
	clear();
	refresh();
	reset_tty();
	save_pager = uinfo.pager;
	uinfo.pager = 0;
	update_utmp();
	do_exec("csh", NULL);
	restore_tty();
	uinfo.pager = save_pager;
	update_utmp();
	clear();
	return 0;
}

int setreadonly()
{
	clear();
	if(!dashf("NOPOST"))
	{
		if( askyn("��ȷ��ʹϵͳ����ֻ��״̬��?",NA,NA)== YEA )
		{
			system("touch NOPOST");
			prints("ϵͳ����ֻ��״̬����");
		}
	}
	else
	{
		if( askyn("��ȷ��ʹϵͳ�˳�ֻ��״̬��?",NA,NA)== YEA )
		{
			system("mv NOPOST NOPOST.no");
			prints("ϵͳ�˳�ֻ��״̬����");
		}
	}
	pressanykey();
	return 1;
}

/* �����ֱ༭ */

int keywords()
{
	char buf[256];
	int aborted;
	strcpy(buf, "etc/.badwords");
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf, NA, YEA);// ������ļ�ͷ, �����޸�ͷ����Ϣ
	if(aborted!=-1)
		securityreport("�����ֱ༭");
	return 1;
}
/*loveni:ˢ��ʮ��*/
int renewtopten()
{
	clear();
	if( askyn("��ȷ������ˢ��ʮ��?",NA,NA)== YEA )
	{
		system("/home/bbs/bin/poststat /home/bbs");
		prints("ʮ���Ѿ�ˢ��!");
	}
	pressanykey();
	return 1;
}

int renewweb()
{
	clear();
	if( askyn("��ȷ��Ҫ����ˢ��Web��ҳ?",NA,NA)==YEA)
	{
		system("/home/www/cgi-bin/bbs/bbsmainbuf > /dev/null");
		prints("Web��ҳ�Ѿ�ˢ��!");
	}
	pressanykey();
	return 1;
}

#endif
#endif

int boardrule()
{
	int     pos,aborted;
	struct boardheader fh;
	char	bname[256];
	char buf[256];
	char fname[256];
	char ans[4];
	int confirm;
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return;
	clear();
	stand_title("���ð��\n");
	clrtoeol();
	if(!gettheboardname(1,"������Ҫ���ð�������������: ",&pos,&fh,bname))
		return -1;
	sprintf(fname,"boards/%s/boardrule",fh.filename);
	sprintf(buf, "(E)�༭ (D)ɾ�� %s����? [E]: ", fh.filename);
	getdata(2, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"��ȷ��Ҫɾ�� %s����?",fh.filename);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("ȡ��ɾ���ж�\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "ɾ��%s����", fh.filename);
			securityreport(secu);
		}
		unlink(fname);
		move(5, 0);
		prints("%s������ɾ��\n", fh.filename);
		pressreturn();
		clear();
		return;
	}
	aborted = vedit(fname, NA, YEA);
	if(aborted!=-1)
	{
		sprintf(buf,"�༭%s��İ��",fh.filename);
		securityreport(buf);
	}
	return 1;
}
	
	

int find_auth_mail()
{
	char username[15];
	char email[80];
	char ip[80];
	char userid[80];
	int len=180;
	char fname[80];
	char genbuf[80];
	char *fileinfo=NULL;
	int find=0;
	int i;
	char findid[15];
	FILE * fp;
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return -1;
	}
	clear();
	stand_title("��ѯ�û���֤����:");
	getdata (2, 0, "�������û�id: ", username, 13, DOECHO, YEA);
	if(username[0]==0)
	{
		pressanykey();
		return -1;
	}
	strtolower(username,username);
	for (i='A';i<='Z'&&(find==0);i++)
	{
		sprintf(fname,"/home/bbs/mail/%c/email.auth",i);
		if(dashf(fname))
		{	
			fp=fopen(fname,"r");
			while (getline(&fileinfo, &len, fp)!= -1 &&find==0) 
			{
				strcpy(email,strtok(fileinfo,","));
				strcpy(userid,strtok(NULL,","));
				strcpy(ip,strtok(NULL,","));
				strtolower(findid,userid);
				if(!strcmp(findid,username))
				{
					find=1;
					move(2,0);
					clrtobot ();
					sprintf(genbuf,"�û�id:%s\n",userid);
					prints(genbuf);
					sprintf(genbuf,"�û���֤Email:%s\n",email);
					prints(genbuf);
					sprintf(genbuf,"�û���֤ip:%s\n",ip);
					prints(genbuf);
				}
			}
		}
	}
	if(find==0)
	{
		move(2,0);
		clrtobot ();
		prints("û���ҵ����û���Ϣ");
	}
	if (fileinfo) free(fileinfo);			
					
                 		
	pressanykey();
	return 0;
}

int find_mail_user()
{
	char address[80];
	char userid[80];
	char email[80];
	int len=180;
	char fname[80];
	char genbuf[80];
	char ip[40];
	char *fileinfo=NULL;
	int find=0;
	FILE * fp;
	char findemail[80];
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return -1;
	}
	clear();
	stand_title("��ѯ������֤���û�:");
	getdata (2, 0, "����������:", address, 80, DOECHO, YEA);
	if(address[0]==0)
	{
		pressanykey();
		return -1;
	}
	sprintf(fname,"/home/bbs/mail/%c/email.auth",toupper(address[0]));
	strtolower(address,address);
	if(dashf(fname))
	{	
		fp=fopen(fname,"r");
		while (getline(&fileinfo, &len, fp)!= -1) 
		{
			strcpy(email,strtok(fileinfo,","));
			strcpy(userid,strtok(NULL,","));
			strcpy(ip,strtok(NULL,","));
			strtolower(findemail,email);
			if(!strcmp(findemail,address))
			{
				find++;
				if(find==1)
				{
					move(2,0);
					clrtobot ();
					sprintf(genbuf,"Email:%s\n",email);
					prints(genbuf);
				}
				sprintf(genbuf,"�û�id:%s\n",userid);
				prints(genbuf);
	 			sprintf(genbuf,"�û���֤ip:%s\n\n",ip);
				prints(genbuf);
			}
		}
	}
	if(find==0)
	{
		move(2,0);
		clrtobot ();
		prints("û���ҵ���������Ϣ");
	}
	if (fileinfo) free(fileinfo);			
					
                 		
	pressanykey();
	return 0;
}
int life666()
{
	int id;
	char fname[256];
	FILE *fp;
	modify_user_mode(ADMIN);
	stand_title("�����û���6�ʺ�");
	if (!gettheuserid(1, "������ʹ���ߴ���: ", &id))	return -1;
	
	
	sprintf(fname, "home/%c/%s/life666", toupper(lookupuser.userid[0]), lookupuser.userid);
	move(4, 0);
	clrtobot();
	
	
	if (!dashf(fname))
	{
		if (askyn("���û���δ��������6�ʺţ��Ƿ�����?", NA, NA) == YEA)
		{
			fp = fopen(fname, "w+");
			if (fp)
			{
				fprintf(fp, "%s", currentuser.userid);
				fclose(fp);
				char secu[STRLEN];
                     	        sprintf(secu, "���� %s ��6�ʺ�", lookupuser.userid);
                                security_report(secu,1);
				prints("�Ѿ�������û���6�ʺ�");
			}
			else
				prints("���������ļ�����");
		}
	}
	else
	{
		if (askyn("���û��Ѿ���������6�ʺţ��Ƿ�ȡ��?", NA, NA) == YEA)
		{
			unlink(fname);
           		char secu[STRLEN];
                        sprintf(secu, "�ջ� %s ��6�ʺ�", lookupuser.userid);
                        security_report(secu,1);

			prints("�Ѿ�ȡ�����û���6�ʺ�");
		}
	}
	pressreturn();
	return 0;
}
