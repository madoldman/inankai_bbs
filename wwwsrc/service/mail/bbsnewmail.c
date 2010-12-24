/*$Id: bbsnewmail.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "mail.h"

int main()
{
        FILE *fp;
        struct fileheader x;
        int filetime, i, unread=0, type;
        char *ptr, buf[512], path[80], dir[80];
	char userid_ex[STRLEN];
        init_all();
        if(!loginok)
                http_fatal("您尚未登录, 请先登录");
        modify_mode(u_info,MAIL+20000);	//bluetent
        printf("<center>\n");
        printf("%s -- 新邮件列表 [使用者: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        sprintf(dir, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]), currentuser.userid);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("目前您的信箱没有任何信件");
        i=0;
        int  total=file_size(dir)/sizeof(x);
        if(total>20)
                i=total-20;
        fseek(fp, sizeof(x)*i, SEEK_SET);

        printf("<table width=610 border=1>\n");
        printf("<tr><td>序号<td>状态<td>发信人<td>日期<td>信件标题\n");
        while(1) {
                i++;
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                if(x.accessed[0] & FILE_READ)
                        continue;
                printf("<tr><td>%d<td>N", i);
                /*modify by brew */
		/* ptr=strtok(x.owner, " (");
                if(ptr==0)
                        ptr=" ";
                ptr=nohtml(ptr);
		*/
		ptr=x.owner;
                getsenderex(ptr,userid_ex);
                 if(strstr(userid_ex,"@")!=NULL){
       /*letter from internet*/
                 printf("<td><a href=bbsmailcon?file=%s&num=%d&mode=0>%s</a>",  x.filename, i-1,nohtmlex(ptr,15));
         }else{
            printf("<td ><a href=bbsqry?userid=%s >%s</a>", userid_ex, nohtmlex(ptr,15));
   /*letter from our site*/
        }
              //printf("<td><a href=bbsqry?userid=%s>%13.13s</a>", ptr, ptr);
                printf("<td>%6.6s", Ctime(atoi(x.filename+2))+4);
                printf("<td><a href=bbsmailcon?file=%s&num=%d>", x.filename,i-1);
                if(strncmp("Re: ", x.title, 4))
                        printf("★ ");
                hprintf("%42.42s", void1(x.title));
                printf(" </a>\n");
                unread++;
        }
        printf("</table><hr>\n");
        printf("您的信箱共有%d封信件, 其中新信%d封.我们只检查了信件列表的最后20封.", total, unread);
        http_quit();
}
