/*$Id: bbsmsg.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char buf[512], path[512];
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ����޷��鿴ѶϢ, ���ȵ�¼");
        sethomefile(path, currentuser.userid, "msgfile.me");
        fp=fopen(path, "r");
        if(fp==0)
                http_fatal("û���κ�ѶϢ");
        printf("<pre>\n");
        while(1) {
                if(fgets(buf, 256, fp)<=0)
                        break;
                hprintf("%s", buf);
        }
        fclose(fp);
        printf("<a onclick='return confirm(\"�����Ҫ�������ѶϢ��?\")' href=bbsdelmsg>�������ѶϢ</a> ");
        printf("<a href=bbsmailmsg>ѶϢ�Ļ���Ϣ</a>");
        printf("</pre>");
        http_quit();
}
