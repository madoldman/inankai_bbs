//bbsfriend.wml.c
//huangxu@nkbbs.org

#include "BBSLIB.inc"
#include "WMLLIB.inc"

//bbsfriend.wml?userid=(1)&act=(2)&start=(3)
//�鿴���༭���ѡ�˵�������ˡ�

int bbsfriend_act(char * info)
{
	int act, start;
	char userid[IDLEN + 1];
	char buf[256];
	int i, j, tmp;
	struct userec * x;
	FILE * fp;
	if (!loginok)
	{
		sprintf (info, "�Ҵҹ��Ͳ���ʹ�ô˹��ܡ�");
		return -1;
	}
	wmlmode (u_info, FRIEND);
	strncpy (userid, getparm("userid"), IDLEN);
	act = atoi(getparm("act"));
	start = atoi(getparm("start")) - 1;
	loadfriend(currentuser.userid);
	if (act)
	{
		tmp = -1;
		for (i = 0; i < friendnum; i++)
		{
			if (!strcasecmp(userid, fff[i].id))
			{
				tmp = i;
				break;
			}
		}
		if (act == 1)//���Ӻ���
		{
			if (tmp >= 0)
			{
				sprintf (info, "%s�Ѿ������ID֮���ˡ�", userid);
				return -2;
			}
			if (friendnum + 1 > MAXFRIENDS)
			{
				printf (info, "���ĺ������Ѿ��ﵽ����%d�����ܼ�����Ӻ��ѡ�", MAXFRIENDS);
				return -3;
			}
			x = getuser(userid);
			if (!x)
			{
				sprintf (info, "���û������ڡ�");
				return -4;
			}
			memset (fff + friendnum, 0, sizeof(struct override));
			strcpy (fff[friendnum].id, x->userid);
			tmp = friendnum;
			friendnum++;
		}
		else if (act == 2)//ɾ������
		{
			if (tmp < 0)
			{
				sprintf (info, "��δ��ӹ��˺��ѡ�");
				return -2;
			}
			memmove (fff + i, fff + i + 1, friendnum - i - 1);
			friendnum--;
			memset (fff + friendnum, 0, sizeof(struct override));
		}
		sprintf(buf, "home/%c/%s/friends", toupper(currentuser.userid[0]), currentuser.userid);
		fp = fopen(buf, "w");
		fwrite(fff, sizeof(struct override), friendnum, fp);
		fclose(fp);
		tmp = tmp / WMLS_FRIEND_PER_PAGE * WMLS_FRIEND_PER_PAGE;
		printf ("<card title=\"���Ѳ����ɹ� -- %s\" ontimer=\"bbsfriend.wml?start=%d\">", BBSNAME, tmp + 1);
		printf ("<timer value=\"50\" />");
		printf ("<p>�ѳɹ�%s����<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor>��</p>", act == 1 ? "���" : "ɾ��", userid, userid);
		printf ("<p><anchor><go href=\"bbsfriend.wml?start=%d\" />��������Զ���ת����ʹ�ô����ӡ�</anchor></p>", tmp + 1);
		return 0;
	}
	if (start < 0 || start >= friendnum)
	{
		start = 0;
	}
	printf ("<card title=\"�������� -- %s\">", BBSNAME);
	tmp = start + WMLS_FRIEND_PER_PAGE;
	if (tmp > friendnum)
	{
		tmp = friendnum;
	}
	for (i = start; i < tmp; i++)
	{
		printf ("<p>#%d <a href=\"bbsqry.wml?userid=%s\">%s</a> ", i + 1, fff[i].id, fff[i].id);
		w_hprintf ("%s", fff[i].exp);
		printf ("</p>");
	}
	printf ("<p>ID��<input name=\"inp_userid\" /> <anchor><go href=\"bbsqry.wml\" method=\"get\"><postfield name=\"userid\" value=\"$(inp_userid)\" /></go>��ѯ�û�</anchor> <anchor><go href=\"bbsfriend.wml?act=1\" method=\"post\"><postfield name=\"userid\" value=\"$(inp_userid)\" /></go>��Ϊ����</anchor> <anchor><go href=\"bbsfriend.wml?act=2\" method=\"post\"><postfield name=\"userid\" value=\"$(inp_userid)\" /></go>ɾ������</anchor></p>");
	if (friendnum > WMLS_FRIEND_PER_PAGE)
	{
		printf ("<p>");
		for (i = 0; i <= friendnum / WMLS_FRIEND_PER_PAGE; i++)
		{
			if (i == 0 || i == friendnum / WMLS_FRIEND_PER_PAGE || abs(i - (start / WMLS_FRIEND_PER_PAGE)) < 3)
			{
				if (i == start / WMLS_FRIEND_PER_PAGE)
				{
					printf ("%d ", i + 1);
				}
				else
				{
					printf ("<a href=\"bbsfriend.wml?start=%d\">%d</a> ", (i * WMLS_FRIEND_PER_PAGE) + 1, i + 1);
				}
			}
		}
		printf ("</p>");
	}
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsfriend_act(buf))
	{
		printf ("<card title=\"����\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />����</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
