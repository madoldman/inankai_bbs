/*$Id: dl.c,v 1.2 2008-12-19 09:55:10 madoldman Exp $
*/
//#define SILENCE
#include "BBSLIB.inc"
#define QMD_PATH "/home/www/upload"

//huangxu@060331
int main()
{
	char strbuf[1024];
	strsncpy(strbuf, getsenv("HTTP_USER_AGENT"), 1023);
	if (strcasestr(strbuf,"baiduspider"))return 0;
	strsncpy(strbuf,getsenv("QUERY_STRING"),1023);
	if(!strncasecmp(strbuf,"/upload/upload2/",16))sprintf(strbuf,"/up2/%s",strbuf+16);
	printf("Location: %s\n",strbuf);
	init_all();
	return 0;
}

