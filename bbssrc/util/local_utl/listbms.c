#include <stdio.h>
#include "bbs.h"

void print_bms()
{
	FILE *fp, *bmfp;
    char path[STRLEN];
	char bmlist[STRLEN];
	char boardmaster[STRLEN];
	char board[STRLEN];
	char username[21];
    struct userec ur;
    int count = 0;
    sprintf(path, "%s/%s", BBSHOME, PASSFILE);
    if((fp = fopen(path, "r")) == NULL)
        return;
	printf("%-13s%-23s%s\n","ID","昵称","负责板面");
	while( fread(&ur, sizeof(struct userec), 1, fp) > 0)
	{
		if (ur.userlevel&PERM_BOARDS)
    	{
        	bzero(bmlist,STRLEN);
			bzero(username, 21);
        	sprintf(boardmaster, "%s/home/%c/%s/.bmfile", 
					BBSHOME, toupper(ur.userid[0]),ur.userid);
        	if ((bmfp = fopen(boardmaster, "rb")) != NULL)
        	{
				int n;
            	for(n=0; n < 3; n++)
            	{
                	if (feof(bmfp))
                    	break;
                	fscanf(bmfp,"%s\n",board);
					strcat(bmlist, board);
					strcat(bmlist,"\t");
            	}
           	 	fclose(bmfp);
       	 	}
			if(bmlist[0] == 0)
				sprintf(bmlist,"尚未管理任何板面");
			snprintf(username,20,"%s",ur.username);
			username[20] = 0;
			printf("\033[32;1m%-13s\033[33;1m%-23s\033[35;1m%s\033[0m\n",ur.userid,username,bmlist);
			count ++;
    	}
	}
	printf("\n");
	printf("总共有%d位板务，感谢您的付出\n",count);
	fclose(fp);
}

int main()
{
	printf("\t\t\t\t\t\033[1;35m%s\033[0m 板务名单\n\n", BBSNAME);
	print_bms();
	return 0;
}
