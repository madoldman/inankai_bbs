/*$Id: bbsbanshow.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
struct deny
{
        char id[80];
        char exp[80];
        char free_time[80];
}
denyuser[256];
int denynum=0;

int loaddenyuser(char *board)
{
        FILE *fp;
        char path[80], buf[256];
        sprintf(path, "boards/%s/deny_users", board);
        fp=fopen(path, "r");
        if(fp==0)
                return;
        while(denynum<100) {
                if(fgets(buf, 80, fp)==0)
                        break;
                sscanf(buf, "%12s %40s %14s解封", denyuser[denynum].id, denyuser[denynum].exp, denyuser[denynum].free_time);
                denynum++;

        }
        fclose(fp);
}


int main()
{
        int i;
        char board[80];
        init_all();
        if(!loginok)
                http_fatal("您尚未登录, 请先登录");
        strsncpy(board, getparm("board"), 30);
        if(!has_read_perm(&currentuser, board))
                http_fatal("错误的讨论区");
        loaddenyuser(board);
        printf("<center>\n");
        printf("%s -- 被封用户名单 [讨论区: %s]<hr color=green><br>\n", BBSNAME, board);
        printf("本板共有 %d 人被封<br>", denynum);
        if(denynum>0)
                printf("<table border=0><tr class=title><td>序号<td>用户帐号<td>被封原因<td>原定解封日期\n");
        for(i=0; i<denynum; i++) {
                printf("<tr><td>%d", i+1);
                printf("<td><a href=bbsqry?userid=%s>%s</a>", denyuser[i].id, denyuser[i].id);
                printf("<td>%s\n", nohtml(denyuser[i].exp));
                printf("<td>%s\n", denyuser[i].free_time);
        }
        printf("</table><hr color=green>\n");
        printf("[<a href=javascript:history.go(-1)>返回</a>]</center>\n");
        http_quit();
}
