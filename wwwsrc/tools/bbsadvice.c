/*$Id: bbsadvice.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ����ύ������飬���ȵ�¼");
        printf("<center>��ϵͳ���ܵ�����뽨��\n");
        printf("<table border=0><tr><td>\n");
        printf("<form name=advice method=post action=bbssndmail?userid=BBSADM&signature=0&mode=1?title=��ϵͳ���ܵ�����뽨��>\n");
        printf("<br>\n");
        printf("<textarea name=text rows=8 cols=40 wrap=physicle>\n\n");
        printf("</textarea><br><div align=center>\n");
        printf("<input type=submit value=�ύ> ");
        printf("<input type=button value=�رձ����� onclick=window.close()>");
        printf("</form>\n");
        printf("</div></table>");
        printf("<script>document.advice.text.focus();</script>");
        http_quit();
}
