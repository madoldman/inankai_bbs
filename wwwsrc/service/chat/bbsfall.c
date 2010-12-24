/*$Id: bbsfall.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        int i;
        init_all();
        modify_mode(u_info,FRIEND+20000);	//bluetent
        if(!loginok)
                http_fatal("您尚未登录, 请先登录");
        loadfriend(currentuser.userid);
        printf("<center>\n");
        printf("%s -- 好友名单 [使用者: %s]<hr color=green><br>\n", BBSNAME, currentuser.userid);
        printf("您共设定了 %d 位好友<br>", friendnum);
        printf("<table border=1><tr><td>序号<td>好友代号<td>好友说明<td>删除好友");
        for(i=0; i<friendnum; i++) {
                printf("<tr><td>%d", i+1);
                printf("<td><a href=bbsqry?userid=%s>%s</a>", fff[i].id, fff[i].id);
                printf("<td>%s\n", nohtml(fff[i].exp));
                printf("<td>[<a onclick='return confirm(\"确实删除吗?\")' href=bbsfdel?userid=%s>删除</a>]", fff[i].id);
        }
        printf("</table><hr color=green>\n");
        printf("[<a href=bbsfadd>添加新的好友</a>]</center>\n");
        //	for(i=0;i<friendnum2;i++)
        //		printf("%d ", u_info->friend[i]);
        http_quit();
}
