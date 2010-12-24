/*$Id: bbsman.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
*/

#include "BBSLIB.inc"
/*改进措施:应该在传入文件的同时,传入文件的序号.
*/

int start=0;
int main()
{
        int i, total=0, mode;
        char board[80], *ptr, buf[80];
        struct shortfile *brd;
        init_all();
        if(!loginok)
                http_fatal("请先登录");
        strsncpy(board, getparm("board"), 60);
        mode=atoi(getparm("mode"));
        start=atoi(getparm("start"));
        brd=getbcache(board);
        if(brd==0)
                http_fatal("错误的讨论区");
        strcpy(board, brd->filename);
        if(!has_BM_perm(&currentuser, board))
                http_fatal("你无权访问本页");
        if(mode<=0 || mode>8)
                http_fatal("错误的参数");
        printf("<table>");
        for(i=0; i<parm_num && i<40; i++) {
                if(!strncmp(parm_name[i], "box", 3)) {
                        total++;
                        if(mode==1) {
                                do_del(board, parm_name[i]+3);
                                continue;
                        }
                        if(mode==6) {
                                rmd(board, parm_name[i]+3);
                                continue;
                        }
                        if(mode==7) {
                                top(board,parm_name[i]+3);
                                continue;
                        }
                        if(mode==8) {
                                digest(board,parm_name[i]+3);
                                continue;
                        }
                        do_set(board, parm_name[i]+3, mode);
                }
        }
        printf("</table>");
        if(total<=0) {
                printf("请先选定文章<br>\n");
                printf("<br><a href=javascript:history.go(-1)>返回管理模式</a>");
                return 0;
        }
        //	printf("<tr><td><center><font color=red>正在跳转...</font></center>\n");
        printf("<a href=bbsmdoc?board=%s&start=%d>操作完成,返回</a>", board, start);

        http_quit();
}

int digest(char *board,char *file)
{

        char buf[PATHLEN],path[PATHLEN],cmd[PATHLEN+STRLEN];
        FILE *fp;
        sethomefile(buf, currentuser.userid,".announcepath");
        if((fp = fopen(buf, "r")) == NULL ) {
                showinfo("您还没有设定丝路,请浏览本板精华区设定丝路");
                return 1;
        }
        fscanf(fp,"%s",path);
        fclose(fp);
        if(!file_exist(path)) {
                printf("无效的路径:%s.丝路已丢失,请重新设定.",path);
                return 1;
        }
        sprintf(buf,"/%s",board);
        if(!strstr(path,buf)) {
                printf("你设定的丝路不在本板内,请确认.<br>丝路:%s<br>讨论区:%s",path,board);
                return 1;
        }


        char dir[STRLEN],filepath[STRLEN];
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(filepath, "boards/%s/%s", board, file);
        fp=fopen(dir, "r+");
        if(fp==0) {
                showinfo("错误的板面.");
                return 1;
        }

        struct fileheader f;
        int i=0;
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                i++;
                if(strcmp(f.filename, file))
                        continue;
                if(f.accessed[0] & FILE_VISIT) {

                        sprintf(cmd,"%s/%s",path,file);
                        if(file_exist(cmd)) {
                                printf("<br>%s %s:在丝路里已经存在该文件,跳过",file,f.title);
                                fclose(fp);
                                return 1;
                        } else
                                printf("<br>提醒:%s %s  以前被收入过精华区",file,f.title);
                }
                f.accessed[0] |= FILE_VISIT;  /* 将标志置位 */
                fseek (fp, -1 * sizeof (struct fileheader), SEEK_CUR);
                fwrite (&f, sizeof (struct fileheader), 1, fp);
                fclose (fp);

                sprintf(cmd,"%s/.Names",path);
                fp=fopen(cmd,"a+");
                if(fp==0) {
                        showinfo("不能打开.Names文件,放弃");
                        return 1;
                }
                fprintf(fp,"Name=%s\nPath=~/%s\nNumb=\n#\n",f.title,file);
                fclose(fp);
                sprintf(cmd,"cp %s %s",filepath,path);
                system(cmd);

/*                if(strcmp(board,"Pictures"))
                        return 0;
                sprintf(buf,"%s/file_appendix",UPLOAD_PATH);
                fp=fopen(buf,"r");
                if(fp==0) {
                        printf("文章%s的附件收入精华区失败",file);
                        return 1;
                }



                struct file_to_appendix a;
                int done=0;
                int filestamp=atoi(file+2);
                while(1) {
                        if(fread(&a, sizeof(struct file_to_appendix), 1, fp)<=0)
                                break;

                        if(a.sign)
                                continue;
                        if(strcmp(a.path,"Pictures"))
                                continue;
                        if(a.filename!=filestamp) {
                                if(done)
                                        break;
                                continue;
                        }
                        done=1;
                        sprintf(buf,"%s/picdigest.log",UPLOAD_PATH);
                        if(!file_exist(buf)) {
                                fp=fopen(buf,"w+");
                                fclose(fp);
                        }
                        append_record(&a,sizeof(struct appendix),buf);


                        sprintf(path,"%s/picdigest",UPLOAD_PATH);
                        if(!file_exist(path))
                                mkdir(path,0770);
                        if(a.appendixname[0])
                                sprintf(cmd,"cp %s/file/%s  %s",UPLOAD_PATH,a.appendixname,path);
                        system(cmd);

                }*/
                return 0;

        }
        fclose(fp);
        showinfo("文件不存在.\n");
        return 0;
}



int do_del(char *board, char *file)
{
        FILE *fp,*fp2;
        int num=0;
        char path[256], buf[256], dir[256], jdir[256],*id=currentuser.userid;
        struct fileheader f;
        struct userec *u;
        struct shortfile *brd=getbcache(board);
	if(!HAS_PERM(PERM_SYSOP))
        	if(!strcmp(board,"deleted")||!strcmp(board,"junk")||!strncmp(board,"syssecurity",11)||!strncmp(board,"syssecurity2",12)||(!strcmp(board,"Punishment")&&!HAS_PERM(PERM_BLEVELS)))
			http_fatal("您没有权力删文");
	sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/%s", board, file);
	sprintf(jdir, "boards/%s/.junk",board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("错误的参数");

        //存在效率问题,也许用static的变量可以解决
        //static int start ,每次从上一次结束的地方开始搜索
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {
			fp2=fopen(jdir, "a");
			fwrite(&f, sizeof(struct fileheader), 1, fp2);
			fclose(fp2);
			
                        del_record(dir, sizeof(struct fileheader), num);
                        sprintf(buf, "※ %s 于%s 删除[FROM: %s]", currentuser.userid, 4+Ctime(time(0)), fromhost);
                        f_append(path, buf);
                        //post_article("junk", f.title, path, f.owner, "", fromhost, -1);
                        //unlink(path);
                        printf("<tr><td>%s  <td>标题:%s <td>删除成功.\n", f.owner, nohtml(f.title));
                        u=getuser(f.owner);
                        if(!junkboard(board) && u) {
                                if(u->numposts>0)
                                        u->numposts--;
                                save_user_data(u);
                        }
                        sprintf(buf, "%s %-12s bbsmdel %s\n", Ctime(time(0))+4, id, board);
                        f_append("trace", buf);
                        fclose(fp);
                        return;
                }
                num++;
        }
        fclose(fp);
        printf("<tr><td><td>%s<td>文件不存在.\n", file);
}


int top(char *board, char *file)
{
        char path[STRLEN], dir[STRLEN];
        struct fileheader f;
        FILE *fp;
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/.top", board);
        char topstyle=atoi(getparm("topstyle"));//huangxu@060407:多彩置顶

        if(!file_exist(path)) {

                fp=fopen(path,"w+");
                if(fp==NULL) {
                        printf("放弃：不能创建.top文件.");
                        http_quit();
                        return;
                }
        } else {
                fp=fopen(path,"r");
                if(fp==NULL) {
                        printf("放弃：不能打开.top文件.");
                        http_quit();
                        return;
                }

                int i=0;
                while(1) {
                        if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                                break;
                        if(!strcmp(f.filename,file)) {
                                printf("此文章已被置顶.");
                                fclose(fp);
                                http_quit();
                                return;
                        }
                        i++;

                }

                if(i>=MAXTOP) {
                        printf("放弃:每个板面最多只允许%d篇置顶.",MAXTOP);
                        fclose(fp);
                        http_quit();
                        return;
                }
        }//else



        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("错误的参数");
        int i=0;
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {
                        f.accessed[1] |= FILE_TIP;
                        fseek (fp, -1 * sizeof (struct fileheader), SEEK_CUR);
                        fwrite (&f, sizeof (struct fileheader), 1, fp);
                        fclose (fp);
                        f.accessed[1] &= ~FILE_TIP;
                        //huangxu@060407:置顶显示原有作者
                        //strcpy (f.owner, currentuser.userid);
                        f.accessed[0] = 0;
                        f.accessed[1] |= FILE_ONTOP;
                        f.accessed[1] |= topstyle;//huangxu@060407
                        append_record(&f,sizeof(struct fileheader),path);
                        return;
                }
                i++;
        }
        fclose(fp);
        printf("<tr><td><td>%s<td>文件不存在.\n", file);
}//top






int rmd(char *board, char *file)
{
        char path[STRLEN], dir[STRLEN];
        struct fileheader f;
        struct rmd r;

        FILE *fp;
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/.thesis", board);

        if(!file_exist(path)) {

                fp=fopen(path,"w+");
                if(fp==NULL) {
                        printf("放弃：不能创建.thesis文件.");
                        http_quit();
                        return;
                }
        }


        fp=fopen(path,"r");
        if(fp==NULL) {
                printf("放弃：不能打开.thesis文件.");
                http_quit();
                return;
        }

        int i=0;
        while(1) {
                if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                        break;
                if(!strcmp(r.en,file)) {
                        printf("您已经推荐过这篇文章了吧?");
                        fclose(fp);
                        http_quit();

                        return;
                }
                i++;

        }

        if(i>=PMAXRMD&& !HAS_PERM(PERM_SYSOP)) {
                printf("您的推荐额已满。每位板务最多可以推荐%d篇文章，您可以尝试删除你以前的推荐。注：如果某篇文章前有一个◆,测说明该推荐\
                       已经被放在首页里面，您将不能够删除.有问题请于讨论区总管联系",PMAXRMD);
                fclose(fp);
                http_quit();
                return;
        }



        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("错误的参数");
        i=0;
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {
                        // del_record(dir, sizeof(struct fileheader), i);
                        bzero(&r,sizeof(struct rmd));
                        strcpy(r.owner,f.owner);
                        strcpy(r.rmder,currentuser.userid);
                        strcpy(r.en,file);
                        strcpy(r.cn,f.title);
                        strcpy(r.board,board);
                        r.sign=0;
                        time(&r.rmdtime);
                        r.psttime=atoi(file+2);



                        append_record(&r,sizeof(struct rmd),path);
                        printf("success");
                        return;


                }
                i++;
        }




        fclose(fp);

        printf("<tr><td><td>%s<td>文件不存在.\n", file);
}//rmd

int do_set(char *board, char *file, int mode)
{
        FILE *fp, *fp2;
        char path[256], dir[256], buf[256], tmp[8], newfile[80];
        struct fileheader f, f2;
        int num=0;
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/%s", board, file);
        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("错误的参数");
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {
                        //			f.accessed[0]=flag;
                        switch(mode) {
                        case 2:
                                if (f.accessed[0] & FILE_MARKED) {
                                        f.accessed[0] &= ~FILE_MARKED;
                                } else {
                                        f.accessed[0] |= FILE_MARKED;
                                }
                                break;
                        case 3:
                                if (f.accessed[0] & FILE_DIGEST) {
                                        f.accessed[0] &= ~FILE_DIGEST;
                                        strcpy(newfile, f.filename);
                                        newfile[0]='G';
                                        sprintf(buf, "/bin/rm %s/boards/%s/%s", BBSHOME, board, newfile);
                                        system(buf);
                                        //////////////////////////////////////////////////////////////////////////
                                        sprintf(dir, "boards/%s/.DIGEST", board);
                                        if(fp2=fopen(dir, "r")) {
                                                while(1) {
                                                        if(fread(&f2, sizeof(struct fileheader), 1, fp2)<=0)
                                                                break;
                                                        if(!strcmp(f2.filename, newfile)) {
                                                                //        	if(strcasecmp(id, f.owner) && !has_BM_perm(&currentuser, board))
                                                                //				http_fatal("你无权删除该文");
                                                                del_record(dir, sizeof(struct fileheader), num);
                                                        }
                                                        num++;
                                                }
                                                fclose(fp2);
                                        }
                                        //////////////////////////////////////////////////////////////////////////
                                } else {
                                        f.accessed[0] |= FILE_DIGEST;
                                        sprintf(buf, "boards/%s/.DIGEST", board);
                                        if(fp2=fopen(buf, "a")) {
                                                f.filename[0]='G';
                                                strcpy(newfile, f.filename);
                                                strcpy(tmp, f.accessed);
                                                f.accessed[0]=0;
                                                fwrite(&f, sizeof(f), 1, fp2);
                                                fclose(fp2);
                                                f.filename[0]='M';
                                                strncpy(f.accessed, tmp, 1);
                                                sprintf(buf, "/bin/cp %s/boards/%s/%s %s/boards/%s/%s", BBSHOME, board, f.filename, BBSHOME, board, newfile);
                                                system(buf);
                                        }
                                }
                                break;
                        case 4:
                                if (f.accessed[0] & FILE_NOREPLY) {
                                        f.accessed[0] &= ~FILE_NOREPLY;
                                } else {
                                        f.accessed[0] |= FILE_NOREPLY;
                                }
                                break;
                        default:
                                f.accessed[0]=0;
                        }
                        if(mode==0)
                                f.accessed[0]=0;
                        fseek(fp, -1*sizeof(struct fileheader), SEEK_CUR);
                        fwrite(&f, sizeof(struct fileheader), 1, fp);
                        fclose(fp);
                        //			printf("<tr><td>%s<td>标题:%s<td>标记成功.\n", f.owner, nohtml(f.title));
                        return;
                }
        }
        fclose(fp);
        printf("<td><td><td>%s<td>文件不存在.\n", file);
}
