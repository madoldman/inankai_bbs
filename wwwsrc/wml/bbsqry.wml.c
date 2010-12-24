//bbsqry.wml.c
//huangxu@nkbbs.org

//查看用户资料用

#include "BBSLIB.inc"
#include "WMLLIB.inc"

//bbsqry.wml?userid=(1)

int bbsqry_act(char * info)
{
	char userid[IDLEN + 2];
	char buf[256], buf2[256];
	struct userec * x;
	struct user_info * u;
	int tmp1, tmp2, i;
	FILE * fp;
	strncpy (userid, getparm("userid"), IDLEN + 1);
	wmlmode(u_info, QUERY);
	if (!*userid)
		//显示表单
	{
		printf ("<card title=\"查询使用者 -- %s\">", BBSNAME);
		printf ("<p><input name=\"inp_userid\" title=\"用户名\" name=\"inp_id\" maxlength=\"%d\" /></p>", IDLEN);
		printf ("<p><anchor>查询<go href=\"bbsqry.wml\" method=\"post\"><postfield name=\"userid\" value=\"$(user_id:esc)\"/></go>\
</anchor>&#32;&#32;<anchor><prev />返回</anchor></p>");
		return 0;
	}
	x = getuser(userid);
	if (!x)
	{
		sprintf (info, "您查找的用户不存在。");
		return -1;
	}
	strcpy (userid, x->userid);
	printf ("<card title=\"查询用户%s -- %s\">", userid, BBSNAME);
	printf ("<p>");
	w_hprintf ("%s （%s） 共上站%d次", x->userid, x->username, x->numlogins);
	if (HAS_DEFINE(x->userdefine, DEF_S_HOROSCOPE))
	{
		w_hprintf (" [%s]%s ", horoscope(x->birthmonth, x->birthday), x->gender == 'F' ? "MM" : "GG");
	}
	count_mails(x->userid, &tmp1, &tmp2);
	if (tmp2)
	{
		w_hprintf ("[信]");
	}
	printf ("<br />");
	getdatestring(x->lastlogin, NA);
	w_hprintf ("上次在[%s]从[%s]登录，", datestring, x->lasthost);
	if(x->lastlogout < x->lastlogin) 
	{
		getdatestring(((time(0)-x->lastlogin)/120)%47+1+x->lastlogin,NA);
	}
	else
	{
		getdatestring(x->lastlogout,NA);
	}
	w_hprintf ("于[%s]离开。", datestring);
	printf ("<br />");
	w_hprintf ("文章数：[%d](%s) 经验值：[%d](%s) 表现值：[%d](%s) 生命力：[%d] ", x->numposts, numpostsstr(x), countexp(x), expstr(x), countperf(x), perfstr(x), count_life_value(x));
	// 只有奖章数量就可以了
	tmp2 = 0;
	for (i = 1; i <= 7; ++i) 
	{
		tmp2 += (x->nummedals >> (i << 2)) & 0x0f;
	}
	if (tmp2)
	{
		w_hprintf ("奖章数：[%d]", tmp2);
	}
	printf ("<br />");
	uleveltochar (buf, x->userlevel);
	printf ("权限：[%s] ", buf);
	fp = fopen("etc/sysops", "r");
	if (fp)
	{
		while(1) 
		{
			*buf = 0;
			*buf2 = 0;
			if(fscanf(fp, "%s %s", buf, buf2) <=0 )
			{
				break;
			}
			if(!strcmp(userid, buf))
			{
				w_hprintf("[%s] ", buf2);
				//也只要一个
				break;
			}
		}
		fclose(fp);
	}
	sprintf(buf, "home/%c/%s/.bmfile", toupper(x->userid[0]), x->userid);
	*buf2 = 0;
	if (fp = fopen(buf, "rt"))
	{
		for(i = 0; i < 3; i++)
		{
			if (feof(fp))
			{
				break;
			}
			fscanf (fp, "%s\n", buf);
			if (!getbcache(buf))
			{
				continue;
			}
			strcat (buf2, buf);
			strcat (buf2, "、");
		}
		fclose(fp);
	}
	if (*buf2)
	{
		*(buf2 + strlen(buf2) - 2) = 0;//最后一个顿号
		w_hprintf ("现任%s板板务 ", buf2);
	}
	if (!(x->userlevel & PERM_SYSOP) && (x->userlevel & PERM_RECLUSION))
	{
		sprintf(buf, "home/%c/%s/Reclusion", toupper(x->userid[0]), x->userid);
		if ((file_exist(buf)) && (fp = fopen(buf, "rt")))
		{
			fscanf(fp, "%d", &tmp1);
			fclose(fp);
		}
		w_hprintf ("[归隐中，还有%d天]", (tmp1 - (time(0) - x->lastlogout) / 86400) > 0 ? (tmp1 - (time(0) - x->lastlogout) / 86400) : 0);
	}
	else if (!(x->userlevel & PERM_SYSOP) && (x->userlevel & PERM_SUICIDE))
	{
		w_hprintf ("[自杀中]");
	}
	printf ("<br />");
	tmp2 = 0;
	for (i = 0; i < MAXACTIVE; i++)
	{
		u=&(shm_utmp->uinfo[i]);
		if(!strcmp(u->userid, x->userid))
		{
			if(u->active==0 || u->pid==0 || u->invisible && !HAS_PERM(PERM_SEECLOAK) && strcmp(u->userid,currentuser.userid) && !canseeme(u->userid))
			{
				continue;
			}
			if (!tmp2)
			{
				printf ("状态：");
			}
			tmp2++;
			if(u->invisible)
			{
				w_hprintf ("[隐]");
			}
			tmp1 = u->mode;//皑皑。
			if (tmp1 > 20000)
			{
				tmp1 -= 20000;
				w_hprintf ("@");
			}
			w_hprintf ("%s ", noansi(ModeType(tmp1)));
		}
	}
	printf ("</p><p>");
	sprintf (buf, "home/%c/%s/plans", toupper(*userid), userid);
	if (file_exist(buf))
	{
		printf ("<anchor><go href=\"bbscon.wml?board=*smd&amp;file=%s\" />看说明档</anchor> ", userid);
	}
	printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;userid=%s\" />写信问候</anchor> <anchor><go href=\"bbsfriend.wml?act=1&amp;userid=%s\" />加为好友</anchor> <anchor><go href=\"bbsfriend.wml?act=2&amp;userid=%s\" />删除好友</anchor> ", WML_VB_MAIL, userid, userid, userid);
	sprintf (buf, "0Announce/PersonalCorpus/%c/%s", toupper(*userid), userid);
	if (file_exist(buf))
	{
		printf ("<anchor><go href=\"bbs0an.wml?path=PersonalCorpus/%c/%s\" />参观文集</anchor> ", toupper(*userid), userid);
	}
	printf ("</p>");
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsqry_act(buf))
	{
		printf ("<card title=\"错误\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />返回</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
