/*$Id: bbsqry.c,v 1.3 2009-10-30 04:06:46 maybeonly Exp $
*/

#include "BBSLIB.inc"
//huangxu@060405:Web�鿴����
char * cmedal2 (unsigned int original, int num, int place)
{
	int	i;
	char retstr [256];
	strcpy (retstr, "");
	if (num <= 1)	// Efan: ����������С�� 2 �����
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
			//"\33[1;31m�� \33[1;33m����վ����\33[m"
			// 0  1234567891  1234567892123456789  31
			printf("<span title='%s %dö'>",tmpstr+17,tmp);
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
                printf("%s -- ��ѯ����<hr color=green>\n", BBSNAME);
                printf("<form action=bbsqry name=qry>\n");
                printf("�������û���: <input name=userid maxlength=12 size=12>\n");
                printf("<input type=submit value=��ѯ�û�>\n");
                printf("</form><hr>\n");
                printf("<script>document.qry.userid.focus();</script>");
                http_quit();
        }
        x=getuser(userid);
        if(x==0) {
                printf("�û� [%s] ������.", userid);
                http_quit();
        }
        printf("</center>\n");
        printf("<table align=center><tr><td><pre style='font-size:14px'>");
        sprintf(buf, "%s ([33m%s[37m) ����վ [32m%d[m ��  ",
                x->userid, x->username, x->numlogins);
        hprintf("%s", buf);
        //huangxu@060401:Web�Ա�
      	if ( HAS_DEFINE(x->userdefine, DEF_S_HOROSCOPE) )
	   			clr = (x->gender == 'F') ? 35 : 36;
				else
				/***********************************************************/
				clr = 32;	/* ���ܱ������۴� ?! :D */
        if(x->userdefine & DEF_S_HOROSCOPE)
                hprintf("[[1;%dm%s[m]", clr, horoscope(x->birthmonth, x->birthday));
        printf("\n");
//huangxu@060405:hideip����
//        hideip = !HAS_PERM(PERM_SEEUSERIP)&&(!(x->userdefine&DEF_NOTHIDEIP))&&strstr(x->lasthost, "10.10.");
//        if(hideip)
//                strcpy(ipstr, "�Ͽ���������");
//        else
                strcpy(ipstr, x->lasthost);

        getdatestring(x->lastlogin,NA);
        hprintf("�ϴ���: [[32m%s[37m] �� [[32m%s[37m] ����վһ�Ρ�\n", datestring, ipstr);
        if(x->lastlogout < x->lastlogin) {
                getdatestring(((time(0)-x->lastlogin)/120)%47+1+x->lastlogin,NA);
        } else {
                getdatestring(x->lastlogout,NA);
        }
        hprintf("��վʱ��: [[32m%s[37m] ", datestring);
        count_mails(x->userid, &tmp1, &tmp2);
        hprintf("����ֵ: [[32m%d[37m]([33m%s[37m) ", countperf(x), perfstr(x));
        if(tmp2!=0)
        	printf("[<font color=green>��</font>]\n");
        else
		printf("\n");

	//printf("����: [<font color=green>%d</font>/%d]\n", tmp2,tmp1);
        hprintf("������: [[32m%d[37m](%s) ", x->numposts, numpostsstr(x));
        hprintf("����ֵ: [[32m%d[37m]([33m%s[37m) ", countexp(x), expstr(x));
        hprintf("��������[[32m%d[37m]\n", count_life_value(x));
	
#ifdef ALLOW_MEDALS
        /* Efan: ������	*/
	//added by lionel
	/*int sum = 0;
	for(i = 1; i <= 7; i++){
		sum += (x -> nummedals >> (i << 2)) & 0x0f;
	}
        hprintf ("��������[\33[1;32m%d\33[37m] (%s):%d\n", sum, cmedal (x->nummedals),x->nummedals );*/
	tmpnum = x->nummedals;
	tmp1 = 0;  // ����λ���жϽ�������
	tmpi=0;
	for (i = 1; i <= 7; ++i) {
		int tmp = (tmpnum >> (i << 2)) & 0x0f;
		if (tmp != 0) {
			tmpi += tmp;
			tmp1 = 8 - i;
		}
	}
	tmpnum &= 0x0f;
	hprintf ("��������[\33[1;32m%d\33[37m] (", tmpi);
	cmedal2 (x->nummedals,tmpnum,tmp1);
	hprintf (")\n");		
#endif

        uleveltochar(&permstr, x->userlevel);
        hprintf("�û�Ȩ��: [%s]", permstr);
        show_special(x->userid);
        if(!(x->userlevel&PERM_SYSOP)&&(x->userlevel&PERM_RECLUSION)) {
                sprintf(buf, "home/%c/%s/Reclusion", toupper(x->userid[0]), x->userid);
                if(fp=fopen(buf,"r")) {
                        fscanf(fp, "%d", &Reclusion);
                        fclose(fp);
                }
                sprintf(life,"[\033[36m�����У�����%d��\033[m]", (Reclusion-(time(0)-x->lastlogout)/86400)>0?(Reclusion-(time(0)-x->lastlogout)/86400):0);
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
									strcat (bmlist, "��");
								}
                                sprintf(bmlist + strlen(bmlist), "<a href=\"bbsdoc?board=%s\">%s</a>", board, board);
                                //else
                                        //sprintf(bmlist,"%s��%s",bmlist,board);
                        }
                }
        }
        if ((x->userlevel&PERM_BOARDS)&&strcmp(bmlist,"")&&strcmp(x->userid,"SYSOP"))
                printf(" ����%s�����\n",bmlist);
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
                        	//huangxu@060401:��telnet��ͳһ
                                hprintf("Ŀǰ %s ״̬����:   ",u->userid);
                        if(u->invisible)
                                hprintf("[36m[��][37m");
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
                //		hprintf("Ŀǰ����վ��, �ϴ���վʱ�� [[1;32m%s[m]\n\n", Ctime(file_time(filename)));
                //		hprintf("Ŀǰ����վ��, �ϴ���վʱ�� [[1;32m%s[m]\n\n", now);

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
                hprintf("[36mû�и���˵����[37m\n");
        }
        printf("</pre>");
        printf("<br><br><a href=bbspstmail?userid=%s&title=û����>[д���ʺ�]</a> ", x->userid);

        //printf("<a href=bbssendmsg?toid=%s target=f4>[����ѶϢ]</a>\n",x->userid);
        printf("<a href=bbsfadd?userid=%s>[�������]</a> ", x->userid);
        printf("<a href=bbsfdel?userid=%s>[ɾ������]</a>", x->userid);

        sprintf(buf,"0Announce/PersonalCorpus/%c/%s",toupper(x->userid[0]),x->userid);
        if(file_exist(buf))
                printf("<a href=bbs0an?path=/PersonalCorpus/%c/%s >[�ι�%s���ļ�]</a>",toupper(x->userid[0]),x->userid,x->userid);

        //	printf("<hr>");
        printf("</table>");
        printf("</center>\n");
        http_quit();
}
