//logout.wml
//huangxu@nkbbs.org

//ע��

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
	printf ("<card title=\"��ӭ���� -- %s\" ontimer=\"index.wml\">", BBSNAME);
	printf ("<timer value=\"1\" />");
	printf ("<p>���Ѿ�ע������ת�С���</p>");
	printf ("<p>��ӭ�ٴη���%s��</p>", BBSNAME);
	printf ("</card>");
	wml_tail();
}
