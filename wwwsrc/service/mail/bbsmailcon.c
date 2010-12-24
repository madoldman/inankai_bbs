/*$Id: bbsmailcon.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "mail.h"
int main()
{
	FILE *fp;
        char buf[512], dir[80], file[80], path[80], *ptr, *id;
	char user_ex[STRLEN];
        struct fileheader x;
        int num, tmp, total,mode;
        init_all();
        modify_mode(u_info,RMAIL+20000);	//bluetent
        strsncpy(file, getparm("file"), 32);
        num=atoi(getparm("num"));
        mode=atoi(getparm("mode"));
        printf("<center>\n");
        id=currentuser.userid;

        if(mode==0)
                printf("%s -- �Ķ��ż� [ʹ����: %s]<hr>\n", BBSNAME, id);
        else
                printf("%s -- �Ķ�����վ�ż� [ʹ����: %s]<hr>\n", BBSNAME, id);


        if(strncmp(file, "M.", 2))
                http_fatal("����Ĳ���1");
        if(strstr(file, "..") || strstr(file, "/"))
                http_fatal("����Ĳ���2");
        if(mode==0)
                sprintf(dir, "mail/%c/%s/.DIR", toupper(id[0]), id);
        else
                sprintf(dir, "mail/%c/%s/.recycle", toupper(id[0]), id);


        total=file_size(dir)/sizeof(x);
        if(total<=0)
                http_fatal("����Ĳ���3");
        printf("<table width=610 border=1>\n");
        printf("<tr><td>\n<pre>");
        sprintf(path, "mail/%c/%s/%s", toupper(id[0]), id, file);
        fp=fopen(path, "r");
	if(fp==NULL)
	{
		sprintf(path,"mail/announce/%s",file);
		fp=fopen(path,"r");
	}
        if(fp==0)
                http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
        while(1) {
                if(fgets(buf, 512, fp)==0)
                        break;
                if(!strncmp(buf, ": ", 2))
                        printf("<font color=808080>");
                hhprintf("%s", void1(buf));
                if(!strncmp(buf, ": ", 2))
                        printf("</font>");
        }
        fclose(fp);
        printf("</pre>\n</table><hr>\n");
        if(mode==0)
                printf("[<a onclick='return confirm(\"�����Ҫɾ���������?\")' href=bbsdelmail?file=%s&mode=0> ɾ��</a>]", file);
        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("dir error2");
        if(num>0) {
                fseek(fp, sizeof(x)*(num-1), SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                printf("[<a href=bbsmailcon?file=%s&num=%d&mode=%d>��һƪ</a>]", x.filename, num-1,mode);
        }

        if(mode==0)
                printf("[<a href=bbsmail>�ż��б�</a>]");
        else {
                printf("[<a href=bbsmail>�ż��б�</a>]");
                printf("[<a href=bbsmailrecycle>����վ</a>]");

        }




        if(num<total-1) {
                fseek(fp, sizeof(x)*(num+1), SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                printf("[<a href=bbsmailcon?file=%s&num=%d&mode=%d>��һƪ</a>]", x.filename, num+1,mode);
        }
        if(mode==0) {
                if(num>=0 && num<total) {
                        char title2[80];
                        fseek(fp, sizeof(x)*num, SEEK_SET);
                        if(fread(&x, sizeof(x), 1, fp)>0) {
                                x.accessed[0]|=FILE_READ;
                                fseek(fp, sizeof(x)*num, SEEK_SET);
                                fwrite(&x, sizeof(x), 1, fp);
                                ptr=x.owner;
                                //mod by brew 2005.6.25
				//getsender(ptr);
				getsenderex(ptr,user_ex);
                                strcpy(title2, x.title);
                                if(strncmp(x.title, "Re:",3))
                                        sprintf(title2, "Re: %s", x.title);
                                title2[60]=0;
                                printf("[<a href='bbspstmail?userid=%s&file=%s&title=%s'>����</a>]",
                                       nohtml(user_ex), x.filename, title2);
                        }
                }

        }

        if(mode==1)
                printf("[<a href='bbsdelmail?file=%s&mode=1'>��ԭ</a>]",file);








        fclose(fp);
        printf("</center>\n");
        http_quit();
}
