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
                showinfo("��û�е�½��");
                http_quit();
                return 1;
        }
        //huangxu@060715:Ȩ�޵���
        if((!HAS_PERM(PERM_OBOARDS)&&(mode==1||mode==2||mode==3||mode==11||mode==12||mode==13||mode==18||mode==28))||
        	(!HAS_PERM(PERM_ACCOUNTS)&&(mode==5||mode==15))||
        	(!HAS_PERM(PERM_ANNOUNCE)&&(mode==9||mode==19||mode==29))||
        	(!HAS_PERM(PERM_WELCOME)&&(mode==16||mode==17||mode==26||mode==27)))
        {
                showinfo("����Ȩ���д��������");
                http_quit();
                return 1;
        }

        printf("<body background=%s><font color=%s>",mytheme.bgpath,mytheme.mf);
        if(mode==1) {//�Ƽ�����������

                strcpy(path,"/home/www/html/news/boards");
                readfile(path,bgbuf);
                strcpy(buf,"��ʽ���£�<br>\
                       Pictures ��ͼ\
                       <br>\
                       ÿ��һ����������Ӣ������ǰ,�������ں�,�м��ÿո������ ��ò�Ҫ����16��������,��ע�⿴�����Ƿ��ŵĿ��Ե���������˳��");\
\
\
\
\
                out("�趨�Ƽ�������",buf,bgbuf,mode);
                http_quit();
                return;

        }

        if(mode==2) {//ϵͳ�������
                strcpy(path,"/home/www/html/news/annouce");
                readfile(path,bgbuf);
                sprintf(buf,"<br>˵��:<br>һ������ռһ��,�벻Ҫ�Լ����뻻�з�.����Ƕ��html����.�벻Ҫ����500����.\
                        <br>ʾ��:<br>    ���ı���:<br>&nbsp&nbsp\
                        ������ף������վ��  <br>\
                        Ƕ��html�����:<br>&nbsp&nbsp\
                        &lt;a href=http://202.113.16.117/cgi-bin/beta/bbstcon?board=Announce&file=M.1054020572.A;&gt;��վ����ͼƬǩ�����Ĺ���&lt;/a&gt;  ") ;

                out("�趨ϵͳ����",buf,bgbuf,mode);
                return;

        }


        if(mode==3) {//�¿���������

                strcpy(path,"/home/www/html/news/newboards");

                readfile(path,bgbuf);

                strcpy(buf,"��ʽ���£�<br>\
                       Pictures ��ͼ\
                       <br>\
                       ÿ��һ����������Ӣ������ǰ,�������ں�,�м��ÿո�������벻Ҫ����10����������");




                out("�¿��������趨",buf,bgbuf,mode);
                http_quit();
                return;

        }

        /*if(mode==4) {//�༭Ⱥ���ż�����
                sethomefile(path, currentuser.userid, "maillist");
                readfile(path,bgbuf);
                sprintf(buf,"<br>��ʽ˵��:<br>һ��idռһ��\
                        <br>ʾ��:<br>hello\
                        <br>hehe\
                        ") ;


                out("�༭����Ⱥ���ż�����",buf,bgbuf,mode);
                return;

        }*/

        if(mode==5) {//����IP����
        	if(!HAS_PERM(PERM_ACCOUNTS))http_fatal("PERM_ACCOUNT ONLY");
                strcpy(path,"/home/bbs/etc/publicip");
                readfile(path,bgbuf);
                sprintf(buf,"<br>˵��:<br>һ��ipռһ��,�벻Ҫ�Լ����뻻�з�.�벻Ҫ����100����¼.\
                        <br>ʾ��:<br>202.113.30.0/24</br>");
                out("�趨����IP",buf,bgbuf,mode);
                return;

        }
        if(mode==9) {//ɾ����ͣ�ϴ�
                strcpy(path, "/home/www/html/news/noupload.html");
                unlink(path);
                strcpy(sectitle,"ɾ����ͣ�ϴ�");
        }

        if(mode==11) {
                strcpy(path,"/home/www/html/news/boards");
                writefile(path);
                strcpy(sectitle,"�༭�Ƽ�������");
        }

        if(mode==12) {
                strcpy(path,"/home/www/html/news/annouce");
                writefile(path);
                strcpy(sectitle,"�޸�ϵͳ����");
        }

        if(mode==13) {
                strcpy(path,"/home/www/html/news/newboards");
                writefile(path);
                strcpy(sectitle,"�޸��¿�������");
                //���������ô��
        }

        /*if(mode==14) {


                sethomefile(path, currentuser.userid, "maillist");
                writefile(path);

        }*/
        if(mode==15) {//huangxu@060520:����IP����
                strcpy(path, "/home/bbs/etc/publicip");
                writefile(path);

        }
        
        //huangxu@060419:����ҳ
        if(mode==16) {//�༭վ�ڹ���
                strcpy(path, "/home/www/html/news/bbsannounce");
                readfile(path,bgbuf);
                sprintf(buf,"<br>��ʽ˵��:<br>&lt;li&gt;����������ݡ���&lt;/li&gt;<br>");
                out("�༭վ�ڹ���",buf,bgbuf,mode);
                return;
        }
        if(mode==17) {//�༭У�ڹ���
                strcpy(path, "/home/www/html/news/campusannounce");
                readfile(path,bgbuf);
                sprintf(buf,"<br>��ʽ˵��:<br>&lt;li&gt;����������ݡ���&lt;/li&gt;<br>");
                out("�༭У�ڹ���",buf,bgbuf,mode);
                return;
        }
        if(mode==18) {//�༭��������
                strcpy(path, "etc/zonemaster");
                readfile(path,bgbuf);
                sprintf(buf,"<br>��ʽ˵��:<br>ÿ��һ��<br>");
                out("�༭У�ڹ���",buf,bgbuf,mode);
                return;
        }
        if(mode==19) {//��ͣ�ϴ�
                strcpy(path, "/home/www/html/news/noupload.html");
                readfile(path,bgbuf);
                sprintf(buf,"<br>��ʽ˵��:htmlҳ��&nbsp;<input type=button value='ɾ��' onclick=\"location.href='adminfile?mode=9';\" /><br>");
                out("��ͣ�ϴ�����",buf,bgbuf,mode);
                return;
        }
        if(mode==26) {
                strcpy(path,"/home/www/html/news/bbsannounce");
                writefile(path);
                strcpy(sectitle,"�༭վ�ڹ���");
        }
        if(mode==27) {
                strcpy(path,"/home/www/html/news/campusannounce");
                writefile(path);
                strcpy(sectitle,"�༭У�ڹ���");
        }
        if(mode==28) {
                strcpy(path,"etc/zonemaster");
                writefile(path);
                strcpy(sectitle,"�༭��������");
        }
        if(mode==29) {
                strcpy(path,"/home/www/html/news/noupload.html");
                writefile(path);
                strcpy(sectitle,"�༭��ֹ�ϴ�");
        }
        printf("</font></body>");
        printf("�����ɹ�");
        if(*sectitle)
        	//huangxu@061229
        {
        	FILE * fp;
 	      	sprintf(path, "tmp/%d.tmp", getpid());
 	      	if(!(fp=fopen(path,"w")))
 	      		http_quit();
 	      	fprintf(fp,"ԭ��%s\n",sectitle);
        	getuinfo(fp);
        	fclose(fp);
        	post_article("syssecurity1", sectitle, path, currentuser.userid, "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1);
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
                printf("�ļ������ڣ��½��ļ�");//ʵ���ϴ򲻿�,������ڵĶ��ܴ�
                bzero(bgbuf,102400);
                //huangxu@061229����w+������
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


        printf("<center>%s -- %s[ʹ����: %s]<hr color=%s></center>", BBSNAME, title,currentuser.userid,mytheme.bar);


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
