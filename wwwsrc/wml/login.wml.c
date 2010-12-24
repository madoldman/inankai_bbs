//login.wml.c
//huangxu@nkbbs.org

//登录
//新：HEADCOOKIES
//新：选择踢掉的session

#include "BBSLIB.inc"
#include "WMLLIB.inc"

int encodingtest()//直接setcookies
{
	char buf[256];
	int r;
	r = code_convert(CHARSET, "utf8", STR_ENCODING_TEST, strlen(STR_ENCODING_TEST), buf, sizeof(buf) - 1);
	if (r < 0)
	{
		return -1;//nothing.error.
	}
	if (!strcmp(buf, getparm(STR_ENCODING_NAME)))
	{
		headerCookie(STR_ENCODING_COOKIES, "utf8");
		return 1;//utf!
	}
	return 0;//nothing
}

int wmllogin(char * buf)
{
	char id[IDLEN + 2], pw[20];
	struct userec *x;
	struct user_info * uol[MULTI_LOGINS];
	char buf2[256], filename[256];
	int i, kick;
	page_init(NULL);
	strncpy(id, getparm("id"), IDLEN + 1);
	strncpy(pw, getparm("pw"), 19);
	if (!*pw)
	{
		strncpy(pw, getparm("pw2"), 19);
	}
	kick = atoi(getparm("kick")) - 1;
	if (!strcasecmp(id, "SYSOP"))
	{
		strcpy (buf, "用户SYSOP登录受限。");
		return -65536;
	}
	if(file_has_word(".bad_host", fromhost)) 
	{
		sprintf (buf, "对不起, 本站不欢迎来自 [%s] 的登录。 若有疑问, 请与SYSOP联系，", fromhost);
		return -256;
	}
	if(loginok && strcasecmp(id, currentuser.userid)) 
	{
		sprintf (buf, "系统检测到目前你的计算机上已经登录有一个帐号 %s，请先退出。", currentuser.userid);
		return 1;
	}
	x = getuser(id);
	if (!x)
	{
		strcpy (buf, "错误的使用者帐号");
		return -1;
	}
	sprintf(buf2, "home/%c/%s/badhost", toupper(x->userid[0]), x->userid);
	if(bad_host(fromhost,buf2)) 
	{
		sprintf (buf, "对不起，此帐号已被设定为不可从 [%s] 登录本站。",fromhost);
		return -257;
	}
	if(strcasecmp(id, "guest")) 
	{
		if(!checkpasswd(x->passwd, pw)) 
		{
			if(*pw)
			{
				sleep(2);
				getdatestring (time(0), NA);
				sprintf(buf2, "%-12.12s  %-30s %s[Wap]\n",id, datestring, fromhost);
				sprintf(filename, "home/%c/%s/logins.bad", toupper(x->userid[0]), x->userid);
				f_append(filename, buf2);
			}
			sprintf (buf, "密码错误");
			return -2;
		}
		if (check_login_limit(x))
		{
			strcpy (buf, "此ID在24小时内上站次数过多，请稍候再来。");
			return -4;
		}
		if(!user_perm(x, PERM_BASIC))
		{
			strcpy (buf, "此帐号已被停机。若有疑问，请用其他帐号在sysop版询问。");
			return -5;
		}
		if (check_multi_d(x, uol, kick))
		{
			wml_httpheader();
			wml_head();
			printf ("<card title=\"登录 -- %s\">", BBSNAME);
			printf ("<p>用户%s已经在本站登录了%d个线程，你需要踢掉一个才能登录。<br />", x->userid, MULTI_LOGINS);
			for (i = 0; i < MULTI_LOGINS; i++)
			{
				printf ("#%d %s %s%s 发呆%d分<br />", i, uol[i]->from, uol[i]->mode >= 20000 ? "@" : "", ModeType(uol[i]->mode >= 20000 ? uol[i]->mode - 20000 : uol[i]->mode), (time(0) - uol[i]->idle_time) / 60);
			}
			printf ("踢掉哪个：<select name=\"inp_kick\">");
			for (i = 0; i < MULTI_LOGINS; i++)
			{
				printf ("<option value=\"%d\">%d</option>", i + 1, i + 1);
			}
			printf ("</select><br />");
			printf ("您的密码：<input type=\"password\" maxlength=\"8\" name=\"inp_pw\" /><br />");
			printf ("<anchor><go href=\"login.wml?id=%s\" method=\"post\"><postfield name=\"pw\" value=\"$(inp_pw)\" /><postfield name=\"kick\" value=\"$(inp_kick)\" /></go>登录</anchor></p>", x->userid);
			return 0;
		}
		x->lastlogin = time(0);
    x->numlogins++;
    strsncpy(x->lasthost, fromhost, 17);
    save_user_data(x);
    currentuser = *x;
	}
	report("WapEnter");
	int iutmpnum, iutmpkey;
	if (!wwwlogin(x, &iutmpnum, &iutmpkey))//0 : succeed
	{
		encodingtest();
		sprintf(buf2, "%d", iutmpnum);
		headerCookie("utmpnum", buf2);
		sprintf(buf2, "%d", iutmpkey);
		headerCookie("utmpkey", buf2);
		headerCookie("utmpuserid", currentuser.userid);
		wml_httpheader();
	}
	else
	{
		strcpy (buf, "抱歉，登录人数太多，请稍候再来：（");
		return -65537;
	}
	sprintf (buf, "用户 %s 登录成功。", x->userid);
	wml_head();
	printf ("<card title=\"登录 -- %s\" ontimer=\"%s\">", BBSNAME, "bbsboa.wml");
	printf ("<timer value=\"50\" />");
	printf ("<p>");
	w_hprintf(buf);
	printf ("</p>");
	printf ("<p>跳转中……</p>");
	printf ("<p><anchor><go href=\"%s\" />如果不能自动跳转，请使用此链接。</anchor></p>", "bbsboa.wml");
	return 0;
}

int main()
{
	char buf[1024];
	int ir;
	if (wmllogin(buf))
	{
		wml_httpheader();
		wml_head();
		printf ("<card title=\"登录失败 -- %s\">", BBSNAME);
		printf ("<p>%s</p>", buf);
		printf ("<p><anchor><prev />返回</anchor></p>");
	}
	printf ("</card>");
	wml_tail();
}

