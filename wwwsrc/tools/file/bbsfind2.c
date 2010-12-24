/*$Id: bbsfind2.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char board[256], match[256], user[256], exp[256];
        struct dir x[20000], xx;
        int i, start, total=0, newmode=0;
        init_all();
        if(!currentuser.userlevel & PERM_BASIC)
                http_fatal("error");
        strsncpy(match, getparm("match"), 30);
        strsncpy(user, getparm("user"), 30);
        strsncpy(exp, getparm("exp"), 30);
        start=atoi(getparm("start"));
        newmode=atoi(getparm("newmode"));
        if((match[0]==0 || match[0]=='.')&&(user[0]==0 || user[0]=='.')&&(exp[0]==0 || exp[0]=='.')&&newmode!=1)
                http_fatal("请输入查询条件");
        //	if((match[0]==0 || match[0]=='.')&&(user[0]==0 || user[0]=='.')) {
        printf("<center> %s -- 全站上载文件查询<hr color=green></center>\n", BBSNAME);
        printf("<body><form name=form1 action=bbsfind2 method=post>\n");
        printf("请输入查询条件：<br>");
        printf("文件名: <input name=match value=\"%s\">\n", match);
        printf("上载者：<input name=user value=\"%s\">\n", user);
        printf("附件说明：<input name=exp value=\"%s\">\n", exp);
        printf("<input type=submit value='确定'>\n");
        printf("</form><script>document.form1.match.focus()</script>\n");
        if(newmode==1)
                http_quit();
        //	}
        fp=fopen(UPLOAD_PATH"/.DIR", "r");
        if(fp==0)
                http_fatal("没有找到文件");
        while(total<20000) {
                if(fread(&xx, sizeof(xx), 1, fp)<=0)
                        break;
                if(!xx.active)
                        continue;
                if(!strcasestr(xx.showname, match)&&strlen(match)>=0)
                        continue;
                if(!strcasestr(xx.userid, user)&&strlen(user)>=0)
                        continue;
                if(!strcasestr(xx.exp, exp)&&strlen(exp)>=0)
                        continue;
                x[total]=xx;
                total++;
        }
        if(strlen(user)==0)
                printf("<center><table width=600><tr bgcolor=e8e8e8><td align=center>序号<td align=center>文件名<td align=center>上载者<td align=center>板面<td align=center width=150>时间<td align=center>大小<td align=center>访问<td align=center>说明<td align=center>管理<td align=center>备注\n");
        else
                printf("<center><table width=600><tr bgcolor=e8e8e8><td align=center>序号<td align=center>文件名<td align=center>板面<td align=center width=150>时间<td align=center>大小<td align=center>访问<td align=center>说明<td align=center>管理<td align=center>备注\n");
        for(i=start; i<total; i++) {
                if(i-start>=1000)
                        break;
                printf("<tr><td align=right>%d", i+1);
                //	printf("<td><a href=showfile?name=%s target=_blank>%s</a>", nohtml(x[i].showname), nohtml(x[i].showname));

                if(x[i].filename) {
                        printf("<td><a href=/cgi-bin/bbs/showfile?name=%s target=_blank>%s</a>", nohtml(x[i].showname), nohtml(x[i].showname));
                } else {
                        printf("<td><a href=/file/%s/%s>%s</a>", x[i].board, nohtml(x[i].showname), nohtml(x[i].showname));
                }
                if(strlen(user)==0)
                        printf("<td>%s", userid_str(x[i].userid));
                printf("<td>%s", x[i].board);
                printf("<td>%s", Ctime(x[i].date));
                printf("<td align=right>%d", x[i].size);
                printf("<td align=right>%d", x[i].click);
                printf("<td>%s", x[i].exp);
                printf("<td align=center><a onclick='return confirm(\"确定删除吗?\")' href=bbsudel?file=%s>删</a>\n", x[i].showname);
                printf("<td><a href=bbsfexp?name=%s>more</a>\n", nohtml(x[i].showname));
        }
        printf("</table><br>\n");
        if(total-start>1000)
                printf("too many files<br>\n");
        printf("[<a href=javascript:history.go(-1)>返回</a>]");
        printf("</body>");
        http_quit();
}
