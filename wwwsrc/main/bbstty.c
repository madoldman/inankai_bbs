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
        //	if(!loginok) http_fatal("�Ҵҹ��Ͳ���ʹ�ô����");
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
                printf("��������̨����<hr color=green>\n");
                printf("<table>");
                printf("<tr><td>/q [userid]</td><td>��ѯ����</td></tr>\n");
                printf("<tr><td>/s [userid] [msg]</td><td>����ѶϢ</td></tr>\n");
                printf("<tr><td>/m [userid] [title]</td><td>�����ż�</td></tr>\n");
                printf("<tr><td>/logout</td><td>ע����¼</td></tr>\n");
                printf("<tr><td>/top10</td><td>�鿴ʮ�����Ż���</td></tr>\n");
                printf("<tr><td>/g [board]</td><td>��ת������</td></tr>\n");
                printf("<tr><td>/u</td><td>�����ķ�</td></tr>\n");
                printf("<tr><td>/f</td><td>��ѯ����</td></tr>\n");
                printf("<tr><td>/d [word]</td><td>����Ӣ���ֵ�</td></tr>\n");
                printf("<tr><td>/c</td><td>���ȫվδ�����</td></tr>\n");
                printf("<tr><td>/h</td><td>��ʾ����������</td></tr>\n");
                printf("</table>\n");
                http_quit();
        }
        //	http_fatal("δ֪����");
        redirect("javascript:history.go(-1)");
        http_quit();
}
