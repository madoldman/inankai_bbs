/*$Id: bbsmailmsg.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        char filename[80];
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ��ܴ���ѶϢ�����ȵ�¼");
        sprintf(filename, "home/%c/%s/msgfile.me", toupper(currentuser.userid[0]), currentuser.userid);
        post_mail(currentuser.userid, "����ѶϢ����", filename, currentuser.userid, currentuser.username, fromhost, -1,0);
        unlink(filename);
        printf("ѶϢ�����Ѿ��Ļ���������");
        printf("<a href='javascript:history.go(-2)'>����</a>");
        http_quit();
}
