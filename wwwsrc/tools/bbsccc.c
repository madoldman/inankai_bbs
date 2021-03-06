/*$Id: bbsccc.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        struct fileheader *x;
        char board[80], *board1, file[80], target[80], boardsearch[80];
        int i,total=0;
        init_all();
        strsncpy(board, getparm("board"), 30);
        strsncpy(file, getparm("file"), 30);
        strsncpy(target, getparm("target"), 30);
        if(!loginok)
	{
                http_fatal("匆匆过客不能进行本项操作");
	}
	if(strcmp(board,"Announce")==0&&!HAS_PERM(PERM_SYSOP)&&!HAS_PERM(PERM_ADMINMENU))
	{
		http_fatal("非站务人员不能从Announce板转载文章");
	}
        if(strcmp(target,"")) {
                for(i=0; i<MAXBOARD; i++) {
                        board1=shm_bcache->bcache[i].filename;
                        if(!has_read_perm(&currentuser, board1))
                                continue;
                        if(!strcasecmp(board1, target)) {
                                total++;
                                strcpy(boardsearch,board1);
                        }
                }
                if (total!=1)
                        http_fatal("错误的讨论区");
                else
                        strcpy (target,boardsearch);
        }
        if(!has_read_perm(&currentuser, board)) {
                printf("%s",board);
                http_fatal("错误的讨论区2");
        }

        x=get_file_ent(board, file);
        if(x==0)
                http_fatal("错误的文件名");

        if(check_post_limit(board)) return 0;

        printf("<center>%s -- 转载文章 [使用者: %s]<hr color=green></center>\n", BBSNAME, currentuser.userid);
        if(target[0]) {
                if(!has_post_perm(&currentuser, target))
                        http_fatal("错误的讨论区名称或你没有在该版发文的权限");
                if(fopen("NOPOST","r")!=NULL&&!HAS_PERM(PERM_OBOARDS))
                        http_fatal("对不起，系统进入只读状态，暂停转载文章");

                return do_ccc(x, board, target);
        }
        printf("<table><tr><td>\n");
        printf("<font color=red>转贴发文注意事项:<br>\n");
        printf("本站规定同样内容的文章严禁在 5 个以上讨论区内重复发表。");
        printf("违者将被封禁在本站发文的权利<br><br></font>\n");
        printf("文章标题: %s<br>\n", nohtml(x->title));
        printf("文章作者: %s<br>\n", x->owner);
        printf("原讨论区: %s<br>\n", board);
        printf("<form action=bbsccc method=post>\n");
        printf("<input type=hidden name=board value=%s>", board);
        printf("<input type=hidden name=file value=%s>", file);
        printf("转载到 <input name=target size=30 maxlength=30> 讨论区. ");
        printf("<input type=submit value=确定></form></td></tr></table>");
}

int do_ccc(struct fileheader *x, char *board, char *board2)
{
        FILE *fp, *fp2;
        char title[512], buf[512], path[200], path2[200], i;
        sprintf(path, "boards/%s/%s", board, x->filename);
        fp=fopen(path, "r");
        if(fp==0)
                http_fatal("文件内容已丢失, 无法转载");
        sprintf(path2, "tmp/%d.tmp", getpid());
        fp2=fopen(path2, "w");
        fprintf(fp2, "\n[37;1m【 以下文字转载自 [32m%s [37m讨论区 】\n", board);
        while(1)
        {
                if(fgets(buf, 256, fp)==0)
                        break;
                fprintf(fp2, "%s", buf);
        }
        fclose(fp);
        fclose(fp2);
        if(!strncmp(x->title, "[转载]", 6))
        {
                sprintf(title, x->title);
        } else
        {
                sprintf(title, "%.55s(转载)", x->title);
        }
        post_article(board2, title, path2, currentuser.userid, currentuser.username, fromhost, -1);
        unlink(path2);
        printf("'%s' 已转贴到 %s 板.<br>\n", nohtml(title), board2);
        printf("<a href='javascript:history.go(-2)'>       返回        </a>");
}
