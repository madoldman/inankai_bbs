/*$Id: bbsfdel.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        int i, total=0;
        char path[80], userid[80];
        struct override f[200];
        init_all();
        modify_mode(u_info,FRIEND+20000);	//bluetent
        if(!loginok)
                http_fatal("您尚未登录，请先登录");
        sprintf(path, "home/%c/%s/friends", toupper(currentuser.userid[0]), currentuser.userid);
        printf("<center>%s -- 好友名单 [使用者: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        strsncpy(userid, getparm("userid"), 13);
        if(userid[0]==0) {
                printf("<form action=bbsfdel>\n");
                printf("请输入欲删除的好友帐号: <input type=text><br>\n");
                printf("<input type=submit>\n");
                printf("</form>");
                http_quit();
        }
        loadfriend(currentuser.userid);
        if(friendnum<=0)
                http_fatal("您没有设定任何好友");
        if(!isfriend(userid))
                http_fatal("此人本来就不在你的好友名单里");
        for(i=0; i<friendnum; i++) {
                if(strcasecmp(fff[i].id, userid)) {
                        memcpy(&f[total], &fff[i], sizeof(struct override));
                        total++;
                }
        }
        fp=fopen(path, "w");
        fwrite(f, sizeof(struct override), total, fp);
        fclose(fp);
        printf("[%s]已从您的好友名单中删除.<br>\n <a href=bbsfall>返回好友名单</a>", userid);
        http_quit();
}
