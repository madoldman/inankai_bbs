/*$Id: bbsdread.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/
#include "BBSLIB.inc"
int main()
{
        FILE *fp;
        char  path[128];
        init_all();
        char *board;
        modify_mode(u_info,DIGEST+20000);
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        strsncpy(path, getparm("path"),128);
        char filebuf[2056],buf[128];
        if(strstr(path, "..") || strstr(path, "SYSHome"))
                http_fatal("此目录不存在");
        sprintf(buf,"0Announce%s/.html", path);
        int fd=0;
        if((fd=open(buf,O_RDONLY))==EOF) {
                showinfo("不能打开文件.!请检查板面是否存在.");
                return 1;
        }
        bzero(filebuf,2056);
        while(read(fd,filebuf,2056)>0) {
                printf("%s",filebuf);
                bzero(filebuf,2056);
        }
        close(fd);
        printf("</pre><center><a href='javascript:history.go(-1)'>点击返回</a> ");
        return 0;
}
