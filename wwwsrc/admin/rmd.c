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
                bm=1;//板务
        if(HAS_PERM(PERM_OBOARDS))
                ob=1;//讨论区


        if(mode==0) {//显示管理页面
                /*   if(!ob) {
                 showinfo("对不起,您无权访问该页面.");
                 return;
                }
                */


                printf("<a name=top></a>");
                printf("\
                       <center>%s -- 文章导读系统 [使用者: %s]<hr color=%s></center>", BBSNAME, currentuser.userid,mytheme.con);

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
                                                                        printf("<tr><td width=30>◆</td>");
                                                                else
                                                                        printf("<tr><td width=30>&nbsp</td>");
                                                                printf("<td align=left width=50%><a href=\"bbstcon?board=%s&file=%s\">%s%46.46s </a></td>", name, r.en,
                                                                       strncmp(r.cn, "Re: ", 4) ? "○ " : "",r.cn);
                                                                printf("<td align=left width=15%>%s</td>", userid_str(r.owner));
                                                                printf("<td align=left >%12.12s</td>", Ctime(r.rmdtime)+4);
                                                                if(ob) {
                                                                        if(r.sign==0)
                                                                                printf("<td align=right><a href=rmd?mode=3&&board=%s&&file=%s>挂上首页</a></td></tr>",r.board,r.en);
                                                                        else
                                                                                printf("<td align=right> <a href=rmd?mode=4&&board=%s&&file=%s>取下</a></td></tr>",r.board,r.en);
                                                                }
                                                                k++;
                                                        }  //while
                                                        printf("</table>");

                                                }//if fp
                                        } //if exists
                                }//if

                        }//for boards

                }//for sec

                printf("<br><br>共有%d篇文章,%d个板面有推荐文章",k,b);
                printf("<br>推荐文章停留时间最多一周。");
                return;

        }//if mode==0

        if(mode==3) {//挂上首页
                if(!ob) {
                        showinfo("对不起,您无权访问该页面.");
                        return;
                }

                int total=file_size(PATHRMD)/(sizeof(struct rmd));
                if(total>=MAXINDEX) {
                        char bufinfo[STRLEN];
                        sprintf(bufinfo,"首页上内容过多,只允许%d篇",MAXINDEX);
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
                        showinfo("放弃：不能打开.thesis文件.");

                        return;
                }

                int i=0;
                while(1) {
                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                break;
                        if(!strcmp(r.en,file)) {
                                if(r.sign==1) {

                                        showinfo("该文章已经在首页里面了");

                                        fclose(fp);

                                        return;
                                }

                                r.sign=1;
                                put_record(&r,sizeof(struct rmd), i,path);
                                append_record(&r,sizeof(struct rmd), PATHRMD);
                                showinfo("操作成功");
                                return;
                        }
                        i++;

                }

                showinfo("文件不在推荐列表中");
        }//if mode==3



        if(mode==4) {//从主页移除
                if(!ob) {
                        showinfo("对不起,您无权访问该页面.");
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
                        showinfo("放弃：不能打开recommanded文件.");
                        return;
                }

                int i=0;
                while(1) {
                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                break;
                        if(!strcmp(r.en,file)) {
                                del_record(PATHRMD,sizeof(struct rmd), i);
                                if(!clearsign(board,file)) {
                                        showinfo("操作成功");
                                        return;
                                } else {
                                        append_record(&r,sizeof(struct rmd), PATHRMD);
                                        showinfo("删除失败");
                                        return;

                                }




                        }
                        i++;

                }

                showinfo("文件并没有被挂上首页");
        }//if mode==3













        if(mode==1) {//删除 斑竹取消推荐文章
                if(!bm) {
                        showinfo("你无权访问该页!");
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
                        showinfo("放弃：不能打开.thesis文件.");
                        return;
                }

                int i=0;
                while(1) {
                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                break;
                        if(!strcmp(r.en,file)) {
                                if(r.sign==1) {

                                        showinfo("该文章已经被放在首页里面了， 您不能够将其删除.有问题请于讨论区总管联系");

                                        fclose(fp);

                                        return;
                                }
                                del_record(path, sizeof(struct rmd), i);
                                showinfo("操作成功");
                                return;
                        }
                        i++;

                }

                showinfo("文件不在推荐列表中");

        }//if mode1



        if(mode==2) {//分区显示文章导读
                int group=atoi(getparm("group"));
                printf("\
                       <center>%s -- 文章导读 [使用者: %s]<hr color=%s></center>", BBSNAME, currentuser.userid,mytheme.con);
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
                                                                printf("<tr><td width=30>◆</td>");
                                                        else
                                                                printf("<tr><td width=30>&nbsp</td>");
                                                        printf("<td align=left width=50%><a href=\"bbstcon?board=%s&file=%s\">%s%46.46s </a></td>", name, r.en,
                                                               strncmp(r.cn, "Re: ", 4) ? "○ " : "",r.cn);
                                                        printf("<td align=left width=15%>%s</td>", userid_str(r.owner));
                                                        printf("<td align=left >%12.12s</td></tr>", Ctime(atoi(r.en+2))+4);

                                                        k++;
                                                }  //while
                                                printf("</table>");

                                        }//if fp
                                } //if exists
                        }//if

                }//for
                printf("%d篇文章  %d 个板面",k,b);
                http_quit();
                return;
        }//fi mode 2












        printf("</font></body>");
        http_quit();
}


/* 判断这个板面是不是位于对应区 */
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
                showinfo("放弃：不能打开.thesis文件.");

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

        return 0;//永远不会出现这种情况
}

