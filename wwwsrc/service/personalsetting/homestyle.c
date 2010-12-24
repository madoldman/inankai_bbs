#include "BBSLIB.inc"

int main()
{
	init_all();
	int blnReload=0,i=0;
	FILE * fp=0,* cfp;
	char *p;
	char buf[256];
	char fn[256];
	printf(
	"<script language=javascript src=/js/common.js></script>"
	);
	//if(*(getparm("reload")))
		//blnReload=1;
	if(!blnReload)
	{
		if(!(fp=fopen("/home/bbs/etc/.csslist","rt")))
			blnReload=1;
	}
	if(blnReload)
	{
		unlink("/home/bbs/etc/.csslist");
		system ("ls /home/www/html/css -1 > /home/bbs/etc/.csslist");
		printf(
"<h3 align=\"center\">正在跳转，请稍候……</h3>"
"<script language=\"javascript\">"
"var tmr;"
"tmr=setTimeout(rld,1000);"
"function rld()"
"{"
"	location.href='homestyle';"
"}"
"</script>"
		);
		http_quit();
	}
	strncpy(buf,getparm("home"),255);
	*(buf+255)=0;
	for(p=buf;*p;p++)
	{
		if(!strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_",*p))
		{
			*p=0;
			break;
		}
	}
	if(*buf)
	{
		sethomefile(fn, currentuser.userid, ".www.style");
		if(cfp=fopen(fn,"w"))
		{
			fprintf(cfp,"%s",buf);
			fclose(cfp);
		}
		printf(
"<h3 align=\"center\">正在跳转，请稍候……</h3>"
"<script language=\"javascript\">"
"var tmr;"
"tmr=setTimeout(rld,10);"
"function rld()"
"{"
"	document.cookie='home_style=%s';"
"	document.cookie='home_style=%s; path=/';"
"	refreshall();"
"	location.href='/bbsmain.htm';"
"}"
"</script>"
		,buf,buf);
		http_quit();
	}
	printf(
"<h1 align=\"center\">自定义样式</h1>"
"<p align=\"center\"><a href=\"/cgi-bin/bbs/bbsdoc?board=ArtsDev\">ArtsDev期待你的创意</a></p>"
"<table align=\"center\" border=\"0\">"
	);
	for(i=0;;i++)
	{
		if(fgets(buf,255,fp)<=0)
			break;
		if(!(i&1))
			printf("<tr>");
		printf(
		"<td><a href=\"homestyle?home=%s\"><img src=\"/css/%s/prev.png\" width=\"320\" border=\"1\" title=\"%s 点击选用\"></a></td>"
		,buf,buf,buf);
		if(i&1)
			printf("</tr>");
	}
	if(i&1)
		printf("</tr>");
	printf(
"</table>"
	);
	fclose(fp);
	return 0;
}
