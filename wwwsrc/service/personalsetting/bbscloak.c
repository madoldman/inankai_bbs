/*$Id: bbscloak.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ��ܽ��д˲���, ���ȵ�¼");
        if(!(currentuser.userlevel & PERM_CLOAK))
                http_fatal("����Ĳ���");
        if(u_info->invisible) {
                u_info->invisible=0;
                printf("����״̬�Ѿ�ֹͣ��.");
        } else {
                u_info->invisible=1;
                printf("����״̬�Ѿ���ʼ��.");
        }
}
