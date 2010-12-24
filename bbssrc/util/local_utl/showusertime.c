#define  FIRSTLOGIN

#include <stdio.h>
#include "../../include/bbs.h"

struct userec aman;

char *MYPASSFILE=".PASSWDS";

main(argc, argv)
int  argc;
char *argv[];
{
    FILE *inf;
    int  i;

    if (argc < 2) {
        printf("Usage: showusertime username\n");
	return 1;
	}

    
    inf = fopen( MYPASSFILE, "rb" );
    if (inf == NULL) {
        printf("Error open %s\n", MYPASSFILE); 
        exit( 0 );
    }

    for (i=0; ; i++) {
        if (fread(&aman, sizeof( aman ), 1, inf ) <= 0) break;
        if (strcmp(aman.userid,argv[1])==0){
		printf("User %s time: %d\n",aman.userid,aman.stay);
		return 0;
	}  
    }

    fclose( inf );
}
