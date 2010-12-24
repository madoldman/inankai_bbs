/*$Id: digestpst.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
 */
#include "BBSLIB.inc"
#include "digest.h"
int main()
{
        init_all();
        if(!loginok) {
                showinfo("您是过客，不能发文，欢迎你加入本站！本站将为您提供更多服务。");
                return 0;
        }
        modify_mode(u_info,POSTING+20000);	//bluetent

        int mode=atoi(getparm("mode"));
        //mode=1 精华区发表文章，mode＝2，过客留言
        if(mode<=0 || mode>3) {
                showinfo("参数错误。");
                return 0;
        }
        char path[PATHLEN];
        strsncpy(path, getparm("path"),PATHLEN);

        char buf[PATHLEN];
        if (strstr (path, "..") || strstr (path, "SYSHome")) {
                showinfo ("此目录不存在");
                return 0;
        }
        sprintf (buf, "0Announce%s/.Names", path);
        if (!file_exist (buf)) {
                showinfo ("此目录不存在");
                return 0;
        }
        char s[20];
        int  r = getfrompath (path, s);

        if (mode==1&&!hasperm (r, s)) {
                showinfo ("你无权在此发文!");
                return 0;
        }
        if(mode==2&&!isguestbook(path)) {
                showinfo ("你无权在此发文!");
                return 0;
        }
        if(mode==3) {

                denyrobot();

                int guestbook=atoi(getparm("guestbook"));
                if(guestbook) {
                        if(!isguestbook(path)) {
                                showinfo ("你无权在此发文!");
                                return 0;
                        }
                } else
                        if (!hasperm (r, s)) {
                                showinfo ("你无权在此发文!");
                                return 0;
                        }
                char title[40];
                strsncpy(title, getparm("title"), 32);
                if(title[0]==0) {
                        showinfo("文章必须要有标题");
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
                       showinfo("文章太长，溢出");
                       return 0;
                } 

                char filename[16];
                sprintf(filename, "tmp/%d.tmp", getpid());
                f_append(filename, content);
                if(digestpost(path,title,filename, currentuser.userid, currentuser.username,sig ,r,s))
                        showinfo("发文内部错误。");
                else
                        showinfo("<a href=bbs0an?path=%s>恭喜，文章发送成功！</a>",path);
                unlink(filename);
                return 0;

        }




        printf("<body background=%s  bgproperties=fixed>",mytheme.bgpath);
        printf("<center>\n");
        if(r==2)
                printf("%s -- 发表文章 [ %s 的个人文集]<hr color=green>", BBSNAME, s);
        else
                printf("%s -- 精华区发表文章 [使用者: %s]<hr color=green>", BBSNAME, currentuser.userid);
        printf("<table border=0  bgcolor=%s >\n",mytheme.con);
        printf("<tr><td>");
        printf("对本站的发展或者系统功能有什么意见建议吗?<a href=bbsdoc?board=Advice>欢迎您到Advice版提出!</a>\n");
        printf("<br><font color=green>开始您的大作吧,我们拭目以待!</font><a href=bbsdoc?board=GoodArticle> 欢迎精品原创 <a/><a href=bbsdoc?board=BBSHelp> 如何使用本站 </a> <a href=bbsdoc?board=sysop> 有事找站长</a> ");

        //printf("<font color=green><br>\n");
        //printf("文责自负!请慎重考虑文章内容是否适合在公众场合发表，请勿肆意灌水.<br>\n</font>");
        printf("<tr><td><form name=form1  method=post action=digestpst?mode=3&path=%s>\n",path);
        if(mode==2)
                printf("<input type=hidden  name=guestbook value=1>");
        printf("使用标题:<input type=text  name=title size=32 maxlength=32>(32个字符)");
        printf("<br>作者：%s", currentuser.userid);
        printf("  使用签名档 ");
        printf("<input type=radio name=signature value=1 checked>1");
        printf("<input type=radio name=signature value=2>2");
        printf("<input type=radio name=signature value=3>3");
        printf("<input type=radio name=signature value=4>4");
        printf("<input type=radio name=signature value=5>5");
        printf("<input type=radio name=signature value=0>0");
        printf(" <a target=_blank href=bbssig>查看签名档</a> ");
        printf("<br><input type=submit class=btn value=立即发表> ");
        printf("<input type=button class=btn value=放弃编辑 onclick='javascript:history.go(-1)'>");

        printf("<br><br>\n<textarea   onkeypress='if(event.keyCode==10) return document.form1.submit()' name=text rows=20 cols=78  wrap=hard>\n\n");
        printf("</textarea></form>\n");

        printf("</table>\
               <script language=javascript>\
               document.form1.title.focus();\
               </script>\
               </body>");
        printf("  <br> <font color=336699 >提示: 在大多浏览器下可使用Ctrl+Enter键快速发表文章.</font><br>");
        http_quit();
}

