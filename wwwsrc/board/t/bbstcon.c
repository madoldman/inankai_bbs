/*$Id: bbstcon.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"

int no_re=0;
/*	bbscon?board=xx&file=xx&start=xx 	*/

int main()
{
        FILE *fp;
        char title[256], userid[80], buf[512], board[80], dir[80], file[80], filename[80], *ptr;
        struct fileheader x;
        int i, num=0, found=0;
        init_all();
        modify_mode(u_info,READING);	//bluetent
        strsncpy(board, getparm("board"), 32);
        strsncpy(file, getparm("file"), 32);
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        printf("<center>\n");
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("错误的讨论区");
        if (!canenterboard (currentuser.userid, board))
                http_fatal ("您已经被取消进入本板的权限");
        strcpy(board, getbcache(board)->filename);
        if(loginok)
                brc_init(currentuser.userid, board);
        printf("<center>%s -- 同主题阅读 [讨论区: <a href=bbsdoc?board=%s>%s</a>]\n",
               BBSNAME, board,board);
        printf("<tr><td></table></center>");
        if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                http_fatal("错误的参数1");
        if(strstr(file, "..") || strstr(file, "/"))
                http_fatal("错误的参数2");
        sprintf(dir, "boards/%s/.DIR", board);
        if(!strcmp(board, "noticeboard"))
                no_re=1;
        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("目录错误");
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                num++;
                if(!strcmp(x.filename, file)) {
                        ptr=x.title;
                        if(!strncmp(ptr, "Re:", 3))
                                ptr+=4;
                        strsncpy(title, ptr, 40);
                        //			(*(int*)(x.title+72))++;
                        //			fseek(fp, -1*sizeof(x), SEEK_CUR);
                        //			fwrite(&x, sizeof(x), 1, fp);
                        fflush(fp);        //add by wzk
                        found=1;
                        strcpy(userid, x.owner);
                        /* change by wzk for delete article */
                        if (userid[0]=='-')
                                printf("文章内容已被删除<br>\n");
                        else if (!postReadable(&x))
			{
				//printf("用户未注册，无法访问");
			}
			else
                                /* end of change */
                                show_file(board, &x, num-1);
                        while(1) {
                                if(fread(&x, sizeof(x), 1, fp)<=0)
                                        break;
                                num++;
                                if(!strncmp(x.title+4, title, 39) && !strncmp(x.title, "Re: ", 4))
                                        /* change by wzk for delete article */
                                        if (userid[0]=='-')
                                                printf("文章内容已被删除<br>\n");
                                        else
                                                /* end of change */
                                                show_file(board, &x, num-1);
                        }
                }
        }
        fclose(fp);
        if(found==0)
                http_fatal("错误的文件名");
        /* the next change by wzk for delete article */
        /* mode=1问题,莫名的问号
        if(!no_re && userid[0]!='-'){
                 printf("[<a href=bbspst?board=%s&file=%s&userid=%s&mode=1&title=\"%s\">回复文章</a>] ",
           board, file, userid, title);
        }*/
        printf("<a href='javascript:history.go(-1)'>返回上一页</a>");
        ptr=x.title;
        if(!strncmp(ptr, "Re: ", 4))
                ptr+=4;
        //printf("</center>\n");
        if(loginok)
                brc_update(currentuser.userid, board);
        check_msg();//bluetent 2002.10.31
        http_quit();
}
char title_in_url[200];
int show_file(char *board, struct fileheader *x, int n)
{
#ifdef MUST_EXISTING_USER
	if (!postReadable(x))
	{
		printf ("Not readable.");
		return 0;
	}
#endif

        FILE *fp;
        char path[80], buf[512];
        if(loginok)
                brc_add_read(x->filename);
        sprintf(path, "boards/%s/%s", board, x->filename);
        fp=fopen(path, "r");
        if(fp==0)
                return;

	kill_quotation_mark(title_in_url,x->title);
        printf("<table width=600 style='BORDER: 1px solid; BORDER-COLOR: 65c8aa;'  cellpadding=4 cellspacing=0 ><tr><td>\n");
        printf("[<a href=bbscon?board=%s&file=%s&num=%d>查看全文</a>] ", board, x->filename, n);
        printf("[<a href='bbspst?board=%s&file=%s&title=%s&userid=%s'>回复本文</a>] ",
               board, x->filename, title_in_url, x->owner);
        //add by ibaif@Taida
        printf("[<a href='bbspstmail?board=%s&file=%s&title=%s&userid=%s'>回复作者</a>] ",
               board, x->filename, title_in_url, x->owner);
        //add end
        printf("[作者: %s] ", userid_str(x->owner));
        printf("[讨论区:<a href=bbsdoc?board=%s>%s</a>]",board,board);
        printf("</td></tr><tr><td style='font-size:14px'><pre>\n");
        while(1)
        {
                if(fgets(buf, 500, fp)==0)
                        break;
                if(!strcmp(board,"Pictures") || !strcmp(board,"SelfPhoto")) {
                        if(!strncmp(buf, ": ", 2))
                                continue;
                        if(!strncmp(buf, "【 在 ", 4))
                                continue;
                        if(!strncmp(buf, "--", 2))
                                break;
                        hhprintf("%s", buf);
                        continue;
                }
                if(!strncmp(buf, ": ", 2))
                        printf("<font color=808080>");
                hhprintf("%s", buf);
                if(!strncmp(buf, ": ", 2))
                        printf("</font>");
        }
        fclose(fp);
        printf("</pre></table>");
        //	printf("<table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table>");
        return 1;
}
