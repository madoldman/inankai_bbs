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
					"ÀëÈÎÕ¾Îñ½±ÕÂ",
					"Ô­´´Ğ´ÊÖ½±ÕÂ",
					"ÓÅĞã°åÎñ½±ÕÂ",
					"ÈÈĞÄÍøÓÑ½±ÕÂ",
					"×¥³æÄÜÊÖ½±ÕÂ",
					"ÀëÈÎÖÙ²Ã½±ÕÂ",
					"ÌØÊâ¹±Ï×½±ÕÂ",
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
	stand_title ("½±Àø¹¦³¼!");
	if (!gettheuserid (1, "ÇëÊäÈëÊ¹ÓÃÕß´úºÅ(Ö±½Ó»Ø³µÈ¡Ïû²Ù×÷)£º", &id) )
		return -1;
	clrtobot ();
	memcpy (& uinfo, & lookupuser, sizeof (uinfo) );
	move (4, 0);
	prints ("ÓÃ»§ %s ÓµÓĞ½±ÕÂÇé¿ö£º\n", uinfo.userid);
	num = uinfo.nummedals;
	for (i = 7; i > 0; --i) {
		nummedal [7 - i] = (num >> (i << 2)) & 0x0f;
		prints ("%30s %d Ã¶\n", medaltype [7 - i], nummedal [7 - i]);
	}
	move (13, 0);
	prints ("     A.ÀëÈÎÕ¾Îñ  B.Ô­´´Ğ´ÊÖ  C.ÓÅĞã°åÎñ  D.ÈÈĞÄÍøÓÑ\n");
	prints ("     E.×¥³æÄÜÊÖ  F.ÀëÈÎÖÙ²Ã  G.ÌØÊâ¹±Ï×  H.³·Ïú½±ÕÂ\n");
	getdata (16, 0, "ÇëÊäÈë½±ÕÂÀàĞÍ: ", buf, 2, DOECHO, YEA);
	if (buf [0] == 0) {
		move (17, 0);
		prints ("²Ù×÷È¡Ïû");
		pressreturn ();
		continue;
	}
	type = toupper (buf [0]) - 'A';
	if (type < 0 || type > 7) {
		move (17, 0);
		prints ("·Ç·¨ÊäÈë");
		pressreturn ();
		continue;
	}
	if (type == 7) {
		sprintf (show, "³·Ïú \033[1;32m%s\033[m ÍøÓÑµÄËùÓĞ½±ÕÂ£¬È·¶¨Âğ£¿[y/N]", uinfo.userid);
		getdata (17, 0, show, buf, 10, DOECHO, YEA);
		if (buf [0] == 'y' || buf [0] == 'Y')
		{
			char bufsys [100];
			uinfo.nummedals = 0;
			if (uinfo.nummedals > 0x8000)
				uinfo.nummedals =0;
			substitute_record (PASSFILE, & uinfo, sizeof (uinfo), id);
			sprintf (bufsys, "³·Ïú %s ËùÓĞ½±ÕÂ", uinfo.userid);
			securityreport (bufsys);
			prints ("²Ù×÷³É¹¦£¡");
			pressreturn ();
			continue;
		}
		else {
			prints ("²Ù×÷È¡Ïû");
			pressreturn ();
			continue;
		}
	}
	move (17, 0);
	clrtobot ();
	move (17, 0);
	sprintf (genbuf, "ÊÚÓè½±ÕÂÀàĞÍ: \033[1;32m%s\033[m", medaltype [type]);
	prints (genbuf);
	getdata (18, 0, "ÊÚÓè¶àÉÙÃ¶Ñ«ÕÂ£¿", buf, 10, DOECHO, YEA);
	if (buf [0] != 0) {
		num = atoi (buf);
		if (num > 15) {
			prints ("´íÎó!!ÇëÊäÈë 0--15 µÄÊı!!");
			pressreturn ();
			continue;
		}
	}
	sprintf (show, "ÊÚÓè \033[1;32m%s\033[m ÍøÓÑ \033[1;32m%d\033[m Ã¶ \033[1;32m%s\033[m£¬È·¶¨Âğ£¿[y/N]", uinfo.userid, num, medaltype [type]);
	getdata (19, 0, show, buf, 10, DOECHO, YEA);
	if (buf [0] == 'y' || buf [0] == 'Y')
	{
		char bufsys [100];

		sprintf (bufsys, "ÊÚÓè %s %s %d Ã¶", uinfo.userid, medaltype [type], num);
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
		prints ("²Ù×÷³É¹¦£¡");
		pressreturn ();
		continue;
	}
	prints ("²Ù×÷È¡Ïû");
	pressreturn ();

}  // Efan: while loop
#endif

	return 0;
}
/* add by livebird ½â³ı¹éÒş */
int un_Reclusion()
{
        int id;
        modify_user_mode(ADMIN);
        if (!check_systempasswd())
        {
                return;
        }
        clear();
        prints("½â³ıÊ¹ÓÃÕß¹éÒş×´Ì¬\n");
        clrtoeol();
        move(1, 0);
        usercomplete("ÊäÈëÓû¸ü¸ÄµÄÊ¹ÓÃÕßÕÊºÅ: ", genbuf);
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
        if( askyn("ÄãÈ·¶¨Òª½â³ıÆä¹éÒş×´Ì¬Âğ?",NA,NA)== NA )
        {
                 prints("\nÊ¹ÓÃÕß '%s' Ã»ÓĞ±»½â³ı¹éÒş×´Ì¬¡£\n",lookupuser.userid);
                 pressreturn();
                 clear();
        }
        else
        {
                if((lookupuser.userlevel & PERM_RECLUSION ) == 0)
                {
                        prints("\nÊ¹ÓÃÕß '%s' Ã»ÓĞ¹éÒş¡£\n", lookupuser.userid);
                        pressreturn();
                        clear();
                }
                else
                {       char    secu[STRLEN];
                        sprintf(secu, "½â³ı %s µÄ¹éÒş×´Ì¬", lookupuser.userid);
                        //securityreport1(secu);

                        lookupuser.userlevel &= ~PERM_RECLUSION;

                        security_report(secu,1);
                        substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
                        prints("\nÊ¹ÓÃÕß '%s' ÒÑ¾­±»½â³ı¹éÒş×´Ì¬¡£\n", lookupuser.userid);
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
	stand_title("ĞŞ¸ÄÊ¹ÓÃÕß×ÊÁÏ");
	if(!gettheuserid(1,"ÇëÊäÈëÊ¹ÓÃÕß´úºÅ: ",&id))
		return -1;
	memcpy(&uinfo, &lookupuser, sizeof(uinfo));

	move(1, 0);
	clrtobot();
	disply_userinfo(&uinfo);
	uinfo_query(&uinfo, 1, id);
	return 0;
}

/* add by yiyo ÔÊĞíĞ£·½¹ÜÀíÕÊºÅ²é¿´ÓÃ»§×ÊÁÏ£¬µ«²»ÄÜĞŞ¸Ä */
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
        if(redhackercheck("²éÑ¯ÍøÓÑ×ÊÁÏ")) return;
	stand_title("²éÑ¯Ê¹ÓÃÕß×ÊÁÏ");
	if(!gettheuserid(1,"ÇëÊäÈëÊ¹ÓÃÕß´úºÅ: ",&id))
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
	stand_title("ÈÎÃü°åÖ÷\n");
	clrtoeol();
	if (!gettheuserid(2,"ÊäÈëÓûÈÎÃüµÄÊ¹ÓÃÕßÕÊºÅ: ",&id))
		return 0;
	if(!gettheboardname(3,"ÊäÈë¸ÃÊ¹ÓÃÕß½«¹ÜÀíµÄÌÖÂÛÇøÃû³Æ: ",&pos,&fh,bname))
		return -1;
	if (is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("Äã²»ÄÜÈÎÃü×Ó·ÖÇøÇø³¤");
                pressanykey();                 
		clear();
                return -1;
        }
	if( fh.BM[0] != '\0' )
	{
		if ( !strncmp(fh.BM,"SYSOPs",6) )
		{
			move(4, 0);
			if(askyn("¸ÃÌÖÂÛÇø°åÖ÷ÊÇ SYSOPs, ÄãÈ·¶¨¸Ã°åĞèÒª°åÖ÷",NA,NA)==NA)
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
				//	    prints("%s ÌÖÂÛÇøÒÑÓĞÈıÃû°åÖ÷",bname);
				prints("%s ÌÖÂÛÇøÒÑÓĞËÄÃû°åÖ÷",bname);
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
		prints("Äã²»ÄÜÈÎÃü guest µ±°åÖ÷");
		pressanykey();
		clear();
		return -1;
	}
        if(strlen(fh.BM)+strlen(lookupuser.userid)>BM_LEN){
                move(5,0);
		prints("ÓôÃÆ,Õâ¸ö¼Ò»ïµÄidÌ«³¤£¬ÏµÍ³ÒªÒç³öÁË¡£¡£¡£");
                pressanykey();
                return -1;
	}

	oldbm = getbnames(lookupuser.userid,bname,&find);
	if( find || (oldbm  == 4 && strcmp (lookupuser.userid, "SYSOP")))
	{
		move ( 5,0);
		prints(" %s ÒÑ¾­ÊÇ%s°åµÄ°åÖ÷ÁË",lookupuser.userid,find?"¸Ã":"ËÄ¸ö");
		pressanykey();
		clear();
		return -1;
	}
	prints("\nÄã½«ÈÎÃü %s Îª %s °å°å%s.\n",lookupuser.userid,bname,bm?"Ö÷":"¸±");
	if( askyn("ÄãÈ·¶¨ÒªÈÎÃüÂğ?",NA,NA)== NA )
	{
		prints("È¡ÏûÈÎÃü°åÖ÷");
		pressanykey();
		clear();
		return -1;
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(8,0);
	prints("ÇëÊäÈëÈÎÃü¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)");
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
		sprintf(secu, "°åÖ÷ÈÎÃü, ¸øÓè %s µÄ°åÖ÷È¨ÏŞ", lookupuser.userid);
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
	//   sprintf(bmfilename, "[¹«¸æ]ÈÎÃü %s Îª %s ÌÖÂÛÇø%s",
	sprintf(bmfilename, "[¹«¸æ]ÈÎÃü %s Îª %s °å%s",
	        lookupuser.userid,fh.filename,bm?"°åÖ÷":"°å¸±");
	securityreport(bmfilename);
	//move(16,0);
	//prints(bmfilename);

	/*  livebird 11.22.2004 */

	move(16,0);
	if( askyn("ÄãÒªÊ¹ÓÃ°åĞ£°åÎñÈÎÃü¹«¸æÂğ?",NA,NA)== NA )
		sprintf(genbuf,"\n\t\t\t¡¾ ¹«¸æ ¡¿\n\n"
	        	"\tÈÎÃü %s Îª %s °å%s£¡\n"
	        	"\tÇë %s ÔÚÈıÌìÄÚµ½ÏàÓ¦ÇøÎñ¹ÜÀí°å±¨µ½¡£\n"
	        	"\tÓâÆÚÎ´ÄÜ±¨µ½Õß£¬½«È¡Ïû°åÖ÷È¨ÏŞ¡£\n"
			"\tÊµÏ°ÆÚÒ»¸öÔÂ£¬Çë¹ã´óÍøÓÑ¼à¶½¡£\n",
	        	lookupuser.userid,bname,bm?"°åÖ÷":"°å¸±",lookupuser.userid);
	else
		sprintf(genbuf,"\n\t\t\t¡¾ ¹«¸æ ¡¿\n\n"
			"\tÈÎÃü %s Îª %s °å%s£¡\n"
			"\tÇë %s µ½°åĞ£ÅàÑµ£¬ÅàÑµÄÚÈİÔÚ¸Ã°åÖÃµ×¡£\n"
			"\tÅàÑµÆÚÎªÈıÌì£¬ÓâÆÚÈ¡ÏûÅàÑµ×Ê¸ñ¡£\n",
			lookupuser.userid,bname,bm?"°åÖ÷":"°å¸±",lookupuser.userid);
	/* end */
	move(18,0);
	prints(bmfilename);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\n\nÈÎÃü¸½ÑÔ£º\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(currboard,bname);
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	//tdhlshx
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"¹§Ï²Äú±»ÈÎÃüÎª%s°åµÄ°åÎñ!",bname);
	sprintf(tmpfile,"tmp/committbm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,´´½¨ÁÙÊ±ÎÄ¼ş´íÎó.ÇëÓëÏµÍ³Î¬»¤ÁªÏµ»òÔÙ´Î³¢ÊÔ!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);

	mail_file(tmpfile,lookupuser.userid,buftitle);
	prints("\n\nÈÎÃüÊé·¢³ö!");
	pressanykey();
	char bmfaqtitle[STRLEN];
	sprintf(bmfaqtitle,"°åÎñÊØÔòÒÔ¼°°åÎñ²Ù×÷ÊÖ²á");
	mail_file("/home/bbs/etc/forbm",lookupuser.userid,bmfaqtitle);
#ifdef ORDAINBM_POST_BOARDNAME

	strcpy(currboard,ORDAINBM_POST_BOARDNAME);
	autoreport(bmfilename,genbuf,YEA,NULL);
#endif

	pressanykey();
	unlink(tmpfile);
	return 0;
}
/*smalldogÈÎÃü×Ó·ÖÇøÇø³¤*/
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
	stand_title("ÈÎÃü×Ó·ÖÇøÇø³¤\n");
	clrtoeol();
	if (!gettheuserid(2,"ÊäÈëÓûÈÎÃüµÄÊ¹ÓÃÕßÕÊºÅ: ",&id))
		return 0;
	if(!gettheboardname(3,"ÊäÈë¸ÃÊ¹ÓÃÕß½«¹ÜÀíµÄ×Ó·ÖÇøÃû³Æ: ",&pos,&fh,bname))
		return -1;
	if (!is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("Äã²»ÄÜÔÚÈÎÃüÇø³¤µÄÊ±ºòÈ´ÈÎÃü°åÃæ°åÖ÷");
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
			prints("%s ×Ó·ÖÇøÒÑÓĞËÄÃûÇø³¤",bname);
			pressreturn();
			clear();
			return -1;
		}
		bm = 0;
	}
	if (!strcmp(lookupuser.userid,"guest"))
	{
		move ( 5,0);
		prints("Äã²»ÄÜÈÎÃü guest µ±×Ó·ÖÇøÇø³¤");
		pressanykey();
		clear();
		return -1;
	}
        if(strlen(fh.BM)+strlen(lookupuser.userid)>BM_LEN)
	{
                move(5,0);
		prints("ÓôÃÆ,Õâ¸ö¼Ò»ïµÄidÌ«³¤£¬ÏµÍ³ÒªÒç³öÁË¡£¡£¡£");
                pressanykey();
                return -1;
	}
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if( find || (oldbm  == 4 && strcmp (lookupuser.userid, "SYSOP")))
	{
		move ( 5,0);
		prints(" %s ÒÑ¾­ÊÇ%s°åµÄ°åÖ÷ÁË",lookupuser.userid,find?"¸Ã":"ËÄ¸ö");
		pressanykey();
		clear();
		return -1;
	}
	prints("\nÄã½«ÈÎÃü %s Îª %s ×Ó·ÖÇøÇø³¤.\n",lookupuser.userid,bname);
	if( askyn("ÄãÈ·¶¨ÒªÈÎÃüÂğ?",NA,NA)== NA )
	{
		prints("È¡ÏûÈÎÃü×Ó·ÖÇøÇø³¤");
		pressanykey();
		clear();
		return -1;
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(8,0);
	prints("ÇëÊäÈëÈÎÃü¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)");
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
		sprintf(secu, "×Ó·ÖÇøÇø³¤ÈÎÃü, ¸øÓè %s µÄÏàÓ¦È¨ÏŞ", lookupuser.userid);
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
	sprintf(bmfilename, "[¹«¸æ]ÈÎÃü %s Îª %s ×Ó·ÖÇøÇø³¤",lookupuser.userid,fh.filename);
	securityreport(bmfilename);
	sprintf(genbuf,"\n\t\t\t¡¾ ¹«¸æ ¡¿\n\n"
        	       "\tÈÎÃü %s Îª %s ×Ó·ÖÇøÇø³¤£¡\n"
        	       "\tÇë %s ÔÚÈıÌìÄÚµ½ÏàÓ¦ÇøÎñ¹ÜÀí°å±¨µ½¡£\n"
                       "\tÓâÆÚÎ´ÄÜ±¨µ½Õß£¬½«È¡Ïû°åÖ÷È¨ÏŞ\n"
		       "\tÊµÏ°ÆÚÒ»¸öÔÂ£¬Çë¹ã´óÍøÓÑ¼à¶½¡£\n",
                       lookupuser.userid,bname,lookupuser.userid);

	move(18,0);
	prints(bmfilename);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\n\nÈÎÃü¸½ÑÔ£º\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(currboard,bname);
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	//tdhlshx
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"¹§Ï²Äú±»ÈÎÃüÎª%s×Ó·ÖÇøµÄÇø³¤!",bname);
	sprintf(tmpfile,"tmp/committbm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,´´½¨ÁÙÊ±ÎÄ¼ş´íÎó.ÇëÓëÏµÍ³Î¬»¤ÁªÏµ»òÔÙ´Î³¢ÊÔ!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);

	mail_file(tmpfile,lookupuser.userid,buftitle);
	prints("\n\nÈÎÃüÊé·¢³ö!");
	pressanykey();
	char bmfaqtitle[STRLEN];
	sprintf(bmfaqtitle,"°åÎñÊØÔòÒÔ¼°°åÎñ²Ù×÷ÊÖ²á");
	mail_file("/home/bbs/etc/forbm",lookupuser.userid,bmfaqtitle);
#ifdef ORDAINBM_POST_BOARDNAME

	strcpy(currboard,ORDAINBM_POST_BOARDNAME);
	autoreport(bmfilename,genbuf,YEA,NULL);
#endif

	pressanykey();
	unlink(tmpfile);
	return 0;
}

/* thunder 2003.5.21 ·¢ËÍ¾ÜĞÅ
   ÇëÈ·±£ÄãÒª·¢ËÍµÄidĞ´¹ıÉêÇë*/

int refs_mail()
{
	int     id, pos,  i;
	struct boardheader fh;

	char nbuf[8], bname[STRLEN];
	strcpy(bname,"");
	char buf[5][STRLEN];
	char note[5][STRLEN]={"","°åÎñ","°åÃæ","¸öÈËÎÄ¼¯","ÓÀ¾ÃÕÊºÅ"};
	modify_user_mode(ADMIN);


	clear();
	stand_title("·¢ËÍ¾ÜĞÅ\n");
	clrtoeol();
	prints("ÇëÈ·±£ÄãÒª·¢ËÍ¾ÜĞÅµÄidĞ´¹ıÉêÇë");
	if (!gettheuserid(2,"ÇëÊäÈë±»¾ÜÕßid: ",&id))
		return 0;
	prints("ÇëÑ¡Ôñ¾ÜĞÅÀà±ğ:\n1.Ê§°ÜµÄ°åÎñÉêÇë\n2.Ê§°ÜµÄ°åÃæÉêÇë\n3.Ê§°ÜµÄ¸öÈËÎÄ¼¯ÉêÇë\n4.Ê§°ÜµÄÓÀ¾ÃÕÊºÅÉêÇë\n5.exit");


	getdata(9, 0, "# [5] ", nbuf, 2, DOECHO, YEA);
	int sel=nbuf[0]-'0';
	if(sel<1 || sel>=5 )
		return 0;
	if(sel==1)
	{
		if(!gettheboardname(10,"ÄÄ¸ö°åÃæµÄ°åÎñ? ",&pos,&fh,bname))
			return -1;
	}
	if(sel==2)
	{

		getdata(10, 0, "ËûÉêÇëµÄ°åÃæÊÇ:", bname, STRLEN-5, DOECHO, YEA);
	}


	/*  prints("¾Ü¾ø%sµÄ%sÉêÇë",lookupuser.userid,note[sel]);
	  if( askyn("  ÄãÈ·¶¨Âğ?",NA,NA)== NA ) {
	      prints("¾Ü¾øÉêÇëÈ¡Ïû");
	      pressanykey();
	      clear();
	      return -1;
	  }
	  */
	prints("ÄãÒª¾Ü¾ø%sÁË,ÓĞÊ²Ã´»°ÒªËµÂğ?¸ø¸öÀíÓÉÏÈ(×î¶àÎåĞĞ£¬°´Enter½áÊø)",lookupuser.userid);
	for (i=0;i<5;i++)
	{
		getdata(i+12, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
	}



	prints("¾Ü¾ø%sµÄ%s%sÉêÇë",lookupuser.userid,bname,note[sel]);
	if( askyn("  ÄãÈ·¶¨Âğ?",NA,NA)== NA )
	{
		prints("\n\n¾Ü¾øÉêÇëÈ¡Ïû");
		pressanykey();
		clear();
		return -1;
	}

	char    secu[STRLEN];
	sprintf(secu, "¾Ü¾øÉêÇë, ¾Ü¾ø %s ÉêÇë%s%s", lookupuser.userid,bname,note[sel]);
	securityreport(secu);

	char tmpfile[STRLEN],title[STRLEN];
	sprintf(tmpfile,"tmp/refsmail.%s", currentuser.userid);
	sprintf(title,"[Í¨¸æ] ¹ØÓÚÄúµÄ%s%sÉêÇë",bname,note[sel]);
	FILE *fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Erroe,²»ÄÜ´´½¨ÁÙÊ±ÎÄ¼ş");
		pressanykey();
		clear();
		return -1;
	}

	char msg[STRLEN];
	if(sel==1)
		sprintf(msg,"Ê×ÏÈ¸ĞĞ»ÄúÉêÇë%s°å°åÎñ.",bname);
	else
		sprintf(msg,"ÄúµÄ%s%sÉêÇëÎÒÃÇÒÑ¾­¿´¹ıÁË.",bname,note[sel]);


	struct tm *tm;
	time_t now;
	time(&now);
	char datestring[STRLEN];
	tm = localtime(&now);
	sprintf(datestring,"%4dÄê%02dÔÂ%02dÈÕ",
	        tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday
	       );
	sprintf(genbuf,"\n\t[Í¨¸æ]\n\tÇ×°®µÄ%s:\n\t\tÄúºÃ!%s\n\t\tÒÅº¶µÄÊÇ,¾­¹ıÕ¾Îñ×éµÄÌÖÂÛ,ÎÒÃÇ²»ÄÜÅú×¼ÄúµÄÉêÇë.\n\t\tÄúÈç¹ûÓĞÎÊÌâ,Çëµ½sysop°å·¢ÎÄÑ¯ÎÊ.\n\t\t×îºó,Ï£ÍûÄúÄÜ¼ÌĞø¹Ø×¢ºÍÖ§³Ö±¾BBS.\n\t\t\t\t\t\t\t\t\t\t  %s Õ¾Îñ×é\n\t\t\t\t\t\t\t\t\t\t   %s",lookupuser.userid,msg,BBSNAME,datestring);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\nÕ¾³¤¸½ÑÔ£º\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}

	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,title);

	unlink(tmpfile);
	prints("\n\n¾ÜĞÅ·¢ËÍ³É¹¦!");
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
	stand_title("°åÖ÷ÀëÖ°\n");
	clrtoeol();
	if (!gettheuserid(2,"ÊäÈëÓûÀëÖ°µÄ°åÖ÷ÕÊºÅ: ",&id))
		return -1;
	if (!gettheboardname(3,"ÇëÊäÈë¸Ã°åÖ÷Òª´ÇÈ¥µÄ°åÃû: ",&pos,&fh,bname))
		return -1;
	if (is_zone(BOARDS,bname))
	{
        	move ( 5,0);
        	prints("Äã²»ÄÜÀëÈÎ×Ó·ÖÇøÇø³¤");
        	pressanykey(); 
                clear();
	        return -1;
	}
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if ( !oldbm || !find)
	{
		move ( 5,0);
		prints(" %s %s°å°åÖ÷£¬ÈçÓĞ´íÎó£¬ÇëÍ¨Öª³ÌĞòÕ¾³¤¡£",lookupuser.userid,(oldbm)?"²»ÊÇ¸Ã":"Ã»ÓĞµ£ÈÎÈÎºÎ");
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
		prints("¶Ô²»Æğ£¬ %s °å°åÖ÷Ãûµ¥ÖĞÃ»ÓĞ %s £¬ÈçÓĞ´íÎó£¬ÇëÍ¨Öª³ÌĞòÕ¾³¤¡£",
		       bname, lookupuser.userid);
		pressanykey();
		clear();
		return -1;
	}
	prints("\nÄã½«È¡Ïû %s µÄ %s °å°å%sÖ°Îñ.\n",
	       lookupuser.userid,bname,bm?"Ö÷":"¸±");
	if( askyn("ÄãÈ·¶¨ÒªÈ¡ÏûËûµÄ¸Ã°å°åÖ÷Ö°ÎñÂğ?",NA,NA)== NA )
	{
		prints("\nºÇºÇ£¬Äã¸Ä±äĞÄÒâÁË£¿ %s ¼ÌĞøÁôÈÎ %s °å°åÖ÷Ö°Îñ£¡",
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
	sprintf(genbuf,"È¡Ïû %s µÄ %s ÌÖÂÛÇø°åÖ÷Ö°Îñ",lookupuser.userid,fh.filename);
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
			sprintf(secu,"°åÖ÷Ğ¶Ö°, È¡Ïû %s µÄ°åÖ÷È¨ÏŞ", lookupuser.userid);
			securityreport(secu);
			move( 9,0);
			prints(secu);
		}
	}
	prints("\n\n");
	if( askyn("ĞèÒªÔÚÏà¹Ø°åÃæ·¢ËÍ¹«¸æÂğ?",YEA,NA)== NA )
	{
		pressanykey();
		return 0;
	}
	prints("\n");
	if(askyn("Õı³£ÀëÈÎÇë°´ Enter ¼üÈ·ÈÏ£¬³·Ö°³Í·£°´ N ¼ü",YEA,NA)==YEA)
		right=1;
	else
		right = 0;
	if(right)
		sprintf(bmfilename,"%s °å%s %s ÀëÈÎ¹«¸æ",bname,bm?"°åÖ÷":"°å¸±",lookupuser.userid);
	else
		sprintf(bmfilename,"[¹«¸æ]³·³ı %s °å°åÎñ %s ",bname,lookupuser.userid);
	strcpy(currboard,bname);
	if(right)
	{
		sprintf(genbuf,"\n\t\t\t¡¾ ¹«¸æ ¡¿\n\n"
		        "\t¾­Õ¾Îñ×éÌÖÂÛ£º\n"
		        "\tÍ¬Òâ %s ´ÇÈ¥ %s °åµÄ%sÖ°Îñ¡£\n"
		        "\tÔÚ´Ë£¬¶Ô%sÔø¾­ÔÚ %s °åµÄĞÁ¿àÀÍ×÷±íÊ¾¸ĞĞ»¡£\n\n"
		        "\tÏ£Íû½ñºóÒ²ÄÜÖ§³Ö±¾°åµÄ¹¤×÷.",
		        lookupuser.userid,bname,bm?"°åÖ÷":"°å¸±",(lookupuser.gender=='F'||lookupuser.gender=='f')?"Ëı":"Ëû",bname);
	}
	else
	{
		sprintf(genbuf,"\n\t\t\t¡¾³·Ö°¹«¸æ¡¿\n\n"
		        "\t¾­Õ¾Îñ×éÌÖÂÛ¾ö¶¨£º\n"
		        "\t³·³ı %s °å°åÎñ %s ¡£\n",
		        bname,lookupuser.userid);
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(14,0);
	prints("ÇëÊäÈë%s¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)",right?"°åÖ÷ÀëÈÎ":"°åÖ÷³·Ö°");
	for (i=0;i<5;i++)
	{
		getdata(i+15, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,right?"\n\nÀëÈÎ¸½ÑÔ£º\n":"\n\n³·Ö°ËµÃ÷£º\n");
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
		prints("Error,´´½¨ÁÙÊ±ÎÄ¼ş´íÎó.ÇëÓëÏµÍ³Î¬»¤ÁªÏµ»òÔÙ´Î³¢ÊÔ!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,buftitle);

	//°åÎñÀëÈÎºóÇå¿ÕË¿Â·
	sethomefile(genbuf, lookupuser.userid, ".announcepath");
	if(dashf(genbuf))
		unlink(genbuf);

	prints("\nÖ´ĞĞÍê±Ï£¡");
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
	stand_title("×Ó·ÖÇøÇø³¤ÀëÖ°\n");
	clrtoeol();
	if (!gettheuserid(2,"ÊäÈëÓûÀëÖ°µÄ×Ó·ÖÇøÇø³¤ÕÊºÅ: ",&id))
		return -1;
	if (!gettheboardname(3,"ÇëÊäÈë¸Ã°åÖ÷Òª´ÇÈ¥µÄ×Ó·ÖÇøÃû: ",&pos,&fh,bname))
		return -1;
        if (!is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("Äã²»ÄÜÀëÈÎ°åÎñ");
                pressanykey();
                clear();
                return -1;
        }
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if ( !oldbm || !find)
	{
		move ( 5,0);
		prints(" %s %s×Ó·ÖÇøÇø³¤£¬ÈçÓĞ´íÎó£¬ÇëÍ¨ÖªÏµÍ³Î¬»¤¡£",lookupuser.userid,(oldbm)?"²»ÊÇ¸Ã":"Ã»ÓĞµ£ÈÎÈÎºÎ");
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
		prints("¶Ô²»Æğ£¬ %s ×Ó·ÖÇøÇø³¤Ãûµ¥ÖĞÃ»ÓĞ %s £¬ÈçÓĞ´íÎó£¬ÇëÍ¨Öª³ÌĞòÕ¾³¤¡£",bname, lookupuser.userid);
		pressanykey();
		clear();
		return -1;
	}
	prints("\nÄã½«È¡Ïû %s µÄ %s ×Ó·ÖÇøÇø³¤Ö°Îñ.\n",lookupuser.userid,bname);
	if( askyn("ÄãÈ·¶¨ÒªÈ¡ÏûËûµÄ¸Ã×Ó·ÖÇøÇø³¤Ö°ÎñÂğ?",NA,NA)== NA )
	{
		prints("\nºÇºÇ£¬Äã¸Ä±äĞÄÒâÁË£¿ %s ¼ÌĞøÁôÈÎ %s ×Ó·ÖÇøÇø³¤Ö°Îñ£¡",
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
	sprintf(genbuf,"È¡Ïû %s µÄ %s ×Ó·ÖÇøÇø³¤Ö°Îñ",lookupuser.userid,fh.filename);
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
			sprintf(secu,"°åÖ÷Ğ¶Ö°, È¡Ïû %s µÄ×Ó·ÖÇøÇø³¤È¨ÏŞ", lookupuser.userid);
			securityreport(secu);
			move( 9,0);
			prints(secu);
		}
	}
	prints("\n\n");
	if(askyn("Õı³£ÀëÈÎÇë°´ Enter ¼üÈ·ÈÏ£¬³·Ö°³Í·£°´ N ¼ü",YEA,NA)==YEA)
		right=1;
	else
		right = 0;
	if(right)
		sprintf(bmfilename,"%s ×Ó·ÖÇøÇø³¤ %s ÀëÈÎ¹«¸æ",bname,lookupuser.userid);
	else
		sprintf(bmfilename,"[¹«¸æ]³·³ı %s ×Ó·ÖÇøÇø³¤ %s ",bname,lookupuser.userid);
	strcpy(currboard,bname);
	if(right)
	{
		sprintf(genbuf,"\n\t\t\t¡¾ ¹«¸æ ¡¿\n\n"
		        "\t¾­Õ¾Îñ×éÌÖÂÛ£º\n"
		        "\tÍ¬Òâ %s ´ÇÈ¥ %s ×Ó·ÖÇøµÄÇø³¤Ö°Îñ¡£\n"
		        "\tÔÚ´Ë£¬¶ÔÄãÔø¾­ÔÚ %s ×Ó·ÖÇøµÄĞÁ¿àÀÍ×÷±íÊ¾¸ĞĞ»¡£\n\n"
		        "\tÏ£Íû½ñºóÒ²ÄÜÖ§³Ö±¾×Ó·ÖÇøµÄ¹¤×÷.",
		        lookupuser.userid,bname,bname);
	}
	else
	{
		sprintf(genbuf,"\n\t\t\t¡¾³·Ö°¹«¸æ¡¿\n\n"
		        "\t¾­Õ¾Îñ×éÌÖÂÛ¾ö¶¨£º\n"
		        "\t³·³ı %s ×Ó·ÖÇøÇø³¤ %s µÄÇø³¤Ö°Îñ¡£\n",
		        bname,lookupuser.userid);
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(14,0);
	prints("ÇëÊäÈë%s¸½ÑÔ(×î¶àÎåĞĞ£¬°´ Enter ½áÊø)",right?"×Ó·ÖÇøÇø³¤ÀëÈÎ":"×Ó·ÖÇøÇø³¤³·Ö°");
	for (i=0;i<5;i++)
	{
		getdata(i+15, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,right?"\n\nÀëÈÎ¸½ÑÔ£º\n":"\n\n³·Ö°ËµÃ÷£º\n");
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
		prints("Error,´´½¨ÁÙÊ±ÎÄ¼ş´íÎó.ÇëÓëÏµÍ³Î¬»¤ÁªÏµ»òÔÙ´Î³¢ÊÔ!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,buftitle);
	
	//×Ó·ÖÇø³¤ÀëÈÎºóÇå¿ÕË¿Â·
	sethomefile(genbuf, lookupuser.userid, ".announcepath");
	if(dashf(genbuf))
		unlink(genbuf);
	
	prints("\nÖ´ĞĞÍê±Ï£¡");
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
	prints("\n¡¾\033[0;1;4;33m×¢Òâ\033[m¡¿ÏµÍ³·¢ÏÖ menu.ini µÄÅäÖÃ"
	       "¿ÉÄÜ´æÔÚÎÊÌâ£¬½¨Òé¼ì²éºóÔÙ¼ÌĞø½øĞĞ£¡\n        ");
	prints("Èç¹û²»Ã÷°×¸ÃÌáÊ¾µÄº¬Òå£¬Çë¿½±´±¾ÆÁĞÅÏ¢µ½¹àË®Õ¾±¨¸æÎÊÌâ£¡\n\n");
	prints("±¾Õ¾Ä¿Ç°µÄÌÖÂÛÇø·ÖÀàÇé¿öÈçÏÂ£º\n\n");
	prints(" ·ÖÇø   ·ÖÇøÃû³Æ     ·ÖÇøÀà±ğÃèÊö             Àà±ğ´úÂë\n");
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
	if(askyn("ÉÏÃæµÄĞÅÏ¢±íÃ÷ menu.ini ¿ÉÄÜÓĞÎó£¬ÄúÈÔÈ»Ïë¼ÌĞøÖ´ĞĞÂğ",NA,NA)==YEA)
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
		prints("Äú½«ÖØĞÂÉè¶¨ [\033[32m%s\033[m] µÄ°åÃæÊôĞÔ [Enter-->Éè¶¨²»±ä]",
		       oldfh->filename);
	}
	else
		prints("Äú½«¿ªÊ¼´´½¨Ò»¸ö\033[32mĞÂ\033[mµÄÌÖÂÛÇø [ENTER-->È¡Ïû´´½¨]");
	while(1)
	{
		struct boardheader dh;
		getdata(3, 0, "ÌÖÂÛÇøÃû³Æ(Ó¢ÎÄÃû): ", buf, 18, DOECHO, YEA);
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
			prints("\n´íÎó! ´ËÌÖÂÛÇøÒÑ¾­´æÔÚ!!");
			if(oldfh)
				continue;
			pressanykey();
			return -1;
		}
		strcpy(newfh.filename, buf);
		break;
	}
	prints("\n[ÌÖÂÛÇøËµÃ÷]·ÖÇø´úÂë + ·ÖÇøÃèÊö + ×ªĞÅ±êÖ¾ + ÌÖÂÛÇø±êÌâ\n");
	if(oldfh)
	{
		prints("³õÊ¼Éè¶¨£º%11s\033[32m%s\033[m\n",oldfh->title,oldfh->title+11);
	}
	else
		prints("¸ñÊ½·¶Àı£ºa[±¾Õ¾] ¡ğ \033[32mÕâÊÇÒ»¸öÕ¾ÄÚÌÖÂÛÇø\033[m\n");
	while(1)
	{
		if(oldfh)
			strcpy(buf,oldfh->title+11);
		else
			buf[0] = '\0';
		getdata(8, 0, "ÌÖÂÛÇø±êÌâ(ÖĞÎÄÃû): ", buf, 41, DOECHO, NA);
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
	if(askyn("¸ÃÌÖÂÛÇøĞèÒª²ÎÓë×ªĞÅÂğ",NA,NA)==YEA)
	{
		strcpy(nntp,"¡ñ");
		newfh.flag |= OUT_FLAG;
	}
	else
	{
		strcpy(nntp,"¡ğ");
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
		prints("©§%s(%d)%-12s\033[m©§",
		       (i+'0'==ch)?"\033[32m":"",i,GroupsInfo.name[i]);
	}
	while(1)
	{
		num[0] = ch;
		num[1] = '\0';
		getdata(11, 0, "¸ÃÌÖÂÛÇø·ÅÖÃÔÚÄÄÒ»·ÖÇø(²Î¿¼ÓÒ±ßÌáÊ¾±àºÅ)£¿: ",
		        num, 2, DOECHO, NA);
		if(num[0]=='\0'||num[0]<'0'||num[0]>'9')
			continue;
		if(GroupsInfo.name[num[0]-'0'][0] == '\0')
			continue;
		break;
	}
	prints("\nµÚ %c ÇøµÄ·ÖÀà·ûºÅ²Î¿¼£º[\033[32m%s\033[m] ·ÖÀàÃèÊö²Î¿¼£º%s",
	       num[0],GroupsInfo.chs[num[0]-'0'],GroupsInfo.items[num[0]-'0']);
	while(1)
	{
		if(oldfh==NULL||!strchr(GroupsInfo.chs[num[0]-'0'],oldfh->title[0]))
			chs[0] = GroupsInfo.chs[num[0]-'0'][0];
		else
			chs[0] = oldfh->title[0];
		chs[1] = '\0';
		getdata(14,0,"ÇëÊäÈë¸ÃÌÖÂÛÇøµÄ·ÖÀà·ûºÅ: ",chs,2,DOECHO,NA);
		if(chs[0]=='\0')
			continue;
		if(strchr(GroupsInfo.chs[num[0]-'0'],chs[0]))
			break;
	}
	move(12,0);
	prints("\nµÚ %c ÇøµÄ·ÖÀà·ûºÅ²Î¿¼£º[%s] ·ÖÀàÃèÊö²Î¿¼£º\033[32m%s\033[m",
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
		getdata(15,0,"ÇëÊäÈë¸ÃÌÖÂÛÇøµÄ·ÖÀàÃèÊö: ",buf,7,DOECHO,NA);
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
		prints("\n°åÖ÷ÉèÖÃ  [ Èç¹û²»ĞèÒª°åÖ÷£¬ÔòÏµÍ³×Ô¶¯ÉèÖÃÎªÓÉ SYSOPs ¹ÜÀí]\n");
		if(askyn("±¾°åĞèÒª°åÖ÷Âğ",YEA,NA)==NA)
			strcpy(newfh.BM, "SYSOPs");
		else
			newfh.BM[0] = '\0';
	}
	else
	{
		prints("\n°åÖ÷ÉèÖÃ  [ Ä¿Ç°°åÃæµÄ°åÖ÷ÊÇ: %s ]\n", oldfh->BM);
		if(askyn("µ±³öÏÖ°åÖ÷Ãûµ¥´íÎóµÄÊ±ºò£¬²ÅĞèÒªĞŞ¸Ä´Ë´¦¡£ĞèÒªĞŞ¸Ä°åÖ÷Ãûµ¥Âğ",
		         NA,NA)==YEA)
		{
			prints("¡¾\033[31m¾¯¸æ\033[m¡¿ĞŞ¸Ä°åÖ÷½ö¹©³ö´íĞŞÕıÊ¹ÓÃ£¬"
			       "°åÖ÷ÈÎÃâÇëÎğ¸Ä¶¯´Ë´¦£¡\n");
			strcpy(newfh.BM,oldfh->BM);
			getdata(20,0,"°åÖ÷Ãûµ¥: ",newfh.BM,sizeof(newfh.BM),DOECHO,NA);
			if(newfh.BM[0] == ' ')
				newfh.BM[0] = '\0';
		}
		else
			strcpy(newfh.BM,oldfh->BM);
	}
	clear();
	if(askyn("±¾ÌÖÂÛÇøĞèÒªÉèÖÃ READ »ò POST ÏŞÖÆÂğ",NA,NA)==YEA)
	{
		char ans[4];
		sprintf(buf, "ÏŞÖÆ (R)ÔÄ¶Á »ò (P)ÕÅÌù ÎÄÕÂ [%c]: ",
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
		prints("Éè¶¨ %s '%s' ÌÖÂÛÇøµÄÈ¨ÏŞ\n",
		       newfh.level & PERM_POSTMASK ? "ÕÅÌù" : "ÔÄ¶Á", newfh.filename);
		newfh.level = setperms(newfh.level, "È¨ÏŞ", NUMPERMS, showperminfo);
		clear();
	}
	move(1,0);
	if(askyn("ÉèÖÃ±¾°åÎªÄäÃû°åÃæÂğ",newfh.flag & ANONY_FLAG?YEA:NA,NA)==YEA)
		newfh.flag |= ANONY_FLAG;
	else
		newfh.flag &= ~ANONY_FLAG;
	move(2,0);
	if(askyn("ÉèÖÃ±¾°åÎª²»¿É Re Ä£Ê½°åÃæÂğ",
	         (newfh.flag&NOREPLY_FLAG)?YEA:NA,NA)==YEA)
		newfh.flag |= NOREPLY_FLAG;
	else
		newfh.flag &= ~NOREPLY_FLAG;
	move(3,0);
	if(oldfh!=NULL)
	{
		if(askyn("ÊÇ·ñÒÆ¶¯¾«»ªÇøµÄÎ»ÖÃ",NA,NA)==YEA)
			a_mv = 2;
		else
			a_mv = 0;
	}
	else
	{
		if(askyn("±¾°åĞèÒªÉèÁ¢¾«»ªÇøÂğ",YEA,NA)==YEA)
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
			prints("©§%s(%d)%s GROUP_%d\033[m",
			       (i+'0'==ch)?"\033[32m":"",i, GroupsInfo.name[i], i);
		}
		while(1)
		{
			strcpy(chs,num);
			getdata(j+1,48,"¾«»ªÇø·Ö²¼ÈçÉÏ£¬ÇëÑ¡Ôñ: ",chs,2,DOECHO,NA);
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
		if(askyn("ÄúÈ·ÈÏÒª¸ü¸ÄÂğ",NA,NA)==NA)
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
			prints("\n³õÊ¼»¯ÌÖÂÛÇøÄ¿Â¼Ê±³ö´í!\n");
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
				prints("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
			else
				prints("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
		}
	}
	else
	{  // ĞŞ¸ÄÌÖÂÛÇø
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
					prints("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
				else
					prints("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
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
			sprintf(genbuf, "¸ü¸ÄÌÖÂÛÇø %s µÄ×ÊÁÏ --> %s",
			        oldfh->filename, newfh.filename);
			report(genbuf);
		}
	}
	else if(append_record(BOARDS, &newfh, sizeof(newfh)) == -1)
	{
		prints("\n³ÉÁ¢ĞÂ°åÊ§°Ü£¡\n");
		pressanykey();
		clear();
		return -1;
	}
	numboards = -1;
	{
		char    secu[STRLEN];
		if(oldfh)
			sprintf(secu, "ĞŞ¸ÄÌÖÂÛÇø£º%s(%s)",oldfh->filename,newfh.filename);
		else
			sprintf(secu, "³ÉÁ¢ĞÂ°å£º%s", newfh.filename);
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
	stand_title("¿ªÆôĞÂÌÖÂÛÇø") ;
	ChangeTheBoard(NULL,0);
	return 0 ;
}
void ShowBrdSet(struct boardheader fh)
{
	int i = 4;
	move(i,0);
	prints("ÌÖÂÛÇø [%s] »ù±¾Çé¿ö£º",fh.filename);
	i += 2;
	move(i++,0);
	prints("ÏîÄ¿Ãû³Æ           ÏîÄ¿ÊôĞÔ");
	move(i++,0);
	prints("------------------------------");
	move(i++,0);
	prints("ÌÖÂÛÇøÃû³Æ       : %s",fh.filename);
	move(i++,0);
	prints("ÌÖÂÛÇø¹ÜÀíÔ±     : %s",fh.BM[0]=='\0'?"ÉĞÎŞ°åÖ÷":fh.BM);
	move(i++,0);
	prints("ÊÇ·ñÎªÄäÃûÌÖÂÛÇø : %s",(fh.flag&ANONY_FLAG) ? "ÄäÃû" : "·ÇÄäÃû");
	move(i++,0);
	prints("ÎÄÕÂÊÇ·ñ¿ÉÒÔ»Ø¸´ : %s",(fh.flag&NOREPLY_FLAG)?"²»¿É Re":"¿ÉÒÔ");
	move(i++,0);
	prints("READ/POST ÏŞÖÆ   : ");
	if(fh.level & ~PERM_POSTMASK)
	{
		prints("%s", (fh.level & PERM_POSTMASK) ? "POST" :
		       (fh.level & PERM_NOZAP) ? "ZAP" : "READ");
	}
	else
		prints("ÎŞÏŞÖÆ");
	move(i++,0);
	prints("±¾ÌÖÂÛÇøËµÃ÷     : %s\n\n",fh.title);
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
	stand_title("ĞŞ¸ÄÌÖÂÛÇø»ù±¾ÊôĞÔ");
	if(!gettheboardname(2,"ÊäÈëÌÖÂÛÇøÃû³Æ: ",&pos,&fh,bname))
		return -1;
	ShowBrdSet(fh);
	move(15, 0);
	if (askyn("ÊÇ·ñĞèÒª¸ü¸ÄÒÔÉÏÊôĞÔ", NA, NA) == YEA)
	{
		clear();
		stand_title("ĞŞ¸ÄÌÖÂÛÇø»ù±¾ÊôĞÔ");
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
        stand_title("¿ªÆôĞÂ×Ó·ÖÇø");
        memset(&newboard, 0, sizeof(newboard));
        move(2, 0);
        //   ansimore2("etc/boardref", NA, 3, 7);
        ansimore2("etc/zoneref", NA, 2, 9);
        while (1)
        {
                getdata(11, 0, "×Ó·ÖÇøÃû³Æ:   ", newboard.filename, 18, DOECHO, YEA);
                if (newboard.filename[0] != 0)
                {
                        struct boardheader dh;
                        if (search_record(BOARDS,&dh,sizeof(dh),cmpbnames,newboard.filename))
                        {
                                prints("\n´íÎó! ´ËÌÖÂÛÇøÒÑ¾­´æÔÚ!!");
                                pressanykey();
                                return -1;
                        }
                }
                else
                        return -1;
                if (valid_brdname(newboard.filename))
                        break;
                prints("\n²»ºÏ·¨Ãû³Æ!!");
        }
        newboard.flag = 0;
        while (1)
        {
                getdata(12, 0, "×Ó·ÖÇøËµÃ÷:   ", newboard.title, 60, DOECHO, YEA);
                if (newboard.title[0] == '\0')
                        return -1;
                if (strstr(newboard.title, "£«"))
                {
                        newboard.flag |= ZONE_FLAG;
                        break;
                }
                else
                        prints("´íÎóµÄ×Ó·ÖÇø¸ñÊ½!!");
        }
        if (   getbnum(newboard.filename) > 0 )
        {
                prints("\n´íÎóµÄ×Ó·ÖÇøÃû³Æ!!\n");
                pressreturn();
                clear();
                return -1;
        }
        newboard.BM[0] = '\0';
        move(13,0);
	if (askyn("¸Ã·ÖÇøÊÇ·ñÎª¶à¼¶×Ó·ÖÇø", NA, NA) == YEA)
        {
                while(1)
                {
                        getdata(15, 0, "¸¸×Ó·ÖÇøÃû³Æ:   ", zonename, 18, DOECHO, YEA);
                        if(search_zone(BOARDS,zonename,newboard.title[0]))
                        {
                                break;
                        }
                }
                strcpy(newboard.owner,zonename);
        }
        if (askyn("ÊÇ·ñÏŞÖÆ´æÈ¡È¨Á¦", NA, NA) == YEA)
        {
                getdata(17, 0, "ÏŞÖÆ Read/Post? [R]: ", ans, 2, DOECHO, YEA);
                if (*ans == 'P' || *ans == 'p')
                        newboard.level = PERM_POSTMASK;
                else
                        newboard.level = 0;
                move(1, 0);
                clrtobot();
                move(2, 0);
                prints("Éè¶¨ %s È¨Á¦. ÌÖÂÛÇø: '%s'\n",
                       (newboard.level & PERM_POSTMASK ? "POST" : "READ"),newboard.filename);
                newboard.level=setperms(newboard.level,"È¨ÏŞ",NUMPERMS,showperminfo);
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
        prints("\nĞÂ×Ó·ÖÇø³ÉÁ¢\n");
        {
                char    secu[STRLEN];
                sprintf(secu, "³ÉÁ¢ĞÂ×Ó·ÖÇø£º%s", newboard.filename);
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
	stand_title("¿ªÆôĞÂÌÖÂÛÇø");
	memset(&newboard, 0, sizeof(newboard));
	move(2, 0);
	//   ansimore2("etc/boardref", NA, 3, 7);
	ansimore2("etc/boardref", NA, 2, 9);
	while (1)
	{
		getdata(11, 0, "ÌÖÂÛÇøÃû³Æ:   ", newboard.filename, 18, DOECHO, YEA);
		if (newboard.filename[0] != 0)
		{
			struct boardheader dh;
			if (search_record(BOARDS,&dh,sizeof(dh),cmpbnames,newboard.filename))
			{
				prints("\n´íÎó! ´ËÌÖÂÛÇøÒÑ¾­´æÔÚ!!");
				pressanykey();
				return -1;
			}
		}
		else
			return -1;
		if (valid_brdname(newboard.filename))
			break;
		prints("\n²»ºÏ·¨Ãû³Æ!!");
	}
	newboard.flag = 0;
	while (1)
	{
		getdata(12, 0, "ÌÖÂÛÇøËµÃ÷:   ", newboard.title, 60, DOECHO, YEA);
		if (newboard.title[0] == '\0')
			return -1;
		if (strstr(newboard.title, "¡ñ") || strstr(newboard.title, "¡Ñ"))
		{
			newboard.flag |= OUT_FLAG;
			break;
		}
		else if (strstr(newboard.title, "¡ğ"))
		{
			newboard.flag &= ~OUT_FLAG;
			break;
		}
		else
			prints("´íÎóµÄ¸ñÊ½, ÎŞ·¨ÅĞ¶ÏÊÇ·ñ×ªĞÅ!!");
	}
	strcpy(vbuf, "vote/");
	strcat(vbuf, newboard.filename);
	setbpath(genbuf, newboard.filename);
	if (   getbnum(newboard.filename) > 0 || mkdir(genbuf, 0755) == -1
	        || mkdir(vbuf, 0755) == -1)
	{
		prints("\n´íÎóµÄÌÖÂÛÇøÃû³Æ!!\n");
		pressreturn();
		clear();
		return -1;
	}
	move(13,0);
	if(askyn("±¾°å³ÏÕ÷°åÖ÷Âğ(·ñÔòÓÉSYSOPs¹ÜÀí)?",YEA,NA)==NA)
		strcpy(newboard.BM, "SYSOPs");
	else
		newboard.BM[0] = '\0';
	move(14, 0);
	if (askyn("ÊÇ·ñÏŞÖÆ´æÈ¡È¨Á¦", NA, NA) == YEA)
	{
		getdata(15, 0, "ÏŞÖÆ Read/Post? [R]: ", ans, 2, DOECHO, YEA);
		if (*ans == 'P' || *ans == 'p')
			newboard.level = PERM_POSTMASK;
		else
			newboard.level = 0;
		move(1, 0);
		clrtobot();
		move(2, 0);
		prints("Éè¶¨ %s È¨Á¦. ÌÖÂÛÇø: '%s'\n",
		       (newboard.level & PERM_POSTMASK ? "POST" : "READ"),newboard.filename);
		newboard.level=setperms(newboard.level,"È¨ÏŞ",NUMPERMS,showperminfo);
		clear();
	}
	else
		newboard.level = 0;
	move(15, 0);
	if (askyn("ÊÇ·ñ¼ÓÈëÄäÃû°å", NA, NA) == YEA)
		newboard.flag |= ANONY_FLAG;
	else
		newboard.flag &= ~ANONY_FLAG;
	if (askyn("¸Ã°åµÄÈ«²¿ÎÄÕÂ¾ù²»¿ÉÒÔ»Ø¸´", NA, NA) == YEA)
		newboard.flag |= NOREPLY_FLAG;
	else
		newboard.flag &= ~NOREPLY_FLAG;
	if (askyn("Èç¹û¸Ã°åÎª·â±ÕÊ½¾ãÀÖ²¿°åÃæÊÇ·ñ¶Ô·Ç»áÔ±Òş²Ø£¿", NA, NA) == YEA)//loveni
		newboard.flag2 |= HIDE_FLAG;
	else
		newboard.flag2 &= ~HIDE_FLAG;
	if (askyn("±¾°åÊÇ·ñ²»ËãÎÄÕÂÊı£¿", NA, NA) == YEA)//loveni
		newboard.flag2 |= JUNK_FLAG;
	else
		newboard.flag2 &= ~JUNK_FLAG;
	if (askyn("±¾°åÎÄÕÂÊÇ·ñ²»Í³¼ÆÈëÊ®´ó£¿", NA, NA) == YEA)//loveni
		newboard.flag2 |= NOTOPTEN_FLAG;
	else
		newboard.flag2 &= ~NOTOPTEN_FLAG;
	if (askyn("±¾°åÊÇ·ñ\033[1;31m²»¹ıÂËÃô¸Ğ×Ö£¿\033[m", NA, NA) == YEA)//loveni
		newboard.flag2 |= NOFILTER_FLAG;
	else
		newboard.flag2 &= ~NOFILTER_FLAG;
	if (askyn("±¾°åÊÇ·ñÇ¿ÖÆÄ£°å·¢ÎÄ£¿", NA, NA) == YEA)//loveni
		newboard.flag2 |= TEMPLATE_FLAG;
	else
		newboard.flag2 &= ~TEMPLATE_FLAG;
        if (askyn("¸Ã°åÊÇ·ñÎª¶ş¼¶°åÃæ", NA, NA) == YEA)
        {
                newboard.flag |= CHILDBOARD_FLAG;
                while(1)
                {
                        getdata(20, 0, "×Ó·ÖÇøÃû³Æ:   ", zonename, 18, DOECHO, YEA);
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
			prints("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
		else
			prints("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
	}
	numboards = -1;
	prints("\nĞÂÌÖÂÛÇø³ÉÁ¢\n");
	{
		char    secu[STRLEN];
		sprintf(secu, "³ÉÁ¢ĞÂ°å£º%s", newboard.filename);
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
	stand_title("ĞŞ¸ÄÌÖÂÛÇø×ÊÑ¶");
	if(!gettheboardname(2,"ÊäÈëÌÖÂÛÇøÃû³Æ: ",&pos,&fh,bname))
		return -1;
	if(fh.flag & ZONE_FLAG)
        {
                move(2,0);
                prints("ÕâÀï²»ÄÜĞŞ¸Ä×Ó·ÖÇøĞÅÏ¢\n");
                pressreturn();
                return -1;
        }
	noidboard = fh.flag & ANONY_FLAG;
	noreply = fh.flag & NOREPLY_FLAG;
	clear();
	move(0, 0);
	memcpy(&newfh, &fh, sizeof(newfh));
	prints("ÌÖÂÛÇøÃû³Æ:  %s               ÌÖÂÛÇø¹ÜÀíÔ±:  %s\n", fh.filename, fh.BM);
	prints("ÌÖÂÛÇøËµÃ÷:     %s\n", fh.title);
//	prints("ÌÖÂÛÇø¹ÜÀíÔ±:   %s\n", fh.BM);
	prints("ÄäÃûÌÖÂÛÇø:     %s             ", (noidboard) ? "Yes" : "No");
	prints("ÎÄÕÂ²»¿ÉÒÔ»Ø¸´: %s\n", (noreply) ? "Yes" : "No");
	prints("·â±ÕÊ½¾ãÀÖ²¿°åÃæÊÇ·ñ¶Ô·Ç»áÔ±Òş²Ø:%s\n",fh.flag2 & HIDE_FLAG ? "Yes" : "No");
	prints("±¾°åÊÇ·ñ²»ËãÎÄÕÂÊı:%s            ",fh.flag2 & JUNK_FLAG ? "Yes" : "No");
	prints("±¾°åÎÄÕÂÊÇ·ñ²»Í³¼ÆÈëÊ®´ó:%s\n",fh.flag2 & NOTOPTEN_FLAG? "Yes" : "No");
	prints("±¾°åÊÇ·ñ\033[1;31m²»¹ıÂËÃô¸Ğ×Ö\033[m:%s\n",fh.flag2 & NOFILTER_FLAG ? "Yes" : "No");//loveni
	prints("±¾°åÊÇ·ñÇ¿ÖÆÄ£°å·¢ÎÄ:%s\n",fh.flag2 & TEMPLATE_FLAG ? "Yes" : "No");//loveni
	prints("ÊÇ·ñ¶ş¼¶ÌÖÂÛÇø: %s ", fh.flag&CHILDBOARD_FLAG ? "Yes":"No");
	strcpy(oldtitle, fh.title);
	prints("ÏŞÖÆ %s È¨Á¦: %s", (fh.level & PERM_POSTMASK) ? "POST" :
	       (fh.level & PERM_NOZAP) ? "ZAP" : "READ",
	       (fh.level & ~PERM_POSTMASK) == 0 ? "²»ÉèÏŞ" : "ÓĞÉèÏŞ");
	move(8, 0);
	if (askyn("ÊÇ·ñ¸ü¸ÄÒÔÉÏ×ÊÑ¶", NA, NA) == NA)
	{
		clear();
		return 0;
	}
	move(9, 0);
	prints("Ö±½Ó°´ <Return> ²»ĞŞ¸Ä´ËÀ¸×ÊÑ¶...");
	while (1)
	{
		getdata(10, 0, "ĞÂÌÖÂÛÇøÃû³Æ: ", genbuf, 18, DOECHO, YEA);
		if (*genbuf != 0)
		{
			struct boardheader dh;
			if (search_record(BOARDS, &dh, sizeof(dh), cmpbnames, genbuf))
			{
				move(2, 0);
				prints("´íÎó! ´ËÌÖÂÛÇøÒÑ¾­´æÔÚ!!");
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
				prints("²»ºÏ·¨µÄÌÖÂÛÇøÃû³Æ!");
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
		getdata(17, 0, "ĞÂÌÖÂÛÇøËµÃ÷: ", genbuf, 60, DOECHO, YEA);
		if (*genbuf != 0)
			strncpy(newfh.title, genbuf, sizeof(newfh.title));
		else
			break;
		if (strstr(newfh.title, "¡ñ") || strstr(newfh.title, "¡Ñ"))
		{
			newfh.flag |= OUT_FLAG;
			break;
		}
		else if (strstr(newfh.title, "¡ğ"))
		{
			newfh.flag &= ~OUT_FLAG;
			break;
		}
		else
			prints("\n´íÎóµÄ¸ñÊ½, ÎŞ·¨ÅĞ¶ÏÊÇ·ñ×ªĞÅ!!");
	}
	move(18, 0);
	//clrtoeol();
	//move(20, 0);
	if( fh.BM[0] != '\0' && strcmp(fh.BM,"SYSOPs"))
	{
		if(askyn("ĞŞ¸ÄÌÖÂÛÇø¹ÜÀíÔ±¡£×¢Òâ£ºÈ·ÓĞ´íÎó²Å¿ÉĞŞ¸Ä¡£²»ĞŞ¸Ä°´»Ø³µ",
		         NA,NA) == YEA)
		{
			getdata(18,0,"ÌÖÂÛÇø¹ÜÀíÔ±(¡¾×¢Òâ¡¿°åÖ÷ÈÎÃâÇëÎğ¸Ä¶¯´Ë´¦£¡): ",
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
		if(askyn("±¾°å³ÏÕ÷°åÖ÷Âğ(·ñ£¬ÔòÓÉSYSOPs¹ÜÀí)?",YEA,NA)==NA)
			strncpy(newfh.BM, "SYSOPs", sizeof(newfh.BM));
		else
			strncpy(newfh.BM, "\0", sizeof(newfh.BM));
	}
	sprintf(buf, "ÄäÃû°å (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
	getdata(19, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag |= ANONY_FLAG;
		else
			newfh.flag &= ~ANONY_FLAG;
	}
	sprintf(buf, "ÎÄÕÂÊÇ·ñ²»¿ÉÒÔ»Ø¸´ (Y/N)? [%c]: ", (noreply) ? 'Y' : 'N');
	getdata(19, 30, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag |= NOREPLY_FLAG;
		else
			newfh.flag &= ~NOREPLY_FLAG;
	}
	
	sprintf(buf, "Èç¹û¸Ã°åÎª·â±ÕÊ½¾ãÀÖ²¿°åÃæÊÇ·ñ¶Ô·Ç»áÔ±Òş²Ø£¿ (Y/N)? [%c]: ", (fh.flag2&HIDE_FLAG) ? 'Y' : 'N');
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
                if(askyn("ÊôÓÚ×Ó·ÖÇø¸ü¸Ä·ñ",NA,NA)==YEA)
                {
                        while(1)
                        {
                                sprintf(buf,"¸ü¸Ä×Ó·ÖÇøÃû×ÖÎª(.ÎªÈ¡Ïû×Ó·ÖÇø): ");
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
                if(askyn("²»ÊôÓÚÈÎºÎ×Ó·ÖÇø¸ü¸Ä·ñ",NA,NA)==YEA)
                {
                        while(1)
                        {
                                sprintf(buf,"¸ü¸Ä×Ó·ÖÇøÃû×Ö: ");
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
	if (askyn("ÊÇ·ñÒÆ¶¯¾«»ªÇøµÄÎ»ÖÃ", NA, NA) == YEA)
		a_mv = 2;
	else
		a_mv = 0;
	move(23, 0);
	if (askyn("ÊÇ·ñ¸ü¸Ä´æÈ¡È¨ÏŞ", NA, NA) == YEA)
	{
		char    ans[4];
		sprintf(genbuf, "ÏŞÖÆ (R)ÔÄ¶Á »ò (P)ÕÅÌù ÎÄÕÂ [%c]: ",
		        (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
		getdata(24, 0, genbuf, ans, 2, DOECHO, YEA);
		if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
			newfh.level &= ~PERM_POSTMASK;
		else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
			newfh.level |= PERM_POSTMASK;
		clear();
		move(2, 0);
		prints("Éè¶¨ %s '%s' ÌÖÂÛÇøµÄÈ¨ÏŞ\n",
		       newfh.level & PERM_POSTMASK ? "ÕÅÌù" : "ÔÄ¶Á", newfh.filename);
		newfh.level = setperms(newfh.level, "È¨ÏŞ", NUMPERMS, showperminfo);
		//clear();
		//getdata(0, 0, "È·¶¨Òª¸ü¸ÄÂğ? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
	}
	//else
	//{
	clear();
	sprintf(buf, "±¾°åÊÇ·ñ²»ËãÎÄÕÂÊı£¿ (Y/N)? [%c]: ", (fh.flag2&JUNK_FLAG) ? 'Y' : 'N');
	getdata(0, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= JUNK_FLAG;
		else
			newfh.flag2 &= ~JUNK_FLAG;
	}
	sprintf(buf, "±¾°åÎÄÕÂÊÇ·ñ²»Í³¼ÆÈëÊ®´ó£¿ (Y/N)? [%c]: ", (fh.flag2&NOTOPTEN_FLAG) ? 'Y' : 'N');
	getdata(1, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= NOTOPTEN_FLAG;
		else
			newfh.flag2 &= ~NOTOPTEN_FLAG;
	}
	sprintf(buf, "±¾°åÊÇ·ñ\033[1;31m²»¹ıÂËÃô¸Ğ×Ö£¿\033[m (Y/N)? [%c]: ", (fh.flag2&NOFILTER_FLAG) ? 'Y' : 'N');
	getdata(2, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= NOFILTER_FLAG;
		else
			newfh.flag2 &= ~NOFILTER_FLAG;
	}
	sprintf(buf, "±¾°åÊÇ·ñÇ¿ÖÆÄ£°å·¢ÎÄ (Y/N)? [%c]: ", (fh.flag2&TEMPLATE_FLAG) ? 'Y' : 'N');
	getdata(3, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= TEMPLATE_FLAG;
		else
			newfh.flag2 &= ~TEMPLATE_FLAG;
	}
	
		
	getdata(4, 0, "È·¶¨Òª¸ü¸ÄÂğ? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
	//}
	if (*genbuf != 'Y' && *genbuf != 'y')
	{
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "ĞŞ¸ÄÌÖÂÛÇø£º%s(%s)", fh.filename, newfh.filename);
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
					prints("\n³ÉÁ¢¾«»ªÇøÊ§°Ü....\n");
				else
					prints("ÒÑ¾­ÖÃÈë¾«»ªÇø...\n");
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
	sprintf(genbuf, "¸ü¸ÄÌÖÂÛÇø %s µÄ×ÊÁÏ --> %s", fh.filename, newfh.filename);
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
	stand_title("Çå³ıË½ÈËĞÅ¼ş");
	move(1, 0);
	prints("Çå³ıËùÓĞÒÑ¶ÁÇÒÎ´ mark µÄĞÅ¼ş\n");
	if (askyn("È·¶¨Âğ", NA, NA) == NA)
	{
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "Çå³ıËùÓĞÊ¹ÓÃÕßÒÑ¶ÁĞÅ¼ş¡£");
		securityreport(secu);
	}

	cleanlog = fopen("mailclean.log", "w");
	move(3, 0);
	prints("ÇëÄÍĞÄµÈºò.\n");
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
	prints("Çå³ıÍê³É! ¼ÇÂ¼µµ mailclean.log.\n");
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
		prints("Ä¿Ç°Éè¶¨:\n");
		trace_state(otflag, "Ò»°ã", ostatb.st_size);
		trace_state(ctflag, "ÁÄÌì", cstatb.st_size);
		move(9, 0);
		prints("<1> ÇĞ»»Ò»°ã¼ÇÂ¼\n");
		prints("<2> ÇĞ»»ÁÄÌì¼ÇÂ¼\n");
		getdata(12, 0, "ÇëÑ¡Ôñ (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);
 
		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "Ò»°ã¼ÇÂ¼ ON";
			} else {
				rename("trace", "trace.old");
				msg = "Ò»°ã¼ÇÂ¼ OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "ÁÄÌì¼ÇÂ¼ ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "ÁÄÌì¼ÇÂ¼ OFF";
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

	stand_title("Éè¶¨Ê¹ÓÃÕß×¢²á×ÊÁÏ");
	for (;;)
	{
		getdata(1, 0, "(0)Àë¿ª  (1)Éó²éĞÂ×¢²á×ÊÁÏ  (2)²éÑ¯Ê¹ÓÃÕß×¢²á×ÊÁÏ ? : ",
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
			prints("\n\nÄ¿Ç°²¢ÎŞĞÂ×¢²á×ÊÁÏ.");
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
			if (askyn("\33[37mÉè¶¨×ÊÁÏÂğ", NA, YEA) == YEA)
			{
				securityreport("Éè¶¨Ê¹ÓÃÕß×¢²á×ÊÁÏ");
				scan_register_form(fname);
			}
		}
	}
	else
	{
		move(1, 0);
		usercomplete("ÇëÊäÈëÒª²éÑ¯µÄ´úºÅ: ", uident);
		if (uident[0] != '\0')
			if (!getuser(uident))
			{
				move(2, 0);
				prints("´íÎóµÄÊ¹ÓÃÕß´úºÅ...");
			}
			else
			{
				sprintf(genbuf, "home/%c/%s/register", toupper(lookupuser.userid[0]), lookupuser.userid);
				if ((fn = fopen(genbuf, "r")) != NULL)
				{
					prints("\n×¢²á×ÊÁÏÈçÏÂ:\n\n");
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
					prints("\n\nÕÒ²»µ½Ëû/ËıµÄ×¢²á×ÊÁÏ!!\n");
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
        stand_title("É¾³ı×Ó·ÖÇø");
        make_blist();
        move(1, 0);
        namecomplete("ÇëÊäÈë×Ó·ÖÇø: ", bname);
        if (bname[0] == '\0')
                return 0;
        bid = getbnum(bname);
        if (get_record(BOARDS, &binfo, sizeof(binfo), bid) == -1 || !(binfo.flag & ZONE_FLAG))
        {
                move(2, 0);
                prints("²»ÕıÈ·µÄ×Ó·ÖÇø\n");
                pressreturn();
                clear();
                return 0;
        }
        if(count_zone(BOARDS,binfo.filename))
        {
                move(2,0);
                prints("×Ó·ÖÇøÄÚÓĞÌÖÂÛÇø£¬²»ÄÜÉ¾³ı\n");
                pressreturn();
                clear();
                return 0;
        }
        ans = askyn("ÄãÈ·¶¨ÒªÉ¾³ıÕâ¸ö×Ó·ÖÇø", NA, NA);
        if (ans != 1)
        {
                move(2, 0);
                prints("È¡ÏûÉ¾³ıĞĞ¶¯\n");
                pressreturn();
                clear();
                return 0;
        }
        {
                char    secu[STRLEN];
                sprintf(secu, "É¾³ı×Ó·ÖÇø£º%s", binfo.filename);
                securityreport(secu);
        }
        sprintf(genbuf, " << '%s' ±» %s É¾³ı >>",
                binfo.filename, currentuser.userid);
        memset(&binfo, 0, sizeof(binfo));
        strncpy(binfo.title, genbuf, STRLEN);
        binfo.level = PERM_SYSOP;
        substitute_record(BOARDS, &binfo, sizeof(binfo), bid);
        move(4, 0);
        prints("\n±¾×Ó·ÖÇøÒÑ¾­É¾³ı...\n");
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
	stand_title("É¾³ıÌÖÂÛÇø");
	make_blist();
	move(1, 0);
	namecomplete("ÇëÊäÈëÌÖÂÛÇø: ", bname);
	if (bname[0] == '\0')
		return 0;
	bid = getbnum(bname);
	if (get_record(BOARDS, &binfo, sizeof(binfo), bid) == -1 || binfo.flag & ZONE_FLAG)
	{
		move(2, 0);
		prints("²»ÕıÈ·µÄÌÖÂÛÇø\n");
		pressreturn();
		clear();
		return 0;
	}
	if(binfo.BM[0]!='\0'&&strcmp(binfo.BM,"SYSOPs"))
	{
		move(5,0);
		prints("¸Ã°å»¹ÓĞ°åÖ÷£¬ÔÚÉ¾³ı±¾°åÇ°£¬ÇëÏÈÈ¡Ïû°åÖ÷µÄÈÎÃü¡£\n");
		pressanykey();
		clear();
		return 0;
	}
	ans = askyn("ÄãÈ·¶¨ÒªÉ¾³ıÕâ¸öÌÖÂÛÇø", NA, NA);
	if (ans != 1)
	{
		move(2, 0);
		prints("È¡ÏûÉ¾³ıĞĞ¶¯\n");
		pressreturn();
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "É¾³ıÌÖÂÛÇø£º%s", binfo.filename);
		securityreport(secu);
	}
	if (seek_in_file("0Announce/.Search", bname))
	{
		move(4, 0);
		if (askyn("ÒÆ³ı¾«»ªÇø", NA, NA) == YEA)
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
	sprintf(genbuf, " << '%s' ±» %s É¾³ı >>",
	        binfo.filename, currentuser.userid);
	memset(&binfo, 0, sizeof(binfo));
	strncpy(binfo.title, genbuf, STRLEN);
	binfo.level = PERM_SYSOP;
	substitute_record(BOARDS, &binfo, sizeof(binfo), bid);

	move(4, 0);
	prints("\n±¾ÌÖÂÛÇøÒÑ¾­É¾³ı...\n");
	pressreturn();
	numboards = -1;
	clear();
	return 0;
}

int redhackercheck(char *s)
{

     return 0;
     if(!HAS_PERM(PERM_LOOKADMIN)) return 0;
     prints("ºì¿ÍÄãºÃ°¡!Ê×ÏÈ¸ĞĞ»ÄúµÄĞÁ¿àÀÍ¶¯.²»¹ı»¹ÊÇ²»ºÃÒâË¼,ÄúÃ»ÓĞ %s µÄÈ¨Àû.\nÈç¹ûÈ·ÊµĞèÒª,ÇëÓÚ±¾Õ¾Ïà¹Ø¹ÜÀíÈËÔ±ÁªÏµ.\n",s);
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
	if(redhackercheck("É¾³ıÕÊºÅ"))
           return ;

 	stand_title("É¾³ıÊ¹ÓÃÕßÕÊºÅ");
	move(2,0);
	if(!gettheuserid(1,"ÇëÊäÈëÓûÉ¾³ıµÄÊ¹ÓÃÕß´úºÅ: ",&id))
		return 0;
	if (!strcmp(lookupuser.userid, "SYSOP"))
	{
		prints("\n¶Ô²»Æğ£¬Äã²»¿ÉÒÔÉ¾³ı SYSOP ÕÊºÅ!!\n");
		pressreturn();
		clear();
		return 0;
	}
	if ((lookupuser.userlevel&PERM_SYSOP)||(lookupuser.userlevel&PERM_ACCOUNTS)||(lookupuser.userlevel&PERM_OBOARDS)||(lookupuser.userlevel&PERM_CHATCLOAK))
	{
		prints("\n¶Ô²»Æğ£¬ÇëÏÈÇå³ıÆäÕ¾ÎñÈ¨ÏŞ!!\n");
		pressreturn();
		clear();
		return 0;
	}
	if (!strcmp(lookupuser.userid, currentuser.userid))
	{
		prints("\n¶Ô²»Æğ£¬Äã²»¿ÉÒÔÉ¾³ı×Ô¼ºµÄÕâ¸öÕÊºÅ!!\n");
		pressreturn();
		clear();
		return 0;
	}
	prints("\n\nÒÔÏÂÊÇ [%s] µÄ²¿·Ö×ÊÁÏ:\n",lookupuser.userid);
	prints("    User ID:  [%s]\n",lookupuser.userid);
	prints("    êÇ   ³Æ:  [%s]\n",lookupuser.username);
	prints("    ĞÕ   Ãû:  [%s]\n",lookupuser.realname);
	strcpy(secu, "bTCPRD#@XWBA#VS-DOM-F0s2345678");
	for (num = 0; num < strlen(secu); num++)
		if (!(lookupuser.userlevel & (1 << num)))
			secu[num] = '-';
	prints("    È¨   ÏŞ: %s\n\n", secu);

	num = getbnames(lookupuser.userid,secu,&num);
	if(num)
	{
		prints("[%s] Ä¿Ç°ÉĞµ£ÈÎÁË %d ¸ö°åµÄ°åÖ÷: ",
		       lookupuser.userid, num);
		for(i = 0 ; i< num ; i ++ )
			prints("%s ",bnames[i]);
		prints("\nÇëÏÈÊ¹ÓÃ°åÖ÷Ğ¶Ö°¹¦ÄÜÈ¡ÏûÆä°åÖ÷Ö°ÎñÔÙ×ö¸Ã²Ù×÷.");
		pressanykey();
		clear();
		return 0;
	}

	sprintf(genbuf,"ÄãÈ·ÈÏÒªÉ¾³ı [%s] Õâ¸ö ID Âğ",lookupuser.userid);
	if (askyn(genbuf, NA, NA) == NA)
	{
		prints("\nÈ¡ÏûÉ¾³ıÊ¹ÓÃÕß...\n");
		pressreturn();
		clear();
		return 0;
	}
	sprintf(genbuf, "Òª°Ñ¸Ã [%s] Õâ¸öID¼ÓÈë²»¿É×¢²á30ÌìÂğ£¿", lookupuser.userid);
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
			prints("¼ÓÈë²»¿É×¢²áid³ö´í£¬ÇëÊÖ¹¤¼ÓÈë£¡");
		close(fileid);
	}
	sprintf(secu, "É¾³ıÊ¹ÓÃÕß£º%s", lookupuser.userid);

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
	prints("\n%s ÒÑ¾­±»Ãğ¾øÁË...\n", lookupuser.userid);
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
	prints("¸ü¸ÄÊ¹ÓÃÕßÈ¨ÏŞ\n");
	clrtoeol();
	move(1, 0);
	usercomplete("ÊäÈëÓû¸ü¸ÄµÄÊ¹ÓÃÕßÕÊºÅ: ", genbuf);
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
	prints("Éè¶¨Ê¹ÓÃÕß '%s' µÄÈ¨ÏŞ \n", genbuf);
	newlevel = setperms(lookupuser.userlevel, "È¨ÏŞ", NUMPERMS, showperminfo);
	move(2, 0);
	if (newlevel == lookupuser.userlevel)
		prints("Ê¹ÓÃÕß '%s' È¨ÏŞÃ»ÓĞ±ä¸ü\n", lookupuser.userid);
	else
	{

		//		lookupuser.userlevel = newlevel;
		{
			char    secu[STRLEN];
			sprintf(secu, "ĞŞ¸Ä %s µÄÈ¨ÏŞ", lookupuser.userid);
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
		 //Çå¿ÕË¿Â·
    	sethomefile(genbuf, lookupuser.userid, ".announcepath");
		if(dashf(genbuf))
			unlink(genbuf);
		prints("Ê¹ÓÃÕß '%s' È¨ÏŞÒÑ¾­¸ü¸ÄÍê±Ï.\n", lookupuser.userid);
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
	prints("¸ü¸ÄÊ¹ÓÃÕß»ù±¾È¨ÏŞ»ò×ÊÁÏ\n");
	clrtoeol();
	move(1, 0);
	usercomplete("ÊäÈëÓû¸ü¸ÄµÄÊ¹ÓÃÕßÕÊºÅ: ", genbuf);
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
		sprintf(secu, "ĞŞ¸Ä %s µÄ»ù±¾È¨ÏŞ»ò×ÊÁÏ", lookupuser.userid);
		securityreport1(secu);
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	prints("Éè¶¨Ê¹ÓÃÕß '%s' µÄ»ù±¾È¨ÏŞ»ò×ÊÁÏ \n\n", lookupuser.userid);
	prints("(1) ·â½û·¢±íÎÄÕÂÈ¨Á¦       (A) »Ö¸´·¢±íÎÄÕÂÈ¨Á¦\n");
	prints("(2) È¡Ïû»ù±¾ÉÏÕ¾È¨Á¦       (B) »Ö¸´»ù±¾ÉÏÕ¾È¨Á¦\n");
	prints("(3) ½ûÖ¹½øÈëÁÄÌìÊÒ         (C) »Ö¸´½øÈëÁÄÌìÊÒÈ¨Á¦\n");
	prints("(4) ½ûÖ¹ºô½ĞËûÈËÁÄÌì       (D) »Ö¸´ºô½ĞËûÈËÁÄÌìÈ¨Á¦\n");
	prints("(5) ½ûÖ¹¸øËûÈË·¢ÏûÏ¢       (E) »Ö¸´¸øËûÈË·¢ÏûÏ¢È¨Á¦\n");
	prints("(6) ĞŞ¸Ä %s »ù±¾×ÊÁÏ\n", lookupuser.userid);
	getdata(12, 0, "ÇëÊäÈëÄãµÄ´¦Àí: ", ans, 3, DOECHO, YEA);
	//tdhlshx modified.
	int flag=1;
	char day1[7];
	int day=0;
	char buf[30],str[24];
	while(1)
	{
		getdata(13,0,"ÇëÊäÈëËµÃ÷:",buf,29,DOECHO,YEA);
		if(killwordsp(buf)!=0)
			break;
	}
	switch(ans[0])
	{
	case '1':
		denycase=1;//loveni
		lookupuser.userlevel &= ~PERM_POST;
		getdata(14,0,"ÊäÈë·â½ûÊ±¼ä(ÒÔÌìÎªµ¥Î»): ",day1,6,DOECHO,YEA);
		strcpy(str,"±»·â½û·¢±íÎÄÕÂÈ¨Á¦\0");
		break;
	case 'a':
	case 'A':
		denycase=1;//loveni
		lookupuser.userlevel |= PERM_POST;
		flag=2;
		strcpy(str,"±»»Ö¸´·¢±íÎÄÕÂÈ¨Á¦\0");
		break;
	case '2':
		denycase=2;//loveni
		lookupuser.userlevel &= ~PERM_BASIC;
		getdata(14,0,"ÊäÈë·â½ûÊ±¼ä(ÒÔÌìÎªµ¥Î»): ",day1,6,DOECHO,YEA);
		strcpy(str,"±»È¡Ïû»ù±¾ÉÏÕ¾È¨Á¦\0");
		break;
	case 'b':
	case 'B':
		denycase=2;//loveni
		lookupuser.userlevel |= PERM_BASIC;
		flag=2;
		strcpy(str,"±»»Ö¸´»ù±¾ÉÏÕ¾È¨Á¦\0");
		break;
	case '3':
		denycase=3;//loveni
		lookupuser.userlevel &= ~PERM_CHAT;
		getdata(14,0,"ÊäÈë·â½ûÊ±¼ä: ",day1,6,DOECHO,YEA);
		strcpy(str,"±»½ûÖ¹½øÈëÁÄÌìÊÒ\0");
		break;
	case 'c':
	case 'C':
		denycase=3;//loveni
		lookupuser.userlevel |= PERM_CHAT;
		flag=2;
		strcpy(str,"±»»Ö¸´½øÈëÁÄÌìÊÒÈ¨Á¦\0");
		break;
	case '4':
		denycase=4;//loveni
		lookupuser.userlevel &= ~PERM_PAGE;
		getdata(14,0,"ÊäÈë·â½ûÊ±¼ä: ",day1,6,DOECHO,YEA);
		strcpy(str,"±»½ûÖ¹ºô½ĞËûÈËÁÄÌì\0");
		break;
	case 'd':
	case 'D':
		denycase=4;//loveni
		lookupuser.userlevel |= PERM_PAGE;
		flag=2;
		strcpy(str,"±»»Ö¸´ºô½ĞËûÈËÁÄÌìÈ¨Á¦\0");
		break;
	case '5':
		denycase=5;//loveni
		lookupuser.userlevel &= ~PERM_MESSAGE;
		getdata(14,0,"ÊäÈë·â½ûÊ±¼ä: ",day1,6,DOECHO,YEA);
		strcpy(str,"±»½ûÖ¹¸øËûÈË·¢ÏûÏ¢\0");
		break;
	case 'e':
	case 'E':
		denycase=5;//loveni
		lookupuser.userlevel |= PERM_MESSAGE;
		flag=2;
		strcpy(str,"±»»Ö¸´¸øËûÈË·¢ÏûÏ¢È¨Á¦\0");
		break;
	case '6':
		flag=0;
		uinfo_change1(12,&lookupuser,&lookupuser);
		break;
	default:
		flag=0;
		prints("\n Ê¹ÓÃÕß '%s' »ù±¾È¨Á¦ºÍ×ÊÁÏÃ»ÓĞ±ä¸ü\n",
		       lookupuser.userid);
		pressreturn();
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "ĞŞ¸Ä %s µÄ»ù±¾È¨ÏŞ»ò×ÊÁÏ", lookupuser.userid);
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
		        "\n  %s ÍøÓÑ: \n\n"
		        "\tºÜ±§Ç¸µÄ¸æËßÄú£¬ÄúÒÑ¾­%s \n\n"
		        "\tÄú±»È¡ÏûÈ¨Á¦µÄÔ­ÒòÊÇ: %s \n\n"
		        "\tÒòÎªÄúµÄĞĞÎª£¬¾ö¶¨Í£Ö¹ÄúµÄÈ¨Á¦ %d Ìì£¬\n\n"
		        "\t%d Ììºó£¬µ±ÄúµÇÂ¼Ê±£¬ÏµÍ³½«ÎªÄú×Ô¶¯½â·â¡£\n\n",
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
		        "\n  %s ÍøÓÑ: \n\n"
		        "\tÄúÒÑ¾­%s, \n\n"
		        "\t ¸½ÑÔ:%s \n\n"
		        "\t¸ĞĞ»ÄúµÄÖ§³Ö.\n\n"
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
	prints("\n Ê¹ÓÃÕß '%s' »ù±¾È¨ÏŞ»ò×ÊÁÏÒÑ¾­¸ü¸ÄÍê±Ï.\n", lookupuser.userid);
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
	    {"ÌØÊâ½øÕ¾¹«²¼À¸", "½øÕ¾»­Ãæ", "½øÕ¾»¶Ó­µµ", "½øÕ¾ÖØÒª¹«¸æ","ÀëÕ¾»­Ãæ"
	     ,"¹«ÓÃ±¸ÍüÂ¼", "²»¿É×¢²áµÄ ID", "²»¿ÉÈ·ÈÏÖ®E-Mail", "²»¿ÉÉÏÕ¾Ö®Î»Ö·"
	     ,"²»ËãPOSTÊıµÄ°å", "ÏµÍ³µ×²¿Á÷¶¯ĞÅÏ¢", "menu1.ini", "Ó©Ä»Ëø¶¨»­Ãæ","menu.ini", "Ã¿ÈÕ×Ô¶¯ËÍĞÅµµ", "¹ÜÀíÕßÃûµ¥", "¶¨Ê±¿³ĞÅÉè¶¨µµ",
	     "¼ÍÄîÈÕÇåµ¥", "ÔİÍ£µÇÂ½(NOLOGIN)", "ÔİÍ£×¢²á(NOREGISTER)", "×ªĞÅºÚÃûµ¥", "×¢²á³É¹¦ĞÅ¼ş","¸øĞÂÊÖµÄĞÅ","×¢²áÊ§°ÜĞÅ¼ş","°åÎñÊØÔòÒÔ¼°°åÃæ²Ù×÷ÊÖ²á","ĞÂÓÃ»§×¢²á·¶Àı","ÓÃ»§µÚÒ»´ÎµÇÂ½¹«¸æ","¹ú¼Ê»áÒéÌüÇåµ¥","BBSNET ×ªÕ¾Çåµ¥","BBSNET ×ªÕ¾Çåµ¥II","Çø¶ÎÉ¾³ı²»Ğè±¸·İÖ®Çåµ¥","WWW·½Ê½½øÕ¾¹«¸æ","FTPËÑË÷Õ¾µãÁĞ±í","Ç¿ÖÆÄ£°æ·¢ÎÄ°åÃæ","Ä¬ÈÏÊÕ²Ø¼Ğ°åÃæ",NULL};*/
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
	    {"ÌØÊâ½øÕ¾¹«²¼À¸", "½øÕ¾»­Ãæ", "½øÕ¾»¶Ó­µµ", "½øÕ¾ÖØÒª¹«¸æ",
	"ÀëÕ¾»­Ãæ" ,"¹«ÓÃ±¸ÍüÂ¼", "ÏµÍ³µ×²¿Á÷¶¯ĞÅÏ¢", "Ó©Ä»Ëø¶¨»­Ãæ",
	"WWW·½Ê½Õ¾ÄÚÍÆ¼ö","WWW·½Ê½Ğ£ÄÚÍÆ¼ö","²»¿É×¢²áµÄ ID","²»¿ÉÈ·ÈÏÖ®E-Mail", 
	"²»¿ÉÉÏÕ¾Ö®Î»Ö·" ,"ÈÈÃÅÌÖÂÛÇø", "ÍÆ¼öÌÖÂÛÇø",
	"Ä¬ÈÏÊÕ²Ø¼Ğ°åÃæ","Ê®´ó°åÃæºÚÃûµ¥","Ê®´ó±êÌâºÚÃûµ¥",
	"Ö¸¶¨Ê®´óÎÄÕÂ","Ö¸¶¨Ê®´ó×£¸£ÎÄÕÂ","±à¼­Ê®´ó","±à¼­Ê®´ó×£¸£","Ğ£ÄÚ¹«¸æ",
	"Õ¾ÄÚ¹«¸æ","SÈ¨ÏŞ²»¿É½øÈë°åÃæ","ÄÚÍøµØÖ·Éè¶¨","¹ÜÀíÕßÃûµ¥","menu1.ini", 
	"menu.ini","ÔİÍ£µÇÂ½(NOLOGIN)", "ÔİÍ£×¢²á(NOREGISTER)",
	"È«Õ¾Ö»¶Á(NOPOST)", "ÔİÍ£¸½¼şÉÏ´«", "¹ÒÕ¾Ë«±¶»ı·Ö",NULL};

/*

"menu1.ini", "menu.ini", "Ã¿ÈÕ×Ô¶¯ËÍĞÅµµ",  "¶¨Ê±¿³ĞÅÉè¶¨µµ",
	     "¼ÍÄîÈÕÇåµ¥", "ÔİÍ£µÇÂ½(NOLOGIN)", "ÔİÍ£×¢²á(NOREGISTER)", "×ªĞÅºÚÃûµ¥", "×¢²á³É¹¦ĞÅ¼ş","¸øĞÂÊÖµÄĞÅ","×¢²áÊ§°ÜĞÅ¼ş","°åÎñÊØÔòÒÔ¼°°åÃæ²Ù×÷ÊÖ²á","ĞÂÓÃ»§×¢²á·¶Àı","ÓÃ»§µÚÒ»´ÎµÇÂ½¹«¸æ","¹ú¼Ê»áÒéÌüÇåµ¥","BBSNET ×ªÕ¾Çåµ¥","BBSNET ×ªÕ¾Çåµ¥II","FTPËÑË÷Õ¾µãÁĞ±í",NULL};*/
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(1, 0);
	prints("±àĞŞÏµÍ³µµ°¸\n\n");
	for (num = 0;  e_file[num] != NULL && explain_file[num] != NULL ; num++){
              
  		prints("[[1;32m%2d[m] %s", num + 1, explain_file[num]);
		if(num < 17 )
			move(4+num,0);
		else
			move(num - 14, 50);
	}
	prints("[[1;32m%2d[m] ¶¼²»Ïë¸Ä\n", num + 1);

	getdata(23, 0, "ÄãÒª±àĞŞÄÄÒ»ÏîÏµÍ³µµ°¸: ", ans, 3, DOECHO, YEA);
	ch = atoi(ans);
	if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
		return;
	ch -= 1;
        

        if((ch>=0&&ch<8)&&!HAS_PERM(PERM_ACBOARD)&&!HAS_PERM(PERM_SYSOP))
	{
             clear();
	     prints("ÄúÃ»ÓĞÈ¨ÏŞĞŞ¸Ä±¾Ïîµµ°¸,ÇëÈ·ÈÏÄúÓĞSYSOP»òACBOARDÈ¨ÏŞ");
             pressanykey();
	     return;
	}
	if((ch==8||ch==9||ch==27)&&!HAS_PERM(PERM_WELCOME)&&!HAS_PERM(PERM_SYSOP))
	{
             clear();
	     prints("ÄúÃ»ÓĞÈ¨ÏŞĞŞ¸Ä±¾Ïîµµ°¸,ÇëÈ·ÈÏÄúÓĞSYSOP»òWELCOMEÈ¨ÏŞ");
             pressanykey();
	     return;
	}
	

        if((ch==10||ch==11||ch==12|| ch == 33)&&!HAS_PERM(PERM_ACCOUNTS)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("ÄúÃ»ÓĞÈ¨ÏŞĞŞ¸Ä±¾Ïîµµ°¸,ÇëÈ·ÈÏÄúÓĞSYSOP»òACCOUNTSÈ¨ÏŞ");
             pressanykey();
             return;

        }
	if((ch==13||ch==14)&&!HAS_PERM(PERM_OBOARDS)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("ÄúÃ»ÓĞÈ¨ÏŞĞŞ¸Ä±¾Ïîµµ°¸,ÇëÈ·ÈÏÄúÓĞSYSOP»òOBOARDSÈ¨ÏŞ");
             pressanykey();
             return;

    }
	if((ch>23&&ch<32&&ch!=27)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("ÄúÃ»ÓĞÈ¨ÏŞĞŞ¸Ä±¾Ïîµµ°¸,ÇëÈ·ÈÏÄúÓĞSYSOPÈ¨ÏŞ");
             pressanykey();
             return;

    }
	if((ch==32)&&!HAS_PERM(PERM_ANNOUNCE)&&!HAS_PERM(PERM_SYSOP))
	{
		clear();
		prints("ÄúÃ»ÓĞÈ¨ÏŞĞŞ¸Ä±¾Ïîµµ°¸,ÇëÈ·ÈÏÄúÓĞSYSOP»òANNOUNCEÈ¨ÏŞ");
		pressanykey();
		return;
	}

	sprintf(buf2, "etc/%s", e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)±à¼­ (D)É¾³ı %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"ÄãÈ·¶¨ÒªÉ¾³ı %s Õâ¸öÏµÍ³µµ",explain_file[ch]);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("È¡ÏûÉ¾³ıĞĞ¶¯\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "É¾³ıÏµÍ³µµ°¸£º%s", explain_file[ch]);
			securityreport(secu);
		}
		unlink(buf2);
		move(5, 0);
		prints("%s ÒÑÉ¾³ı\n", explain_file[ch]);
		pressreturn();
		clear();
		return;
	}
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf2, NA, YEA); /* ²»Ìí¼ÓÎÄ¼şÍ·, ÔÊĞíĞŞ¸ÄÍ·²¿ĞÅÏ¢ */
	clear();
	if (aborted != -1)
	{
		prints("%s ¸üĞÂ¹ı", explain_file[ch]);
		{
			char    secu[STRLEN];
			sprintf(secu, "ĞŞ¸ÄÏµÍ³µµ°¸£º%s", explain_file[ch]);
			securityreport(secu);
		}

		if (!strcmp(e_file[ch], "../Welcome"))
		{
			unlink("Welcome.rec");
			prints("\nWelcome ¼ÇÂ¼µµ¸üĞÂ");
		}
		else if(!strcmp(e_file[ch],"whatdate"))
		{
			brdshm->fresh_date = time(0);
			prints("\n¼ÍÄîÈÕÇåµ¥ ¸üĞÂ");
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
	    {"¾«»ªÇøÊ¹ÓÃËµÃ÷", "ÌÖÂÛÇøÊ¹ÓÃËµÃ÷", "ÁÄÌìÊÒÊ¹ÓÃËµÃ÷", "ÁÄÌìÊÒ¹ÜÀíÔ±Ö¸Áî",
	     "±à¼­¹¦ÄÜÖ¸ÁîËµÃ÷", "ºÃÓÑÑ¡µ¥¹¦ÄÜ¼ü", "ĞÅ¼ş·¢ËÍÊ§°ÜĞÅÏ¢", "ÓÊ¼şÑ¡µ¥Ê¹ÓÃËµÃ÷",
	     "¶à¹¦ÄÜÔÄ¶ÁÑ¡µ¥ËµÃ÷", "ÔÄ¶ÁÎÄÕÂ¹¦ÄÜ¼ü", "Ñ¶Ï¢ä¯ÀÀÆ÷Ê¹ÓÃËµÃ÷", "»µÈËÑ¡µ¥¹¦ÄÜ¼ü",
	     "Ê¹ÓÃÕßÉÏÏßÑ¡µ¥Ê¹ÓÃËµÃ÷", "×¢²áÊ¹ÓÃÕßÑ¡µ¥ËµÃ÷", "¾«»ªÇøÊÕ²ØÑ¡µ¥Ê¹ÓÃËµÃ÷", "ÉúÈÕºØĞÅ","ÍâÍø·¢ÎÄÓÃ»§","ÄÚÍøµØÖ·Éè¶¨","Ê®´ó°åÃæºÚÃûµ¥","Ê®´ó±êÌâºÚÃûµ¥","±à¼­Ê®´ó","Ğ£ÄÚ¹«¸æ","Õ¾ÄÚ¹«¸æ",NULL};*/
	static char *e_file[] =
	    {"help/announcereadhelp", "help/boardreadhelp", "help/chathelp", "help/chatophelp",
	     "help/edithelp", "help/friendshelp", "help/mailerror-explain", "help/mailreadhelp",
	     "help/mainreadhelp", "help/morehelp", "help/msghelp", "help/rejectshelp",
	     "help/serlisthelp", "help/usershelp", "help/favdigesthelp","etc/s_fill","etc/fornewcomers", "etc/f_fill", "etc/forbm","etc/register", "etc/firstlogin","ftp/FTPFileList","etc/chatstation","etc/bbsnet.ini","etc/bbsnet1.ini",".blockmail","etc/whatdate","etc/birthday","etc/expire.ctl", "etc/autopost","etc/BMCLOAK","etc/WarningBM","etc/specbm",NULL};

	static char *explain_file[] =
	    {"¾«»ªÇøÊ¹ÓÃËµÃ÷", "ÌÖÂÛÇøÊ¹ÓÃËµÃ÷", "ÁÄÌìÊÒÊ¹ÓÃËµÃ÷", "ÁÄÌìÊÒ¹ÜÀíÔ±Ö¸Áî",
	     "±à¼­¹¦ÄÜÖ¸ÁîËµÃ÷", "ºÃÓÑÑ¡µ¥¹¦ÄÜ¼ü", "ĞÅ¼ş·¢ËÍÊ§°ÜĞÅÏ¢", "ÓÊ¼şÑ¡µ¥Ê¹ÓÃËµÃ÷",
	     "¶à¹¦ÄÜÔÄ¶ÁÑ¡µ¥ËµÃ÷", "ÔÄ¶ÁÎÄÕÂ¹¦ÄÜ¼ü", "Ñ¶Ï¢ä¯ÀÀÆ÷Ê¹ÓÃËµÃ÷", "»µÈËÑ¡µ¥¹¦ÄÜ¼ü",
	     "Ê¹ÓÃÕßÉÏÏßÑ¡µ¥Ê¹ÓÃËµÃ÷", "×¢²áÊ¹ÓÃÕßÑ¡µ¥ËµÃ÷", "¾«»ªÇøÊÕ²ØÑ¡µ¥Ê¹ÓÃËµÃ÷", "×¢²á³É¹¦ĞÅ¼ş","¸øĞÂÊÖµÄĞÅ","×¢²áÊ§°ÜĞÅ¼ş","°åÎñÊØÔòÒÔ¼°°åÃæ²Ù×÷ÊÖ²á","ĞÂÓÃ»§×¢²á·¶Àı","ÓÃ»§µÚÒ»´ÎµÇÂ½¹«¸æ","FTPËÑË÷Õ¾µãÁĞ±í","¹ú¼Ê»áÒéÌüÇåµ¥","BBSNET ×ªÕ¾Çåµ¥","BBSNET ×ªÕ¾Çåµ¥II", "×ªĞÅºÚÃûµ¥","¼ÍÄîÈÕÇåµ¥","ÉúÈÕºØĞÅ","¶¨Ê±¿³ĞÅÉè¶¨µµ","Ã¿ÈÕ×Ô¶¯ËÍĞÅµµ","°åÎñÒşÉíÉÏÕ¾¿¼ºË","È¡Ïû°åÎñÒşÉíÈ¨ÏŞÇ°¾¯¸æ","²»¿¼ºËÒşÉíÉÏÕ¾°åÎñÖ®Ãûµ¥",NULL};
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(1, 0);
	prints("±àĞŞÏµÍ³µµ°¸2\n\n");
	/* Efan: ´Ë´¦ÓĞbug£¬ÒòÎªÊÇ°ïÖúĞÅÏ¢£¬ËùÒÔ²»»áÓĞ"menu.ini"³öÏÖ
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
	prints("[[1;32m%2d[m] ¶¼²»Ïë¸Ä\n", num + 1);

	getdata(23, 0, "ÄãÒª±àĞŞÄÄÒ»ÏîÏµÍ³µµ°¸: ", ans, 3, DOECHO, YEA);
	ch = atoi(ans);
	
	if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
		return;
	ch -= 1;
	if((ch==30||ch==31||ch==32)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("ÄúÃ»ÓĞÈ¨ÏŞĞŞ¸Ä±¾Ïîµµ°¸,ÇëÈ·ÈÏÄúÓĞSYSOPÈ¨ÏŞ");
             pressanykey();
             return;

        }
	sprintf(buf2, "%s", e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)±à¼­ (D)É¾³ı %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"ÄãÈ·¶¨ÒªÉ¾³ı %s ",explain_file[ch]);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("È¡ÏûÉ¾³ıĞĞ¶¯\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "É¾³ıÏµÍ³µµ°¸2£º%s", explain_file[ch]);
			securityreport(secu);
		}
		unlink(buf2);
		move(5, 0);
		prints("%s ÒÑÉ¾³ı\n", explain_file[ch]);
		pressreturn();
		clear();
		return;
	}
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf2, NA, YEA); /* ²»Ìí¼ÓÎÄ¼şÍ·, ÔÊĞíĞŞ¸ÄÍ·²¿ĞÅÏ¢ */
	clear();
	if (aborted != -1)
	{
		prints("%s ¸üĞÂ¹ı", explain_file[ch]);
		{
			char    secu[STRLEN];
			sprintf(secu, "±àĞŞÏµÍ³µµ°¸2£º%s", explain_file[ch]);
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
	if (!get_msg("ËùÓĞÊ¹ÓÃÕß", buf2, 1))
	{
		return 0;
	}

        securityreport(buf2);

	if (apply_ulist(dowall) == -1)
	{
		move(2, 0);
		prints("ÏßÉÏ¿ÕÎŞÒ»ÈË\n");
		pressanykey();
	}
	prints("\nÒÑ¾­¹ã²¥Íê±Ï...\n");
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
		prints("¶Ô²»Æğ£¬ÏµÍ³ÃÜÂëÖ»ÄÜÓÉ SYSOP ĞŞ¸Ä£¡");
		pressanykey();
		return;
	}
	getdata(2, 0, "ÇëÊäÈëĞÂµÄÏµÍ³ÃÜÂë(Ö±½Ó»Ø³µÔòÈ¡ÏûÏµÍ³ÃÜÂë): ",
	        passbuf, 19, NOECHO, YEA);
	if(passbuf[0] == '\0')
	{
		if( askyn("ÄãÈ·¶¨ÒªÈ¡ÏûÏµÍ³ÃÜÂëÂğ?",NA,NA)== YEA )
		{
			unlink("etc/.syspasswd");
			securityreport("[32mÈ¡ÏûÏµÍ³ÃÜÂë[37m");
		}
		return ;
	}
	getdata(3, 0, "È·ÈÏĞÂµÄÏµÍ³ÃÜÂë: ", prepass, 19, NOECHO, YEA);
	if (strcmp(passbuf, prepass))
	{
		move(4,0);
		prints("Á½´ÎÃÜÂë²»ÏàÍ¬, È¡Ïû´Ë´ÎÉè¶¨.");
		pressanykey();
		return;
	}
	if ((pass = fopen("etc/.syspasswd", "w")) == NULL)
	{
		move(4, 0);
		prints("ÏµÍ³ÃÜÂëÎŞ·¨Éè¶¨....");
		pressanykey();
		return;
	}
	fprintf(pass, "%s\n", genpasswd(passbuf));
	fclose(pass);
	move(4, 0);
	prints("ÏµÍ³ÃÜÂëÉè¶¨Íê³É....");
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
	getdata(1, 0, "ÇëÊäÈëÍ¨ĞĞ°µºÅ: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
	{
		prints("\n\n°µºÅ²»ÕıÈ·, ²»ÄÜÖ´ĞĞ¡£\n");
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
		prints("\n\nKey ²»ÕıÈ·, ²»ÄÜÖ´ĞĞ¡£\n");
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
		if( askyn("ÄãÈ·¶¨Ê¹ÏµÍ³½øÈëÖ»¶Á×´Ì¬Âğ?",NA,NA)== YEA )
		{
			system("touch NOPOST");
			prints("ÏµÍ³½øÈëÖ»¶Á×´Ì¬¡­¡­");
		}
	}
	else
	{
		if( askyn("ÄãÈ·¶¨Ê¹ÏµÍ³ÍË³öÖ»¶Á×´Ì¬Âğ?",NA,NA)== YEA )
		{
			system("mv NOPOST NOPOST.no");
			prints("ÏµÍ³ÍË³öÖ»¶Á×´Ì¬¡­¡­");
		}
	}
	pressanykey();
	return 1;
}

/* Ãô¸Ğ×Ö±à¼­ */

int keywords()
{
	char buf[256];
	int aborted;
	strcpy(buf, "etc/.badwords");
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf, NA, YEA);// ²»Ìí¼ÓÎÄ¼şÍ·, ÔÊĞíĞŞ¸ÄÍ·²¿ĞÅÏ¢
	if(aborted!=-1)
		securityreport("Ãô¸Ğ×Ö±à¼­");
	return 1;
}
/*loveni:Ë¢ĞÂÊ®´ó*/
int renewtopten()
{
	clear();
	if( askyn("ÄãÈ·¶¨Á¢¼´Ë¢ĞÂÊ®´ó?",NA,NA)== YEA )
	{
		system("/home/bbs/bin/poststat /home/bbs");
		prints("Ê®´óÒÑ¾­Ë¢ĞÂ!");
	}
	pressanykey();
	return 1;
}

int renewweb()
{
	clear();
	if( askyn("ÄãÈ·¶¨ÒªÁ¢¼´Ë¢ĞÂWebÊ×Ò³?",NA,NA)==YEA)
	{
		system("/home/www/cgi-bin/bbs/bbsmainbuf > /dev/null");
		prints("WebÊ×Ò³ÒÑ¾­Ë¢ĞÂ!");
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
	stand_title("ÉèÖÃ°å¹æ\n");
	clrtoeol();
	if(!gettheboardname(1,"ÊäÈëĞèÒªÉèÖÃ°å¹æµÄÌÖÂÛÇøÃû³Æ: ",&pos,&fh,bname))
		return -1;
	sprintf(fname,"boards/%s/boardrule",fh.filename);
	sprintf(buf, "(E)±à¼­ (D)É¾³ı %s°å°å¹æ? [E]: ", fh.filename);
	getdata(2, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"ÄãÈ·¶¨ÒªÉ¾³ı %s°å°å¹æ?",fh.filename);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("È¡ÏûÉ¾³ıĞĞ¶¯\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "É¾³ı%s°å°å¹æ", fh.filename);
			securityreport(secu);
		}
		unlink(fname);
		move(5, 0);
		prints("%s°å°å¹æÒÑÉ¾³ı\n", fh.filename);
		pressreturn();
		clear();
		return;
	}
	aborted = vedit(fname, NA, YEA);
	if(aborted!=-1)
	{
		sprintf(buf,"±à¼­%s°åµÄ°å¹æ",fh.filename);
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
	stand_title("²éÑ¯ÓÃ»§ÈÏÖ¤ÓÊÏä:");
	getdata (2, 0, "ÇëÊäÈëÓÃ»§id: ", username, 13, DOECHO, YEA);
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
					sprintf(genbuf,"ÓÃ»§id:%s\n",userid);
					prints(genbuf);
					sprintf(genbuf,"ÓÃ»§ÈÏÖ¤Email:%s\n",email);
					prints(genbuf);
					sprintf(genbuf,"ÓÃ»§ÈÏÖ¤ip:%s\n",ip);
					prints(genbuf);
				}
			}
		}
	}
	if(find==0)
	{
		move(2,0);
		clrtobot ();
		prints("Ã»ÓĞÕÒµ½¸ÃÓÃ»§ĞÅÏ¢");
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
	stand_title("²éÑ¯ÓÊÏäÈÏÖ¤µÄÓÃ»§:");
	getdata (2, 0, "ÇëÊäÈëÓÊÏä:", address, 80, DOECHO, YEA);
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
				sprintf(genbuf,"ÓÃ»§id:%s\n",userid);
				prints(genbuf);
	 			sprintf(genbuf,"ÓÃ»§ÈÏÖ¤ip:%s\n\n",ip);
				prints(genbuf);
			}
		}
	}
	if(find==0)
	{
		move(2,0);
		clrtobot ();
		prints("Ã»ÓĞÕÒµ½¸ÃÓÊÏäĞÅÏ¢");
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
	stand_title("ÊÚÓèÓÃ»§ÓÀ6ÕÊºÅ");
	if (!gettheuserid(1, "ÇëÊäÈëÊ¹ÓÃÕß´úºÅ: ", &id))	return -1;
	
	
	sprintf(fname, "home/%c/%s/life666", toupper(lookupuser.userid[0]), lookupuser.userid);
	move(4, 0);
	clrtobot();
	
	
	if (!dashf(fname))
	{
		if (askyn("¸ÃÓÃ»§ÉĞÎ´±»ÊÚÓèÓÀ6ÕÊºÅ£¬ÊÇ·ñÊÚÓè?", NA, NA) == YEA)
		{
			fp = fopen(fname, "w+");
			if (fp)
			{
				fprintf(fp, "%s", currentuser.userid);
				fclose(fp);
				char secu[STRLEN];
                     	        sprintf(secu, "ÊÚÓè %s ÓÀ6ÕÊºÅ", lookupuser.userid);
                                security_report(secu,1);
				prints("ÒÑ¾­ÊÚÓè¸ÃÓÃ»§ÓÀ6ÕÊºÅ");
			}
			else
				prints("´´½¨¿ØÖÆÎÄ¼ş´íÎó");
		}
	}
	else
	{
		if (askyn("¸ÃÓÃ»§ÒÑ¾­±»ÊÚÓèÓÀ6ÕÊºÅ£¬ÊÇ·ñÈ¡Ïû?", NA, NA) == YEA)
		{
			unlink(fname);
           		char secu[STRLEN];
                        sprintf(secu, "ÊÕ»Ø %s ÓÀ6ÕÊºÅ", lookupuser.userid);
                        security_report(secu,1);

			prints("ÒÑ¾­È¡Ïû¸ÃÓÃ»§ÓÀ6ÕÊºÅ");
		}
	}
	pressreturn();
	return 0;
}
