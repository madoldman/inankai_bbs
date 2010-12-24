/*$Id: bbsfoot.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#define NOIDLE
#include "BBSLIB.inc"
int main()
{
        int dt=0;
        int mail_total, mail_unread;
        char *id="guest";
        init_all();
        if(loginok)
			id=currentuser.userid;
   
        printf("<script src=/js/common.js></script>");
	printf("<meta http-equiv=pragma content=no-cache>"); 
        printf("<style type=text/css>\nA {color: #0000FF}\ninput {font-size: 9pt; border: #666666; border-top-width: 1px; border-right-width: 1px; border-bottom-width: 1px; border-left-width: 1px}</style>\n");
        printf("<body id=\"bbsfoot\">\n");

        if(newmsg())
			http_quit();
        refreshto("bbsfoot", 180);
        printf("<a href=\"javascript:chgstat()\">导航条 </a>&nbsp");

        printf("用户[<a href=bbsqry?userid=%s target=f3>%s</a>]", id, id);
        getdatestring(time(0), NA);
        printf("[%s]", datestring);
        if(HAS_PERM(PERM_SEECLOAK))
                printf("在线[<a href=bbsusr target=f3>%d</a>]", count_online());
        else
                printf("在线[<a href=bbsusr target=f3>%d</a>]", count_online2());
        
		if(!loginok) http_quit();
		
        count_mails(id, &mail_total,&mail_unread);
        printf("新信[<a href=bbsmail target=f3><font color=red>%d</font></a>]", mail_unread);

       
		dt=abs(time(0) - *(int*)(u_info->from+30))/60;
        u_info->idle_time=time(0);
        printf("[%d小时%d分]", dt/60, dt%60);

        printf(" [<a href=bbsfoot>刷新</a>]\n");
      
	if(u_info->mode!=POSTING+20000&&u_info->mode!=SMAIL+20000){	
             modify_mode(u_info,MMENU+20000);
	     if(mail_unread)
  	        printf("<script language=javascript>if(confirm(\"您有%d封新邮件,立即查看吗?\")) top.f3.location=\"bbsnewmail\";</script>",mail_unread);
	}
	printf("<a href=bbsfriend target=f3> 在线好友 </a>");

		if(currentuser.userlevel & PERM_PAGE)
			printf("<a href=bbssendmsg>送信息</a>");

        http_quit();
}


int newmsg(){
        modify_mode(u_info,MSG+20000);
	char buf[256],buf2[256];
	char toid[IDLEN+1];
	int topid;
	if(!loginok)
		return 0;
	sprintf(buf, "home/%c/%s/wwwmsg", toupper(currentuser.userid[0]), currentuser.userid);
	int size=file_size(buf);
	if(size<=0)
		return 0;
	msgsound();
        bzero(buf2,256);
	get_record(buf2,129,0,buf);
	del_record(buf,129,0);
        sscanf(buf2+12, "%s", toid);
        sscanf(buf2+122, "%d", &topid);
        hprintf("%s",buf2);
	printf("<a href=bbssendmsg?toid=%s&topid=%d>  回复 </a><a href=bbsfoot>  忽略</a>",toid,topid);
	return 1;
}

int msgsound(){
	printf("<bgsound src=/msg.wav>\n");

}
