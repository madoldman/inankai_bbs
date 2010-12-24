/*$Id: bbstheme.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
*/
#include"BBSLIB.inc"
struct themename
{
        char en[16];
        char cn[16];
}
tmpname;

int main()
{
        init_all();
        if(file_has_word("/home/bbs/.designer",currentuser.userid)!=1) {
                showinfo("对不起,仅有系统维护和美工设计人员可以访问本页面.");
                return 1;
        }
        int mode =atoi( getparm("mode"));

        if(mode==0) {

                printf("\
                       <center>%s -- 系统风格设置管理 [使用者: %s]<hr color=green></center>", BBSNAME, currentuser.userid);
                printf("\
                       添加:\
                       <form action=bbstheme method=post name=theme>\
                       <input type=hidden name=mode value=1>\
                       <input type=hidden name=css >\
                       英文名字  <input name=en maxlength= 16 size=12 > 12个字符<br>\
                       中文名字  <input name=cn maxlength=16  size=16 ><br>\
\
                       导航条颜色  <input name=nav size=16 maxlength=6 ><br>\
                       主窗体背景色  <input name=bg size=16 maxlength=6 ><br>\
                       左侧字体 颜色  <input name=lf size=16 maxlength=6 ><br>\
                       主窗体字体颜色  <input name=mf size=16 maxlength=6 ><br>\
                       重色   <input name=bar size=16 maxlength=6 ><br>\
                       浅色    <input name=con size=16  maxlength=6><br>\
                       主窗体背景图片<input name=bgpath maxlength=80  size=60>请填入有效的图片路径<br>\
                       左侧背景图片  <input name=lbg maxlength=40  size=40>请填入有效的图片路径,最多40个字符<br>\
                       css文件&nbsp&nbsp大小不超过2K<br><br><textarea   onkeypress='if(event.keyCode==10) return document.theme.submit()' name=css rows=20 cols=80 wrap=physicle>\
                       </textarea>\
\
\
                       <br>&nbsp\
                       <input type=submit value=可以啦>\
                       <input type=reset onclick='javascript:history.go(-1)'  value=返回>\
\
\
                       </form>	");

                printf("<br>现有风格<br>");
                char buf[128];
                sprintf(buf,"%s/theme/.DIR",DOCROOT);


                if(!file_exist(buf)) {
                        printf("系统风格库为空");
                        http_quit();
                }
                FILE *fp=fopen(buf,"rb");
                if(fp==NULL) {
                        printf("ERROR,can not open .DIR");
                        http_quit();
                }
                int i=0;
                while(fread(&tmpname,sizeof(struct themename),1,fp)==1) {

                        printf("<br>%s<a href=bbstheme?mode=2&&theme=%s>&nbsp&nbsp删除</a>",tmpname.cn,tmpname.en);

                        printf("<a href=bbstheme?mode=3&&theme=%s>&nbsp&nbsp修改</a>",tmpname.en);

                        i++;
                }
                printf("<br>共%d种",i);
                fclose(fp);
                http_quit();
        }


        if(mode==1) {
                addtheme();
                redirect("bbstheme");
        }

        if(mode==2) {//删除

                deltheme(getparm("theme"));
                redirect("bbstheme");
        }


        if(mode==3) {
                struct theme tmptheme;
                char theme[16],buf[128];

                strcpy( theme,getparm("theme"));
                sprintf(buf,"%s/theme/%s/style",DOCROOT,theme);
                FILE * fp=fopen(buf,"r");
                if(fp==NULL) {

                        printf("%s: can not open.",buf);
                        http_quit();
                }
                fread(&tmptheme,sizeof(struct theme),1,fp);
                fclose(fp);


                sprintf(buf,"%s/theme/%s/css",DOCROOT,theme);
                char bgbuf[2056];
                int fd=0;
                if((fd=open(buf,O_RDONLY))==EOF) {
                        printf("css file missing.");
                        http_quit();
                } else {
                        bzero(bgbuf,2056);
                        read(fd,bgbuf,2056);
                        close(fd);
                }




                printf("\
                       <center>%s -- 系统风格设置管理 [使用者: %s]<hr color=green></center>", BBSNAME, currentuser.userid);
                printf("\
                       修改:	 %s\
                       <form action=bbstheme method=post name=theme>\
                       <input type=hidden name=mode value=4>\
                       <input type=hidden name=en  value=%s>\
                       <input type=hidden name=modified  value=1>\
\
                       中文名字  <input name=cn maxlength=16  size=16 value=%s  ><br>\
                       导航条颜色  <input name=nav size=16 maxlength=6 value=%s ><br>\
                       主窗体背景色  <input name=bg size=16 maxlength=6   value=%s ><br>\
                       左侧字体 颜色  <input name=lf size=16 maxlength=6 value=%s  ><br>\
                       主窗体字体颜色  <input name=mf size=16 maxlength=6 value=%s  ><br>\
                       重色   <input name=bar size=16 maxlength=6 value=%s  ><br>\
                       浅色    <input name=con size=16  maxlength=6 value=%s ><br>\
                       背景图片<input name=bgpath size=80  value=%s ><br>\
                       左侧背景图片  <input name=lbg maxlength=40 size=60  value=%s >请填入有效的图片路径,最多40个字符<br>\
                       css文件<br><br><textarea   onkeypress='if(event.keyCode==10) return document.theme.submit()' name=css rows=20 cols=80 wrap=physicle>%s</textarea><br>&nbsp\
                       <input type=submit value=可以啦>\
                       <input type=reset onclick='javascript:history.go(-1)'  value=cancel></form> ",tmptheme.cn,tmptheme.en,tmptheme.cn,tmptheme.nav,tmptheme.bg,tmptheme.lf,tmptheme.mf,tmptheme.bar,tmptheme.con,tmptheme.bgpath,tmptheme.lbg,bgbuf);

                http_quit();
        }


        if(mode==4) {


                deltheme(getparm("en"));
                addtheme();
                redirect("bbstheme");
        }

}

int deltheme(char *name)
{
        char theme[16],buf[128];

        strcpy(theme,name);
        if(!strcmp(theme,"")) {
                showinfo("请指定您要删除的风格!");
                return 1;
        }

        sprintf(buf,"%s/theme/.DIR",DOCROOT);
        FILE *fp=fopen(buf,"r");
        int size= sizeof(struct themename);
        int i=0;
        while(fread(&tmpname,size,1,fp)==1) {
                if(strcmp(tmpname.en,theme)==0) {
                        del_record(buf,size,i);

                        break;
                }
                i++;
        }
        sprintf(buf,"rm -rf %s/theme/%s",DOCROOT,theme);
        system(buf);
}

int addtheme()
{
        struct theme tmptheme;
        strcpy(tmptheme.en,getparm("en"));
        strcpy(tmptheme.cn,getparm("cn"));
       
   //    if(!strcmp(tmptheme.cn,"绿色奇迹")) strcpy(tmptheme.en,"green");
 
        strcpy(tmptheme.nav,getparm("nav"));
        strcpy(tmptheme.bg,getparm("bg"));
        strcpy(tmptheme.bar,getparm("bar"));
        strcpy(tmptheme.con,getparm("con"));
        strcpy(tmptheme.lf,getparm("lf"));
        strcpy(tmptheme.mf,getparm("mf"));
        strcpy(tmptheme.bgpath,getparm("bgpath"));
        strcpy(tmptheme.lbg,getparm("lbg"));
        char buf[128];

        sprintf(buf,"/home/www/html/theme/%s",tmptheme.en);
        if(!file_exist(buf))
                mkdir(buf,0770);
        else {
                int i=atoi( getparm("modified"));
                if(i==0) {
                        printf("theme %s exsits,error",tmptheme.en);
                        http_quit();
                        return;
                }
        }
        FILE *fp;
        strcpy(buf,"/home/www/html/theme/.DIR");
        fp=fopen(buf,"a+");
        strcpy(tmpname.cn,tmptheme.cn);
        strcpy(tmpname.en,tmptheme.en);
        fwrite(&tmpname,sizeof(struct themename),1,fp);
        fclose(fp);

        sprintf(buf,"/home/www/html/theme/%s/style",tmptheme.en);
        fp=fopen(buf,"w+");

        sprintf(buf,"/home/www/html/theme/%s/css",tmptheme.en);
        strcpy(tmptheme.css,buf);
        fwrite(&tmptheme,sizeof(struct theme),1,fp);
        fclose(fp);


        char bgbuf[2056];
        strcpy(bgbuf,getparm("css"));
        if((fp=fopen(buf,"w+"))!=NULL) {

                fprintf(fp,"%s",bgbuf);
                fclose(fp);
        }



}



