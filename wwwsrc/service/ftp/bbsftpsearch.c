/*$Id: bbsftpsearch.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "ftpsearch.h"
int main()
{
        char buf[512];
        char file[80];
        char keystr[80]; //关键字
        int has=1;
        init_all();
        if(!loginok)
                http_fatal("您尚未登录");
        modify_mode(u_info,FTPSEARCH);
        strsncpy(keystr, getparm("keystr"), 80);
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        if(!strlen(keystr)) {
                printf("<table width=600 align=center><tr height=150><td align=center valign=bottom style='font-size:14px'><font color=blue>局域网FTP搜索引擎<p><p></font></td></tr>");
                printf("<tr><td align=center style='font-size:14px'><form name=ftpsearch>请输入关键字：<input name=keystr type=text size=50 maxlength=80><input type=submit value=搜索> (<font color=red>%d</font>人在线)</form><script language=javascript>document.ftpsearch.keystr.focus();</script></td></tr>", countuser());
                printf("<tr><td><table><tr>");
                printf("<td><a href=bbsmyftp?type=4>我要加盟</a></td>");
                if(SiteInList(fromhost))
                        printf("<td> | <a href=bbsmyftp>管理站点数据</a> | <a href=bbsmyftp?type=3>已上传数据的站点列表</a></td>");
                printf("</tr></table></td></tr>");
                printf("</tr><tr><td width=80%%>【说明】 本搜索引擎用来查找南开大学校内教育网和宿舍网FTP站点的数据。<br>各位站长可以自行上传自己的站点数据，服务器端对原始数据进行重新编码以供搜索。<br>因该引擎仅依照编码后的数据进行搜索，故不会对网络负载产生影响。</td></tr>");
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
                http_fatal("请输入关键字");
        if((fp=fopen(LISTFILE, "r"))==NULL)
                http_fatal("内部错误");
        printf("<form name=ftpsearch>请输入关键字：<input name=keystr type=text size=50 maxlength=80 value='%s'><input type=submit value=搜索></form>[<a href=bbsftpsearch>返回搜索首页</a>]<hr>", keystr);
        while(fgets(tmp, 80, fp)) {
                strtok(tmp, ": \r\t\n");
                sprintf(searchfile, "/home/bbs/ftp/%s.dat", tmp);
                if((fp2=fopen(searchfile,"rb"))==NULL)
                        continue;
                while(fread(&record,sizeof(record),1,fp2)!=0) {
                        if (strcasestr(record.FileName,keystr)) {
                                //cout<<"["<<(record.IsDir=='d'?"目录：":"文件：")<<record.FileSize<<" byte  时间："<<record.FileTime<<"]"<<endl;
                                //cout<<"<a href=ftp://nk:nk@"<<FileLine<<record.PathName<<record.FileName<<">ftp://nk@nk:"<<FileLine<<record.PathName<<record.FileName<<"</a>"<<endl;
                                sprintf(sizestr, "<font color=blue>[%d字节]</font>", record.FileSize);
                                printf("<img src=/%s.gif><a href='ftp://nk:nk@%s%s%s'>ftp://nk:nk@%s%s%s</a>%s<br>", (record.IsDir=='d')?"folder":"link", tmp, record.PathName, record.FileName, tmp, record.PathName, record.FileName, (record.IsDir=='d')?"":sizestr);
                                found=1;
                        }
                }
                fclose(fp2);
        }
        fclose(fp);
        if(!found)
                printf("未找到匹配记录。");
        sprintf(buf, "%s '%s'", found?"Y":"N", keystr);
        ftplog(SEARCHLOG, buf, fromhost);
        printf("<hr><form name=ftpsearch>请输入关键字：<input name=keystr type=text size=50 maxlength=80 value='%s'><input type=submit value=搜索></form>[<a href=bbsftpsearch>返回搜索首页</a>]", keystr);

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
