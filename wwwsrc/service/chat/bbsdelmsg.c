/*$Id: bbsdelmsg.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        char path[80];
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ��ܴ���ѶϢ, ���ȵ�¼");
        sethomefile(path, currentuser.userid, "msgfile.me");
        unlink(path);
        printf("��ɾ������ѶϢ����");
}
