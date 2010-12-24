//bbssettings.wml.c
//huangxu@nkbbs.org

//修改个人资料和切换隐身

#include "BBSLIB.inc"
#include "WMLLIB.inc"

//bbssettings.wml?act=(0)
//act:
// 0 浏览
// 1 提交
// 2 切换隐身

int bbssettings_act(char * info)
{
	int act;
	char buf[1024], buf2[1024];
	char pw1[16], pw2[16], pw3[16];
	int i, tmp;
	char *p;
	if (!loginok)
	{
		strcpy(info, "匆匆过客不能使用此功能。");
		return -1;
	}
	wmlmode(u_info, USERDEF);
	act = atoi(getparm("act"));
	*buf = 0;
	*buf2 = 0;
	if (act == 1)
	{
		strsncpy (buf, getparm("nick"), 30);
		for (p = buf; *p; p++)
		{
			if (*p < ' ' && *p > 0 || *p == -1)
			{
				*p = ' ';
			}
		}
		if (strlen(buf) > 1)
		{
			strcpy(currentuser.username, buf);
		}
		else 
		{
			strcat(buf2, "昵称太短 ");
		}
		strsncpy(buf, getparm("realname"), 9);
		if (strlen(buf) > 1) 
		{
			strcpy(currentuser.realname, buf);
		}
		else 
		{
			strcat(buf2, "真实姓名太短 ");
		}
		strsncpy(buf, getparm("address"), 40);
		if (strlen(buf) > 8) 
		{
			strcpy(currentuser.address, buf);
		}
		else
		{
			strcat(buf2, "居住地址太短 ");
		}
		strsncpy(buf, getparm("email"), 32);
		if (strlen(buf) > 8 && strchr(buf, '@')) 
		{
			strcpy(currentuser.email, buf);
		}
		else 
		{
			strcat(buf2, "Email地址不合法 ");
		}
		tmp = atoi(getparm("year"));
		if (tmp > 1900 && tmp < 1998) 
		{
			currentuser.birthyear = tmp - 1900;
		}
		else
		{
			strcat(buf2, "错误的出生年份 ");
		}
		tmp = atoi(getparm("month"));
		if (tmp > 0 && tmp <= 12) 
		{
			currentuser.birthmonth = tmp;
		}
		else 
		{
			strcat(buf2, "错误的出生月份 ");
		}
		tmp = atoi(getparm("day"));
		if (tmp > 0 && tmp <= 31) 
		{
			currentuser.birthday = tmp;
		}
		else 
		{
			strcat(buf2, "错误的出生日期 ");
		}
		strsncpy(buf, getparm("gender"), 2);
		if (*buf == 'F' || * buf == 'M')
		{
			currentuser.gender = *buf;
		}
		else
		{
			strcat(buf2, "不能识别的性别 ");
		}
		strsncpy(pw1, getparm("pw1"), 13);
		strsncpy(pw2, getparm("pw2"), 13);
		strsncpy(pw3, getparm("pw3"), 13);
		if (*pw1 && *pw2 && *pw3)
		{
			if(strcmp(pw2, pw3))
			{
				strcat(buf2, "两次密码不相同 ");
			}
			else if(strlen(pw2)<2)
			{
				strcat(buf2, "新密码太短 ");
			}
			else if(!checkpasswd(currentuser.passwd, pw1))
			{
				strcat(buf2, "密码不正确 ");
			}
			else
			{
				strcpy(currentuser.passwd, crypt1(pw2, pw2));
				strcat (buf2, "修改密码成功 ");
			}
		}
		save_user_data(&currentuser);
	}
	else if (act == 2)
	{
		if (!(currentuser.userlevel & PERM_CLOAK))
		{
			sprintf (info, "您没有足够的权限。");
			return -2;
		}
		u_info->invisible = !u_info->invisible;
	}
	printf ("<card title=\"用户资料 -- %s\">", BBSNAME);
	if (act == 1 && *buf2)
	{
		printf ("<p><em>信息：%s</em></p>", buf2);
	}
	printf ("<p>用户ID：<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor><br />", currentuser.userid, currentuser.userid);
	w_hsprintf (buf, "%s", currentuser.username);
	printf("您的昵称：<input name=\"inp_nick\" value=\"%s\" maxlength=\"30\" /><br />", buf);
	printf("旧密码：<input type=\"password\" name=\"inp_pw1\" /><br />");
	printf("新密码：<input type=\"password\" name=\"inp_pw2\" /><br />");
	printf("请确认：<input type=\"password\" name=\"inp_pw3\" /><br />");
	printf("如不修改密码，请留空以上三处。<br />");
	printf("发表大作：%d 篇<br />", currentuser.numposts);
	tmp = 0;
	for(i = 1; i <= 7; i++)
	{
		tmp += (currentuser.nummedals >> (i << 2)) & 0x0f;
	}
	printf("获得勋章：%d 枚<br />", tmp);
	printf("信件数量：%d 封<br />", currentuser.nummails);
	printf("上站次数：%d 次<br />", currentuser.numlogins);
	printf("上站时间：%d 分钟<br />", currentuser.stay / 60);
	w_hsprintf(buf, "%s", currentuser.realname);
	printf("真实姓名：<input name=\"inp_realname\" value=\"%s\" maxlength=\"16\" /><br />", buf);
	w_hsprintf(buf, "%s", currentuser.address);
	printf("居住地址：<input name=\"inp_address\" value=\"%s\" maxlength=\"40\" /><br />", buf);
	printf("帐号建立：%s<br />", Ctime(currentuser.firstlogin));
	printf("最近光临：%s<br />", Ctime(currentuser.lastlogin));
	printf("来源地址：%s<br />", currentuser.lasthost);
	w_hsprintf(buf, "%s", currentuser.email);
	printf("电子邮件：<input name=\"inp_email\" value=\"%s\" maxlength=\"32\" /><br />", buf);
	printf("出生日期：<input name=\"inp_year\" value=\"%d\" maxlength=\"4\" />年", currentuser.birthyear+1900);
	printf("<input name=\"inp_month\" value=\"%d\" maxlength=\"2\" />月", currentuser.birthmonth);
	printf("<input name=\"inp_day\" value=\"%d\" maxlength=\"2\" />日<br />", currentuser.birthday);
	printf("用户性别：<select name=\"inp_gender\" value=\"%c\">", currentuser.gender);
	printf("<option value=\"M\" title=\"男\">男</option><option value=\"F\" title=\"女\">女</option>");
	printf("</select><br />");
	if (HAS_PERM(PERM_CLOAK))
	{
		printf ("隐身状态：<anchor><go href=\"bbssettings.wml?act=2\" />%s</anchor><br />", u_info->invisible ? "隐身" : "可见");
	}
	char * postfields[] = {"nick", "pw1", "pw2", "pw3", "realname", "address", "email", "year", "month", "day", "gender"};
	printf("<anchor><go href=\"bbssettings.wml?act=1\" method=\"post\">");
	wmlPostfields(postfields, sizeof(postfields) / sizeof(*postfields));
	printf("</go>提交</anchor> <anchor><go href=\"bbsfriend.wml\" />好友名单</anchor></p>");
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbssettings_act(buf))
	{
		printf ("<card title=\"错误 -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}

