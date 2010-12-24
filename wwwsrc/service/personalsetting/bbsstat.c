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
                http_fatal("�Ҵҹ��Ͳ���������");
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
        printf("<center>%s -- ��������ͳ�� [ʹ����: %s]<hr color=green>\n",
               BBSNAME, currentuser.userid);
        printf("<table width=320><tr><td>��Ŀ<td>��ֵ<td>ȫվ����<td>��Ա���\n");
        printf("<tr><td>��վ����<td>%d��<td>%d<td>TOP %5.2f%%",
               (time(0)-currentuser.firstlogin)/86400, lifes, (lifes*100.)/total);
        printf("<tr><td>��վ����<td>%d��<td>%d<td>TOP %5.2f%%",
               currentuser.numlogins, logins, logins*100./total);
        printf("<tr><td>��������<td>%d��<td>%d<td>TOP %5.2f%%",
               currentuser.numposts, posts, posts*100./total);
        printf("<tr><td>����ʱ��<td>%d��<td>%d<td>TOP %5.2f%%",
               currentuser.stay/60, stays, stays*100./total);
        printf("<tr><td>�ܾ���ֵ<td>%d��<td>%d<td>TOP %5.2f%%",
               myexp,exps,exps*100./total);
        printf("</table><br>���û���: %d", total);
}
