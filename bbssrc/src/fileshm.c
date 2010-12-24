/*
$Id: fileshm.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"

extern void showstuff(char *buf);

struct FILESHM
{
	char    line[FILE_MAXLINE][FILE_BUFSIZE];
	int     fileline;
	int     max;
	time_t  update;
};

struct STATSHM
{
	char    line[FILE_MAXLINE][FILE_BUFSIZE];
	time_t  update;
};

struct FILESHM *welcomeshm;
struct FILESHM *goodbyeshm;
struct FILESHM *issueshm;
struct FILESHM *endlineshm = NULL;//bluetent 2003.1.2
struct STATSHM *statshm;

int fill_shmfile(int mode, char* fname, char* shmkey)
{
	FILE   *fffd;
	char   *ptr;
	char    buf[FILE_BUFSIZE];
	struct stat st;
	time_t  ftime, now;
	int     lines = 0, nowfn = 0, maxnum;
	struct FILESHM *tmp;
	switch (mode)
	{
	case 1:
		maxnum = MAX_ISSUE;
		break;
	case 2:
		maxnum = MAX_GOODBYE;
		break;
	case 3:
		maxnum = MAX_WELCOME;
		break;
	case 4:
	case 5:
		maxnum = MAX_ENDLINE;//bluetent
		break;
	}
	now = time(0);
	if (stat(fname, &st) < 0)
	{
		return 0;
	}
	ftime = st.st_mtime;
	tmp = (void *) attach_shm(shmkey, 5000 + mode * 10, sizeof(struct FILESHM) * maxnum);
	switch (mode)
	{
	case 1:
		issueshm = tmp;
		break;
	case 2:
		goodbyeshm = tmp;
		break;
	case 3:
		welcomeshm = tmp;
		break;
	case 4:
	case 5:
		endlineshm = (void *) attach_shm(shmkey, 5060, sizeof(struct FILESHM) * maxnum);//bluetent
		break;

	}

	if (abs(now - tmp[0].update) < 86400 && ftime < tmp[0].update)
	{
		return 1;
	}
	if ((fffd = fopen(fname, "r")) == NULL)
	{
		return 0;
	}
	while ((fgets(buf, FILE_BUFSIZE, fffd) != NULL) && nowfn < maxnum)
	{
		if (lines > FILE_MAXLINE)
			continue;
		if(mode==5 && buf[0]=='#')
			continue;
		if (strstr(buf, "@logout@") || strstr(buf, "@login@"))
		{
			tmp[nowfn].fileline = lines;
			tmp[nowfn].update = now;
			nowfn++;
			lines = 0;
			continue;
		}
		ptr = tmp[nowfn].line[lines];
		memcpy(ptr, buf, sizeof(buf));
		lines++;
	}
	fclose(fffd);
	tmp[nowfn].fileline = lines;
	tmp[nowfn].update = now;
	nowfn++;
	tmp[0].max = nowfn;
	return 1;
}

int fill_statshmfile(char* fname, int mode)
{
	FILE   *fp;
	time_t  ftime;
	char   *ptr;
	char    buf[FILE_BUFSIZE];
	struct stat st;
	time_t  now;
	int     lines = 0;
	if (stat(fname, &st) < 0)
	{
		return 0;
	}
	ftime = st.st_mtime;
	now = time(0);

	if (mode == 0 || statshm == NULL)
	{
		statshm = (void *) attach_shm("STAT_SHMKEY", 5100, sizeof(struct STATSHM) * 2);
	}
	if (abs(now - statshm[mode].update) < 86400 && ftime < statshm[mode].update)
	{
		return 1;
	}
	if ((fp = fopen(fname, "r")) == NULL)
	{
		return 0;
	}
	memset(&statshm[mode], 0, sizeof(struct STATSHM));
	while ((fgets(buf, FILE_BUFSIZE, fp) != NULL) && lines < FILE_MAXLINE)
	{
		ptr = statshm[mode].line[lines];
		memcpy(ptr, buf, sizeof(buf));
		lines++;
	}
	fclose(fp);
	statshm[mode].update = now;
	return 1;
}

void show_shmfile(struct FILESHM* fh)
{
	int     i;
	char    buf[FILE_BUFSIZE];
	for (i = 0; i < fh->fileline; i++)
	{
		strcpy(buf, fh->line[i]);
		showstuff(buf/*, 0*/);
	}
}

int show_statshm(FILE* fh, int mode)
{
	int     i;
	char    buf[FILE_BUFSIZE];
	if (fill_statshmfile(fh, mode))
	{
		if ((mode == 0 && DEFINE(DEF_GRAPH)) || (mode == 1 && DEFINE(DEF_TOP10)))
		{
			clear();

			for (i = 0; i <= 24; i++)
			{
				if (statshm[mode].line[i] == NULL)
					break;
				strcpy(buf, statshm[mode].line[i]);
				prints(buf);
			}
		}
		if (mode == 1 )
			shmdt(statshm);
		return 1;
	}
	return 0;
}

void show_goodbyeshm()
{
	int     logouts;
	logouts = goodbyeshm[0].max;
	clear();
	show_shmfile(&goodbyeshm[(currentuser.numlogins % ((logouts <= 1) ? 1 : logouts))]);
	shmdt(goodbyeshm);
}

void show_welcomeshm()
{
	int     welcomes;
	welcomes = welcomeshm[0].max;
	clear();
	show_shmfile(&welcomeshm[(currentuser.numlogins % ((welcomes <= 1) ? 1 : welcomes))]);
	if (DEFINE(DEF_TOP10))
		pressanykey();
	shmdt(welcomeshm);
}


void show_issue()
{
	int     issues = issueshm[0].max;
	show_shmfile(&issueshm[(issues <= 1) ? 0 :
	                       ((time(0) / 86400) % (issues))]);
	shmdt(issueshm);
}
//add by bluetent 2003.1.2
int show_endline()
{
	static int i = 0;
	char buf[FILE_BUFSIZE + 10];
	int y, x;
	if (endlineshm == NULL)
		return 0;
	if (endlineshm->fileline <= 0)
		return 0;
	strcpy(buf, endlineshm[i / FILE_MAXLINE].line[i % FILE_MAXLINE]);
	getyx(&y, &x);
	showstuff(buf);
	move(y, x);
	i++;
	if (i / FILE_MAXLINE >= MAX_ENDLINE
	        || i >=
	        FILE_MAXLINE * (endlineshm->max - 1) +
	        endlineshm[i / FILE_MAXLINE].fileline)
		i = 0;
	return 1;
}
