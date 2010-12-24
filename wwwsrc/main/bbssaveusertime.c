/*
 * This file is added by Bo Yang and used to record the user online time data as soon as possible. 
 * The idea behind the method is that every time user leave bbs page, there must be a unload event
 * trigered in browser, we just let the unload event handler to send a request to this file and in 
 * this file, we record the data.
 */

#include "BBSLIB.inc"

int main()
{
	int stay, pid;
	init_all ();
	if (!loginok){
		return 0;
	}

	char num[16];
	strsncpy (num, getparm ("utmpnum"), 12);
	int i = atoi (num);

	FILE * fp = fopen (".UTMP.lock", "a");
	flock (fileno(fp), LOCK_EX);

	struct user_info *ui = &shm_utmp->uinfo[i];
	int now = time (0);
	currentuser.stay += now - ui->lastrecord;
	save_user_data (&currentuser);
	ui->lastrecord = now;
	
	flock (fileno(fp), LOCK_UN);
	fclose (fp);
}
