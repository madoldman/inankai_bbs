/*$Id: bbsform.c,v 1.3 2008-12-30 09:45:49 madoldman Exp $
*/
#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        int type;
        init_all();
        type=atoi(getparm("type"));
        if(!loginok)
                http_fatal("����δ��¼, �����µ�¼��");
        printf("%s -- ��дע�ᵥ<hr color=green>\n", BBSNAME);
        check_if_ok();
        if(type==1) {
                check_submit_form();
                http_quit();
        }
        printf("����, %s, ע�ᵥͨ��ȷ�Ϻ�ſɻ�÷�������,�����ż���ѶϢ��Ȩ��, ������������������ʵ��д<br><hr>\n", currentuser.userid);
        printf("<form method=post action=bbsform?type=1>\n");
        printf("��ʵ����: <input name=realname type=text maxlength=80 size=8 value='%s'><br>\n",
               nohtml(currentuser.realname));
        printf("ѧУϵ��: <input name=dept type=text maxlength=320 size=32 value='%s'>(������λ)<br>\n",
               nohtml(currentuser.reginfo));
        printf("��ס��ַ: <input name=address type=text maxlength=320 size=32 value='%s'><br>\n",
               nohtml(currentuser.address));
        printf("����绰: <input name=phone type=text maxlength=320 size=32><br><hr><br>\n");
        printf("<input type=submit value=�����> <input type=reset>");
        http_quit();
}

int check_if_ok()
{
        if(user_perm(&currentuser, PERM_LOGINOK))
                http_fatal("���Ѿ�ͨ����վ�������֤, �����ٴ���дע�ᵥ.");
        if(has_fill_form())
                http_fatal("Ŀǰվ����δ��������ע�ᵥ�������ĵȴ�.");
}

int check_submit_form()
{
        FILE *fp;
        fp=fopen("new_register", "a");
        if(fp==0)
                http_fatal("ע���ļ�������֪ͨSYSOP");
        getdatestring(time(0),NA);
        fprintf(fp, "usernum: %d, %s\n", getusernum(currentuser.userid)+1, datestring);
        fprintf(fp, "userid: %s\n", currentuser.userid);
        fprintf(fp, "realname: %s\n", getparm("realname"));
        fprintf(fp, "dept: %s\n", getparm("dept"));
        fprintf(fp, "addr: %s\n", getparm("address"));
        fprintf(fp, "phone: %s\n", getparm("phone"));
        fprintf(fp, "regip: %s\n", fromhost);
        fprintf(fp, "----\n" );
        fclose(fp);
        printf("����ע�ᵥ�ѳɹ��ύ. վ�����������������, ��������������.");
}
