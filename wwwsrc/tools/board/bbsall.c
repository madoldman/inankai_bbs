/*$Id: bbsall.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"

int main()
{
        struct shortfile data[MAXBOARD], *x;
        int i, total=0;
        char *ptr;
        init_all();
        modify_mode(u_info,READNEW+20000);	//bluetent
        printf("<style type=text/css>A {color: #0000f0}</style>");
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        for(i=0; i<MAXBOARD; i++) {
                x=&(shm_bcache->bcache[i]);
                if(x->filename[0]<=32 || x->filename[0]>'z')
                        continue;
                if(!has_read_perm(&currentuser, x->filename))
                        continue;
                memcpy(&data[total], x, sizeof(struct shortfile));
                total++;
        }
        qsort(data, total, sizeof(struct shortfile), cmpboard);
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        printf("<center>\n");
        //  printf("%s -- 全部讨论区 [讨论区数: %d]<hr color=green>", BBSNAME, total);
        printf("<center><img src=/allboards_b.gif></center><hr color=green>");
        printf("本站目前共有讨论区数: %d<br>", total);
        printf("<table width=610>\n");
        //	printf("<tr><td> </td><td> </td><td>讨论区名称</td><td>类别</td><td>中文描述</td><td>板主</td><td>文章数</td></tr>\n");
        printf("<tr><td> </td><td> </td><td>讨论区名称</td><td>类别</td><td></td><td>中文描述</td><td>板主</td></tr>\n");
        for(i=0; i<total; i++) {
                printf("<tr><td align=right>%d</td>", i+1);
                printf("<td>%s", board_read(data[i].filename) ? "◇" : "◆");
                printf("<td><a href=bbs%sdoc?board=%s>%s</a></td>", (currentuser.userdefine & DEF_THESIS)?"t":"", data[i].filename, data[i].filename);
                printf("<td>%6.6s</td>", data[i].title+1);
                if(data[i].flag&VOTE_FLAG) {
                        printf("<td><a href=bbsvote?board=%s><font color=red>V</font></a></td>", data[i].filename);
                } else
                        printf("<td></td>");

                printf("<td><a href=bbs%sdoc?board=%s>%s</a></td>", (currentuser.userdefine & DEF_THESIS)?"t":"", data[i].filename, data[i].title+7);
                ptr=strtok(data[i].BM, " ,;");
                if(ptr==0)
                        ptr="无";
                if(!strcmp(ptr, "SYSOPs")||!strcmp(ptr, "无"))
                        printf("<td>%s</td>", ptr);
                else
                        printf("<td><a href=bbsqry?userid=%s>%s</a></td>", ptr, ptr);
                //		printf("<td align=right>%d</td>\n", filenum(data[i].filename));

        }
        printf("</tr></table><hr></center>\n");
        http_quit();
}
