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
    int  maxstay = atoi(argv[1]);

    if (argc < 2) {
        printf("Usage: scanbaduser <maxtime> \n");
	return 1;
	}

    
    inf = fopen( MYPASSFILE, "rb" );
    if (inf == NULL) {
        printf("Error open %s\n", MYPASSFILE); 
        exit( 0 );
    }

    for (i=0; ; i++) {
        if (fread(&aman, sizeof( aman ), 1, inf ) <= 0) break;
        if (aman.stay >= maxstay){
		printf("%s\n",aman.userid);
	}  
    }

    fclose( inf );
}
