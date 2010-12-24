/*$Id: bbsfriend.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "user.h"
struct user_info user[MAXACTIVE];

int cmpuser(a, b)
struct user_info *a, *b;
{
        char id1[80], id2[80];
        sprintf(id1, "%d%s", !isfriend(a->userid), a->userid);
        sprintf(id2, "%d%s", !isfriend(b->userid), b->userid);
        return strcasecmp(id1, id2);
}

int main()
{
        int i, total=0, fh, shmkey, shmid;
        struct user_info *x;
        char modetypestr[80], ipstr[80];//bluetent
        int iswwwmode, hideip;//bluetent
        init_all();
        modify_mode(u_info,FRIEND+20000);	//bluetent
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        printf("<center>\n");
        printf("%s -- 在线好友列表 [使用者: %s]<hr>\n", BBSNAME, currentuser.userid);
        for(i=0; i<MAXACTIVE; i++) {
                x=&(shm_utmp->uinfo[i]);
                if(x->active==0)
                        continue;
                if(x->invisible && !HAS_PERM(PERM_SEECLOAK) && strcasecmp(x->userid,currentuser.userid) && !canseeme(x->userid))
                //huangxu@060714:我的爱人（mylover）
                        continue;
                memcpy(&user[total], x, sizeof(struct user_info));
                total++;
        }
        printf("<table width=610 border=0>\n");
        printf("<tr><td>序号<td>友<td>使用者代号<td>使用者昵称<td>来自<td>动态<td>发呆\n");
        qsort(user, total, sizeof(struct user_info), cmpuser);
        for(i=0; i<total; i++) {
                int dt=(time(0)-user[i].idle_time)/60;
                if(!isfriend(user[i].userid))
                        continue;
                printf("<tr><td>%d", i+1);
                printf("<td>%s", hisfriend(&user[i])?"<font color=green>√</font>":"<font color=red>√</font>");
                printf("%s", user[i].invisible ? "<font color=green>C</font>" : " ");
                printf("<td><a href=bbsqry?userid=%s>%s</a>", user[i].userid, user[i].userid);
                printf("<td><a href=bbsqry?userid=%s>%24.24s </a>", user[i].userid, nohtml(user[i].username));
//huangxu@060405
//                hideip = !HAS_PERM(PERM_SEEUSERIP)&&(user[i].from[22]=='H')&&strstr(user[i].from, "10.10.");
//                if(hideip)
//                        strcpy(ipstr, "南开宿舍内网");
//                else
                        strcpy(ipstr, user[i].from);
                //		if(user[i].mode>20000)
                //			sprintf(ipstr, "<font class=c35>%s</font>", ipstr);
                if(user[i].mode>20000)
                        printf("<td><font class=c35>%s</font>", ipstr);
                else
                        printf("<td>%s", ipstr);
                if(user[i].mode>20000)
                        sprintf(modetypestr, "%s", ModeType(user[i].mode-20000));
                else
                        sprintf(modetypestr, "%s", ModeType(user[i].mode));
                if (user[i].invisible)
                        printf("<td><font class=c36>%s</font></td>", modetypestr);
                else
                        printf("<td>%s</td>", modetypestr);
                if(dt==0) {
                        printf("<td> \n");
                } else {
                        printf("<td>%d\n", dt);
                }
        }
        printf("</table>\n");
        if(total==0)
                printf("目前没有好友在线");
        printf("<hr>");
        printf("[<a href=bbsfall>全部好友名册</a>]");
        printf("</center>\n");
        check_msg();//bluetent 2002.10.31
        http_quit();
}
