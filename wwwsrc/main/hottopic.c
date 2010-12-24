#include "BBSLIB.inc"

void trimend(char* src,int lenth);

int main()
{
	FILE *fp;
	int n;
	char s1[256];
	char brd[256],id[16], title[256];
	char title_in_url[240];
	char path[]="etc/posts/day";
	html_init();
	fp=fopen(path, "r");
	if(fp==NULL)
	return 1;
	fgets(s1, 255, fp);
	fgets(s1, 255, fp);
	//head
	printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/transitional.dtd\"> "
	"<html><head><title>十大话题</title><meta http-equiv=\"content-type\" content=\"text/html; charset=gb18030\">"
	"<meta HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\" /><meta name=\"%s\" content=\"%s top10bless\">"
	"<style type=\"text/css\">\nbody{padding: 0px;background-color: #FFF;font: 100.01% \"Trebuchet MS\",Verdana,Arial,sans-serif}"
	"h1,h2,p{margin: 0 10px}h1{font-size: 250%;color: #FFF}h2{font-size: 200%;color: #f0f0f0}p{padding-bottom:1em}"
	"h2{padding-top: 0.3em}div#nifty{ margin: auto auto;background: #0000CC;width: 508px; text-align: center}"
	"b.rtop, b.rbottom{display:block;background: #FFF}b.rtop b, b.rbottom b{display:block;height: 1px;overflow: hidden; background: #0000CC}"
	"b.rtop b.r4, b.rbottom b.r4{margin: 0 1px;height: 2px}"
	".top10title {  font-size: 12px; color: #3333FF; text-decoration: none; background-color: #ECF2FF; text-align: left}"
	".top10id { font-size: 12px; color: #888888; text-decoration: none; background-color: #ECF2FF; font-family: \"Arial\", \"Helvetica\", \"sans-serif\";text-align: left}"
	".top10board { font-size: 12px; color: #9999FF; text-decoration: none; background-color: #ECF2FF; font-family: \"Courier New\", \"Courier\", \"mono\"; font-style: italic; font-weight: bold;text-align: left}"
	".top10 { font-size: 18pt; color: #FFFFFF; text-decoration: none; background-color: #0000CC; font-family: \"新宋体\", \"宋体\", \"Fixedsys\"; text-align: center; font-weight: bold}"
	"div#blesstop{ margin: auto auto;background: #CC3300;width: 508px; text-align: center}"
	"b.rtopbless, b.rbottombless{display:block;background: #FFF}b.rtopbless b, b.rbottombless b{display:block;height: 1px;overflow: hidden; background: #CC3300}"
	"b.r1{margin: 0 5px}b.r2{margin: 0 3px}b.r3{margin: 0 2px}b.rtopbless b.r4, b.rbottombless b.r4{margin: 0 1px;height: 2px}"
	".top10blesstitlebless {  font-size: 12px; color: #FF3300; text-decoration: none; background-color: #FFFBFB; text-align: left}"
	".top10blessidbless { font-size: 12px; color: #009933; text-decoration: none; background-color: #FFFBFB; font-family: \"Arial\", \"Helvetica\", \"sans-serif\";text-align: left}"
	".top10blessboardbless { font-size: 12px; color: #9999FF; text-decoration: none; background-color: #FFFBFB; font-family: \"Courier New\", \"Courier\", \"mono\"; font-style: italic; font-weight: bold;text-align: left}"
	".top10bless { font-size: 18pt; color: #FFFFFF; text-decoration: none; background-color: #CC3300; font-family: \"新宋体\", \"宋体\", \"Fixedsys\"; text-align: center; font-weight: bold}"
	".top10blessboardblesslink { font-size: 14.7px; color: #FFFFFF; text-decoration: none; background-color: #CC3300; font-family: \"楷体_GB2312\"; text-align: center; font-weight: bold }"
	".star {  color: #FF9966; font-weight: bold}</style></head>",BBSID,BBSID);
	printf("<body><div id=\"nifty\"><b class=\"rtop\"><b class=\"r1\"></b><b class=\"r2\"></b><b class=\"r3\"></b><b class=\"r4\"></b></b>"
	"<div align=\"center\"><table width=\"500\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr>"
	"<td colspan=\"3\"  class=\"top10\"> 热门话题钟点播报 </td></tr><tr height=\"5px\"></tr>");
	for(n=1; n<=10; n++) {
		if(fgets(s1, 255, fp)<=0)break;
		sscanf(s1+45, "%s", brd);
		sscanf(s1+122, "%s", id);
		if(fgets(s1, 255, fp)<=0)break;
		strsncpy(title, s1+27, 60);
		trimend(title,59);
		kill_quotation_mark(title_in_url,title);
		printf("<tr><td class=\"top10board\"><a href=\"/cgi-bin/bbs/bbsdoc?board=%s\" class=\"top10board\">%s</a></td>"
		"<td class=\"top10title\"><a href=\"/cgi-bin/bbs/newcon?board=%s&amp;title=%s\" class=\"top10title\">○%s </a> "
		"</td><td class=\"top10id\"> <a href=\"/cgi-bin/bbs/bbsqry?userid=%s\" class=\"top10id\">%s</a></td></tr>",
		brd,brd,brd,title_in_url,title,id,id);
	}
	fclose(fp);
	printf("</table></div><b class=\"rbottom\"><b class=\"r4\"></b><b class=\"r3\"></b><b class=\"r2\"></b><b class=\"r1\"></b></b>"
	"</div><p></p><p></p><div id=\"blesstop\"><b class=\"rtopbless\"><b class=\"r1\"></b><b class=\"r2\"></b><b class=\"r3\"></b><b class=\"r4\"></b></b>"
	"<div align=\"center\"><table width=\"500\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\"><tr> "
	"<td colspan=\"2\"  class=\"top10bless\">本日十大衷心祝福</td></tr><tr>"
	"<td colspan=\"2\" class=\"top10blessboardblesslink\" height=\"22px\"><a href=\"/cgi-bin/bbs/bbstdoc?board=Blessing\" class=\"top10blessboardblesslink\">欢迎光临Blessing·星语心愿 </a></td>"
	"</tr>");
	fp=fopen("etc/bless/day", "r");
	if(fp==NULL)
	return 1;
	fgets(s1, 255, fp);
	fgets(s1, 255, fp);
	for(n=1; n<=10; n++) {
		if(fgets(s1, 255, fp)<=0)break;
		sscanf(s1+45, "%s", brd);
		sscanf(s1+122, "%s", id);
		if(fgets(s1, 255, fp)<=0)break;
		strsncpy(title, s1+27, 60);
		trimend(title,59);
		kill_quotation_mark(title_in_url,title);
		printf("<tr><td class=\"top10blesstitlebless\"><a href=\"/cgi-bin/bbs/newcon?board=Blessing&amp;title=%s\" class=\"top10blesstitlebless\">"
		"<span class=\"star\">★&nbsp;</span>%s </a></td><td class=\"top10blessidbless\">"
		"<a href=\"/cgi-bin/bbs/bbsqry?userid=%s\" class=\"top10blessidbless\">%s</a></td></tr>",
		title_in_url,title,id,id);
	}
	printf("</table>");
	fclose(fp);
	printf("</table></div><b class=\"rbottombless\"><b class=\"r4\"></b><b class=\"r3\"></b><b class=\"r2\"></b><b class=\"r1\"></b></b>"
	"</div></body></html>");
	return 0;
}

void trimend(char* src,int lenth)
{
	char * p;
	for (p=src+lenth;(*p==0x20||(!*p))&&p>src;p--)
		*p=0;
	return;
}
