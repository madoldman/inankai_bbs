/*$Id: bbshelp.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char path[512], buf[512], ch, tmp[80], *ptr;
        NOTADDCSSFILE=1;
        init_all();
        strsncpy(path, "/groups/GROUP_0/sysop/wwwhelp/", 511);
        sprintf(path, "%s%s", path, getparm("file"));
        buf[0]=0;
        sprintf(buf, "0Announce%s", path);
        printf("<style type=text/css>\
               body {font-size:14.9px}\
               table {font-size:14.9px}\
               a {color:red;text-decoration:none}\
               </style>");
        printf("<body background=/bg_1.gif bgproperties=fixed><center><table border=0 width=400 align=middle>");
        printf("<tr><td><pre>");
        fp=fopen(buf, "r");
        printf("%s",buf);//return ;
        if(fp==0)
                http_fatal("错误的文件名");
        while(1) {
                if(fgets(buf, 256, fp)<=0)
                        break;
                printf("%s", buf);
        }
        fclose(fp);
        printf("</pre></table><hr>\n");
        printf("[<a href='javascript:history.go(-1)'>返回上一页</a>] ");
        printf("</center></body>\n");
        http_quit();
}
