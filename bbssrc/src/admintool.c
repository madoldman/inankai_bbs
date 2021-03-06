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
					"离任站务奖章",
					"原创写手奖章",
					"优秀板务奖章",
					"热心网友奖章",
					"抓虫能手奖章",
					"离任仲裁奖章",
					"特殊贡献奖章",
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
	stand_title ("奖励功臣!");
	if (!gettheuserid (1, "请输入使用者代号(直接回车取消操作)：", &id) )
		return -1;
	clrtobot ();
	memcpy (& uinfo, & lookupuser, sizeof (uinfo) );
	move (4, 0);
	prints ("用户 %s 拥有奖章情况：\n", uinfo.userid);
	num = uinfo.nummedals;
	for (i = 7; i > 0; --i) {
		nummedal [7 - i] = (num >> (i << 2)) & 0x0f;
		prints ("%30s %d 枚\n", medaltype [7 - i], nummedal [7 - i]);
	}
	move (13, 0);
	prints ("     A.离任站务  B.原创写手  C.优秀板务  D.热心网友\n");
	prints ("     E.抓虫能手  F.离任仲裁  G.特殊贡献  H.撤销奖章\n");
	getdata (16, 0, "请输入奖章类型: ", buf, 2, DOECHO, YEA);
	if (buf [0] == 0) {
		move (17, 0);
		prints ("操作取消");
		pressreturn ();
		continue;
	}
	type = toupper (buf [0]) - 'A';
	if (type < 0 || type > 7) {
		move (17, 0);
		prints ("非法输入");
		pressreturn ();
		continue;
	}
	if (type == 7) {
		sprintf (show, "撤销 \033[1;32m%s\033[m 网友的所有奖章，确定吗？[y/N]", uinfo.userid);
		getdata (17, 0, show, buf, 10, DOECHO, YEA);
		if (buf [0] == 'y' || buf [0] == 'Y')
		{
			char bufsys [100];
			uinfo.nummedals = 0;
			if (uinfo.nummedals > 0x8000)
				uinfo.nummedals =0;
			substitute_record (PASSFILE, & uinfo, sizeof (uinfo), id);
			sprintf (bufsys, "撤销 %s 所有奖章", uinfo.userid);
			securityreport (bufsys);
			prints ("操作成功！");
			pressreturn ();
			continue;
		}
		else {
			prints ("操作取消");
			pressreturn ();
			continue;
		}
	}
	move (17, 0);
	clrtobot ();
	move (17, 0);
	sprintf (genbuf, "授予奖章类型: \033[1;32m%s\033[m", medaltype [type]);
	prints (genbuf);
	getdata (18, 0, "授予多少枚勋章？", buf, 10, DOECHO, YEA);
	if (buf [0] != 0) {
		num = atoi (buf);
		if (num > 15) {
			prints ("错误!!请输入 0--15 的数!!");
			pressreturn ();
			continue;
		}
	}
	sprintf (show, "授予 \033[1;32m%s\033[m 网友 \033[1;32m%d\033[m 枚 \033[1;32m%s\033[m，确定吗？[y/N]", uinfo.userid, num, medaltype [type]);
	getdata (19, 0, show, buf, 10, DOECHO, YEA);
	if (buf [0] == 'y' || buf [0] == 'Y')
	{
		char bufsys [100];

		sprintf (bufsys, "授予 %s %s %d 枚", uinfo.userid, medaltype [type], num);
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
		prints ("操作成功！");
		pressreturn ();
		continue;
	}
	prints ("操作取消");
	pressreturn ();

}  // Efan: while loop
#endif

	return 0;
}
/* add by livebird 解除归隐 */
int un_Reclusion()
{
        int id;
        modify_user_mode(ADMIN);
        if (!check_systempasswd())
        {
                return;
        }
        clear();
        prints("解除使用者归隐状态\n");
        clrtoeol();
        move(1, 0);
        usercomplete("输入欲更改的使用者帐号: ", genbuf);
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
        if( askyn("你确定要解除其归隐状态吗?",NA,NA)== NA )
        {
                 prints("\n使用者 '%s' 没有被解除归隐状态。\n",lookupuser.userid);
                 pressreturn();
                 clear();
        }
        else
        {
                if((lookupuser.userlevel & PERM_RECLUSION ) == 0)
                {
                        prints("\n使用者 '%s' 没有归隐。\n", lookupuser.userid);
                        pressreturn();
                        clear();
                }
                else
                {       char    secu[STRLEN];
                        sprintf(secu, "解除 %s 的归隐状态", lookupuser.userid);
                        //securityreport1(secu);

                        lookupuser.userlevel &= ~PERM_RECLUSION;

                        security_report(secu,1);
                        substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
                        prints("\n使用者 '%s' 已经被解除归隐状态。\n", lookupuser.userid);
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
	stand_title("修改使用者资料");
	if(!gettheuserid(1,"请输入使用者代号: ",&id))
		return -1;
	memcpy(&uinfo, &lookupuser, sizeof(uinfo));

	move(1, 0);
	clrtobot();
	disply_userinfo(&uinfo);
	uinfo_query(&uinfo, 1, id);
	return 0;
}

/* add by yiyo 允许校方管理帐号查看用户资料，但不能修改 */
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
        if(redhackercheck("查询网友资料")) return;
	stand_title("查询使用者资料");
	if(!gettheuserid(1,"请输入使用者代号: ",&id))
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
	stand_title("任命板主\n");
	clrtoeol();
	if (!gettheuserid(2,"输入欲任命的使用者帐号: ",&id))
		return 0;
	if(!gettheboardname(3,"输入该使用者将管理的讨论区名称: ",&pos,&fh,bname))
		return -1;
	if (is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("你不能任命子分区区长");
                pressanykey();                 
		clear();
                return -1;
        }
	if( fh.BM[0] != '\0' )
	{
		if ( !strncmp(fh.BM,"SYSOPs",6) )
		{
			move(4, 0);
			if(askyn("该讨论区板主是 SYSOPs, 你确定该板需要板主",NA,NA)==NA)
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
				//	    prints("%s 讨论区已有三名板主",bname);
				prints("%s 讨论区已有四名板主",bname);
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
		prints("你不能任命 guest 当板主");
		pressanykey();
		clear();
		return -1;
	}
        if(strlen(fh.BM)+strlen(lookupuser.userid)>BM_LEN){
                move(5,0);
		prints("郁闷,这个家伙的id太长，系统要溢出了。。。");
                pressanykey();
                return -1;
	}

	oldbm = getbnames(lookupuser.userid,bname,&find);
	if( find || (oldbm  == 4 && strcmp (lookupuser.userid, "SYSOP")))
	{
		move ( 5,0);
		prints(" %s 已经是%s板的板主了",lookupuser.userid,find?"该":"四个");
		pressanykey();
		clear();
		return -1;
	}
	prints("\n你将任命 %s 为 %s 板板%s.\n",lookupuser.userid,bname,bm?"主":"副");
	if( askyn("你确定要任命吗?",NA,NA)== NA )
	{
		prints("取消任命板主");
		pressanykey();
		clear();
		return -1;
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(8,0);
	prints("请输入任命附言(最多五行，按 Enter 结束)");
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
		sprintf(secu, "板主任命, 给予 %s 的板主权限", lookupuser.userid);
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
	//   sprintf(bmfilename, "[公告]任命 %s 为 %s 讨论区%s",
	sprintf(bmfilename, "[公告]任命 %s 为 %s 板%s",
	        lookupuser.userid,fh.filename,bm?"板主":"板副");
	securityreport(bmfilename);
	//move(16,0);
	//prints(bmfilename);

	/*  livebird 11.22.2004 */

	move(16,0);
	if( askyn("你要使用板校板务任命公告吗?",NA,NA)== NA )
		sprintf(genbuf,"\n\t\t\t【 公告 】\n\n"
	        	"\t任命 %s 为 %s 板%s！\n"
	        	"\t请 %s 在三天内到相应区务管理板报到。\n"
	        	"\t逾期未能报到者，将取消板主权限。\n"
			"\t实习期一个月，请广大网友监督。\n",
	        	lookupuser.userid,bname,bm?"板主":"板副",lookupuser.userid);
	else
		sprintf(genbuf,"\n\t\t\t【 公告 】\n\n"
			"\t任命 %s 为 %s 板%s！\n"
			"\t请 %s 到板校培训，培训内容在该板置底。\n"
			"\t培训期为三天，逾期取消培训资格。\n",
			lookupuser.userid,bname,bm?"板主":"板副",lookupuser.userid);
	/* end */
	move(18,0);
	prints(bmfilename);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\n\n任命附言：\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(currboard,bname);
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	//tdhlshx
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"恭喜您被任命为%s板的板务!",bname);
	sprintf(tmpfile,"tmp/committbm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,创建临时文件错误.请与系统维护联系或再次尝试!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);

	mail_file(tmpfile,lookupuser.userid,buftitle);
	prints("\n\n任命书发出!");
	pressanykey();
	char bmfaqtitle[STRLEN];
	sprintf(bmfaqtitle,"板务守则以及板务操作手册");
	mail_file("/home/bbs/etc/forbm",lookupuser.userid,bmfaqtitle);
#ifdef ORDAINBM_POST_BOARDNAME

	strcpy(currboard,ORDAINBM_POST_BOARDNAME);
	autoreport(bmfilename,genbuf,YEA,NULL);
#endif

	pressanykey();
	unlink(tmpfile);
	return 0;
}
/*smalldog任命子分区区长*/
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
	stand_title("任命子分区区长\n");
	clrtoeol();
	if (!gettheuserid(2,"输入欲任命的使用者帐号: ",&id))
		return 0;
	if(!gettheboardname(3,"输入该使用者将管理的子分区名称: ",&pos,&fh,bname))
		return -1;
	if (!is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("你不能在任命区长的时候却任命板面板主");
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
			prints("%s 子分区已有四名区长",bname);
			pressreturn();
			clear();
			return -1;
		}
		bm = 0;
	}
	if (!strcmp(lookupuser.userid,"guest"))
	{
		move ( 5,0);
		prints("你不能任命 guest 当子分区区长");
		pressanykey();
		clear();
		return -1;
	}
        if(strlen(fh.BM)+strlen(lookupuser.userid)>BM_LEN)
	{
                move(5,0);
		prints("郁闷,这个家伙的id太长，系统要溢出了。。。");
                pressanykey();
                return -1;
	}
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if( find || (oldbm  == 4 && strcmp (lookupuser.userid, "SYSOP")))
	{
		move ( 5,0);
		prints(" %s 已经是%s板的板主了",lookupuser.userid,find?"该":"四个");
		pressanykey();
		clear();
		return -1;
	}
	prints("\n你将任命 %s 为 %s 子分区区长.\n",lookupuser.userid,bname);
	if( askyn("你确定要任命吗?",NA,NA)== NA )
	{
		prints("取消任命子分区区长");
		pressanykey();
		clear();
		return -1;
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(8,0);
	prints("请输入任命附言(最多五行，按 Enter 结束)");
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
		sprintf(secu, "子分区区长任命, 给予 %s 的相应权限", lookupuser.userid);
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
	sprintf(bmfilename, "[公告]任命 %s 为 %s 子分区区长",lookupuser.userid,fh.filename);
	securityreport(bmfilename);
	sprintf(genbuf,"\n\t\t\t【 公告 】\n\n"
        	       "\t任命 %s 为 %s 子分区区长！\n"
        	       "\t请 %s 在三天内到相应区务管理板报到。\n"
                       "\t逾期未能报到者，将取消板主权限\n"
		       "\t实习期一个月，请广大网友监督。\n",
                       lookupuser.userid,bname,lookupuser.userid);

	move(18,0);
	prints(bmfilename);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\n\n任命附言：\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}
	strcpy(currboard,bname);
	strcpy(genbuftemp,genbuf);
	autoreport(bmfilename,genbuf,YEA,NULL);
	strcpy(genbuf,genbuftemp);
	//tdhlshx
	char buftitle[STRLEN],tmpfile[STRLEN];
	sprintf(buftitle,"恭喜您被任命为%s子分区的区长!",bname);
	sprintf(tmpfile,"tmp/committbm.%s",lookupuser.userid);
	FILE * fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Error,创建临时文件错误.请与系统维护联系或再次尝试!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);

	mail_file(tmpfile,lookupuser.userid,buftitle);
	prints("\n\n任命书发出!");
	pressanykey();
	char bmfaqtitle[STRLEN];
	sprintf(bmfaqtitle,"板务守则以及板务操作手册");
	mail_file("/home/bbs/etc/forbm",lookupuser.userid,bmfaqtitle);
#ifdef ORDAINBM_POST_BOARDNAME

	strcpy(currboard,ORDAINBM_POST_BOARDNAME);
	autoreport(bmfilename,genbuf,YEA,NULL);
#endif

	pressanykey();
	unlink(tmpfile);
	return 0;
}

/* thunder 2003.5.21 发送拒信
   请确保你要发送的id写过申请*/

int refs_mail()
{
	int     id, pos,  i;
	struct boardheader fh;

	char nbuf[8], bname[STRLEN];
	strcpy(bname,"");
	char buf[5][STRLEN];
	char note[5][STRLEN]={"","板务","板面","个人文集","永久帐号"};
	modify_user_mode(ADMIN);


	clear();
	stand_title("发送拒信\n");
	clrtoeol();
	prints("请确保你要发送拒信的id写过申请");
	if (!gettheuserid(2,"请输入被拒者id: ",&id))
		return 0;
	prints("请选择拒信类别:\n1.失败的板务申请\n2.失败的板面申请\n3.失败的个人文集申请\n4.失败的永久帐号申请\n5.exit");


	getdata(9, 0, "# [5] ", nbuf, 2, DOECHO, YEA);
	int sel=nbuf[0]-'0';
	if(sel<1 || sel>=5 )
		return 0;
	if(sel==1)
	{
		if(!gettheboardname(10,"哪个板面的板务? ",&pos,&fh,bname))
			return -1;
	}
	if(sel==2)
	{

		getdata(10, 0, "他申请的板面是:", bname, STRLEN-5, DOECHO, YEA);
	}


	/*  prints("拒绝%s的%s申请",lookupuser.userid,note[sel]);
	  if( askyn("  你确定吗?",NA,NA)== NA ) {
	      prints("拒绝申请取消");
	      pressanykey();
	      clear();
	      return -1;
	  }
	  */
	prints("你要拒绝%s了,有什么话要说吗?给个理由先(最多五行，按Enter结束)",lookupuser.userid);
	for (i=0;i<5;i++)
	{
		getdata(i+12, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
	}



	prints("拒绝%s的%s%s申请",lookupuser.userid,bname,note[sel]);
	if( askyn("  你确定吗?",NA,NA)== NA )
	{
		prints("\n\n拒绝申请取消");
		pressanykey();
		clear();
		return -1;
	}

	char    secu[STRLEN];
	sprintf(secu, "拒绝申请, 拒绝 %s 申请%s%s", lookupuser.userid,bname,note[sel]);
	securityreport(secu);

	char tmpfile[STRLEN],title[STRLEN];
	sprintf(tmpfile,"tmp/refsmail.%s", currentuser.userid);
	sprintf(title,"[通告] 关于您的%s%s申请",bname,note[sel]);
	FILE *fp=fopen(tmpfile,"w+");
	if(fp==NULL)
	{
		prints("Erroe,不能创建临时文件");
		pressanykey();
		clear();
		return -1;
	}

	char msg[STRLEN];
	if(sel==1)
		sprintf(msg,"首先感谢您申请%s板板务.",bname);
	else
		sprintf(msg,"您的%s%s申请我们已经看过了.",bname,note[sel]);


	struct tm *tm;
	time_t now;
	time(&now);
	char datestring[STRLEN];
	tm = localtime(&now);
	sprintf(datestring,"%4d年%02d月%02d日",
	        tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday
	       );
	sprintf(genbuf,"\n\t[通告]\n\t亲爱的%s:\n\t\t您好!%s\n\t\t遗憾的是,经过站务组的讨论,我们不能批准您的申请.\n\t\t您如果有问题,请到sysop板发文询问.\n\t\t最后,希望您能继续关注和支持本BBS.\n\t\t\t\t\t\t\t\t\t\t  %s 站务组\n\t\t\t\t\t\t\t\t\t\t   %s",lookupuser.userid,msg,BBSNAME,datestring);
	for(i=0; i< 5 ; i++)
	{
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,"\n站长附言：\n");
		strcat(genbuf,buf[i]);
		strcat(genbuf,"\n");
	}

	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,title);

	unlink(tmpfile);
	prints("\n\n拒信发送成功!");
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
	stand_title("板主离职\n");
	clrtoeol();
	if (!gettheuserid(2,"输入欲离职的板主帐号: ",&id))
		return -1;
	if (!gettheboardname(3,"请输入该板主要辞去的板名: ",&pos,&fh,bname))
		return -1;
	if (is_zone(BOARDS,bname))
	{
        	move ( 5,0);
        	prints("你不能离任子分区区长");
        	pressanykey(); 
                clear();
	        return -1;
	}
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if ( !oldbm || !find)
	{
		move ( 5,0);
		prints(" %s %s板板主，如有错误，请通知程序站长。",lookupuser.userid,(oldbm)?"不是该":"没有担任任何");
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
		prints("对不起， %s 板板主名单中没有 %s ，如有错误，请通知程序站长。",
		       bname, lookupuser.userid);
		pressanykey();
		clear();
		return -1;
	}
	prints("\n你将取消 %s 的 %s 板板%s职务.\n",
	       lookupuser.userid,bname,bm?"主":"副");
	if( askyn("你确定要取消他的该板板主职务吗?",NA,NA)== NA )
	{
		prints("\n呵呵，你改变心意了？ %s 继续留任 %s 板板主职务！",
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
	sprintf(genbuf,"取消 %s 的 %s 讨论区板主职务",lookupuser.userid,fh.filename);
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
			sprintf(secu,"板主卸职, 取消 %s 的板主权限", lookupuser.userid);
			securityreport(secu);
			move( 9,0);
			prints(secu);
		}
	}
	prints("\n\n");
	if( askyn("需要在相关板面发送公告吗?",YEA,NA)== NA )
	{
		pressanykey();
		return 0;
	}
	prints("\n");
	if(askyn("正常离任请按 Enter 键确认，撤职惩罚按 N 键",YEA,NA)==YEA)
		right=1;
	else
		right = 0;
	if(right)
		sprintf(bmfilename,"%s 板%s %s 离任公告",bname,bm?"板主":"板副",lookupuser.userid);
	else
		sprintf(bmfilename,"[公告]撤除 %s 板板务 %s ",bname,lookupuser.userid);
	strcpy(currboard,bname);
	if(right)
	{
		sprintf(genbuf,"\n\t\t\t【 公告 】\n\n"
		        "\t经站务组讨论：\n"
		        "\t同意 %s 辞去 %s 板的%s职务。\n"
		        "\t在此，对%s曾经在 %s 板的辛苦劳作表示感谢。\n\n"
		        "\t希望今后也能支持本板的工作.",
		        lookupuser.userid,bname,bm?"板主":"板副",(lookupuser.gender=='F'||lookupuser.gender=='f')?"她":"他",bname);
	}
	else
	{
		sprintf(genbuf,"\n\t\t\t【撤职公告】\n\n"
		        "\t经站务组讨论决定：\n"
		        "\t撤除 %s 板板务 %s 。\n",
		        bname,lookupuser.userid);
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(14,0);
	prints("请输入%s附言(最多五行，按 Enter 结束)",right?"板主离任":"板主撤职");
	for (i=0;i<5;i++)
	{
		getdata(i+15, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,right?"\n\n离任附言：\n":"\n\n撤职说明：\n");
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
		prints("Error,创建临时文件错误.请与系统维护联系或再次尝试!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,buftitle);

	//板务离任后清空丝路
	sethomefile(genbuf, lookupuser.userid, ".announcepath");
	if(dashf(genbuf))
		unlink(genbuf);

	prints("\n执行完毕！");
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
	stand_title("子分区区长离职\n");
	clrtoeol();
	if (!gettheuserid(2,"输入欲离职的子分区区长帐号: ",&id))
		return -1;
	if (!gettheboardname(3,"请输入该板主要辞去的子分区名: ",&pos,&fh,bname))
		return -1;
        if (!is_zone(BOARDS,bname))
        {
                move ( 5,0);
                prints("你不能离任板务");
                pressanykey();
                clear();
                return -1;
        }
	oldbm = getbnames(lookupuser.userid,bname,&find);
	if ( !oldbm || !find)
	{
		move ( 5,0);
		prints(" %s %s子分区区长，如有错误，请通知系统维护。",lookupuser.userid,(oldbm)?"不是该":"没有担任任何");
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
		prints("对不起， %s 子分区区长名单中没有 %s ，如有错误，请通知程序站长。",bname, lookupuser.userid);
		pressanykey();
		clear();
		return -1;
	}
	prints("\n你将取消 %s 的 %s 子分区区长职务.\n",lookupuser.userid,bname);
	if( askyn("你确定要取消他的该子分区区长职务吗?",NA,NA)== NA )
	{
		prints("\n呵呵，你改变心意了？ %s 继续留任 %s 子分区区长职务！",
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
	sprintf(genbuf,"取消 %s 的 %s 子分区区长职务",lookupuser.userid,fh.filename);
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
			sprintf(secu,"板主卸职, 取消 %s 的子分区区长权限", lookupuser.userid);
			securityreport(secu);
			move( 9,0);
			prints(secu);
		}
	}
	prints("\n\n");
	if(askyn("正常离任请按 Enter 键确认，撤职惩罚按 N 键",YEA,NA)==YEA)
		right=1;
	else
		right = 0;
	if(right)
		sprintf(bmfilename,"%s 子分区区长 %s 离任公告",bname,lookupuser.userid);
	else
		sprintf(bmfilename,"[公告]撤除 %s 子分区区长 %s ",bname,lookupuser.userid);
	strcpy(currboard,bname);
	if(right)
	{
		sprintf(genbuf,"\n\t\t\t【 公告 】\n\n"
		        "\t经站务组讨论：\n"
		        "\t同意 %s 辞去 %s 子分区的区长职务。\n"
		        "\t在此，对你曾经在 %s 子分区的辛苦劳作表示感谢。\n\n"
		        "\t希望今后也能支持本子分区的工作.",
		        lookupuser.userid,bname,bname);
	}
	else
	{
		sprintf(genbuf,"\n\t\t\t【撤职公告】\n\n"
		        "\t经站务组讨论决定：\n"
		        "\t撤除 %s 子分区区长 %s 的区长职务。\n",
		        bname,lookupuser.userid);
	}
	for(i=0;i<5;i++)
		buf[i][0] = '\0';
	move(14,0);
	prints("请输入%s附言(最多五行，按 Enter 结束)",right?"子分区区长离任":"子分区区长撤职");
	for (i=0;i<5;i++)
	{
		getdata(i+15, 0, ": ", buf[i], STRLEN-5, DOECHO, YEA);
		if(buf[i][0] == '\0')
			break;
		if(i == 0)
			strcat(genbuf,right?"\n\n离任附言：\n":"\n\n撤职说明：\n");
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
		prints("Error,创建临时文件错误.请与系统维护联系或再次尝试!");
		pressanykey();
		return -1;
	}
	fprintf(fp,"%s",genbuf);
	fclose(fp);
	mail_file(tmpfile,lookupuser.userid,buftitle);
	
	//子分区长离任后清空丝路
	sethomefile(genbuf, lookupuser.userid, ".announcepath");
	if(dashf(genbuf))
		unlink(genbuf);
	
	prints("\n执行完毕！");
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
	prints("\n【\033[0;1;4;33m注意\033[m】系统发现 menu.ini 的配置"
	       "可能存在问题，建议检查后再继续进行！\n        ");
	prints("如果不明白该提示的含义，请拷贝本屏信息到灌水站报告问题！\n\n");
	prints("本站目前的讨论区分类情况如下：\n\n");
	prints(" 分区   分区名称     分区类别描述             类别代码\n");
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
	if(askyn("上面的信息表明 menu.ini 可能有误，您仍然想继续执行吗",NA,NA)==YEA)
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
		prints("您将重新设定 [\033[32m%s\033[m] 的板面属性 [Enter-->设定不变]",
		       oldfh->filename);
	}
	else
		prints("您将开始创建一个\033[32m新\033[m的讨论区 [ENTER-->取消创建]");
	while(1)
	{
		struct boardheader dh;
		getdata(3, 0, "讨论区名称(英文名): ", buf, 18, DOECHO, YEA);
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
			prints("\n错误! 此讨论区已经存在!!");
			if(oldfh)
				continue;
			pressanykey();
			return -1;
		}
		strcpy(newfh.filename, buf);
		break;
	}
	prints("\n[讨论区说明]分区代码 + 分区描述 + 转信标志 + 讨论区标题\n");
	if(oldfh)
	{
		prints("初始设定：%11s\033[32m%s\033[m\n",oldfh->title,oldfh->title+11);
	}
	else
		prints("格式范例：a[本站] ○ \033[32m这是一个站内讨论区\033[m\n");
	while(1)
	{
		if(oldfh)
			strcpy(buf,oldfh->title+11);
		else
			buf[0] = '\0';
		getdata(8, 0, "讨论区标题(中文名): ", buf, 41, DOECHO, NA);
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
	if(askyn("该讨论区需要参与转信吗",NA,NA)==YEA)
	{
		strcpy(nntp,"●");
		newfh.flag |= OUT_FLAG;
	}
	else
	{
		strcpy(nntp,"○");
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
		prints("┃%s(%d)%-12s\033[m┃",
		       (i+'0'==ch)?"\033[32m":"",i,GroupsInfo.name[i]);
	}
	while(1)
	{
		num[0] = ch;
		num[1] = '\0';
		getdata(11, 0, "该讨论区放置在哪一分区(参考右边提示编号)？: ",
		        num, 2, DOECHO, NA);
		if(num[0]=='\0'||num[0]<'0'||num[0]>'9')
			continue;
		if(GroupsInfo.name[num[0]-'0'][0] == '\0')
			continue;
		break;
	}
	prints("\n第 %c 区的分类符号参考：[\033[32m%s\033[m] 分类描述参考：%s",
	       num[0],GroupsInfo.chs[num[0]-'0'],GroupsInfo.items[num[0]-'0']);
	while(1)
	{
		if(oldfh==NULL||!strchr(GroupsInfo.chs[num[0]-'0'],oldfh->title[0]))
			chs[0] = GroupsInfo.chs[num[0]-'0'][0];
		else
			chs[0] = oldfh->title[0];
		chs[1] = '\0';
		getdata(14,0,"请输入该讨论区的分类符号: ",chs,2,DOECHO,NA);
		if(chs[0]=='\0')
			continue;
		if(strchr(GroupsInfo.chs[num[0]-'0'],chs[0]))
			break;
	}
	move(12,0);
	prints("\n第 %c 区的分类符号参考：[%s] 分类描述参考：\033[32m%s\033[m",
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
		getdata(15,0,"请输入该讨论区的分类描述: ",buf,7,DOECHO,NA);
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
		prints("\n板主设置  [ 如果不需要板主，则系统自动设置为由 SYSOPs 管理]\n");
		if(askyn("本板需要板主吗",YEA,NA)==NA)
			strcpy(newfh.BM, "SYSOPs");
		else
			newfh.BM[0] = '\0';
	}
	else
	{
		prints("\n板主设置  [ 目前板面的板主是: %s ]\n", oldfh->BM);
		if(askyn("当出现板主名单错误的时候，才需要修改此处。需要修改板主名单吗",
		         NA,NA)==YEA)
		{
			prints("【\033[31m警告\033[m】修改板主仅供出错修正使用，"
			       "板主任免请勿改动此处！\n");
			strcpy(newfh.BM,oldfh->BM);
			getdata(20,0,"板主名单: ",newfh.BM,sizeof(newfh.BM),DOECHO,NA);
			if(newfh.BM[0] == ' ')
				newfh.BM[0] = '\0';
		}
		else
			strcpy(newfh.BM,oldfh->BM);
	}
	clear();
	if(askyn("本讨论区需要设置 READ 或 POST 限制吗",NA,NA)==YEA)
	{
		char ans[4];
		sprintf(buf, "限制 (R)阅读 或 (P)张贴 文章 [%c]: ",
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
		prints("设定 %s '%s' 讨论区的权限\n",
		       newfh.level & PERM_POSTMASK ? "张贴" : "阅读", newfh.filename);
		newfh.level = setperms(newfh.level, "权限", NUMPERMS, showperminfo);
		clear();
	}
	move(1,0);
	if(askyn("设置本板为匿名板面吗",newfh.flag & ANONY_FLAG?YEA:NA,NA)==YEA)
		newfh.flag |= ANONY_FLAG;
	else
		newfh.flag &= ~ANONY_FLAG;
	move(2,0);
	if(askyn("设置本板为不可 Re 模式板面吗",
	         (newfh.flag&NOREPLY_FLAG)?YEA:NA,NA)==YEA)
		newfh.flag |= NOREPLY_FLAG;
	else
		newfh.flag &= ~NOREPLY_FLAG;
	move(3,0);
	if(oldfh!=NULL)
	{
		if(askyn("是否移动精华区的位置",NA,NA)==YEA)
			a_mv = 2;
		else
			a_mv = 0;
	}
	else
	{
		if(askyn("本板需要设立精华区吗",YEA,NA)==YEA)
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
			prints("┃%s(%d)%s GROUP_%d\033[m",
			       (i+'0'==ch)?"\033[32m":"",i, GroupsInfo.name[i], i);
		}
		while(1)
		{
			strcpy(chs,num);
			getdata(j+1,48,"精华区分布如上，请选择: ",chs,2,DOECHO,NA);
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
		if(askyn("您确认要更改吗",NA,NA)==NA)
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
			prints("\n初始化讨论区目录时出错!\n");
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
				prints("\n成立精华区失败....\n");
			else
				prints("已经置入精华区...\n");
		}
	}
	else
	{  // 修改讨论区
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
					prints("\n成立精华区失败....\n");
				else
					prints("已经置入精华区...\n");
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
			sprintf(genbuf, "更改讨论区 %s 的资料 --> %s",
			        oldfh->filename, newfh.filename);
			report(genbuf);
		}
	}
	else if(append_record(BOARDS, &newfh, sizeof(newfh)) == -1)
	{
		prints("\n成立新板失败！\n");
		pressanykey();
		clear();
		return -1;
	}
	numboards = -1;
	{
		char    secu[STRLEN];
		if(oldfh)
			sprintf(secu, "修改讨论区：%s(%s)",oldfh->filename,newfh.filename);
		else
			sprintf(secu, "成立新板：%s", newfh.filename);
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
	stand_title("开启新讨论区") ;
	ChangeTheBoard(NULL,0);
	return 0 ;
}
void ShowBrdSet(struct boardheader fh)
{
	int i = 4;
	move(i,0);
	prints("讨论区 [%s] 基本情况：",fh.filename);
	i += 2;
	move(i++,0);
	prints("项目名称           项目属性");
	move(i++,0);
	prints("------------------------------");
	move(i++,0);
	prints("讨论区名称       : %s",fh.filename);
	move(i++,0);
	prints("讨论区管理员     : %s",fh.BM[0]=='\0'?"尚无板主":fh.BM);
	move(i++,0);
	prints("是否为匿名讨论区 : %s",(fh.flag&ANONY_FLAG) ? "匿名" : "非匿名");
	move(i++,0);
	prints("文章是否可以回复 : %s",(fh.flag&NOREPLY_FLAG)?"不可 Re":"可以");
	move(i++,0);
	prints("READ/POST 限制   : ");
	if(fh.level & ~PERM_POSTMASK)
	{
		prints("%s", (fh.level & PERM_POSTMASK) ? "POST" :
		       (fh.level & PERM_NOZAP) ? "ZAP" : "READ");
	}
	else
		prints("无限制");
	move(i++,0);
	prints("本讨论区说明     : %s\n\n",fh.title);
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
	stand_title("修改讨论区基本属性");
	if(!gettheboardname(2,"输入讨论区名称: ",&pos,&fh,bname))
		return -1;
	ShowBrdSet(fh);
	move(15, 0);
	if (askyn("是否需要更改以上属性", NA, NA) == YEA)
	{
		clear();
		stand_title("修改讨论区基本属性");
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
        stand_title("开启新子分区");
        memset(&newboard, 0, sizeof(newboard));
        move(2, 0);
        //   ansimore2("etc/boardref", NA, 3, 7);
        ansimore2("etc/zoneref", NA, 2, 9);
        while (1)
        {
                getdata(11, 0, "子分区名称:   ", newboard.filename, 18, DOECHO, YEA);
                if (newboard.filename[0] != 0)
                {
                        struct boardheader dh;
                        if (search_record(BOARDS,&dh,sizeof(dh),cmpbnames,newboard.filename))
                        {
                                prints("\n错误! 此讨论区已经存在!!");
                                pressanykey();
                                return -1;
                        }
                }
                else
                        return -1;
                if (valid_brdname(newboard.filename))
                        break;
                prints("\n不合法名称!!");
        }
        newboard.flag = 0;
        while (1)
        {
                getdata(12, 0, "子分区说明:   ", newboard.title, 60, DOECHO, YEA);
                if (newboard.title[0] == '\0')
                        return -1;
                if (strstr(newboard.title, "＋"))
                {
                        newboard.flag |= ZONE_FLAG;
                        break;
                }
                else
                        prints("错误的子分区格式!!");
        }
        if (   getbnum(newboard.filename) > 0 )
        {
                prints("\n错误的子分区名称!!\n");
                pressreturn();
                clear();
                return -1;
        }
        newboard.BM[0] = '\0';
        move(13,0);
	if (askyn("该分区是否为多级子分区", NA, NA) == YEA)
        {
                while(1)
                {
                        getdata(15, 0, "父子分区名称:   ", zonename, 18, DOECHO, YEA);
                        if(search_zone(BOARDS,zonename,newboard.title[0]))
                        {
                                break;
                        }
                }
                strcpy(newboard.owner,zonename);
        }
        if (askyn("是否限制存取权力", NA, NA) == YEA)
        {
                getdata(17, 0, "限制 Read/Post? [R]: ", ans, 2, DOECHO, YEA);
                if (*ans == 'P' || *ans == 'p')
                        newboard.level = PERM_POSTMASK;
                else
                        newboard.level = 0;
                move(1, 0);
                clrtobot();
                move(2, 0);
                prints("设定 %s 权力. 讨论区: '%s'\n",
                       (newboard.level & PERM_POSTMASK ? "POST" : "READ"),newboard.filename);
                newboard.level=setperms(newboard.level,"权限",NUMPERMS,showperminfo);
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
        prints("\n新子分区成立\n");
        {
                char    secu[STRLEN];
                sprintf(secu, "成立新子分区：%s", newboard.filename);
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
	stand_title("开启新讨论区");
	memset(&newboard, 0, sizeof(newboard));
	move(2, 0);
	//   ansimore2("etc/boardref", NA, 3, 7);
	ansimore2("etc/boardref", NA, 2, 9);
	while (1)
	{
		getdata(11, 0, "讨论区名称:   ", newboard.filename, 18, DOECHO, YEA);
		if (newboard.filename[0] != 0)
		{
			struct boardheader dh;
			if (search_record(BOARDS,&dh,sizeof(dh),cmpbnames,newboard.filename))
			{
				prints("\n错误! 此讨论区已经存在!!");
				pressanykey();
				return -1;
			}
		}
		else
			return -1;
		if (valid_brdname(newboard.filename))
			break;
		prints("\n不合法名称!!");
	}
	newboard.flag = 0;
	while (1)
	{
		getdata(12, 0, "讨论区说明:   ", newboard.title, 60, DOECHO, YEA);
		if (newboard.title[0] == '\0')
			return -1;
		if (strstr(newboard.title, "●") || strstr(newboard.title, "⊙"))
		{
			newboard.flag |= OUT_FLAG;
			break;
		}
		else if (strstr(newboard.title, "○"))
		{
			newboard.flag &= ~OUT_FLAG;
			break;
		}
		else
			prints("错误的格式, 无法判断是否转信!!");
	}
	strcpy(vbuf, "vote/");
	strcat(vbuf, newboard.filename);
	setbpath(genbuf, newboard.filename);
	if (   getbnum(newboard.filename) > 0 || mkdir(genbuf, 0755) == -1
	        || mkdir(vbuf, 0755) == -1)
	{
		prints("\n错误的讨论区名称!!\n");
		pressreturn();
		clear();
		return -1;
	}
	move(13,0);
	if(askyn("本板诚征板主吗(否则由SYSOPs管理)?",YEA,NA)==NA)
		strcpy(newboard.BM, "SYSOPs");
	else
		newboard.BM[0] = '\0';
	move(14, 0);
	if (askyn("是否限制存取权力", NA, NA) == YEA)
	{
		getdata(15, 0, "限制 Read/Post? [R]: ", ans, 2, DOECHO, YEA);
		if (*ans == 'P' || *ans == 'p')
			newboard.level = PERM_POSTMASK;
		else
			newboard.level = 0;
		move(1, 0);
		clrtobot();
		move(2, 0);
		prints("设定 %s 权力. 讨论区: '%s'\n",
		       (newboard.level & PERM_POSTMASK ? "POST" : "READ"),newboard.filename);
		newboard.level=setperms(newboard.level,"权限",NUMPERMS,showperminfo);
		clear();
	}
	else
		newboard.level = 0;
	move(15, 0);
	if (askyn("是否加入匿名板", NA, NA) == YEA)
		newboard.flag |= ANONY_FLAG;
	else
		newboard.flag &= ~ANONY_FLAG;
	if (askyn("该板的全部文章均不可以回复", NA, NA) == YEA)
		newboard.flag |= NOREPLY_FLAG;
	else
		newboard.flag &= ~NOREPLY_FLAG;
	if (askyn("如果该板为封闭式俱乐部板面是否对非会员隐藏？", NA, NA) == YEA)//loveni
		newboard.flag2 |= HIDE_FLAG;
	else
		newboard.flag2 &= ~HIDE_FLAG;
	if (askyn("本板是否不算文章数？", NA, NA) == YEA)//loveni
		newboard.flag2 |= JUNK_FLAG;
	else
		newboard.flag2 &= ~JUNK_FLAG;
	if (askyn("本板文章是否不统计入十大？", NA, NA) == YEA)//loveni
		newboard.flag2 |= NOTOPTEN_FLAG;
	else
		newboard.flag2 &= ~NOTOPTEN_FLAG;
	if (askyn("本板是否\033[1;31m不过滤敏感字？\033[m", NA, NA) == YEA)//loveni
		newboard.flag2 |= NOFILTER_FLAG;
	else
		newboard.flag2 &= ~NOFILTER_FLAG;
	if (askyn("本板是否强制模板发文？", NA, NA) == YEA)//loveni
		newboard.flag2 |= TEMPLATE_FLAG;
	else
		newboard.flag2 &= ~TEMPLATE_FLAG;
        if (askyn("该板是否为二级板面", NA, NA) == YEA)
        {
                newboard.flag |= CHILDBOARD_FLAG;
                while(1)
                {
                        getdata(20, 0, "子分区名称:   ", zonename, 18, DOECHO, YEA);
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
			prints("\n成立精华区失败....\n");
		else
			prints("已经置入精华区...\n");
	}
	numboards = -1;
	prints("\n新讨论区成立\n");
	{
		char    secu[STRLEN];
		sprintf(secu, "成立新板：%s", newboard.filename);
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
	stand_title("修改讨论区资讯");
	if(!gettheboardname(2,"输入讨论区名称: ",&pos,&fh,bname))
		return -1;
	if(fh.flag & ZONE_FLAG)
        {
                move(2,0);
                prints("这里不能修改子分区信息\n");
                pressreturn();
                return -1;
        }
	noidboard = fh.flag & ANONY_FLAG;
	noreply = fh.flag & NOREPLY_FLAG;
	clear();
	move(0, 0);
	memcpy(&newfh, &fh, sizeof(newfh));
	prints("讨论区名称:  %s               讨论区管理员:  %s\n", fh.filename, fh.BM);
	prints("讨论区说明:     %s\n", fh.title);
//	prints("讨论区管理员:   %s\n", fh.BM);
	prints("匿名讨论区:     %s             ", (noidboard) ? "Yes" : "No");
	prints("文章不可以回复: %s\n", (noreply) ? "Yes" : "No");
	prints("封闭式俱乐部板面是否对非会员隐藏:%s\n",fh.flag2 & HIDE_FLAG ? "Yes" : "No");
	prints("本板是否不算文章数:%s            ",fh.flag2 & JUNK_FLAG ? "Yes" : "No");
	prints("本板文章是否不统计入十大:%s\n",fh.flag2 & NOTOPTEN_FLAG? "Yes" : "No");
	prints("本板是否\033[1;31m不过滤敏感字\033[m:%s\n",fh.flag2 & NOFILTER_FLAG ? "Yes" : "No");//loveni
	prints("本板是否强制模板发文:%s\n",fh.flag2 & TEMPLATE_FLAG ? "Yes" : "No");//loveni
	prints("是否二级讨论区: %s ", fh.flag&CHILDBOARD_FLAG ? "Yes":"No");
	strcpy(oldtitle, fh.title);
	prints("限制 %s 权力: %s", (fh.level & PERM_POSTMASK) ? "POST" :
	       (fh.level & PERM_NOZAP) ? "ZAP" : "READ",
	       (fh.level & ~PERM_POSTMASK) == 0 ? "不设限" : "有设限");
	move(8, 0);
	if (askyn("是否更改以上资讯", NA, NA) == NA)
	{
		clear();
		return 0;
	}
	move(9, 0);
	prints("直接按 <Return> 不修改此栏资讯...");
	while (1)
	{
		getdata(10, 0, "新讨论区名称: ", genbuf, 18, DOECHO, YEA);
		if (*genbuf != 0)
		{
			struct boardheader dh;
			if (search_record(BOARDS, &dh, sizeof(dh), cmpbnames, genbuf))
			{
				move(2, 0);
				prints("错误! 此讨论区已经存在!!");
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
				prints("不合法的讨论区名称!");
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
		getdata(17, 0, "新讨论区说明: ", genbuf, 60, DOECHO, YEA);
		if (*genbuf != 0)
			strncpy(newfh.title, genbuf, sizeof(newfh.title));
		else
			break;
		if (strstr(newfh.title, "●") || strstr(newfh.title, "⊙"))
		{
			newfh.flag |= OUT_FLAG;
			break;
		}
		else if (strstr(newfh.title, "○"))
		{
			newfh.flag &= ~OUT_FLAG;
			break;
		}
		else
			prints("\n错误的格式, 无法判断是否转信!!");
	}
	move(18, 0);
	//clrtoeol();
	//move(20, 0);
	if( fh.BM[0] != '\0' && strcmp(fh.BM,"SYSOPs"))
	{
		if(askyn("修改讨论区管理员。注意：确有错误才可修改。不修改按回车",
		         NA,NA) == YEA)
		{
			getdata(18,0,"讨论区管理员(【注意】板主任免请勿改动此处！): ",
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
		if(askyn("本板诚征板主吗(否，则由SYSOPs管理)?",YEA,NA)==NA)
			strncpy(newfh.BM, "SYSOPs", sizeof(newfh.BM));
		else
			strncpy(newfh.BM, "\0", sizeof(newfh.BM));
	}
	sprintf(buf, "匿名板 (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
	getdata(19, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag |= ANONY_FLAG;
		else
			newfh.flag &= ~ANONY_FLAG;
	}
	sprintf(buf, "文章是否不可以回复 (Y/N)? [%c]: ", (noreply) ? 'Y' : 'N');
	getdata(19, 30, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag |= NOREPLY_FLAG;
		else
			newfh.flag &= ~NOREPLY_FLAG;
	}
	
	sprintf(buf, "如果该板为封闭式俱乐部板面是否对非会员隐藏？ (Y/N)? [%c]: ", (fh.flag2&HIDE_FLAG) ? 'Y' : 'N');
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
                if(askyn("属于子分区更改否",NA,NA)==YEA)
                {
                        while(1)
                        {
                                sprintf(buf,"更改子分区名字为(.为取消子分区): ");
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
                if(askyn("不属于任何子分区更改否",NA,NA)==YEA)
                {
                        while(1)
                        {
                                sprintf(buf,"更改子分区名字: ");
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
	if (askyn("是否移动精华区的位置", NA, NA) == YEA)
		a_mv = 2;
	else
		a_mv = 0;
	move(23, 0);
	if (askyn("是否更改存取权限", NA, NA) == YEA)
	{
		char    ans[4];
		sprintf(genbuf, "限制 (R)阅读 或 (P)张贴 文章 [%c]: ",
		        (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
		getdata(24, 0, genbuf, ans, 2, DOECHO, YEA);
		if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
			newfh.level &= ~PERM_POSTMASK;
		else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
			newfh.level |= PERM_POSTMASK;
		clear();
		move(2, 0);
		prints("设定 %s '%s' 讨论区的权限\n",
		       newfh.level & PERM_POSTMASK ? "张贴" : "阅读", newfh.filename);
		newfh.level = setperms(newfh.level, "权限", NUMPERMS, showperminfo);
		//clear();
		//getdata(0, 0, "确定要更改吗? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
	}
	//else
	//{
	clear();
	sprintf(buf, "本板是否不算文章数？ (Y/N)? [%c]: ", (fh.flag2&JUNK_FLAG) ? 'Y' : 'N');
	getdata(0, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= JUNK_FLAG;
		else
			newfh.flag2 &= ~JUNK_FLAG;
	}
	sprintf(buf, "本板文章是否不统计入十大？ (Y/N)? [%c]: ", (fh.flag2&NOTOPTEN_FLAG) ? 'Y' : 'N');
	getdata(1, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= NOTOPTEN_FLAG;
		else
			newfh.flag2 &= ~NOTOPTEN_FLAG;
	}
	sprintf(buf, "本板是否\033[1;31m不过滤敏感字？\033[m (Y/N)? [%c]: ", (fh.flag2&NOFILTER_FLAG) ? 'Y' : 'N');
	getdata(2, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= NOFILTER_FLAG;
		else
			newfh.flag2 &= ~NOFILTER_FLAG;
	}
	sprintf(buf, "本板是否强制模板发文 (Y/N)? [%c]: ", (fh.flag2&TEMPLATE_FLAG) ? 'Y' : 'N');
	getdata(3, 0, buf, genbuf, 4, DOECHO, YEA);
	if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
	{
		if (*genbuf == 'y' || *genbuf == 'Y')
			newfh.flag2 |= TEMPLATE_FLAG;
		else
			newfh.flag2 &= ~TEMPLATE_FLAG;
	}
	
		
	getdata(4, 0, "确定要更改吗? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
	//}
	if (*genbuf != 'Y' && *genbuf != 'y')
	{
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "修改讨论区：%s(%s)", fh.filename, newfh.filename);
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
					prints("\n成立精华区失败....\n");
				else
					prints("已经置入精华区...\n");
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
	sprintf(genbuf, "更改讨论区 %s 的资料 --> %s", fh.filename, newfh.filename);
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
	stand_title("清除私人信件");
	move(1, 0);
	prints("清除所有已读且未 mark 的信件\n");
	if (askyn("确定吗", NA, NA) == NA)
	{
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "清除所有使用者已读信件。");
		securityreport(secu);
	}

	cleanlog = fopen("mailclean.log", "w");
	move(3, 0);
	prints("请耐心等候.\n");
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
	prints("清除完成! 记录档 mailclean.log.\n");
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
		prints("目前设定:\n");
		trace_state(otflag, "一般", ostatb.st_size);
		trace_state(ctflag, "聊天", cstatb.st_size);
		move(9, 0);
		prints("<1> 切换一般记录\n");
		prints("<2> 切换聊天记录\n");
		getdata(12, 0, "请选择 (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);
 
		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "一般记录 ON";
			} else {
				rename("trace", "trace.old");
				msg = "一般记录 OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "聊天记录 ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "聊天记录 OFF";
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

	stand_title("设定使用者注册资料");
	for (;;)
	{
		getdata(1, 0, "(0)离开  (1)审查新注册资料  (2)查询使用者注册资料 ? : ",
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
			prints("\n\n目前并无新注册资料.");
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
			if (askyn("\33[37m设定资料吗", NA, YEA) == YEA)
			{
				securityreport("设定使用者注册资料");
				scan_register_form(fname);
			}
		}
	}
	else
	{
		move(1, 0);
		usercomplete("请输入要查询的代号: ", uident);
		if (uident[0] != '\0')
			if (!getuser(uident))
			{
				move(2, 0);
				prints("错误的使用者代号...");
			}
			else
			{
				sprintf(genbuf, "home/%c/%s/register", toupper(lookupuser.userid[0]), lookupuser.userid);
				if ((fn = fopen(genbuf, "r")) != NULL)
				{
					prints("\n注册资料如下:\n\n");
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
					prints("\n\n找不到他/她的注册资料!!\n");
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
        stand_title("删除子分区");
        make_blist();
        move(1, 0);
        namecomplete("请输入子分区: ", bname);
        if (bname[0] == '\0')
                return 0;
        bid = getbnum(bname);
        if (get_record(BOARDS, &binfo, sizeof(binfo), bid) == -1 || !(binfo.flag & ZONE_FLAG))
        {
                move(2, 0);
                prints("不正确的子分区\n");
                pressreturn();
                clear();
                return 0;
        }
        if(count_zone(BOARDS,binfo.filename))
        {
                move(2,0);
                prints("子分区内有讨论区，不能删除\n");
                pressreturn();
                clear();
                return 0;
        }
        ans = askyn("你确定要删除这个子分区", NA, NA);
        if (ans != 1)
        {
                move(2, 0);
                prints("取消删除行动\n");
                pressreturn();
                clear();
                return 0;
        }
        {
                char    secu[STRLEN];
                sprintf(secu, "删除子分区：%s", binfo.filename);
                securityreport(secu);
        }
        sprintf(genbuf, " << '%s' 被 %s 删除 >>",
                binfo.filename, currentuser.userid);
        memset(&binfo, 0, sizeof(binfo));
        strncpy(binfo.title, genbuf, STRLEN);
        binfo.level = PERM_SYSOP;
        substitute_record(BOARDS, &binfo, sizeof(binfo), bid);
        move(4, 0);
        prints("\n本子分区已经删除...\n");
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
	stand_title("删除讨论区");
	make_blist();
	move(1, 0);
	namecomplete("请输入讨论区: ", bname);
	if (bname[0] == '\0')
		return 0;
	bid = getbnum(bname);
	if (get_record(BOARDS, &binfo, sizeof(binfo), bid) == -1 || binfo.flag & ZONE_FLAG)
	{
		move(2, 0);
		prints("不正确的讨论区\n");
		pressreturn();
		clear();
		return 0;
	}
	if(binfo.BM[0]!='\0'&&strcmp(binfo.BM,"SYSOPs"))
	{
		move(5,0);
		prints("该板还有板主，在删除本板前，请先取消板主的任命。\n");
		pressanykey();
		clear();
		return 0;
	}
	ans = askyn("你确定要删除这个讨论区", NA, NA);
	if (ans != 1)
	{
		move(2, 0);
		prints("取消删除行动\n");
		pressreturn();
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "删除讨论区：%s", binfo.filename);
		securityreport(secu);
	}
	if (seek_in_file("0Announce/.Search", bname))
	{
		move(4, 0);
		if (askyn("移除精华区", NA, NA) == YEA)
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
	sprintf(genbuf, " << '%s' 被 %s 删除 >>",
	        binfo.filename, currentuser.userid);
	memset(&binfo, 0, sizeof(binfo));
	strncpy(binfo.title, genbuf, STRLEN);
	binfo.level = PERM_SYSOP;
	substitute_record(BOARDS, &binfo, sizeof(binfo), bid);

	move(4, 0);
	prints("\n本讨论区已经删除...\n");
	pressreturn();
	numboards = -1;
	clear();
	return 0;
}

int redhackercheck(char *s)
{

     return 0;
     if(!HAS_PERM(PERM_LOOKADMIN)) return 0;
     prints("红客你好啊!首先感谢您的辛苦劳动.不过还是不好意思,您没有 %s 的权利.\n如果确实需要,请于本站相关管理人员联系.\n",s);
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
	if(redhackercheck("删除帐号"))
           return ;

 	stand_title("删除使用者帐号");
	move(2,0);
	if(!gettheuserid(1,"请输入欲删除的使用者代号: ",&id))
		return 0;
	if (!strcmp(lookupuser.userid, "SYSOP"))
	{
		prints("\n对不起，你不可以删除 SYSOP 帐号!!\n");
		pressreturn();
		clear();
		return 0;
	}
	if ((lookupuser.userlevel&PERM_SYSOP)||(lookupuser.userlevel&PERM_ACCOUNTS)||(lookupuser.userlevel&PERM_OBOARDS)||(lookupuser.userlevel&PERM_CHATCLOAK))
	{
		prints("\n对不起，请先清除其站务权限!!\n");
		pressreturn();
		clear();
		return 0;
	}
	if (!strcmp(lookupuser.userid, currentuser.userid))
	{
		prints("\n对不起，你不可以删除自己的这个帐号!!\n");
		pressreturn();
		clear();
		return 0;
	}
	prints("\n\n以下是 [%s] 的部分资料:\n",lookupuser.userid);
	prints("    User ID:  [%s]\n",lookupuser.userid);
	prints("    昵   称:  [%s]\n",lookupuser.username);
	prints("    姓   名:  [%s]\n",lookupuser.realname);
	strcpy(secu, "bTCPRD#@XWBA#VS-DOM-F0s2345678");
	for (num = 0; num < strlen(secu); num++)
		if (!(lookupuser.userlevel & (1 << num)))
			secu[num] = '-';
	prints("    权   限: %s\n\n", secu);

	num = getbnames(lookupuser.userid,secu,&num);
	if(num)
	{
		prints("[%s] 目前尚担任了 %d 个板的板主: ",
		       lookupuser.userid, num);
		for(i = 0 ; i< num ; i ++ )
			prints("%s ",bnames[i]);
		prints("\n请先使用板主卸职功能取消其板主职务再做该操作.");
		pressanykey();
		clear();
		return 0;
	}

	sprintf(genbuf,"你确认要删除 [%s] 这个 ID 吗",lookupuser.userid);
	if (askyn(genbuf, NA, NA) == NA)
	{
		prints("\n取消删除使用者...\n");
		pressreturn();
		clear();
		return 0;
	}
	sprintf(genbuf, "要把该 [%s] 这个ID加入不可注册30天吗？", lookupuser.userid);
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
			prints("加入不可注册id出错，请手工加入！");
		close(fileid);
	}
	sprintf(secu, "删除使用者：%s", lookupuser.userid);

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
	prints("\n%s 已经被灭绝了...\n", lookupuser.userid);
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
	prints("更改使用者权限\n");
	clrtoeol();
	move(1, 0);
	usercomplete("输入欲更改的使用者帐号: ", genbuf);
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
	prints("设定使用者 '%s' 的权限 \n", genbuf);
	newlevel = setperms(lookupuser.userlevel, "权限", NUMPERMS, showperminfo);
	move(2, 0);
	if (newlevel == lookupuser.userlevel)
		prints("使用者 '%s' 权限没有变更\n", lookupuser.userid);
	else
	{

		//		lookupuser.userlevel = newlevel;
		{
			char    secu[STRLEN];
			sprintf(secu, "修改 %s 的权限", lookupuser.userid);
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
		 //清空丝路
    	sethomefile(genbuf, lookupuser.userid, ".announcepath");
		if(dashf(genbuf))
			unlink(genbuf);
		prints("使用者 '%s' 权限已经更改完毕.\n", lookupuser.userid);
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
	prints("更改使用者基本权限或资料\n");
	clrtoeol();
	move(1, 0);
	usercomplete("输入欲更改的使用者帐号: ", genbuf);
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
		sprintf(secu, "修改 %s 的基本权限或资料", lookupuser.userid);
		securityreport1(secu);
	}
	move(1, 0);
	clrtobot();
	move(2, 0);
	prints("设定使用者 '%s' 的基本权限或资料 \n\n", lookupuser.userid);
	prints("(1) 封禁发表文章权力       (A) 恢复发表文章权力\n");
	prints("(2) 取消基本上站权力       (B) 恢复基本上站权力\n");
	prints("(3) 禁止进入聊天室         (C) 恢复进入聊天室权力\n");
	prints("(4) 禁止呼叫他人聊天       (D) 恢复呼叫他人聊天权力\n");
	prints("(5) 禁止给他人发消息       (E) 恢复给他人发消息权力\n");
	prints("(6) 修改 %s 基本资料\n", lookupuser.userid);
	getdata(12, 0, "请输入你的处理: ", ans, 3, DOECHO, YEA);
	//tdhlshx modified.
	int flag=1;
	char day1[7];
	int day=0;
	char buf[30],str[24];
	while(1)
	{
		getdata(13,0,"请输入说明:",buf,29,DOECHO,YEA);
		if(killwordsp(buf)!=0)
			break;
	}
	switch(ans[0])
	{
	case '1':
		denycase=1;//loveni
		lookupuser.userlevel &= ~PERM_POST;
		getdata(14,0,"输入封禁时间(以天为单位): ",day1,6,DOECHO,YEA);
		strcpy(str,"被封禁发表文章权力\0");
		break;
	case 'a':
	case 'A':
		denycase=1;//loveni
		lookupuser.userlevel |= PERM_POST;
		flag=2;
		strcpy(str,"被恢复发表文章权力\0");
		break;
	case '2':
		denycase=2;//loveni
		lookupuser.userlevel &= ~PERM_BASIC;
		getdata(14,0,"输入封禁时间(以天为单位): ",day1,6,DOECHO,YEA);
		strcpy(str,"被取消基本上站权力\0");
		break;
	case 'b':
	case 'B':
		denycase=2;//loveni
		lookupuser.userlevel |= PERM_BASIC;
		flag=2;
		strcpy(str,"被恢复基本上站权力\0");
		break;
	case '3':
		denycase=3;//loveni
		lookupuser.userlevel &= ~PERM_CHAT;
		getdata(14,0,"输入封禁时间: ",day1,6,DOECHO,YEA);
		strcpy(str,"被禁止进入聊天室\0");
		break;
	case 'c':
	case 'C':
		denycase=3;//loveni
		lookupuser.userlevel |= PERM_CHAT;
		flag=2;
		strcpy(str,"被恢复进入聊天室权力\0");
		break;
	case '4':
		denycase=4;//loveni
		lookupuser.userlevel &= ~PERM_PAGE;
		getdata(14,0,"输入封禁时间: ",day1,6,DOECHO,YEA);
		strcpy(str,"被禁止呼叫他人聊天\0");
		break;
	case 'd':
	case 'D':
		denycase=4;//loveni
		lookupuser.userlevel |= PERM_PAGE;
		flag=2;
		strcpy(str,"被恢复呼叫他人聊天权力\0");
		break;
	case '5':
		denycase=5;//loveni
		lookupuser.userlevel &= ~PERM_MESSAGE;
		getdata(14,0,"输入封禁时间: ",day1,6,DOECHO,YEA);
		strcpy(str,"被禁止给他人发消息\0");
		break;
	case 'e':
	case 'E':
		denycase=5;//loveni
		lookupuser.userlevel |= PERM_MESSAGE;
		flag=2;
		strcpy(str,"被恢复给他人发消息权力\0");
		break;
	case '6':
		flag=0;
		uinfo_change1(12,&lookupuser,&lookupuser);
		break;
	default:
		flag=0;
		prints("\n 使用者 '%s' 基本权力和资料没有变更\n",
		       lookupuser.userid);
		pressreturn();
		clear();
		return 0;
	}
	{
		char    secu[STRLEN];
		sprintf(secu, "修改 %s 的基本权限或资料", lookupuser.userid);
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
		        "\n  %s 网友: \n\n"
		        "\t很抱歉的告诉您，您已经%s \n\n"
		        "\t您被取消权力的原因是: %s \n\n"
		        "\t因为您的行为，决定停止您的权力 %d 天，\n\n"
		        "\t%d 天后，当您登录时，系统将为您自动解封。\n\n",
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
		        "\n  %s 网友: \n\n"
		        "\t您已经%s, \n\n"
		        "\t 附言:%s \n\n"
		        "\t感谢您的支持.\n\n"
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
	prints("\n 使用者 '%s' 基本权限或资料已经更改完毕.\n", lookupuser.userid);
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
	    {"特殊进站公布栏", "进站画面", "进站欢迎档", "进站重要公告","离站画面"
	     ,"公用备忘录", "不可注册的 ID", "不可确认之E-Mail", "不可上站之位址"
	     ,"不算POST数的板", "系统底部流动信息", "menu1.ini", "萤幕锁定画面","menu.ini", "每日自动送信档", "管理者名单", "定时砍信设定档",
	     "纪念日清单", "暂停登陆(NOLOGIN)", "暂停注册(NOREGISTER)", "转信黑名单", "注册成功信件","给新手的信","注册失败信件","板务守则以及板面操作手册","新用户注册范例","用户第一次登陆公告","国际会议厅清单","BBSNET 转站清单","BBSNET 转站清单II","区段删除不需备份之清单","WWW方式进站公告","FTP搜索站点列表","强制模版发文板面","默认收藏夹板面",NULL};*/
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
	    {"特殊进站公布栏", "进站画面", "进站欢迎档", "进站重要公告",
	"离站画面" ,"公用备忘录", "系统底部流动信息", "萤幕锁定画面",
	"WWW方式站内推荐","WWW方式校内推荐","不可注册的 ID","不可确认之E-Mail", 
	"不可上站之位址" ,"热门讨论区", "推荐讨论区",
	"默认收藏夹板面","十大板面黑名单","十大标题黑名单",
	"指定十大文章","指定十大祝福文章","编辑十大","编辑十大祝福","校内公告",
	"站内公告","S权限不可进入板面","内网地址设定","管理者名单","menu1.ini", 
	"menu.ini","暂停登陆(NOLOGIN)", "暂停注册(NOREGISTER)",
	"全站只读(NOPOST)", "暂停附件上传", "挂站双倍积分",NULL};

/*

"menu1.ini", "menu.ini", "每日自动送信档",  "定时砍信设定档",
	     "纪念日清单", "暂停登陆(NOLOGIN)", "暂停注册(NOREGISTER)", "转信黑名单", "注册成功信件","给新手的信","注册失败信件","板务守则以及板面操作手册","新用户注册范例","用户第一次登陆公告","国际会议厅清单","BBSNET 转站清单","BBSNET 转站清单II","FTP搜索站点列表",NULL};*/
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(1, 0);
	prints("编修系统档案\n\n");
	for (num = 0;  e_file[num] != NULL && explain_file[num] != NULL ; num++){
              
  		prints("[[1;32m%2d[m] %s", num + 1, explain_file[num]);
		if(num < 17 )
			move(4+num,0);
		else
			move(num - 14, 50);
	}
	prints("[[1;32m%2d[m] 都不想改\n", num + 1);

	getdata(23, 0, "你要编修哪一项系统档案: ", ans, 3, DOECHO, YEA);
	ch = atoi(ans);
	if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
		return;
	ch -= 1;
        

        if((ch>=0&&ch<8)&&!HAS_PERM(PERM_ACBOARD)&&!HAS_PERM(PERM_SYSOP))
	{
             clear();
	     prints("您没有权限修改本项档案,请确认您有SYSOP或ACBOARD权限");
             pressanykey();
	     return;
	}
	if((ch==8||ch==9||ch==27)&&!HAS_PERM(PERM_WELCOME)&&!HAS_PERM(PERM_SYSOP))
	{
             clear();
	     prints("您没有权限修改本项档案,请确认您有SYSOP或WELCOME权限");
             pressanykey();
	     return;
	}
	

        if((ch==10||ch==11||ch==12|| ch == 33)&&!HAS_PERM(PERM_ACCOUNTS)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("您没有权限修改本项档案,请确认您有SYSOP或ACCOUNTS权限");
             pressanykey();
             return;

        }
	if((ch==13||ch==14)&&!HAS_PERM(PERM_OBOARDS)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("您没有权限修改本项档案,请确认您有SYSOP或OBOARDS权限");
             pressanykey();
             return;

    }
	if((ch>23&&ch<32&&ch!=27)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("您没有权限修改本项档案,请确认您有SYSOP权限");
             pressanykey();
             return;

    }
	if((ch==32)&&!HAS_PERM(PERM_ANNOUNCE)&&!HAS_PERM(PERM_SYSOP))
	{
		clear();
		prints("您没有权限修改本项档案,请确认您有SYSOP或ANNOUNCE权限");
		pressanykey();
		return;
	}

	sprintf(buf2, "etc/%s", e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)编辑 (D)删除 %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"你确定要删除 %s 这个系统档",explain_file[ch]);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("取消删除行动\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "删除系统档案：%s", explain_file[ch]);
			securityreport(secu);
		}
		unlink(buf2);
		move(5, 0);
		prints("%s 已删除\n", explain_file[ch]);
		pressreturn();
		clear();
		return;
	}
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf2, NA, YEA); /* 不添加文件头, 允许修改头部信息 */
	clear();
	if (aborted != -1)
	{
		prints("%s 更新过", explain_file[ch]);
		{
			char    secu[STRLEN];
			sprintf(secu, "修改系统档案：%s", explain_file[ch]);
			securityreport(secu);
		}

		if (!strcmp(e_file[ch], "../Welcome"))
		{
			unlink("Welcome.rec");
			prints("\nWelcome 记录档更新");
		}
		else if(!strcmp(e_file[ch],"whatdate"))
		{
			brdshm->fresh_date = time(0);
			prints("\n纪念日清单 更新");
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
	    {"精华区使用说明", "讨论区使用说明", "聊天室使用说明", "聊天室管理员指令",
	     "编辑功能指令说明", "好友选单功能键", "信件发送失败信息", "邮件选单使用说明",
	     "多功能阅读选单说明", "阅读文章功能键", "讯息浏览器使用说明", "坏人选单功能键",
	     "使用者上线选单使用说明", "注册使用者选单说明", "精华区收藏选单使用说明", "生日贺信","外网发文用户","内网地址设定","十大板面黑名单","十大标题黑名单","编辑十大","校内公告","站内公告",NULL};*/
	static char *e_file[] =
	    {"help/announcereadhelp", "help/boardreadhelp", "help/chathelp", "help/chatophelp",
	     "help/edithelp", "help/friendshelp", "help/mailerror-explain", "help/mailreadhelp",
	     "help/mainreadhelp", "help/morehelp", "help/msghelp", "help/rejectshelp",
	     "help/serlisthelp", "help/usershelp", "help/favdigesthelp","etc/s_fill","etc/fornewcomers", "etc/f_fill", "etc/forbm","etc/register", "etc/firstlogin","ftp/FTPFileList","etc/chatstation","etc/bbsnet.ini","etc/bbsnet1.ini",".blockmail","etc/whatdate","etc/birthday","etc/expire.ctl", "etc/autopost","etc/BMCLOAK","etc/WarningBM","etc/specbm",NULL};

	static char *explain_file[] =
	    {"精华区使用说明", "讨论区使用说明", "聊天室使用说明", "聊天室管理员指令",
	     "编辑功能指令说明", "好友选单功能键", "信件发送失败信息", "邮件选单使用说明",
	     "多功能阅读选单说明", "阅读文章功能键", "讯息浏览器使用说明", "坏人选单功能键",
	     "使用者上线选单使用说明", "注册使用者选单说明", "精华区收藏选单使用说明", "注册成功信件","给新手的信","注册失败信件","板务守则以及板面操作手册","新用户注册范例","用户第一次登陆公告","FTP搜索站点列表","国际会议厅清单","BBSNET 转站清单","BBSNET 转站清单II", "转信黑名单","纪念日清单","生日贺信","定时砍信设定档","每日自动送信档","板务隐身上站考核","取消板务隐身权限前警告","不考核隐身上站板务之名单",NULL};
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
	{
		return;
	}
	clear();
	move(1, 0);
	prints("编修系统档案2\n\n");
	/* Efan: 此处有bug，因为是帮助信息，所以不会有"menu.ini"出现
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
	prints("[[1;32m%2d[m] 都不想改\n", num + 1);

	getdata(23, 0, "你要编修哪一项系统档案: ", ans, 3, DOECHO, YEA);
	ch = atoi(ans);
	
	if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
		return;
	ch -= 1;
	if((ch==30||ch==31||ch==32)&&!HAS_PERM(PERM_SYSOP))
	{
	     clear();
	     prints("您没有权限修改本项档案,请确认您有SYSOP权限");
             pressanykey();
             return;

        }
	sprintf(buf2, "%s", e_file[ch]);
	move(3, 0);
	clrtobot();
	sprintf(buf, "(E)编辑 (D)删除 %s? [E]: ", explain_file[ch]);
	getdata(3, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"你确定要删除 %s ",explain_file[ch]);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("取消删除行动\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "删除系统档案2：%s", explain_file[ch]);
			securityreport(secu);
		}
		unlink(buf2);
		move(5, 0);
		prints("%s 已删除\n", explain_file[ch]);
		pressreturn();
		clear();
		return;
	}
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf2, NA, YEA); /* 不添加文件头, 允许修改头部信息 */
	clear();
	if (aborted != -1)
	{
		prints("%s 更新过", explain_file[ch]);
		{
			char    secu[STRLEN];
			sprintf(secu, "编修系统档案2：%s", explain_file[ch]);
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
	if (!get_msg("所有使用者", buf2, 1))
	{
		return 0;
	}

        securityreport(buf2);

	if (apply_ulist(dowall) == -1)
	{
		move(2, 0);
		prints("线上空无一人\n");
		pressanykey();
	}
	prints("\n已经广播完毕...\n");
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
		prints("对不起，系统密码只能由 SYSOP 修改！");
		pressanykey();
		return;
	}
	getdata(2, 0, "请输入新的系统密码(直接回车则取消系统密码): ",
	        passbuf, 19, NOECHO, YEA);
	if(passbuf[0] == '\0')
	{
		if( askyn("你确定要取消系统密码吗?",NA,NA)== YEA )
		{
			unlink("etc/.syspasswd");
			securityreport("[32m取消系统密码[37m");
		}
		return ;
	}
	getdata(3, 0, "确认新的系统密码: ", prepass, 19, NOECHO, YEA);
	if (strcmp(passbuf, prepass))
	{
		move(4,0);
		prints("两次密码不相同, 取消此次设定.");
		pressanykey();
		return;
	}
	if ((pass = fopen("etc/.syspasswd", "w")) == NULL)
	{
		move(4, 0);
		prints("系统密码无法设定....");
		pressanykey();
		return;
	}
	fprintf(pass, "%s\n", genpasswd(passbuf));
	fclose(pass);
	move(4, 0);
	prints("系统密码设定完成....");
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
	getdata(1, 0, "请输入通行暗号: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
	{
		prints("\n\n暗号不正确, 不能执行。\n");
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
		prints("\n\nKey 不正确, 不能执行。\n");
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
		if( askyn("你确定使系统进入只读状态吗?",NA,NA)== YEA )
		{
			system("touch NOPOST");
			prints("系统进入只读状态……");
		}
	}
	else
	{
		if( askyn("你确定使系统退出只读状态吗?",NA,NA)== YEA )
		{
			system("mv NOPOST NOPOST.no");
			prints("系统退出只读状态……");
		}
	}
	pressanykey();
	return 1;
}

/* 敏感字编辑 */

int keywords()
{
	char buf[256];
	int aborted;
	strcpy(buf, "etc/.badwords");
	modify_user_mode(EDITSFILE);
	aborted = vedit(buf, NA, YEA);// 不添加文件头, 允许修改头部信息
	if(aborted!=-1)
		securityreport("敏感字编辑");
	return 1;
}
/*loveni:刷新十大*/
int renewtopten()
{
	clear();
	if( askyn("你确定立即刷新十大?",NA,NA)== YEA )
	{
		system("/home/bbs/bin/poststat /home/bbs");
		prints("十大已经刷新!");
	}
	pressanykey();
	return 1;
}

int renewweb()
{
	clear();
	if( askyn("你确定要立即刷新Web首页?",NA,NA)==YEA)
	{
		system("/home/www/cgi-bin/bbs/bbsmainbuf > /dev/null");
		prints("Web首页已经刷新!");
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
	stand_title("设置板规\n");
	clrtoeol();
	if(!gettheboardname(1,"输入需要设置板规的讨论区名称: ",&pos,&fh,bname))
		return -1;
	sprintf(fname,"boards/%s/boardrule",fh.filename);
	sprintf(buf, "(E)编辑 (D)删除 %s板板规? [E]: ", fh.filename);
	getdata(2, 0, buf, ans, 2, DOECHO, YEA);
	if (ans[0] == 'D' || ans[0] == 'd')
	{
		sprintf(buf,"你确定要删除 %s板板规?",fh.filename);
		confirm = askyn(buf, NA, NA);
		if (confirm != 1)
		{
			move(5, 0);
			prints("取消删除行动\n");
			pressreturn();
			clear();
			return;
		}
		{
			char    secu[STRLEN];
			sprintf(secu, "删除%s板板规", fh.filename);
			securityreport(secu);
		}
		unlink(fname);
		move(5, 0);
		prints("%s板板规已删除\n", fh.filename);
		pressreturn();
		clear();
		return;
	}
	aborted = vedit(fname, NA, YEA);
	if(aborted!=-1)
	{
		sprintf(buf,"编辑%s板的板规",fh.filename);
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
	stand_title("查询用户认证邮箱:");
	getdata (2, 0, "请输入用户id: ", username, 13, DOECHO, YEA);
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
					sprintf(genbuf,"用户id:%s\n",userid);
					prints(genbuf);
					sprintf(genbuf,"用户认证Email:%s\n",email);
					prints(genbuf);
					sprintf(genbuf,"用户认证ip:%s\n",ip);
					prints(genbuf);
				}
			}
		}
	}
	if(find==0)
	{
		move(2,0);
		clrtobot ();
		prints("没有找到该用户信息");
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
	stand_title("查询邮箱认证的用户:");
	getdata (2, 0, "请输入邮箱:", address, 80, DOECHO, YEA);
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
				sprintf(genbuf,"用户id:%s\n",userid);
				prints(genbuf);
	 			sprintf(genbuf,"用户认证ip:%s\n\n",ip);
				prints(genbuf);
			}
		}
	}
	if(find==0)
	{
		move(2,0);
		clrtobot ();
		prints("没有找到该邮箱信息");
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
	stand_title("授予用户永6帐号");
	if (!gettheuserid(1, "请输入使用者代号: ", &id))	return -1;
	
	
	sprintf(fname, "home/%c/%s/life666", toupper(lookupuser.userid[0]), lookupuser.userid);
	move(4, 0);
	clrtobot();
	
	
	if (!dashf(fname))
	{
		if (askyn("该用户尚未被授予永6帐号，是否授予?", NA, NA) == YEA)
		{
			fp = fopen(fname, "w+");
			if (fp)
			{
				fprintf(fp, "%s", currentuser.userid);
				fclose(fp);
				char secu[STRLEN];
                     	        sprintf(secu, "授予 %s 永6帐号", lookupuser.userid);
                                security_report(secu,1);
				prints("已经授予该用户永6帐号");
			}
			else
				prints("创建控制文件错误");
		}
	}
	else
	{
		if (askyn("该用户已经被授予永6帐号，是否取消?", NA, NA) == YEA)
		{
			unlink(fname);
           		char secu[STRLEN];
                        sprintf(secu, "收回 %s 永6帐号", lookupuser.userid);
                        security_report(secu,1);

			prints("已经取消该用户永6帐号");
		}
	}
	pressreturn();
	return 0;
}
