/*$Id: rmd.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
*/
#include"BBSLIB.inc"
int  main()
{
        init_all();
        printf("<body background=%s ><font color=%s>",mytheme.bgpath,mytheme.mf);

        int mode=atoi(getparm("mode"));
        int bm=0,ob=0;


        char board[STRLEN];
        struct shortfile * bd;
        strsncpy(board, getparm("board"), 60);
        if(has_BM_perm(&currentuser, board))
                bm=1;//����
        if(HAS_PERM(PERM_OBOARDS))
                ob=1;//������


        if(mode==0) {//��ʾ����ҳ��
                /*   if(!ob) {
                 showinfo("�Բ���,����Ȩ���ʸ�ҳ��.");
                 return;
                }
                */


                printf("<a name=top></a>");
                printf("\
                       <center>%s -- ���µ���ϵͳ [ʹ����: %s]<hr color=%s></center>", BBSNAME, currentuser.userid,mytheme.con);

                int i=0,b=0,k=0;


                for(i=0; i<SECNUM; i++) {
                        printf("<a href=#%d>|%s|</a>",i,secname[i][0]);
                }

                printf("<br><br>");
                for(i=0; i<SECNUM; i++) {
                        printf("<br><table width=100%><tr><td><font size=3><a name=%d></a> %s&nbsp;<a href=bbsboa?%d>%s</a></font></td><td width=10% align=right><a href=#top>top</a></td></tr></table>",i,seczonename[i],i, secname[i][0]);


                        //FILE *fp;
                        //fp=fopen("0Announce/bbslist/mactodayboard2", "r");
                        //if(fp==0) {
                        //        printf("can not  open board file");
                        //        return;
                        //}

                        int       j=0,t=0;
                        struct rmd r;
                        char path[STRLEN];
                        char buf[256], tmp[80], name[80], cname[80];
                        
                        for(j=0; j<MAXBOARD; j++) {
                                //if(fgets(buf, 150, fp)==0)
                                //        break;
                                if(j==0)
                                        continue;
                                //t=sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp, cname, tmp);
                                bd=&(shm_bcache->bcache[j]);
                                strcpy(name, bd->filename);
                                strcpy(cname, bd->title + 11);
                                
                                if(/*t==6*/isthiskind(name, i)) {

                                        /* printf("<table  width=30%>");
                                         printf("<tr><td bgcolor=%s><a href=bbs%sdoc?board=%s>%s&nbsp%s</a></td></tr></table> ",mytheme.con, (currentuser.userdefine & DEF_THESIS)?"t":"", name,name,cname);
                                        */ sprintf(path, "boards/%s/.thesis", name);
                                        if(file_exist(path)) {
                                                FILE *fp=fopen(path,"r");
                                                if(fp!=NULL) {

                                                        int total=file_size(path)/(sizeof(struct rmd));
                                                        if(total==0) {
                                                                //fclose(fp);
                                                                continue;
                                                        }
                                                        b++;
                                                        printf("<table  width=30%>");
                                                        printf("<tr><td bgcolor=%s><a href=bbs%sdoc?board=%s>%s&nbsp%s</a></td></tr></table> ",mytheme.con, (currentuser.userdefine & DEF_THESIS)?"t":"", name,name,cname);
                                                        printf("<table width=100%>");
                                                        while(1) {
                                                                if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                                                        break;
                                                                if(r.sign==1)
                                                                        printf("<tr><td width=30>��</td>");
                                                                else
                                                                        printf("<tr><td width=30>&nbsp</td>");
                                                                printf("<td align=left width=50%><a href=\"bbstcon?board=%s&file=%s\">%s%46.46s </a></td>", name, r.en,
                                                                       strncmp(r.cn, "Re: ", 4) ? "�� " : "",r.cn);
                                                                printf("<td align=left width=15%>%s</td>", userid_str(r.owner));
                                                                printf("<td align=left >%12.12s</td>", Ctime(r.rmdtime)+4);
                                                                if(ob) {
                                                                        if(r.sign==0)
                                                                                printf("<td align=right><a href=rmd?mode=3&&board=%s&&file=%s>������ҳ</a></td></tr>",r.board,r.en);
                                                                        else
                                                                                printf("<td align=right> <a href=rmd?mode=4&&board=%s&&file=%s>ȡ��</a></td></tr>",r.board,r.en);
                                                                }
                                                                k++;
                                                        }  //while
                                                        printf("</table>");

                                                }//if fp
                                        } //if exists
                                }//if

                        }//for boards

                }//for sec

                printf("<br><br>����%dƪ����,%d���������Ƽ�����",k,b);
                printf("<br>�Ƽ�����ͣ��ʱ�����һ�ܡ�");
                return;

        }//if mode==0

        if(mode==3) {//������ҳ
                if(!ob) {
                        showinfo("�Բ���,����Ȩ���ʸ�ҳ��.");
                        return;
                }

                int total=file_size(PATHRMD)/(sizeof(struct rmd));
                if(total>=MAXINDEX) {
                        char bufinfo[STRLEN];
                        sprintf(bufinfo,"��ҳ�����ݹ���,ֻ����%dƪ",MAXINDEX);
                        showinfo(bufinfo);
                        return;
                }


                char file[STRLEN],board[STRLEN];
                strcpy(file,getparm("file"));
                strcpy(board,getparm("board"));
                char path[STRLEN];
                struct rmd r;

                FILE *fp;
                sprintf(path, "boards/%s/.thesis", board);


                fp=fopen(path,"r");
                if(fp==NULL) {
                        showinfo("���������ܴ�.thesis�ļ�.");

                        return;
                }

                int i=0;
                while(1) {
                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                break;
                        if(!strcmp(r.en,file)) {
                                if(r.sign==1) {

                                        showinfo("�������Ѿ�����ҳ������");

                                        fclose(fp);

                                        return;
                                }

                                r.sign=1;
                                put_record(&r,sizeof(struct rmd), i,path);
                                append_record(&r,sizeof(struct rmd), PATHRMD);
                                showinfo("�����ɹ�");
                                return;
                        }
                        i++;

                }

                showinfo("�ļ������Ƽ��б���");
        }//if mode==3



        if(mode==4) {//����ҳ�Ƴ�
                if(!ob) {
                        showinfo("�Բ���,����Ȩ���ʸ�ҳ��.");
                        return;
                }


                char file[STRLEN],board[STRLEN];
                strcpy(file,getparm("file"));
                strcpy(board,getparm("board"));
                char path[STRLEN];
                struct rmd r;

                FILE *fp;



                fp=fopen(PATHRMD,"r");
                if(fp==NULL) {
                        showinfo("���������ܴ�recommanded�ļ�.");
                        return;
                }

                int i=0;
                while(1) {
                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                break;
                        if(!strcmp(r.en,file)) {
                                del_record(PATHRMD,sizeof(struct rmd), i);
                                if(!clearsign(board,file)) {
                                        showinfo("�����ɹ�");
                                        return;
                                } else {
                                        append_record(&r,sizeof(struct rmd), PATHRMD);
                                        showinfo("ɾ��ʧ��");
                                        return;

                                }




                        }
                        i++;

                }

                showinfo("�ļ���û�б�������ҳ");
        }//if mode==3













        if(mode==1) {//ɾ�� ����ȡ���Ƽ�����
                if(!bm) {
                        showinfo("����Ȩ���ʸ�ҳ!");
                        return;
                }
                char file[STRLEN];
                strcpy(file,getparm("file"));
                char path[STRLEN];
                struct rmd r;

                FILE *fp;

                sprintf(path, "boards/%s/.thesis", board);


                fp=fopen(path,"r");
                if(fp==NULL) {
                        showinfo("���������ܴ�.thesis�ļ�.");
                        return;
                }

                int i=0;
                while(1) {
                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                break;
                        if(!strcmp(r.en,file)) {
                                if(r.sign==1) {

                                        showinfo("�������Ѿ���������ҳ�����ˣ� �����ܹ�����ɾ��.�����������������ܹ���ϵ");

                                        fclose(fp);

                                        return;
                                }
                                del_record(path, sizeof(struct rmd), i);
                                showinfo("�����ɹ�");
                                return;
                        }
                        i++;

                }

                showinfo("�ļ������Ƽ��б���");

        }//if mode1



        if(mode==2) {//������ʾ���µ���
                int group=atoi(getparm("group"));
                printf("\
                       <center>%s -- ���µ��� [ʹ����: %s]<hr color=%s></center>", BBSNAME, currentuser.userid,mytheme.con);
                printf(" %s&nbsp;<a href=bbsboa?%d>%s</a>",seczonename[group],group, secname[group][0]);

                FILE *fp;
                fp=fopen("0Announce/bbslist/mactodayboard2", "r");
                if(fp==0) {
                        printf("can not  open board file");
                        return;
                }

                int       k=0,j=0,i;
                struct rmd r;
                char path[STRLEN];
                char buf[256], tmp[80], name[80], cname[80];
                int b=0;
                for(j=0; j<MAXBOARD; j++) {
                        if(fgets(buf, 150, fp)==0)
                                break;
                        if(j==0)
                                continue;
                        i=sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp, cname, tmp);
                        if(i==6&&isthiskind(name, group)) {
                                b++;
                                printf("<table  width=30%>");
                                printf("<tr><td bgcolor=%s><a href=bbs%sdoc?board=%s>%s&nbsp%s</a></td></tr></table> ",mytheme.con, (currentuser.userdefine & DEF_THESIS)?"t":"", name,name,cname);
                                sprintf(path, "boards/%s/.thesis", name);
                                if(file_exist(path)) {
                                        FILE *fp=fopen(path,"r");
                                        if(fp!=NULL) {
                                                printf("<table width=100%>");
                                                while(1) {
                                                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                                                break;
                                                        if(r.sign==1)
                                                                printf("<tr><td width=30>��</td>");
                                                        else
                                                                printf("<tr><td width=30>&nbsp</td>");
                                                        printf("<td align=left width=50%><a href=\"bbstcon?board=%s&file=%s\">%s%46.46s </a></td>", name, r.en,
                                                               strncmp(r.cn, "Re: ", 4) ? "�� " : "",r.cn);
                                                        printf("<td align=left width=15%>%s</td>", userid_str(r.owner));
                                                        printf("<td align=left >%12.12s</td></tr>", Ctime(atoi(r.en+2))+4);

                                                        k++;
                                                }  //while
                                                printf("</table>");

                                        }//if fp
                                } //if exists
                        }//if

                }//for
                printf("%dƪ����  %d ������",k,b);
                http_quit();
                return;
        }//fi mode 2












        printf("</font></body>");
        http_quit();
}


/* �ж���������ǲ���λ�ڶ�Ӧ�� */
int isthiskind(char *board, int sec)
{
        struct shortfile *x;
        int i;
        for(i=0; i<MAXBOARD; i++) {
                x=&(shm_bcache->bcache[i]);
                if(!strcmp(x->filename, board))
                        break;
        }
        if(x->filename[0]<=32 || x->filename[0]>'z')
                return 0;
        if(!has_read_perm(&currentuser, x->filename))
                return 0;
        if(!strchr(seccode[sec], x->title[0]))
                return 0;
        return 1;

}

int clearsign(char *board,char *file)
{
        FILE *fp;
        char path[STRLEN];
        sprintf(path, "boards/%s/.thesis", board);
        fp=fopen(path,"r");
        if(fp==NULL) {
                showinfo("���������ܴ�.thesis�ļ�.");

                return 1;
        }
        struct rmd r;

        int i=0;
        while(1) {
                if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                        break;
                if(!strcmp(r.en,file)) {

                        r.sign=0;
                        put_record(&r,sizeof(struct rmd), i,path);
                        return 0;
                }
                i++;

        }

        return 0;//��Զ��������������
}

