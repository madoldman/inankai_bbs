/*$Id: bbsdenyadd.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
struct deny
{
        char id[80];
        char exp[80];
        //	int free_time;
        char free_time[80];
}
denyuser[256];
int denynum=0;
int loaddenyuser(char *board)
{
        FILE *fp;
        char path[80], buf[256];
        sprintf(path, "boards/%s/deny_users", board);
        fp=fopen(path, "r");
        if(fp==0)
                return;
        while(denynum<100) {
                if(fgets(buf, 80, fp)==0)
                        break;
                sscanf(buf, "%-12s %-40s %14.14s���", denyuser[denynum].id, denyuser[denynum].exp, denyuser[denynum].free_time);
                denynum++;
        }
        fclose(fp);
}

int savedenyuser(char *board)
{
        FILE *fp;
        int i;
        char path[80], buf[256], *exp;
        sprintf(path, "boards/%s/deny_users", board);
        fp=fopen(path, "a");
        if(fp==0)
                return;
        for(i=0; i<denynum; i++) {
                int m;
                exp=denyuser[i].exp;
                if(denyuser[i].id[0]==0)
                        continue;
                for(m=0; exp[m]; m++) {
                        if(exp[m]<=32 && exp[m]>0)
                                exp[m]='.';
                }
                fprintf(fp, "%-12s %-40s %14.14s���\n", denyuser[i].id, denyuser[i].exp, denyuser[i].free_time);
        }
        fclose(fp);
}

int main()
{
        int i;
        char exp[80], board[80], *userid;
        int dt;
        init_all();
        if(!loginok)
                http_fatal("����δ��¼, ���ȵ�¼");
        strsncpy(board, getparm("board"), 30);
        strsncpy(exp, getparm("exp"), 30);
        dt=atoi(getparm("dt"));
        if(!has_read_perm(&currentuser, board))
                http_fatal("�����������");
        if(!has_BM_perm(&currentuser, board))
                http_fatal("����Ȩ���б�����");
        loaddenyuser(board);
        userid=getparm("userid");
        if(userid[0]==0)
                return show_form(board);
        if(getuser(userid)==0)
                http_fatal("�����ʹ�����ʺ�");
        strcpy(userid, getuser(userid)->userid);
        if(dt<1 || dt>31)
                http_fatal("�����뱻������(1-31)");
        if(exp[0]==0)
                http_fatal("���������ԭ��");
        for(i=0; i<denynum; i++)
                if(!strcasecmp(denyuser[i].id, userid))
                        http_fatal("���û��Ѿ�����");
        if(denynum>40)
                http_fatal("̫���˱�����");
        strsncpy(denyuser[denynum].id, userid, 13);
        strsncpy(denyuser[denynum].exp, exp, 30);
        getdatestring(time(0)+dt*86400,NA);
        //	denyuser[denynum].free_time=time(0)+dt*86400;
        strcpy(denyuser[denynum].free_time, datestring);
        denynum++;
        savedenyuser(board);
        printf("��� %s �ɹ�<br>\n", userid);
        inform_add(board, userid, exp, dt);
        printf("[<a href=bbsdenyall?board=%s>���ر����ʺ�����</a>]", board);
        http_quit();
}

int show_form(char *board)
{
        printf("<center>%s -- ������� [������: %s]<hr color=green>\n", BBSNAME, board);
        printf("<form action=bbsdenyadd><input type=hidden name=board value='%s'>", board);
        printf("���ʹ����<input name=userid size=12> ����POSTȨ <input name=dt size=2> ��, ԭ��<input name=exp size=20>\n");
        printf("<input type=submit value=ȷ��></form>");
}

int inform_add(char *board, char *user, char *exp, int dt)
{
        char msg[512];
        int info_type=1;
        FILE *fp;
        char path[80], title[80];
        sprintf(title, "%s��ȡ����%s��ķ���Ȩ��", user, board);
        sprintf(path, "tmp/%d.tmp", getpid());
        fp=fopen(path, "w");
        getdatestring(time(0)+dt*86400,NA);
        sprintf(msg,
                "\n  %s ����: \n\n"
                "\t�Һܱ�Ǹ�ظ����������Ѿ���ȡ���� %s ��ġ�%s��Ȩ����\n\n"
                "\t���������ԭ���ǣ� [%s]\n\n",
                user, board, info_type==1?"����":"����",exp);
        sprintf(msg,
                "%s\t���ھ���ֹͣ���ڱ����Ȩ�� [%d] ��.\n\n"
                "\t[%d] ���,�����ڱ��巢��ʱϵͳ���Զ����.\n\n",
                msg,dt,dt);
        sprintf(msg,
                "%s\t��������,����%s��ϵ���,����ֱ�ӵ�0��Appeal������йظ�ʽ����Ͷ��.\n\n"
                "\t\t\t\t\t%s �� %s\n",
                msg,currentuser.userid,board,currentuser.userid);
        fprintf(fp, msg);
        fclose(fp);
        post_article(board, title, path, BBSID, "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1);
        sprintf(path, "tmp/%d.tmp", getpid());
        fp=fopen(path, "w");
        fprintf(fp, msg);
        fclose(fp);
        post_article("Punishment", title, path, currentuser.userid, currentuser.username, "�Զ�����ϵͳ", -1);
        post_mail(user, title, path, currentuser.userid, currentuser.username, "�Զ�����ϵͳ", -1,0);
        unlink(path);
        fp=fopen(path, "w");
        fprintf(fp,"ԭ����:%s\n",exp);
        getuinfo(fp);
        fclose(fp);
        post_article("syssecurity3", title, path, currentuser.userid, "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1);
        unlink(path);
        printf("ϵͳ�Ѿ�����֪ͨ��%s.<br>\n", user);
}
