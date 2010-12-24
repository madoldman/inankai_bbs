/*$Id: adminfile.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
 */
#include "BBSLIB.inc"
int main()
{
        init_all();


        int mode=0;
        mode =atoi(getparm("mode"));
        char bgbuf[102400];
        char path[STRLEN];
        char buf[512];
        
        char sectitle[80];
        //huangxu@061229
        *sectitle=0;



        if(!loginok) {
                showinfo("您没有登陆！");
                http_quit();
                return 1;
        }
        //huangxu@060715:权限调整
        if((!HAS_PERM(PERM_OBOARDS)&&(mode==1||mode==2||mode==3||mode==11||mode==12||mode==13||mode==18||mode==28))||
        	(!HAS_PERM(PERM_ACCOUNTS)&&(mode==5||mode==15))||
        	(!HAS_PERM(PERM_ANNOUNCE)&&(mode==9||mode==19||mode==29))||
        	(!HAS_PERM(PERM_WELCOME)&&(mode==16||mode==17||mode==26||mode==27)))
        {
                showinfo("您无权进行此项操作！");
                http_quit();
                return 1;
        }

        printf("<body background=%s><font color=%s>",mytheme.bgpath,mytheme.mf);
        if(mode==1) {//推荐讨论区管理

                strcpy(path,"/home/www/html/news/boards");
                readfile(path,bgbuf);
                strcpy(buf,"格式如下：<br>\
                       Pictures 贴图\
                       <br>\
                       每行一个讨论区，英文名在前,中文名在后,中间用空格隔开。 最好不要超过16隔讨论区,请注意看板面是否排的开以调整讨论区顺序");\
\
\
\
\
                out("设定推荐讨论区",buf,bgbuf,mode);
                http_quit();
                return;

        }

        if(mode==2) {//系统公告管理
                strcpy(path,"/home/www/html/news/annouce");
                readfile(path,bgbuf);
                sprintf(buf,"<br>说明:<br>一个公告占一行,请不要自己加入换行符.可以嵌入html代码.请不要超过500个字.\
                        <br>示列:<br>    纯文本的:<br>&nbsp&nbsp\
                        热烈庆祝八周年站庆  <br>\
                        嵌入html代码的:<br>&nbsp&nbsp\
                        &lt;a href=http://202.113.16.117/cgi-bin/beta/bbstcon?board=Announce&file=M.1054020572.A;&gt;本站关于图片签名档的公告&lt;/a&gt;  ") ;

                out("设定系统公告",buf,bgbuf,mode);
                return;

        }


        if(mode==3) {//新开论区管理

                strcpy(path,"/home/www/html/news/newboards");

                readfile(path,bgbuf);

                strcpy(buf,"格式如下：<br>\
                       Pictures 贴图\
                       <br>\
                       每行一个讨论区，英文名在前,中文名在后,中间用空格隔开。请不要超过10个讨论区。");




                out("新开讨论区设定",buf,bgbuf,mode);
                http_quit();
                return;

        }

        /*if(mode==4) {//编辑群体信件名单
                sethomefile(path, currentuser.userid, "maillist");
                readfile(path,bgbuf);
                sprintf(buf,"<br>格式说明:<br>一个id占一行\
                        <br>示列:<br>hello\
                        <br>hehe\
                        ") ;


                out("编辑个人群体信件名单",buf,bgbuf,mode);
                return;

        }*/

        if(mode==5) {//公用IP设置
        	if(!HAS_PERM(PERM_ACCOUNTS))http_fatal("PERM_ACCOUNT ONLY");
                strcpy(path,"/home/bbs/etc/publicip");
                readfile(path,bgbuf);
                sprintf(buf,"<br>说明:<br>一个ip占一行,请不要自己加入换行符.请不要超过100条记录.\
                        <br>示列:<br>202.113.30.0/24</br>");
                out("设定公用IP",buf,bgbuf,mode);
                return;

        }
        if(mode==9) {//删除暂停上传
                strcpy(path, "/home/www/html/news/noupload.html");
                unlink(path);
                strcpy(sectitle,"删除暂停上传");
        }

        if(mode==11) {
                strcpy(path,"/home/www/html/news/boards");
                writefile(path);
                strcpy(sectitle,"编辑推荐讨论区");
        }

        if(mode==12) {
                strcpy(path,"/home/www/html/news/annouce");
                writefile(path);
                strcpy(sectitle,"修改系统公告");
        }

        if(mode==13) {
                strcpy(path,"/home/www/html/news/newboards");
                writefile(path);
                strcpy(sectitle,"修改新开讨论区");
                //这个还有用么？
        }

        /*if(mode==14) {


                sethomefile(path, currentuser.userid, "maillist");
                writefile(path);

        }*/
        if(mode==15) {//huangxu@060520:公用IP设置
                strcpy(path, "/home/bbs/etc/publicip");
                writefile(path);

        }
        
        //huangxu@060419:新首页
        if(mode==16) {//编辑站内公告
                strcpy(path, "/home/www/html/news/bbsannounce");
                readfile(path,bgbuf);
                sprintf(buf,"<br>格式说明:<br>&lt;li&gt;……你的内容……&lt;/li&gt;<br>");
                out("编辑站内公告",buf,bgbuf,mode);
                return;
        }
        if(mode==17) {//编辑校内公告
                strcpy(path, "/home/www/html/news/campusannounce");
                readfile(path,bgbuf);
                sprintf(buf,"<br>格式说明:<br>&lt;li&gt;……你的内容……&lt;/li&gt;<br>");
                out("编辑校内公告",buf,bgbuf,mode);
                return;
        }
        if(mode==18) {//编辑各区区长
                strcpy(path, "etc/zonemaster");
                readfile(path,bgbuf);
                sprintf(buf,"<br>格式说明:<br>每行一个<br>");
                out("编辑校内公告",buf,bgbuf,mode);
                return;
        }
        if(mode==19) {//暂停上传
                strcpy(path, "/home/www/html/news/noupload.html");
                readfile(path,bgbuf);
                sprintf(buf,"<br>格式说明:html页面&nbsp;<input type=button value='删除' onclick=\"location.href='adminfile?mode=9';\" /><br>");
                out("暂停上传附件",buf,bgbuf,mode);
                return;
        }
        if(mode==26) {
                strcpy(path,"/home/www/html/news/bbsannounce");
                writefile(path);
                strcpy(sectitle,"编辑站内公告");
        }
        if(mode==27) {
                strcpy(path,"/home/www/html/news/campusannounce");
                writefile(path);
                strcpy(sectitle,"编辑校内公告");
        }
        if(mode==28) {
                strcpy(path,"etc/zonemaster");
                writefile(path);
                strcpy(sectitle,"编辑区长名单");
        }
        if(mode==29) {
                strcpy(path,"/home/www/html/news/noupload.html");
                writefile(path);
                strcpy(sectitle,"编辑禁止上传");
        }
        printf("</font></body>");
        printf("操作成功");
        if(*sectitle)
        	//huangxu@061229
        {
        	FILE * fp;
 	      	sprintf(path, "tmp/%d.tmp", getpid());
 	      	if(!(fp=fopen(path,"w")))
 	      		http_quit();
 	      	fprintf(fp,"原因：%s\n",sectitle);
        	getuinfo(fp);
        	fclose(fp);
        	post_article("syssecurity1", sectitle, path, currentuser.userid, "自动发信系统", "自动发信系统", -1);
        	unlink(path);
        }
        http_quit();
}





int check(FILE * fp)
{
        if(fp==NULL) {
                printf("can not open file");
                return 0;
        }
        return 1;
}
int readfile(char *path,char *bgbuf)
{

        int fd=0;
        if((fd=open(path,O_RDONLY))==EOF) {
                printf("文件不存在，新建文件");//实际上打不开,假设存在的都能打开
                bzero(bgbuf,102400);
                //huangxu@061229：别w+啊……
                //fopen(path,"w+");
                //http_quit();
        } else {
                bzero(bgbuf,102400);
                read(fd,bgbuf,102400);
                close(fd);
        }

}

int out(char *title,char *inc,char *bgbuf,int mode)
{


        printf("<center>%s -- %s[使用者: %s]<hr color=%s></center>", BBSNAME, title,currentuser.userid,mytheme.bar);


        printf(" <br>\
               <form action=adminfile method=post name=form1>\
               <input type=hidden name=mode value=%d>\
               %s\
               <br><br><textarea   onkeypress='if(event.keyCode==10) return document.form1.submit()' name=text rows=10 cols=100 wrap=off>%s</textarea><br>&nbsp\
               <input type=submit value=ok>\
               <input type=button value=cancel onclick='javascript:history.go(-1)'>\
\
\
               ",mode+10,inc,bgbuf);
}

int writefile(char *path)
{
        char bgbuf[102400];
        strcpy(bgbuf,getparm("text"));
        if(bgbuf!=NULL) {
                FILE *fp=fopen(path,"w+");
                if(check(fp)) {
                        fprintf(fp,"%s",bgbuf);
                        fclose(fp);
                }
        }
        return 0;
}
