/*$Id: bbsdenydel.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
struct deny
{
        char id[80];
        char exp[80];
        //	int free_time;
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
                //		sscanf(buf, "%-12s %-40s %-14s解封", denyuser[denynum].id, denyuser[denynum].exp, denyuser[denynum].free_time);
                //		sscanf(buf, "%-14s解封", denyuser[denynum].free_time);
                sscanf(buf, "%12s %40s %14s解封", denyuser[denynum].id, denyuser[denynum].exp, denyuser[denynum].free_time);
                denynum++;

        }
        fclose(fp);
}

int savedenyuser(char *board)
{
        FILE *fp;
        int i;
        char path[80], buf[256], *exp;
        sprintf(path, "boards/%s/deny_users", board);
        fp=fopen(path, "w");
        if(fp==0)
                return;
        for(i=0; i<denynum; i++) {
                int m;
                exp=denyuser[i].exp;
                if(denyuser[i].id[0]==0)
                        continue;
                for(m=0; exp[m]; m++) {
                        if(exp[m]<=32 && exp[m]>0)
                                exp[m]='.';
                }
                fprintf(fp, "%-12s %-40s %14.14s解封\n", denyuser[i].id, denyuser[i].exp, denyuser[i].free_time);
        }
        fclose(fp);
}

int main()
{
        int i;
        char board[80], *userid;
        init_all();
        if(!loginok)
                http_fatal("您尚未登录, 请先登录");
        strsncpy(board, getparm("board"), 30);
        if(!has_read_perm(&currentuser, board))
                http_fatal("错误的讨论区");
        if(!has_BM_perm(&currentuser, board))
                http_fatal("你无权进行本操作");
        loaddenyuser(board);
        userid=getparm("userid");
        for(i=0; i<denynum; i++) {
                if(!strcasecmp(denyuser[i].id, userid)) {
                        denyuser[i].id[0]=0;
                        savedenyuser(board);
                        printf("已经给 %s 解封. <br>\n", userid);
                        inform(board, userid);

                        printf("[<a href=bbsdenyall?board=%s>返回被封名单</a>]", board);
                        http_quit();
                }
        }
        http_fatal("这个用户不在被封名单中");
        http_quit();
}
