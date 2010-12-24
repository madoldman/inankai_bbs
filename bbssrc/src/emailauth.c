#include <stdio.h>
#include "bbs.h"
#ifndef EMAILAUTH //   "AuthMaster"
#define  EMAILAUTH    "AuthMaster"
#define  EMAILAUTHB     'A'
#endif
//add by smalldog
int _Readln22( FILE* fp, char *buf ,int bufsize)
{
	int retl = 0;
	int line=0;
	char cn = '\0';
	do
	{
		if( (cn=fgetc(fp)) == EOF )
	        {
		        buf[retl]=0;
		        return retl;
		}
		if(line==1)
		{
			buf[ retl++ ] = cn;
		}
		switch(cn)
		{
			case '\n':
			case '\r':
				line++;
				if( retl && line==2)
				{
					buf[ retl ] = 0;
					return retl;
				}	
				break;
		}
                if(bufsize-1<=retl)
	        {
		        buf[ retl ] = 0;
		        return retl;
		}
	}while( 1 );
	return 1;
}
//add by brew
int _Readln2( FILE* fp, char *buf ,int bufsize)
{
    int retl = 0;
        char cn = '\0';
    do
        {
                if( (cn=fgetc(fp)) == EOF )
                {
                        buf[retl]=0;
                        return retl;
                }
 
                buf[ retl++ ] = cn;
 
                switch(cn)
                {
                case '\n':
                case '\r':
                        if( retl )
                        {
                                buf[ retl ] = 0;
                                return retl;
                        }
                        break;
                }
                if(bufsize-1<=retl)
                {
                        buf[ retl ] = 0;
                        return retl;
                }
        }
        while( 1 );
        return 1;
}

int del_authrow_has_userid(char *file, char *word) {
    FILE *fp1;
    FILE *fp2;
    char buf[1024];
    char file2[STRLEN];
    char wordd[STRLEN+20];
    char *pcmpbuf;
        int  bfount=0;
        strcpy(wordd,".edu.cn,");
        strcat(wordd,word);
        strcat(wordd,",");
    fp1=fopen(file, "r");
        sprintf(file2,"%s.tmp",file);
        fp2=fopen(file2,"w");
    if(fp1==0||fp2==0) return 0;//mail/T/email.auth
	flock(fileno(fp1),LOCK_EX);
	 flock(fileno(fp2),LOCK_EX);
        while(1) {
            bzero(buf, 1024);
            if(_Readln2(fp1,buf,1024)>0)
            {
                if(bfount==0)
                {
 
                        pcmpbuf=strstr(buf,".edu.cn");
                        if(pcmpbuf!=NULL&&strstr(pcmpbuf,wordd)!=0)
                        {
			        bfount = 1;
			}
			else
			   fwrite(buf,strlen(buf),1,fp2);
                }
                else
                {
                        fwrite(buf,strlen(buf),1,fp2);
                }
            }
            else
              break;
    }
    fflush(fp2);
    sprintf(buf,"cp -f %s %s",file2,file);
    system(buf);
    flock(fileno(fp1),LOCK_UN);
    flock(fileno(fp2),LOCK_UN);
    fclose(fp1);
    fclose(fp2);
    return 0;
 
}
char  GetAuthUserMailF(char *userid)
{
      char    filename[STRLEN];
      char    emailaddr[STRLEN]={0};
      FILE *fp;
      sprintf(filename, "home/%c/%s/.webauth.done", toupper(userid[0]), userid);
      fp=fopen(filename, "r");
      if(fp==0)
	return 0;
      _Readln22(fp,emailaddr,STRLEN);
      fclose(fp);
	
      return emailaddr[0];
}
