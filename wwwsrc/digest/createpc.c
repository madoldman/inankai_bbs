/*$Id: createpc.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
 */
#include "BBSLIB.inc"
#include "digest.h"
int main()
{
        init_all();
        //modify_mode(u_info, ADMIN+20000);
        int mode=atoi(getparm("mode"));
        if(mode<0||mode>2) {
                showinfo("错误的参数.");
                return 1;
        }

        if(!(currentuser.userlevel & PERM_LOGINOK)) {
                showinfo("您还没有通过注册，暂时不能使用此项服务.");
                return 0;
        }

        if(mode==0) {
                printf("<body background=%s  bgproperties=fixed>",mytheme.bgpath);
                printf("<center>\n");
                printf("%s -- 创建个人文集 [使用者: %s]<hr color=green></center>", BBSNAME, currentuser.userid);


                printf("<form name=form1  method=post action=createpc?mode=1>\n");

                if(!HAS_PERM(PERM_OBOARDS) )
                        printf("<input type=hidden name=id size=40 maxlength=12 value=%s>",currentuser.userid);
                else
                        printf("所有者ID:<input type=text  name=id size=40 maxlength=12>(用户id12个字符)");
                printf("<br>文集名称:<input type=text  size=40 maxlength=32  name=name>(最多32个字符)");
                printf("<br><input type=submit class=btn value='创  建'>");
                printf(" </form>\n");
                printf("</body></html>");
                return 0;
        }
                char id[16],name[40];
                strsncpy(id, getparm("id"),13);
                strsncpy(name, getparm("name"),32);
                if(!strcmp(id,"")) {
                        showinfo("请填写用户id!");
                        return 1;
                }

                if(!HAS_PERM(PERM_OBOARDS)&&strcmp(id,currentuser.userid)) {
                        showinfo("您无权进行此操作!");
                        return 1;
                }


                struct userec *x;
                x=getuser(id);
                if(x==0) {
                        showinfo("该用户不存在!");
                        return 1;
                }
         char file[256],buf[256];
         sprintf(file,"0Announce/PersonalCorpus/%c/%s/.Names",toupper(id[0]),id);
	 if(mode==1){
	        strsncpy(name, getparm("name"),32);
                if(!strcmp(name,"")){
                      showinfo("请填写文集名称!");
                      return 1;
		}
                if(file_exist(file)) {
                        showinfo("%s的文集已经存在!",id);
                        return 1;
                }
                sprintf(buf,"0Announce/PersonalCorpus/%c/%s",toupper(id[0]),id);
                mkdir(buf,0755);
                sprintf(buf,"#\n# Title=%-38.38s\n#\n",name);
                f_append(file,buf);


                sprintf(buf,"Name=%-38.38s(BM: %s)\nPath=~/%s\nNumb=\n#\n",name,id,id);
                sprintf(file,"0Announce/PersonalCorpus/%c/.Names",toupper(id[0]));
                f_append(file,buf);


                getdatestring(time(0), NA);
                sprintf(file,"0Announce/PersonalCorpus/Log");
                sprintf(buf,"%-12.12s    %14.14s    %.38s\n",id, datestring,name);
                f_append(file,buf);

                x->userlevel |= PERM_PERSONAL;
                if(save_user_data(x))
                {
                        showinfo("恭喜!创建成功!<br>Id:%s<br>名称:%s",id,name);
                        char path[80];
                        char title[240];
                        FILE * fp;
                        sprintf(path, "tmp/%d.tmp", getpid());
                        fp=fopen(path, "w");
                        getuinfo(fp);
                        fclose(fp);
                        sprintf(title,"创建个人文集 %s",currentuser.userid);
                        post_article("syssecurity1", title, path, currentuser.userid, currentuser.username, "自动发信系统", -1);
                        unlink(path);
                }
                else
                        showinfo("更新用户信息失败!");
                return 0;
        }

        if(mode==2){
             if(!file_exist(file)) {
                   showinfo("%s的文集不存在!",id);
                   return 1;
             }
             sprintf(buf,"rm -rf 0Announce/PersonalCorpus/%c/%s",toupper(id[0]),id);
// printf("%s",buf);return 0;
             system(buf);
             x->userlevel &= ~ PERM_PERSONAL;
             save_user_data(x);
	     printf("%s的文集已被删除!<br><font color=red>http://202.113.16.117/person/%c/%s.html.tgz</font>为其今日的备份,一天后将会被系统删除.",id,toupper(id[0]),id);
            
	     sprintf(file,"0Announce/PersonalCorpus/%c/.Names",toupper(id[0]));
	     char tmp[80];
	     sprintf(tmp,"0Announce/PersonalCorpus/%c/.Names.tmp",toupper(id[0]));
	     FILE *fp=fopen(file,"r+");
             FILE *ftmp=fopen(tmp,"w+");
             if(fp==0||ftmp==0){
                printf("上级目录没有更新.");return -1;
             }
             char buf2[256],last[256];
             sprintf(buf,"Path=~/%s\n",id);
  
//printf("%s",buf);return 0;           
             strcpy(buf2,"");
             while(1){
                  strcpy(last,buf2);
                  if (fgets(buf2, 255, fp)<= 0)  break;
	          //printf("last:%s\n buf2:%s",last,buf2);
		  if (!strcmp(buf2,buf)){
                       if(fgets(buf2, 255, fp)<= 0) break;
		       if (fgets(buf2, 255, fp)<= 0)  break;
                       if (fgets(buf2, 255, fp)<= 0)  break;
                  }
                  else
                       fprintf(ftmp,"%s",last);	
            }
            fprintf(ftmp,"%s",last);        
            fcloseall();
            sprintf(buf,"cd 0Announce/PersonalCorpus/%c/;mv .Names.tmp .Names",toupper(id[0]));
            //printf("%s",buf); 
	   system(buf);
            
  }

} 
