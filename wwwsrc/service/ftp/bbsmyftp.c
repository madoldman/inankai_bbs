/*$Id: bbsmyftp.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

/* type defines
0 维护首页
1 更新站点资料
2 删除站点资料
3 站点列表
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
        if(atoi(getparm("type"))==0) {
                if(!SiteInList(fromhost))
                        http_fatal("您尚未注册您的站点");
                printf("<center>FTP站点管理系统<hr></center>");
                printf("<p>您好，欢迎进入FTP站点管理系统。凡是向Resource板主登记过的站点均可进入本系统。");
                printf("<p>您的站点为<font color=red>%s</font>，欢迎光临。", fromhost);
                printf("<p>请选择：<a href=bbsmyftp?type=1>【更新站点资料】</a> <a href=bbsmyftp?type=2 onclick='return confirm(\"确实要删除%s的站点资料吗？\")'>【删除站点资料】</a> <a href=bbsftpsearch>【返回搜索首页】</a>", fromhost);
        } else if(atoi(getparm("type"))==1) {
                if(!SiteInList(fromhost))
                        http_fatal("您尚未注册您的站点");
                printf("<center>FTP站点管理系统：更新站点资料 <a href=bbsmyftp>【返回管理首页】</a><hr></center>");
                printf("<br>【说明】如果您自申请站点以来尚未上传过资料或者需要提供最新信息，请在这里更新站点资料。");
                printf("<br>严禁上传垃圾数据影响搜索引擎的正常运行，如果发现，第一次警告，第二次将从搜索范围内删除该站点，谢谢合作。");
                printf("<br>【更新步骤】<br><b>一、生成资料文件。</b><br>这里我们以Windows操作系统为例介绍如何生成该文件。<br>I、在某路径（如C:\\下）建立一个文本文件（假设文件名为command.txt，名称可以随便起），在该文件中输入以下四行：<font color=blue><br>nk<br>nk<br>ls -lR<br>bye<br></font>然后保存。<br>II、在命令行方式下输入（假设您的当前路径为C:\\，站点IP为10.10.10.10，输出文件名为abc）：<br><font color=blue>ftp -s:command.txt 10.10.10.10 > abc</font><br>即可将资料文件生成至abc这个文件中。用记事本打开这个文件可以发现这些资料其实就是您的FTP站点所有目录和文件的列表。<br><b>二、上传资料文件。</b><br>确认文件生成无误后，在下方选择您刚刚生成的资料文件C:\\abc，并单击“上传”，进入下一步操作。<br><br><form method=post action=bbsftpdataupload enctype='multipart/form-data'><input type=file name=up><input type=submit value='上传'></form><br>");
        } else if(atoi(getparm("type"))==2) {
                if(!SiteInList(fromhost))
                        http_fatal("您尚未注册您的站点");
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
                printf("共计%d个站点.", i);
        } else if(atoi(getparm("type"))==4) {
                printf("如果您拥有南开大学教育网或局域网FTP站点，请与Resource板板主联系，欢迎您的加入！");
        }
}

