/*$Id: bbsftpsearch.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
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
        strsncpy(keystr, getparm("keystr"), 80);
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        if(!strlen(keystr)) {
                printf("<table width=600 align=center><tr height=150><td align=center valign=bottom style='font-size:14px'><font color=blue>������FTP��������<p><p></font></td></tr>");
                printf("<tr><td align=center style='font-size:14px'><form name=ftpsearch>������ؼ��֣�<input name=keystr type=text size=50 maxlength=80><input type=submit value=����> (<font color=red>%d</font>������)</form><script language=javascript>document.ftpsearch.keystr.focus();</script></td></tr>", countuser());
                printf("<tr><td><table><tr>");
                printf("<td><a href=bbsmyftp?type=4>��Ҫ����</a></td>");
                if(SiteInList(fromhost))
                        printf("<td> | <a href=bbsmyftp>����վ������</a> | <a href=bbsmyftp?type=3>���ϴ����ݵ�վ���б�</a></td>");
                printf("</tr></table></td></tr>");
                printf("</tr><tr><td width=80%%>��˵���� �������������������Ͽ���ѧУ�ڽ�������������FTPվ������ݡ�<br>��λվ�����������ϴ��Լ���վ�����ݣ��������˶�ԭʼ���ݽ������±����Թ�������<br>�����������ձ��������ݽ����������ʲ�������縺�ز���Ӱ�졣</td></tr>");
                printf("</table>");
        } else {
                sprintf(file, "tmp/%d.tmp", getpid());
                search(keystr, file);
                printf("</pre>");
                unlink(file);
        }
        http_quit();
}

int search(char *keystr, char *file)
{
        FILE *fp, *fp2;
        struct FileInfo record;
        char FileLine[MAX_LINE_LEN];
        char tmp[80], buf[256], searchfile[80], sizestr[80];
        int found=0;
        if(strlen(keystr)==0)
                http_fatal("������ؼ���");
        if((fp=fopen(LISTFILE, "r"))==NULL)
                http_fatal("�ڲ�����");
        printf("<form name=ftpsearch>������ؼ��֣�<input name=keystr type=text size=50 maxlength=80 value='%s'><input type=submit value=����></form>[<a href=bbsftpsearch>����������ҳ</a>]<hr>", keystr);
        while(fgets(tmp, 80, fp)) {
                strtok(tmp, ": \r\t\n");
                sprintf(searchfile, "/home/bbs/ftp/%s.dat", tmp);
                if((fp2=fopen(searchfile,"rb"))==NULL)
                        continue;
                while(fread(&record,sizeof(record),1,fp2)!=0) {
                        if (strcasestr(record.FileName,keystr)) {
                                //cout<<"["<<(record.IsDir=='d'?"Ŀ¼��":"�ļ���")<<record.FileSize<<" byte  ʱ�䣺"<<record.FileTime<<"]"<<endl;
                                //cout<<"<a href=ftp://nk:nk@"<<FileLine<<record.PathName<<record.FileName<<">ftp://nk@nk:"<<FileLine<<record.PathName<<record.FileName<<"</a>"<<endl;
                                sprintf(sizestr, "<font color=blue>[%d�ֽ�]</font>", record.FileSize);
                                printf("<img src=/%s.gif><a href='ftp://nk:nk@%s%s%s'>ftp://nk:nk@%s%s%s</a>%s<br>", (record.IsDir=='d')?"folder":"link", tmp, record.PathName, record.FileName, tmp, record.PathName, record.FileName, (record.IsDir=='d')?"":sizestr);
                                found=1;
                        }
                }
                fclose(fp2);
        }
        fclose(fp);
        if(!found)
                printf("δ�ҵ�ƥ���¼��");
        sprintf(buf, "%s '%s'", found?"Y":"N", keystr);
        ftplog(SEARCHLOG, buf, fromhost);
        printf("<hr><form name=ftpsearch>������ؼ��֣�<input name=keystr type=text size=50 maxlength=80 value='%s'><input type=submit value=����></form>[<a href=bbsftpsearch>����������ҳ</a>]", keystr);

        return 1;
}


int countuser()
{
        extern struct UTMPFILE *shm_utmp;
        struct user_info *x;
        int total=0, i;
        for(i=0; i<MAXACTIVE; i++) {
                x=&(shm_utmp->uinfo[i]);
                if(x->active==0)
                        continue;
                if(x->mode==20086)
                        total++;
        }
        return total;

}
