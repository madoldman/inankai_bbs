/*$Id: bbssndmail.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "mail.h"
int main()
{
        FILE *fp;
        char userid[80], filename[80], dir[80], title[80], title2[80], buf[80], *content;
        int t, i, sig, backup, mode=0;
        struct fileheader x;
        struct userec *u;
        init_all();
        if(!loginok)
                http_fatal("匆匆过客不能写信，请先登录");
        if(!(currentuser.userlevel & PERM_POST))
                http_fatal("您尚未通过注册，暂无发送信件权限");
        modify_mode(u_info,SMAIL+20000);	//bluetent
        strsncpy(userid, getparm("userid"), 40);
        strsncpy(title, getparm("title"), 50);
        backup=strlen(getparm("backup"));
        int listmail=atoi(getparm("listmail"));
        if(!strcmp(userid,"")&&listmail>=1&&listmail<=9)
                //listmail=1
                ;
        else
                if(!strstr(userid, "@")) 
		{
                        u=getuser(userid);
                        if(u==0)
                        {
			        http_fatal("收信人帐号不存在.");
			}
			if(strcmp(userid,"guest")==0)
			{
				http_fatal("您不能给guest帐号写信.");
			}
                        strcpy(userid, u->userid);
                }
		else
		if(!isvalidEmail(userid))
		{
			http_fatal("收信人帐号不存在.");
			
		}
        for(i=0; i<strlen(title); i++)
                if(title[i]<27 && title[i]>=-1)
                        title[i]=' ';
        sig=atoi(getparm("signature"));
        //mode=atoi(getparm("mode"));
        content=getparm("text");

        if(title[0]==0)
                strcpy(title, "没主题");
        if(listmail)
        	//huangxu@061217:title只有50长，所以加上头也是安全长度之内的
        {
        	sprintf(title2,"[群体信件]%s",title);
        	strcpy(title,title2);
        }	
        sprintf(filename, "tmp/%d.tmp", getpid());
        f_append(filename, content);
        sprintf(title2, "{%s} %s", userid, title);
        title2[70]=0;
        if(backup)
                post_mail(currentuser.userid, title2, filename, currentuser.userid, currentuser.username, fromhost, sig-1,1);
        if(listmail) {//群体信件
                char path[STRLEN];
                if(listmail>1)//huangxu@070425:群信名单
                {
                	sprintf(buf, "maillist%d", listmail);
                }
                else
                {
                	strcpy(buf, "maillist");
                }
                sethomefile(path, currentuser.userid, buf);
                FILE *listfile=fopen(path,"r");
                if(listfile==NULL) {
                        showinfo("不能打开群体信件名单！");
                        http_quit();
                        return 1;
                }
                printf("开始发送....<br>");
                char *bufid,bufid2[16];
                while(1) {
                        bzero(bufid2,16);
                        if(fgets(bufid2, 16, listfile)<=0)
                                break;
                        bufid=trim(bufid2);
                        u=getuser(bufid);
                        if(u==0) {
                                printf("用户%s不存在.<br>",bufid);
                                continue;
                        }
                        strcpy(bufid, u->userid);
                        i = post_mail(bufid, title, filename, currentuser.userid, currentuser.username, fromhost, sig-1,1);
                        if ( i== -3 )
                                printf("[%s]不想收到您的信件,邮件发送失败<br>\n", bufid);
                        else
                                printf("%s<br>",bufid);
                }
                printf("发送完毕.<br>");

        } else {
                i = post_mail(userid, title, filename, currentuser.userid, currentuser.username, fromhost, sig-1,1);
                /* the next line add by yiyo for deny user's mail */
                if ( i== -3 )
                        printf("[%s]不想收到您的信件,邮件发送失败<br>\n", userid);
                // else
                /*add by yiyo for deny user's mail*/
                //if(mode==1) printf("<center>意见或建议发送成功.<br><br>[<a href=javascript:window.close()>关闭本窗口</a>]\n");
                else {
                        printf("信件已寄给%s.<br>\n", userid);
                        char *appendixlist=getparm("attach");
                        char *appendix=strtok(appendixlist,",");
                        struct file_to_appendix  a;
                        char path[STRLEN];
                        if(strstr(userid, "@"))
                                strcpy(userid,""); //互联网信件
                        while( appendix != NULL ) {
                                //bzero(&a,sizeof(struct appendix));
                                strcpy(a.appendixname,appendix);
                                a.filename=i;
                                a.sign=1;
                                strcpy(a.path,userid);
                                sprintf(path,"%s/file_appendix",UPLOAD_PATH);
                                append_record(&a,sizeof(struct appendix),path);
                                appendix = strtok( NULL,",");
                        }

                }

        }

        if(backup)
                printf("信件已经备份.<br>\n");
        unlink(filename);
        showinfo("恭喜,发送成功!<br><a href=bbsmail>返回收件箱</a>");
        http_quit();
}

