#include "BBSLIB.inc"
#define MAX_FILE_LEN  100
#define MAX_PATH_LEN  150
#define MAX_LINE_LEN  256
#define FILE_TIME_LINE 14
#define LISTDIR		"/home/bbs/ftp"
#define LISTFILE	LISTDIR"/FTPFileList"
#define LOGDIR		LISTDIR"/logs"
#define ADMINLOG		LOGDIR"/admin.log"
#define SEARCHLOG	LOGDIR"/search.log"
struct FileInfo
{
        char FileName[MAX_FILE_LEN];
        char PathName[MAX_PATH_LEN];
        int  FileSize;
        char FileTime[FILE_TIME_LINE];
        char IsDir;
};

/* 该IP是否在FTP搜索范围内 */
int SiteInList(char *ip)
{
	FILE *fp;
	char buf[80];
	if((fp=fopen(LISTFILE, "r"))==NULL) return 0;
	while(fgets(buf, 80, fp))
	{
		strtok(buf, ":\r\n\t");
		if(!strcmp(buf, ip))
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

int ftplog(char *file, char *s, char *ip)
{
	char buf[512];
	getdatestring(time(0),NA);
	sprintf(buf, "%-14s %22.22s %-16s %s\n", currentuser.userid, datestring, ip, s);
	f_append(file, buf);
	return 1;



}
