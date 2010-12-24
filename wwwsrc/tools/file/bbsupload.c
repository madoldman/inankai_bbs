/*$Id: bbsupload.c,v 1.5 2009-12-29 06:41:52 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "filemgr.inc"
//huangxu@060718:��������ϵͳ

#define QMD_PATH "/home/www/upload"
int main()
{
        int maxsize=1024*1024*20;
        char lifetip[128];
		char buf[1024];
        init_all();

        //huangxu@061229
        if(file_exist("/home/www/html/news/noupload.html"))
        {
        	redirect("/news/noupload.html");
        }

        if(!loginok) {
                showinfo("����û�е�½,�޷�ճ������");
                return 1;
        }
        if(!(currentuser.userlevel & PERM_POST)) {
                showinfo("����δͨ��ע�ᣬ��������Ȩ��,�������û����дע�ᵥ�������ھ���ϸ��д");
                return 1;
        }
        printf("<script>\
               function picsubmit() {\
               if(document.upload.up.value.length<4){alert('��ѡ���ļ�'); return false;};\
               {var o=document.createElement('p');o.innerHTML='Uploading...<br />';\
               document.body.insertBefore(o,document.upload);\
               var s=document.upload.action;if(s.indexOf('*')>=0)s=s.substring(0,s.indexOf('*'));\
               s+=document.upload.autoresize.checked?'*':'';document.upload.action=s; return true;}\
       }\
               </script>");
        printf("</head><body bgcolor=d8e8f3>");


        char filename[STRLEN],tmpfile[STRLEN];
        strncpy(filename,getparm("filename"),STRLEN);
	char * p;
//huangxu@061019:filename no ()
        for (p=filename;*p;p++)
        {
        	if (strchr("()[]{}/\\<>|`~!@#$%^&*?\'\";:",*p))

        		*p='_';
        }

	
        char board[STRLEN];
        strncpy(board,getparm("board"),STRLEN);
	if(*board && *board != '*') 
	{
		struct shortfile *x1;
		shm_init();
		x1=getbcache(board);
		if(x1==0)
			http_fatal("�����������");
		struct fmg_attdir ad;//huangxu@070414
		ad=fmg_openattdir(board,0);
		if(!(ad.fp))return 0;
		fmg_sizestr(buf, ad.fh.maxfsize ? ad.fh.maxfsize : INT_SYSMAXUPFILESIZE);
		sprintf(lifetip, "��%s/%s", buf, fmg_lifestr[ad.fh.defaultlife]);	
		fmg_closeattdir(&ad);
	}
	strlower(board);


 //if(u_info->mode!=SMAIL+20000&&u_info->mode!=QMD+20000 &&!strcmp(board,"") )
 //huangux@070414
	if (u_info->mode == SMAIL + 20000 || !strcmp(board, "*mail"))
	{
		fmg_sizestr(buf, INT_SYSMAXUPFILESIZE);
		sprintf(lifetip, "��%s/7��", buf);
	}
	else if (u_info->mode == QMD + 20000 || !strcmp(board, "*qmd"))
	{
		strcpy(lifetip, "��500KB");
	}
	else if (!strcmp(board, "*restorepc"))
	{
		strcpy(lifetip, "Upload pc tar ball and restore it");
	}
	else if (*board && *board != '*')//read boardmgr
	{
//		sprintf(lifetip, "���帽��Ĭ��������%s", fmg_lifestr[ad.fh.defaultlife]);
	}
	else//fatal
                 http_fatal("״̬���������ԡ�");

        if(filename[0]) {//����ͼƬǩ���� && restore pc
                if(u_info->mode==QMD+20000 || !strcmp(board, "*qmd")) {
                        maxsize=1024*500;//��С����Ϊ500k
                        int pid=atoi(getparm("tmpfile"));
                        sprintf(tmpfile, "tmp/%d.upload", pid);
                        if(!file_exist(tmpfile)) {
                                http_fatal("������ļ���");
                                return 1;
                        }

                        int size=file_size(tmpfile);
                        if(size==0) {
                                unlink(tmpfile);
                                http_fatal("ͼƬ��СΪ0");
                                return 1;
                        }
                        if(size>maxsize) {
                                unlink(tmpfile);
                                http_fatal("�ļ�̫��");
                                return 1;
                        }

                        char path[STRLEN];
                        sprintf(path,"%s/qmd/%c/",QMD_PATH,toupper(currentuser.userid[0]));
                        if(!file_exist(path)) {
                                mkdir(path,0755);
                        }

                        char *p=strtok(filename,".");
                        char *name;
                        while( p != NULL ) {
                                name=p;
                                p = strtok( NULL, "." );
                        }


                        char  dotname[12],cmd[STRLEN];
                        strtolower(dotname,name);

                        if(!validpic(dotname)) {
                                showinfo("�޷�ʶ���ͼƬ��ʽ.");
                                return 1;
                        }
                        deletedformer();

                        sprintf(path,"qmd/%c/%s.%s",toupper(currentuser.userid[0]),currentuser.userid,dotname);
                        sprintf(cmd, "cp %s %s/%s",tmpfile,QMD_PATH,path);
                        system(cmd);
                        unlink(tmpfile);

                        sprintf(cmd,"http://%s/upload/%s",BBSHOST,path);
                        addtoplans(cmd);
                        printf("<script language=javascript>alert(\"���óɹ�!�����ж��ǩ����ʱ,���һ��Ϊ��ͼƬǩ����.\");</script>");
                        redirect("setting");
                        return 0;
                }
				else if (!strcmp(board, "*restorepc"))
				{
                        int pid=atoi(getparm("tmpfile"));
                        sprintf(tmpfile, "tmp/%d.upload", pid);
                        if(!file_exist(tmpfile)) {
                                http_fatal("������ļ���");
                                return 1;
                        }

                        int size=file_size(tmpfile);
                        if(size==0) {
                                unlink(tmpfile);
                                http_fatal("��СΪ0");
                                return 1;
                        }
                        if(size>10 * 1048576) {
                                unlink(tmpfile);
                                http_fatal("�ļ�̫��");
                                return 1;
                        }
						char cmd[1024];
						sprintf (cmd, " /home/bbs/bin/restorePc.sh %s ", tmpfile);
						system (cmd);
						sprintf (cmd, "/cgi-bin/bbs/bbs0an?path=/PersonalCorpus/%c/%s",
								toupper(currentuser.userid[0]),currentuser.userid);
						//redirect(cmd);
						sprintf ("<script type=\"text/javascript\">if (window.parent)window.parent.location='%s';</script>", cmd);
						return 0;
				}
        }

        int done=0;
        if(filename[0]) {//����upload
                int pid=atoi(getparm("tmpfile"));
                if(pid==0) {
                        http_fatal("û���ҵ���ʱ�ļ�");
                        return 0;
                }
                sprintf(tmpfile, "tmp/%d.upload", pid);
                if(!file_exist(tmpfile)) {
                        http_fatal( "tmpfile  ������ļ���");
                        return 1;
                }
                int size=file_size(tmpfile);
                if(size==0) {
                        unlink(tmpfile);
                        http_fatal("��û���ϴ��κ����ݣ��뷵��");
                        return 1;
                }
                if(size>maxsize) {
                        unlink(tmpfile);
                        http_fatal("�ļ�̫��");
                        return 1;
                }
                if(upload(filename,tmpfile,board/*,&a*/)) {
                        printf("���ܴ����ļ�,���ܱ��帽���ѳ����޶�");
                        unlink(tmpfile);
                        return 1;
                }
                done=1;
        }
        printf("<div style=\"line-height:32px; position:absolute; top: 4px; \">");
        printf("<form method=\"post\" name=\"upload\" action=\"bbsdoupload?%s\" enctype=\"multipart/form-data\" /> \n",board);
        printf("<input type=\"file\" name=\"up\" />");
        printf("<input type=\"submit\" class=\"btn\" onclick=\"return picsubmit();\"  value=\"�� ��\" /> \n");
        printf("<input type=\"hidden\" name=\"board\" value=\"%s\" />",board);
        //printf("<input type=\"checkbox\" name=\"autoresize\" value=\"autoresize\" checked=\"checked\" />");
        //printf("<span title=\"�Զ�������640*480���ص�jpgͼƬ��С����bmp��ʽת��Ϊpng��ʽ\">�Զ�����ͼƬ</span>");
        printf("&nbsp;%s", lifetip);
        printf("</form></div></body>");
        if(done)
                printf("<script language=javascript>alert(\"��ϲ,%s�ϴ����!\");</script>",filename);
}

int safemkdir(board,date)
{
        char buf[STRLEN];
        if(u_info->mode!=SMAIL+20000) {
                sprintf(buf,"%s/f/%s",UPLOAD_PATH,board);
                if(!file_exist(buf))
                        mkdir(buf,0755);
                sprintf(buf,"%s/f/%s/%s",UPLOAD_PATH,board,date);
                if(!file_exist(buf))
                        mkdir(buf,0755);
        } else {
                sprintf(buf,"%s/m/%s",UPLOAD_PATH,date);
                if(!file_exist(buf))
                        mkdir(buf,0755);
        }
}

int upload(char *filename,char *tmpfile,char *board)
{

        char buf[128]={0};
        char url[128]={0};
        char cmd[256]={0};
        char formobj[16]="bbspstform";
        char name[STRLEN]={0};
        time_t tt=time(0);
        struct tm *t=localtime(&tt);
        char date[8];
        sprintf(date,"%02d%02d",t->tm_mon+1,t->tm_mday);

        safemkdir(board,date);

        if(u_info->mode==SMAIL+20000) {
                strcpy(formobj,"bbsmailform");
                //printf("%s ",formobj);
                //�ż���Ҫһ���̶��ϵİ�ȫ��,ͨ��������ø������ļ�������.
                getrandomstr(filename,name);
                sprintf(buf,"%s/m/%s/%s",UPLOAD_PATH,date,name);
                if(file_exist(buf))   return 1;//fail
                sprintf(url,"http://%s/up2/m/%s/%s",BBSHOST,date,name);

        } else {

                int size=strlen(filename);
                if(size>60)
                        filename=filename+size-60;//��֤name���ᳬ��80���ַ�
                sprintf(buf,"%s/f/%s/%s/%s",UPLOAD_PATH,board,date,filename);
                if(!file_exist(buf))    strcpy(name,filename);
                else{
                        int i,t;
                        for(i=0; i<10; i++) {
                                t=time(0)+i;
                                sprintf(buf,"%s/f/%s/%s/%d%s",UPLOAD_PATH,board,date,t,filename);
                                if(!file_exist(buf))
                                        break;
                        }
                        if(i>=9)
                                return 1;
                        sprintf(name,"%d%s",t,filename);
                }


                sprintf(buf,"%s/f/%s/%s/%s",UPLOAD_PATH,board,date,name);
                sprintf(url,"http://%s/up2/f/%s/%s/%s",BBSHOST,board,date,name);
        }

        sprintf(cmd, "cp %s %s", tmpfile,buf);
        system(cmd);
        unlink(tmpfile);
        if((*board)&&!fmg_addsinglefile(board,buf,currentuser.userid))return 1;

        printf("<script language=javascript>\
               parent.document.%s.text.value += '\\n'+'%s';\
               parent.document.%s.attach.value=1;\
               </script>", formobj,url,formobj);
        return 0;
}

/*��������ַ�������name
  ��Ӱ��filename
  32λ�����,���filename�����8λ */
int getrandomstr(char *filename,char *name)
{
        char buf[80];
        char *p=filename;
        int size=strlen(filename);
        int i;
        if(size>8)
        {
          p+=size-8;
          char *x=p;//huangxu@061021:�����ж�
          i=0;
          for(x=p;*x;x++)
          {
          	if(*x&0x80)
          		i^=1;
          	else
          		break;
          }
          if(i)*p='_';
        }
        srandom(time(0));
        for(i=0;i<32;i++)
                buf[i]='A'+random()%25;
        buf[i]=0;
        strcpy(name,buf);
        strcat(name,p);
}

int addtoplans(char *path)
{

        char buf[STRLEN];
        sethomefile(buf, currentuser.userid, "signatures");
        if(!file_exist(buf)) {
                FILE *fp=fopen(buf,"w+");
                fprintf(fp,"%s\n",path);
                fclose(fp);
                return 0;
        }

        FILE *fp=fopen(buf,"r+");
        if(fp==NULL)
                return 1;
        int i=0;
        while (fgets(buf,80,fp)!=NULL) {
                i++;
                if(strstr(buf,path)) {
                        fclose(fp);
                        return 0;
                }
                if(i%6==0)
                        i=0;
        }

        if(i==0)
                i=6;
        for (;i<6 ;i++ ) {
                fprintf(fp,"\n");
        }
        fprintf(fp,"%s\n",path);
        fclose(fp);
}

int validpic(char *name)
{
        char *dotname[]={"jpg","gif","jpe","jpeg","png"};
        int i=0;
        for(;i<5;i++) {
                if(!strcmp(dotname[i],name))
                        return 1;
        }
        return 0;
}

int deletedformer()
{
        char *dotname[]={".jpg",".gif",".jpe",".jpeg",".png"},path[STRLEN];
        int i=0;
        for(;i<5;i++) {
                sprintf(path,"%s/qmd/%c/%s%s",QMD_PATH,toupper(currentuser.userid[0]),currentuser.userid,dotname[i]);
                if(file_exist(path)) {
                        char cmd[STRLEN];
                        sprintf(cmd, "rm %s",path);
                        system(cmd);
                        return 1;
                }
        }
        return 0;
}

