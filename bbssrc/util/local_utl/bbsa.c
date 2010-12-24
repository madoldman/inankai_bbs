//#include "BBSLIB.inc"
//#include "boardrc.inc"
#include <time.h>
#include <stdio.h>
#include <rpcsvc/rstat.h>
#include "bbs.h"

int main(argc, argv)
int  argc;
char **argv;
{
	FILE *fp;
	FILE *fp2;
	FILE *fp3;
	int i;
	char board[80],buf[256],dir[80],filenameold[80];

	struct fileheader x,y;

	strcpy(board, argv[1]);
	printf("Current Board:%s\n",board);

	printf("Copying backup files to the current site...\n");

	sprintf(buf,"/bin/cp /backup/nkbbs/bbs/boards/%s/*.A /nkbbs/bbs/boards/%s",board,board);
	system(buf);

	printf("Changing permissions...\n");

	sprintf(buf,"chown bbs /nkbbs/bbs/boards/%s/*",board);
	system(buf);

	sprintf(buf,"chgrp bbs /nkbbs/bbs/boards/%s/*",board);
	system(buf);

	sprintf(buf,"/bin/cp /nkbbs/bbs/boards/%s/.DIR.old /nkbbs/bbs/boards/%s/.DIR.tmp",board,board);
	system(buf);

	printf("Setting files...\n");
	sprintf(buf,"/bin/cp /nkbbs/bbs/boards/%s/.DIR /nkbbs/bbs/boards/%s/.DIR1",board,board);
	system(buf);
	sprintf(dir, BBSHOME"/boards/%s/.DIR", board);
	sprintf(buf, BBSHOME"/boards/%s/.DIR.tmp", board);
	if(fp=fopen(dir,"r")){
		fclose(fp);
	}
	if(fp2=fopen(buf, "r")) {
		for(i=0;i<20000;i++){
		fseek(fp2, i*sizeof(struct fileheader), SEEK_SET);
		if(fread(&y, sizeof(y), 1, fp)<=0) break;
	}
	}
			strcpy(filenameold,y.filename);

	fclose(fp2);
	printf("\nOldest File:%s\n",filenameold);
    fp=fopen(dir, "r");
	fp2=fopen(buf, "a");
	if(fp){
		printf("Converting files...\n");
		for(i=0;i<20000;i++){
		fseek(fp, i*sizeof(struct fileheader), SEEK_SET);
		if(fread(&x, sizeof(x), 1, fp)<=0) break;
		if(atoi(x.filename+2)>atoi(filenameold+2)){
			fwrite(&x, sizeof(x), 1, fp2);
		}
		}
			fclose(fp2);
			fclose(fp);
	}
	sprintf(buf, BBSHOME"/boards/%s/.DIR.tmp", board);
	fp=fopen(buf, "r");
	sprintf(buf, BBSHOME"/boards/%s/.DIR.new", board);
	fp2=fopen(buf, "w");
	for(i=0;i<20000;i++)
	{
		fseek(fp, i*sizeof(struct fileheader), SEEK_SET);
		if(fread(&x, sizeof(x), 1, fp)<=0) break;
		sprintf(buf,BBSHOME"/boards/%s/%s",board,x.filename);
		if (fp3=fopen(buf, "r"))
		{
			fwrite(&x, sizeof(x), 1, fp2);
			fclose(fp3);
		}
		else
			printf("deleted file:%s\n",x.filename);
	}
	fclose(fp);
	fclose(fp2);
	
	sprintf(buf,"/bin/cp /nkbbs/bbs/boards/%s/.DIR.new /nkbbs/bbs/boards/%s/.DIR",board,board);

	system(buf)	;
	printf("done.");
//	http_fatal("done");
}
