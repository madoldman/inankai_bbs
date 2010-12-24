#include "BBSLIB.inc"
#include "string.h"
#include "mail.h"

int _Readln3( FILE* fp, char *buf ,int bufsize)
{
    int retl = 0;
    char cn = '\0';
    do
     {
                if( (cn=fgetc(fp)) == EOF )
                {
                        buf[retl]=0;
                        return retl;
                }
		
                switch(cn)
                {
                case '\n':
                case '\r':
		        if( retl )
                        {
                                buf[ retl ] = 0;
                                return retl;
                        }
                        break;
		default:
			buf[ retl++ ] = cn;
			if(bufsize-1<=retl)
	                {
        	                buf[ retl ] = 0;
                	        return retl;
                	}

			break;
                }
        }
        while( 1 );
        return 1;
}
int file_get_emailbyid(char *file,char *userid,char *emailaddr)
{
        FILE *fp;
        char buf[1024];
        char *lpbuf;
        int  count=0;
        fp=fopen(file, "r");

        if(fp==0) return 0;//mail/T/email.auth
        while(1) {
                if(_Readln3(fp,buf,1024)>0)
                {
                        lpbuf=strstr(buf,".edu.cn,");
                        lpbuf[7]=0;
                        lpbuf+=8;
                       ;// http_fatal("get Buff:%s,%s",buf,lpbuf);
                        if(!strncasecmp(lpbuf, userid,strlen(userid)))
                        {
                          fclose(fp);
                          strcpy(emailaddr,buf);
                          return 1;
                        }
                }
                else
                break;
        //      strcat(readbuff,":\r\n");
        }
        fclose(fp);
        return 0;
}
 

int GetMailFile(char *userid,char *authfile,char *authtitle)
{
        char    filename[STRLEN];
        int     i;
        FILE *fp;
        sprintf(filename, "home/%c/%s/.webauth.U", toupper(userid[0]), userid);
 
        if(!file_exist(filename)) {
                return 0;
        }
        fp=fopen(filename, "r");
        if(fp==0)
            http_fatal("file error");
        _Readln3(fp,authtitle,STRLEN);
        _Readln3(fp,authfile,STRLEN);
        fclose(fp);
        if(strlen(authtitle)==0||strlen(authfile)==0)
        {
                return 0;
        }
	
       	for(i=strlen(authfile)-1;i>=0;i--)
           if(authfile[i]==' '||authfile[i]=='\n'||authfile[i]=='\r')
               authfile[i]=0;
     	for(i=strlen(authtitle)-1;i>=0;i--)
           if(authtitle[i]==' '||authtitle[i]=='\n'||authtitle[i]=='\r')
               authtitle[i]=0;
	//http_fatal("test2:%s,%s.",authtitle,authfile);
	sprintf(filename,"home/%c/%s/%s",toupper(userid[0]), userid,authfile);
 	if(!file_exist(filename))
        {//������ǰ�Ķ���
 	   sprintf(filename,"home/%c/%s/.webauth",toupper(userid[0]), userid);	
           strcat(filename,strrchr(authtitle,'.'));
	   strcat(filename,&authfile[9+strlen(userid)]);
	   strcpy(authfile,filename);
	  // http_fatal("test2:%s,%s.",authtitle,authfile);
	   if(!file_exist(authfile))
		return 0;	
        }
	strcpy(authfile,filename);	
	//http_fatal("test:%s,%s",authtitle,authfile);

	return 1;
}
 
int main()
{
        int pid, n, t, i, online=0;
        char buf[256], buf2[256], id[20], pw[20],activecode[20],iemail[STRLEN];
        
        char filename[80],email[STRLEN];
        char authtitle[STRLEN],authfile[STRLEN];
        struct userec *x;
        FILE *fp;
        init_all();
        strsncpy(id, getparm("id"), 13);
        strsncpy(pw, getparm("pw"), 13);
        strsncpy(email, getparm("email"), STRLEN-13);
        if(strlen(id)==0)
        {
        	http_fatal("�û�������Ϊ��");
        }
        if(strlen(pw)==0)
        {
        	http_fatal("���벻��Ϊ��");
        }
        if(strlen(email)==0)
        {
        	http_fatal("�������䲻��Ϊ��");
        }
        if(!isvalidEmail(email)||
           (strlen(strstr(email,".edu.cn"))!=7)||
		strstr(email,"bbs.nankai.edu.cn")!=NULL)
        {
           http_fatal("ʮ�ֶԲ��𣬱�վ��ʱ���������û��Լ��Ǳ�У�����û��ṩע����񡣻�ӭ����л�����ٱ�վ���㲻���Ĵ�������^_^%s,%s",email,fromhost);
        }
        
        if (!strcasecmp(id, "SYSOP"))
                http_fatal("������SYSOP��Web��ʽ�µ�¼�����˳�");
 
        for(i=0; i<MAXACTIVE; i++)
                if(shm_utmp->uinfo[i].active)
                        online++;
 
 
 
        if(loginok && strcasecmp(id, currentuser.userid)) {
                http_fatal("ϵͳ��⵽Ŀǰ��ļ�������Ѿ���¼��һ���ʺ� %s�������˳�.(%s)",
                           currentuser.userid, "ѡ������logout, ���߹ر��������������");
        }
        x=getuser(id);
        if(x==0)
                http_fatal("�����ʹ�����ʺ�");
 
        if(strcasecmp(id, "guest")==0)
	http_fatal("�Բ��𣬴��ʺŲ��ṩ��֤����.");
        if(!checkpasswd(x->passwd, pw)) {
                if(pw[0]!=0)
                        sleep(2);
                getdatestring(time(0),NA);
                sprintf(buf, "%-12.12s  %-30s %s[Web]\n",id, datestring, fromhost);
                sprintf(filename, "home/%c/%s/logins.bad", toupper(x->userid[0]), x->userid);
                f_append(filename, buf);
                http_fatal("����!����:101");
                return 1;
        }

        if(check_login_limit(x)) return 0;

        if(!user_perm(x, PERM_BASIC))
                http_fatal("���ʺ��ѱ�ͣ��, ��������, ���������ʺ���sysop��ѯ��.");
        t=x->lastlogin;
        x->lastlogin=time(0);
        save_user_data(x);
        if(abs(t-time(0))<5)
                http_fatal("���ε�¼�������!");
        //check                         //check the web auth users..by brew.....
        sprintf(filename, "home/%c/%s/.webauth.U", toupper(id[0]), id);
        if(file_exist(filename))
        {
                //check whether exist user auth.file
                if(abs(t-time(0))<2*60*0)                            
                {//����2����֮�ڲ������ж��ˡ�
                        printf("<p align=center>ϵͳ���ڸ���������һ���ʼ���</p>");
                        printf("<p align=center>���Ժ����ԡ���</p>");
                        printf("<center><a href=http://bbs.nankai.edu.cn>��ҳ</a></center>");
                        http_quit();
                }
                else
                {
                //int file_get_emailbyid(char *file,char *userid,char *emailaddr)
                    //check email	
                    sprintf(filename, "mail/%c/email.auth.U", toupper(email[0]), email);
                    if(file_get_emailbyid(filename,id,iemail)==0||
                    strcasecmp(iemail,email))	
                         http_fatal("�����ַ����ȷ");
                    //send mail...
                    if(GetMailFile(id,authfile,authtitle)==0)
                         http_fatal("�ڲ�����:102,����ϵվ��ϵͳά��");
                    //http_fatal("sending %s,%s,%s,%s",email,id,authfile,authtitle); 
                    
                    post_imail(iemail,authtitle,
                                authfile,"AuthMaster",BBSNAME,BBSHOST,1);

                    printf("<p align=center>ϵͳ�Ѿ��ɹ��ĳ��ļ����š�</p>");         
                    printf("<center><a href=http://bbs.nankai.edu.cn>��ҳ</a></center>");
                    http_quit();
                }

        }
        else
        {
            sprintf(filename, "home/%c/%s/.webauth.done", toupper(id[0]), id);
    	    if(file_exist(filename))
    	    {
    	    	printf("<p align=center>�����ʺ��Ѿ����������ȷ�����ˣ�</p>");
                printf("<center><a href=http://bbs.nankai.edu.cn>��ҳ</a></center>");
                http_quit();
    	    }
    	    else
    	    {
    	        printf("<p align=center>�Բ���,��û����������������֤��</p>");
    	        printf("<p align=center>�����ʲô����,����ϵվ��ϵͳά��!</p>");
    	    	http_quit();
    	    }
        }
  }
                                                    
