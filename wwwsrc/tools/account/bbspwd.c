/*$Id: bbspwd.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        int type;
        char pw1[20], pw2[20], pw3[20];
        init_all();
        if(!loginok)
                http_fatal("����δ��¼, ���ȵ�¼");
        modify_mode(u_info,USERDEF+20000);	//bluetent
        type=atoi(getparm("type"));
        if(type==0) {
                printf("%s -- �޸����� [�û�: %s]<hr color=green>\n",
                       BBSNAME, currentuser.userid);
                printf("<form action=bbspwd?type=1 method=post>\n");
                printf("��ľ�����: <input maxlength=12 size=12 type=password name=pw1><br>\n");
                printf("���������: <input maxlength=12 size=12 type=password name=pw2><br>\n");
                printf("������һ��: <input maxlength=12 size=12 type=password name=pw3><br><br>\n");
                printf("<input type=submit value=ȷ���޸�>\n");
                http_quit();
        }
        strsncpy(pw1, getparm("pw1"), 13);
        strsncpy(pw2, getparm("pw2"), 13);
        strsncpy(pw3, getparm("pw3"), 13);
        if(strcmp(pw2, pw3))
                http_fatal("�������벻��ͬ");
        if(strlen(pw2)<2)
                http_fatal("������̫��");
        if(!checkpasswd(currentuser.passwd, pw1))
                http_fatal("���벻��ȷ");
        strcpy(currentuser.passwd, crypt1(pw2, pw2));
        save_user_data(&currentuser);
        printf("[%s] �����޸ĳɹ�.", currentuser.userid);
}

