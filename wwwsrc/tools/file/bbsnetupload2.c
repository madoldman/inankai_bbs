/*$Id: bbsnetupload2.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
static struct dir x;

int main()
{
        char file[80], name[80], fileurl[150];
        int i, level, pid;
        int maxsize=102400;//允许的上传附件大小
        //	int currexp=0;//用户的经验值
        //	currexp=countexp(&currentuser);
        init_all();
        if(!loginok)
                http_fatal("匆匆过客无法执行本操作，请先登录");
        if(currentuser.stay/3600<100)
                http_fatal("上站累计时数未满100小时，无权使用本功能");
        if((time(0)-currentuser.firstlogin)/86400<60)
                http_fatal("注册帐号时间未超过60天，无权使用本功能");
        x.active=1;
        x.level=atoi(getparm("level"));
        strsncpy(x.board, getparm("userid"), 20);
        strsncpy(x.exp, getparm("exp"), 72);
        strsncpy(x.userid, currentuser.userid, 13);
        strsncpy(x.showname, getparm("name"), 36);
        do_check(x.showname);
        x.date=time(0);
        strsncpy(x.type, getparm("type"), 30);
        x.live=30;
        for(i=0; x.showname[i]; i++) {
                int c=x.showname[i];
                if(c<-1)
                        continue;
                if(isalnum(c) || c=='.' || c=='-')
                        continue;
                x.showname[i]='_';
        }
        //	level=atoi(getparm("level"));
        pid=atoi(getparm("file"));
        sprintf(file, "tmp/%d.netsave", pid);
        strsncpy(name, getparm("name"), 30);
        //	if(!has_post_perm(&currentuser, x.board)) http_fatal("错误的讨论区或无权上载至本讨论区");
        if(!file_exist(file)) {
                http_fatal("错误的文件名");
        }
        x.size=file_size(file);
        //	if(x.size<=0)http_fatal("您没有上传任何内容，请返回");
        {
                char buf[256];
                sprintf(buf, "%16.16s %s %d %s %s %s\n",
                        Ctime(time(0))+4, currentuser.userid, x.size, fromhost, x.board, x.showname);
                f_append("netsave.log", buf);
        }
        /*	if(x.size>maxsize) {
        		unlink(file);
        		http_fatal("文件太大");
        	}*/
        upload(file, &x);
        unlink(file);
        sprintf(fileurl, "http://%s/cgi-bin/bbs/netshowfile?name=%s",BBSHOST, x.showname);//bluetent
        printf("附件上载成功, ");
        /*	printf("URL为 <font color=green>%s </font><br>\n", fileurl);
        	printf("本URL已经自动追加到正在发表的文章末尾.");
        	printf("(以上URL(绿色部分)可拷贝至文章中直接引用, 系统会判断类型自动生成超链接.)<br>\n");
        	printf("如不拷贝, 则只能在板面的文章上载区看到本附件.<hr color=green>\n");
        	printf("评定等级: %d星<br>\n", x.level+1);
        	printf("保留时间: %d天<br>\n", x.live);
        */	
        printf("文件大小: %d字节<br>\n", x.size);
        printf("文件类型: %s<br>\n", x.type);
        printf("文件名称: %s<br>\n", x.showname);
        printf("上载人ID: %s<br>\n", x.userid);
        printf("上载时间: %s<br>\n", Ctime(time(0))+4);
        //	printf("上载板面: %s<br>\n", x.board);
        printf("文件说明: %s<br>\n", nohtml(x.exp));
        //	printf("<hr color=green><a href='javascript:window.close()'>关闭此窗口</a>\n");
        printf("<hr color=green><a href=bbsnetfdoc>返回网络存储选单</a>\n");
        //add by bluetent 2002.12.22
        /*	strsncpy(closewindow, getparm("closewindow"), 10);
                if (!strcmp("yes", closewindow))
                        printf("
        				<script language=javascript>
        		        opener.document.bbspstform.text.value += ' %s ';
        				</script>
                        ", fileurl);
         
        	//add end*/
        http_quit();
}

int badfile(char *filename)
{
        int i;
        for(i=0; filename[i]; i++)
                if(!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", filename[i]))
                        return 1;
        return 0;
}

int upload(char *file, struct dir *x)
{
        FILE *fp;
        int t=time(0);
        char buf[256], dir[256];
        x->filename=t;
        sprintf(buf, NETSAVE_PATH"/%d", t);
        link(file, buf);
        sprintf(dir, NETSAVE_PATH"/.DIR");
        fp=fopen(dir, "a");
        fwrite(x, sizeof(struct dir), 1, fp);
        fclose(fp);
        return 0;
}

int do_check(char *show)
{
        FILE *fp;
        struct dir x;
        show[30]=0;
        fp=fopen(NETSAVE_PATH"/.DIR", "r");
        if(fp==0)
                return;
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                if(!strcmp(x.showname, show)) {
                        sprintf(show, "%d%s", time(0), x.showname);
                        fclose(fp);
                        return;
                }
        }
        fclose(fp);
}
