/*$Id: bbsedit.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"

struct shortfile * brd;

int main()
{
        FILE *fp;
        int type=0, i, num, l;
        char userid[80], buf[512], path[512], file[512], board[512], title[80]="";
        //struct shortfile *brd;
        struct fileheader  *x;
        char c;//huangxu@060331
        init_all();
        modify_mode(u_info,EDIT+20000);	//bluetent
        if(!loginok)
                http_fatal("匆匆过客不能修改文章，请先登录");
        strsncpy(board, getparm("board"), 20);
        type=atoi(getparm("type"));
        brd=getbcache(board);
        if(brd==0)
                http_fatal("错误的讨论区!");
        strcpy(board, brd->filename);
        strsncpy(file, getparm("file"), 20);
        if(!has_post_perm(&currentuser, board))
                http_fatal("错误的讨论区或者您无权在此讨论区发表文章");
        x=get_file_ent(board, file);
        if(strstr(file, "..") || strstr(file, "/"))
                http_fatal("错误的参数");
        if(x==0)
                http_fatal("错误的参数");
        if(strcmp(x->owner, currentuser.userid))
                if(!has_BM_perm(&currentuser, board))
                        http_fatal("你无权修改此文章");

        printf("<center>%s -- 修改文章 [使用者: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        if(type!=0)
                return update_form(board, file);
        printf("<table border=1>\n");
        printf("<tr><td>");
        printf("<tr><td><form method=post action=bbsedit>\n");
        hsprintf(buf, "%s", x->title);
        printf("使用标题: %s 讨论区: %s<br>\n", buf, board);
        printf("本文作者：%s<br>\n", currentuser.userid);
        sprintf(path, "boards/%s/%s", board, file);
        fp=fopen(path, "r");
        if(fp==0)
                http_fatal("文件丢失");
        fseek(fp,0,2);
        l=ftell(fp);
        fseek(fp,0,0);
        for (i=4;l>0&&i>0;l--)//huangxu@060331
        {
        	c=fgetc(fp);
       		if (c==0x0a)
        		i--;
        }
        if (l<=0)  http_fatal("错误的文件");

        printf("<textarea name=text rows=20 cols=80 wrap=physicle>");
        while(1) {
                if(fgets(buf, 500, fp)==0)
                        break;
                        //为什么这些代码无法工作？
                /*for (;i>0;i--)
                {
                	printf("CHAR::%d::",*(buf+44));
                	pbuf++;
                	pbuf=strchr(pbuf,0x0a);
                	if (!pbuf) 
                	{
                		printf("NOTFOUND%d",i);
                		break;
                		}
                		printf("FOUND:%d,Length:%d,str=%s::",pbuf-buf,strlen(buf),buf);
                }*/
                if(!strcasestr(buf, "</textarea>"))
                        printf("%s", buf);
        }
        fclose(fp);
        printf("</textarea>\n");
        printf("<tr><td class=post align=center>\n");
        printf("<input type=hidden name=type value=1>\n");
        printf("<input type=hidden name=board value=%s>\n", board);
        printf("<input type=hidden name=file value=%s>\n", file);
        printf("<input type=submit value=存盘> \n");
        printf("<input type=button value=放弃编辑 onclick=javascript:history.go(-1)></form>\n");
        printf("</table>");
        http_quit();
}

int update_form(char *board, char *file)
{
        FILE *fp;
        char *buf=getparm("text"), path[80];
        int i,l;//huangxu@060331
        //这里为了安全
        char * pbuf,c;
        char rbuf[512];
        if(!(brd->flag2&NOFILTER_FLAG)&&(bad_words(buf)))
	{
                printf("<script language=\"javascript\">alert('您的文章可能包含不便显示的内容，已经提交审核。请返回并进行修改。');history.go(-1);</script>");
		return 1;
	}
        sprintf(path, "boards/%s/%s", board, file);
        if (!(fp=fopen(path,"r")))
        	http_fatal("错误的文件!");
        fseek(fp,0,2);
        l=ftell(fp);
        fseek(fp,0,0);
        pbuf=rbuf;
        for (i=4;l>0&&i>0;l--)//huangxu@060331
        {
        	*(pbuf++)=c=fgetc(fp);
       		if (c==0x0a)
        		i--;
        }
        fclose(fp);
        if (l<=0)  http_fatal("错误的文件!");
        fp=fopen(path, "w");
        if(fp==0)
                http_fatal("无法存盘");
       	fwrite(rbuf,1,pbuf-rbuf,fp);
        fprintf(fp, "%s", buf);
        //huangxu@060331
				fprintf(fp,
				        "\n[1;36m※ 修改:·%s 于 %16.16s 修改本文·Web[FROM: %-.20s][m\n",
				        currentuser.userid, Ctime(time(0))+ 4, fromhost);
        //fprintf(fp, "\n※ 修改:·%s 於 %s 修改本文·%s Web[FROM: %s] ", currentuser.userid, Ctime(time(0))+4,BBSHOST, (!(currentuser.userdefine & DEF_NOTHIDEIP)&&isdormip(fromhost))?"南开宿舍内网":fromhost);
        fclose(fp);
        printf("修改文章成功.<br><a href=bbsdoc?board=%s>返回本讨论区</a>", board);
}
