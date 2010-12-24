#include <string.h>
#include "bbs.h"

char    tname[STRLEN];
char    fname[STRLEN];
extern char currboard[STRLEN];
extern char someoneID[31];

int marked_all(int type)
{
	struct fileheader post;
	int     fd;
	char    dname[STRLEN],buf[STRLEN],board2[STRLEN];
	char	tempname1[50],tempname2[50];
	struct stat st1, st2;
	int 	recnum=0;
	sprintf(dname, "boards/%s/%s", currboard, DOT_DIR);
	sprintf(fname, "boards/%s/%s2", currboard, DOT_DIR);

	setbdir(board2, currboard);
	sprintf(dname, "%s",board2);
	sprintf(fname, "%s2",board2);
	//d  move(0,0);
       //dd prints("%d:files:%s,%s",digestmode,dname,fname);
       //d pressanykey();

	
	switch(type)
	{
	case 0:
		sprintf(tname, "boards/%s/%s", currboard, MARKED_DIR);
		break;
	case 1:
		sprintf(tname, "boards/%s/%s", currboard, AUTHOR_DIR);
		break;
	case 2:
	case 3:
		sprintf(tname, "boards/%s/SOMEONE.%s.DIR.%d",currboard,someoneID,type-2);
		break;
	case 4:
		sprintf(tname,"boards/%s/KEY.%s.DIR",currboard,currentuser.userid);
		break;
	}
	if (stat(dname, &st1) == -1)
		return -2;
	if (stat(tname, &st2) != -1&&digestmode==NA)
	{
		if (st2.st_mtime >= st1.st_mtime)
		{
	return (st2.st_size/sizeof(struct fileheader));//add by brew
			return 1;
		}
	}
	sprintf(buf, "cp %s %s", dname, fname);
	system(buf);
	unlink(tname);
	
	if ((fd = open(fname, O_RDONLY, 0)) == -1)
		return -3;
	while (read(fd,&post,sizeof(struct fileheader))==sizeof(struct fileheader))
	{
		switch(type)
		{
		case 0:
			if( post.accessed[ 0 ] & FILE_MARKED )
			{
				append_record(tname, &post, sizeof(post));
				recnum++;
			}
			break;
		case 1:
			if(  strncmp(post.title,"Re:", 3)&& strncmp(post.title,"RE:", 3))
			{
				append_record(tname, &post, sizeof(post));
				recnum++;
			}
			break;
		case 2:
		case 4:
			if(type ==2 )
				strtolower(tempname1,post.owner);
			else
				strtolower(tempname1,post.title);
			strtolower(tempname2,someoneID);
			if ( strstr(tempname1,tempname2) )
			{
				append_record(tname, &post, sizeof(post));
				recnum++;
			}
			break;
		case 3:
			if ( !strcasecmp(post.owner,someoneID))
			{
				append_record(tname, &post, sizeof(post));
				recnum++;
			}
			break;
		}
	}
	close(fd);
	unlink(fname);
	return recnum;
}
