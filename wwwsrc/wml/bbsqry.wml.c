//bbsqry.wml.c
//huangxu@nkbbs.org

//�鿴�û�������

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
		//��ʾ��
	{
		printf ("<card title=\"��ѯʹ���� -- %s\">", BBSNAME);
		printf ("<p><input name=\"inp_userid\" title=\"�û���\" name=\"inp_id\" maxlength=\"%d\" /></p>", IDLEN);
		printf ("<p><anchor>��ѯ<go href=\"bbsqry.wml\" method=\"post\"><postfield name=\"userid\" value=\"$(user_id:esc)\"/></go>\
</anchor>&#32;&#32;<anchor><prev />����</anchor></p>");
		return 0;
	}
	x = getuser(userid);
	if (!x)
	{
		sprintf (info, "�����ҵ��û������ڡ�");
		return -1;
	}
	strcpy (userid, x->userid);
	printf ("<card title=\"��ѯ�û�%s -- %s\">", userid, BBSNAME);
	printf ("<p>");
	w_hprintf ("%s ��%s�� ����վ%d��", x->userid, x->username, x->numlogins);
	if (HAS_DEFINE(x->userdefine, DEF_S_HOROSCOPE))
	{
		w_hprintf (" [%s]%s ", horoscope(x->birthmonth, x->birthday), x->gender == 'F' ? "MM" : "GG");
	}
	count_mails(x->userid, &tmp1, &tmp2);
	if (tmp2)
	{
		w_hprintf ("[��]");
	}
	printf ("<br />");
	getdatestring(x->lastlogin, NA);
	w_hprintf ("�ϴ���[%s]��[%s]��¼��", datestring, x->lasthost);
	if(x->lastlogout < x->lastlogin) 
	{
		getdatestring(((time(0)-x->lastlogin)/120)%47+1+x->lastlogin,NA);
	}
	else
	{
		getdatestring(x->lastlogout,NA);
	}
	w_hprintf ("��[%s]�뿪��", datestring);
	printf ("<br />");
	w_hprintf ("��������[%d](%s) ����ֵ��[%d](%s) ����ֵ��[%d](%s) ��������[%d] ", x->numposts, numpostsstr(x), countexp(x), expstr(x), countperf(x), perfstr(x), count_life_value(x));
	// ֻ�н��������Ϳ�����
	tmp2 = 0;
	for (i = 1; i <= 7; ++i) 
	{
		tmp2 += (x->nummedals >> (i << 2)) & 0x0f;
	}
	if (tmp2)
	{
		w_hprintf ("��������[%d]", tmp2);
	}
	printf ("<br />");
	uleveltochar (buf, x->userlevel);
	printf ("Ȩ�ޣ�[%s] ", buf);
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
				//ҲֻҪһ��
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
			strcat (buf2, "��");
		}
		fclose(fp);
	}
	if (*buf2)
	{
		*(buf2 + strlen(buf2) - 2) = 0;//���һ���ٺ�
		w_hprintf ("����%s����� ", buf2);
	}
	if (!(x->userlevel & PERM_SYSOP) && (x->userlevel & PERM_RECLUSION))
	{
		sprintf(buf, "home/%c/%s/Reclusion", toupper(x->userid[0]), x->userid);
		if ((file_exist(buf)) && (fp = fopen(buf, "rt")))
		{
			fscanf(fp, "%d", &tmp1);
			fclose(fp);
		}
		w_hprintf ("[�����У�����%d��]", (tmp1 - (time(0) - x->lastlogout) / 86400) > 0 ? (tmp1 - (time(0) - x->lastlogout) / 86400) : 0);
	}
	else if (!(x->userlevel & PERM_SYSOP) && (x->userlevel & PERM_SUICIDE))
	{
		w_hprintf ("[��ɱ��]");
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
				printf ("״̬��");
			}
			tmp2++;
			if(u->invisible)
			{
				w_hprintf ("[��]");
			}
			tmp1 = u->mode;//������
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
		printf ("<anchor><go href=\"bbscon.wml?board=*smd&amp;file=%s\" />��˵����</anchor> ", userid);
	}
	printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;userid=%s\" />д���ʺ�</anchor> <anchor><go href=\"bbsfriend.wml?act=1&amp;userid=%s\" />��Ϊ����</anchor> <anchor><go href=\"bbsfriend.wml?act=2&amp;userid=%s\" />ɾ������</anchor> ", WML_VB_MAIL, userid, userid, userid);
	sprintf (buf, "0Announce/PersonalCorpus/%c/%s", toupper(*userid), userid);
	if (file_exist(buf))
	{
		printf ("<anchor><go href=\"bbs0an.wml?path=PersonalCorpus/%c/%s\" />�ι��ļ�</anchor> ", toupper(*userid), userid);
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
		printf ("<card title=\"����\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />����</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
