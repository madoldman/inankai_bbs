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
        printf("<nobr><center>%s -- ���������ط���<hr color=green>\n", BBSNAME);
        printf("��ע�������澫������ѹ����ʽ�洢�������غ���winzip�ȹ��߽�ѹ��ĳһĿ¼���Ķ���ѹ����ÿ�ܸ���һ�Ρ�<br>");
        printf("<table border=1>\n");
        printf("<tr><td>���<td>����<td>��С(�ֽ�)<td>����ʱ��\n");
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
