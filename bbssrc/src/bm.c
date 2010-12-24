#include "bbs.h"
extern  cmpbnames();
//add by tdhlshx for a deny bug which makes system not deliver the deny_mail&post
extern int digestmode;


int addtodeny(char *uident,char *msg,int ischange, int info_type)
{
	char buf[50],strtosave[256],buf2[50];
	time_t nowtime;
	int day, seek;

#ifndef  BOARD_READDENY

	info_type = 1;
#endif

	seek = SeekInNameList(uident);
	if(seek && !ischange)
	{
		move(3, 0);
		prints(" %s 已经在名单中。", uident);
		pressanykey();
		return -1;
	}
	if (ischange && !seek)
	{
		move(3,0);
		prints(" %s 不在名单中。", uident);
		pressreturn();
		return -1;
	}
	clear();
	while(1)
	{
		getdata(3,0,"输入说明: ", buf,40,DOECHO,YEA);
		if(killwordsp(buf)!=0)
			break;
	}
	nowtime=time(0);
	getdatestring(nowtime,NA);
	getdata(4,0,"输入封禁时间[缺省为 1 天, 0 为放弃]: ",buf2,4,DOECHO,YEA);
	if(buf2[0]=='\0')
		day =1;
	else
		day = atoi(buf2);
	if(day <= 0)
		return -1;
	//版主不能封ID超过一个月. tdhlshx 2003.5.24.
	if(day>31)
	{
		if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK)))
		{
			prints(" \n  对不起,您没有权限封ID超过31天  \n ");
			pressanykey();
			return -1;
		}
	}//add end
	nowtime += day * 86400;
	getdatestring(nowtime,NA);
	sprintf(strtosave,"%-12s %-40s %14.14s解封", uident,buf,datestring);
	if(!ischange)
	{
		sprintf(msg,
		        "\n  %s 网友: \n\n"
		        "\t我很抱歉地告诉您，您已经被取消在 %s 板的『%s』权力。\n\n"
		        "\t您被封禁的原因是 [%s]\n\n"
		        "\t现在决定停止你在本板的权力 [%d] 天。\n\n"
		        //	"\t请您于 [%14.14s] 向 %s 发信申请解封。\n\n",
		        "\t[%d] 天后,当您在本板发文时系统将自动解封。\n\n"  //自动解封
                        "\t如有异议，请与%s联系解决，或者直接到0区Appeal板参照有关格式进行投诉。\n\n"
    		        "\t\t\t\t\t%s 板 %s",
		        uident, currboard, info_type==1?"发文":"进入",buf, day,
		        day,currentuser.userid,currboard,currentuser.userid);
		//	datestring, currentuser.userid);
	}
	else
	{
		sprintf(msg,
		        "\n  %s 网友: \n\n"
		        "\t关于您在 %s 板被取消『%s』权力问题，现变更如下：\n\n"
		        "\t封禁的原因： [%s]\n\n"
		        "\t从现在开始，停止该权力时间： [%d] 天\n\n"
		        //	"\t请您于 [%14.14s] 向 %s 发信申请解封。\n\n",
		        "\t[%d] 天后,当你在本板发文时系统将自动解封 \n\n"  //自动解封
		        "\t\t\t\t\t\t\t\t\t\t\tBy %s",
		        uident, currboard, info_type==1?"发文":"进入",buf, day,
		        day,currentuser.userid);
		//	datestring, currentuser.userid);
	}
	if(info_type == 2)
		setbfile( genbuf, currboard,"board.deny" );
	else
		setbfile( genbuf, currboard,"deny_users" );
	if (ischange)
		del_from_file(genbuf, uident);
	return add_to_file(genbuf,strtosave);
}
/* Modify End. */

int deldeny(char *uident, int info_type)
{
	char    fn[STRLEN];
	if(info_type==2)
		setbfile(fn, currboard, "board.deny");
	else if(info_type==3)
		setbfile(fn, currboard, "board.allow");
	else
		setbfile(fn, currboard, "deny_users");
	return del_from_file(fn, uident);
}
#ifdef BOARD_CONTROL
/*int control_user()
{
	char  boardallow[STRLEN];
	int  cnt;

	setbfile(boardallow, currboard, "board.allow");
	if(!HAS_PERM(PERM_SYSOP))
	{
		if (!chk_currBM(currBM))
			return DONOTHING;
		if(!dashf(boardallow))
			return DONOTHING;
	}
	while(1)
	{
		cnt = ListFileEdit(boardallow,"编辑『本板成员』名单");

                char buf[256];
                sprintf(buf,"INFO %s 编辑成员名单",currboard);
                /*securityreport(buf);

		if(cnt == 0)
		{
			prints("\n本板成员名单为空，系统将自动取消本板的成员板面属性\n");
			if(askyn("您确定要取消本板的成员板面属性吗",NA,NA)==YEA)
			{
				unlink(boardallow);
				break;
			}
		}
		else
			break;
	}
	return FULLUPDATE;
}*/
int control_user()
{
	char  boardallow[STRLEN];
	char  boardwrite[STRLEN];
	int  cnt;
	char tmp[4];
	int mode=0;
	int setmode=0;
	setbfile(boardallow, currboard, "board.allow");
	setbfile(boardwrite ,currboard,"board.write");
	if (dashf(boardallow)) 
	{
		mode=1;
		if(dashf(boardwrite)) unlink(boardwrite);
	}
	if (dashf(boardwrite)) mode=2;
	if(HAS_PERM(PERM_OBOARDS))
	{
		clear();
		move(3,0);
		if(mode==1) prints("\033[1;31m本板已经设置允许进板名单\033[m");
		if(mode==2) prints("\033[1;31m本板已经设置允许发文名单\033[m");
		getdata(4,0,"设定：本板(1)允许进板名单 (2)允许发文名单 (0) 取消 [0]",tmp,2,DOECHO,YEA);
		if(tmp[0]==0)return FULLUPDATE;
		if(tmp[0]=='0') return FULLUPDATE;
		if(tmp[0]=='1')
		{
			if(mode==2) 
			{
				move(5,0);
				if(askyn("本板已经设置允许发文名单,确认删除吗?",NA,NA)==YEA)
				unlink(boardwrite);
				else return FULLUPDATE;
			}
			setmode=1;
		}
		if(tmp[0]=='2')
		{
			if(mode==1) 
			{
				move(5,0);
				if(askyn("本板已经设置进板名单,确认删除吗?",NA,NA)==YEA)
				unlink(boardallow);
				else return FULLUPDATE;
			}
			setmode=2;
		}
	}
	if(chk_currBM(currBM)&&!HAS_PERM(PERM_OBOARDS)) setmode=mode;
	if(setmode==0) return DONOTHING;
       while(1)
	{
		if(setmode==1) cnt = ListFileEdit(boardallow,"编辑『本板进板成员』名单");
		if(setmode==2) cnt = ListFileEdit(boardwrite,"编辑『本板发文成员』名单");
		char buf[256];
             if(setmode==1)  sprintf(buf,"编辑%s板进板成员名单",currboard);
             if(setmode==2)  sprintf(buf,"编辑%s板发文成员名单",currboard);
		securityreport4(buf);
    	if(cnt == 0&&HAS_PERM(PERM_OBOARDS))
		{
				prints("\n本板成员名单为空，系统将自动取消本板的俱乐部板面属性\n");
				if(askyn("您确定要取消本板的俱乐部板面属性吗",NA,NA)==YEA)
				{
					if(setmode==1) unlink(boardallow);
					if(setmode==2) unlink(boardwrite);
					break;
				}
		}
		else
			break;
	}
	return FULLUPDATE;
}      /* loveni:俱乐部版面*/
#endif

int BM_ontop (int ent, struct fileheader* fileinfo, char* direct)
{
	int n;
	FILE *fp;
	struct fileheader fh;
	int size = sizeof (struct fileheader);
	char genbuf [STRLEN];
	if (!chk_currBM (currBM) )
		return DONOTHING;

	if (fileinfo->accessed[1] & FILE_ONTOP)
		return DONOTHING;
	setbfile (genbuf, currboard, ".top");
	n = get_num_records (genbuf, size);
	if (fileinfo->accessed[1] & FILE_TIP)
	{
		int fd;
		int nfh;


		sprintf (genbuf, "文章取消置顶：●%-.55s", fileinfo->title);
		if (askyn(genbuf, NA, YEA) == NA)
		{
			move(t_lines - 1, 0);
			prints("放弃...");
			clrtoeol();
			egetch();
			return PARTUPDATE;
		}
		setbfile (genbuf, currboard, ".top");
		fd = open (genbuf, O_RDWR);
		if (fd != -1)
		{
			while ((nfh = read (fd, &fh, size)) == size)
				if (!strcmp (fileinfo->filename, fh.filename))
					break;
			if (nfh == size)
			{
				while (read (fd, &fh, size) == size)
				{
					lseek (fd, -2 * size, SEEK_CUR);
					write (fd, &fh, size);
					lseek (fd, size, SEEK_CUR);
				}
				close (fd);
				truncate (genbuf, (n-1) * size);
			}
			else
			{
				//by brew
				close(fd);
			}

		}

		fileinfo->accessed[1] &= ~FILE_TIP;
		substitute_record (direct, fileinfo, size, ent);
		return FULLUPDATE;
	}

	if (n >= MAX_BRD_ONTOP)
	{
		move (3, 0);
		clrtobot ();
		move (5, 0);
		prints ("        本板置顶文章数已达上限，不能再设置置顶文章!");
		pressreturn ();
		return FULLUPDATE;
	}
	fp = fopen (genbuf, "ab+");
	if (!fp)
	{
		move (3, 0);
		clrtobot ();
		move (5, 0);
		prints ("        不能打开文件，置顶失败!");
		pressreturn ();
		return FULLUPDATE;
	}

	memcpy (&fh, fileinfo, size);
	fh.accessed [1] |= FILE_ONTOP;
	fh.accessed [0] = 0;  //modified bylivebird
	if (fileinfo->accessed [0]& FILE_NOREPLY)
		fh.accessed [0] |= FILE_NOREPLY;
	strcpy (fh.owner, fileinfo->owner);
	//strcpy (fh.owner, currentuser.userid);
	char ch[4];
      ch[0] = 0;
      getdata (t_lines - 1, 0,"使用置底提示: 1)\33[1;33m[提示]\33[m 2)\33[1;31m[公告]\33[m 3)\33[1;35m[板规]\33[m 4)\33[1;32m[推荐]\33[m 5)\33[1;34m[精华]\33[m 6)\33[1;36m[其它]\33[m \33[1;33m[1]\33[m:",  ch, 3, DOECHO, YEA);

    if (ch[0] == '2') fh.accessed[1] |= FILE_STYLE1;	
    if (ch[0] == '3') fh.accessed[1] |= FILE_STYLE2;
    if (ch[0] == '4') fh.accessed[1] |= FILE_STYLE3;
    if (ch[0] == '5') fh.accessed[1] |= FILE_STYLE4;
    if (ch[0] == '6') fh.accessed[1] |= FILE_STYLE5;
	if (fwrite (&fh, size, 1, fp) != 1)
	{
		move (3, 0);
		clrtobot ();
		move (5, 0);
		prints ("        不能写入文件，置顶失败!");
		pressreturn ();
		return FULLUPDATE;
	}
	fileinfo->accessed [1] |= FILE_TIP;
	substitute_record(direct, fileinfo, size, ent);
	fclose (fp);
	move (t_lines -1, 0);
	clrtoeol ();
	sprintf (genbuf, "文章置底：●%s", fileinfo->title);
	prints (genbuf);
	egetch ();
	return FULLUPDATE;
}

int deny_user()
{
	char    uident[STRLEN], ans[8], repbuf[STRLEN];
	char    msgbuf[512], buf[50];
	int     count, info_type;

	if (!chk_currBM(currBM))
		return DONOTHING;
	clear();
	//add by tdhlshx 2003.6.24. for a bug that when deny sb. in special mode
	if(digestmode!=NA)
	{
		move(3,5);
		prints("请到一般模式下执行封禁功能. ^_^ ");
		pressanykey();
		return DONOTHING;
	}   //add end
	/*   if (check_anonyboard(currboard)==YEA&&!HAS_PERM(PERM_SYSOP)){
	      prints("\n匿名版版主不知道发文者身份, 故不应知道该限制谁发表文章\n");
	      prints("因此设定无法 Post 的名单之权限不开放给版主使用\n");
	      prints("若有需要设定到此名单, 请向通知sysop处理\n");
	      pressreturn();
	      clear();
	      return FULLUPDATE;
	   }
	*/
#ifndef  BOARD_READDENY
	info_type = 1;
#else

	if(!HAS_PERM(PERM_SYSOP))
	{
		setbfile(genbuf, currboard, "board.deny");
		if(!dashf(genbuf))
			info_type = 1;
		else
			info_type = 2;
	}
	else
		info_type = 2;
	if(info_type == 2)
	{
		getdata(3,0,"设定：本板(1)禁止发文名单 (2)禁止进入名单 (0) 取消 [1]",
				ans,2,DOECHO,YEA);
		if(ans[0] == '0')
			return FULLUPDATE;
		if(ans[0]!='2')
			info_type = 1;
		else
			info_type = 2;
		if(info_type == 2)
		{
			move(5,0);
			prints("您将要设定本板的 [禁止进入] 名单\n设定 [禁止进入] 名单后，");
			prints("所有被加入 [禁止进入] 名单的 ID 均不可以进入本板。\n");
			move(8,0);
			if(askyn("您确定要设定该名单么",NA,NA)==NA)
				return FULLUPDATE;
		}
	}
#endif
	while (1)
	{
		clear();
		prints("设定本板的 [%s] 名单\n", info_type==1?"禁止发文":"禁止进入");
		if(info_type == 1)
			setbfile(genbuf, currboard, "deny_users");
		else
			setbfile(genbuf, currboard, "board.deny");
		count = listfilecontent(genbuf, 3);
		if (count)
		{
			/*      //add for 自动解封
			    if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK)))
				    getdata(1,0,"(A)增加  (C)改变 or (E)离开[E]: ",ans,7,DOECHO,YEA);
			    else//add end*/
			getdata(1, 0, "(A)增加 (D)删除 (C)改变 or (E)离开[E]: ",
			        ans, 7, DOECHO, YEA);
		}
		else
		{
			getdata(1, 0, "(A)增加 or (E)离开 [E]: ",
			        ans, 7, DOECHO, YEA);
		}
		if (*ans == 'A' || *ans == 'a')
		{
			move(2, 0);
			usercomplete("输入准备加入名单的使用者ID: ", uident);
			if(*uident!='\0'&& !strcmp(uident,"guest"))
			{
				move(3,0);
				clrtobot();
				move(6,3);
				prints("faint...guest 也不放过?人家本来就不能发文章么.");
				pressanykey();
				return DONOTHING;
			}
			if (*uident != '\0' && getuser(uident))
			{
				if (addtodeny(uident,msgbuf,0,info_type) == 1)
				{
					sprintf(repbuf, "%s被取消在%s板的%s权限",
					        uident,currboard, info_type==1?"发文":"进入");
					securityreport4(repbuf);
					if(msgbuf[0]!='\0')
						autoreport(repbuf,msgbuf,YEA,uident);
				}
			}
		}
		else if ((*ans == 'C'|| *ans == 'c'))
		{
			//add tdhlshx,只有站长才能改封人处罚
			/*    if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK))){
			    
			   	       prints(" \n        请勿轻易更改处罚决定.如果必要,请向站务发信说明.^_^  \n ");
			             pressanykey();
			             return FULLUPDATE;                             
			     }
			 但愿不要被启用. */
			move(2,0);
			namecomplete("请输入被修改者 ID: ", uident);
			if( *uident != '\0' )
			{
				if(addtodeny(uident,msgbuf,1,info_type)==1)
				{
					sprintf(repbuf, "修改对%s被取消%s权限的处理",
					        uident,info_type==1?"发文":"进入");
					securityreport4(repbuf);
					if(msgbuf[0]!='\0')
						autoreport(repbuf,msgbuf,YEA,uident);
				}
			}
		}
		else if ((*ans == 'D' || *ans == 'd') && count)
		{
			/*	      //add for 自动解封
				      if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK)))
					      return FULLUPDATE;*/
			move(2, 0);
			namecomplete("请输入被取消者 ID: ", uident);
			if (uident[0] != '\0'&& SeekInNameList(uident))
			{
				//	    while(1)
				//		{
				getdata(3,0,"输入解封附言: ", buf,40,DOECHO,YEA);
				//	      if(killwordsp(buf)!=0)break;
				//	    }
				if (deldeny(uident,info_type))
				{
					sprintf(repbuf, "恢复%s在%s板的%s权限",
					        uident,currboard, info_type==1?"发文":"进入");
					securityreport4(repbuf);
					sprintf(msgbuf, "\n  %s 网友：\n\n"
					        "\t因封禁时间已过，现恢复您在 [%s] 板的『%s』权利。\n\n",
					        uident, currboard,info_type==1?"发表文章":"进入本板");
					if(strcmp(buf, ""))
						sprintf(msgbuf, "%s\t解封附言：[%s]\n", msgbuf, buf);
					autoreport(repbuf,msgbuf,YEA,uident);
				}
			}
		} /*else if(*ans == 'R' || *ans == 'r') {
		         if(info_type == 1)setbfile(genbuf, currboard, "deny_users");
		         else setbfile(genbuf, currboard, "board.deny");
		         unlink(genbuf);
			 sprintf(repbuf, "删除 [%s 板%s]名单", currboard, 
			    info_type==1?"禁止发文":"禁止进入");
			 securityreport(repbuf);
			 return FULLUPDATE;
		      }*/ else
			break;
	}
	clear();
	return FULLUPDATE;
}
//add by yiyo 区段操作
int Rangefunc(int ent, struct fileheader *fileinfo, char *direct)
{
	struct fileheader fhdr;
	char annpath[512];
	char buf[STRLEN],ans[8],info[STRLEN],bname[STRLEN],dbname[STRLEN];
	char items[8][7] =
	    {"保留","文摘","不可RE","删除","精华区","暂存","转载","恢复"};
	int type, num1,num2,i,max=8;//,full=0;
	int fdr, ssize;
	int punish;
	extern int SR_BMDELFLAG;
	extern char quote_file[120], quote_title[120], quote_board[120];
	if (!strcmp(currboard, "junk") || !strncmp(currboard, "syssecurity",11))
		return DONOTHING;
	if(uinfo.mode != READING)
		return DONOTHING;
	if(!chk_currBM(currBM))
		return DONOTHING;
	saveline(t_lines - 1, 0);
	//if(strcmp(currboard, "deleted")&&strcmp(currboard,"junk"))
	if(!HAS_PERM(PERM_SYSOP))
		max=7;
	strcpy(info,"区段:");
	for(i=0;i<max;i++)
	{
		sprintf(buf," %d)%s",i+1,items[i]);
		strcat(info,buf);
	}
	strcat(info," [0]: ");
	getdata(t_lines-1,0,info,ans,2,DOECHO,YEA);
	type = atoi(ans);
	if(type <= 0 || type > max)
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	if(   digestmode&&(type==2||type==3||type==7)// modified by livebird del type == 8
	        || digestmode>1&&type!=5&&type!=6&&type!=8 )
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	move(t_lines-1,0);
	clrtoeol();
	prints("区段操作: %s",items[type-1]);
	getdata(t_lines-1,20,"首篇文章编号: ",ans,6,DOECHO,YEA);
	num1=atoi(ans);
	if(num1>0)
	{
		getdata(t_lines-1,40,"末片文章编号: ",ans,6,DOECHO,YEA);
		num2=atoi(ans);
	}
	if(num1<=0||num2<=0||num2<=num1)
	{
		move(t_lines-1,60);
		prints("操作错误...");
		egetch();
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	if(type != 7)
	{
		sprintf(info,"区段 [%s] 操作范围 [ %d -- %d ]，确定吗",
		        items[type-1], num1,num2);
		if(askyn(info,NA,YEA)==NA)
		{
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
	}
	/* add by yiyo 从严删文 */
	if(type == 4)
	{
		move(t_lines - 1, 0);
		if(askyn("严惩么[文章数减2]",NA,NA)==NA)
			punish=0;
		else
			punish=1;
	}
	/* add end */
	if(type == 5)
	{
		FILE *fn;
		sethomefile(annpath, currentuser.userid,".announcepath");
		if((fn = fopen(annpath, "r")) == NULL )
		{
			presskeyfor("对不起, 你没有设定丝路. 请先用 f 设定丝路.",t_lines-1);
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
		fscanf(fn,"%s",annpath);
		fclose(fn);
		if (!dashd(annpath))
		{
			presskeyfor("你设定的丝路已丢失, 请重新用 f 设定.",t_lines-1);
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
	} //else if( type == 6 ){
	// if(askyn("[收入暂存档模式: 一般/精简] 采用精简方式吗",YEA,YEA)==NA)
	//   full = 1;
	//   }
	else if ( type == 7 )
	{
		clear();
		prints("\n\n您将进行区段转载。转载范围是：[%d -- %d]\n",num1,num2);
		prints("当前板面是：[ %s ] \n", currboard);
		if(!get_a_boardname(bname,"请输入要转贴的讨论区名称: "))
			return FULLUPDATE;
		if(!strcmp(bname,currboard)&&uinfo.mode != RMAIL)
		{
			prints("\n\n对不起，本文就在您要转载的板面上，所以无需转载。\n");
			pressreturn();
			clear();
			return FULLUPDATE;
		}
		if(askyn("确定要转载吗",NA,NA)==NA)
			return FULLUPDATE;
	}
	if((fdr = open(direct,O_RDONLY,0)) == -1)
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	ssize = sizeof(struct fileheader);
	if(lseek(fdr, (num1-1)*ssize, SEEK_SET)==-1)
	{
		close(fdr);
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	while(read(fdr,&fhdr,ssize) == ssize)
	{
		if(num1 > num2 )
			break;
		switch(type)
		{
		case 1:
			mark_post(num1, &fhdr, direct);
			break;
		case 2:
			digest_post(num1, &fhdr, direct);
			break;
		case 3:
			underline_post(num1, &fhdr, direct);
			break;
		case 4:
			{
				int result;
				SR_BMDELFLAG = YEA;
				result = del_post(num1, &fhdr, direct,punish);
				SR_BMDELFLAG = NA;
				if(result == -1)
				{
					close(fdr);
					return DIRCHANGED;
				}
				if (result != DONOTHING)
				{
					lseek(fdr, (-1)*ssize, SEEK_CUR);
					num1 --;
					num2--;
				}
				break;
			}
		case 5:
			fhdr.accessed[0] |= FILE_VISIT;
			substitute_record(direct, &fhdr, sizeof(fhdr), num1);
			a_Import("0Announce", currboard, &fhdr, annpath, YEA);
			break;
		case 6:
			//           a_Save("0Announce", currboard, &fhdr, YEA, full);
			a_Save("0Announce", currboard, &fhdr, YEA);
			break;
		case 7:
			if (uinfo.mode != RMAIL)
				sprintf(genbuf, "boards/%s/%s", currboard, fhdr.filename);
			else
				sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]),
				        currentuser.userid, fhdr.filename);
			strncpy(quote_file, genbuf, sizeof(quote_file));
			quote_file[sizeof(quote_file) - 1] = '\0';
			strcpy(quote_title, fhdr.title);
			strcpy(quote_board, currboard);
			strcpy(dbname, currboard);
			strcpy(currboard, bname);
			post_cross('l', 0);
			strcpy(currboard, dbname);
			break;
		case 8:
			SR_BMDELFLAG = YEA;
			UndeleteArticle(num1,&fhdr,direct);
			SR_BMDELFLAG = NA;
			break;
		}
		num1 ++;
	}
	close(fdr);
	saveline(t_lines - 1, 1);
	return DIRCHANGED;
}


/*成功返回问题个数,失败返回0
  对模板文件进行分析,生成二进制的格式文件
*/
int dealquestion(char *buf,int *size)
{
        int len=strlen(buf),i=0;
        int start=0,end=len-1;
        for(;i<len;i++)
                if(buf[i]==' ') {
                        start=i;
                        break;
                }

        for(i=len-1;i>start;i--)
                if(buf[i]>'9'||buf[i]<'0') {
                        end=i;
                        break;
                }
        if(start==0){
            prints("缺少序号");
            return 1;
        }
        if(end==len-1){
            prints("缺少字数限制");    
            return 1;
        }
        *size=atoi(buf+end+1);
        if(*size>AS)
                *size=AS;
        else if(*size<0)
                *size=8;
        buf[end]='\0';
        strcpy(buf,buf+start);
        return 0;
}
/*读模板文件正文,分析之   
*/
int writebin(char *s,int q,FILE *fp)
{
        fwrite(&q,sizeof(int),1,fp);
	int size=strlen(s);
	fwrite(&size,sizeof(int),1,fp);
        fwrite(s,size,1,fp);
}
int readbin(char *s,int *q,FILE *fp)
{
	int tmp;
        if(fread(&tmp,sizeof(int),1,fp)<=0) return 0;
        if(tmp==9999) return 0;
        *q=tmp;
        fread(&tmp,sizeof(int),1,fp);
        fread(s,tmp,1,fp);
        return 1;
 
}
int patterncompile(char *file)
 {
        char buf[256];
        FILE *fp=fopen(genbuf, "r+");
		sprintf(buf,"%s.question",file);
        FILE *fq=fopen(buf,"w+");
        sprintf(buf,"%s.bin",file);
        FILE *fbin=fopen(buf,"w+");

		clear();
        if(fp==0||fq==0||fbin==0){
			prints("文件打开失败.");
			pressreturn();
			return 0;
		}
   
        struct question q;
	bzero(&q,sizeof(q));

        int titlefound=0;
        int beginfound=0;
        int i=0,size=0,error=0;
        int toomany=0;
		

        while(1) {
                if(fgets(buf,256,fp)<=0)    break;
                if(buf[0]=='#')  continue;
                if(!strcmp(buf,"\n")) continue;
                if(!strncmp(buf,"title=",6)) {
                        if(buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]='\0';
                        titlefound=1;
                        char *ptr=strstr(buf+6,"$0");
                        if(ptr==0)
                             strncpy(q.title1,buf+6,STRLEN);
			else {
                                *ptr='\0';
                                strncpy(q.title1,buf+6,STRLEN);
                                strncpy(q.title2,ptr+2,STRLEN);
                                continue;
                        }
                }

                if(titlefound){
                        if(strstr(buf,"begin:")){
                                beginfound=1;
                                while(1) {
                                        if(fgets(buf,256,fp)<=0) break;
                                        if(buf[0]=='#')  continue;
                                        int len=strlen(buf);
                                        int last=0;
                                        int m=0,index=0,step=0;
                                        for(;m<=len;m++) {
                                                if(buf[m]=='$') {
                                                        if(!isdigit(buf[m+1]))
                                                                continue;
                                                        if(isdigit(buf[m+2])) {
                                                                step=3;
                                                                index=(buf[m+1]-'0')*10+buf[m+2]-'0';
                                                        } else {
                                                                step=2;
                                                                index=buf[m+1]-'0';
                                                        }
                                                        if(index<=i){
                                                                buf[m]='\0';
		 writebin(buf+last,index,fbin);
                                                                last=m+step;
                                                                m+=step-1;
                                                        }
                                                }//if
                                        }//for
                writebin(buf+last,8888,fbin);
                                }//while
	        writebin("",9999,fbin);
                                break;
                        }//if beginfound

                        if(!toomany) {
			        i++;
                                if(buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]='\0';
                                if(dealquestion(buf,&size)==1){
									error++;
									prints(": Line %d\n",i);
								}else{	
									if(i<=MQ){
										strcpy(q.q[i-1].con,buf);
										q.q[i-1].size=size;
									}else{
										 prints("系统只支持%d个问题.多余的将不予显示.\n",MQ);
										 toomany=1;
										 i=MQ;
									}
								}							
						}//if toomany
                }//if titlefound
        }//while(1)
       
        if(i==0){
           prints("错误:没有找到模板问题.您必须设置问题,以被用户作答.\n");
           error++;
	}

        if(!titlefound) {
                prints("错误:找不到模板标题.标题是一行以\"title=\"开始的文字.\n");
                error++;
        }
        if(!beginfound) {
                prints("错误:找不到模板正文开始标识,\"begin:\"单独占一行.\n");
                error++;
        }

		prints("共 %d 个错误.\n",error);
		
    	if(error){
		    pressreturn();
	            fcloseall();	
           	    return 0;
	}
	else{
	  	    prints("恭喜,有效的模板文件!");
	  	    fwrite(&q,sizeof(struct question),1,fq);
		    pressreturn();
                    fcloseall();
                    return i;
        }
 }


 
int editpattern()
{
   char patternrecord[STRLEN];
   setbfile(patternrecord, currboard, "pattern");
   if(!HAS_PERM(PERM_SYSOP))
       if (!chk_currBM(currBM)) return DONOTHING;

   
   int sel,i,aborted;
   struct pattern p;
   char ans[3];
   while (1) {
            int n=get_num_records(patternrecord,sizeof(struct pattern));
            clear();
            prints("现有模板数:%4d\n", n);
           
	    move(2, 0);
  	    clrtoeol();
   	    showpattern(patternrecord);
        getdata(1,0,"(A)增加 (D)删除 (M)修改模板设定 (E)编辑模板文件 (O)完成 [O]: ",
         ans, 2, DOECHO, YEA);
	    if(ans[0]=='0'||ans[0]=='O') break;
        if(ans[0]!='a'&&ans[0]!='A'&&ans[0]!='d'&&ans[0]!='D'
		  &&ans[0]!='M'&&ans[0]!='m'&&ans[0]!='E'&&ans[0]!='e'){
	     
	    break; 
        }	     
      if (ans[0]=='m'||ans[0]=='d'||ans[0]=='e'||ans[0]=='D'||ans[0]=='M'||ans[0]=='E'){
         
         char ch[8];
		 getdata(t_lines-2,0, "请输入序号(enter返回):", ch, 3, DOECHO, YEA);
         sel=atoi(ch);
		 if (ch[0]=='\0') continue;
	     if(sel>n||sel<=0) {
			 prints("错误的序号.");
			 continue;
		 }
      }
     
	  char mybuf[STRLEN];
      switch (ans[0]) {
         case 'A':
         case 'a': 
                           clear();
			   move(2,0);
               if(n>=MP) {
			     prints("每个板面模板总数不得超过%d.\n", MP);
				 break;
			   }
			   bzero(&p,sizeof(p));
		       getdata(2,0, "模板名称:", genbuf, 40, DOECHO, YEA);
			   strcpy(p.name,genbuf);
			  
			   Q:
			   sprintf(mybuf,"问题个数(最多%d个,0取消):",MQ);
  		       getdata(3,0, mybuf, genbuf, 3, DOECHO, YEA);
			   i=atoi(genbuf);
			   if(i==0) continue;
			   if(i<0||i>MQ) goto  Q;
			   p.question=i;
			   sprintf(p.file,"%d",time(0));
			   p.time=time(0);
			   strcpy(p.author,currentuser.userid);
			   append_record(patternrecord,&p,sizeof(p));
			
                           presskeyfor("按任意键开始编辑发文模板文件.");  
			   setbfile(genbuf,currboard,p.file);
              		   ED:
			     aborted = vedit(genbuf, NA, YEA);
          		     if (aborted == -1) {
				   pressreturn();
				   continue;
          		     } 
			   int e=patterncompile(genbuf);
			   if(e==0){
               		   if(askyn("再编辑吗?", YEA, NA) == NA){
                       	   sprintf(mybuf,"%s.question",genbuf);
                           unlink(mybuf);
                           sprintf(mybuf,"%s.bin",genbuf);
			   unlink(mybuf);
                           clear();
                           move(10,30);
 		           presskeyfor("该模板目前处于不可用状态."); 
			   continue;
			   }
                           goto  ED;
			   }else{
			       if(e<p.question){
				     prints("该模板的实际问题数与设置的不一致,请注意修改."); 
   				     egetch();
				   }
			   }
			 
			   break;
               
         case 'D':
         case 'd':
               sprintf(genbuf,"确定删除第%d个模板吗?",sel);
   	           if (askyn(genbuf, NA, NA) == NA){
			       prints(" 取消"); 
				   egetch();
				   continue;
			   }
			   get_record(patternrecord,&p,sizeof(p),sel);
			   setbfile(genbuf,currboard,p.file);
			   unlink(genbuf);
			   sprintf(mybuf,"%s.question",genbuf);
               unlink(mybuf);
               sprintf(mybuf,"%s.bin",genbuf);
			   unlink(mybuf);
			   delete_record (patternrecord, sizeof (struct pattern), sel);  
            break;
	     case 'E':
	     case 'e':
                            
                   get_record(patternrecord,&p,sizeof(p),sel);
                   setbfile(genbuf,currboard,p.file);    		       
  			       p.time=time(0);
			       strcpy(p.author,currentuser.userid);
   			       substitute_record(patternrecord,&p,sizeof(p),sel);
				   goto ED;
			   
         case 'M':
         case 'm':
                   clear();
                   move(2,0);
		              get_record(patternrecord,&p,sizeof(p),sel);
  			       p.time=time(0);
				   strcpy(p.author,currentuser.userid);
				   getdata(2,0, "模板新名称:", genbuf, 40, DOECHO, YEA);
			       strcpy(p.name,genbuf);
			       
			       QQ:
  		           getdata(3,0, "问题个数(最多40个,0取消):", genbuf, 3, DOECHO, YEA);
			       i=atoi(genbuf);
			       if(i==0) continue;
			       if(i<0||i>40) goto  QQ;
			       p.question=i;
				   sprintf(genbuf,"确定要更改吗?",sel);
    	           if (askyn(genbuf, NA, NA) == NA){
         			       prints(" 取消"); 
		       		       egetch();
				           continue;
			       }
				   substitute_record(patternrecord,&p,sizeof(p),sel);
            break;
			default:break;
          }
   }//while
}

int showpattern(char *file)
{
     struct pattern p;
     FILE *fp=fopen(file,"r");
     if(fp==0) return 1;
     int i=0;
     prints(" %-10s%-40s%10s","序号","名称","问题个数");
     prints("\n-------------------------------------------------------------\n");
     while(1){
        if(fread(&p, sizeof(p), 1, fp)<=0) break;
        i++;
        prints(" %-10d%-40s%10d\n",i,p.name,p.question);
     }
     fclose(fp);
     return 0;
}


int rmd_thesis (int ent, struct fileheader* fileinfo, char* direct)
{
	int n;
	FILE *fp;
	char thesispath [STRLEN];
	char filepath[STRLEN];
	struct rmd r;
	struct stat st;
	if (!chk_currBM (currBM) )
		return DONOTHING;
	setbfile(thesispath, currboard, ".thesis");
	fp=fopen(thesispath,"ab+");
	if(fp==NULL) {
		move (3, 0);
		clrtobot ();
		move (5, 10);
		prints ("打开文件失败,不能设置web推荐!");
		pressreturn ();
		fclose(fp);
		return FULLUPDATE;
	}
	n = 0;
	while(1) {
		if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
		    break;
		if(!strcmp(r.en, fileinfo->filename)) 
		{
			sprintf (genbuf, "取消推荐：●%-.55s", fileinfo->title);
			if (askyn(genbuf, NA, YEA) == NA)
			{
				move(t_lines - 1, 0);
				prints("放弃...");
				clrtoeol();
				egetch();
				fclose(fp);
				return PARTUPDATE;
			}
			else{
				if(r.sign==1) {
					move(3,0);
					clrtobot();
					move(5,0);
					prints("该文章已经被放在首页里面了，您不能够将其删除，请与讨论区总管联系");
					pressreturn();
					fclose(fp);
					return PARTUPDATE;
				}
				move(t_lines - 1, 0 );
			        if(delete_record(thesispath, sizeof(struct rmd), n + 1) != -1)
					prints("取消推荐成功！");
				else
					prints("取消失败！");
				clrtoeol();
				egetch();
				fclose(fp);
				return FULLUPDATE;
			}		
		}
		n ++;
	}
	if(n >= PMAXRMD && !HAS_PERM(PERM_SYSOP)) 
	{
		move(3,0);
		clrtobot ();
		move (5, 0);
		prints("您的推荐名额已满，板务最多可以推荐%d篇文章，您可以尝试删除以前的推荐。\n", PMAXRMD);
		prints("如果某篇文章已经被放在首页里面，您将不能够删除，请与讨论区总管联系删除");
		pressreturn ();
		fclose(fp);
		return FULLUPDATE;
	}
	sprintf (genbuf, "推荐文章：●%-.55s", fileinfo->title);
	if (askyn(genbuf, NA, YEA) == NA)
	{
		move(t_lines - 1, 0);
		prints("放弃...");
		clrtoeol();
		egetch();
		fclose(fp);
		return PARTUPDATE;
	}
	else
	{
		bzero(&r,sizeof(struct rmd));
		strcpy(r.owner, fileinfo->owner);
		strcpy(r.rmder, currentuser.userid);
		strcpy(r.en, fileinfo->filename);
		strcpy(r.cn, fileinfo->title);
		strcpy(r.board, currboard);
		r.sign=0;
		time(&r.rmdtime);
		setbfile(filepath, currboard, fileinfo->filename);
		fstat(filepath, &st);
		r.psttime = st.st_mtime;
		append_record(thesispath, &r, sizeof(struct rmd));
		move(t_lines - 1, 0 );
		prints("推荐文章成功.");
		clrtoeol();
		egetch();
		fclose(fp);
		return FULLUPDATE;
	}
}
