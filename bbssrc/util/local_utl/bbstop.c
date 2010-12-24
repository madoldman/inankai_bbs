/* bbstop.c -- compute the top login/stay/post */
/* $Id: bbstop.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $ */

#include <stdio.h>
#define REAL_INFO
#include "bbs.h"

struct userec aman;
struct userec allman[ MAXUSERS ];
char   passwd_file[ 256 ];
char   *home_path;
//char datestring[30];

int firstlogin_cmp(b, a)
struct userec *a, *b;
{
	return (b->firstlogin - a->firstlogin);
}
int login_cmp(b, a)
struct userec *a, *b;
{
    return (a->numlogins - b->numlogins);
}

int post_cmp(b, a)
struct userec *a, *b;
{
    return (a->numposts - b->numposts);
}

int stay_cmp(b, a)
struct userec *a, *b;
{
    return (a->stay - b->stay);
}

int perm_c(a)
struct userec *a;
{
    return (a->numlogins/5 + a->numposts + a->stay/3600 + (time(0) - a->firstlogin)/86400);
}

int perm_cmp(b, a)
struct userec *a, *b;
{
    return (perm_c(a)-perm_c(b));
//a->numlogins/5 + a->numposts + a->stay/3600 + (time(0) - a->firstlogin)/86400 - 
//                (b->numlogins/5 + b->numposts + b->stay/3600 + (time(0) - b->firstlogin)/86400));
}

// Add by hightman
/*
int money_cmp(b, a)
struct userec *a, *b;
{
    return (a->money+a->nummedals*10000 - b->money - b->nummedals*10000);
}
// Add end
*/

top_login( num )
{
    int i, j, rows = (num + 1) / 2;
    char buf1[ 80 ], buf2[ 80 ];

    printf("\n\n%s", "\
[1;37m              ===========  [1;36m  ÉÏÕ¾´ÎÊýÅÅÐÐ°ñ [37m   ============ \n\n\
Ãû´Î ´úºÅ       êÇ³Æ           ´ÎÊý    Ãû´Î ´úºÅ       êÇ³Æ            ´ÎÊý \n\
==== ===============================   ==== ================================\n[0m\
");
    for (i = 0; i < rows; i++) {
        sprintf(buf1,  "[%2d] %-10.10s %-14.14s %3d",
            i+1, allman[ i ].userid, allman[ i ].username, 
            allman[ i ].numlogins );
        j = i + rows ;
        sprintf(buf2,  "[%2d] %-10.10s %-14.14s   %3d", 
            j+1, allman[ j ].userid, allman[ j ].username, 
            allman[ j ].numlogins );

        printf("%-39.39s%-39.39s[m\n", buf1, buf2); 
    }
}

top_stay( num )
{
    int i, j, rows = (num + 1) / 2;
    char buf1[ 80 ], buf2[ 80 ];

    printf("\n\n%s", "\
[1;37m              ===========   [36m ÉÏÕ¾×ÜÊ±ÊýÅÅÐÐ°ñ [37m   ============ \n\n\
Ãû´Î ´úºÅ       êÇ³Æ           ×ÜÊ±Êý  Ãû´Î ´úºÅ       êÇ³Æ           ×ÜÊ±Êý \n\
==== ================================  ==== ================================\n[0m\
");
    for (i = 0; i < rows; i++) {
        sprintf(buf1,  "[%2d] %-10.10s %-14.14s%4d:%2d",
            i+1, allman[ i ].userid, allman[ i ].username, 
            allman[ i ].stay/3600,(allman[ i ].stay%3600)/60 );
        j = i + rows ;
        sprintf(buf2,  "[%2d] %-10.10s %-14.14s%4d:%2d", 
            j+1, allman[ j ].userid, allman[ j ].username, 
             allman[ j ].stay/3600,(allman[ j ].stay%3600)/60 );

        printf("%-39.39s%-39.39s[m\n", buf1, buf2); 
    }
}

top_post( num )
{
    int i, j, rows = (num + 1) / 2;
    char buf1[ 80 ], buf2[ 80 ];

    printf("\n\n%s", "\
              [1;37m===========  [36m  ÌÖÂÛ´ÎÊýÅÅÐÐ°ñ [37m   ============ \n\n\
Ãû´Î ´úºÅ       êÇ³Æ           ´ÎÊý    Ãû´Î ´úºÅ       êÇ³Æ            ´ÎÊý \n\
==== ===============================  ===== ================================\n[0m\
");
    for (i = 0; i < rows; i++) {
        sprintf(buf1,  "[%2d] %-10.10s %-14.14s %3d",
            i+1, allman[ i ].userid, allman[ i ].username, 
            allman[ i ].numposts );
        j = i + rows;
        sprintf(buf2,  "[%2d] %-10.10s %-14.14s   %3d", 
            j+1, allman[ j ].userid, allman[ j ].username, 
            allman[ j ].numposts );

        printf("%-39.39s%-39.39s[m\n", buf1, buf2);
    }
}

top_perm( num )
{
    int i, j, rows = (num + 1) / 2;
    char buf1[ 80 ], buf2[ 80 ];

    printf("\n\n%s", "\
              [1;37m===========    [36m×Ü±íÏÖ»ý·ÖÅÅÐÐ°ñ[37m    ============ \n\
                   [32m ¹«Ê½£ºÉÏÕ¾´ÎÊý/5+ÎÄÕÂÊý+ÉÏÕ¾¼¸Ð¡Ê±[37m\n\
Ãû´Î ´úºÅ       êÇ³Æ            »ý·Ö   Ãû´Î ´úºÅ       êÇ³Æ              »ý·Ö \n\
==== ===============================   ==== =================================\n[0m\
");
    for (i = 0; i < rows; i++) {
        sprintf(buf1,  "[%2d] %-10.10s %-14.14s %5d",
            i+1, allman[ i ].userid, allman[ i ].username,perm_c(&allman[i])); 
/*            (allman[ i ].numlogins/5)+allman[ i ].numposts+(allman[ i ].stay/3600)+((time(0)-allman[i].firstlogin)/86400));
*/        j = i + rows;
        sprintf(buf2,  "[%2d] %-10.10s %-14.14s   %5d", 
            j+1, allman[ j ].userid, allman[ j ].username,perm_c(&allman[j])); 
/*            (allman[ j ].numlogins/5)+allman[ j ].numposts+(allman[ j ].stay/3600)+((time(0)-allman[i].firstlogin)/86400) );
*/
        printf("%-39.39s%-39.39s[m\n", buf1, buf2);
    }
}



top_firstlogin( num )
{
    int i=0,j=0;
	int a=0,b=0,c=0,k=0;
    char buf1[ 80 ];
	struct tm *tm;
	char weeknum[7][3]={"Ìì","Ò»","¶þ","Èý","ËÄ","Îå","Áù"};
	char datestring[30];
	int exp=0;
	int accounttime[10];
	for (k=0;k<10 ;k++ )
	{
		accounttime[k]=0;
	}

    printf("\n\n%s", "\
              [1;37m===========    [36m×¢²áÕÊºÅÊ±¼äÅÅÐÐ°ñ[1;37m    ============ \n\
Ãû´Î ÕÊºÅ             êÇ³Æ                 ×¢²áÊ±¼ä                      ¾­ÑéÖµ\n\
==== ================ ==================== ============================== ===== \n[1m\
");
	while(j<1000) {
		tm = localtime(&allman[i].firstlogin);
		sprintf(datestring,"%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ÐÇÆÚ%2s",
		tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
		tm->tm_hour,tm->tm_min,tm->tm_sec,
		weeknum[tm->tm_wday]);
		if(tm->tm_year!=70)
		{
			exp=(allman[i].numlogins/5)+allman[i].numposts+(allman[i].stay/3600)+(time(0)-allman[i].firstlogin)/86400;
			if(exp>=10000)a++;
			else if(exp>=5000)b++;
			else if(exp>=3000)c++;
			accounttime[(tm->tm_year-96)]++;
			sprintf(buf1,  "%4d %-16.16s %-20.20s %30s %s%5d[37m",
		        j+1, allman[ i ].userid, allman[ i ].username,datestring,(exp>=10000?"[33m":""),exp);
			printf("%s\n",buf1);
			j++;
		}
		i++;
	}
	printf("\nÒÔÉÏËùÁÐÕÊºÅ°´¾­ÑéÖµ(exp)·Ö²¼ÈçÏÂ£º\n");
	printf("     exp>=10000£º%3d¸ö\n",a);
	printf("5000<=exp<10000£º%3d¸ö\n",b);
	printf("3000<=exp< 5000£º%3d¸ö\n",c);
	printf("           ÆäÓà£º%3d¸ö\n",1000-a-b-c);
	printf("ÒÔÉÏËùÁÐÕÊºÅ°´×¢²áÊ±¼ä·Ö²¼ÈçÏÂ£º\n");
	for(i=0;i<6;i++)
	{
		if (accounttime[i]==0)
		{
			break;
		}
		printf("%dÄê£º%d¸ö\n",1996+i,accounttime[i]);
	}
}/*
//add by hightman
top_money( num )
{
    int i, j, rows = (num + 1) / 2;
    char buf1[ 80 ], buf2[ 80 ];

    printf("\n\n%s", "\
              [1;37m===========  [36m  ½ñÈÕ´æ¿îÅÅÐÐ°ñ [37m   ============
\n\n\
Ãû´Î ´úºÅ       êÇ³Æ           Ç®Êý    Ãû´Î ´úºÅ       êÇ³Æ            Ç®Êý \n\
==== ================================= ===== ===============================[0m\n");
    for (i = 0; i < rows; i++) {
        sprintf(buf1,  "[%2d] %-10.10s %-12.12s %8d",
            i+1, allman[ i ].userid, allman[ i ].username,
            allman[ i ].money+allman[i].nummedals*10000);
        j = i + rows;
        sprintf(buf2,  "[%2d] %-10.10s %-12.12s %6d",
            j+1, allman[ j ].userid, allman[ j ].username,
            allman[ j ].money+allman[j].nummedals*10000);

        printf("%-39.39s%-39.39s[m\n", buf1, buf2);
    }
}
//add end
*/

main(argc, argv)
int  argc;
char **argv;
{
    FILE *inf;
    int  i, no = 0,mode=0;

    if (argc < 4 ) {
        printf("Usage: %s bbs_home num_top mode\nmode=(0All 1Logins 2Posts 3Stay 4perm 5money)\n", argv[ 0 ]);
        exit( 1 );
    }

    home_path = argv[ 1 ];
    sprintf(passwd_file, "%s/.PASSWDS", home_path);
    
    no = atoi( argv[ 2 ] );
    mode=atoi(argv[ 3 ]);
    if(mode>5||mode<1)
        mode=0;
    if (no == 0) no = 20;

    inf = fopen( passwd_file, "rb" );

    if (inf == NULL) { 
        printf("Sorry, the data is not ready.\n"); 
        exit( 0 ); 
    }

    for (i=0; ; i++) {
        if (fread(&allman[ i ], sizeof( aman ), 1, inf ) <= 0) break;
        if(strcmp(allman[ i ].userid,"guest")==0)
        {
                i--;
                continue;
        }
    }

    if(mode==1||mode==0)
    {
            qsort(allman, i, sizeof( aman ), login_cmp);
            top_login( no );
    }

    if(mode==2||mode==0)
    {
            qsort(allman, i, sizeof( aman ), post_cmp);
            top_post( no );
    }

    if(mode==3||mode==0)
    {
            qsort(allman, i, sizeof( aman ), stay_cmp);
            top_stay( no );
    }
    if(mode==4||mode==0)
    {
            qsort(allman, i, sizeof( aman ), perm_cmp);
            top_perm( no );
    }
	if(mode==5||mode==0)
	{
			qsort(allman, i, sizeof( aman ), firstlogin_cmp);
			top_firstlogin( no );
	}
/*
// Add by hightman
    if(mode==5||mode==0)
    {
           qsort(allman, i, sizeof( aman ), money_cmp);
           top_money( no );
    }
// Add end
*/
    printf("\n");
}
