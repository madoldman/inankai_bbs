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
                if(!strncmp(buf, "�� �� ", 4))
                        continue;
                if(strstr(buf, "�� ��Դ:��"))
                        continue;
                for(i=0; buf[i]; i++)
                        if(buf[i]<0)
                                size2++;
                size+=strlen(trim(buf));
        }
        fclose(fp);
E:
        /*        if(size<2048) {
                        sprintf(buf, "(<font style='font-size:12px; color:#008080'>%d��</font>)", size-size2/2);
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
                http_fatal("����δ��¼, ���ȵ�¼");
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
        printf("%s -- �ʼ�����վ[%s]\n", BBSNAME, currentuser.userid);
        printf("<hr color=green width=650>\n");
        sprintf(dir, "mail/%c/%s/.recycle", toupper(currentuser.userid[0]), currentuser.userid);




        total=file_size(dir)/sizeof(struct fileheader);
        if(total<0 || total>30000)
                http_fatal("�ż�̫��,���������վ.");
        if(total==0) {
                printf("<center>\n");
                printf("��Ļ���վΪ��.<br><br><br><a href=bbsmail> �����ռ���</a>");
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
        }//��һ��ʹ�û���վ,�½��ļ�.





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
                printf("[<a href=bbsmailrecycle?start=0>��һҳ</a>] ");
                printf("[<a href=bbsmailrecycle?start=%d>��һҳ</a>] ", i);
        }
        if(start<total-19) {
                i=start+19;
                if(i>total-1)
                        i=total-1;
                printf("[<a href=bbsmailrecycle?start=%d>��һҳ</a>] ", i);
                printf("[<a href=bbsmailrecycle>���һҳ</a>]");
        }

        printf("[<a href='javascript:SelectAll()'>ѡ��ȫ��</a>]");
        printf("<input class=btn type=submit value=��ת��> �� <input style='height:20px' type=text name=start size=3> ��</form></table>");
        printf("<table><form name=form1 topmargin=0 leftmargin=0 action=bbsman2recycle method=post><tr><td>\n");
        printf("<input class=btn onclick='return confirm(\"ȷʵҪ��ԭ���б�ǵ��ż���\")' type=submit value='��ԭ��ǵ��ż�'>\n");

        //	printf("[������������Ϊ: %dK, Ŀǰ��ʹ��: %dK, �ż�����: %d��] ", max, size, total);
        //printf("<input type=button class=btn onclick=javascript:location.href='bbsmailclear' value=��ջ���վ>\n");
        //printf("<input type=button class=btn onclick=javascript:location.href='bbsmail' value=�ҵ��ռ���>\n");

        printf("</td><td align=right width=70%>\
               <input type=button class=btn onclick=\"if(confirm('ȷ��Ҫ�����?')) location.href='bbsmailclear'\"  value=��ջ���վ>");
        printf("<a href=bbsmail> �ҵ��ռ��� </a>");
        printf("</td></tr></table>\n");

        printf("<table width=610 cellspacing=1 cellpadding=2 bgColor=%s>\n", color0);
        printf("<tr height=20><td align=middle bgColor=%s><font color=#FFFFFF>���<td align=middle bgColor=%s><font color=#FFFFFF>��ԭ<td align=middle bgColor=%s><font color=#FFFFFF>״̬<td align=middle bgColor=%s><font color=#FFFFFF>������<td align=middle bgColor=%s><font color=#FFFFFF>����<td align=middle bgColor=%s><font color=#FFFFFF>�ż�����<td align=middle bgColor=%s><font color=#FFFFFF>����\n", color0, color0, color0, color0, color0, color0, color0);
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
                        printf("�� ");
                hprintf("%42.42s", void1(data[i].title));
                sprintf(filename, "mail/%c/%s/%s",
                        toupper(currentuser.userid[0]), currentuser.userid, data[i].filename);
                printf("</a><td bgColor=%s align=right>%s\n",tablec,  eff_size(filename));
        }
        free(data);
        printf("</table></form><hr color=green width=650>\n");
        http_quit();

}
