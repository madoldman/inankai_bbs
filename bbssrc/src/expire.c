/* 
 * expire.c		-- expirepost replacement tool, an article out-of-date
 *				   check tool.
 *
 * porting from Maple 2.36 by Nighthawk.bbs@www.cn.nctu.edu.tw
 *
 * A part of Firebird BBS 3.0
 *
 * Copyright (c) 1998, 1999, Firebird BBS 3.0 Project, All rights reserved.
 *
 * CVS: $Id: expire.c,v 1.5 2010-07-05 04:58:44 madoldman Exp $
 */ 
/* -------------------------------------------------------*/ 
/*  util/expire.c        ( NTHU CS MapleBBS Ver 2.36 )    */ 
/* -------------------------------------------------------*/ 
/*  target : 自动砍信工具程式                             */ 
/*  create : 95/03/29                                     */ 
/*  update : 95/12/15                                     */ 
/* -------------------------------------------------------*/ 
/*  syntax : expire [day] [max] [min]                     */ 
/* -------------------------------------------------------*/ 
/*  移植到 FireBird      by Nighthawk                     */ 
/* -------------------------------------------------------*/ 

#include <stdlib.h>
#include <strings.h>
#include "bbs.h"

#define DEF_DAYS        300
#define DEF_MAXP        30000
#define DEF_MINP        20000

#define EXPIRE_CONF     BBSHOME"/etc/expire.ctl"
#define BBSBOARDS BBSHOME"/.BOARDS"

char   *bpath = BBSHOME "/boards";
report()
{
}

struct life {
	char    bname[16];	/*  board ID */ 
	int     days;		/*  expired days */ 
	int     maxp;		/*  max post */ 
	int     minp;		/*  min post */ 
};
typedef struct life life;


void expire(life *brd, int mode)
{
	struct fileheader head;
	struct stat state;
	struct stat state2;
	char    lockfile[128], tmpfile[128], bakfile[128], topfile[128];
	char    fpath[128], index[128], *fname;
	int     total;
	int     fd, fdr, fdw, done, keep, fdt;
	int     duetime, ftime;
	struct fileheader top_files[MAX_BRD_ONTOP];

	printf("%s--mode:%d\n", brd->bname,mode);

#ifdef  VERBOSE
	if (brd->days < 1) {
		printf(":Err: expire time must more than 1 day.\n");
		return;
	} else if (brd->maxp < 100) {
		printf(":Err: maxmum posts number must more than 100.\n");
		return;
	}
#endif

	if(mode == 1) {
		int size = sizeof (struct fileheader);
		int nfh;
		struct stat st;
		sprintf(index, "%s/%s/.junk", bpath, brd->bname); 
		sprintf(topfile,"%s/%s/.top", bpath, brd->bname);
		memset(top_files, 0, sizeof(top_files));
		if(stat(topfile, &st) == 0 && S_ISREG(st.st_mode))
			fdt = open(topfile, O_RDONLY);
		if(fdt > 0){
			int i = 0;
			while ((nfh = read (fdt, top_files + i, size)) == size)
				i ++;
			close(fdt);
		}
	}
    else 
         sprintf(index, "%s/%s/.DIR", bpath, brd->bname);
	sprintf(lockfile, "%s.lock", index);
	if ((fd = open(lockfile, O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
		return;
	flock(fd, LOCK_EX);

	strcpy(fpath, index);
	fname = (char *) strrchr(fpath, '.');

	duetime = time(NULL) - brd->days * 24 * 60 * 60;
	done = 0;
	if ((fdr = open(index, O_RDONLY, 0)) > 0) {
		fstat(fdr, &state);
		total = state.st_size / sizeof(head);
		sprintf(tmpfile, "%s.new", index);
		unlink(tmpfile);
		fdt = -1;
		if ((fdw = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) > 0) {
			while (read(fdr, &head, sizeof head) == sizeof head) {
				done = 1;
				
				char filebuf[STRLEN];
				sprintf (filebuf, "%s/%s/%s", bpath, brd->bname,head.filename);
				stat(filebuf,&state2);
				ftime=state2.st_mtime;/*loveni:删文时间按文件修改日期算*/



				//ftime = atoi(head.filename + 2);
				if (head.accessed[0] & (FILE_MARKED|FILE_DIGEST|FILE_TIP) ||
					head.accessed[1] & FILE_ONTOP || total <= brd->minp)
				      keep = 1;
				else if (head.owner[0] == '-')
					keep = 0;
				else if (ftime < duetime || total > brd->maxp)
					keep = 0;
				else
					keep = 1;

				/*清除回收站模式*/
				if(mode==1){
					if (ftime < duetime || total > brd->maxp){
						int i = 0;
						keep = 0;
						for(i = 0; i < MAX_BRD_ONTOP && top_files[i].filename[0]; i ++){
							if(!strcmp(top_files[i].filename, head.filename)){
								keep = 1;
								break;
							}
						}
					}
					else 
						keep = 1;
				}

				if (keep) {
					if (safewrite(fdw, (char *)&head, sizeof head) == -1) {
						done = 0;
						break;
					}
				} else {
					strcpy(fname, head.filename);
					unlink(fpath);
				       // printf("%s\n", fname);
					total--;
				}
			}
			close(fdw);
		}
		close(fdr);
	}
	if (done) {
		sprintf(bakfile, "%s.old", index);
		if (rename(index, bakfile) != -1)
			rename(tmpfile, index);
	}
	flock(fd, LOCK_UN);
	close(fd);
}



int checkperboard(struct boardheader *bp)
{
        //static i=0;
        life brd;
        strcpy(brd.bname, bp->filename);
        brd.days=7;
        brd.maxp=10000;
        brd.minp=1;
        expire(&brd,1);
        //printf("%d %s\n",++i,bp->filename);
}

int checkdeleted()
{
    apply_record(BBSBOARDS, checkperboard, sizeof(struct boardheader));
}


int main(int argc, char* argv[])
{
        checkdeleted();

	FILE   *fin;
	int     number;
	size_t  count;
	life    db, table[MAXBOARD], *key;
	//struct dirent *de;
	//DIR    *dirp;
	char   *ptr, *bname, buf[256];
	db.days = ((argc > 1) && (number = atoi(argv[1])) > 0) ? number : DEF_DAYS;
	db.maxp = ((argc > 2) && (number = atoi(argv[2])) > 0) ? number : DEF_MAXP;
	db.minp = ((argc > 3) && (number = atoi(argv[3])) > 0) ? number : DEF_MINP;

	/*  --------------- */ 
	/*  load expire.ctl */ 
	/*  --------------- */ 

	count = 0;
	if (fin = fopen(EXPIRE_CONF, "r")) {
		while (fgets(buf, 256, fin)) {
			if (buf[0] == '#')
				continue;

			bname = (char *) strtok(buf, " \t\r\n");
			if (bname && *bname) {
				ptr = (char *) strtok(NULL, " \t\r\n");
				if (ptr && (number = atoi(ptr)) > 0) {
					key = &(table[count++]);
					strcpy(key->bname, bname);
					key->days = number;
					key->maxp = db.maxp;
					key->minp = db.minp;

					ptr = (char *) strtok(NULL, " \t\r\n");
					if (ptr && (number = atoi(ptr)) > 0) {
						key->maxp = number;

						ptr = (char *) strtok(NULL, " \t\r\n");
						if (ptr && (number = atoi(ptr)) > 0) {
							key->minp = number;
						}
					}
				}
			}
                     if(key)
		     {
                     	printf("%s %d %d %d \n",key->bname,key->days,key->maxp,key->minp);
                     	expire(key,0);            
		     }
		}
		fclose(fin);
	}


    //    checkdeleted();
        return 0;

         
        /* 
	if (count > 1) {
		qsort(table, count, sizeof(life), (const void *) strcasecmp);
	}
	
	if (!(dirp = opendir(bpath))) {
		printf(":Err: unable to open %s\n", bpath);
		return -1;
	}
	while (de = readdir(dirp)) {
		ptr = de->d_name;
		if (ptr[0] > ' ' && ptr[0] != '.') {
			if (count)
				key = (life *) bsearch(ptr, table, count, sizeof(life), (const void *) strcasecmp);
			else
				key = NULL;
			if (!key)
				key = &db;
			strcpy(key->bname, ptr);
			expire(key);
		}
	}
	closedir(dirp);*/ 
}

