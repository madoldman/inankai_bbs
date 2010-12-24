//bbssettings.wml.c
//huangxu@nkbbs.org

//�޸ĸ������Ϻ��л�����

#include "BBSLIB.inc"
#include "WMLLIB.inc"

//bbssettings.wml?act=(0)
//act:
// 0 ���
// 1 �ύ
// 2 �л�����

int bbssettings_act(char * info)
{
	int act;
	char buf[1024], buf2[1024];
	char pw1[16], pw2[16], pw3[16];
	int i, tmp;
	char *p;
	if (!loginok)
	{
		strcpy(info, "�Ҵҹ��Ͳ���ʹ�ô˹��ܡ�");
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
			strcat(buf2, "�ǳ�̫�� ");
		}
		strsncpy(buf, getparm("realname"), 9);
		if (strlen(buf) > 1) 
		{
			strcpy(currentuser.realname, buf);
		}
		else 
		{
			strcat(buf2, "��ʵ����̫�� ");
		}
		strsncpy(buf, getparm("address"), 40);
		if (strlen(buf) > 8) 
		{
			strcpy(currentuser.address, buf);
		}
		else
		{
			strcat(buf2, "��ס��ַ̫�� ");
		}
		strsncpy(buf, getparm("email"), 32);
		if (strlen(buf) > 8 && strchr(buf, '@')) 
		{
			strcpy(currentuser.email, buf);
		}
		else 
		{
			strcat(buf2, "Email��ַ���Ϸ� ");
		}
		tmp = atoi(getparm("year"));
		if (tmp > 1900 && tmp < 1998) 
		{
			currentuser.birthyear = tmp - 1900;
		}
		else
		{
			strcat(buf2, "����ĳ������ ");
		}
		tmp = atoi(getparm("month"));
		if (tmp > 0 && tmp <= 12) 
		{
			currentuser.birthmonth = tmp;
		}
		else 
		{
			strcat(buf2, "����ĳ����·� ");
		}
		tmp = atoi(getparm("day"));
		if (tmp > 0 && tmp <= 31) 
		{
			currentuser.birthday = tmp;
		}
		else 
		{
			strcat(buf2, "����ĳ������� ");
		}
		strsncpy(buf, getparm("gender"), 2);
		if (*buf == 'F' || * buf == 'M')
		{
			currentuser.gender = *buf;
		}
		else
		{
			strcat(buf2, "����ʶ����Ա� ");
		}
		strsncpy(pw1, getparm("pw1"), 13);
		strsncpy(pw2, getparm("pw2"), 13);
		strsncpy(pw3, getparm("pw3"), 13);
		if (*pw1 && *pw2 && *pw3)
		{
			if(strcmp(pw2, pw3))
			{
				strcat(buf2, "�������벻��ͬ ");
			}
			else if(strlen(pw2)<2)
			{
				strcat(buf2, "������̫�� ");
			}
			else if(!checkpasswd(currentuser.passwd, pw1))
			{
				strcat(buf2, "���벻��ȷ ");
			}
			else
			{
				strcpy(currentuser.passwd, crypt1(pw2, pw2));
				strcat (buf2, "�޸�����ɹ� ");
			}
		}
		save_user_data(&currentuser);
	}
	else if (act == 2)
	{
		if (!(currentuser.userlevel & PERM_CLOAK))
		{
			sprintf (info, "��û���㹻��Ȩ�ޡ�");
			return -2;
		}
		u_info->invisible = !u_info->invisible;
	}
	printf ("<card title=\"�û����� -- %s\">", BBSNAME);
	if (act == 1 && *buf2)
	{
		printf ("<p><em>��Ϣ��%s</em></p>", buf2);
	}
	printf ("<p>�û�ID��<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor><br />", currentuser.userid, currentuser.userid);
	w_hsprintf (buf, "%s", currentuser.username);
	printf("�����ǳƣ�<input name=\"inp_nick\" value=\"%s\" maxlength=\"30\" /><br />", buf);
	printf("�����룺<input type=\"password\" name=\"inp_pw1\" /><br />");
	printf("�����룺<input type=\"password\" name=\"inp_pw2\" /><br />");
	printf("��ȷ�ϣ�<input type=\"password\" name=\"inp_pw3\" /><br />");
	printf("�粻�޸����룬����������������<br />");
	printf("���������%d ƪ<br />", currentuser.numposts);
	tmp = 0;
	for(i = 1; i <= 7; i++)
	{
		tmp += (currentuser.nummedals >> (i << 2)) & 0x0f;
	}
	printf("���ѫ�£�%d ö<br />", tmp);
	printf("�ż�������%d ��<br />", currentuser.nummails);
	printf("��վ������%d ��<br />", currentuser.numlogins);
	printf("��վʱ�䣺%d ����<br />", currentuser.stay / 60);
	w_hsprintf(buf, "%s", currentuser.realname);
	printf("��ʵ������<input name=\"inp_realname\" value=\"%s\" maxlength=\"16\" /><br />", buf);
	w_hsprintf(buf, "%s", currentuser.address);
	printf("��ס��ַ��<input name=\"inp_address\" value=\"%s\" maxlength=\"40\" /><br />", buf);
	printf("�ʺŽ�����%s<br />", Ctime(currentuser.firstlogin));
	printf("������٣�%s<br />", Ctime(currentuser.lastlogin));
	printf("��Դ��ַ��%s<br />", currentuser.lasthost);
	w_hsprintf(buf, "%s", currentuser.email);
	printf("�����ʼ���<input name=\"inp_email\" value=\"%s\" maxlength=\"32\" /><br />", buf);
	printf("�������ڣ�<input name=\"inp_year\" value=\"%d\" maxlength=\"4\" />��", currentuser.birthyear+1900);
	printf("<input name=\"inp_month\" value=\"%d\" maxlength=\"2\" />��", currentuser.birthmonth);
	printf("<input name=\"inp_day\" value=\"%d\" maxlength=\"2\" />��<br />", currentuser.birthday);
	printf("�û��Ա�<select name=\"inp_gender\" value=\"%c\">", currentuser.gender);
	printf("<option value=\"M\" title=\"��\">��</option><option value=\"F\" title=\"Ů\">Ů</option>");
	printf("</select><br />");
	if (HAS_PERM(PERM_CLOAK))
	{
		printf ("����״̬��<anchor><go href=\"bbssettings.wml?act=2\" />%s</anchor><br />", u_info->invisible ? "����" : "�ɼ�");
	}
	char * postfields[] = {"nick", "pw1", "pw2", "pw3", "realname", "address", "email", "year", "month", "day", "gender"};
	printf("<anchor><go href=\"bbssettings.wml?act=1\" method=\"post\">");
	wmlPostfields(postfields, sizeof(postfields) / sizeof(*postfields));
	printf("</go>�ύ</anchor> <anchor><go href=\"bbsfriend.wml\" />��������</anchor></p>");
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbssettings_act(buf))
	{
		printf ("<card title=\"���� -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}

