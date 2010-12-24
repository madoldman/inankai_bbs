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
		prints(" %s �Ѿ��������С�", uident);
		pressanykey();
		return -1;
	}
	if (ischange && !seek)
	{
		move(3,0);
		prints(" %s ���������С�", uident);
		pressreturn();
		return -1;
	}
	clear();
	while(1)
	{
		getdata(3,0,"����˵��: ", buf,40,DOECHO,YEA);
		if(killwordsp(buf)!=0)
			break;
	}
	nowtime=time(0);
	getdatestring(nowtime,NA);
	getdata(4,0,"������ʱ��[ȱʡΪ 1 ��, 0 Ϊ����]: ",buf2,4,DOECHO,YEA);
	if(buf2[0]=='\0')
		day =1;
	else
		day = atoi(buf2);
	if(day <= 0)
		return -1;
	//�������ܷ�ID����һ����. tdhlshx 2003.5.24.
	if(day>31)
	{
		if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK)))
		{
			prints(" \n  �Բ���,��û��Ȩ�޷�ID����31��  \n ");
			pressanykey();
			return -1;
		}
	}//add end
	nowtime += day * 86400;
	getdatestring(nowtime,NA);
	sprintf(strtosave,"%-12s %-40s %14.14s���", uident,buf,datestring);
	if(!ischange)
	{
		sprintf(msg,
		        "\n  %s ����: \n\n"
		        "\t�Һܱ�Ǹ�ظ����������Ѿ���ȡ���� %s ��ġ�%s��Ȩ����\n\n"
		        "\t���������ԭ���� [%s]\n\n"
		        "\t���ھ���ֹͣ���ڱ����Ȩ�� [%d] �졣\n\n"
		        //	"\t������ [%14.14s] �� %s ���������⡣\n\n",
		        "\t[%d] ���,�����ڱ��巢��ʱϵͳ���Զ���⡣\n\n"  //�Զ����
                        "\t�������飬����%s��ϵ���������ֱ�ӵ�0��Appeal������йظ�ʽ����Ͷ�ߡ�\n\n"
    		        "\t\t\t\t\t%s �� %s",
		        uident, currboard, info_type==1?"����":"����",buf, day,
		        day,currentuser.userid,currboard,currentuser.userid);
		//	datestring, currentuser.userid);
	}
	else
	{
		sprintf(msg,
		        "\n  %s ����: \n\n"
		        "\t�������� %s �屻ȡ����%s��Ȩ�����⣬�ֱ�����£�\n\n"
		        "\t�����ԭ�� [%s]\n\n"
		        "\t�����ڿ�ʼ��ֹͣ��Ȩ��ʱ�䣺 [%d] ��\n\n"
		        //	"\t������ [%14.14s] �� %s ���������⡣\n\n",
		        "\t[%d] ���,�����ڱ��巢��ʱϵͳ���Զ���� \n\n"  //�Զ����
		        "\t\t\t\t\t\t\t\t\t\t\tBy %s",
		        uident, currboard, info_type==1?"����":"����",buf, day,
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
		cnt = ListFileEdit(boardallow,"�༭�������Ա������");

                char buf[256];
                sprintf(buf,"INFO %s �༭��Ա����",currboard);
                /*securityreport(buf);

		if(cnt == 0)
		{
			prints("\n�����Ա����Ϊ�գ�ϵͳ���Զ�ȡ������ĳ�Ա��������\n");
			if(askyn("��ȷ��Ҫȡ������ĳ�Ա����������",NA,NA)==YEA)
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
		if(mode==1) prints("\033[1;31m�����Ѿ����������������\033[m");
		if(mode==2) prints("\033[1;31m�����Ѿ���������������\033[m");
		getdata(4,0,"�趨������(1)����������� (2)���������� (0) ȡ�� [0]",tmp,2,DOECHO,YEA);
		if(tmp[0]==0)return FULLUPDATE;
		if(tmp[0]=='0') return FULLUPDATE;
		if(tmp[0]=='1')
		{
			if(mode==2) 
			{
				move(5,0);
				if(askyn("�����Ѿ���������������,ȷ��ɾ����?",NA,NA)==YEA)
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
				if(askyn("�����Ѿ����ý�������,ȷ��ɾ����?",NA,NA)==YEA)
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
		if(setmode==1) cnt = ListFileEdit(boardallow,"�༭����������Ա������");
		if(setmode==2) cnt = ListFileEdit(boardwrite,"�༭�����巢�ĳ�Ա������");
		char buf[256];
             if(setmode==1)  sprintf(buf,"�༭%s������Ա����",currboard);
             if(setmode==2)  sprintf(buf,"�༭%s�巢�ĳ�Ա����",currboard);
		securityreport4(buf);
    	if(cnt == 0&&HAS_PERM(PERM_OBOARDS))
		{
				prints("\n�����Ա����Ϊ�գ�ϵͳ���Զ�ȡ������ľ��ֲ���������\n");
				if(askyn("��ȷ��Ҫȡ������ľ��ֲ�����������",NA,NA)==YEA)
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
}      /* loveni:���ֲ�����*/
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


		sprintf (genbuf, "����ȡ���ö�����%-.55s", fileinfo->title);
		if (askyn(genbuf, NA, YEA) == NA)
		{
			move(t_lines - 1, 0);
			prints("����...");
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
		prints ("        �����ö��������Ѵ����ޣ������������ö�����!");
		pressreturn ();
		return FULLUPDATE;
	}
	fp = fopen (genbuf, "ab+");
	if (!fp)
	{
		move (3, 0);
		clrtobot ();
		move (5, 0);
		prints ("        ���ܴ��ļ����ö�ʧ��!");
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
      getdata (t_lines - 1, 0,"ʹ���õ���ʾ: 1)\33[1;33m[��ʾ]\33[m 2)\33[1;31m[����]\33[m 3)\33[1;35m[���]\33[m 4)\33[1;32m[�Ƽ�]\33[m 5)\33[1;34m[����]\33[m 6)\33[1;36m[����]\33[m \33[1;33m[1]\33[m:",  ch, 3, DOECHO, YEA);

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
		prints ("        ����д���ļ����ö�ʧ��!");
		pressreturn ();
		return FULLUPDATE;
	}
	fileinfo->accessed [1] |= FILE_TIP;
	substitute_record(direct, fileinfo, size, ent);
	fclose (fp);
	move (t_lines -1, 0);
	clrtoeol ();
	sprintf (genbuf, "�����õף���%s", fileinfo->title);
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
		prints("�뵽һ��ģʽ��ִ�з������. ^_^ ");
		pressanykey();
		return DONOTHING;
	}   //add end
	/*   if (check_anonyboard(currboard)==YEA&&!HAS_PERM(PERM_SYSOP)){
	      prints("\n�����������֪�����������, �ʲ�Ӧ֪��������˭��������\n");
	      prints("����趨�޷� Post ������֮Ȩ�޲����Ÿ�����ʹ��\n");
	      prints("������Ҫ�趨��������, ����֪ͨsysop����\n");
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
		getdata(3,0,"�趨������(1)��ֹ�������� (2)��ֹ�������� (0) ȡ�� [1]",
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
			prints("����Ҫ�趨����� [��ֹ����] ����\n�趨 [��ֹ����] ������");
			prints("���б����� [��ֹ����] ������ ID �������Խ��뱾�塣\n");
			move(8,0);
			if(askyn("��ȷ��Ҫ�趨������ô",NA,NA)==NA)
				return FULLUPDATE;
		}
	}
#endif
	while (1)
	{
		clear();
		prints("�趨����� [%s] ����\n", info_type==1?"��ֹ����":"��ֹ����");
		if(info_type == 1)
			setbfile(genbuf, currboard, "deny_users");
		else
			setbfile(genbuf, currboard, "board.deny");
		count = listfilecontent(genbuf, 3);
		if (count)
		{
			/*      //add for �Զ����
			    if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK)))
				    getdata(1,0,"(A)����  (C)�ı� or (E)�뿪[E]: ",ans,7,DOECHO,YEA);
			    else//add end*/
			getdata(1, 0, "(A)���� (D)ɾ�� (C)�ı� or (E)�뿪[E]: ",
			        ans, 7, DOECHO, YEA);
		}
		else
		{
			getdata(1, 0, "(A)���� or (E)�뿪 [E]: ",
			        ans, 7, DOECHO, YEA);
		}
		if (*ans == 'A' || *ans == 'a')
		{
			move(2, 0);
			usercomplete("����׼������������ʹ����ID: ", uident);
			if(*uident!='\0'&& !strcmp(uident,"guest"))
			{
				move(3,0);
				clrtobot();
				move(6,3);
				prints("faint...guest Ҳ���Ź�?�˼ұ����Ͳ��ܷ�����ô.");
				pressanykey();
				return DONOTHING;
			}
			if (*uident != '\0' && getuser(uident))
			{
				if (addtodeny(uident,msgbuf,0,info_type) == 1)
				{
					sprintf(repbuf, "%s��ȡ����%s���%sȨ��",
					        uident,currboard, info_type==1?"����":"����");
					securityreport4(repbuf);
					if(msgbuf[0]!='\0')
						autoreport(repbuf,msgbuf,YEA,uident);
				}
			}
		}
		else if ((*ans == 'C'|| *ans == 'c'))
		{
			//add tdhlshx,ֻ��վ�����ܸķ��˴���
			/*    if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK))){
			    
			   	       prints(" \n        �������׸��Ĵ�������.�����Ҫ,����վ����˵��.^_^  \n ");
			             pressanykey();
			             return FULLUPDATE;                             
			     }
			 ��Ը��Ҫ������. */
			move(2,0);
			namecomplete("�����뱻�޸��� ID: ", uident);
			if( *uident != '\0' )
			{
				if(addtodeny(uident,msgbuf,1,info_type)==1)
				{
					sprintf(repbuf, "�޸Ķ�%s��ȡ��%sȨ�޵Ĵ���",
					        uident,info_type==1?"����":"����");
					securityreport4(repbuf);
					if(msgbuf[0]!='\0')
						autoreport(repbuf,msgbuf,YEA,uident);
				}
			}
		}
		else if ((*ans == 'D' || *ans == 'd') && count)
		{
			/*	      //add for �Զ����
				      if(!(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_CHATCLOAK)))
					      return FULLUPDATE;*/
			move(2, 0);
			namecomplete("�����뱻ȡ���� ID: ", uident);
			if (uident[0] != '\0'&& SeekInNameList(uident))
			{
				//	    while(1)
				//		{
				getdata(3,0,"�����⸽��: ", buf,40,DOECHO,YEA);
				//	      if(killwordsp(buf)!=0)break;
				//	    }
				if (deldeny(uident,info_type))
				{
					sprintf(repbuf, "�ָ�%s��%s���%sȨ��",
					        uident,currboard, info_type==1?"����":"����");
					securityreport4(repbuf);
					sprintf(msgbuf, "\n  %s ���ѣ�\n\n"
					        "\t����ʱ���ѹ����ָֻ����� [%s] ��ġ�%s��Ȩ����\n\n",
					        uident, currboard,info_type==1?"��������":"���뱾��");
					if(strcmp(buf, ""))
						sprintf(msgbuf, "%s\t��⸽�ԣ�[%s]\n", msgbuf, buf);
					autoreport(repbuf,msgbuf,YEA,uident);
				}
			}
		} /*else if(*ans == 'R' || *ans == 'r') {
		         if(info_type == 1)setbfile(genbuf, currboard, "deny_users");
		         else setbfile(genbuf, currboard, "board.deny");
		         unlink(genbuf);
			 sprintf(repbuf, "ɾ�� [%s ��%s]����", currboard, 
			    info_type==1?"��ֹ����":"��ֹ����");
			 securityreport(repbuf);
			 return FULLUPDATE;
		      }*/ else
			break;
	}
	clear();
	return FULLUPDATE;
}
//add by yiyo ���β���
int Rangefunc(int ent, struct fileheader *fileinfo, char *direct)
{
	struct fileheader fhdr;
	char annpath[512];
	char buf[STRLEN],ans[8],info[STRLEN],bname[STRLEN],dbname[STRLEN];
	char items[8][7] =
	    {"����","��ժ","����RE","ɾ��","������","�ݴ�","ת��","�ָ�"};
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
	strcpy(info,"����:");
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
	prints("���β���: %s",items[type-1]);
	getdata(t_lines-1,20,"��ƪ���±��: ",ans,6,DOECHO,YEA);
	num1=atoi(ans);
	if(num1>0)
	{
		getdata(t_lines-1,40,"ĩƬ���±��: ",ans,6,DOECHO,YEA);
		num2=atoi(ans);
	}
	if(num1<=0||num2<=0||num2<=num1)
	{
		move(t_lines-1,60);
		prints("��������...");
		egetch();
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	if(type != 7)
	{
		sprintf(info,"���� [%s] ������Χ [ %d -- %d ]��ȷ����",
		        items[type-1], num1,num2);
		if(askyn(info,NA,YEA)==NA)
		{
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
	}
	/* add by yiyo ����ɾ�� */
	if(type == 4)
	{
		move(t_lines - 1, 0);
		if(askyn("�ϳ�ô[��������2]",NA,NA)==NA)
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
			presskeyfor("�Բ���, ��û���趨˿·. ������ f �趨˿·.",t_lines-1);
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
		fscanf(fn,"%s",annpath);
		fclose(fn);
		if (!dashd(annpath))
		{
			presskeyfor("���趨��˿·�Ѷ�ʧ, �������� f �趨.",t_lines-1);
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
	} //else if( type == 6 ){
	// if(askyn("[�����ݴ浵ģʽ: һ��/����] ���þ���ʽ��",YEA,YEA)==NA)
	//   full = 1;
	//   }
	else if ( type == 7 )
	{
		clear();
		prints("\n\n������������ת�ء�ת�ط�Χ�ǣ�[%d -- %d]\n",num1,num2);
		prints("��ǰ�����ǣ�[ %s ] \n", currboard);
		if(!get_a_boardname(bname,"������Ҫת��������������: "))
			return FULLUPDATE;
		if(!strcmp(bname,currboard)&&uinfo.mode != RMAIL)
		{
			prints("\n\n�Բ��𣬱��ľ�����Ҫת�صİ����ϣ���������ת�ء�\n");
			pressreturn();
			clear();
			return FULLUPDATE;
		}
		if(askyn("ȷ��Ҫת����",NA,NA)==NA)
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


/*�ɹ������������,ʧ�ܷ���0
  ��ģ���ļ����з���,���ɶ����Ƶĸ�ʽ�ļ�
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
            prints("ȱ�����");
            return 1;
        }
        if(end==len-1){
            prints("ȱ����������");    
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
/*��ģ���ļ�����,����֮   
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
			prints("�ļ���ʧ��.");
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
										 prints("ϵͳֻ֧��%d������.����Ľ�������ʾ.\n",MQ);
										 toomany=1;
										 i=MQ;
									}
								}							
						}//if toomany
                }//if titlefound
        }//while(1)
       
        if(i==0){
           prints("����:û���ҵ�ģ������.��������������,�Ա��û�����.\n");
           error++;
	}

        if(!titlefound) {
                prints("����:�Ҳ���ģ�����.������һ����\"title=\"��ʼ������.\n");
                error++;
        }
        if(!beginfound) {
                prints("����:�Ҳ���ģ�����Ŀ�ʼ��ʶ,\"begin:\"����ռһ��.\n");
                error++;
        }

		prints("�� %d ������.\n",error);
		
    	if(error){
		    pressreturn();
	            fcloseall();	
           	    return 0;
	}
	else{
	  	    prints("��ϲ,��Ч��ģ���ļ�!");
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
            prints("����ģ����:%4d\n", n);
           
	    move(2, 0);
  	    clrtoeol();
   	    showpattern(patternrecord);
        getdata(1,0,"(A)���� (D)ɾ�� (M)�޸�ģ���趨 (E)�༭ģ���ļ� (O)��� [O]: ",
         ans, 2, DOECHO, YEA);
	    if(ans[0]=='0'||ans[0]=='O') break;
        if(ans[0]!='a'&&ans[0]!='A'&&ans[0]!='d'&&ans[0]!='D'
		  &&ans[0]!='M'&&ans[0]!='m'&&ans[0]!='E'&&ans[0]!='e'){
	     
	    break; 
        }	     
      if (ans[0]=='m'||ans[0]=='d'||ans[0]=='e'||ans[0]=='D'||ans[0]=='M'||ans[0]=='E'){
         
         char ch[8];
		 getdata(t_lines-2,0, "���������(enter����):", ch, 3, DOECHO, YEA);
         sel=atoi(ch);
		 if (ch[0]=='\0') continue;
	     if(sel>n||sel<=0) {
			 prints("��������.");
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
			     prints("ÿ������ģ���������ó���%d.\n", MP);
				 break;
			   }
			   bzero(&p,sizeof(p));
		       getdata(2,0, "ģ������:", genbuf, 40, DOECHO, YEA);
			   strcpy(p.name,genbuf);
			  
			   Q:
			   sprintf(mybuf,"�������(���%d��,0ȡ��):",MQ);
  		       getdata(3,0, mybuf, genbuf, 3, DOECHO, YEA);
			   i=atoi(genbuf);
			   if(i==0) continue;
			   if(i<0||i>MQ) goto  Q;
			   p.question=i;
			   sprintf(p.file,"%d",time(0));
			   p.time=time(0);
			   strcpy(p.author,currentuser.userid);
			   append_record(patternrecord,&p,sizeof(p));
			
                           presskeyfor("���������ʼ�༭����ģ���ļ�.");  
			   setbfile(genbuf,currboard,p.file);
              		   ED:
			     aborted = vedit(genbuf, NA, YEA);
          		     if (aborted == -1) {
				   pressreturn();
				   continue;
          		     } 
			   int e=patterncompile(genbuf);
			   if(e==0){
               		   if(askyn("�ٱ༭��?", YEA, NA) == NA){
                       	   sprintf(mybuf,"%s.question",genbuf);
                           unlink(mybuf);
                           sprintf(mybuf,"%s.bin",genbuf);
			   unlink(mybuf);
                           clear();
                           move(10,30);
 		           presskeyfor("��ģ��Ŀǰ���ڲ�����״̬."); 
			   continue;
			   }
                           goto  ED;
			   }else{
			       if(e<p.question){
				     prints("��ģ���ʵ�������������õĲ�һ��,��ע���޸�."); 
   				     egetch();
				   }
			   }
			 
			   break;
               
         case 'D':
         case 'd':
               sprintf(genbuf,"ȷ��ɾ����%d��ģ����?",sel);
   	           if (askyn(genbuf, NA, NA) == NA){
			       prints(" ȡ��"); 
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
				   getdata(2,0, "ģ��������:", genbuf, 40, DOECHO, YEA);
			       strcpy(p.name,genbuf);
			       
			       QQ:
  		           getdata(3,0, "�������(���40��,0ȡ��):", genbuf, 3, DOECHO, YEA);
			       i=atoi(genbuf);
			       if(i==0) continue;
			       if(i<0||i>40) goto  QQ;
			       p.question=i;
				   sprintf(genbuf,"ȷ��Ҫ������?",sel);
    	           if (askyn(genbuf, NA, NA) == NA){
         			       prints(" ȡ��"); 
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
     prints(" %-10s%-40s%10s","���","����","�������");
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
		prints ("���ļ�ʧ��,��������web�Ƽ�!");
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
			sprintf (genbuf, "ȡ���Ƽ�����%-.55s", fileinfo->title);
			if (askyn(genbuf, NA, YEA) == NA)
			{
				move(t_lines - 1, 0);
				prints("����...");
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
					prints("�������Ѿ���������ҳ�����ˣ������ܹ�����ɾ���������������ܹ���ϵ");
					pressreturn();
					fclose(fp);
					return PARTUPDATE;
				}
				move(t_lines - 1, 0 );
			        if(delete_record(thesispath, sizeof(struct rmd), n + 1) != -1)
					prints("ȡ���Ƽ��ɹ���");
				else
					prints("ȡ��ʧ�ܣ�");
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
		prints("�����Ƽ����������������������Ƽ�%dƪ���£������Գ���ɾ����ǰ���Ƽ���\n", PMAXRMD);
		prints("���ĳƪ�����Ѿ���������ҳ���棬�������ܹ�ɾ���������������ܹ���ϵɾ��");
		pressreturn ();
		fclose(fp);
		return FULLUPDATE;
	}
	sprintf (genbuf, "�Ƽ����£���%-.55s", fileinfo->title);
	if (askyn(genbuf, NA, YEA) == NA)
	{
		move(t_lines - 1, 0);
		prints("����...");
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
		prints("�Ƽ����³ɹ�.");
		clrtoeol();
		egetch();
		fclose(fp);
		return FULLUPDATE;
	}
}
