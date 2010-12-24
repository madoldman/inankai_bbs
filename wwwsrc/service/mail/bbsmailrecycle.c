/*$Id: bbsmailrecycle.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"

char *eff_size(char *file)
{
        FILE *fp;
        static char buf[256];
        int i, size, size2=0;
        size=file_size(file);
        if(size>3000|| size==0)
                goto E;
        size=0;
        fp=fopen(file, "r");
        if(fp==0)
                return "-";
        for(i=0; i<3; i++)
                if(fgets(buf, 255, fp)==0)
                        break;
        while(1) {
                if(fgets(buf, 255, fp)==0)
                        break;
                if(!strcmp(buf, "--\n"))
                        break;
                if(!strncmp(buf, ": ", 2))
                        continue;
                if(!strncmp(buf, "【 在 ", 4))
                        continue;
                if(strstr(buf, "※ 来源:．"))
                        continue;
                for(i=0; buf[i]; i++)
                        if(buf[i]<0)
                                size2++;
                size+=strlen(trim(buf));
        }
        fclose(fp);
E:
        /*        if(size<2048) {
                        sprintf(buf, "(<font style='font-size:12px; color:#008080'>%d字</font>)", size-size2/2);
                } else {
                        sprintf(buf, "(<font style='font-size:12px; color:#f00000'>%d.%dK</font>)", size/1000, (size/100)%10);
                }*/
        sprintf(buf, "%d", size-size2/2);
        return buf;
}

int main()
{
        FILE *fp;
        int filetime, i, start, total;
        int ret, max=0, size=0;
        char *ptr, buf[512], dir[80];
        struct fileheader *data;
        setreuid(BBSUID, BBSUID);
        init_all();
        modify_mode(u_info,MAIL+20000);	//bluetent
        if(!loginok)
                http_fatal("您尚未登录, 请先登录");
        strsncpy(buf, getparm("start"), 10);
        start=atoi(buf)-1;
        if(buf[0]==0)
                start=999999;
        // printf("<META  content=no-cache>");
        printf("<script language=javascript>function SelectAll(){\
               for (i=0;i<document.form1.length;i++)\
       {\
               if (document.form1.elements[i].type==\"checkbox\")\
               if (! document.form1.elements[i].checked)\
       {\
               document.form1.elements[i].checked=true;\
       }\
       }\
       }</script>");
        printf("<center>\n");
        printf("%s -- 邮件回收站[%s]\n", BBSNAME, currentuser.userid);
        printf("<hr color=green width=650>\n");
        sprintf(dir, "mail/%c/%s/.recycle", toupper(currentuser.userid[0]), currentuser.userid);




        total=file_size(dir)/sizeof(struct fileheader);
        if(total<0 || total>30000)
                http_fatal("信件太多,请清理回收站.");
        if(total==0) {
                printf("<center>\n");
                printf("你的回收站为空.<br><br><br><a href=bbsmail> 返回收件箱</a>");
                http_quit();
        }

        data=(struct fileheader *)calloc(total, sizeof(struct fileheader));
        if(data==0)
                http_fatal("memory overflow");
        if(!file_exist(dir)) {
                char buf[80];
                sprintf(buf, "mail/%c/%s", toupper(currentuser.userid[0]), currentuser.userid);
                if(!file_exist(buf))
                        mkdir(buf, 0770);
                fp=fopen(dir, "a+");
                fclose(fp);
        }//第一次使用回收站,新建文件.





        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("dir error");
        total=fread(data, sizeof(struct fileheader), total, fp);
        fclose(fp);
        printf("<table><form><tr><td>");
        if(start>total-19)
                start=total-19;
        if(start<0)
                start=0;
        if(start>0) {
                i=start-19;
                if(i<0)
                        i=0;
                printf("[<a href=bbsmailrecycle?start=0>第一页</a>] ");
                printf("[<a href=bbsmailrecycle?start=%d>上一页</a>] ", i);
        }
        if(start<total-19) {
                i=start+19;
                if(i>total-1)
                        i=total-1;
                printf("[<a href=bbsmailrecycle?start=%d>下一页</a>] ", i);
                printf("[<a href=bbsmailrecycle>最后一页</a>]");
        }

        printf("[<a href='javascript:SelectAll()'>选中全部</a>]");
        printf("<input class=btn type=submit value=跳转到> 第 <input style='height:20px' type=text name=start size=3> 封</form></table>");
        printf("<table><form name=form1 topmargin=0 leftmargin=0 action=bbsman2recycle method=post><tr><td>\n");
        printf("<input class=btn onclick='return confirm(\"确实要还原所有标记的信件吗\")' type=submit value='还原标记的信件'>\n");

        //	printf("[您的信箱容量为: %dK, 目前已使用: %dK, 信件总数: %d封] ", max, size, total);
        //printf("<input type=button class=btn onclick=javascript:location.href='bbsmailclear' value=清空回收站>\n");
        //printf("<input type=button class=btn onclick=javascript:location.href='bbsmail' value=我的收件箱>\n");

        printf("</td><td align=right width=70%>\
               <input type=button class=btn onclick=\"if(confirm('确定要清空吗?')) location.href='bbsmailclear'\"  value=清空回收站>");
        printf("<a href=bbsmail> 我的收件箱 </a>");
        printf("</td></tr></table>\n");

        printf("<table width=610 cellspacing=1 cellpadding=2 bgColor=%s>\n", color0);
        printf("<tr height=20><td align=middle bgColor=%s><font color=#FFFFFF>序号<td align=middle bgColor=%s><font color=#FFFFFF>还原<td align=middle bgColor=%s><font color=#FFFFFF>状态<td align=middle bgColor=%s><font color=#FFFFFF>发信人<td align=middle bgColor=%s><font color=#FFFFFF>日期<td align=middle bgColor=%s><font color=#FFFFFF>信件标题<td align=middle bgColor=%s><font color=#FFFFFF>字数\n", color0, color0, color0, color0, color0, color0, color0);
        for(i=start; i<start+19 && i<total; i++) {
                int type='N', type2=' ';
                char filename[256];
                printf("<tr><td align=right bgColor=%s>%d", color1, i+1);
                printf("<td align=middle bgColor=%s><input style='height:14px' type=checkbox name=box%s>", tablec, data[i].filename);
                if(data[i].accessed[0] & FILE_READ)
                        type=' ';
                if(data[i].accessed[0] & FILE_MARKED)
                        type= (type=='N') ? 'M' : 'm';
                if(type!='N') {
                        printf("<td align=middle bgColor=%s>%c%c", tablec,  type, type2);
                } else {
                        printf("<td align=middle bgColor=%s><img src=/newmail.gif>", tablec);
                }
                ptr=strtok(data[i].owner, " (");
                if(ptr==0)
                        ptr=" ";
                ptr=nohtml(ptr);
                printf("<td bgColor=%s><a href=bbsqry?userid=%s>%13.13s</a>", tablec, ptr, ptr);
                filetime=atoi(data[i].filename+2);
                printf("<td bgColor=%s>%12.12s", tablec, Ctime(filetime)+4);
                printf("<td bgColor=%s><a href=bbsmailcon?file=%s&num=%d&mode=1>", tablec , data[i].filename, i);
                if(strncmp("Re: ", data[i].title, 4))
                        printf("★ ");
                hprintf("%42.42s", void1(data[i].title));
                sprintf(filename, "mail/%c/%s/%s",
                        toupper(currentuser.userid[0]), currentuser.userid, data[i].filename);
                printf("</a><td bgColor=%s align=right>%s\n",tablec,  eff_size(filename));
        }
        free(data);
        printf("</table></form><hr color=green width=650>\n");
        http_quit();

}
