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
                http_fatal("������ļ���");
        sprintf(buf, "0Announce%s", path);
        fp=fopen(buf, "r");
        if(fp==0)
                http_fatal("������ļ���");


        int r=getfrompath(path,board);
        if(r==0)
                strcpy(board,"");


        if(board[0])
                sprintf(buf, "%s��", board);
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        if(r==2)
                printf("<center>%s -- %s�ĸ����ļ�<hr>\n", BBSNAME, board);
        else
                printf("<center>%s -- %s������<hr>\n", BBSNAME, board);
        printf("<table border=0 width=610 align=middle>");
        printf("<tr><td style='font-size:14px'><pre>");
        while(1) {
                if(fgets(buf, 256, fp)==0)
                        break;
                hhprintf("%s", void1(buf));
        }
        fclose(fp);
        printf("</pre></table><hr>\n");
        printf("<a href='javascript:history.go(-1)'>������һҳ</a> <a href=\"/js/telnetview.html?url=/cgi-bin/bbs/jbbscon?file=%s\" target=_blank> Telnet��ʽ��������</a> ",path);
        if(r==1) {
                printf("<a href=bbsdoc?board=%s>  ��������</a>", board);
                //printf("<a href=bbs0an?path=%s>   ���徫����ҳ </a>",path);
        }
        printf("</center>\n");
        //check_msg();//bluetent 2002.10.31
        http_quit();
}
