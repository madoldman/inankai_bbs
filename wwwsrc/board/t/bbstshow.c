/*$Id: bbstshow.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"
int main()
{
        FILE *fp;
        char buf[1024], title[80], board[80], dir[80], first_file[80], userid[16];
        struct shortfile *x1;
        struct fileheader x, x0;
        int sum=0, total=0, sumfirst=0;
        init_all();
        strsncpy(board, getparm("board"), 32);
        strsncpy(title, getparm("title"), 42);
        x1=getbcache(board);
        if(x1==0)
                http_fatal("错误的讨论区");
        strcpy(board, x1->filename);
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("错误的讨论区");
        if (!canenterboard (currentuser.userid, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal ("您已经被取消了进本板的权限");
        sprintf(buf, "bbsman?board=%s&mode=1", board);
        sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("错误的讨论区目录");
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        printf("<center>%s -- 同主题展开阅读 <a href=bbsdoc?board=%s>[讨论区: %s] </a>[主题 '%s']</center>\n",
               BBSNAME, board, board,nohtml(title));
        //		printf("<table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table>");
        //	printf("<table border=1><tr><td>编号<td>作者<td>日期<td>标题\n");
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)==0)
                        break;
                sum++;
                if(!strncmp(title, x.title, 40) ||
                                (!strncmp(title, x.title+4, 40) && !strncmp(x.title, "Re: ", 4))) {
                        if(total==0)
                                strcpy(first_file, x.filename);
                        strcpy(userid, x.owner);
                        /*			printf("<tr><td>%d", sum);
                        			printf("<td>%s", userid_str(x.owner));
                        			if(!(x.accessed[0]&(FILE_MARKED|FILE_DIGEST))) {
                        				char buf2[20];
                        				sprintf(buf2, "&box%s=on", x.filename);
                        				if(strlen(buf)<500) strcat(buf, buf2);
                        			}
                        			printf("<td>%6.6s", Ctime(atoi(x.filename+2))+4);
                        			printf("<td><a href=bbscon?board=%s&file=%s&num=%d>%s</a>\n", 
                        				board, x.filename, sum-1, void1(x.title));*/
                        if (userid[0]=='-')
                                continue;
                        else
                                show_file(board, &x, sum-1);
                        total++;
                        if(total==1)
                                sumfirst=sum-1;
                }
        }
        fclose(fp);
        printf("\n本主题共有文章 %d 篇 \n", total);
        printf("<a href=bbsdoc?board=%s&start=%d>本讨论区</a> ", board, sumfirst);
        if(total>0) {
                if(has_BM_perm(&currentuser, board))
                        printf("<a onclick='return confirm(\"确定同主题全部删除?\")' href=%s>同主题删除</a>", buf);
        }
        //printf("</center>");
        http_quit();
}
int show_file(char *board, struct fileheader *x, int n)
{
        FILE *fp;
        char path[80], buf[512];
        if(loginok)
                brc_add_read(x->filename);
        sprintf(path, "boards/%s/%s", board, x->filename);
        fp=fopen(path, "r");
        if(fp==0)
                return;
        printf("<table width=600  style='BORDER: 1px solid; BORDER-COLOR: 65c8aa;'  cellpadding=4 cellspacing=0  align=center><tr><td>\n");
        printf("[<a href=bbscon?board=%s&file=%s&num=%d>本篇全文</a>] ", board, x->filename, n);
        printf("[<a href='bbspst?board=%s&file=%s&title=%s&userid=%s'>回复本文</a>] ",
               board, x->filename, x->title, x->owner);
        printf("[本篇作者: %s] ", userid_str(x->owner));
        printf("<a href=bbsdoc?board=%s>本讨论区</a>",board);
        printf("</td></tr><tr><td style='font-size:14px'><pre>\n");
        while(1)
        {
                /*
                if(fgets(buf, 500, fp)==0) break;
                if(!strncmp(buf, ": ", 2)) continue;
                if(!strncmp(buf, "【 在 ", 4)) continue;
                //		if(!strncmp(buf, "--", 2)) break;
                hhprintf("%s", buf);
                 */
                if(fgets(buf, 500, fp)==0)
                        break;
                if(!strcmp(board,"Pictures") || !strcmp(board,"SelfPhoto") ) {
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
        printf("</table></pre>");
        //	printf("<table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table>");
        return 1;
}
