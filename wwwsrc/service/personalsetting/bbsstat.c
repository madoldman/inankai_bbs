/*$Id: bbsstat.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        struct userec x;
        int logins=0, posts=0, stays=0, lifes=0, total=0, exps=0;
        //huangxu@061017
        init_all();
        if(!loginok)
                http_fatal("匆匆过客不加入排名");
        int myexp=countexp(&currentuser);
        fp=fopen(".PASSWDS", "r");
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                if(x.userid[0]<'A')
                        continue;
                if(x.userlevel==0)
                        continue;
                if(x.numposts>=currentuser.numposts)
                        posts++;
                if(x.numlogins>=currentuser.numlogins)
                        logins++;
                if(x.stay>=currentuser.stay)
                        stays++;
                if(x.firstlogin<=currentuser.firstlogin)
                        lifes++;
                if(countexp(&x)>=myexp)
                        exps++;
                total++;
        }
        fclose(fp);
        printf("<center>%s -- 个人排名统计 [使用者: %s]<hr color=green>\n",
               BBSNAME, currentuser.userid);
        printf("<table width=320><tr><td>项目<td>数值<td>全站排名<td>相对比例\n");
        printf("<tr><td>本站网龄<td>%d天<td>%d<td>TOP %5.2f%%",
               (time(0)-currentuser.firstlogin)/86400, lifes, (lifes*100.)/total);
        printf("<tr><td>上站次数<td>%d次<td>%d<td>TOP %5.2f%%",
               currentuser.numlogins, logins, logins*100./total);
        printf("<tr><td>发表文章<td>%d次<td>%d<td>TOP %5.2f%%",
               currentuser.numposts, posts, posts*100./total);
        printf("<tr><td>在线时间<td>%d分<td>%d<td>TOP %5.2f%%",
               currentuser.stay/60, stays, stays*100./total);
        printf("<tr><td>总经验值<td>%d点<td>%d<td>TOP %5.2f%%",
               myexp,exps,exps*100./total);
        printf("</table><br>总用户数: %d", total);
}
