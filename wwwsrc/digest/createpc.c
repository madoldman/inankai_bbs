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
                showinfo("����Ĳ���.");
                return 1;
        }

        if(!(currentuser.userlevel & PERM_LOGINOK)) {
                showinfo("����û��ͨ��ע�ᣬ��ʱ����ʹ�ô������.");
                return 0;
        }

        if(mode==0) {
                printf("<body background=%s  bgproperties=fixed>",mytheme.bgpath);
                printf("<center>\n");
                printf("%s -- ���������ļ� [ʹ����: %s]<hr color=green></center>", BBSNAME, currentuser.userid);


                printf("<form name=form1  method=post action=createpc?mode=1>\n");

                if(!HAS_PERM(PERM_OBOARDS) )
                        printf("<input type=hidden name=id size=40 maxlength=12 value=%s>",currentuser.userid);
                else
                        printf("������ID:<input type=text  name=id size=40 maxlength=12>(�û�id12���ַ�)");
                printf("<br>�ļ�����:<input type=text  size=40 maxlength=32  name=name>(���32���ַ�)");
                printf("<br><input type=submit class=btn value='��  ��'>");
                printf(" </form>\n");
                printf("</body></html>");
                return 0;
        }
                char id[16],name[40];
                strsncpy(id, getparm("id"),13);
                strsncpy(name, getparm("name"),32);
                if(!strcmp(id,"")) {
                        showinfo("����д�û�id!");
                        return 1;
                }

                if(!HAS_PERM(PERM_OBOARDS)&&strcmp(id,currentuser.userid)) {
                        showinfo("����Ȩ���д˲���!");
                        return 1;
                }


                struct userec *x;
                x=getuser(id);
                if(x==0) {
                        showinfo("���û�������!");
                        return 1;
                }
         char file[256],buf[256];
         sprintf(file,"0Announce/PersonalCorpus/%c/%s/.Names",toupper(id[0]),id);
	 if(mode==1){
	        strsncpy(name, getparm("name"),32);
                if(!strcmp(name,"")){
                      showinfo("����д�ļ�����!");
                      return 1;
		}
                if(file_exist(file)) {
                        showinfo("%s���ļ��Ѿ�����!",id);
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
                        showinfo("��ϲ!�����ɹ�!<br>Id:%s<br>����:%s",id,name);
                        char path[80];
                        char title[240];
                        FILE * fp;
                        sprintf(path, "tmp/%d.tmp", getpid());
                        fp=fopen(path, "w");
                        getuinfo(fp);
                        fclose(fp);
                        sprintf(title,"���������ļ� %s",currentuser.userid);
                        post_article("syssecurity1", title, path, currentuser.userid, currentuser.username, "�Զ�����ϵͳ", -1);
                        unlink(path);
                }
                else
                        showinfo("�����û���Ϣʧ��!");
                return 0;
        }

        if(mode==2){
             if(!file_exist(file)) {
                   showinfo("%s���ļ�������!",id);
                   return 1;
             }
             sprintf(buf,"rm -rf 0Announce/PersonalCorpus/%c/%s",toupper(id[0]),id);
// printf("%s",buf);return 0;
             system(buf);
             x->userlevel &= ~ PERM_PERSONAL;
             save_user_data(x);
	     printf("%s���ļ��ѱ�ɾ��!<br><font color=red>http://202.113.16.117/person/%c/%s.html.tgz</font>Ϊ����յı���,һ��󽫻ᱻϵͳɾ��.",id,toupper(id[0]),id);
            
	     sprintf(file,"0Announce/PersonalCorpus/%c/.Names",toupper(id[0]));
	     char tmp[80];
	     sprintf(tmp,"0Announce/PersonalCorpus/%c/.Names.tmp",toupper(id[0]));
	     FILE *fp=fopen(file,"r+");
             FILE *ftmp=fopen(tmp,"w+");
             if(fp==0||ftmp==0){
                printf("�ϼ�Ŀ¼û�и���.");return -1;
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
