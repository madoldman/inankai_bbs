/* $Id: newcon.c,v 1.5 2010-07-01 10:13:47 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"
int no_re=0;
/* 	bbscon?board=xx&file=xx&start=xx 	*/
char bufnav[2048];
int mode;
int start;
int total;
int ads=0;
char flgjunk=0;//huangxu@060406:����վ����

char title_in_url[800];
char title[256];

int page=0;
int perpage=10;
int pagecount=0;

int maxpage(int total,int size)
{
	int i=total/size;
	if(total%size)i++;
	return i-1;
}

int showpage(char * board, char * vfrom)
{
	      if(pagecount>perpage)
        {
        	printf("<div id=divpagesrc>");
					printf("<form action=newcon?board=%s&vfrom=%s&title=%s method=post><p align=right>",board,vfrom,title_in_url);
					printf("<a href=newcon?board=%s&vfrom=%s&title=%s>��һҳ</a>&nbsp;",board,vfrom,title_in_url);
					if(page>0)
						printf("<a href=newcon?board=%s&vfrom=%s&title=%s&page=%d>ǰһҳ</a>&nbsp;",board,vfrom,title_in_url,page);
					else
						printf("<font color=#666666>ǰһҳ</font>&nbsp;");
					printf("<a href=\"newcon?board=%s&vfrom=%s&title=%s&showall=yes\" title=ȫ��չ��><font color=red>��ǰҳ�룺%d/%d(%d��)</font></a>&nbsp;", board, vfrom, title_in_url,page+1,maxpage(pagecount,perpage)+1,pagecount);
					printf("��ת����<input type=text name=page value=%d size=4>ҳ<input type=submit value=Go>&nbsp;",page+1);
					if(pagecount>=perpage*(page+1))
						printf("<a href=newcon?board=%s&vfrom=%s&title=%s&page=%d>��һҳ</a>&nbsp;",board,vfrom,title_in_url,page+2);
					else
						printf("<font color=#666666>��һҳ</font>&nbsp;");
					printf("<a href=newcon?board=%s&vfrom=%s&title=%s&page=%d>��ĩҳ</a>&nbsp;",board,vfrom,title_in_url,maxpage(pagecount,perpage)+1);
					printf("</p></form>");
					printf("</div>");
					printf("<script language=javascript>var o=document.getElementById('divpagesrc');document.getElementById('divpage').innerHTML=o.innerHTML;</script>");
				}
				return 0;
}

int main()
{
        FILE *fp;
        char userid[80], buf[512], board[80], dir[80], file[80], filename[80];
		char startfile[80], vfrom[80];
		char vblink[1024];
        struct fileheader x, *xv;
        int i, found=0;
        //huangxu@060513
        init_all();
        modify_mode(u_info,READING);	//bluetent
        strsncpy(board, getparm("board"), 32);
		strsncpy(vfrom, getparm("vfrom"), 32);
		xv = getbcache(vfrom);
		if (!strcmp(board, vfrom))
		{
			xv = NULL;
		}
		if (!*vfrom || !xv)
		{
			strcpy(vfrom, board);
		}
		*vblink = 0;
		if (xv)
		{
			sprintf(vblink, "(VBoard: <a href=\"bbsdoc?board=%s\">%s</a>)", xv->filename, xv->title + 10);
		}
        strsncpy(file, getparm("file"), 32);
        mode=atoi(getparm("mode"));
		strsncpy(startfile, getparm("startfile"), sizeof(startfile));
        char *ptr=getparm("title");//huangxu@060401
        if ((!mode)&&(!*ptr)) mode=3;//huangxu@060401:��Щ���ⲻ������Ĭ�ϵ���modeΪ3
        start=atoi(getparm("num"));
        //huangxu@060513:��ҳ�ɡ����ɰ���ʮ��
        if(*getparm("showall"))perpage=1000000;
		if(i = atoi(getparm("perpage")))
		{
			perpage=i;
		}
        page=atoi(getparm("page"))-1;
        if(page<0)page=0;
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("�����������");
        if (!canenterboard (currentuser.userid, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal ("���Ѿ���ȡ�����뱾���Ȩ��");
        strcpy(board, getbcache(board)->filename);
        if(loginok)
                brc_init(currentuser.userid, board);
        if (strlen(getparm("junk"))&&(has_BM_perm(&currentuser, board)||HAS_PERM(PERM_FORCEPAGE)))
        	flgjunk=1;//huangxu@060406:junkģʽ
      printf("�Ķ����� ������: <b><a href=bbsdoc?board=%s>%s</a>%s</b>\n", board,board,vblink);
        if (flgjunk)//huangxu@060406:����վ��
        	sprintf(dir, "boards/%s/.junk",board);
        else if (*file == 'G')
			sprintf(dir, "boards/%s/.DIGEST", board);
		else
        	sprintf(dir, "boards/%s/.DIR", board);
        if(!strcmp(board, "noticeboard"))
                no_re=1;

        total=file_size(dir)/sizeof(struct fileheader);
        if(start>=total)
                start=total-1;
        if(start<0)
                start=0;
        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("Ŀ¼����");
        if(mode==3)
		{
                if(start==0){
                   while(1) {
					   if(fread(&x, sizeof(x), 1, fp)<=0){
			   printf("���²�����!");
                           break;
                       }
                       if(!strcmp(x.filename, file)) {
        printf("<a href=bbsdoc?start=%d&board=%s%s>������һҳ</a>",start,vfrom,flgjunk?"&junk=yes":"");
                            show_file(board,&x, vfrom);
                            break;
                       }
					   start++;
                   }
                   fcloseall();
                   return 0;
                }

                fseek(fp,sizeof(struct fileheader)*start,SEEK_SET);
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        http_fatal("��ȡ����.");
              //  ads=1;
                show_file(board,&x,vfrom);
                fcloseall();
               // googleads();
                return 0;
        }
 
        if(!strncmp(ptr, "Re:", 3))
            ptr+=4;
        strsncpy(title, ptr, 60);
        if(strcmp(file,"")) {
                if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                        http_fatal("����Ĳ���1");
                if(strstr(file, "..") || strstr(file, "/"))
                        http_fatal("����Ĳ���2");

                fseek(fp,sizeof(struct fileheader)*start,SEEK_SET);
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        http_fatal("��ȡ����.");
                if(strcmp(x.filename, file))
                        http_fatal("����,��ˢ��!");
                found=1;
        }else{
                while(1) {
                        if(fread(&x, sizeof(x), 1, fp)<=0)
						{
                                break;
						}
                        if(!titleComp(x.title, title) &&
							 (!*startfile || !strcmp(x.filename, startfile)))
							{
                                found=1;
                                break;
							}
                }
        }
        if(found==0)
                http_fatal("���²�����.");
        //			(*(int*)(x.title+72))++;
        //			fseek(fp, -1*sizeof(x), SEEK_CUR);
        //			fwrite(&x, sizeof(x), 1, fp);
        //fflush(fp);        //add by wzk
        //found=1;
#ifdef MUST_EXISTING_USER
	if(!http_postReadable(&x))http_fatal("���û�δע��");
#endif
	strcpy(userid, x.owner);
        kill_quotation_mark(title_in_url,title);
        printf("<div id=divpage></div>");
        /*  change by wzk for delete article */
       	if (userid[0]=='-')
                http_fatal("���������ѱ�ɾ��<br>\n");
        else
                /*  end of change */
                if(pagecount>=page*perpage&&pagecount<(page+1)*perpage)//huangxu@060513
                        show_file(board,&x,vfrom);
        int n=0;
        pagecount=1;
        strcpy(title,x.title);
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                //             num++;
                if(mode==2){
                        show_file(board,&x,vfrom);
                        n++;
                        if(n==7)
                                break;
                      //huangxu@060401
                      //Ϊ�˷�ֹһ��δ֪����
                      //�������Ǳ����ǳƲ���ȷ
                      //ֻ����ͷ������Ч
                }     else
                        //if(!strncmp(x.title+4, title, 39) && !strncmp(x.title, "Re: ", 4))
                        if(!titleComp(x.title, title))
						{
								 {
                        	
                                /*  change by wzk for delete article */
                                if (userid[0]!='-')
                                	{
                                		if (pagecount>=page*perpage&&pagecount<(page+1)*perpage)
                                        show_file(board,&x,vfrom);
                                    pagecount++;
                                  }
                                // printf("���������ѱ�ɾ��<br>\n");
                                //  else
                                /*  end of change */
                                //                      show_file(board, &x, num-1);
                                //}
							}
                        }
        }
        fclose(fp);
        /*if(found==0)
                http_fatal("������ļ���");*/
        /*  the next change by wzk for delete article */
        /*  mode=1����,Ī�����ʺ�
        if(!no_re && userid[0]!='-'){
                 printf("[<a href=bbspst?board=%s&vfrom=%s&file=%s&userid=%s&mode=1&title=\"%s\">�ظ�����</a>] ",
           board,vfrom, file, userid, title);
        }*/
        //huangxu@060513
        showpage(board, vfrom);
        printf("<center><a href=bbsdoc?start=%d&board=%s%s>������һҳ</a>",start,vfrom,flgjunk?"&junk=yes":"");
        /*ptr=x.title;
        if(!strncmp(ptr, "Re: ", 4))
                ptr+=4;
        //printf("\n");*/
        if(loginok)
                brc_update(currentuser.userid, board);
        //        check_msg();//bluetent 2002.10.31
        http_quit();
}
int show_file(char *board, struct fileheader *x, char * vfrom)
{
#ifdef MUST_EXISTING_USER
if(!http_postReadable(x))http_fatal("���û�δע��");
#endif
        char title[STRLEN];
	char blank[2];
        char *ptr=x->title;
	*blank = 0;
        if(!strncmp(ptr, "Re:", 3))
          ptr+=4;
        strsncpy(title, ptr, 60);

        brc_init(currentuser.userid, board);
        brc_add_read(x->filename);
        brc_update(currentuser.userid, board);
        //printf("ok");return 0;
        FILE *fp;
        char path[80], buf[512];
        if(loginok)
                brc_add_read(x->filename);
        sprintf(path, "boards/%s/%s", board, x->filename);
        fp=fopen(path, "r");
        if(fp==0)
        {
		http_fatal("���������ѱ�ɾ��<br>\n");
		   return;
	}
        printf("<p>");

        if(fgets(buf, 256, fp)==0)
        {
                printf("��ȡ�ļ�����!");
                fclose(fp);
                return 0;
        }
        char *id,*s;
	s=blank;// sillyhan@2009/01/10 init s
        if(!strncmp(buf, "������: ", 8))
        {
                ptr=strdup(buf);
                id=strtok(ptr+8, " ");
                s=strtok(0, ",");
                if(s==0)
                        s=blank;
                fgets(buf, 256, fp);
                fgets(buf, 256, fp);

        }
time_t t=atoi(x->filename+2);
getdatestring(t,NA);
      printf(" <a href=bbsqry?userid=%s><b>%s</b>%s</a> %s <br><font color=red>%s</font>",x->owner,x->owner,s,datestring,x->title);
       //delete by brew,,why FREE???
	//free(ptr);
        shownav(board,x,title,vfrom);
        printf("%s",bufnav);
        int w=80;
        if(ads){
           printf("<table><tr><td width=80% valign=top>");
           w=100;
        }
        printf("<table  style='BORDER: 1px solid; BORDER-COLOR: a0a0a0;'  cellpadding=0 cellspacing=0  width=%d%>\n",w);
        printf("<pre><tr> <td bgcolor=f6f6f6><pre><font color=navy>");
        int con=1,qmd=0,cite=0;
        while(1)
        {
                if(fgets(buf, 500, fp)==0)
                        break;
                if(!strcmp(board,"Pictures") || !strcmp(board,"SelfPhoto")||!strcmp(board,"Photography")) {
                        if(!strncmp(buf, ": ", 2))
                                continue;
                        if(!strncmp(buf, "�� �� ", 4))
                                continue;
                        hhprintf("%s", buf);
                        continue;
                }
                if(!strncmp(buf, ": ", 2) ||!strncmp(buf, "�� �� ", 4)) {
                        if(con==1&&qmd==0) {
                                printf("</pre></font></td></tr><tr><td><pre><font color=808080>");
                                con=0;
                                cite=1;
                        }
                        hhprintf("%s", buf);
                } else {


                        if(!strncmp(buf, "--", 2)) {
                                con=0;
                                qmd=1;
                                printf("</pre></font></td></tr><tr><td bgcolor=f6f6f6><pre>");
                        }

                        if(cite==1&&qmd==0) {
                                printf("</pre></font></td></tr><tr> <td bgcolor=f6f6f6><pre><font color=navy>");
                                cite=0;
                                con=1;
                        }
                        hhprintf("%s", buf);
                }
        }
        fclose(fp);
        printf("</td></tr></pre></table>");
        printf("</td><td width=20% valign=top>");
        if(ads) googleads();
        printf("</td></tr></table>");
        return 1;
}
int shownav(char *board, struct fileheader *x,char *title, char * vfrom)
{
//	char *title_in_url[200];
	kill_quotation_mark(title_in_url,title);
        strcpy(bufnav,"");
        if(mode==3)
            sprintf(bufnav,"%s<a href=newcon?board=%s&vfrom=%s&num=%d&mode=3%s> ��һƪ </a>  <a href=newcon?board=%s&vfrom=%s&num=%d&mode=3%s> ��һƪ</a>",bufnav,board,vfrom,(start-1<0)?0:(start-1),flgjunk?"&junk=yes":"",board,vfrom,(start+1>total)?total:(start+1),flgjunk?"&junk=yes":"");
        if (!flgjunk)//huangxu@060406
        {
        sprintf(bufnav, "%s<a href=\"bbspst?board=%s&vfrom=%s&file=%s&userid=%s&title=%s \"> �ظ�</a>", bufnav, board, vfrom, x->filename, x->owner, title_in_url);
        sprintf(bufnav, "%s<a              href=\"bbspstmail?board=%s&file=%s&userid=%s&title=%s \"> ����</a>", bufnav, board, x->filename, x->owner, title_in_url);
        sprintf(bufnav, "%s<a href=bbsfwd?board=%s&file=%s&vfrom=%s> ת��</a>", bufnav, board,x->filename, vfrom);
        sprintf(bufnav, "%s<a href=bbsccc?board=%s&file=%s&vfrom=%s> ת��</a>", bufnav, board, x->filename, vfrom);
      	}
        if(!strcasecmp(currentuser.userid, x->owner) || has_BM_perm(&currentuser, board))
        {
        				if (flgjunk)//huangxu@060406:�ָ�
                	sprintf(bufnav, "%s<a onclick='return confirm(\"�����Ҫ�ָ�������?\")' href=bbsrst?board=%s&file=%s> �ָ�</a>", bufnav, board, x->filename);
                else
                {
                	sprintf(bufnav, "%s<a onclick='return confirm(\"�����Ҫɾ��������?\")' href=bbsdel?board=%s&file=%s&vfrom=%s> ɾ��</a>", bufnav, board, x->filename, vfrom);
                	sprintf(bufnav, "%s<a href=bbsedit?board=%s&file=%s&vfrom=%s> �޸�</a>", bufnav, board, x->filename, vfrom);
                }
        }
        //if (HAS_PERM(PERM_BOARDS)||HAS_PERM(PERM_PERSONAL))
        {
        	sprintf(bufnav, "%s<a href=\"bbs2an?board=%s&file=%s\"> ����</a>",bufnav,board,x->filename);
        }
        if (!flgjunk)//huangxu@060406
        {
        	//huangxu@060408:ͬ���⻹�����ұ�
        	sprintf(bufnav, "%s<a href=\"/js/telnetview.html?url=/cgi-bin/bbs/jbbscon?board=%s&file=%s\" target=_blank> Telnet��ʽ</a>", bufnav, board, x->filename);
        	sprintf(bufnav, "%s<a href=\"newcon?board=%s&title=%s&vfrom=%s%s\"> ͬ����</a>", bufnav, board, title_in_url,vfrom,flgjunk?"&junk=yes":"");
        	sprintf(bufnav, "%s<a href=\"newcon?board=%s&perpage=1&title=%s&vfrom=%s%s\"> ͬ�����һƪ</a>", bufnav, board, title_in_url,vfrom,flgjunk?"&junk=yes":"");
        	sprintf(bufnav, "%s<a href=\"newcon?board=%s&startfile=%s&showall=1&title=%s&vfrom=%s%s\"> ͬ���������չ��</a>", bufnav, board, x->filename, title_in_url, vfrom, flgjunk?"&junk=yes":"");
        }
        return 0;
}
