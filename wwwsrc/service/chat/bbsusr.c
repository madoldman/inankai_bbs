/*$Id: bbsusr.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "user.h"
struct user_info user[MAXACTIVE];
int my_t_lines;

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
        int i, start, total=0, fh, shmkey, shmid;
        struct user_info *x;
        char search[80];
        char modetypestr[80], ipstr[80];//bluetent
        int iswwwmode, hideip;//bluetent
        init_all();
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        printf("<center>\n");
        if(HAS_PERM(PERM_SEECLOAK))
                printf("%s -- 在线用户列表 [目前在线: %d人]<hr>\n", BBSNAME, count_online());
        else
                printf("%s -- 在线用户列表 [目前在线: %d人]<hr>\n", BBSNAME, count_online2());
        for(i=0; i<MAXACTIVE; i++) {
                x=&(shm_utmp->uinfo[i]);
                if(x->active==0)
                        continue;
                if(x->invisible && !HAS_PERM(PERM_SEECLOAK) && strcasecmp(x->userid,currentuser.userid) && !canseeme(x->userid))//huangxu@060714:mylover
                        continue;
                memcpy(&user[total], x, sizeof(struct user_info));
                total++;
        }
        start=atoi(getparm("start"));
        my_t_lines=atoi(getparm("my_t_lines"));
        if(my_t_lines<10 || my_t_lines>40)
                my_t_lines=20;
        printf("<table width=610>\n");
        printf("<tr><td>序号<td>友<td>使用者代号<td>使用者昵称<td>来自<td>动态<td>发呆\n");
        qsort(user, total, sizeof(struct user_info), cmpuser);
        if(start>total-5)
                start=total-5;
        if(start<0)
                start=0;
        char friendmark[STRLEN];
        int mine=0,his=0;
        for(i=start; i<start+my_t_lines && i<total; i++) {
                int dt=(time(0)-user[i].idle_time)/60;
                printf("<tr><td>%d", i+1);

                strcpy(friendmark,"");
                mine=isfriend(user[i].userid);
                his=hisfriend(&user[i]);
                if(mine&&!his)
                        strcpy(friendmark,"<font color=red>√</font>");
                if(!mine&&his)
                        strcpy(friendmark,"<font color=Fuchsia>√</font>");
                if(mine&&his)
                        strcpy(friendmark,"<font color=green>√</font>");
                printf("<td>%s", friendmark);

                printf("%s", user[i].invisible ? "<font color=green>C</font>" : " ");
                printf("<td><a href=bbsqry?userid=%s>%s</a>", user[i].userid, user[i].userid);
                printf("<td><a href=bbsqry?userid=%s>%24.24s </a>", user[i].userid, nohtml(user[i].username));
                //huangxu@060405:no hide ip
                //hideip = !HAS_PERM(PERM_SEEUSERIP)&&(user[i].from[22]=='H')&&strstr(user[i].from, "10.10.");
                //if(hideip)
                //        strcpy(ipstr, "南开宿舍内网");
                //else
                        strcpy(ipstr, user[i].from);
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
        printf("<hr>");
        printf("[<a href='bbsufind?search=*'>全部</a>] ");
        for(i='A'; i<='Z'; i++)
                printf("<a href=bbsufind?search=%c>-%c-</a>", i, i);
        printf("<br>\n");
        printf("[<a href=bbsfriend>在线好友</a>] ");
        if(start>0)
                printf("[<a href=bbsusr?start=%d>上一页</a>]", start-20);
        if(start<total-my_t_lines)
                printf("[<a href=bbsusr?start=%d>下一页</a>]", start+my_t_lines);
        printf("<br><form action=bbsusr>\n");
        printf("<input type=submit value=跳转到第> ");
        printf("<input type=input size=4 name=start> 个使用者</form>");
        printf("</center>\n");
        //check_msg();//bluetent 2002.10.31
        http_quit();
}
