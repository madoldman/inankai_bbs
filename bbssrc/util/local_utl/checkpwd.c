#include <stdio.h>
#include "bbs.h"
#define MYPASSFILE "/home/bbs/.PASSWDS"
int main(int argc, char** argv)
{
	int i;
	struct userec aman;
	FILE* inf;
	if(argc != 3){
		printf("usage:%s userid password!\n", argv[0]);
		return -1;
	}
	inf = fopen( MYPASSFILE, "rb" );
	if (inf == NULL) {
		printf("3\n");
		return 3;
	}

	if (argv[1] != NULL)
	{   
		for (i=0; ; i++) {
			if(fread(&aman, sizeof(aman), 1, inf ) <= 0) 
				break;
			if (!strcasecmp(aman.userid, argv[1]))
				break;
		}
	}
	if (strcasecmp(aman.userid, argv[1])){		
		printf("2\n");
		return 2;
	}
	if(checkpasswd(aman.passwd,argv[2])){
		printf("0\n");
		return 0;
	}
	else{
		printf("1\n");
		return 1;
	}
	return 0;
}
