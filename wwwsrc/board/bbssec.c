/*$Id: bbssec.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        int i;
        init_all();
        modify_mode(u_info,LOGIN+20000);	//bluetent
        printf("<style type=text/css>A {color: #0000f0}</style>");
        printf("<center>\n");
        //   	printf("%s -- 分类讨论区 <hr color=green>\n", BBSNAME);
        printf("<img src=/fenlei_b.gif><hr color=green>\n");
        printf("<table>\n");
        //   	printf("<tr><td>区号<td>类别<td>描述\n");
        for(i=0; i<SECNUM; i++) {
                printf("<tr><td align=right>%d<td><a href=bbsboa?%d>%s</a>", i, i, secname[i][0]);
                printf("<td><a href=bbsboa?%d>%s</a>\n", i, secname[i][1]);
        }
        printf("</table><hr color=green>\n");
        //   	printf("[<a href=bbsall>全部讨论区</a>]");
        //   	printf("[<a href=bbs0an>精华公布栏</a>]");
        printf("</center>\n");
        printf("<br><a href=bbsmain>.</a>");
        http_quit();
}
