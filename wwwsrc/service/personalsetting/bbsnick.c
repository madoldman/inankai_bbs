/*$Id: bbsnick.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        int i;
        unsigned char nick[80];
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ����޷��ı��ǳ�");
        strsncpy(nick, getparm("nick"), 30);
        if(nick[0]==0) {
                printf("%s -- ��ʱ�ı��ǳ�(�����ķ���Ч) [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
                printf("<form action=bbsnick>���ǳ�<input name=nick size=24 maxlength=24 type=text value='%s'> \n",
                       u_info->username);
                printf("<input type=submit value=ȷ��>");
                printf("</form>");
                http_quit();
        }
        for(i=0; nick[i]; i++)
                if(nick[i]<32 || nick[i]==255)
                        nick[i]=' ';
        strsncpy(u_info->username, nick, 32);
        printf("��ʱ����ǳƳɹ�");
}
