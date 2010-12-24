
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../../include/bbs.h"

struct userec aman;

char *MYPASSFILE=".PASSWDS";

void display_binary(void)
{
    fwrite(&aman, sizeof(aman),1,stdout);
}

void display_userinfo(void)
{
    printf("User ID:		%s\n", aman.userid);
    printf("First login:	%d\n", aman.firstlogin);
    printf("Lasthost:		%s\n", aman.lasthost);
    printf("Login Number:	%d\n", aman.numlogins);
    printf("Post Number:	%d\n", aman.numposts);
    printf("Medal Number:	%d\n", aman.nummedals);
    printf("Password:		%s\n", aman.passwd);
    printf("User Name: 		%s\n", aman.username);
    
}

void display_usage(void)
{
    printf("showuserinfo -n user_number -i user_id\n");
    printf("For more information, please refer the source code, it is really simple.\n");
}

int main(int argc, char **argv)
{
    FILE *inf;
    int i, binary = 0;
    char *userid = NULL;
    int   usernum = -1;
    int   opt = 0;
    const char* optstr = "i:n:h?b";

    opt = getopt(argc, argv, optstr);
    while(opt!=-1)
    {
        switch(opt)
        {
            case 'i':
		userid = optarg;
		break;
	    case 'n':
		usernum = atoi(optarg);
		break;
  	    case 'h':
		display_usage();
		return 0;
	    case '?':
		display_usage();
		return 0;
	    case 'b':
		binary = 1;
		break;
	    default:
		//no way
		break;
        }

        opt = getopt(argc, argv, optstr);
    }//end of while
   
    inf = fopen( MYPASSFILE, "rb" );
    if (inf == NULL) {
        printf("Error open %s\n", MYPASSFILE); 
        exit( 0 );
    }
   
    if (userid!=NULL)
    {
    	for (i=0; ; i++) {
            if (fread(&aman, sizeof( aman ), 1, inf ) <= 0) break;
            if (strcmp(aman.userid,userid)==0){
                printf("User Index is : 	%d\n", i);
		binary ? display_binary(): display_userinfo();
		break;
	    }  
        }
    }

    if (usernum!=-1)
    {
	if (fseek(inf, usernum*sizeof(aman), SEEK_SET)==-1)
	{
 	    printf("No such index\n");
	    exit(1);
        }

	if (fread(&aman, sizeof(aman), 1, inf) <=0)
        {
	    printf("Read user index %d error\n", usernum);
	    exit(1);
 	}
	
	binary? display_binary():display_userinfo();
    }

    fclose( inf );

}
