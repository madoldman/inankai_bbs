/*$Id: digestpst.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
 */
#include "BBSLIB.inc"
#include "digest.h"
int main()
{
        init_all();
        if(!loginok) {
                showinfo("���ǹ��ͣ����ܷ��ģ���ӭ����뱾վ����վ��Ϊ���ṩ�������");
                return 0;
        }
        modify_mode(u_info,POSTING+20000);	//bluetent

        int mode=atoi(getparm("mode"));
        //mode=1 �������������£�mode��2����������
        if(mode<=0 || mode>3) {
                showinfo("��������");
                return 0;
        }
        char path[PATHLEN];
        strsncpy(path, getparm("path"),PATHLEN);

        char buf[PATHLEN];
        if (strstr (path, "..") || strstr (path, "SYSHome")) {
                showinfo ("��Ŀ¼������");
                return 0;
        }
        sprintf (buf, "0Announce%s/.Names", path);
        if (!file_exist (buf)) {
                showinfo ("��Ŀ¼������");
                return 0;
        }
        char s[20];
        int  r = getfrompath (path, s);

        if (mode==1&&!hasperm (r, s)) {
                showinfo ("����Ȩ�ڴ˷���!");
                return 0;
        }
        if(mode==2&&!isguestbook(path)) {
                showinfo ("����Ȩ�ڴ˷���!");
                return 0;
        }
        if(mode==3) {

                denyrobot();

                int guestbook=atoi(getparm("guestbook"));
                if(guestbook) {
                        if(!isguestbook(path)) {
                                showinfo ("����Ȩ�ڴ˷���!");
                                return 0;
                        }
                } else
                        if (!hasperm (r, s)) {
                                showinfo ("����Ȩ�ڴ˷���!");
                                return 0;
                        }
                char title[40];
                strsncpy(title, getparm("title"), 32);
                if(title[0]==0) {
                        showinfo("���±���Ҫ�б���");
                        return 1;
                }
                int i;
                for(i=0; i<strlen(title); i++)
                        if(title[i]<=27 && title[i]>=-1)
                                title[i]=' ';
                int sig=atoi(getparm("signature"));
                char *content=getparm("text");

                int len=strlen(content);
                if(len>40960){
                       showinfo("����̫�������");
                       return 0;
                } 

                char filename[16];
                sprintf(filename, "tmp/%d.tmp", getpid());
                f_append(filename, content);
                if(digestpost(path,title,filename, currentuser.userid, currentuser.username,sig ,r,s))
                        showinfo("�����ڲ�����");
                else
                        showinfo("<a href=bbs0an?path=%s>��ϲ�����·��ͳɹ���</a>",path);
                unlink(filename);
                return 0;

        }




        printf("<body background=%s  bgproperties=fixed>",mytheme.bgpath);
        printf("<center>\n");
        if(r==2)
                printf("%s -- �������� [ %s �ĸ����ļ�]<hr color=green>", BBSNAME, s);
        else
                printf("%s -- �������������� [ʹ����: %s]<hr color=green>", BBSNAME, currentuser.userid);
        printf("<table border=0  bgcolor=%s >\n",mytheme.con);
        printf("<tr><td>");
        printf("�Ա�վ�ķ�չ����ϵͳ������ʲô���������?<a href=bbsdoc?board=Advice>��ӭ����Advice�����!</a>\n");
        printf("<br><font color=green>��ʼ���Ĵ�����,������Ŀ�Դ�!</font><a href=bbsdoc?board=GoodArticle> ��ӭ��Ʒԭ�� <a/><a href=bbsdoc?board=BBSHelp> ���ʹ�ñ�վ </a> <a href=bbsdoc?board=sysop> ������վ��</a> ");

        //printf("<font color=green><br>\n");
        //printf("�����Ը�!�����ؿ������������Ƿ��ʺ��ڹ��ڳ��Ϸ������������ˮ.<br>\n</font>");
        printf("<tr><td><form name=form1  method=post action=digestpst?mode=3&path=%s>\n",path);
        if(mode==2)
                printf("<input type=hidden  name=guestbook value=1>");
        printf("ʹ�ñ���:<input type=text  name=title size=32 maxlength=32>(32���ַ�)");
        printf("<br>���ߣ�%s", currentuser.userid);
        printf("  ʹ��ǩ���� ");
        printf("<input type=radio name=signature value=1 checked>1");
        printf("<input type=radio name=signature value=2>2");
        printf("<input type=radio name=signature value=3>3");
        printf("<input type=radio name=signature value=4>4");
        printf("<input type=radio name=signature value=5>5");
        printf("<input type=radio name=signature value=0>0");
        printf(" <a target=_blank href=bbssig>�鿴ǩ����</a> ");
        printf("<br><input type=submit class=btn value=��������> ");
        printf("<input type=button class=btn value=�����༭ onclick='javascript:history.go(-1)'>");

        printf("<br><br>\n<textarea   onkeypress='if(event.keyCode==10) return document.form1.submit()' name=text rows=20 cols=78  wrap=hard>\n\n");
        printf("</textarea></form>\n");

        printf("</table>\
               <script language=javascript>\
               document.form1.title.focus();\
               </script>\
               </body>");
        printf("  <br> <font color=336699 >��ʾ: �ڴ��������¿�ʹ��Ctrl+Enter�����ٷ�������.</font><br>");
        http_quit();
}

