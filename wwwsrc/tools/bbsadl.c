/*$Id: bbsadl.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/
#define DIGESTPATH "/home/www/digest/"
#include "BBSLIB.inc"

int main()
{
        int i, no=0;
        char brd[100], buf[256];
        init_all();
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        printf("<nobr><center>%s -- 精华区下载服务<hr color=green>\n", BBSNAME);
        printf("【注】各板面精华区以压缩格式存储，请下载后用winzip等工具解压到某一目录后阅读。压缩包每周更新一次。<br>");
        printf("<table border=1>\n");
        printf("<tr><td>序号<td>名称<td>大小(字节)<td>更新时间\n");
        for(i=0; i<MAXBOARD; i++) {
                strsncpy(brd, shm_bcache->bcache[i].filename, 60);
                sprintf(buf, "%s%s.html.tar.gz", DIGESTPATH, brd);
                if(!file_exist(buf))
                        continue;
                no++;
                getdatestring(file_time(buf), NA);
                printf("<tr><td>%d<td><a href=/digest/%s.html.tar.gz>%s</a><td align=right>%d<td>%s\n", no, brd, brd, file_size(buf), datestring);
        }
        printf("</table>");
}
