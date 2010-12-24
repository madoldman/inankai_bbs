/*$Id: bbslogin.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"
#ifndef	EMAILAUTH //   "AuthMaster"
#define  EMAILAUTH    "AuthMaster"
#define  EMAILAUTHB	'A'
#endif
//add by brew
int _Readln2( FILE* fp, char *buf ,int bufsize)
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
    
		buf[ retl++ ] = cn;
		
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
		}
		if(bufsize-1<=retl)
		{
			buf[ retl ] = 0;
			return retl;
		}
	}
	while( 1 );
	return 1;
}
int del_authrow_has_userid(char *file, char *word) {
    FILE *fp1;
    FILE *fp2;
    char buf[1024];
    char file2[STRLEN];
    char wordd[STRLEN+20];
    char *pcmpbuf;
    int  count=0; 
	int  bfount=0;
	strcpy(wordd,"nankai.edu.cn,");
	strcat(wordd,word);
	strcat(wordd,",");
    fp1=fopen(file, "r"); 
	sprintf(file2,"%s.tmp",file);
	fp2=fopen(file2,"w");
     flock(fileno(fp1),LOCK_EX);
     flock(fileno(fp2),LOCK_EX);
	
    if(fp1==0||fp2==0) return 0;//mail/T/email.auth
	while(1) { 
            bzero(buf, 1024);
            if(_Readln2(fp1,buf,1024)>0) 
            {
                if(bfount==0)
                {
			
			pcmpbuf=strstr(buf,"nankai.edu.cn");
			if(buf[strlen(buf)-1]!='\n')
			   strcat(buf,"\n");
			if(strstr(pcmpbuf,wordd)!=0)
				bfount = 1;
			else
			     fwrite(buf,strlen(buf),1,fp2);
                } 
		else
		{
			if(buf[strlen(buf)-1]!='\n')
                           strcat(buf,"\n");
			fwrite(buf,strlen(buf),1,fp2);
		}
            }
            else
              break;
    }
	fflush(fp1);
	fflush(fp2);
       // sprintf(buf,"cp -f %s %s",file2,file);
       // system(buf);
    rename(file2,file);
    flock(fileno(fp1),LOCK_UN);
    flock(fileno(fp2),LOCK_UN);
    fclose(fp1);
    fclose(fp2);
    return 0;

}
int updateMailAuthFiles(char *userid,char *emailaddr)
{
     char filename[STRLEN];
     FILE *fp;
     char buff[1024];
     sprintf(filename, "home/%c/%s/.webauth.U", toupper(userid[0]), userid);
     unlink(filename); 
     sprintf(filename, "home/%c/%s/.webauth.done", toupper(userid[0]), userid);
     f_append(filename,emailaddr);
     fp=fopen(filename, "a"); 
     if(fp) 
     {
      	fprintf(fp, "\nWtime:%d", time(0));
   	fclose(fp);
     }

     sprintf(filename,"mail/%c/email.auth",toupper(emailaddr[0]));   
     sprintf(buff,"%s,%s,%s,%d\n",emailaddr,userid,fromhost,time(NULL));
     fs_append(filename,buff);

     strcat(filename,".U");
     del_authrow_has_userid(filename,userid);
     return 1;
}
int activeMailUserbyCode(char *regcode,char *userid)
{
///*
        char    filename[STRLEN];
        char    authtitle[STRLEN]={0};
        char    authfile[STRLEN]={0};
	char	emailaddr[STRLEN]={0};
	char	*pemail;
	int	i;
        FILE *fp;
	if(regcode==NULL||strlen(regcode)<8)
		return 0;
        sprintf(filename, "home/%c/%s/.webauth.U", toupper(userid[0]), userid);
 
        if(!file_exist(filename)) {
                return 0;
        }
        fp=fopen(filename, "r");
        if(fp==0)
            http_fatal("file error");
        //fread(authfile,STRLEN,1,fp);
        _Readln2(fp,authtitle,STRLEN);
        _Readln2(fp,authfile,STRLEN);
        fclose(fp);
   
        if(strlen(authtitle)==0||strlen(authfile)==0)
        {
                return 0;
        }
	strcpy(emailaddr,&authfile[10+strlen(userid)]);
	pemail=strstr(emailaddr,".cn");
	if(pemail==NULL)return 0;
	pemail[3]=0;
      //  for(i=strlen(emailaddr)-1;i>=0;i--)
         //     if(emailaddr[i]==' '||emailaddr[i]=='\n'||emailaddr[i]=='\r')
           //         emailaddr[i]=0;
 
///http_fatal("e:%s,%s,%s,%s",userid,emailaddr,
//	strstr(	strchr(authtitle,':'),userid)
  //      ,strstr(&authtitle[8],regcode));
	if(strstr(strchr(authtitle,':'),userid)
	&&strstr(strchr(authtitle,':'),regcode))
	{
		sprintf(filename,"WEB Auth Code Succeed: %s,%s,%s from %s\n",userid,emailaddr,Ctime(time(0)),fromhost);
                fs_append("webregtrace",filename);     
//:		http_fatal("e:%s,%s",userid,emailaddr);
		return updateMailAuthFiles(userid,emailaddr);
	}//*/
	return 0;
}
int WebAuthMailExist(char *userid)
{   
	int i,total;
	struct fileheader  fileh;
	char	filename[STRLEN];
	char	authtitle[STRLEN]={0};
	char	authfile[STRLEN]={0};
	char	emailaddr[STRLEN];
	char	buff[STRLEN];
	char 	regcode[10]; 
	FILE *fp;
	sprintf(filename, "home/%c/%s/.webauth.U", toupper(userid[0]), userid);

	if(!file_exist(filename)) {
		return 0;
	}
	fp=fopen(filename, "r");
	if(fp==0)
			http_fatal("file error");
	//fread(authfile,STRLEN,1,fp);
	_Readln2(fp,authtitle,STRLEN);
	_Readln2(fp,authfile,STRLEN);
 	fclose(fp);
	if(strlen(authtitle)==0||strlen(authfile)==0)
	{
		return 0;
	}
	
	sprintf(filename, "mail/%c/%s/.DIR",EMAILAUTHB,EMAILAUTH);
	total=file_size(filename)/sizeof(struct fileheader);
	if(!file_exist(filename)) {
		char buf[80];
		sprintf(buf, "mail/%c/%s", toupper(userid[0]), userid);
		if(!file_exist(buf))
			mkdir(buf, 0770);
		fp=fopen(filename, "a+");
		fclose(fp);
		return 0;
	}	
	fp=fopen(filename, "r");
	if(fp==0)
		http_fatal("dir error");
	///trim
	for(i=strlen(authtitle)-1;i>=0;i--)
		if(authtitle[i]==' '||authtitle[i]=='\n'||authtitle[i]=='\r')
		authtitle[i]=0;
	//http_fatal("dir error:%s:%s:%s:",filename,authfile,authtitle);	
	for(i=0;i<total;i++)
	{
		fread((void*)&fileh, sizeof(struct fileheader), 1, fp);
		if(strstr(fileh.title,authtitle)!=NULL
		 &&strstr(fileh.owner," <")!=NULL)
		{//find it...
			
			//mail check
			strcpy(emailaddr,strstr(fileh.owner," <")+2);
			emailaddr[strlen(emailaddr)-1]=0;
			if(strstr(fileh.owner," <")==NULL)
			 http_fatal("email:%s",emailaddr);
		//	http_fatal("email:err:%s:%s:",emailaddr,
		//		authfile);
			if(strstr(authfile,emailaddr)==NULL||
			   strstr(emailaddr,"@mail.nankai.edu.cn")==NULL)
			     continue;//not match...
			
			//no need regcode check !!
			fclose(fp);
			
			del_record(filename, sizeof(struct fileheader), i);
			sprintf(filename, "mail/%c/%s/.DELW",EMAILAUTHB,EMAILAUTH);
                        append_record(&fileh,sizeof(struct fileheader),filename);
			sprintf(filename,"WEB Auth Email Succeed: %s,%s,%s from %s\n",userid,emailaddr,Ctime(time(0)),fromhost);
			fs_append("webregtrace",filename);
    		        updateMailAuthFiles(userid,emailaddr);	
			return 1;

		}
	}
	fclose(fp);
	//http_fatal("user Not %s:",authfile,filename);
	return 0;
}

int main()
{
        int pid, n, t, i, online=0;
        char buf[256], buf2[256], id[20], pw[20],activecode[20];
        char filename[80], ipshow[32];
        struct userec *x;
        FILE *fp;
        init_all();
        printf("<script language=javascript>");
        printf("function gethost()"
        "{	var s=location.href;	if(s.substring(0,7)!='http://')		s='http://'+s;	s=s.substring(7);	var t='';"
        "	var i;	for(i=0;i<s.length;i++)	{		switch(s.charAt(i))		{			case '/':				return t;				break;"
        "			case '@':			case ':':				t='';				break;			case '=':			case ';':			case ',':			case ' ':"
        "				break;			default:				t+=s.charAt(i);			break;		}	}	return t;}"
        "function init(){	document.cookie='currhost='+gethost();}init();</script>");
        strsncpy(id, getparm("id"), 13);
        strsncpy(pw, getparm("pw"), 13);
	strsncpy(activecode,getparm("acode"),10);
        
	if (!strcasecmp(id, "SYSOP"))
                http_fatal("������SYSOP��Web��ʽ�µ�¼�����˳�");

        for(i=0; i<MAXACTIVE; i++)
                if(shm_utmp->uinfo[i].active)
                        online++;


        if(file_has_word(".bad_host", fromhost)) {
                http_fatal("�Բ���, ��վ����ӭ���� [%s] �ĵ�¼. <br>��������, ����SYSOP��ϵ.", fromhost);
        }
        multi_ip_check(fromhost,online,id); //bluetent
        //huangxu@060408
        if(loginok && strcasecmp(id, currentuser.userid)) {
                http_fatal("ϵͳ��⵽Ŀǰ��ļ�������Ѿ���¼��һ���ʺ� %s�������˳�.(%s)",
                           currentuser.userid, "<a href=/cgi-bin/bbs/bbslogout>������ע����ID</a>������<a href=/index.html>����ص�ԭ��״̬</a>��");
        }
        x=getuser(id);
        if(x==0)
                http_fatal("�����ʹ�����ʺ�");
        sprintf(buf2, "home/%c/%s/badhost", toupper(x->userid[0]), x->userid);
        if(bad_host(fromhost,buf2)) {
                http_fatal("�Բ��𣬴��ʺ��ѱ��趨Ϊ���ɴ� [%s] ��¼��վ.",fromhost);
        }

        if(strcasecmp(id, "guest")) {
                if(!checkpasswd(x->passwd, pw)) {
                        if(pw[0]!=0)
                                sleep(2);
                        getdatestring(time(0),NA);
                        sprintf(buf, "%-12.12s  %-30s %s[Web]\n",id, datestring, fromhost);
                        sprintf(filename, "home/%c/%s/logins.bad", toupper(x->userid[0]), x->userid);
                        f_append(filename, buf);
                        showinfo("�������!");
                        return 1;
                }
                //huangxu@070408:�û���Ŀ¼��ʧ�����
                sprintf(filename, "home/%c/%s", toupper(x->userid[0]), x->userid);
                if (!file_isdir(filename))
                {
                	sprintf(filename, "mkdir home/%c/%s", toupper(x->userid[0]), x->userid);
                	system (filename);
                	report("Web_Rebuild_home");
                	http_fatal("������Ŀ¼�����������ʧ�����ѳ����ؽ���<br />������ǵ�һ�ο�������Ϣ�������µ�¼��<br />\
                	������µ�¼��Ч�����������쳣���뵽BugReport�巴ӳ��");
                }
                if (check_multi(x))
                {
                	http_fatal("��Ǹ������¼�˹����̣߳����ε�¼����ȡ����");
                }

/*
if(!strcmp(x->userid,"ideer")) {
          printf("%d",x->numlogins);
return 0;
}
*/
                if(check_login_limit(x)) http_fatal("���ʺ�24Сʱ����վ�������࣬���Ժ�������");
                selffree(x);
                if(!user_perm(x, PERM_BASIC))
                        http_fatal("���ʺ��ѱ�ͣ��, ��������, ���������ʺ���sysop��ѯ��.");
                if (user_perm(x, PERM_RECLUSION) && !user_perm(x, PERM_SYSOP)) {
                        //modified by bluetent 2002.12.05
                        FILE *fp;
                        char tmp[256];
                        char reclusionbuf[256];
                        char reclusiondaychar[8];
                        int reclusionday=30;
                        sprintf(reclusionbuf, "home/%c/%s/Reclusion", toupper(x->userid[0]), x->userid);
                        fp=fopen(reclusionbuf,"r");
                        if(fp) {
                                fscanf(fp,"%s",reclusiondaychar);
                                fclose(fp);
                        } else
                                strcpy(reclusiondaychar,"30");
                        reclusionday=atoi(reclusiondaychar);
                        //modified end*/

                        if((time(0)-x->lastlogin)/(24*60*60)<reclusionday) {
                                //printf("<script>alert('reclusionday=%d')</script>", reclusionday);
                                http_fatal("���ʺ��Ѿ�����,����û���ʸ��س�����,���ڹ���%d���������״̬", reclusionday);
                        }
                        x->userlevel &= ~PERM_RECLUSION;
                        post_article("notepad", "�س�����--���ֻ�����!!!", "etc/ccjh", x->userid, x->username, fromhost, -1);
                        printf("<script> alert('���Ĺ���״̬�ѽ��!\n�����Լ���ʹ�ô��ʺŵ�½��վ!!!')</script>\n");
                }
		
                t=x->lastlogin;
                x->lastlogin=time(0);
                save_user_data(x);
                if(abs(t-time(0))<5)
                        http_fatal("���ε�¼�������!");
		//check                         //check the web auth users..by brew.....
                sprintf(filename, "home/%c/%s/.webauth.U", toupper(id[0]), id);
		//http_fatal("end1");
                if(file_exist(filename))
                {
		
                        //check whether exist user auth.file
                        if(abs(t-time(0))>20&&(
				activeMailUserbyCode(activecode,id)||
				WebAuthMailExist(id))
				)
                        {//����2����֮�ڲ������ж��ˡ�
				printf("<h1 align=center >��ϲ��������ʺ��ѱ�����:)</h1>");
				printf("<p align=center>�����µ�½����ˢ��ҳ��</p>");
				printf("<center><a href=%s>��ҳ</a></center>",BBSHOST);
				http_quit();
                        }
                        else
                        {
		            printf("<p>�Բ�������ע����Ϣû�еõ�email��֤�����յ�ϵͳ�ĳ��ļ����ź�ֱ�ӻظ���</p>");
 			printf("<p align=center >���������֤�룬��������������֤��</p>");
			printf("<FORM name=bbslogin action=\"bbslogin\" method=post>");
			printf("<p align=center>�û���:	");
			printf("<input maxLength=12 size=10 name=id>");
			printf("��  ��:");
			printf("<input type=password maxLength=8 size=10 name=pw>");
			printf("EMail��֤��:");
			printf("<input maxLength=8 size=10 name=acode>");
			printf("<input type=submit value=\"��¼\" name=Submit>");
			printf("</p></form>");
			printf("<br><br><h3><a href=\"http://%s/ngetcode.html\" align=center >�����û���յ�ϵͳ�������ż�, �����������...</a></h3><br>",BBSHOST);

                        http_fatal("�ʺ���δ����");
                        }
 
                }
                x->numlogins++;
                strsncpy(x->lasthost, fromhost, 17);
                save_user_data(x);
                currentuser=*x;
        }
        getdatestring(time(0),NA);
        sprintf(buf, "%s %s %s\n", datestring, x->userid, fromhost);
        f_append("www.log", buf);
        sprintf(buf, "%s ENTER %-12s @%s [www]\n", datestring, x->userid, fromhost);
        f_append("usies", buf);
        n=0;

 	report("WEBEnter");   
     	//http_fatal("end5)");
	//	if(!loginok && strcasecmp(id, "guest"))
        //if(!loginok)
        int iutmpnum, iutmpkey;
                if (!wwwlogin(x, &iutmpnum, &iutmpkey))//0 : succeed
                {
                        sprintf(buf, "%d", iutmpnum);
                        setcookie("utmpnum", buf);
                        sprintf(buf, "%d", iutmpkey);
                        setcookie("utmpkey", buf);
                        setcookie("utmpuserid", currentuser.userid);
                        set_my_cookie();
                }
                else
                {
                	http_fatal("��Ǹ����¼����̫�࣬���Ժ���������");
                }
        
   /*     if(!strcmp(getparm("redir"), "showfile")&&strcmp(getparm("name"), "")) {
                sprintf(buf, "/cgi-bin/bbs/showfile?name=%s", getparm("name"));
                redirect(buf);
        } else if (!strcmp(getparm("redir"), "tools")) {
                sprintf(buf, "/tools.htm");
                redirect(buf);
        } else*/

//  if(!strcmp(getenv("Referrer"),"treg.htm")) redirect("/cgi-bin/bbs/bbstshirt");
  //else
   


     redirect("/index.html");
}


/* add by bluetent ͬһ IP ������վ�� id ���޶� 2002.01.11 */

int multi_ip_check(fromhost,curr_login_num,uid)
char *fromhost;
int   curr_login_num;
char *uid;
{
        struct  user_info *uentp;
        int i;
        int count=0;
        int countguest=0;
        char buf[256];
        for(i=0;i<curr_login_num;i++) {
                uentp=&(shm_utmp->uinfo[i]);
                if(!uentp->active||!uentp->pid)
                        continue;
                if(!strcmp(uentp->from,fromhost)&&!strcmp(uentp->userid,"guest"))
                        countguest++;
        }

        if(countguest>=MAXGUESTPERIP&&!strcasecmp(uid,"guest")) {
                sprintf(buf, "ͬip��guest��¼�����Ѿ�����%d��,�����߽���ȡ��\n", MAXGUESTPERIP);
                http_fatal(buf);
        }
        /*
           for(i=0;i<curr_login_num;i++){
             uentp=&(shm_utmp->uinfo[i]);
             if(!uentp->active||!uentp->pid)continue; 
             if(!strcmp(uentp->from,fromhost))count++; 
           } 
             if(count>=MAXMULTIPLEIP&&(strstr(fromhost,"10.10.1")||strstr(fromhost,"202.113.239")))
            {
        	sprintf(buf,"ͬip��¼�����Ѿ�����%d��,�����߽���ȡ��\n",MAXMULTIPLEIP);
        	http_fatal(buf);
           } 
        */
}

int selffree(struct userec * u)//huangxu@060714:�Զ���⣬������˵��selffree
{
	char path[128];
	char buf[80];
	FILE * fp;
	char * permfile[]={"","permpost","permbasic","permchat","permpage","permmessage"};
	char * permname[]={"","��������Ȩ��","������վȨ��","����������Ȩ��","������������Ȩ��","�����˷���ϢȨ��"};
	int permlevel[]={0,PERM_POST,PERM_BASIC,PERM_CHAT,PERM_PAGE,PERM_MESSAGE};
	int i;
	for (i=1;i<6;i++)
	{
		if(u->userlevel&permlevel[i])continue;
		sprintf(path,"home/%c/%s/%s",toupper(u->userid[0]),u->userid,permfile[i]);
		if(fp=fopen(path,"r"))
		{
			fgets(buf,9,fp);
			if(time(NULL)-file_time(path)>atoi(buf)*86400)
			{
				u->userlevel|=permlevel[i];
				save_user_data(u);
				sprintf(buf,"�ָ� %s ��%s",u->userid,permname[i]);
				unlink(path);
				sprintf(path, "tmp/%d.tmp", getpid());
				fp=fopen(path, "w");
				fprintf(fp,"%s ���ѣ�\n"
				"\t�����������ָֻ�����%s��\n"
				"\t��л����֧�֡�",u->userid,permname[i]);
				fclose(fp);
				post_article("Punishment", buf, path, u->userid, u->username, fromhost, -1);
				post_mail(u->userid, buf, path, u->userid, u->username, fromhost, -1, 0);
				fp=fopen(path,"w");
				fprintf(fp,"�ָ� %s �� %s \n\n%s",u->userid,permname[i],sprintgetuinfo(*u));
				fclose(fp);
				post_article("syssecurity1", buf, path, u->userid, u->username, fromhost, -1);
				unlink(path);
			}
			else
				fclose(fp);
		}
	}
	return 0;
}
