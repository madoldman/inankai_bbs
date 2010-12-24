/*生日发贺信 by loveni*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bbs.h>
#define file_time(x) f_stat(x)->st_mtime
#define file_exist(x) (file_time(x)!=0)

struct stat *f_stat(char *file) 
{
        static struct stat buf;
        bzero(&buf, sizeof(buf));
        if(stat(file, &buf)==-1) bzero(&buf, sizeof(buf));
        return &buf;
}



int safewrite(int fd, char *buf, int size)
{
	int     cc, sz = size, origsz = size;
	char   *bp = buf;

	do
	{
		cc = write(fd, bp, sz);
		if ((cc < 0) && (errno != EINTR))
		{
			printf("safewrite err!");
			return -1;
		}
		if (cc > 0)
		{
			bp += cc;
			sz -= cc;
		}
	}
	while (sz > 0);
	return origsz;
}

int append_record(char *filename, char *record,int size)
{
	int     fd;
	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1)
	{
		printf("open file error in append_record()");
		return -1;
	}
	flock(fd, LOCK_EX);
	lseek(fd, 0, SEEK_END);
	if (safewrite(fd, record, size) == -1)
		printf("apprec write err!");
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

int mail_file(char *tmpfile,char * userid,char * title)
{
	struct fileheader newmessage;
	struct stat st;
	char    fname[STRLEN], filepath[STRLEN], *ip;
	char genbuf[STRLEN],save_title[STRLEN],save_filename[STRLEN];
	int     fp, count;
	memset(&newmessage, 0, sizeof(newmessage));
	sprintf(genbuf, "%s (%s)",BBSID,BBSNAME);
	strncpy(newmessage.owner, genbuf, STRLEN);
	strncpy(newmessage.title, title, STRLEN);
	strncpy(save_title, newmessage.title, STRLEN);
	sprintf(filepath, "/home/bbs/mail/%c/%s", toupper(userid[0]), userid);
	if (stat(filepath, &st) == -1)
	{
		if (mkdir(filepath, 0755) == -1)
			return -1;
	}
	else
	{
		if (!(st.st_mode & S_IFDIR))
			return -1;
	}
	sprintf(fname, "M.%d.A", time(NULL));
	sprintf(filepath, "/home/bbs/mail/%c/%s/%s", toupper(userid[0]), userid, fname);
	ip = strrchr(fname, 'A');
	count = 0;
	while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1)
	{
		if (*ip == 'Z')
			ip++, *ip = 'A', *(ip + 1) = '\0';
		else
			(*ip)++;
		sprintf(filepath, "mail/%c/%s/%s", toupper(userid[0]), userid, fname);
		if (count++ > MAX_POSTRETRY)
		{
			return -1;
		}
	}
	close(fp);
	strcpy(newmessage.filename, fname);
	strncpy(save_filename, fname, STRLEN);
	sprintf(filepath, "/home/bbs/mail/%c/%s/%s", toupper(userid[0]), userid, fname);
	sprintf(genbuf,"/bin/cp %s %s",tmpfile,filepath);
	system(genbuf);
	sprintf(genbuf, "/home/bbs/mail/%c/%s/%s", toupper(userid[0]), userid, ".DIR");
	if (append_record(genbuf, &newmessage, sizeof(newmessage)) == -1)
		return -1;

}
int main(argc, argv)
char   *argv[];
{
	FILE   *fp;
	time_t  now;
	int     i, j = 0;
	struct userec aman;
	struct tm *tmnow;
	char    buf[256];
	if(!file_exist("/home/bbs/etc/birthday"))
	{
		printf("Error Open birthday file\n");
		return 0;
	}
	sprintf(buf, "%s/.PASSWDS", BBSHOME);
	if ((fp = fopen(buf, "rb")) == NULL) {
		printf("Can't open record data file.\n");
		return 1;
	}
	
	now = time(0);
	tmnow = localtime(&now);
	for (i = 0;; i++) 
	{
		if (fread(&aman, sizeof(struct userec), 1, fp) <= 0)
			break;
			
		/* 以下的  *名人* 不需要算在内 */
		if (!strcasecmp(aman.userid, "SYSOP") || !strcasecmp(aman.userid, "guest") )
			continue;
			
		if (aman.birthmonth == 0)
			continue;
		if (aman.birthmonth == tmnow->tm_mon + 1 &&	aman.birthday == tmnow->tm_mday) {
			printf( "%s\n", aman.userid);
			mail_file("/home/bbs/etc/birthday",aman.userid,"南开人社区恭祝您生日快乐");
			j++;
		}
	}
	

	fclose(fp);
}
