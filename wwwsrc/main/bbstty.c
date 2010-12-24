/*$Id: bbstty.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        char *cmd;
        char buf[256];
        char s1[256]="", s2[256]="", s3[356]="";
        int r;
        init_all();
        cmd=getparm("cmd");
        //	if(!loginok) http_fatal("匆匆过客不能使用此项功能");
        r=sscanf(cmd, "%80s %80s %80s", s1, s2, s3);
        if(!strcasecmp(s1, "/q")) {
                sprintf(buf, "bbsqry?userid=%s", s2);
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/s")) {
                sprintf(buf, "bbssendmsg?destid=%s&msg=%s", s2, s3);
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/m")) {
                sprintf(buf, "bbspstmail?userid=%s&title=%s", s2, s3);
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/logout")) {
                printf("<script>top.location='bbslogout'</script>");
                http_quit();
        }
        if(!strcasecmp(s1, "/top10")) {
                sprintf(buf, "bbstop10");
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/g")) {
                sprintf(buf, "bbssel?board=%s", s2);
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/u")) {
                sprintf(buf, "bbsusr");
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/f")) {
                sprintf(buf, "bbsfind");
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/d")) {
                if(s2[0]==0) {
                        sprintf(buf, "bbsdict");
                } else {
                        sprintf(buf, "bbsdict?type=1&word=%s", s2);
                }
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/c")) {
                sprintf(buf, "bbsclearall");
                redirect(buf);
                http_quit();
        }
        if(!strcasecmp(s1, "/h")) {
                printf("<pre>\n");
                printf("超级控制台命令<hr color=green>\n");
                printf("<table>");
                printf("<tr><td>/q [userid]</td><td>查询网友</td></tr>\n");
                printf("<tr><td>/s [userid] [msg]</td><td>发送讯息</td></tr>\n");
                printf("<tr><td>/m [userid] [title]</td><td>发送信件</td></tr>\n");
                printf("<tr><td>/logout</td><td>注销登录</td></tr>\n");
                printf("<tr><td>/top10</td><td>查看十大热门话题</td></tr>\n");
                printf("<tr><td>/g [board]</td><td>跳转到板面</td></tr>\n");
                printf("<tr><td>/u</td><td>环顾四方</td></tr>\n");
                printf("<tr><td>/f</td><td>查询文章</td></tr>\n");
                printf("<tr><td>/d [word]</td><td>查中英文字典</td></tr>\n");
                printf("<tr><td>/c</td><td>清除全站未读标记</td></tr>\n");
                printf("<tr><td>/h</td><td>显示本帮助界面</td></tr>\n");
                printf("</table>\n");
                http_quit();
        }
        //	http_fatal("未知命令");
        redirect("javascript:history.go(-1)");
        http_quit();
}
