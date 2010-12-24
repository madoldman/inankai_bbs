/*$Id: bbsdoreg.c,v 1.5 2009-01-09 05:20:16 maybeonly Exp $
*/
#include "BBSLIB.inc"
#include "mail.h"
#include "string.h"

int
countusers(uentp)
struct userec *uentp;
{
        static int totalusers;
        char    permstr[11];
        if (uentp == NULL) {
                int     c = totalusers;
                totalusers = 0;
                return c;
        }
        if (uentp->numlogins != 0 && uleveltochar(permstr, uentp->userlevel) != 0)
                totalusers++;
        return 0;
}
//End
/*int is_bad_id(char *s) {
	FILE *fp;
	char buf[256], buf2[256];
	fp=fopen(".badname", "r");
	if(fp==0) return 0;
	while(1) {
		if(fgets(buf, 250, fp)==0) break;
		if(sscanf(buf, "%s", buf2)!=1) continue;
		if(strcasestr(s, buf2)) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}
*/
/*
//modified by bluetent 2002.12.22
void str2lower( char *dst, char *src )
{
    for( ; *src; src++ )
       *dst++ = tolower( *src );
    *dst = '\0';
}
*/
//char readbuff[10240]={0};
int _Readln( FILE* fp, char *buf ,int bufsize)
{
    	int retl = 0;
	char cn = '\0';
    	do
	{
		if( (cn=fgetc( fp)) ==EOF )
		{
//			http_fatal("err:%c", cn);
			buf[retl]=0;
			return retl;
		}
   		//http_fatal("err:%c", cn);
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
//modified end
int badymd(int y, int m, int d)
{
        int max[]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if((y%4==0 && y%100!=0) || y%400==0)
                max[2]=29;
        if(y<10 || y>100 || m<1 || m>12)
                return 1;
        if(d<0 || d>max[m])
                return 1;
        return 0;
}
int file_get_emailCnt(char *file,char *word)
{
        FILE *fp;
        char buf[1024];
        int  count=0;
//	strcpy(readbuff,"buff:=");
        fp=fopen(file, "r");
        if(fp==0) return -1;//mail/T/email.auth
        while(1) {
                bzero(buf, 1024);
                if(_Readln(fp,buf,1024)>0)
		{
  //      	      strcat(readbuff,buf);
			  if(!strncasecmp(buf, word,strlen(word))) 
			{
			  count++;
 			}
                }
		else
		  break;
	//	strcat(readbuff,":\r\n");
        }
        fclose(fp);
        return count;
}

int file_has_email(char *file, char *word) {
	FILE *fp;
	char buf[256], buf2[256];
	fp=fopen(file, "r");

 	if(fp==0) return -1;//mail/T/email.auth
	while(1) {
		if(_Readln(fp,buf,1024)>0)
		{
		    if(!strcasecmp(buf, word)) {
			fclose(fp);
			return 1;
		    }
		}
		else break;
	}
	fclose(fp);
	return 0;
}

int main()
{
        FILE *fp;
        struct userec x;
        int i, gender, xz,regtime;
        char buf[80], filename[80], pass1[80], pass2[80], dept[80], phone[80], assoc[80], salt[80], words[1024],regcodebuf[10];
	char inv[40], nkbbspw[10];
	const char public_word[]="abCEjuzoljwkuwowrlknvslfjl2894jklr382798hvhy243278ikjk_jkflsouynangowovz";
	char mail_title[1024];
	init_all();
        bzero(&x, sizeof(x));
        xz=atoi(getparm("xz"));
        int mode=atoi(getparm("mode"));
	strsncpy(x.userid, getparm("userid"), 13);
	strcpy(currentuser.userid, x.userid);
	
       if(!isAllowedIp(fromhost))
        {
		strsncpy(x.email, getparm("email"), STRLEN-13);
		if(!isvalidEmail(x.email)||
			!endswith(x.email,".edu.cn"))
		{
			http_fatal("十分对不起，本站暂时不对外网用户以及非本校邮箱用户提供注册服务。欢迎并感谢您光临本站，你不妨四处看看先^_^%s,%s",x.email,fromhost);
		}
		sprintf(filename,"mail/%c/email.auth",toupper(x.email[0]));
		i=file_get_emailCnt(filename,x.email);
		if(i>=MAX_EMAIL_AUTH_SUPPORT)
		{
			http_fatal("对不起，过多的id已经通过该邮箱验证。");
		}
		strcat(filename,".U");
		i+=file_get_emailCnt(filename,x.email);
                if(i>=MAX_EMAIL_AUTH_SUPPORT)
                {
                        http_fatal("对不起，您的邮箱已经申请过多的id。");
                }
		if(file_get_emailCnt(filename,fromhost)>=50)
		{
	          http_fatal("对不起，您的ip地址已经申请过多的id。");
		}
	//	if(strstr(x.email,"tanyy@mail.nankai.edu.cn")==NULL)
	  //      	http_fatal("对不起，尚无此功能");

//		http_fatal("test %d:%s:%s:<br>%s",i,filename,x.email,readbuff);
	}
 
        strsncpy(pass1, getparm("pass1"), 13);
        strsncpy(pass2, getparm("pass2"), 13);
        strsncpy(x.username, getparm("username"), 32);
        strsncpy(x.realname, getparm("realname"), 32);
        strsncpy(dept, getparm("dept"), 75);
        strsncpy(x.address, getparm("address"), 32);
        strsncpy(x.email, getparm("email"), STRLEN-13);
        strsncpy(phone, getparm("phone"), 32);
        strsncpy(assoc, getparm("assoc"), 32);
        strsncpy(words, getparm("words"), 1000);
	strsncpy(inv, getparm("invitation"), sizeof(inv));
	strsncpy(nkbbspw, getparm("nkbbspw"), sizeof(nkbbspw));
        x.gender='M';
        if(atoi(getparm("gender")))
                x.gender='F';
        x.birthyear=atoi(getparm("year"))-1900;
        x.birthmonth=atoi(getparm("month"));
        x.birthday=atoi(getparm("day"));
        for(i=0; x.userid[i]; i++)
                if(!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", toupper(x.userid[i])))
                        http_fatal("帐号只能由英文字母组成");
        if(strlen(x.userid)<2)
                http_fatal("帐号长度太短(2-12字符)");
        if(is_bad_id(x.userid))
                http_fatal("不雅帐号或禁止注册的id, 请重新选择");
        if(getuser(x.userid))
                http_fatal("此帐号已经有人使用,请重新选择。");
        if(mode==1){
           sprintf(buf,"/bbsreg.php?userid=%s&email=%s", x.userid,x.email);
           redirect(buf);
		}
        if(strlen(pass1)<4)
                http_fatal("密码太短(至少4字符)");
        if(strcmp(pass1, pass2))
                http_fatal("两次输入的密码不一致, 请确认密码");
        if(strlen(x.username)<2)
                http_fatal("请输入昵称(昵称长度至少2个字符)");
        if(strlen(x.realname)<4)
                http_fatal("请输入真实姓名(请用中文, 至少2个字)");
        if(strlen(dept)<6)
                http_fatal("工作单位的名称长度至少要6个字符(或3个汉字)");
        if(strlen(x.address)<6)
                http_fatal("通讯地址长度至少要6个字符(或3个汉字)");
        if(strlen(phone)<2)
                http_fatal("请填入电话号码");
        //if(strlen(assoc)<2) http_fatal("请填入校友会");
        if(badstr(x.passwd)||badstr(x.username)||badstr(x.realname))
                http_fatal("您的注册单中含有非法字符");
        if(badstr(dept)||badstr(x.address)||badstr(x.email)||badstr(phone))
                http_fatal("您的注册单中含有非法字符");
        if(badymd(x.birthyear, x.birthmonth, x.birthday))
                http_fatal("请输入您的出生年月");
        //if(is_bad_id(x.userid)) http_fatal("不雅帐号或禁止注册的id, 请重新选择");
        //if(getuser(x.userid)) http_fatal("此帐号已经有人使用,请重新选择。");
	//if (check_nkbbs_user(x.userid, inv, nkbbspw) == 2)
	//{
		//http_fatal("对不起，bbs.nankai.edu.cn上的同名ID不属于您，注册失败");
	//}
	//if (!check_uuid(inv, x.userid))
	//{
		//http_fatal("邀请码错误！");
	//}
        sprintf(salt, "%c%c", 65+rand()%26, 65+rand()%26);
        strsncpy(x.passwd, crypt1(pass1, salt), 14);
        strcpy(x.termtype, "vt100");
        strncpy(x.lasthost, fromhost, sizeof(x.lasthost));
        x.userlevel=PERM_BASIC;
        x.firstlogin=time(0);
        x.lastlogin=time(0);
        x.userdefine=-1;
        x.flags[0]=CURSOR_FLAG | PAGER_FLAG;
	x.userdefine &= ~DEF_USEGB;
        
		if(xz==2)
                currentuser.userdefine ^= DEF_S_HOROSCOPE;

		f_append("trace.post", "G");
		rmpc(x.userid);
		sprintf(filename, "mail/%c/%s", toupper(x.userid[0]), x.userid);
		if (file_exist(filename)) 
		{
			sprintf (buf, "rm -rf %s", filename);
			system(buf);
		}
		mkdir(filename,0755);
		sprintf(filename, "home/%c/%s", toupper(x.userid[0]), x.userid);
		if (file_exist(filename)) 
		{
			sprintf (buf, "rm -rf %s", filename);
			system(buf);
		}
		mkdir(filename, 0755);
		sprintf(filename, "/home/www/upload/qmd/%c/%s.*", toupper(x.userid[0]), x.userid);
		sprintf (buf, "rm -rf %s", filename);
		system(buf);

		
		//add by brew...
		regtime=time(NULL);
		for(i=0;i<8;i++)
		{
		   regcodebuf[i]=public_word[rand()%strlen(public_word)];
		}
		regcodebuf[i]=0;
		if(!isAllowedIp(fromhost))
		{
			//sprintf(filename, "home/%c/%s", toupper(x.userid[0]), x.userid);
		sprintf(mail_title,"%s[%s]邮件认证:%s.%s",BBSNAME,BBSID,x.userid,regcodebuf);
	 sprintf(filename, "home/%c/%s/.webauth.U", toupper(x.userid[0]), x.userid);
			f_append(filename,mail_title);
			f_append(filename,"\n");
			f_append(filename,".webauth.");
			f_append(filename,regcodebuf);
			f_append(filename,".");
			f_append(filename,x.email);
			f_append(filename,"\n"); 
	 sprintf(filename, "home/%c/%s/.webauth.%s.%s", toupper(x.userid[0]), x.userid,regcodebuf,x.email);
			f_append("webregtrace", filename);
			f_append("webregtrace","\n");
			if(!file_exist(filename))
			{	
				fp=fopen(filename, "a");
				if(fp) 
				{
					fprintf(fp, "欢迎您注册%s[%s]帐号",BBSNAME,BBSID);
					fprintf(fp, "您注册的id是: %s\n",    	x.userid);
					fprintf(fp, "您的验证码是: %s\n",regcodebuf);
					fprintf(fp, "其他信息：realname: %s\n",  	x.realname);
					fprintf(fp, "dept: %s\n",    	dept);
					fprintf(fp, "addr: %s\n",      	getparm("address"));
					//当年谁没事儿非要加南开大学的！！！哼哼。
					fprintf(fp, "phone: %s\n",     	phone);
					fprintf(fp, "assoc: %s\n",		assoc);
					fprintf(fp, "直接回复本信件即可取得地址认证。" );
					fprintf(fp, "您也可以使用下面的链接取得认证(将网址复制到地址栏:\n");
					fprintf(fp, "http://%s/auth/activepost.php?id=%s&code=%s\n", BBSHOST, x.userid, regcodebuf);
					fprintf(fp, "或者访问下述网页,填写您的id和验证码:\n");
					fprintf(fp, "http://%s/auth/activepost.php\n", BBSHOST);
					fprintf(fp, "----\n" );
					fclose(fp);
				}
			}
			else
			{
				
				http_fatal("认证出错。错误代码：101");
			}		
		}
	//	else
		{//2001.09.04 Haishion add for no first time
	        	adduser(&x);
			//2001.09.04 Haishion add for no first time
			getdatestring(time(0), NA);
			fp=fopen("new_register", "a");
			if(fp) {
					fprintf(fp, "usernum: %d, %s\n", 	getusernum(x.userid)+1, datestring);
					fprintf(fp, "userid: %s\n",    	x.userid);
					fprintf(fp, "realname: %s\n",  	x.realname);
					fprintf(fp, "dept: %s\n",    	dept);
					fprintf(fp, "addr: %s\n",      	x.address);
					fprintf(fp, "phone: %s\n",     	phone);
					fprintf(fp, "regip: %s\n",      fromhost);
					//huangxu@061201:13jj好厉害
					fprintf(fp, "assoc: %s\n",		assoc);
					fprintf(fp, "----\n" );
					fclose(fp);
			}
		}
        printf("<center><table width=70%><td><td>\n");
        printf("亲爱的<font color=green >%s</font>:<br>",x.userid);
        printf("您好!<br>欢迎光临BBS%s, 您的注册信息已经被成功提交了.", BBSNAME);
        if(isAllowedIp(fromhost))
		{
			printf("<p>我们会确认您的身份,一般24小时以内就会有答复.敬请留意您的站内信箱.</font>如果您的资料真实,您将会很容易通过认证.如果万一这次您没有通过,系统会提醒您重新填写真实注册信息.<font color=red>你只有通过认证, 才能发表文章,发送私人信件和讯息.</font></p>");
			printf("您目前拥有本站基本的权限, 包括阅读文章、环顾四方、接收私人信件、接收他人的消息、进入聊天室等等.<br>");
		}
		else
		{
			printf("<p>我们会确认您的身份,并给您的南开邮箱发送了确认信。</font> 敬请留意您的邮箱。<font color=red>你需要简单的回复认证信, 才能登陆本站。</font></p>");
			 printf("<p>我们将保留三天您的id。逾期未受到您的回信登陆本站的信息，我们将收回此ID。</p>");

	//		printf("您目前拥有本站基本的权限, 包括阅读文章、环顾四方、接收私人信件、接收他人的消息、进入聊天室等等.<br>");

		}
        printf("本站地址是 %s , 支持<a href=http://%s/>Web</a>, <a  href=telnet://%s/>Telnet</a>等多种登录方式.欢迎您常来!<br>",BBSHOST,BBSHOST);
        printf("<br>如果您有任何疑问，可以去sysop(站长的工作室)板发文求助.</table>");
        printf("<hr color=green><br>您的基本资料如下:<br>\n");
        printf("<table border=1 width=400>");
        printf("<tr><td>帐号位置: <td>%d\n", getusernum(x.userid));
        printf("<tr><td>使用者代号: <td>%s (%s)\n", x.userid, x.username);
        printf("<tr><td>姓  名: <td>%s<br>\n", x.realname);
        printf("<tr><td>昵  称: <td>%s<br>\n", x.username);
        printf("<tr><td>上站位置: <td>%s<br>\n", x.lasthost);
        printf("<tr><td>电子邮件: <td>%s<br></table><br>\n", x.email);
		printf("<p style=\"text-align:center; color: red; font-weight:bold;\">如果在bbs.nankai.edu.cn上存在名为“%s”的ID，则您需要登录至该站发信给iNankai说明情况后才会通过验证，敬请谅解。</p>", x.userid);
        printf("<center><a href=\"%s\" target=_top>返回登录页面</a>\n", getparm("reg_referer")?getparm("reg_referer"):"/");
		if(isAllowedIp(fromhost))
		{
			newcomer(&x, words);
			getdatestring(time(0),NA);
			sprintf(buf, "%s %-12s %d\n", datestring, x.userid, getusernum(x.userid));
			f_append("wwwreg.log", buf);
			sprintf(buf, "%s APPLY uid [www] from %s\n", datestring, fromhost);
			f_append("usies", buf);
		}
		else
		{
//post_imail(char *userid, char *title, char *file, \
			char *id, char *nickname, char *ip, int sig) 
//			post_imail(x.email,mail_title,
//				filename,"AuthMaster","我爱南开站","202.113.16.117",1);
			sprintf(filename,"mail/%c/email.auth.U",toupper(x.email[0]));
			sprintf(buf,"%s,%s,%s,%d\n",x.email,x.userid,fromhost,regtime);
			fs_append(filename,buf);
			sprintf(filename, "home/%c/%s/.webauth.%s.%s", toupper(x.userid[0]), x.userid,regcodebuf,x.email);
			post_imail(x.email,mail_title,
                                filename,"AuthMaster",BBSNAME,BBSHOST,1);

		}

}

int badstr(unsigned char *s)
{
        int i;
        for(i=0; s[i]; i++)
                if(s[i]!=9 &&(s[i]<32 || s[i]==255))
                        return 1;
        return 0;
}

int newcomer(struct userec *x, char *words)
{
        FILE *fp;
        char filename[80];
        sprintf(filename, "tmp/%d.tmp", getpid());
        fp=fopen(filename, "w");
        fprintf(fp, "大家好, \n\n");
        fprintf(fp, "我是 %s(%s), 来自 %s\n", x->userid, x->username, fromhost);
        fprintf(fp, "%s初来此地报到, 请大家多多指教.\n\n",x->gender=='M'?"小弟":"小女子");
        fprintf(fp, "自我介绍:\n\n");
        fprintf(fp, "%s", words);
        fclose(fp);
        char buf[STRLEN];
        sprintf(buf,"Web: %s@%s",x->userid,x->username);
        post_article("newcomers", buf, filename, x->userid, x->username, fromhost, -1);
        unlink(filename);
}

int adduser(struct userec *x)
{
        //	extern struct UTMPFILE *utmpshm;
        int i;
        FILE *fp;
        fp=fopen(".PASSWDS", "r+");
        flock(fileno(fp), LOCK_EX);
        for(i=0; i<MAXUSERS; i++)
        {
                if(shm_ucache->userid[i][0]==0) {
                        strncpy(shm_ucache->userid[i], x->userid, 13);
                        save_user_data(x);
                        break;
                }
        }
        //	utmpshm->usersum=i+1;
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        system("touch .PASSFLUSH");
        countusers(NULL);

}

int rmpc(char * id)
{
	char file[256],buf[256];
	sprintf(file,"0Announce/PersonalCorpus/%c/%s/.Names",toupper(id[0]),id);
	if(!file_exist(file)) {
	   return 1;
	}
	sprintf(buf,"rm -rf 0Announce/PersonalCorpus/%c/%s",toupper(id[0]),id);
	// printf("%s",buf);return 0;
	system(buf);
	sprintf(file,"0Announce/PersonalCorpus/%c/.Names",toupper(id[0]));
	char tmp[80];
	sprintf(tmp,"0Announce/PersonalCorpus/%c/.Names.tmp",toupper(id[0]));
	FILE *fp=fopen(file,"r+");
	FILE *ftmp=fopen(tmp,"w+");
	if(fp==0||ftmp==0){
		return -1;
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

int check_nkbbs_user(char* userid, char* uuid, char* pass)
{
	FILE* fp;
	int result = 1;
	char buf[STRLEN], filepath[STRLEN];
	if(!userid || !uuid)
		return 2;
	if (!*pass)
	{
		sprintf(buf,"%s/bin/checkuser2.sh '%s' '%s'",BBSHOME, userid,uuid);
	}
	else
	{
		sprintf(buf, "%s/bin/checkuser.sh '%s' '%s'", BBSHOME, userid, pass); 
	}
	sprintf(filepath, "%s/tmp/result_%s", BBSHOME, userid);
	system(buf);
	fp = fopen(filepath,"r");
	if(!fp)
		return 2;
	fscanf(fp,"%d",&result);
	fclose(fp);
	unlink(filepath);
	return result;
}


