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
                showinfo("�Բ���,����ϵͳά�������������Ա���Է��ʱ�ҳ��.");
                return 1;
        }
        int mode =atoi( getparm("mode"));

        if(mode==0) {

                printf("\
                       <center>%s -- ϵͳ������ù��� [ʹ����: %s]<hr color=green></center>", BBSNAME, currentuser.userid);
                printf("\
                       ���:\
                       <form action=bbstheme method=post name=theme>\
                       <input type=hidden name=mode value=1>\
                       <input type=hidden name=css >\
                       Ӣ������  <input name=en maxlength= 16 size=12 > 12���ַ�<br>\
                       ��������  <input name=cn maxlength=16  size=16 ><br>\
\
                       ��������ɫ  <input name=nav size=16 maxlength=6 ><br>\
                       �����屳��ɫ  <input name=bg size=16 maxlength=6 ><br>\
                       ������� ��ɫ  <input name=lf size=16 maxlength=6 ><br>\
                       ������������ɫ  <input name=mf size=16 maxlength=6 ><br>\
                       ��ɫ   <input name=bar size=16 maxlength=6 ><br>\
                       ǳɫ    <input name=con size=16  maxlength=6><br>\
                       �����屳��ͼƬ<input name=bgpath maxlength=80  size=60>��������Ч��ͼƬ·��<br>\
                       ��౳��ͼƬ  <input name=lbg maxlength=40  size=40>��������Ч��ͼƬ·��,���40���ַ�<br>\
                       css�ļ�&nbsp&nbsp��С������2K<br><br><textarea   onkeypress='if(event.keyCode==10) return document.theme.submit()' name=css rows=20 cols=80 wrap=physicle>\
                       </textarea>\
\
\
                       <br>&nbsp\
                       <input type=submit value=������>\
                       <input type=reset onclick='javascript:history.go(-1)'  value=����>\
\
\
                       </form>	");

                printf("<br>���з��<br>");
                char buf[128];
                sprintf(buf,"%s/theme/.DIR",DOCROOT);


                if(!file_exist(buf)) {
                        printf("ϵͳ����Ϊ��");
                        http_quit();
                }
                FILE *fp=fopen(buf,"rb");
                if(fp==NULL) {
                        printf("ERROR,can not open .DIR");
                        http_quit();
                }
                int i=0;
                while(fread(&tmpname,sizeof(struct themename),1,fp)==1) {

                        printf("<br>%s<a href=bbstheme?mode=2&&theme=%s>&nbsp&nbspɾ��</a>",tmpname.cn,tmpname.en);

                        printf("<a href=bbstheme?mode=3&&theme=%s>&nbsp&nbsp�޸�</a>",tmpname.en);

                        i++;
                }
                printf("<br>��%d��",i);
                fclose(fp);
                http_quit();
        }


        if(mode==1) {
                addtheme();
                redirect("bbstheme");
        }

        if(mode==2) {//ɾ��

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
                       <center>%s -- ϵͳ������ù��� [ʹ����: %s]<hr color=green></center>", BBSNAME, currentuser.userid);
                printf("\
                       �޸�:	 %s\
                       <form action=bbstheme method=post name=theme>\
                       <input type=hidden name=mode value=4>\
                       <input type=hidden name=en  value=%s>\
                       <input type=hidden name=modified  value=1>\
\
                       ��������  <input name=cn maxlength=16  size=16 value=%s  ><br>\
                       ��������ɫ  <input name=nav size=16 maxlength=6 value=%s ><br>\
                       �����屳��ɫ  <input name=bg size=16 maxlength=6   value=%s ><br>\
                       ������� ��ɫ  <input name=lf size=16 maxlength=6 value=%s  ><br>\
                       ������������ɫ  <input name=mf size=16 maxlength=6 value=%s  ><br>\
                       ��ɫ   <input name=bar size=16 maxlength=6 value=%s  ><br>\
                       ǳɫ    <input name=con size=16  maxlength=6 value=%s ><br>\
                       ����ͼƬ<input name=bgpath size=80  value=%s ><br>\
                       ��౳��ͼƬ  <input name=lbg maxlength=40 size=60  value=%s >��������Ч��ͼƬ·��,���40���ַ�<br>\
                       css�ļ�<br><br><textarea   onkeypress='if(event.keyCode==10) return document.theme.submit()' name=css rows=20 cols=80 wrap=physicle>%s</textarea><br>&nbsp\
                       <input type=submit value=������>\
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
                showinfo("��ָ����Ҫɾ���ķ��!");
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
       
   //    if(!strcmp(tmptheme.cn,"��ɫ�漣")) strcpy(tmptheme.en,"green");
 
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



