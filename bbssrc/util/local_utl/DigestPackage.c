#include "../../include/bbs.h"
#include <sys/stat.h>
#define SECNUM 14

int dashf(char *fname)
{
	struct stat st;
	return (stat(fname, &st) == 0 && S_ISREG(st.st_mode));
}

int main(int argc, char *argv[])
{
	struct boardheader *bptr, boards[MAXBOARD];
	int n, i, numboards=0,num=0;
	FILE *fp;
	char buf[256];
	char seccode[SECNUM][5]={
	"a", "b", "cde", "fg", "hi", "jk", "lm", "no", "pq", "rs", "tuv","","w","x"
	};

	if((fp=fopen("/home/bbs/.BOARDS", "r"))==NULL) return 0;
	for (n=0 ; n<MAXBOARD; n++)
	{
		if(!fread(&boards[n], sizeof(struct boardheader), 1, fp)) break;
		numboards++;
	}
	fclose(fp);
	for (n = 0; n < numboards; n++)
	{
		bptr = &boards[n];
		if(!(bptr->filename[0]))
			continue; /* Òþ²Ø±»É¾³ýµÄ°åÃæ */
		if(bptr->level > 0)  
			continue;
//		if ((bptr->level & PERM_POSTMASK))  continue;
//		if (boardprefix != NULL && strchr(boardprefix, bptr->title[0]) == NULL && boardprefix[0] != '*') continue;
//		printf("%s\n", bptr->filename);
//		num++;
		sprintf(buf,"%s/boards/%s/board.allow", BBSHOME, bptr->filename);
		if(dashf(buf))
			continue;
		if(bptr->flag & ZONE_FLAG)
			continue;
		for(i=0; i <SECNUM; i++)
		{
			if(!strchr(seccode[i], bptr->title[0]))
				continue;
			else
			{
				if(i==atoi(argv[1]))
				{
					printf("%d %s\n", atoi(argv[0]), bptr->filename);
                    if(!strcmp(bptr->filename,"Books") 
					   || !strcmp(bptr->filename,"EmpriseNovel") 
					   || !strcmp(bptr->filename,"Emprise")
					   || !strcmp(bptr->filename,"Ambassador_Home")
					   || !strcmp(bptr->filename,"BBSDev")
					   || !strcmp(bptr->filename,"IDMaster")
					   || !strcmp(bptr->filename,"BBSRules")
					   || !strcmp(bptr->filename,"Arbitration2")
					   || !strcmp(bptr->filename,"MedalMaker")
					   || !strcmp(bptr->filename,"PracticeTeam")
					   || !strcmp(bptr->filename,"PreparatoryWork")) 
						continue;
					sprintf(buf, "/home/bbs/bin/Xhtml /home/bbs/0Announce/groups/GROUP_%d/%s", i, bptr->filename);
					system(buf);
				}
			}
		}
	}
//		printf("%d\n", num);
	return 1;
}
