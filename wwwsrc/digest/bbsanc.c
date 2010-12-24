/*$Id: bbsanc.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char board[32], path[512], buf[512], ch, tmp[80], *ptr;
        init_all();
        modify_mode(u_info,DIGEST+20000);	//bluetent
        strsncpy(path, getparm("path"), 511);
        if(strstr(path, ".Search") || strstr(path, ".Names")|| strstr(path, "..")|| strstr(path, "SYSHome"))
                http_fatal("错误的文件名");
        sprintf(buf, "0Announce%s", path);
        fp=fopen(buf, "r");
        if(fp==0)
                http_fatal("错误的文件名");


        int r=getfrompath(path,board);
        if(r==0)
                strcpy(board,"");


        if(board[0])
                sprintf(buf, "%s版", board);
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        if(r==2)
                printf("<center>%s -- %s的个人文集<hr>\n", BBSNAME, board);
        else
                printf("<center>%s -- %s精华区<hr>\n", BBSNAME, board);
        printf("<table border=0 width=610 align=middle>");
        printf("<tr><td style='font-size:14px'><pre>");
        while(1) {
                if(fgets(buf, 256, fp)==0)
                        break;
                hhprintf("%s", void1(buf));
        }
        fclose(fp);
        printf("</pre></table><hr>\n");
        printf("<a href='javascript:history.go(-1)'>返回上一页</a> <a href=\"/js/telnetview.html?url=/cgi-bin/bbs/jbbscon?file=%s\" target=_blank> Telnet样式阅览本文</a> ",path);
        if(r==1) {
                printf("<a href=bbsdoc?board=%s>  本讨论区</a>", board);
                //printf("<a href=bbs0an?path=%s>   本板精华首页 </a>",path);
        }
        printf("</center>\n");
        //check_msg();//bluetent 2002.10.31
        http_quit();
}
