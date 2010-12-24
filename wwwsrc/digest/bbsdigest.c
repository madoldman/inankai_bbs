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
                printf("目录不存在");
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
                printf("顺序一样,没有移动");
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
        if(index_infile[0]<num)//后移
                for(i=0;i<total;i++) {
                        if(i==(index_infile[0]-1))
                                continue;
                        additem(fp,a[i].title,a[i].path,++j);
                        if((i+1)==num)
                                additem(fp,a[index_infile[0]-1].title,a[index_infile[0]-1].path,++j);

                }
        else//前移
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
                 printf("该档案被标记为复制,不能删除");
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
                                                printf("<br>删除%s",a[i].title);
                                        } else {
                                                printf("<br>注意:您只可以删除空目录.<font color=green>%s</font>不空,跳过.",a[i].title);
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
                printf("您的粘贴板为空.");
                return 1;
        }
        FILE *fp=fopen(buf,"r");
        sprintf(buf,"%s/.Names",path);
        FILE *names=fopen(buf,"a+");
        if(fp==0||names==0) {
                printf("打开文件失败.<br>");
                return 1;
        }

        while(1) {
                if(fread(&item,sizeof(struct cpitem),1,fp)<=0)
                        break;
                sprintf(file,"%s/%s",item.fpath,item.filename);
                if(strstr(path,file)) {
                        printf("<br>%s %s :不能复制进子目录.跳过",path,file);
                        continue;
                }
                if(!file_exist(file)) {
                        printf("%s %s 不存在,可能已被删除.",item.title,item.filename);
                        continue;
                }
                sprintf(cmd,"%s/%s",path,item.filename);
                if(file_exist(cmd)) {
                        printf("<br>目标 %s 存在.",item.title);
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
                printf("您还没有设定丝路.");
                return 1;
        }
        fscanf(fp,"%s",buf);
        fclose(fp);
        if(!file_exist(buf)) {
                printf("无效的路径:%s.丝路已丢失,请重新设定.",buf);
                return 1;
        }
        strcpy(path,buf);
        return 0;
}

int showroad()
{
        char buf[PATHLEN],path[1024];
        if(!getroad(buf)) {
                printf("当前丝路为%s",buf);
                cnpath(buf,path);
                printf("<br>%s",path);
        }
        printf("<br>");
        printf("<font color=red>什么是丝路?</font><br><p>丝路就是一个路径,当您在板面内把文章收入精华区时, 都被放到该目录下了.您可以在精华区的管理模式,改变丝路的设定,把当前目录设定为丝路</p>");
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
                printf("太多的项目,创建失败");
                return 1;
        }







        char file[PATHLEN],name[12];
        sprintf(file,"%s/.Names",path);
        FILE *fp=fopen(file,"a+");
        if(fp==0) {
                showinfo("不能打开文件.Names");
                return 1;
        }
        if(!strcmp(title,"welcome"))
                strcpy(name,"welcome");
        else
                createname(name);
        sprintf(file,"%s/%s",path,name);
        if(file_exist(file)) {
                printf("%s存在,不能创建",file);
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
                printf("不能打开文件%s",file);
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

        if(mode==1) {//查看丝路
                if(!HAS_PERM(PERM_OBOARDS) && !HAS_PERM(PERM_BOARDS) ) {
                        printf("您没有自己的丝路！");
                        goto E;
                }
                showroad();
                goto E;

        }

        char linkpath[PATHLEN];
        strncpy(linkpath,getparm("path"),PATHLEN);
        if(!strcmp(linkpath,"")||strstr(linkpath, "..") || strstr(linkpath, "SYSHome"))
                http_fatal("此目录不存在!");
        char path[PATHLEN];
        sprintf(path,"0Announce%s",linkpath);
        if(!file_exist(path)) {
                printf("%s",path);
                showinfo("此目录不存在!");
                return 1;
        }

        char s[20];
        int r=getfrompath(linkpath,s);
        if(!hasperm(r,s)) {
                showinfo("您无权访问该页!");
                return 0;
        }


        if(mode==2) {//新建项目
                printf("<script>\
                       function check(type) {\
                       if(document.form1.title.value.length<1){alert('请输入名字!'); return false;}\
                       document.form1.type.value=type;\
                       return true;\
               }\
                       </script>");
               // <input type=submit class=btn value=创建文件 onclick='return check(1)'>
                printf("<form name=form1 method=post action=bbsdigest?mode=12&path=%s>",linkpath);
                printf("请输入该项目的名字(32个字符)<br>\
                       <input  type=text name=title size=32 maxlength=32 value=''>\
                       <input type=hidden name=type>\
                 \
                       <input type=submit class=btn value=创建目录 onclick='return check(2)'> </form>");
                printf("<br><br>要创建一个只有自己可以访问的隐藏目录,项目的名字请以<font color=green>&lt;HIDE&gt;</font>开头<br>如果目录名字以<font color=green>&lt;GUESTBOOK&gt;</font>开头,我们将会为您创建一个留言本<br><a href=bbs0an?mode=1&path=%s>返回本页管理模式</a>",linkpath);
                return 0;
        }

        if(mode==4) {//粘贴
                pasteitem(path);
                goto E;
        }

        if(mode==11) {//设定丝路
//                if(r==2) {
//                        printf("个人文集没有丝路操作");
//                        goto E;//个人文集不能设定丝路。
//                }huangxu@060715:为嘛聂
                char buf[40];
                FILE *fp;
                sethomefile(buf, currentuser.userid,".announcepath");
                if((fp = fopen(buf, "w+")) == NULL ) {
                        printf("%s创建失败",buf);
                        return 1;
                }
                fprintf(fp,"0Announce%s",linkpath);
                fclose(fp);
                goto E;
        }

        if(mode==12) {//新建文件,目录

                denyrobot();

                int i=0;
                int type=atoi(getparm("type"));
              /*  if(type==1)
                        i=15;
                else if (type==2)
                        i=25;
                else {
                        showinfo("请选择文件或者目录.");
                        return 1;
                }*/
                i=25;
                if(strlen(path)>=(PATHLEN-i)) {
                        showinfo("目录结构太深,无法创建.");
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
                showinfo("请先选定文章<br>\n");
                return 0;
        }

        if(mode==3) {
                if(copyitem(path))
                        printf("系统出错.");
                goto E;
        }

        if(mode==5) {
                if(delitem(path))
                        printf("系统出错.");
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
        showinfo("操作完成,返回");
}
