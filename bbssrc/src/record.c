/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: record.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"

#define BUFSIZE (8192)

#ifdef SYSV
int flock(int fd, int op)
{
	switch (op)
	{
	case LOCK_EX:
		return lockf(fd, F_LOCK, 0);
	case LOCK_UN:
		return lockf(fd, F_ULOCK, 0);
	default:
		return -1;
	}
}
#endif

int safewrite(int fd, char *buf, int size)
{
	int     cc, sz = size, origsz = size;
	char   *bp = buf;

	do
	{
		cc = write(fd, bp, sz);
		if ((cc < 0) && (errno != EINTR))
		{
			report("safewrite err!");
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


long get_num_records(char* filename, int size)
{
	struct stat st;
	if (stat(filename, &st) == -1)
		return 0;
	return (st.st_size / size);
}

int append_record(char* filename, char* record, int size)
{
	int     fd;
	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1)
	{
		report("open file error in append_record()");
		return -1;
	}
	flock(fd, LOCK_EX);
	lseek(fd, 0, SEEK_END);
	if (safewrite(fd, record, size) == -1)
		report("apprec write err!");
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}
void toobigmesg()
{
	report("record size too big!!\n");
}


int apply_record(char *filename, int (*fptr)(), int size)
{
	char    abuf[BUFSIZE];
	int     fd;

	if (size > BUFSIZE)
	{
		toobigmesg();
		return -1;
	}

	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	while (read(fd, abuf, size) == size)
		if ((*fptr) (abuf) == QUIT)
		{
			close(fd);
			return QUIT;
		}
	close(fd);
	return 0;
}

int search_record(char* filename, char* rptr, int size, int (*fptr)(), char* farg)
{
	int     fd;
	int     id = 1;
	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return 0;
	while (read(fd, rptr, size) == size)
	{
		if ((*fptr) (farg, rptr))
		{
			close(fd);
			return id;
		}
		id++;
	}
	close(fd);
	return 0;
}
int search_zone(char *filename,char *zone,char zoneflag)
{
        FILE *fp;
        struct boardheader myheader;
        if((fp=fopen(filename,"r"))==NULL)
        {
                return 0;         
	}
        while(fread(&myheader,sizeof(struct boardheader),1,fp)>0)
        {
                if(strcmp(myheader.filename,zone)==0 && (myheader.flag & ZONE_FLAG) && myheader.title[0]==zoneflag)
                {
			fclose(fp);
                        return 1;
                }
        }
	fclose(fp);
        return 0;
}
int is_zone(char *filename,char *zonename)
{
        FILE *fp;
        struct boardheader myheader;
        if((fp=fopen(filename,"r"))==NULL)
        {
                return 0;
        }
        while(fread(&myheader,sizeof(struct boardheader),1,fp)>0)
        {
                if(strcmp(myheader.filename,zonename)==0 && (myheader.flag & ZONE_FLAG))
                {
			fclose(fp);
                        return 1;
                }
        }
	fclose(fp);
        return 0;
}
/*
int count_zone(char *filename,char *zonename)
{
        int num=0;
        FILE *fp;
	//char tempbuf[STRLEN];
        struct boardheader myheader;
        if((fp=fopen(filename,"r"))==NULL)
        {
                return 0;
        }
        while(fread(&myheader,sizeof(struct boardheader),1,fp)>0)
        {
                if((myheader.flag & CHILDBOARD_FLAG) && strcmp(myheader.owner,zonename)==0)                
		{
			if(!(myheader.filename[0]))
				continue;
			if (!(myheader.level & PERM_POSTMASK) && !HAS_PERM(myheader.level) && (myheader.level & PERM_NOZAP))
                                continue;
#ifdef BOARD_CONTROL
        		if( !HAS_PERM(PERM_SYSOP))
        		{	
                		setbfile(tempbuf, myheader.filename, "board.allow");
                		if(dashf(tempbuf)&&!seek_in_file(tempbuf,currentuser.userid))
				continue;
        		}
#endif
                        num++;
                }
        }
        return num;
}
*/
int get_record(char *filename,char *rptr,int size,int id)
{
	int     fd;
	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
	{
		close(fd);
		return -1;
	}
	if (read(fd, rptr, size) != size)
	{
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int get_records(char* filename, char* rptr, int size, int id, int number)
{
	int     fd;
	int     n;
	if ((fd = open(filename, O_RDONLY, 0)) == -1)
		return -1;
	if (lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
	{
		close(fd);
		return 0;
	}
	if ((n = read(fd, rptr, size * number)) == -1)
	{
		close(fd);
		return -1;
	}
	close(fd);
	return (n / size);
}

int substitute_record(char *filename, char *rptr, int size, int id)
{
	int     fd;
	int	numr;
	if(id < 1)
	{
		report("substitue_record(...) id <= 0");
		return -1;
	}
	if(id > 70000)
	{ // if id too big, maybe error!
		report("substitue_record(...) id > 70000");
		return -1;
	}
	numr = get_num_records (filename, size);
	if (id > numr)
	{
		/*
				setbfile (genbuf, currboard, ".top");
				id = id - numr;
				fd = open (genbuf, O_RDWR);
				if (fd == -1) return -1;
				flock (fd, LOCK_EX);
				if (lseek (fd, (off_t) (size * (id -1)), SEEK_SET) == -1) {
					report ("subrec seek err");
				}
				else {
					if (safewrite (fd, rptr, size) != size)
						report ("subrec write err");
				}
				flock (fd, LOCK_UN);
				close (fd);
		*/
		return 0;
	}
	if((fd=open(filename,O_WRONLY|O_CREAT,0644))==-1)
		return -1;
	flock(fd, LOCK_EX);
	if(lseek(fd, (off_t) (size * (id - 1)), SEEK_SET) == -1)
	{
		report("subrec seek err");
	}
	else
	{
		if (safewrite(fd, rptr, size) != size)
			report("subrec write err");
	}
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

void tmpfilename(char *filename, char *tmpfile, char *deleted)
{
	char   *ptr, *delfname, *tmpfname;

	strcpy(tmpfile, filename);
	delfname = ".deleted";
	tmpfname = ".tmpfile";
	if ((ptr = strrchr(tmpfile, '/')) != NULL)
	{
		strcpy(ptr + 1, delfname);
		strcpy(deleted, tmpfile);
		strcpy(ptr + 1, tmpfname);
	}
	else
	{
		strcpy(deleted, delfname);
		strcpy(tmpfile, tmpfname);
	}
}

int delete_record(char* filename, int size, int id)
{
	char    tmpfile[STRLEN], deleted[STRLEN];
	char    abuf[BUFSIZE];
	int     fdr, fdw, fd;
	int     count;

	if (size > BUFSIZE)
	{
		toobigmesg();
		return -1;
	}

	tmpfilename(filename, tmpfile, deleted);
	if ((fd = open(".dellock", O_RDWR | O_CREAT | O_APPEND, 0644)) == -1)
		return -1;
	flock(fd, LOCK_EX);

	if ((fdr = open(filename, O_RDONLY, 0)) == -1)
	{
		report("delrec open err");
		flock(fd, LOCK_UN);
		close(fd);
		return -1;
	}
	if ((fdw = open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
	{
		flock(fd, LOCK_UN);
		report("delrec tmp err");
		close(fd);
		close(fdr);
		return -1;
	}
	count = 1;
	while (read(fdr, abuf, size) == size)
		if (id != count++ && (safewrite(fdw, abuf, size) == -1))
		{
			unlink(tmpfile);
			close(fdr);
			close(fdw);
			report("delrec write err");
			flock(fd, LOCK_UN);
			close(fd);
			return -1;
		}
	close(fdr);
	close(fdw);
	if (rename(filename, deleted) == -1 ||
	        rename(tmpfile, filename) == -1)
	{
		flock(fd, LOCK_UN);
		report("delrec rename err");
		close(fd);
		return -1;
	}
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

int delete_range(char *filename, int id1, int id2, int delg, int modenow)
{
	struct fileheader fhdr;
	char    tmpfile[STRLEN], deleted[STRLEN];
	int     fdr, fdw, fd, count,cond;
	FILE	*judgefp;
	tmpfilename(filename, tmpfile, deleted);
	if((fd=open(".dellock",O_RDWR|O_CREAT|O_APPEND, 0644)) == -1)
		return -1;
	flock(fd, LOCK_EX);

	if ((fdr = open(filename, O_RDONLY, 0)) == -1)
	{
		flock(fd, LOCK_UN);
		close(fd);
		return -1;
	}
	if((fdw=open(tmpfile, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1)
	{
		close(fdr);
		flock(fd, LOCK_UN);
		close(fd);
		return -1;
	}
	count = 1;
	while(read(fdr, &fhdr, sizeof(fhdr))==sizeof(fhdr))
	{
		switch (delg)
		{
		case 0:
			cond=count<id1||count>id2||fhdr.accessed[0] & (FILE_MARKED|FILE_DIGEST)||fhdr.accessed[1] & FILE_TIP;
			break;
		case 1:
			cond=count<id1||count>id2||fhdr.accessed[0] & (FILE_MARKED)|| fhdr.accessed[1] & FILE_TIP;
			break;
		case 2:
			cond=count<id1||count>id2||fhdr.accessed[0] & (FILE_MARKED|FILE_DIGEST);
		case 3:
			cond=count<id1||count>id2||fhdr.accessed[0] & (FILE_MARKED);
		}
		if(cond)
		{
			if ((safewrite(fdw,(char *)&fhdr,sizeof(fhdr))==-1))
			{
				close(fdw);
				unlink(tmpfile);
				close(fdr);
				flock(fd, LOCK_UN);
				close(fd);
				return -1;
			}
		}
		else
		{
			char   *t;
			char    buf[STRLEN], fullpath[STRLEN];
			strcpy(buf, filename);
			if ((t = strrchr(buf, '/')) != NULL)
				*t = '\0';
			sprintf(fullpath, "%s/%s", buf, fhdr.filename);
#ifdef BACKUP_RANGE_DELETE
			/*
				 if (strcmp(currboard, "deleted")&&strcmp(currboard,"junk")){
				    cancelpost(currboard, currentuser.userid,&fhdr,
				       !strcmp(fhdr.owner, currentuser.userid));
			         } 
			*/
			/* added by wer to backup the posts deleted with 'D'*/
			/* Expand the non-backup range by quickmouse */
//			if(!NotBackupBoard())
//			{
				/*if(!strcmp(fhdr.owner, currentuser.userid))
					cancelpost(currboard, currentuser.userid,
					           &fhdr, 1);
				else*/
			cancelpost(currboard, currentuser.userid,&fhdr, 0);
//			}
		/*	else if(strcmp(currboard, "deleted")&&strcmp(currboard, "junk")&&strcmp(currboard, "deleted2"))
			{
				cancelpost(currboard, currentuser.userid,
				           &fhdr, 2);
			}*/
#endif
			if(modenow)
			{
				if((judgefp=fopen(fullpath,"r"))!=NULL)
                                {
                                        close(judgefp);
                                        unlink(fullpath);
                                }
			}
		}
		count++;
	}
	close(fdw);
	close(fdr);
	if (rename(filename, deleted) == -1)
	{
		flock(fd, LOCK_UN);
		close(fd);
		return -1;
	}
	if (rename(tmpfile, filename) == -1)
	{
		flock(fd, LOCK_UN);
		close(fd);
		return -1;
	}
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

int update_file(char *dirname, int size, int ent, int (*filecheck)(), void (*fileupdate)())
{
	char    abuf[BUFSIZE];
	int     fd;

	if (size > BUFSIZE)
	{
		toobigmesg();
		return -1;
	}

	if ((fd = open(dirname, O_RDWR)) == -1)
		return -1;

	flock(fd, LOCK_EX);
	if (lseek(fd, (off_t) (size * (ent - 1)), SEEK_SET) != -1)
	{
		if (read(fd, abuf, size) == size)
		{
			if ((*filecheck) (abuf))
			{
				lseek(fd, (off_t) (-size), SEEK_CUR);
				(*fileupdate) (abuf);
				if (safewrite(fd, abuf, size) != size)
				{
					report("update err");
					flock(fd, LOCK_UN);
					close(fd);
					return -1;
				}
				flock(fd, LOCK_UN);
				close(fd);
				return 0;
			}
		}
	}
	lseek(fd, 0, SEEK_SET);
	while (read(fd, abuf, size) == size)
	{
		if ((*filecheck) (abuf))
		{
			lseek(fd, (off_t) (-size), SEEK_CUR);
			(*fileupdate) (abuf);
			if (safewrite(fd, abuf, size) != size)
			{
				report("update err");
				flock(fd, LOCK_UN);
				close(fd);
				return -1;
			}
			flock(fd, LOCK_UN);
			close(fd);
			return 0;
		}
	}
	flock(fd, LOCK_UN);
	close(fd);
	return -1;
}

int delete_file(char* dirname, int size, int ent, int (*filecheck)())
{
	char    abuf[BUFSIZE];
	int     fd;
	struct stat st;
	long    numents;

	if (size > BUFSIZE)
	{
		toobigmesg();
		return -1;
	}

	if ((fd = open(dirname, O_RDWR)) == -1)
		return -1;

	flock(fd, LOCK_EX);
	fstat(fd, &st);
	numents = ((long) st.st_size) / size;

	if (((long) st.st_size) % size != 0)
		report("align err\n");

	if (lseek(fd, (off_t) (size * (ent - 1)), SEEK_SET) != -1)
	{
		if (read(fd, abuf, size) == size)
		{
			if ((*filecheck) (abuf))
			{
				int     i;
				for (i = ent; i < numents; i++)
				{
					if (lseek(fd, (off_t) (i * size), SEEK_SET) == -1)
						break;
					if (read(fd, abuf, size) != size)
						break;
					if (lseek(fd, (off_t) ((i - 1) * size), SEEK_SET) == -1)
						break;
					if (safewrite(fd, abuf, size) != size)
						break;
				}
				ftruncate(fd, (off_t) size * (numents - 1));
				flock(fd, LOCK_UN);
				close(fd);
				return 0;
			}
		}
	}
	lseek(fd, 0, SEEK_SET);
	ent = 0;
	while (read(fd, abuf, size) == size)
	{
		if ((*filecheck) (abuf))
		{
			int     i;
			for (i = ent; i < numents; i++)
			{
				if (lseek(fd, (off_t) ((i + 1) * size), SEEK_SET) == -1)
					break;
				if (read(fd, abuf, size) != size)
					break;
				if (lseek(fd, (off_t) (i * size), SEEK_SET) == -1)
					break;
				if (safewrite(fd, abuf, size) != size)
					break;
			}
			ftruncate(fd, (off_t) size * (numents - 1));
			flock(fd, LOCK_UN);
			close(fd);
			return 0;
		}
		ent++;
	}
	flock(fd, LOCK_UN);
	close(fd);
	return -1;
}

//****************  created by swen  02.5.31  文章顺序移动子函数**********
int move_file(char* dirname,int size, int ent, int new_pos)
{
	char tmpbuf[BUFSIZE],buf[BUFSIZE];
	int fd ;
	struct stat st ;
	long numents ;

	if( size > BUFSIZE)
	{
		toobigmesg();
		return -1 ;
	}
	if((fd = open(dirname,O_RDWR)) == -1)
		return -1 ;

	flock(fd,LOCK_EX) ;
	fstat(fd,&st) ;
	numents = ((long)st.st_size)/size ;

	if( (new_pos > numents)||(new_pos < 1) )
	{
		flock(fd,LOCK_UN);
		close(fd);
		return -2;                             //超过范围
	}
	if(((long)st.st_size) % size != 0)
	{
		fprintf(stderr,"align err\n") ;
		flock(fd,LOCK_UN);
		close(fd);
		return -1;
	}
	if(lseek(fd,size*(ent-1),SEEK_SET) != -1)
	{
		int i;
		read(fd,tmpbuf,size);
		if(new_pos > ent)
		{
			for(i = ent; i < new_pos; i++)
			{
				if(lseek(fd,(i)*size,SEEK_SET) == -1)
					break ;
				if(read(fd,buf,size) != size)
					break ;
				if(lseek(fd,(i-1)*size,SEEK_SET) == -1)
					break ;
				if(safewrite(fd,buf,size) != size)
					break ;
			}
			lseek(fd,(new_pos - 1) * size,SEEK_SET);
			safewrite(fd,tmpbuf,size);
			flock(fd,LOCK_UN) ;
			close(fd);
			return 0 ;
		}
		else
		{
			for(i = ent - 2; i >= new_pos-1; i --)
			{
				if(lseek(fd,(i)*size,SEEK_SET) == -1)
					break ;
				if(read(fd,buf,size) != size)
					break ;
				if(lseek(fd,(i + 1)*size,SEEK_SET) == -1)
					break ;
				if(safewrite(fd,buf,size) != size)
					break ;
			}
			lseek(fd,(new_pos - 1) * size,SEEK_SET);
			safewrite(fd,tmpbuf,size);
			flock(fd,LOCK_UN);
			close(fd);
			return 0;
		}
	}
	flock(fd,LOCK_UN);
	close(fd);
	return -1;
}
//***************************************************************************

//add by yiyo for sec_hand.c
int insert_record(char* fpath, char* data, int size, int pos, int num)
{
	int fd;
	off_t off, len;
	struct stat st;

	if ((fd = open(fpath, O_RDWR | O_CREAT, 0600)) < 0)
		return -1;

	flock(fd, LOCK_EX);

	fstat(fd, &st);
	len = st.st_size;

	/* lkchu.990428: ernie patch 如果 len=0 & pos>0
	                 (在刚开精华区目录进去贴上，选下一个) 时会写入垃圾 */
	off = len ? size * pos : 0;
	lseek(fd, off, SEEK_SET);

	size *= num;
	len -= off;
	if (len > 0)
	{
		fpath = (char *) malloc(pos = len + size);
		memcpy(fpath, data, size);
		read(fd, fpath + size, len);
		lseek(fd, off, SEEK_SET);
		data = fpath;
		size = pos;
	}

	write(fd, data, size);

	flock(fd, LOCK_UN);

	close(fd);

	if (len > 0)
		free(data);

	return 0;
}

