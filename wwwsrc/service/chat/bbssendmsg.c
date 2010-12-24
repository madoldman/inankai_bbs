/*$Id: bbssendmsg.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "NEWLIB.inc"
#include "user.h"

int main()
{
        int i;
        int mode, topid=0;
        char toid[20], msg[1024];
        char msgbuf[1024];
        init_all();
      
		if(!loginok)
	        http_fatal("您还没有登录!");
        if(!(currentuser.userlevel & PERM_PAGE))
            http_fatal ("您不具有发送讯息的权限.");
       
        printf("\
               <STYLE type=text/css>\
               .input{　\
               border-top :  none ;\
               border-bottom : 1px solid #000000;　\
               border-left : 0  ;　\
               border-right : 0  ;　\
               background-color : #eff3ff ;　\
               color : 000000 ;　\
               font-family : arial ;\
               font : 9pt ;\
              }\
			  </style>");

        strsncpy(toid, getparm("toid"), 13);
        if(!strcasecmp(getparm("enc"),"utf8"))
        {
        	strncpy(msg, getparm("msg"), 1023);
        	strcpy(msgbuf,msg);
        	i=255;
        	code_convert ("utf-8", "gb18030", msgbuf, 255, msg, 1024);
      	}
      	else
      	{
        	strsncpy(msg, getparm("msg"), 50);
        }
        topid=atoi(getparm("topid"));
	if(strcmp(msg,""))
		sendmsg(toid,topid,msg);
	else
		writemsg(toid,topid);
}

int bbsfoot()
{
    printf("<a href=bbsfoot>返回</a>");
	exit(0);
}

int sendmsg(char *toid,int topid,char *msg)
{
	    modify_mode(u_info,MMENU+20000);
            struct userec *x=getuser(toid);
            if(x==NULL){
           	 printf("用户%s不存在",toid);
                 bbsfoot();
            }
            strcpy(toid,x->userid);
	    int i,mode;
	    if(!strcasecmp(toid, currentuser.userid)){
                printf("开玩笑,你要自己跟自己聊天?");
                bbsfoot();
	    }
/*
            char path[256];
            sethomefile(path, toid, "rejects");
            if(file_has_word(path,currentuser.userid)){
                printf("发送错误.");
                bbsfoot();
            }
  */                 

	   char path[256];
	   sethomefile(path, toid, "rejects");
           if(file_has_word(path,currentuser.userid)){
              printf("您在对方的黑名单里面,消息无法送出.");
              bbsfoot();
           }

            for(i=0; i<MAXACTIVE; i++){
	    if(!shm_utmp->uinfo[i].active) continue;
            if(strcasecmp(shm_utmp->uinfo[i].userid, toid))	continue;
            if(topid!=0 && shm_utmp->uinfo[i].pid!=topid)
                break;
            topid=shm_utmp->uinfo[i].pid;
            if(!(shm_utmp->uinfo[i].pager & ALLMSG_PAGER))
                if(!(shm_utmp->uinfo[i].pager & FRIENDMSG_PAGER)) {
                    printf("对方讯息器已关闭.");
                    bbsfoot();
                }else
                if(!hisfriend(&shm_utmp->uinfo[i])) {
                    printf("对方讯息器只对好友开放.");
                    bbsfoot();
                }    
           
            if(shm_utmp->uinfo[i].invisible && !(currentuser.userlevel & PERM_SEECLOAK))
                break;
            mode=shm_utmp->uinfo[i].mode;
            if(mode==BBSNET || mode==PAGE || mode==LOCKSCREEN) continue;
            if(send_msg(currentuser.userid, u_info->pid, toid, topid, msg)!=0){
                printf("发送消息失败");
                bbsfoot();
	    }else{
	    	printf("成功送出。");
		redirect("bbsfoot");
		http_quit();
	   }
	}

        printf("%s不在线,或者目前不能接收讯息.",toid);
        bbsfoot();        
}

int writemsg(char *toid,int topid)
{
        modify_mode(u_info,MSG+20000);
	printf("<form name=form0 action=bbssendmsg method=post>");
	if(topid!=0) 
		printf("<input type=hidden name=topid value=%d>",topid);
        printf("对<input class=input name=toid size=12  maxlength=12 ");
	if(strcmp(toid,""))
		printf("readonly value=%s",toid);
       	
	printf(" >说<input name=msg class=input  maxlength=50  size=50>\
	        <input type=submit value=发送>");
        printf("<a href=bbsfoot>  返回</a></form>");
        if(strcmp(toid,""))
	    printf("<script>document.form0.msg.focus();</script>");
        else
	    printf("<script>document.form0.toid.focus();</script>");
}
