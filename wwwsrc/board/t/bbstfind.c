/*$Id: bbstfind.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char buf[1024], title[80], board[80], dir[80], first_file[80];
        struct shortfile *x1;
        struct fileheader x, x0;
        int sum=0, total=0;
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
                http_fatal ("您已经被取消进入本板的权限");
        sprintf(buf, "bbsman?board=%s&mode=1", board);
        sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("错误的讨论区目录");
        printf("<style type=text/css>table{font-size: 14px}</style>");
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        printf("<center>%s -- 同主题查找 [讨论区: %s] [主题 '%s']\n",
               BBSNAME, board, nohtml(title));
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)==0)
                        break;
                sum++;
                if(!strncmp(title, x.title, 40) ||
                                (!strncmp(title, x.title+4, 40) && !strncmp(x.title, "Re: ", 4))) {
                        if(total==0) {
                                strcpy(first_file, x.filename);
                                printf("<br><br><a href=bbsdoc?board=%s>本讨论区</a> ", board);
                                printf("<a href=bbstcon?board=%s&file=%s>本主题全部展开</a> ", board, first_file);
                                if(has_BM_perm(&currentuser, board))
                                        printf("<a onclick='return confirm(\"确定同主题全部删除?\")' href=%s>同主题删除</a>", buf);
                                printf("<hr color=green>");
                                printf("<br><table border=0><tr style='font-size:14px'><td>编号<td>作者<td>日期<td>标题\n");

                        }
                        printf("<tr style='font-size:14px'><td>%d", sum);
                        printf("<td>%s", userid_str(x.owner));
                        if(!(x.accessed[0]&(FILE_MARKED|FILE_DIGEST))) {
                                char buf2[20];
                                sprintf(buf2, "&box%s=on", x.filename);
                                if(strlen(buf)<500)
                                        strcat(buf, buf2);
                        }
                        printf("<td>%6.6s", Ctime(atoi(x.filename+2))+4);
                        printf("<td><a href=bbscon?board=%s&file=%s&num=%d>%s</a>\n",
                               board, x.filename, sum-1, void1(x.title));
                        total++;
                }
        }
        fclose(fp);
        if(total>0) {

                printf("</table>");
                printf("<br><hr color=green><a href=bbsdoc?board=%s>本讨论区</a> ", board);
                printf("<a href=bbstcon?board=%s&file=%s>本主题全部展开</a> ", board, first_file);
                if(has_BM_perm(&currentuser, board))
                        printf("<a onclick='return confirm(\"确定同主题全部删除?\")' href=%s>同主题删除</a>", buf);

        } else {

                printf("<br><hr color=green><a href=bbsdoc?board=%s>本讨论区</a> ", board);
                printf("&nbsp共找到 %d 篇 \n", total);
        }
        http_quit();
}
