/*$Id: bbsmsg.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char buf[512], path[512];
        init_all();
        if(!loginok)
                http_fatal("匆匆过客无法查看讯息, 请先登录");
        sethomefile(path, currentuser.userid, "msgfile.me");
        fp=fopen(path, "r");
        if(fp==0)
                http_fatal("没有任何讯息");
        printf("<pre>\n");
        while(1) {
                if(fgets(buf, 256, fp)<=0)
                        break;
                hprintf("%s", buf);
        }
        fclose(fp);
        printf("<a onclick='return confirm(\"你真的要清除所有讯息吗?\")' href=bbsdelmsg>清除所有讯息</a> ");
        printf("<a href=bbsmailmsg>讯息寄回信息</a>");
        printf("</pre>");
        http_quit();
}
