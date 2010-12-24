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
        {//兼容以前的东西
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
        	http_fatal("用户名不能为空");
        }
        if(strlen(pw)==0)
        {
        	http_fatal("密码不能为空");
        }
        if(strlen(email)==0)
        {
        	http_fatal("电子邮箱不能为空");
        }
        if(!isvalidEmail(email)||
           (strlen(strstr(email,".edu.cn"))!=7)||
		strstr(email,"bbs.nankai.edu.cn")!=NULL)
        {
           http_fatal("十分对不起，本站暂时不对外网用户以及非本校邮箱用户提供注册服务。欢迎并感谢您光临本站，你不妨四处看看先^_^%s,%s",email,fromhost);
        }
        
        if (!strcasecmp(id, "SYSOP"))
                http_fatal("不允许SYSOP在Web方式下登录，请退出");
 
        for(i=0; i<MAXACTIVE; i++)
                if(shm_utmp->uinfo[i].active)
                        online++;
 
 
 
        if(loginok && strcasecmp(id, currentuser.userid)) {
                http_fatal("系统检测到目前你的计算机上已经登录有一个帐号 %s，请先退出.(%s)",
                           currentuser.userid, "选择正常logout, 或者关闭所有浏览器窗口");
        }
        x=getuser(id);
        if(x==0)
                http_fatal("错误的使用者帐号");
 
        if(strcasecmp(id, "guest")==0)
	http_fatal("对不起，此帐号不提供认证服务.");
        if(!checkpasswd(x->passwd, pw)) {
                if(pw[0]!=0)
                        sleep(2);
                getdatestring(time(0),NA);
                sprintf(buf, "%-12.12s  %-30s %s[Web]\n",id, datestring, fromhost);
                sprintf(filename, "home/%c/%s/logins.bad", toupper(x->userid[0]), x->userid);
                f_append(filename, buf);
                http_fatal("错误!代码:101");
                return 1;
        }

        if(check_login_limit(x)) return 0;

        if(!user_perm(x, PERM_BASIC))
                http_fatal("此帐号已被停机, 若有疑问, 请用其他帐号在sysop版询问.");
        t=x->lastlogin;
        x->lastlogin=time(0);
        save_user_data(x);
        if(abs(t-time(0))<5)
                http_fatal("两次登录间隔过密!");
        //check                         //check the web auth users..by brew.....
        sprintf(filename, "home/%c/%s/.webauth.U", toupper(id[0]), id);
        if(file_exist(filename))
        {
                //check whether exist user auth.file
                if(abs(t-time(0))<2*60*0)                            
                {//限制2分钟之内不允许判断了。
                        printf("<p align=center>系统正在给您发送上一封邮件！</p>");
                        printf("<p align=center>请稍后再试……</p>");
                        printf("<center><a href=http://bbs.nankai.edu.cn>主页</a></center>");
                        http_quit();
                }
                else
                {
                //int file_get_emailbyid(char *file,char *userid,char *emailaddr)
                    //check email	
                    sprintf(filename, "mail/%c/email.auth.U", toupper(email[0]), email);
                    if(file_get_emailbyid(filename,id,iemail)==0||
                    strcasecmp(iemail,email))	
                         http_fatal("邮箱地址不正确");
                    //send mail...
                    if(GetMailFile(id,authfile,authtitle)==0)
                         http_fatal("内部错误:102,请联系站内系统维护");
                    //http_fatal("sending %s,%s,%s,%s",email,id,authfile,authtitle); 
                    
                    post_imail(iemail,authtitle,
                                authfile,"AuthMaster",BBSNAME,BBSHOST,1);

                    printf("<p align=center>系统已经成功寄出的激活信。</p>");         
                    printf("<center><a href=http://bbs.nankai.edu.cn>主页</a></center>");
                    http_quit();
                }

        }
        else
        {
            sprintf(filename, "home/%c/%s/.webauth.done", toupper(id[0]), id);
    	    if(file_exist(filename))
    	    {
    	    	printf("<p align=center>您的帐号已经被激活，无需确认信了！</p>");
                printf("<center><a href=http://bbs.nankai.edu.cn>主页</a></center>");
                http_quit();
    	    }
    	    else
    	    {
    	        printf("<p align=center>对不起,您没有申请外网发文认证！</p>");
    	        printf("<p align=center>如果有什么问题,请联系站内系统维护!</p>");
    	    	http_quit();
    	    }
        }
  }
                                                    
