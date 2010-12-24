/*$Id: bbsmytheme.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
 */
#include"BBSLIB.inc"
#define DOCROOT "/home/www/html"

int main()
{
        init_all();
        if(!loginok) {
                printf("对不起,您还没有登陆,不能进行风格设定");
                http_quit();
        }

        char buf[128];
        int err=0;
        FILE *fp;
        struct theme tmptheme;
        struct themename {
                char en[16];
                char cn[16];
        }
        tmpname;

        printf("<script src=/js/common.js></script>");

        int mode =atoi( getparm("mode"));
        if(mode==0) {

                printf("\
                       <center>%s --  我的风格设定 [使用者: %s]", BBSNAME, currentuser.userid);
                printf("<br><br>");


                printf("</center>本站倾情为您打造非凡个性体验<center>");
                sprintf(buf,"%s/theme/.DIR",DOCROOT);
                if(!file_exist(buf)) {
                        printf("系统风格库为空");
                        http_quit();
                }
                fp=fopen(buf,"r");
                if(fp==NULL) {
                        printf("ERROR,can not open .DIR");
                        http_quit();
                }
                int i=0;
                while(fread(&tmpname,sizeof(struct themename),1,fp)==1) {
                        i++;
                        printf("<br><a href=bbsmytheme?mode=3&&theme=%s>%s</a>",tmpname.en,tmpname.cn);

                }
                fclose(fp);
                if(i!=0)
                        printf("</center><br><a href=bbsmytheme?mode=4>恢复默认设置</a>");
                printf("<br><font color=green>如果有一天,当您登陆之后,没有看到您熟悉的风格,很可能是因为我们更新了系统风格库,您的自选风格已经不存在了,就请您应用更好的吧!");


                int n=showshare();


                printf("</font><br><br><br>更具个性化？你也可以定义自己的风格，充分展现您的超凡个性！<br>\
                       <font color=ff0000>注意:如果 您对下列各项不是很了解的话，建议您使用系统设置。颜色都是用六位的RGB表示。 </font>");


                char bgbuf[2056],path[STRLEN];
                struct theme tmptheme;
                sprintf(buf,"%s/home/%c/%s/.theme/style",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
                sprintf(path,"%s/home/%c/%s/.theme/css",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);


                if(file_exist(buf)) {

                        printf("<p><b><i>您以前的设定:</i></b></p>");
                        if(!n)
                                printf("<a href=bbsmytheme?mode=6>共享我的设定</a></p>");
                        else
                                printf("<a href=bbsmytheme?mode=7>取消共享</a> 注意:该操作将影响他人的使用.</p>");

                        FILE * fp=fopen(buf,"r");
                        if(fp==NULL) {

                                printf("%s: can not open.",buf);
                                http_quit();
                        }
                        fread(&tmptheme,sizeof(struct theme),1,fp);
                        fclose(fp);



                        int fd=0;
                        if((fd=open(path,O_RDONLY))==EOF) {
                                printf("css file missing.");
                                http_quit();
                        } else {
                                bzero(bgbuf,2056);
                                read(fd,bgbuf,2056);
                                close(fd);

                        }

                } else {
                        bzero(&tmptheme,sizeof(struct theme));
                        bzero(bgbuf,2056);
                }
                printf("\
                       \
                       <form action=bbsmytheme method=post name=theme>\
                       <input type=hidden name=mode value=1>\
                       \
                       \
                       导航条颜色  <input name=nav size=16 maxlength=6 value=%s ><br>\
                       主窗体背景色  <input name=bg size=16 maxlength=6   value=%s ><br>\
                       左侧字体 颜色  <input name=lf size=16 maxlength=6 value=%s  ><br>\
                       主窗体字体颜色  <input name=mf size=16 maxlength=6 value=%s  ><br>\
                       重色   <input name=bar size=16 maxlength=6 value=%s  ><br>\
                       浅色    <input name=con size=16  maxlength=6 value=%s ><br>\
                       背景图片<input name=bgpath size=80 maxlength=80  value=%s >请输入有效的图片链接，相对路径绝对路径均可<br>\
\
                       左侧背景图片  <input name=lbg maxlength=40  size=40 value=%s>请填入有效的图片路径,最多40个字符<br>\
                       css文件<br><br><textarea   onkeypress='if(event.keyCode==10) return document.theme.submit()' name=css rows=20 cols=80 wrap=physicle>%s</textarea><br>&nbsp\
                       <input type=submit value=应用我的自设风格>\
                       <input type=reset onclick='javascript:history.go(-1)' value=返回>\
\
\
                       </form> ",tmptheme.nav,tmptheme.bg,tmptheme.lf,tmptheme.mf,tmptheme.bar,tmptheme.con,tmptheme.bgpath,tmptheme.lbg,bgbuf);

                http_quit();
        }

        if(mode==1) {//用户设定自己的风格

                strcpy(tmptheme.nav,getparm("nav"));
                strcpy(tmptheme.bg,getparm("bg"));
                strcpy(tmptheme.bar,getparm("bar"));
                strcpy(tmptheme.con,getparm("con"));
                strcpy(tmptheme.bgpath,getparm("bgpath"));
                strcpy(tmptheme.lf,getparm("lf"));
                strcpy(tmptheme.mf,getparm("mf"));
                strcpy(tmptheme.lbg,getparm("lbg"));





                sprintf(buf,"home/%c/%s/.theme",toupper(currentuser.userid[0]),currentuser.userid);
                savethemepath(buf);
                char path[STRLEN];
                sprintf(path,"%s/style",buf);
                fp=fopen(path,"w+");
                sprintf(path,"%s/css",buf);
                strcpy(tmptheme.css,path);
                fwrite(&tmptheme,sizeof(struct theme),1,fp);
                fclose(fp);

                strcpy(mytheme.en,"");
                strcpy(mytheme.cn,"");
                strcpy(mytheme.nav,tmptheme.nav);
                strcpy(mytheme.bg,tmptheme.bg);
                strcpy(mytheme.bar,tmptheme.bar);
                strcpy(mytheme.con,tmptheme.con);
                strcpy(mytheme.bgpath,tmptheme.bgpath);
                strcpy(mytheme.lf,tmptheme.lf);
                strcpy(mytheme.mf,tmptheme.mf);
                strcpy(mytheme.css,tmptheme.css);
                strcpy(mytheme.lbg,tmptheme.lbg);

                char bgbuf[2056];
                strcpy(bgbuf,getparm("css"));

                fp=fopen(path,"w+");
                if(fp==NULL) {

                        printf("can not creat file : %s  ",path);
                        http_quit();
                }
                fprintf(fp,"%s",bgbuf);
                fclose(fp);

                printf("\
                       <script language=javascript>\
                       refreshall();\
                       </script>");

                redirect("bbsmytheme");
        }



        if(mode==4||mode==5) {//恢复默认设定
                if(mode==4)
                        sprintf(buf,"%s/theme/default",DOCROOT);
                else {
                        char id[IDLEN+1];
                        strncpy(id,getparm("id"),IDLEN);
                        if(getuser(id))
                                sprintf(buf,"home/%c/%s/.theme",toupper(id[0]),id);
                }
                savethemepath(buf);
                printf("\
                       <script language=javascript>\
                       refreshall();\
                       </script>");

                redirect("bbsmytheme");
                return 0;
        }

        if(mode==6) {

                char buf[STRLEN];
                sprintf(buf,"%s/theme/share",DOCROOT);
                FILE *fp=fopen(buf,"a+");
                if(fp==0)
                        return 0;
                fprintf(fp,"%s\n",currentuser.userid);
                fclose(fp);
                redirect("bbsmytheme");
                return 0;

        }

        if(mode==7) {
                char tmp[STRLEN];
                sprintf(buf,"%s/theme/share",DOCROOT);
                sprintf(tmp,"%s/theme/share.tmp",DOCROOT);
                FILE *fp=fopen(buf,"r");
                FILE *ftmp=fopen(tmp,"w+");
                if(fp==0 || ftmp==0) {
                        showinfo("文件读取失败!");
                        return -1;
                }
                while(1) {


                        if(fgets(buf,STRLEN,fp)<=0)
                                break;
                        buf[strlen(buf)-1]='\0';
                        if(!strcmp(buf,currentuser.userid))
                                continue;
                        fprintf(ftmp,"%s\n",buf);

                }
                fcloseall();
                char cmd[256];
                sprintf(cmd,"mv -f %s %s/theme/share ",tmp,DOCROOT);
                //printf("%s",cmd);
                system(cmd);
                redirect("bbsmytheme");
                return 0;
        }



        if(mode==3) {

                char theme[16];
                strcpy(theme,getparm("theme"));
                sprintf(buf,"%s/theme/%s/style",DOCROOT,theme);

                fp=fopen(buf,"r");
                int size=fread(&mytheme,sizeof(struct theme),1,fp);
                fclose(fp);
                printf("\
                       %s<br>\
                       hello  %s<br>\
                       %s<br>    %s<br>    %s<br>    %s<br>\
                       %d<br>%s\
                       ",theme,mytheme.en,mytheme.cn,mytheme.nav,mytheme.bar,mytheme.bg,size,mytheme.css);


                sprintf(buf,"%s/theme/%s",DOCROOT,theme);
                savethemepath(buf);
                printf("\
                       <script language=javascript>\
                       refreshall();\
                       </script>\
\
\
\
                       ");
                redirect("bbsmytheme");
        }

        printf("错误的模式!");
}

int savethemepath(char *path)
{
        char buf[128];
        sprintf(buf,"%s/home/%c/%s/.theme",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
        if(!file_exist(buf))
                mkdir(buf,0770);


        sprintf(buf,"%s/home/%c/%s/.theme/themepath",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
        FILE * fp=fopen(buf,"w+");
        fprintf(fp,"%s",path);
        fclose(fp);
        return 0;
}

int showshare()
{
        char buf[STRLEN];
        sprintf(buf,"%s/theme/share",DOCROOT);
        FILE *fp=fopen(buf,"r");
        if(fp==0)
                return 0;
        int i=0,n=0;
        printf("<p>您的BBS风格如何?赶快拿出来秀一下与大家一起分享吧!<br>网友共享风格:<br></p>");
        while(1) {
                if(fgets(buf,STRLEN,fp)<=0)
                        break;
                buf[strlen(buf)-1]='\0';
                if(!strcmp(buf,currentuser.userid))
                        n=1;
                i++;
                printf("<a href=bbsmytheme?id=%s&mode=5>  %s  </a>",buf,buf);
                if(i%10==0)
                        printf("<br>");
        }
        return n;
}
