//#include "BBSLIB.inc"
//#include "boardrc.inc"
#include <time.h>
#include <stdio.h>
#include <rpcsvc/rstat.h>
#include "bbs.h"
	char datestring[80];

int main(argc, argv)
int  argc;
char **argv;
{
	char board[80];
	char buf[256];
	strcpy(board, argv[1]);
	sprintf(buf,"/bin/cp /nkbbs/bbs/boards/%s/.DIR1 /nkbbs/bbs/boards/%s/.DIR",board,board);
	system(buf);
}