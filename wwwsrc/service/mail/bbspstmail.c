/*$Id: bbspstmail.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
#include <string.h>

int main()
{
        FILE *fp;
        int i;

        char userid[80], buf[512], path[512], file[512], board[512], title[80]="";
        init_all();
        if(!loginok)
                http_fatal("匆匆过客不能写信，请先登录");
        if(!(currentuser.userlevel & PERM_POST))
                http_fatal("您尚未通过注册，暂无发送信件权限");
        modify_mode(u_info,SMAIL+20000);	//bluetent
        strsncpy(file, getparm("file"), 20);
        strsncpy(title, nohtml(getparm("title")), 50);
        strsncpy(userid, getparm("userid"), 40);
        strsncpy(board, getparm("board"), 20);//ibaif@taida
        if(file[0]!='M' && file[0])
                http_fatal("错误的文件名");
        int mode =atoi(getparm("mode"));//0为单个信件，1为群体信件

        printf("<center>\n");

        if(mode==0)
                printf("%s -- 寄语信鸽 [使用者: %s]\n<br>", BBSNAME, currentuser.userid);
        else
                printf("%s -- 发送群体信件 [使用者: %s]\n<br>", BBSNAME, currentuser.userid);

        printf("<table  cellspacing=1 cellpadding=2>");
        printf("<tr><td bgcolor=eeeee><br>");
        //	printf("<form name=bbsmailform  method=post action=bbssndmail?userid=%s>\n", userid);
        //   	printf("信件标题: <input type=text name=title size=40 maxlength=46  value='%s'> ", title);
        //   	printf("发信人: &nbsp;%s<br>\n", currentuser.userid);
        printf("<form name=bbsmailform  method=post action=bbssndmail?userid=%s><br>", userid);
        
        if(mode==0)
        {
                if(strlen(board)>0)
                        printf("收信人&nbsp;&nbsp;：&nbsp;%s\n<input type=hidden name=userid value='%s'><br>", nohtml(userid), nohtml(userid));
                else
                        printf("收信人：&nbsp;&nbsp;<input type=text name=userid value='%s'><br>", nohtml(userid));
        }
        else//huangxu@070425:群信名单
        {
        	printf("选择群信名单,点序号直接编辑:&nbsp;");
        	for(i = 1; i <= 9; i++)
        	{
          	printf("<input type=radio name=listmail value=%d %s><a href=\"/js/userlist.htm?file1=1&file2=%d\" target=_blank>%d</a>&nbsp;", i, i==1?"checked":"", i, i);
          }
          printf("<br>\n");
        }


        
       if(strlen(board)>0) {
            printf("信件标题：Re:%s", title);
            printf("<input type=hidden name=title size=10 value='Re:%s'><br>", title);
       } else
            printf("信件标题: <input type=text name=title size=40 maxlength=46  value='%s'><br>", title);


        printf("使用签名档 ");
        printf("<input type=radio name=signature value=1 checked>1\n");
        printf("<input type=radio name=signature value=2>2\n");
        printf("<input type=radio name=signature value=3>3\n");
        printf("<input type=radio name=signature value=4>4\n");
        printf("<input type=radio name=signature value=5>5\n");
        printf("<input type=radio name=signature value=0>0\n");
        printf(" [<a target=_blank href=bbssig>查看签名档</a>] ");
        printf("备份<input type=checkbox name=backup><br>\n");
        printf("<table width=35% border=0><tr><td>");
        if(mode==0) {
                printf("<iframe scrolling=\"no\" src=\"bbsupload?board=%s\" height=\"40\" width=\"100%\" ></iframe>",board);
                printf("<input type=hidden name=attach value=>");
        }
        printf("</td></tr><tr><td>");
        printf("<input type=submit  class=btn onclick=\"this.value='请稍候...';this.disabled=true;bbsmailform.submit();\" value='立即发送'> ");
        printf("<input type=reset class=btn  value=我要重写>\n");
        printf("</td></tr><tr><td>");
        printf("\n<textarea   onkeypress='if(event.keyCode==10) return document.bbsmailform.submit()'onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.bbsmailform.submit(); return false;}' name=text rows=18 cols=78 wrap=hard> \n");
        if(file[0]) {
                int lines=0;
                //baif
                if(strlen(board)>0) {
                        printf("【 在 %s 的发贴中提到: 】\n", userid);
                        sprintf(path, "boards/%s/%s", board, file);
                } else {
                        printf("【 在 %s 的来信中提到: 】\n", userid);
                        sprintf(path, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, file);
                }
                fp=fopen(path, "r");
                if(fp) {
                        for(i=0; i<4; i++)
                                if(fgets(buf, 500, fp)==0)
                                        break;
                        while(1) {
                                bzero(buf,512);
                                if(fgets(buf, 500, fp)==0)
                                        break;
                                if(!strncmp(buf, ": 【", 4))
                                        continue;
                                if(!strncmp(buf, ": : ", 4))
                                        continue;
                                if(!strncmp(buf, "--\n", 3))
                                        break;
                                if(buf[0]=='\n')
                                        continue;
                                ;
                                if(!strcasestr(buf, "</textarea>"))
                                        printf(": %s", buf);
                                if(lines++>20) {
                                        printf(": (以下引言省略 ... ...)");
                                        break;
                                }
                        }
                        fprintf(fp, "\n");
                        fclose(fp);
                }
        }
        printf("</textarea></td></tr></form>");

        printf("<center><font color=336699>在大多数浏览器下,您写完信件后,可以按Ctrl+Enter或Ctrl+w快速发送</font><br></center>");
        http_quit();
}

