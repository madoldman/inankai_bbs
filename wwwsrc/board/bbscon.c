/*$Id: bbscon.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"
char bufnav[2048];

int main()
{
        FILE *fp;
        char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
        struct fileheader x;
        int num, tmp, total, mode=0;
        init_all();
        modify_mode(u_info,READING+20000);	//bluetent
        mode=atoi(getparm("mode"));//如果是阅读文摘区则mode为1, 管理区为2
        strsncpy(board, getparm("board"), 32);
        strsncpy(file, getparm("file"), 32);
        num=atoi(getparm("num"));
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("错误的讨论区");
        if (!canenterboard (currentuser.userid, board))
                http_fatal ("您已经被取消进入本板的权限");
        strcpy(board, getbcache(board)->filename);
        if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                http_fatal("错误的参数1");
        if(strstr(file, "..") || strstr(file, "/"))
                http_fatal("错误的参数2");

        int iscompact=0;
        struct compact p;
        if(mode==1)
                sprintf(dir, "boards/%s/.DIGEST", board);
        else
                if(mode==3) {
                        iscompact=1;
                        char path[STRLEN];
                        int compact=atoi(getparm("compact"));
                        if(compact==0)
                                compact=1;

                        sprintf(path,"compact/%s/compact",board);
                        get_record(&p,sizeof(p),compact-1,path);
                        sprintf(dir,"compact/%s/%d/.DIR",board,p.dir);
                } else
                        sprintf(dir, "boards/%s/.DIR", board);
        total=file_size(dir)/sizeof(x);
        //huangxu@070517:link to deleted files
        if(total<=0)
                http_fatal("此讨论区不存在或者为空");
                if(!(fp=fopen(dir,"rb")))http_fatal("无法打开目录");
                   while(1) {
                       if(fread(&x, sizeof(x), 1, fp)<=0){
			   http_fatal("文章不存在!");
                           break;
                       }
                       if(!strcmp(x.filename, file)) {
                            break;
                       }
                   }
                   fcloseall();
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        printf("<table width=100%% bgcolor=%s border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table>",mytheme.con);
        shownav(board, total, dir, file, num, mode);
        printf("%s",bufnav);//huangxu@060402:一个很xx的错误……
        printf("<table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table><br><br>");
        printf("<table cellspacing=1 cellpadding=2 align=center width=500>");
        printf("<tr><td style='font-size:14px'>\n<pre>\n");
        if(iscompact)
                sprintf(filename,"compact/%s/%d/%s",board,p.dir,file);
        else
                sprintf(filename, "boards/%s/%s", board, file);
        /* add by wzk for delete article*/
        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("dir error2");
        if(num>0) {
                fseek(fp, sizeof(x)*(num), SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
        }

        //            if(num==0) istop(x,board,file);

        fclose(fp);

#ifdef MUST_EXISTING_USER
	if(!http_postReadable(&x))http_fatal("该用户未注册");
#endif


	/* end of change */
        fp=fopen(filename, "r");
        /* the next line change by wzk for delete article */
        if(fp==0 || x.owner[0]=='-')
                http_fatal("本文不存在或者已被删除");
        while(1) {
                char *id, *s;
                if(fgets(buf, 256, fp)==0)
                        break;
                if(!strncmp(buf, "发信人: ", 8)) {
                        ptr=strdup(buf);
                        id=strtok(ptr+8, " ");
                        s=strtok(0, "");
                        if(id==0)
                                id=" ";
                        if(s==0)
                                s="\n";
                        if(strlen(id)<13 && getuser(id)) {
                                printf("发信人: %s%s", userid_str(id), s);
                                free(ptr);
                                continue;
                        }
                        free(ptr);
                }
                if(!strncmp(buf, ": ", 2))
                        printf("<font color=808080>");
                hhprintf("%s", void1(buf));
                if(!strncmp(buf, ": ", 2))
                        printf("</font>");
        }
        fclose(fp);

        if(!strcmp(x.owner, currentuser.userid)&&!mode)
                printf("<p><center>[<a onclick='return confirm(\"确实切换本文的不可回复属性吗?\")' href=bbsnoreply?board=%s&file=%s>切换本文不可回复属性</a>]</center>", board, file);
        printf("\n</pre></td></tr></table><br><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table>");
        printf("%s",bufnav);
        printf("<table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table>");
        check_msg();//bluetent 2002.10.31
        http_quit();
}
char title_in_url[240];

int shownav(char *board, int total, char *dir, char *file, int num, int mode)
{
        FILE *fp;
        char *ptr, buf[256], str[256], title[80]; 
//	char title_in_url[240];
        char *buf2="&";
        struct fileheader x;
        int length, i=0;
        fp=fopen(dir, "r+");
        if(num>0 && num<=total) {
                fseek(fp, sizeof(x)*num, SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                brc_init(currentuser.userid, board);
                brc_add_read(x.filename);
                brc_update(currentuser.userid, board);
        }

        ptr=x.title;
        if(!strncmp(ptr, "Re: ", 4))
                ptr+=4;
        strcpy(title, ptr);

        title[strlen(ptr)]=0;
	kill_quotation_mark(title_in_url,title);
        sprintf(bufnav, "%s<table><tr><td>", bufnav);
        if(mode==0)
                sprintf(bufnav, "%s<font color=#009966>·</font><a href=\"bbspst?board=%s&file=%s&userid=%s&title=%s \">回复文章</a>", bufnav, board, file, x.owner, title_in_url);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsfwd?board=%s&file=%s>转寄/推荐</a>", bufnav, board, file);
        //if(mode==0)
        sprintf(bufnav, "%s<font color=#009966>·</font><a\
                href=\"bbspstmail?board=%s&file=%s&userid=%s&title=%s \">回复作者</a\
                >", bufnav, board, file, x.owner, title_in_url);
        //add by ibaif
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsccc?board=%s&file=%s>转贴</a>", bufnav, board, file);
        if(mode!=3)
                if(!strcasecmp(currentuser.userid, x.owner) || has_BM_perm(&currentuser, board)) {
                        sprintf(bufnav, "%s<font color=#009966>·</font><a onclick='return confirm(\"你真的要删除本文吗?\")' href=bbsdel?board=%s&file=%s>删除</a>", bufnav, board, file);
                        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsedit?board=%s&file=%s>修改</a>", bufnav, board, file);
                }
        if(fp==0)
                http_fatal("dir error2");

        if(num>0) {
                fseek(fp, sizeof(x)*(num-1), SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                if(mode)
                        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbscon?board=%s&file=%s&num=%d&mode=%d>上一篇</a>", bufnav, board, x.filename, num-1, mode);
                else
                        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbscon?board=%s&file=%s&num=%d>上一篇</a>", bufnav, board, x.filename, num-1);
        }
        switch(mode) {
        case 1:
                sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsgdoc?board=%s&start=%d>文摘区</a>", bufnav, board, num);
                break;
        case 2:
                sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsmdoc?board=%s&start=%d>管理模式</a>", bufnav, board, num);
                break;
        default:
                sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsdoc?board=%s&start=%d>讨论区</a>", bufnav, board, num);
        }
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=javascript:history.go(-1)>返回</a>", bufnav);
        if(num<total-1) {
                fseek(fp, sizeof(x)*(num+1), SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                if(mode)
                        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbscon?board=%s&file=%s&num=%d&mode=%d>下一篇</a>", bufnav, board, x.filename, num+1, mode);
                else
                        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbscon?board=%s&file=%s&num=%d>下一篇</a>", bufnav, board, x.filename, num+1);
        }
        fclose(fp);
        if(mode==0)
                sprintf(bufnav, "%s<font color=#009966>·</font><a href=\"bbstfind?board=%s&title=%s \">同主题阅读</a>", bufnav, board, title_in_url);
        if(mode==0)
                sprintf(bufnav, "%s<font color=#009966>·</font><a href=\"bbstshow?board=%s&title=%s \">同主题全部展开</a>", bufnav, board, title_in_url);
        sprintf(bufnav, "%s<font color=#009966>·</font>", bufnav);
        sprintf(bufnav, "%s</td></tr></table>", bufnav);
        return 1;
}

