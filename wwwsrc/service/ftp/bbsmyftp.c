/*$Id: bbsmyftp.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

/* type defines
0 ά����ҳ
1 ����վ������
2 ɾ��վ������
3 վ���б�
*/

#include "ftpsearch.h"
int main()
{
        char buf[512];
        char file[80];
        char keystr[80]; //�ؼ���
        int has=1;
        init_all();
        if(!loginok)
                http_fatal("����δ��¼");
        modify_mode(u_info,FTPSEARCH);
        if(atoi(getparm("type"))==0) {
                if(!SiteInList(fromhost))
                        http_fatal("����δע������վ��");
                printf("<center>FTPվ�����ϵͳ<hr></center>");
                printf("<p>���ã���ӭ����FTPվ�����ϵͳ��������Resource�����Ǽǹ���վ����ɽ��뱾ϵͳ��");
                printf("<p>����վ��Ϊ<font color=red>%s</font>����ӭ���١�", fromhost);
                printf("<p>��ѡ��<a href=bbsmyftp?type=1>������վ�����ϡ�</a> <a href=bbsmyftp?type=2 onclick='return confirm(\"ȷʵҪɾ��%s��վ��������\")'>��ɾ��վ�����ϡ�</a> <a href=bbsftpsearch>������������ҳ��</a>", fromhost);
        } else if(atoi(getparm("type"))==1) {
                if(!SiteInList(fromhost))
                        http_fatal("����δע������վ��");
                printf("<center>FTPվ�����ϵͳ������վ������ <a href=bbsmyftp>�����ع�����ҳ��</a><hr></center>");
                printf("<br>��˵���������������վ��������δ�ϴ������ϻ�����Ҫ�ṩ������Ϣ�������������վ�����ϡ�");
                printf("<br>�Ͻ��ϴ���������Ӱ������������������У�������֣���һ�ξ��棬�ڶ��ν���������Χ��ɾ����վ�㣬лл������");
                printf("<br>�����²��衿<br><b>һ�����������ļ���</b><br>����������Windows����ϵͳΪ������������ɸ��ļ���<br>I����ĳ·������C:\\�£�����һ���ı��ļ��������ļ���Ϊcommand.txt�����ƿ�������𣩣��ڸ��ļ��������������У�<font color=blue><br>nk<br>nk<br>ls -lR<br>bye<br></font>Ȼ�󱣴档<br>II���������з�ʽ�����루�������ĵ�ǰ·��ΪC:\\��վ��IPΪ10.10.10.10������ļ���Ϊabc����<br><font color=blue>ftp -s:command.txt 10.10.10.10 > abc</font><br>���ɽ������ļ�������abc����ļ��С��ü��±�������ļ����Է�����Щ������ʵ��������FTPվ������Ŀ¼���ļ����б�<br><b>�����ϴ������ļ���</b><br>ȷ���ļ�������������·�ѡ�����ո����ɵ������ļ�C:\\abc�����������ϴ�����������һ��������<br><br><form method=post action=bbsftpdataupload enctype='multipart/form-data'><input type=file name=up><input type=submit value='�ϴ�'></form><br>");
        } else if(atoi(getparm("type"))==2) {
                if(!SiteInList(fromhost))
                        http_fatal("����δע������վ��");
                sprintf(buf, "%s/%s", LISTDIR, fromhost);
                unlink(buf);
                sprintf(buf, "%s/%s.dat", LISTDIR, fromhost);
                unlink(buf);
                ftplog(ADMINLOG, "DELETE", fromhost);
                redirect("bbsmyftp");
        } else if(atoi(getparm("type"))==3) {
                FILE *fp;
                char buf[80], buf2[256];
                int i=0;
                printf("<table><tr>");
                fp=fopen(LISTFILE, "r");
                while(fgets(buf, 80, fp)) {
                        strtok(buf, ": \r\t\n");
                        printf("<td>");
                        printf(buf);
                        i++;
                        if(i%5==0)
                                printf("<tr>");
                }
                fclose(fp);
                printf("</table>");
                printf("����%d��վ��.", i);
        } else if(atoi(getparm("type"))==4) {
                printf("�����ӵ���Ͽ���ѧ�������������FTPվ�㣬����Resource�������ϵ����ӭ���ļ��룡");
        }
}

