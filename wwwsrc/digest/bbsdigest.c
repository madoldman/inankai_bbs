/*$Id: bbsdigest.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/
#include "BBSLIB.inc"
#include "digest.h"
struct cpitem
{
        char title[STRLEN];
        char filename[STRLEN];
        char fpath[PATHLEN];
};
#define MAXNUM 1024
struct item
{
        char title[STRLEN];
        char path[16];
}
a[MAXNUM];
char title[STRLEN];
int total=0,index_infile[40],parmnum=0;


int inititem(char *path)
{
        char buf[PATHLEN];
        sprintf(buf, "%s/.Names", path);
        FILE * fp=fopen(buf, "r");
        if(fp==0) {
                printf("Ŀ¼������");
                return 1;
        }
        total=0;
        bzero(a,sizeof(a));
        while(1) {
                if(fgets(buf, 256, fp)==0)
                        break;
                if(!strncmp(buf, "# Title=", 8))
                        strcpy(title, buf+8);
                if(!strncmp(buf, "Name=", 5) && total<1023) {
                        strcpy(a[total].title, trim(buf+5));
                        strcpy(a[total].path, "");
                        total++;
                }
                if(!strncmp(buf, "Path=~", 6) && total>0) {
                        sprintf(a[total-1].path, "%s", trim(buf+7));
                }
        }
        fclose(fp);
        return 0;
}

int addtitle(FILE *fp,char *title)
{
        fprintf(fp,"#\n# Title=%s\n#\n",title);
        return 0;
}

int additem(FILE *fp,char *title,char *path,int n)
{
        fprintf(fp,"Name=%s\nPath=~/%s\nNumb=%d\n#\n",title,path,n);
        return 0;
}

int moveitem(char *path,int num)
{
        if(inititem(path))
                return 1;
        if(num<=0)
                num=1;
        if(num>=total)
                num=total;
        if(num==index_infile[0]) {
                printf("˳��һ��,û���ƶ�");
                return 1;
        }



        char fpath[PATHLEN];
        sprintf(fpath,"%s/.Names",path);
        FILE *fp=fopen(fpath,"w+");
        if(fp==0)
                return 1;
        addtitle(fp,title);
        int j=0;
        int i;
        if(index_infile[0]<num)//����
                for(i=0;i<total;i++) {
                        if(i==(index_infile[0]-1))
                                continue;
                        additem(fp,a[i].title,a[i].path,++j);
                        if((i+1)==num)
                                additem(fp,a[index_infile[0]-1].title,a[index_infile[0]-1].path,++j);

                }
        else//ǰ��
                for(i=0;i<total;i++) {
                        if(i==(index_infile[0]-1))
                                continue;
                        if((i+1)==num)
                                additem(fp,a[index_infile[0]-1].title,a[index_infile[0]-1].path,++j);
                        additem(fp,a[i].title,a[i].path,++j);
                }
        fclose(fp);
        return 0;
}

int savenames(FILE *fp)
{
        int i;
        for(i=0;i<total;i++)
                additem(fp,a[i].title,a[i].path,i+1);
        return 0;
}

int debg()
{
        printf("<br>\
               parmnum %d<br>\
               index[0] %d<br>\
               total  %d <br>\
               ",parmnum,index_infile[0],total);
        int i;
        for(i=0;i<total;)
                printf("<br>%3d%40s%20s" ,++i,a[i].title,a[i].path);
}

int renameitem(char *path,char *filename)
{
        if(inititem(path))
                return 1;
        char fpath[PATHLEN];
        sprintf(fpath,"%s/.Names",path);
        FILE *fp=fopen(fpath,"w+");
        if(fp==0)
                return 1;
        addtitle(fp,title);
        // debg();
        strcpy(a[index_infile[0]-1].title,filename);
        // debg();
        savenames(fp);
        fclose(fp);
        // debg();
        return 0;
}

int delitem(char *path)
{
        /*char title[STRLEN],fpath[PATHLEN];
        int copyed=0;
        struct cpitem item;
        sethomefile(buf, currentuser.userid,".copypaste");
        FILE *fp=fopen(buf,"r");
        if(fp==0) return 1;
        sprintf(fpath,"0Announce%s",linkpath);
        while(1){
           if(fread(&item,sizeof(struct cpitem),1,fp)<=0) break;
           if(!strcmp(item.filename,itemname))
             if(!strcmpy(item.fpath,fpath)){
                 printf("�õ��������Ϊ����,����ɾ��");
              } 
        }             */

        if(inititem(path))
                return 1;
        char fpath[PATHLEN];
        sprintf(fpath,"%s/.Names",path);
        int inpic=0;
        if(strstr(fpath,"Pictures"))
                inpic=1;
        FILE *fp=fopen(fpath,"w+");
        if(fp==0)
                return 1;
        addtitle(fp,title);
        int deleted[MAXNUM],i;
        bzero(deleted,MAXNUM);
        for(i=0;i<parmnum;i++)
                deleted[index_infile[i]-1]=1;
        char cmd[272],buf[PATHLEN];
        int j=0;
        for(i=0;i<total;i++) {
                if(deleted[i]) {
                        if(a[i].path[0]) {
                                sprintf(buf,"%s/%s",path,a[i].path);
                                if(file_isdir(buf)) {
                                        if(isempty(buf)) {
                                                sprintf(cmd,"rm -rf %s",buf);
                                                system(cmd);
                                                if(inpic)
                                                        delappendix(a[i].path);
                                                printf("<br>ɾ��%s",a[i].title);
                                        } else {
                                                printf("<br>ע��:��ֻ����ɾ����Ŀ¼.<font color=green>%s</font>����,����.",a[i].title);
                                                additem(fp,a[i].title,a[i].path,++j);
                                        }
                                }
                        }
                        continue;
                }
                additem(fp,a[i].title,a[i].path,++j);
        }
        fclose(fp);
        return 0;
}

int isempty(char *path)
{
        char buf[PATHLEN];
        sprintf(buf, "%s/.Names", path);
        FILE *fp=fopen(buf,"r");
        if(fp==0)
                return 1;
        while(1) {
                if(fgets(buf, 256, fp)==0)
                        break;
                if(!strncmp(buf, "Name=", 5))
                        return 0;
        }
        return 1;
}


int delappendix(char *filename)
{


}


int copyitem(char *path)
{
        char buf[STRLEN];
        struct cpitem item;
        FILE *fp;
        sethomefile(buf, currentuser.userid,".copypaste");
        fp=fopen(buf,"w+");
        if(fp==0)
                return 1;
        if(inititem(path))
                return 1;
        int copyed[MAXNUM],i;
        bzero(copyed,MAXNUM);
        for(i=0;i<parmnum;i++)
                copyed[index_infile[i]-1]=1;
        for(i=0;i<total;i++) {
                if(copyed[i]==0)
                        continue;
                strcpy(item.title,a[i].title);
                strcpy(item.filename,a[i].path);
                sprintf(item.fpath,"%s",path);
                fwrite(&item,sizeof(item),1,fp);
        }
        return 0;
}

int pasteitem(char *path)
{
        char file[PATHLEN],cmd[PATHLEN],buf[STRLEN];
        struct cpitem item;
        sethomefile(buf, currentuser.userid,".copypaste");
        if(file_size(buf)<=0) {
                printf("����ճ����Ϊ��.");
                return 1;
        }
        FILE *fp=fopen(buf,"r");
        sprintf(buf,"%s/.Names",path);
        FILE *names=fopen(buf,"a+");
        if(fp==0||names==0) {
                printf("���ļ�ʧ��.<br>");
                return 1;
        }

        while(1) {
                if(fread(&item,sizeof(struct cpitem),1,fp)<=0)
                        break;
                sprintf(file,"%s/%s",item.fpath,item.filename);
                if(strstr(path,file)) {
                        printf("<br>%s %s :���ܸ��ƽ���Ŀ¼.����",path,file);
                        continue;
                }
                if(!file_exist(file)) {
                        printf("%s %s ������,�����ѱ�ɾ��.",item.title,item.filename);
                        continue;
                }
                sprintf(cmd,"%s/%s",path,item.filename);
                if(file_exist(cmd)) {
                        printf("<br>Ŀ�� %s ����.",item.title);
                        continue;
                }
                fprintf(names,"Name=%s\nPath=~/%s\nNumber=\n#\n",item.title,item.filename);
                sprintf(cmd, "cp -rf  %s %s ",file ,path);
                system(cmd);
        }
        fcloseall();
}

int getroad(char *path)
{
        char buf[PATHLEN];
        FILE *fp;
        sethomefile(buf, currentuser.userid,".announcepath");
        strcpy(path,"");
        if((fp = fopen(buf, "r")) == NULL ) {
                printf("����û���趨˿·.");
                return 1;
        }
        fscanf(fp,"%s",buf);
        fclose(fp);
        if(!file_exist(buf)) {
                printf("��Ч��·��:%s.˿·�Ѷ�ʧ,�������趨.",buf);
                return 1;
        }
        strcpy(path,buf);
        return 0;
}

int showroad()
{
        char buf[PATHLEN],path[1024];
        if(!getroad(buf)) {
                printf("��ǰ˿·Ϊ%s",buf);
                cnpath(buf,path);
                printf("<br>%s",path);
        }
        printf("<br>");
        printf("<font color=red>ʲô��˿·?</font><br><p>˿·����һ��·��,�����ڰ����ڰ��������뾫����ʱ, �����ŵ���Ŀ¼����.�������ھ������Ĺ���ģʽ,�ı�˿·���趨,�ѵ�ǰĿ¼�趨Ϊ˿·</p>");
        return 0;

}
int createname(char *name)
{
        char buf[32];
        int i;
        srandom(time(0));
        for(i=0;i<9;i++)
                buf[i]='A'+random()%25;
        buf[i]=0;
        strcpy(name,buf);
}

int createitem(char *path,char *title,int type)
{
        if(inititem(path))
                return 1;
        if(total>1020) {
                printf("̫�����Ŀ,����ʧ��");
                return 1;
        }







        char file[PATHLEN],name[12];
        sprintf(file,"%s/.Names",path);
        FILE *fp=fopen(file,"a+");
        if(fp==0) {
                showinfo("���ܴ��ļ�.Names");
                return 1;
        }
        if(!strcmp(title,"welcome"))
                strcpy(name,"welcome");
        else
                createname(name);
        sprintf(file,"%s/%s",path,name);
        if(file_exist(file)) {
                printf("%s����,���ܴ���",file);
                fclose(fp);
                return 1;
        }

        fprintf(fp,"Name=%s\nPath=~/%s\nNumb=\n#\n",title,name);
        fclose(fp);
        if(type==1) {
                fopen(file,"w+");
                //  printf("ok");
                return 0;
        }
        if(type==2)
                mkdir(file,0770);
        sprintf(file,"%s/%s/.Names",path,name);
        fp=fopen(file,"w+");
        if(fp==0) {
                printf("���ܴ��ļ�%s",file);
                return 1;
        }
        fprintf(fp,"#\n# Title=%s\n#\n",title);
        fclose(fp);
        return 0;
}

int main()
{
        init_all();
        modify_mode(u_info,DIGEST+20000);
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        int mode=atoi(getparm("mode"));

        if(mode==1) {//�鿴˿·
                if(!HAS_PERM(PERM_OBOARDS) && !HAS_PERM(PERM_BOARDS) ) {
                        printf("��û���Լ���˿·��");
                        goto E;
                }
                showroad();
                goto E;

        }

        char linkpath[PATHLEN];
        strncpy(linkpath,getparm("path"),PATHLEN);
        if(!strcmp(linkpath,"")||strstr(linkpath, "..") || strstr(linkpath, "SYSHome"))
                http_fatal("��Ŀ¼������!");
        char path[PATHLEN];
        sprintf(path,"0Announce%s",linkpath);
        if(!file_exist(path)) {
                printf("%s",path);
                showinfo("��Ŀ¼������!");
                return 1;
        }

        char s[20];
        int r=getfrompath(linkpath,s);
        if(!hasperm(r,s)) {
                showinfo("����Ȩ���ʸ�ҳ!");
                return 0;
        }


        if(mode==2) {//�½���Ŀ
                printf("<script>\
                       function check(type) {\
                       if(document.form1.title.value.length<1){alert('����������!'); return false;}\
                       document.form1.type.value=type;\
                       return true;\
               }\
                       </script>");
               // <input type=submit class=btn value=�����ļ� onclick='return check(1)'>
                printf("<form name=form1 method=post action=bbsdigest?mode=12&path=%s>",linkpath);
                printf("���������Ŀ������(32���ַ�)<br>\
                       <input  type=text name=title size=32 maxlength=32 value=''>\
                       <input type=hidden name=type>\
                 \
                       <input type=submit class=btn value=����Ŀ¼ onclick='return check(2)'> </form>");
                printf("<br><br>Ҫ����һ��ֻ���Լ����Է��ʵ�����Ŀ¼,��Ŀ����������<font color=green>&lt;HIDE&gt;</font>��ͷ<br>���Ŀ¼������<font color=green>&lt;GUESTBOOK&gt;</font>��ͷ,���ǽ���Ϊ������һ�����Ա�<br><a href=bbs0an?mode=1&path=%s>���ر�ҳ����ģʽ</a>",linkpath);
                return 0;
        }

        if(mode==4) {//ճ��
                pasteitem(path);
                goto E;
        }

        if(mode==11) {//�趨˿·
//                if(r==2) {
//                        printf("�����ļ�û��˿·����");
//                        goto E;//�����ļ������趨˿·��
//                }huangxu@060715:Ϊ����
                char buf[40];
                FILE *fp;
                sethomefile(buf, currentuser.userid,".announcepath");
                if((fp = fopen(buf, "w+")) == NULL ) {
                        printf("%s����ʧ��",buf);
                        return 1;
                }
                fprintf(fp,"0Announce%s",linkpath);
                fclose(fp);
                goto E;
        }

        if(mode==12) {//�½��ļ�,Ŀ¼

                denyrobot();

                int i=0;
                int type=atoi(getparm("type"));
              /*  if(type==1)
                        i=15;
                else if (type==2)
                        i=25;
                else {
                        showinfo("��ѡ���ļ�����Ŀ¼.");
                        return 1;
                }*/
                i=25;
                if(strlen(path)>=(PATHLEN-i)) {
                        showinfo("Ŀ¼�ṹ̫��,�޷�����.");
                        return 1;
                }
                char title[40];
                strncpy(title,getparm("title"),32);
                createitem(path,title,type);
                goto E;
        }

        parmnum=0;
        int i;
        for(i=0; i<parm_num && i<40; i++) {
                if(!strncmp(parm_name[i], "box", 3)) {
                        index_infile[parmnum]=atoi(parm_name[i]+3);
                        parmnum++;
                }
        }

        if(parmnum<=0) {
                showinfo("����ѡ������<br>\n");
                return 0;
        }

        if(mode==3) {
                if(copyitem(path))
                        printf("ϵͳ����.");
                goto E;
        }

        if(mode==5) {
                if(delitem(path))
                        printf("ϵͳ����.");
                goto E;
        }

        if(mode==6) {
                char buftitle[STRLEN];
                strncpy(buftitle,getparm("a"),32);
                if(buftitle[0])
                        renameitem(path,buftitle);
                goto E;
        }

        if(mode==7) {
                int i=atoi(getparm("a"));
                if(i!=0)
                        moveitem(path,i);
                goto E;
        }

E:
        showinfo("�������,����");
}
