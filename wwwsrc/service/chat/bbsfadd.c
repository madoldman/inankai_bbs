/*$Id: bbsfadd.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char path[80], userid[80], exp[80];
        init_all();
        modify_mode(u_info,FRIEND+20000);	//bluetent
        if(!loginok)
                http_fatal("����δ��¼�����ȵ�¼");
        sprintf(path, "home/%c/%s/friends", toupper(currentuser.userid[0]), currentuser.userid);
        printf("<center>%s -- �������� [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        strsncpy(userid, getparm("userid"), 13);
        strsncpy(exp, getparm("exp"), 32);
        loadfriend(currentuser.userid);
        if(userid[0]==0) {
                if(userid[0])
                        printf("<font color=red>���������˵��</font>");
                printf("<form action=bbsfadd>\n");
                printf("������������ĺ����ʺ�: <input type=text name=userid value='%s'><br>\n",
                       userid);
                printf("�������������ѵ�˵��: <input type=text name=exp>\n",
                       exp);
                printf("<br><input type=submit value=ȷ��></form>\n");
                http_quit();
        }
        if(!getuser(userid))
                http_fatal("�����ʹ�����ʺ�");
        if(friendnum>=MAXFRIENDS - 1)
                http_fatal("���ĺ��������Ѵﵽ����, ��������µĺ���");
        if(isfriend(userid))
                http_fatal("�����Ѿ�����ĺ�����������");
        strcpy(fff[friendnum].id, getuser(userid)->userid);
        strcpy(fff[friendnum].exp, exp);
        friendnum++;
        fp=fopen(path, "w");
        fwrite(fff, sizeof(struct override), friendnum, fp);
        fclose(fp);
        printf("[%s]�Ѽ������ĺ�������.<br>\n <a href=bbsfall>���غ�������</a>", userid);
        http_quit();
}
