/*$Id: bbslogout.c,v 1.5 2011-01-25 08:26:54 maybeonly Exp $
*/

#include "BBSLIB.inc"
int main()
{
        int stay, pid;
        init_all();
        //	if(!loginok) redirect("/default.html");
        if(!loginok) {
          redirect("/default.html");  
          return 0;  
        }                                 
      /* 
        pid=u_info->pid;
        if(pid>0)
                kill(pid, SIGHUP);
*/
        char num[16];
        strsncpy(num, getparm("utmpnum"), 12);
        int i=atoi(num);
        
 FILE * fp=fopen(".UTMP.lock", "a");
 flock(fileno(fp), LOCK_EX);

 struct user_info *u=&shm_utmp->uinfo[i];
 u->active=0;
 /*
  To make user online time is accounted correctly even if he just close the browser before click the logout link. 
  The main solution is that:
  1. Add a new field lastrecord into the user_info structure, this field is used to store the last time the user experience is calculated
  2. Record the user online data whenever there is an unload event at the user page
 */
 char buf[80];
 int logintime;
 snprintf(buf, sizeof(buf), "%s/etc/double_stay", BBSHOME);
 if(dashf(buf))
	logintime =  (time(0) - u->lastrecord) * 2;
 else
	logintime = time(0) - u->lastrecord;
 currentuser.stay+=logintime ;
 u->lastrecord = time(0);
 save_user_data(&currentuser);
 memset(u, 0, sizeof(struct user_info));
 //huangxu@071203:在这里清除结构试试看？
 flock(fileno(fp), LOCK_UN);
 fclose(fp);


 report("WEBExit");
        setcookie("utmpkey", "");
        setcookie("utmpnum", "");
        setcookie("utmpuserid", "");
        setcookie("my_t_lines", "");
        setcookie("my_link_mode", "");
        setcookie("my_def_mode", "");
        setcookie("my_localpic_mode", "");
        //	redirect(FIRST_PAGE);
        redirect("/");
}
