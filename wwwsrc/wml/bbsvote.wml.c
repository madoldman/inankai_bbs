//bbsvote.wml.c
//ͬ�������ļ���ȫ��bbsvote.c�޸Ķ���
//huangxu@nkbbs.org

//ͶƱ

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "../../bbssrc/include/vote.h"
//�ǲ��Ǹ�����һ��ͷ��
static char *vote_type[] = { "�Ƿ�", "��ѡ", "��ѡ", "����", "�ʴ�" };
static char *vote_yn[] = {"�޳�", "����", "��Ȩ"};
void setvfile(char *buf, char *bname, char *filename)
{
	sprintf(buf, "vote/%s/%s", bname, filename);
}

int votesug_inp(struct ballot * v)
{
	int i;
	printf ("<p>���������Ľ��飺");
	for (i = 0; i < 3; i++)
	{
		printf ("<br /><input name=\"inp_sug%d\" maxlength=\"78\" value=\"", i);
		w_hprintf ("%s", v->msg[i]);
		printf ("\" />");
	}
	printf ("</p>");
	return 0;
}

int votesug_pst()
{
	int i;
	char buf[256];
	for (i = 0; i < 3; i++)
	{
		sprintf (buf, "sug%d", i);
		wmlPostfield(buf);
	}
	return 0;
}

int bbsvote_act(char * info)
{
	FILE *fp;
	struct votebal currvote, ent;
	char buf[STRLEN], buf1[512];
	char buf2[1024];
	struct ballot uservote;
	//	struct votelog log;
	int aborted = NA, pos;
	int i, chkd;
	unsigned int j, multiroll = 0;
	char board[80];
	char posturl[256];
	char controlfile[STRLEN];
	char *date, *tmp1, *tmp2;
	char flagname[STRLEN];
	char logname[STRLEN];
	int voted_flag;		//�û��Ƿ�Ͷ������Ʊ
	int num_voted;		//������ж�����Ͷ��Ʊ
	int num_of_vote;	//����ǿ�����ͶƱ��
	int votenum;		//������û�ѡ����еڼ���ͶƱ
	int votevalue = 0;
	int procvote;
	time_t closedate;
	struct stat st;
	int now_t=time(0);
	if (!loginok) 
	{
		strcpy(info, "�Ҵҹ��Ͳ���ͶƱ��");
		return -1;
	}
	strsncpy(board, getparm("B"), 32);
	if (!*board)
	{
		strsncpy(board, getparm("board"), 32);
	}
	votenum = atoi(getparm("votenum"));
	procvote = atoi(getparm("procvote"));
	modify_mode(u_info, VOTING);
	if(!has_post_perm(&currentuser, board) || !HAS_PERM(PERM_VOTE))//ȥ���ɵ�ù��ȫվͶƱ
	{
		strcpy(info, "����Ȩ�ڱ�����ͶƱ��");
		return -1;
	}
	sprintf(controlfile, "vote/%s/%s", board, "control");
	num_of_vote = (stat(controlfile, &st) == -1) ? 0 : st.st_size / sizeof (struct votebal);
	if (!num_of_vote)
	{
		strcpy(info, "��Ǹ, Ŀǰ��û���κ�ͶƱ���С�");
		return -2;
	}
	fp = fopen(controlfile, "r");
	if (!fp)//��Ų��ᷢ��
	{
		strcpy(info, "ϵͳ����");
		return -256;
	}
	//��if���������
	if (!votenum) //����ͶƱ�б�
	{
		printf ("<card title=\"ͶƱ�� -- %s\">", BBSNAME);
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />%s��</anchor>ͶƱ</p>", board, board);
		for (i = 1; i <= num_of_vote; i++) 
		{
			fread(&ent, sizeof (struct votebal), 1, fp);
			sprintf(flagname, "vote/%s/flag.%d", board, (int) ent.opendate);
			num_voted = (stat(flagname, &st) ==	-1) ? 0 : st.st_size / sizeof (struct ballot);
			closedate = ent.opendate + ent.maxdays * 86400;
			//ע�⣬�����date���˽���ʱ��
			w_hsprintf(buf2, "%s", ent.title);//�������
			printf("<p><anchor><go href=\"bbsvote.wml?board=%s&amp;votenum=%d\" />#%d %s<br />", board, i, i, buf2);
			//����ͶƱ�߱������������ˣ�
			datestr(buf2, closedate);
			//���Ҳ�������������ˣ�
			printf ("������%s ���룺%d</anchor></p>", buf2, num_voted);
		}
		fclose(fp);
		return 0;
	}
	else 
	{
		if (votenum > num_of_vote)
		{
			strcpy(info, "��������");
			return -3;
		}
		fseek(fp, sizeof (struct votebal) * (votenum - 1), 0);
		fread(&currvote, sizeof (struct votebal), 1, fp);
		fclose(fp);
		sprintf(flagname, "vote/%s/flag.%d", board, (int) currvote.opendate);
		num_voted = (stat(flagname, &st) == -1) ? 0 : st.st_size / sizeof (struct ballot);
		pos = 0;
		fp = fopen(flagname, "r");
		voted_flag = NA;
		if (fp) 
		{
			for (i = 1; i <= num_voted; i++) 
			{
				fread(&uservote, sizeof (struct ballot), 1, fp);
				if (!strcasecmp(uservote.uid, currentuser.userid)) 
				{
					voted_flag = YEA;
					pos = i;
					break;
				}
			}
			fclose(fp);
		}
		if (!voted_flag)
		{
			memset(&uservote, 0, sizeof (uservote));
		}
		if (procvote == 0) 
			//-------ͶƱȨ���ж�
			//û��ϸ��-_-b
		{
			if (currentuser.firstlogin > currvote.opendate)	
			{
				strcpy(info, "�Բ���, ��ͶƱ�����ʺ�����֮ǰ������������ͶƱ��");
				return -3;
			}
			else if (!HAS_PERM(currvote.level & ~(LISTMASK | VOTEMASK)))
			{
				strcpy(info, "�Բ�����Ŀǰ����Ȩ�ڱ�Ʊ��ͶƱ��");
				return -4;
			}
			else if(currvote.level & LISTMASK )
			{
				char listfilename[STRLEN];
				setvfile(listfilename, board, "vote.list");
				if(!dashf(listfilename)) 
				{
					strcpy(info, "�Բ��𣬱�Ʊ����Ҫ�趨��ͶƱ���᷽�ɽ���ͶƱ��");
					return -5;
				}
				else if(!seek_in_file(listfilename,currentuser.userid))
				{
					strcpy(info, "�Բ���, ͶƱ�������Ҳ������Ĵ�����");
					return -6;
				}
			}
			else if(currvote.level & VOTEMASK )
			{
				if(currentuser.numlogins < currvote.x_logins || currentuser.numposts < currvote.x_posts ||currentuser.stay < currvote.x_stay * 3600
					|| currentuser.firstlogin > currvote.opendate - currvote.x_live * 86400)
				{
					strcpy(info, "�Բ�����Ŀǰ�в����ʸ��ڱ�Ʊ��ͶƱ��");
					return -7;
				}
			}
			closedate =	currvote.opendate + currvote.maxdays * 86400;
			printf ("<card title=\"ͶƱ�� -- %s\">", BBSNAME);
			w_hsprintf (buf2, "%s", currvote.title);
			printf("<p>ͶƱ���⣺ %s<br />", buf2);
			printf("ͶƱ���ͣ� %s<br />", vote_type[currvote.type - 1]);
			printf("����ͶƱ�� <anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor><br />", currvote.userid, currvote.userid);
			printf("���ڰ��棺 <anchor><go href=\"bbsdoc.wml?board=%s\" />%s</anchor><br />", board, board);
			datetime(buf2, closedate);
			printf("����ʱ�䣺 %s<br />", buf2);
			if (currvote.type != VOTE_ASKING)
			{
				printf("���ͶƱ�� %d<br />", currvote.maxtkt);
				printf("ͶƱ˵����<br />");
				sprintf(buf, "vote/%s/desc.%d", board, (int) currvote.opendate);
				fp = fopen(buf, "r");
				if (fp)
				{
					while (1) 
					{
						if (fgets(buf1, sizeof (buf1), fp) == 0)
						{
							break;
						}
						w_hprintf("%s", buf1);
						printf ("<br />");
					}
					fclose(fp);
				}
			}
			printf ("</p>");
			//multiroll���ֶ���û����
			sprintf (posturl, "bbsvote.wml?board=%s&amp;votenum=%d", board, votenum);
			switch (currvote.type) 
			{
				case VOTE_SINGLE:
					//��ѡ
					j =	uservote.voted + (uservote.voted << currvote.totalitems);
					chkd = 0;
					//���ǵ�ǰѡ�е�
					printf ("<p>ѡ���б�<br />");
					for (i = 0; i < currvote.totalitems; i++) 
					{
						w_hprintf ("ѡ��%d %s", i + 1, currvote.items[i]);
						printf ("<br />");
						if (j & 1)
						{
							chkd = i + 1;
						}
						j >>= 1;
					}
					printf ("��ѡ��<select name=\"inp_vote\" value=\"%d\">", chkd);
					printf ("<option value=\"0\">δѡ��</option>");
					for (i = 0; i < currvote.totalitems; i++)
					{
						printf ("<option value=\"%d\">%d</option>", i + 1, i + 1);
					}
					printf ("</select></p>");
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"2\" />", posturl);
					wmlPostfield("vote");
					votesug_pst();
					printf ("</go>ͶƱ</anchor></p>");
					break;
				case VOTE_MULTI:
					//��ѡ
					j = uservote.voted + (uservote.voted << currvote.totalitems);
					printf ("<p>ѡ���б�<em>�����Ͷ%dƱ��</em>��<br />", currvote.maxtkt);
					for (i = 0; i < currvote.totalitems; i++) 
					{
						printf ("<select name=\"inp_votemulti%d\" value=\"%d\"><option value=\"0\">��ѡ��</option><option value=\"1\">��ѡ��</option></select> ", i + 1, (j & 1));
						w_hprintf ("%s", currvote.items[i]);
						printf ("<br />");
						j >>= 1;
					}
					printf ("</p>");
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"3\" />", posturl);
					for (i = 0; i < currvote.totalitems; i++)
					{
						sprintf (buf2, "votemulti%d", i + 1);
						wmlPostfield(buf2);
					}
					votesug_pst();
					printf ("</go>ͶƱ</anchor></p>");
					break;
				case VOTE_YN:
					//�Ƿ� ... ���Ǹ���ѡһ����ô��
					j =	uservote.voted + (uservote.voted << currvote.totalitems);
					chkd = 0;
					//���ǵ�ǰѡ�е�
					printf ("<p>ѡ���б�<br />");
					for (i = 0; i < currvote.totalitems; i++) 
					{
						if (j & 1)
						{
							chkd = i + 1;
							break;
						}
						j >>= 1;
					}
					printf ("��ѡ��<select name=\"inp_vote\" value=\"%d\">", chkd);
					printf ("<option value=\"0\">δѡ��</option>");
					for (i = 0; i < currvote.totalitems; i++)
					{
						printf ("<option value=\"%d\">", i + 1);
						w_hprintf ("%s", currvote.items[i]);
						printf ("</option>");
					}
					printf ("</select></p>");
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"1\" />", posturl);
					//���ﲻͬ����ѡprocvote=2
					wmlPostfield("vote");
					votesug_pst();
					printf ("</go>ͶƱ</anchor></p>");
					break;
				case VOTE_VALUE:
					//��ֵ
					printf ("<p>������һ��ֵ��");
					printf ("<input name=\"inp_votevalue\" value=\"%d\" /></p>", uservote.voted);
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"4\" />", posturl);
					wmlPostfield("votevalue");
					votesug_pst();
					printf ("</go>ͶƱ</anchor></p>");
					break;
				case VOTE_ASKING:
					votesug_inp(&uservote);
					printf ("<p><anchor><go href=\"%s\" method=\"post\"><postfield name=\"procvote\" value=\"5\" />", posturl);
					votesug_pst();
					printf ("</go>ͶƱ</anchor></p>");
					break;
				default:
					printf ("<p>û���������͵�ͶƱ����</p>");
					return 0;
					//:(
				//<-switch
			}
		}
		else
		{
			if (procvote != currvote.type)
			{
				strcpy(info, "ͶƱ����");
				return -1;
			}
			switch (procvote) 
			{
				case 2:	//VOTE_SINGLE
					votevalue = 1;
					votevalue <<= atoi(getparm("vote")) - 1;
					if (atoi(getparm("vote")) > currvote.totalitems + 1)
					{
						strcpy(info, "��Чѡ�");
						return -1;
					}
					aborted = (votevalue == uservote.voted);
					break;
				case 3:	//VOTE_MULTI
					votevalue = 0;
					j = 0;
					for (i = currvote.totalitems - 1; i >= 0; i--) 
					{
						votevalue <<= 1;
						sprintf(buf, "votemulti%d", i + 1);
						votevalue |= !!atoi(getparm(buf));
						j += !!atoi(getparm(buf));
					}
					aborted = (votevalue == uservote.voted);
					if (j > currvote.maxtkt) 
					{
						sprintf(info, "��ͶƱ���ֻ��ѡ��%d��ѡ�", currvote.maxtkt);
						return -1;
					}
					break;
				case 1:	//VOTE_YN
					votevalue = 1;
					j = atoi(getparm("vote")) - 1;
					if (j >	currvote.totalitems || j < 0)
					{
						strcpy(info, "��Чѡ�");
						return -1;
					}
					votevalue <<= j;
					aborted = (votevalue == uservote.voted);
					break;
				case 4:	//VOTE_VALUE
					aborted = ((votevalue = atoi(getparm("votevalue"))) == uservote.voted);
					if (votevalue < 0 || votevalue > currvote.maxtkt) 
					{
						sprintf(info, "Ӧ������0��%d֮�䣨������", currvote.maxtkt);
						return -1;
					}
					break;
				case 5: //VOTE_ASKING
					aborted = NA;
					break;
				//<-switch
			}
			printf ("<card title=\"ͶƱ�� -- %s\">", BBSNAME);
			if (aborted == YEA) 
			{
				printf ("<p>δ�޸ĸ�ͶƱ��ѡ�</p>");
				return 0;
			}
			else
			{
				fp = fopen(flagname, "r+");
				if (fp == 0)
				{
					fp = fopen(flagname, "w+");
				}
				if (fp)
				{
					flock(fileno(fp), LOCK_EX);
					if (pos > 0)
					{
						fseek(fp, (pos - 1) * sizeof (struct ballot), SEEK_SET);
					}
					else
					{
						fseek(fp, 0, SEEK_END);
					}
					strcpy(uservote.uid, currentuser.userid);
					uservote.voted = votevalue;
					for (i = 0; i < 3; i++)
					{
						sprintf (buf2, "sug%d", i);
						strncpy (uservote.msg[i], getparm(buf2), 78);
						removeInvisibleChars(uservote.msg[i], ' ');
					}
					fwrite(&uservote, sizeof (struct ballot), 1, fp);
					flock(fileno(fp), LOCK_UN);
					fclose(fp);
					printf("<p>�Ѿ�����Ͷ��Ʊ���С�</p>");
					sprintf(buf, "%s %s %s", currentuser.userid, currentuser.lasthost, Ctime(now_t));
					fs_append(BBSHOME"/vote.log", buf);
				}
				else
				{
					printf ("<p>ϵͳ������һ��СС������-_-b</p>");
				}
			}
		}
	}
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsvote_act(buf))
	{
		printf ("<card title=\"���� -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />����</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}


