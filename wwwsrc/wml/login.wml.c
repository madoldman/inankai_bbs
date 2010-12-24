//login.wml.c
//huangxu@nkbbs.org

//��¼
//�£�HEADCOOKIES
//�£�ѡ���ߵ���session

#include "BBSLIB.inc"
#include "WMLLIB.inc"

int encodingtest()//ֱ��setcookies
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
		strcpy (buf, "�û�SYSOP��¼���ޡ�");
		return -65536;
	}
	if(file_has_word(".bad_host", fromhost)) 
	{
		sprintf (buf, "�Բ���, ��վ����ӭ���� [%s] �ĵ�¼�� ��������, ����SYSOP��ϵ��", fromhost);
		return -256;
	}
	if(loginok && strcasecmp(id, currentuser.userid)) 
	{
		sprintf (buf, "ϵͳ��⵽Ŀǰ��ļ�������Ѿ���¼��һ���ʺ� %s�������˳���", currentuser.userid);
		return 1;
	}
	x = getuser(id);
	if (!x)
	{
		strcpy (buf, "�����ʹ�����ʺ�");
		return -1;
	}
	sprintf(buf2, "home/%c/%s/badhost", toupper(x->userid[0]), x->userid);
	if(bad_host(fromhost,buf2)) 
	{
		sprintf (buf, "�Բ��𣬴��ʺ��ѱ��趨Ϊ���ɴ� [%s] ��¼��վ��",fromhost);
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
			sprintf (buf, "�������");
			return -2;
		}
		if (check_login_limit(x))
		{
			strcpy (buf, "��ID��24Сʱ����վ�������࣬���Ժ�������");
			return -4;
		}
		if(!user_perm(x, PERM_BASIC))
		{
			strcpy (buf, "���ʺ��ѱ�ͣ�����������ʣ����������ʺ���sysop��ѯ�ʡ�");
			return -5;
		}
		if (check_multi_d(x, uol, kick))
		{
			wml_httpheader();
			wml_head();
			printf ("<card title=\"��¼ -- %s\">", BBSNAME);
			printf ("<p>�û�%s�Ѿ��ڱ�վ��¼��%d���̣߳�����Ҫ�ߵ�һ�����ܵ�¼��<br />", x->userid, MULTI_LOGINS);
			for (i = 0; i < MULTI_LOGINS; i++)
			{
				printf ("#%d %s %s%s ����%d��<br />", i, uol[i]->from, uol[i]->mode >= 20000 ? "@" : "", ModeType(uol[i]->mode >= 20000 ? uol[i]->mode - 20000 : uol[i]->mode), (time(0) - uol[i]->idle_time) / 60);
			}
			printf ("�ߵ��ĸ���<select name=\"inp_kick\">");
			for (i = 0; i < MULTI_LOGINS; i++)
			{
				printf ("<option value=\"%d\">%d</option>", i + 1, i + 1);
			}
			printf ("</select><br />");
			printf ("�������룺<input type=\"password\" maxlength=\"8\" name=\"inp_pw\" /><br />");
			printf ("<anchor><go href=\"login.wml?id=%s\" method=\"post\"><postfield name=\"pw\" value=\"$(inp_pw)\" /><postfield name=\"kick\" value=\"$(inp_kick)\" /></go>��¼</anchor></p>", x->userid);
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
		strcpy (buf, "��Ǹ����¼����̫�࣬���Ժ���������");
		return -65537;
	}
	sprintf (buf, "�û� %s ��¼�ɹ���", x->userid);
	wml_head();
	printf ("<card title=\"��¼ -- %s\" ontimer=\"%s\">", BBSNAME, "bbsboa.wml");
	printf ("<timer value=\"50\" />");
	printf ("<p>");
	w_hprintf(buf);
	printf ("</p>");
	printf ("<p>��ת�С���</p>");
	printf ("<p><anchor><go href=\"%s\" />��������Զ���ת����ʹ�ô����ӡ�</anchor></p>", "bbsboa.wml");
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
		printf ("<card title=\"��¼ʧ�� -- %s\">", BBSNAME);
		printf ("<p>%s</p>", buf);
		printf ("<p><anchor><prev />����</anchor></p>");
	}
	printf ("</card>");
	wml_tail();
}

