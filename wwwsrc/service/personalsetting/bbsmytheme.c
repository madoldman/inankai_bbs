/*$Id: bbsmytheme.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
 */
#include"BBSLIB.inc"
#define DOCROOT "/home/www/html"

int main()
{
        init_all();
        if(!loginok) {
                printf("�Բ���,����û�е�½,���ܽ��з���趨");
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
                       <center>%s --  �ҵķ���趨 [ʹ����: %s]", BBSNAME, currentuser.userid);
                printf("<br><br>");


                printf("</center>��վ����Ϊ������Ƿ���������<center>");
                sprintf(buf,"%s/theme/.DIR",DOCROOT);
                if(!file_exist(buf)) {
                        printf("ϵͳ����Ϊ��");
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
                        printf("</center><br><a href=bbsmytheme?mode=4>�ָ�Ĭ������</a>");
                printf("<br><font color=green>�����һ��,������½֮��,û�п�������Ϥ�ķ��,�ܿ�������Ϊ���Ǹ�����ϵͳ����,������ѡ����Ѿ���������,������Ӧ�ø��õİ�!");


                int n=showshare();


                printf("</font><br><br><br>���߸��Ի�����Ҳ���Զ����Լ��ķ�񣬳��չ�����ĳ������ԣ�<br>\
                       <font color=ff0000>ע��:��� �������и���Ǻ��˽�Ļ���������ʹ��ϵͳ���á���ɫ��������λ��RGB��ʾ�� </font>");


                char bgbuf[2056],path[STRLEN];
                struct theme tmptheme;
                sprintf(buf,"%s/home/%c/%s/.theme/style",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
                sprintf(path,"%s/home/%c/%s/.theme/css",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);


                if(file_exist(buf)) {

                        printf("<p><b><i>����ǰ���趨:</i></b></p>");
                        if(!n)
                                printf("<a href=bbsmytheme?mode=6>�����ҵ��趨</a></p>");
                        else
                                printf("<a href=bbsmytheme?mode=7>ȡ������</a> ע��:�ò�����Ӱ�����˵�ʹ��.</p>");

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
                       ��������ɫ  <input name=nav size=16 maxlength=6 value=%s ><br>\
                       �����屳��ɫ  <input name=bg size=16 maxlength=6   value=%s ><br>\
                       ������� ��ɫ  <input name=lf size=16 maxlength=6 value=%s  ><br>\
                       ������������ɫ  <input name=mf size=16 maxlength=6 value=%s  ><br>\
                       ��ɫ   <input name=bar size=16 maxlength=6 value=%s  ><br>\
                       ǳɫ    <input name=con size=16  maxlength=6 value=%s ><br>\
                       ����ͼƬ<input name=bgpath size=80 maxlength=80  value=%s >��������Ч��ͼƬ���ӣ����·������·������<br>\
\
                       ��౳��ͼƬ  <input name=lbg maxlength=40  size=40 value=%s>��������Ч��ͼƬ·��,���40���ַ�<br>\
                       css�ļ�<br><br><textarea   onkeypress='if(event.keyCode==10) return document.theme.submit()' name=css rows=20 cols=80 wrap=physicle>%s</textarea><br>&nbsp\
                       <input type=submit value=Ӧ���ҵ�������>\
                       <input type=reset onclick='javascript:history.go(-1)' value=����>\
\
\
                       </form> ",tmptheme.nav,tmptheme.bg,tmptheme.lf,tmptheme.mf,tmptheme.bar,tmptheme.con,tmptheme.bgpath,tmptheme.lbg,bgbuf);

                http_quit();
        }

        if(mode==1) {//�û��趨�Լ��ķ��

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



        if(mode==4||mode==5) {//�ָ�Ĭ���趨
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
                        showinfo("�ļ���ȡʧ��!");
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

        printf("�����ģʽ!");
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
        printf("<p>����BBS������?�Ͽ��ó�����һ������һ������!<br>���ѹ�����:<br></p>");
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
