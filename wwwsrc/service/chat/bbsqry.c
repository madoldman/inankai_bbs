/*$Id: bbsqry.c,v 1.3 2009-10-30 04:06:46 maybeonly Exp $
*/

#include "BBSLIB.inc"
//huangxu@060405:Web查看奖章
char * cmedal2 (unsigned int original, int num, int place)
{
	int	i;
	char retstr [256];
	strcpy (retstr, "");
	if (num <= 1)	// Efan: 奖章种类数小于 2 的情况
	{
		hprintf (cmedal (place));
		return;
	}
	int tmp;
	for (i = 7; i > 0; --i) {
		tmp = (original >> (i << 2)) & 0x0f;
		if (tmp) {
			char tmpstr [256];
			strcpy (tmpstr, cmedal(8-i));
			tmpstr[9]=0;
			tmpstr[29]=0;
			//"\33[1;31m★ \33[1;33m离任站务奖章\33[m"
			// 0  1234567891  1234567892123456789  31
			printf("<span title='%s %d枚'>",tmpstr+17,tmp);
			hprintf("%s\33[m",tmpstr);
			printf("</span>");
		}
	}
	hprintf("");
	*retstr=0;
	return;
	
}

int main()
{
        FILE *fp, *bmfp;
        char userid2[14], filename[80], buf[512], permstr[11], bmlist[200], life[80], boardmaster[80], board[20], ipstr[80];
        struct userec *x;
        struct user_info *u;
        int i, n, tmp1, tmp2, tmpnum,num, clr, tmpmode, Reclusion=0, hideip;
	unsigned int tmpi;
        char now[80];
        init_all();
        modify_mode(u_info,QUERY+20000);	//bluetent
        strsncpy(userid2, getparm("userid"), 13);
        char *userid=trim(userid2);
        printf("<body background=%s  bgproperties=fixed>",mytheme.bgpath);
        printf("<center>");
        if(!strcmp(userid,"guest")){
               redirect("/reginfo.htm"); 
	       return 0;          
        }
 
        if(userid[0]==0) {
                printf("%s -- 查询网友<hr color=green>\n", BBSNAME);
                printf("<form action=bbsqry name=qry>\n");
                printf("请输入用户名: <input name=userid maxlength=12 size=12>\n");
                printf("<input type=submit value=查询用户>\n");
                printf("</form><hr>\n");
                printf("<script>document.qry.userid.focus();</script>");
                http_quit();
        }
        x=getuser(userid);
        if(x==0) {
                printf("用户 [%s] 不存在.", userid);
                http_quit();
        }
        printf("</center>\n");
        printf("<table align=center><tr><td><pre style='font-size:14px'>");
        sprintf(buf, "%s ([33m%s[37m) 共上站 [32m%d[m 次  ",
                x->userid, x->username, x->numlogins);
        hprintf("%s", buf);
        //huangxu@060401:Web性别
      	if ( HAS_DEFINE(x->userdefine, DEF_S_HOROSCOPE) )
	   			clr = (x->gender == 'F') ? 35 : 36;
				else
				/***********************************************************/
				clr = 32;	/* 安能辨我是雄雌 ?! :D */
        if(x->userdefine & DEF_S_HOROSCOPE)
                hprintf("[[1;%dm%s[m]", clr, horoscope(x->birthmonth, x->birthday));
        printf("\n");
//huangxu@060405:hideip作废
//        hideip = !HAS_PERM(PERM_SEEUSERIP)&&(!(x->userdefine&DEF_NOTHIDEIP))&&strstr(x->lasthost, "10.10.");
//        if(hideip)
//                strcpy(ipstr, "南开宿舍内网");
//        else
                strcpy(ipstr, x->lasthost);

        getdatestring(x->lastlogin,NA);
        hprintf("上次在: [[32m%s[37m] 从 [[32m%s[37m] 到本站一游。\n", datestring, ipstr);
        if(x->lastlogout < x->lastlogin) {
                getdatestring(((time(0)-x->lastlogin)/120)%47+1+x->lastlogin,NA);
        } else {
                getdatestring(x->lastlogout,NA);
        }
        hprintf("离站时间: [[32m%s[37m] ", datestring);
        count_mails(x->userid, &tmp1, &tmp2);
        hprintf("表现值: [[32m%d[37m]([33m%s[37m) ", countperf(x), perfstr(x));
        if(tmp2!=0)
        	printf("[<font color=green>信</font>]\n");
        else
		printf("\n");

	//printf("信箱: [<font color=green>%d</font>/%d]\n", tmp2,tmp1);
        hprintf("文章数: [[32m%d[37m](%s) ", x->numposts, numpostsstr(x));
        hprintf("经验值: [[32m%d[37m]([33m%s[37m) ", countexp(x), expstr(x));
        hprintf("生命力：[[32m%d[37m]\n", count_life_value(x));
	
#ifdef ALLOW_MEDALS
        /* Efan: 奖章数	*/
	//added by lionel
	/*int sum = 0;
	for(i = 1; i <= 7; i++){
		sum += (x -> nummedals >> (i << 2)) & 0x0f;
	}
        hprintf ("奖章数：[\33[1;32m%d\33[37m] (%s):%d\n", sum, cmedal (x->nummedals),x->nummedals );*/
	tmpnum = x->nummedals;
	tmp1 = 0;  // 根据位置判断奖章类型
	tmpi=0;
	for (i = 1; i <= 7; ++i) {
		int tmp = (tmpnum >> (i << 2)) & 0x0f;
		if (tmp != 0) {
			tmpi += tmp;
			tmp1 = 8 - i;
		}
	}
	tmpnum &= 0x0f;
	hprintf ("奖章数：[\33[1;32m%d\33[37m] (", tmpi);
	cmedal2 (x->nummedals,tmpnum,tmp1);
	hprintf (")\n");		
#endif

        uleveltochar(&permstr, x->userlevel);
        hprintf("用户权限: [%s]", permstr);
        show_special(x->userid);
        if(!(x->userlevel&PERM_SYSOP)&&(x->userlevel&PERM_RECLUSION)) {
                sprintf(buf, "home/%c/%s/Reclusion", toupper(x->userid[0]), x->userid);
                if(fp=fopen(buf,"r")) {
                        fscanf(fp, "%d", &Reclusion);
                        fclose(fp);
                }
                sprintf(life,"[\033[36m归隐中，还有%d天\033[m]", (Reclusion-(time(0)-x->lastlogout)/86400)>0?(Reclusion-(time(0)-x->lastlogout)/86400):0);
        } else
                sprintf(life,"");

        if (x->userlevel&PERM_BOARDS) {
                //strcpy(bmlist,"");
				*bmlist = 0;
                sprintf(boardmaster, "%s/home/%c/%s/.bmfile", BBSHOME, toupper(x->userid[0]),x->userid);
                if ((bmfp = fopen(boardmaster, "rb")) != NULL) {
                        int flag=0;
                        for(n=0; n < 3; n++) {
                                if (feof(bmfp))
                                        break;
                                fscanf(bmfp,"%s\n",board);
                                if(getbcache(board)==NULL)//modified by bluetent
                                {
                                        flag=1;
                                        continue;
                                }
                                if(n&&flag!=1)
								{
									strcat (bmlist, "、");
								}
                                sprintf(bmlist + strlen(bmlist), "<a href=\"bbsdoc?board=%s\">%s</a>", board, board);
                                //else
                                        //sprintf(bmlist,"%s、%s",bmlist,board);
                        }
                }
        }
        if ((x->userlevel&PERM_BOARDS)&&strcmp(bmlist,"")&&strcmp(x->userid,"SYSOP"))
                printf(" 现任%s板板务\n",bmlist);
        else
                hprintf("%s\n", life);
        //
        num=0;
        for(i=0; i<MAXACTIVE; i++) {
                u=&(shm_utmp->uinfo[i]);
                if(!strcmp(u->userid, x->userid)) {
                        if(u->active==0 || u->pid==0 || u->invisible && !HAS_PERM(PERM_SEECLOAK) && strcmp(u->userid,currentuser.userid) && !canseeme(u->userid))
                                continue;//huangxu@060714:mylover
                        num++;
                        if(num==1)
                        	//huangxu@060401:和telnet下统一
                                hprintf("目前 %s 状态如下:   ",u->userid);
                        if(u->invisible)
                                hprintf("[36m[隐][37m");
                        tmpmode=u->mode;
                        if (tmpmode>20000)
                        {
                                tmpmode-=20000;
                                hprintf("[37m@");//huangxu@060405
                        }
                        hprintf("[32m%s[m ", ModeType(tmpmode));
                        if(num%5==0)
                                printf("\n");
                }
        }
        if(num==0) {
                /*	if(x->lastlogout < x->lastlogin){
                         now = ((time(0)-x->lastlogin)/120)%47+1+x->lastlogin;
                      } else  {
                         now = x->lastlogout;
                      }	 
                */
                //		sprintf(filename, "home/%c/%s", toupper(x->userid[0]), x->userid);
                //		hprintf("目前不在站上, 上次离站时间 [[1;32m%s[m]\n\n", Ctime(file_time(filename)));
                //		hprintf("目前不在站上, 上次离站时间 [[1;32m%s[m]\n\n", now);

        }
        printf("\n");
        sprintf(filename, "home/%c/%s/plans", toupper(x->userid[0]), x->userid);
        fp=fopen(filename, "r");
        if(fp) {
                while(1) {
                        if(fgets(buf, 256, fp)==0)
                                break;
                        hhprintf(buf);
                }
                fclose(fp);
        } else {
                hprintf("[36m没有个人说明档[37m\n");
        }
        printf("</pre>");
        printf("<br><br><a href=bbspstmail?userid=%s&title=没主题>[写信问候]</a> ", x->userid);

        //printf("<a href=bbssendmsg?toid=%s target=f4>[发送讯息]</a>\n",x->userid);
        printf("<a href=bbsfadd?userid=%s>[加入好友]</a> ", x->userid);
        printf("<a href=bbsfdel?userid=%s>[删除好友]</a>", x->userid);

        sprintf(buf,"0Announce/PersonalCorpus/%c/%s",toupper(x->userid[0]),x->userid);
        if(file_exist(buf))
                printf("<a href=bbs0an?path=/PersonalCorpus/%c/%s >[参观%s的文集]</a>",toupper(x->userid[0]),x->userid,x->userid);

        //	printf("<hr>");
        printf("</table>");
        printf("</center>\n");
        http_quit();
}
