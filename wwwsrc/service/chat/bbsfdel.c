/*$Id: bbsfdel.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        int i, total=0;
        char path[80], userid[80];
        struct override f[200];
        init_all();
        modify_mode(u_info,FRIEND+20000);	//bluetent
        if(!loginok)
                http_fatal("����δ��¼�����ȵ�¼");
        sprintf(path, "home/%c/%s/friends", toupper(currentuser.userid[0]), currentuser.userid);
        printf("<center>%s -- �������� [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        strsncpy(userid, getparm("userid"), 13);
        if(userid[0]==0) {
                printf("<form action=bbsfdel>\n");
                printf("��������ɾ���ĺ����ʺ�: <input type=text><br>\n");
                printf("<input type=submit>\n");
                printf("</form>");
                http_quit();
        }
        loadfriend(currentuser.userid);
        if(friendnum<=0)
                http_fatal("��û���趨�κκ���");
        if(!isfriend(userid))
                http_fatal("���˱����Ͳ�����ĺ���������");
        for(i=0; i<friendnum; i++) {
                if(strcasecmp(fff[i].id, userid)) {
                        memcpy(&f[total], &fff[i], sizeof(struct override));
                        total++;
                }
        }
        fp=fopen(path, "w");
        fwrite(f, sizeof(struct override), total, fp);
        fclose(fp);
        printf("[%s]�Ѵ����ĺ���������ɾ��.<br>\n <a href=bbsfall>���غ�������</a>", userid);
        http_quit();
}
