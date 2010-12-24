/*$Id: chatlogin.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        int pid, n, t;
        char buf[256], id[20], pw[20];
        struct userec *x;
        FILE *fp;
        init_all();
        strsncpy(id, getparm("id"), 13);
        strsncpy(pw, getparm("pw"), 13);
        if(loginok && strcasecmp(id, currentuser.userid)) {
                http_fatal("系统检测到目前你的计算机上已经登录有一个帐号 %s，请先退出.(%s)",
                           currentuser.userid, "选择正常logout, 或者关闭所有浏览器窗口");
        }
        x=getuser(id);
        if(x==0)
                http_fatal("错误的使用者帐号");
        if(strcasecmp(id, "guest")) {
                if(!checkpasswd(x->passwd, pw)) {
                        if(pw[0]!=0)
                                sleep(2);
                        sprintf(buf, "%s %s %s\n", Ctime(time(0))+4, id, fromhost);
                        f_append("badlogin.www", buf);
                        http_fatal("密码错误");
                }
                if(!user_perm(x, PERM_BASIC))
                        http_fatal("此帐号已被停机, 若有疑问, 请用其他帐号在sysop版询问.");
                if(file_has_word(".bansite", fromhost)) {
                        http_fatal("对不起, 本站不欢迎来自 [%s] 的登录. <br>若有疑问, 请与SYSOP联系.", fromhost);
                }
                t=x->lastlogin;
                x->lastlogin=time(0);
                save_user_data(x);
                if(abs(t-time(0))<5)
                        http_fatal("两次登录间隔过密!");
                x->numlogins++;
                strsncpy(x->lasthost, fromhost, 17);
                save_user_data(x);
                currentuser=*x;
        }
        sprintf(buf, "%s %s %s\n", Ctime(time(0)), x->userid, fromhost);
        f_append("www.log", buf);
        sprintf(buf, "%s ENTER %-12s @%s [www]\n", Ctime(time(0))+4, x->userid, fromhost);
        f_append("usies", buf);
        n=0;
        if(!loginok && strcasecmp(id, "guest"))
        {
        int iutmpnum, iutmpkey;
                if (!wwwlogin(x, &iutmpnum, &iutmpkey))//0 : succeed
                {
                        sprintf(buf, "%d", iutmpnum);
                        setcookie("utmpnum", buf);
                        sprintf(buf, "%d", iutmpkey);
                        setcookie("utmpkey", buf);
                        setcookie("utmpuserid", currentuser.userid);
                        set_my_cookie();
                }
                else
                {
                	http_fatal("抱歉，登录人数太多，请稍候再来：（");
                }
        }
        redirect("bbschat");
}

void add_msg()
{
        int i;
        FILE *fp;
        char buf[129], file[256], *id=currentuser.userid;
        sprintf(file, "touch home/%c/%s/wwwmsg.flush", toupper(id[0]), id);
        system(file);
        sprintf(file, "home/%c/%s/msgfile", toupper(id[0]), id);
        i=file_size(file)/129;
        if(get_record(&buf, 129, i-1, file)<=0)
                return;
        sprintf(file, "home/%c/%s/wwwmsg", toupper(id[0]), id);
        fp=fopen(file, "a");
        fwrite(buf, 129, 1, fp);
        fclose(fp);
}

void abort_program()
{
        int stay;
        struct userec *x;
        f_append("err", "ok");
        if(strcmp(u_info->userid, currentuser.userid))
                bzero(u_info, sizeof(struct user_info));
        stay=abs(time(0) - *(int*)(u_info->from+30));
        if(stay>7200)
                stay=7200;
        x=getuser(currentuser.userid);
        if(x) {
                x->stay+=stay;
                save_user_data(x);
        }
        exit(0);
}

int wwwagent()
{
        int i;
        for(i=0; i<1024; i++)
                close(i);
        for(i=0; i<NSIG; i++)
                signal(i, SIG_IGN);
        signal(SIGUSR2, add_msg);
        signal(SIGHUP, abort_program);
        while(1) {
                sleep(60);
                if(abs(time(0) - u_info->idle_time)>600) {
                        f_append("err", "idle timeout");
                        abort_program();
                }
        }
        exit(0);
}
