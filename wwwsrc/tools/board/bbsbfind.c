/*$Id: bbsbfind.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        int num=0, total=0, type, dt, m=0, g=0, og=0, u=0;
        char dir[80], title[80], title2[80], title3[80], board[80], userid[80], buf[80];
        struct shortfile *brd;
        struct fileheader x;
        init_all();
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        printf("<center>%s -- 板内文章搜索<hr color=green><br>\n", BBSNAME);
        type=atoi(getparm("type"));
        strsncpy(board, getparm("board"), 30);
        if(type==0)
                return show_form(board);
        strsncpy(title, getparm("title"), 60);
        strsncpy(title2, getparm("title2"), 60);
        strsncpy(title3, getparm("title3"), 60);
        strsncpy(userid, getparm("userid"), 60);
        dt=atoi(getparm("dt"));
        if(!strcasecmp(getparm("m"), "on"))
                m=1;
        if(!strcasecmp(getparm("g"), "on"))
                g=1;
        if(!strcasecmp(getparm("og"), "on"))
                og=1;
        if(!strcasecmp(getparm("u"), "on"))
                u=1;
        if(dt<0)
                dt=0;
        if(dt>9999)
                dt=9999;
        brd=getbcache(board);
        if(brd==0)
                http_fatal("错误的讨论区");
        strcpy(board, brd->filename);
        if(!has_read_perm(&currentuser, board))
                http_fatal("错误的讨论区名称");
        sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("讨论区错误或没有目前文章");
        printf("查找讨论区'%s'内, 标题含: '%s' ", board, nohtml(title));
        if(title2[0])
                printf("和 '%s' ", nohtml(title2));
        if(title3[0])
                printf("不含 '%s' ", nohtml(title3));
        if(g&&m)
                strcpy(buf, "精华文摘");
        else if(g)
                strcpy(buf, "文摘");
        else if(m)
                strcpy(buf, "精华");
        else
                strcpy(buf, "");
        printf("作者为: '%s', '%d'天以内的%s文章%s.<br>\n",
               userid[0] ? userid_str(userid) : "所有作者", dt, buf, u?"(包含附件)":"");
        printf("<table width=610>\n");
        printf("<tr><td>编号<td>标记<td>作者<td>日期<td>标题\n");
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)==0)
                        break;
                num++;
                if(title[0] && !strcasestr(x.title, title))
                        continue;
                if(title2[0] && !strcasestr(x.title, title2))
                        continue;
                if(userid[0] && strcasecmp(x.owner, userid))
                        continue;
                if(title3[0] && strcasestr(x.title, title3))
                        continue;
                if(abs(time(0)-atoi(x.filename+2))>dt*86400)
                        continue;
                if(m && !(x.accessed[0] & FILE_MARKED))
                        continue;
                if(g && !(x.accessed[0] & FILE_DIGEST))
                        continue;
                if(og && !strncmp(x.title, "Re: ", 4))
                        continue;
                if(u && x.filename[STRLEN-3]!='U')
                        continue;
                total++;
                printf("<tr><td>%d", num);
                printf("<td>%s", flag_str(x.accessed[0]));
                printf("<td>%s", userid_str(x.owner));
                printf("<td>%12.12s", 4+Ctime(atoi(x.filename+2)));
                printf("<td><a href=bbscon?board=%s&file=%s&num=%d>%46.46s </a>\n", board, x.filename, num-1, x.title);
                if(total>=999)
                        break;
        }
        fclose(fp);
        printf("</table>\n");
        printf("<br>共找到 %d 篇文章符合条件", total);
        if(total>999)
                printf("(匹配结果过多, 省略第1000以后的查询结果)");
        printf("<br>\n");
        printf("[<a href=bbsdoc?board=%s>返回本讨论区</a>] [<a href='javascript:history.go(-1)'>返回上一页</a>]", board);
        http_quit();
}

int show_form(char *board)
{
        printf("<table><form action=bbsbfind?type=1 method=post>\n");
        printf("<tr><td>板面名称: <input type=text maxlength=24 size=24 name=board value='%s'><br>\n", board);
        printf("<tr><td>标题含有: <input type=text maxlength=50 size=20 name=title> AND ");
        printf("<input type=text maxlength=50 size=20 name=title2>\n");
        printf("<tr><td>标题不含: <input type=text maxlength=50 size=20 name=title3>\n");
        printf("<tr><td>作者帐号: <input type=text maxlength=12 size=12 name=userid><br>\n");
        printf("<tr><td>时间范围: <input type=text maxlength=4  size=4  name=dt value=7> 天以内<br>\n");
        printf("<tr><td>文摘文章: <input type=checkbox name=g> ");
        printf("<tr><td>精华文章: <input type=checkbox name=m> ");
        printf("<tr><td>包含附件: <input type=checkbox name=u> ");
        printf("<tr><td>不含跟贴: <input type=checkbox name=og><br><br>\n");
        printf("<tr><td><input type=submit value=递交查询结果>\n");
        printf("</form></table>");
        printf("[<a href='bbsdoc?board=%s'>返回上一页</a>] [<a href=bbsfind>全站文章查询</a>]", board);
        http_quit();
}
