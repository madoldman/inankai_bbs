/*$Id: bbstopb10.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char buf[256], tmp[256], name[256], cname[256], cc[256], poschg[80];
        int i, r, pos, max;
        init_all();
        fp=fopen("0Announce/bbslist/mactodayboard2", "r");
        if(fp==0)
                http_fatal("error 1");
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        printf("<center>%s -- 热门讨论区<hr color=green>", BBSNAME);
        printf("<table border=0>\n");
        printf("<tr><td>名次<td>升降<td>板名<td>中文板名<td align=right>人气<td width=200>\n");
        for(i=0; i<MAXBOARD; i++) {
                if(fgets(buf, 150, fp)==0)
                        break;
                if(i==0)
                        continue;
                r=sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp, cname, cc);
                if(i==1)
                        max=atoi(cc);
                if(r==6&&strcmp(name, "Average")) {
                        pos=oldchartpos(name);
                        if(pos==0)
                                sprintf(poschg, " ");
                        else if(i<pos) {
                                sprintf(poschg, "<font color=red>↑%d</font>", pos-i);
                        } else if(i==pos) {
                                strcpy(poschg, "<font color=black>-</font>");
                        } else if(i>pos) {
                                sprintf(poschg, "<font color=blue>↓%d</font>", i-pos);
                        }

                        printf("<tr><td>%d<td align=center>%s<td><a href=bbsdoc?board=%s>%s</a><td width=200><a href=bbsdoc?board=%s>%s</a><td align=right>%s<td width=200><table width=%d%% cellspacing=0 cellpadding=0 bgcolor=#80CCA2 height=6><tr><td></table>\n",
                               i, poschg, name, name, name, cname, cc, atoi(cc)*100/max);
                }
        }
        printf("</table>\n<br>[<a href=javascript:history.go(-1)>返回</a>]</center>\n");
        fclose(fp);
}

int oldchartpos(char *board)
{
        FILE *fp;
        int i, r;
        char buf[256], tmp[80], name[80];
        fp=fopen("0Announce/bbslist/todayboard2.old", "r");
        if(fp==0)
                return 0;
        for(i=0; i<MAXBOARD; i++) {
                if(fgets(buf, 150, fp)==0)
                        break;
                if(i==0)
                        continue;
                r=sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp, tmp, tmp);
                if(r==6&&!strcmp(name, board)) {
                        fclose(fp);
                        return i;
                }
        }
        fclose(fp);
        return 0;
}
