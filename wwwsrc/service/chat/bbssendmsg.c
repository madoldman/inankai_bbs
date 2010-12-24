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
	        http_fatal("����û�е�¼!");
        if(!(currentuser.userlevel & PERM_PAGE))
            http_fatal ("�������з���ѶϢ��Ȩ��.");
       
        printf("\
               <STYLE type=text/css>\
               .input{��\
               border-top :  none ;\
               border-bottom : 1px solid #000000;��\
               border-left : 0  ;��\
               border-right : 0  ;��\
               background-color : #eff3ff ;��\
               color : 000000 ;��\
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
    printf("<a href=bbsfoot>����</a>");
	exit(0);
}

int sendmsg(char *toid,int topid,char *msg)
{
	    modify_mode(u_info,MMENU+20000);
            struct userec *x=getuser(toid);
            if(x==NULL){
           	 printf("�û�%s������",toid);
                 bbsfoot();
            }
            strcpy(toid,x->userid);
	    int i,mode;
	    if(!strcasecmp(toid, currentuser.userid)){
                printf("����Ц,��Ҫ�Լ����Լ�����?");
                bbsfoot();
	    }
/*
            char path[256];
            sethomefile(path, toid, "rejects");
            if(file_has_word(path,currentuser.userid)){
                printf("���ʹ���.");
                bbsfoot();
            }
  */                 

	   char path[256];
	   sethomefile(path, toid, "rejects");
           if(file_has_word(path,currentuser.userid)){
              printf("���ڶԷ��ĺ���������,��Ϣ�޷��ͳ�.");
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
                    printf("�Է�ѶϢ���ѹر�.");
                    bbsfoot();
                }else
                if(!hisfriend(&shm_utmp->uinfo[i])) {
                    printf("�Է�ѶϢ��ֻ�Ժ��ѿ���.");
                    bbsfoot();
                }    
           
            if(shm_utmp->uinfo[i].invisible && !(currentuser.userlevel & PERM_SEECLOAK))
                break;
            mode=shm_utmp->uinfo[i].mode;
            if(mode==BBSNET || mode==PAGE || mode==LOCKSCREEN) continue;
            if(send_msg(currentuser.userid, u_info->pid, toid, topid, msg)!=0){
                printf("������Ϣʧ��");
                bbsfoot();
	    }else{
	    	printf("�ɹ��ͳ���");
		redirect("bbsfoot");
		http_quit();
	   }
	}

        printf("%s������,����Ŀǰ���ܽ���ѶϢ.",toid);
        bbsfoot();        
}

int writemsg(char *toid,int topid)
{
        modify_mode(u_info,MSG+20000);
	printf("<form name=form0 action=bbssendmsg method=post>");
	if(topid!=0) 
		printf("<input type=hidden name=topid value=%d>",topid);
        printf("��<input class=input name=toid size=12  maxlength=12 ");
	if(strcmp(toid,""))
		printf("readonly value=%s",toid);
       	
	printf(" >˵<input name=msg class=input  maxlength=50  size=50>\
	        <input type=submit value=����>");
        printf("<a href=bbsfoot>  ����</a></form>");
        if(strcmp(toid,""))
	    printf("<script>document.form0.msg.focus();</script>");
        else
	    printf("<script>document.form0.toid.focus();</script>");
}
