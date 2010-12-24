//logout.wml
//huangxu@nkbbs.org

//注销

#include "BBSLIB.inc"
#include "WMLLIB.inc"

int main()
{
	int stay, pid;
	page_init(NULL);
	if(loginok) 
	{
		char num[16];
		strsncpy(num, getparm("utmpnum"), 12);
		int i=atoi(num);
		FILE * fp=fopen(".UTMP.lock", "a");
		flock(fileno(fp), LOCK_EX);
		struct user_info *u=&shm_utmp->uinfo[i];
		u->active=0;
		int logintime=time(0) - u->lastrecord;
		currentuser.stay+=logintime;
		u->lastrecord = time(0);
		save_user_data(&currentuser);
		memset(u, 0, sizeof(struct user_info));
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
	}
	headerCookie("utmpnum", "");
	headerCookie("utmpkey", "");
	headerCookie("utmpuserid", "");
	wml_httpheader();
	report("WapExit");
	wml_head();
	printf ("<card title=\"欢迎再来 -- %s\" ontimer=\"index.wml\">", BBSNAME);
	printf ("<timer value=\"1\" />");
	printf ("<p>您已经注销，跳转中……</p>");
	printf ("<p>欢迎再次访问%s。</p>", BBSNAME);
	printf ("</card>");
	wml_tail();
}
