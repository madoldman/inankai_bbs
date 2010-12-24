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
                sscanf(buf, "%-12s %-40s %14.14s解封", denyuser[denynum].id, denyuser[denynum].exp, denyuser[denynum].free_time);
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
                fprintf(fp, "%-12s %-40s %14.14s解封\n", denyuser[i].id, denyuser[i].exp, denyuser[i].free_time);
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
                http_fatal("您尚未登录, 请先登录");
        strsncpy(board, getparm("board"), 30);
        strsncpy(exp, getparm("exp"), 30);
        dt=atoi(getparm("dt"));
        if(!has_read_perm(&currentuser, board))
                http_fatal("错误的讨论区");
        if(!has_BM_perm(&currentuser, board))
                http_fatal("你无权进行本操作");
        loaddenyuser(board);
        userid=getparm("userid");
        if(userid[0]==0)
                return show_form(board);
        if(getuser(userid)==0)
                http_fatal("错误的使用者帐号");
        strcpy(userid, getuser(userid)->userid);
        if(dt<1 || dt>31)
                http_fatal("请输入被封天数(1-31)");
        if(exp[0]==0)
                http_fatal("请输入封人原因");
        for(i=0; i<denynum; i++)
                if(!strcasecmp(denyuser[i].id, userid))
                        http_fatal("此用户已经被封");
        if(denynum>40)
                http_fatal("太多人被封了");
        strsncpy(denyuser[denynum].id, userid, 13);
        strsncpy(denyuser[denynum].exp, exp, 30);
        getdatestring(time(0)+dt*86400,NA);
        //	denyuser[denynum].free_time=time(0)+dt*86400;
        strcpy(denyuser[denynum].free_time, datestring);
        denynum++;
        savedenyuser(board);
        printf("封禁 %s 成功<br>\n", userid);
        inform_add(board, userid, exp, dt);
        printf("[<a href=bbsdenyall?board=%s>返回被封帐号名单</a>]", board);
        http_quit();
}

int show_form(char *board)
{
        printf("<center>%s -- 版务管理 [讨论区: %s]<hr color=green>\n", BBSNAME, board);
        printf("<form action=bbsdenyadd><input type=hidden name=board value='%s'>", board);
        printf("封禁使用者<input name=userid size=12> 本板POST权 <input name=dt size=2> 天, 原因<input name=exp size=20>\n");
        printf("<input type=submit value=确认></form>");
}

int inform_add(char *board, char *user, char *exp, int dt)
{
        char msg[512];
        int info_type=1;
        FILE *fp;
        char path[80], title[80];
        sprintf(title, "%s被取消在%s板的发文权限", user, board);
        sprintf(path, "tmp/%d.tmp", getpid());
        fp=fopen(path, "w");
        getdatestring(time(0)+dt*86400,NA);
        sprintf(msg,
                "\n  %s 网友: \n\n"
                "\t我很抱歉地告诉您，您已经被取消在 %s 板的『%s』权力。\n\n"
                "\t您被封禁的原因是： [%s]\n\n",
                user, board, info_type==1?"发文":"进入",exp);
        sprintf(msg,
                "%s\t现在决定停止你在本板的权利 [%d] 天.\n\n"
                "\t[%d] 天后,当您在本板发文时系统将自动解封.\n\n",
                msg,dt,dt);
        sprintf(msg,
                "%s\t如有异议,请与%s联系解决,或者直接到0区Appeal板参照有关格式进行投诉.\n\n"
                "\t\t\t\t\t%s 板 %s\n",
                msg,currentuser.userid,board,currentuser.userid);
        fprintf(fp, msg);
        fclose(fp);
        post_article(board, title, path, BBSID, "自动发信系统", "自动发信系统", -1);
        sprintf(path, "tmp/%d.tmp", getpid());
        fp=fopen(path, "w");
        fprintf(fp, msg);
        fclose(fp);
        post_article("Punishment", title, path, currentuser.userid, currentuser.username, "自动发信系统", -1);
        post_mail(user, title, path, currentuser.userid, currentuser.username, "自动发信系统", -1,0);
        unlink(path);
        fp=fopen(path, "w");
        fprintf(fp,"原因是:%s\n",exp);
        getuinfo(fp);
        fclose(fp);
        post_article("syssecurity3", title, path, currentuser.userid, "自动发信系统", "自动发信系统", -1);
        unlink(path);
        printf("系统已经发信通知了%s.<br>\n", user);
}
