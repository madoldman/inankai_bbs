#include "BBSLIB.inc"

int main()
{
	init_all();
	printf("<h1 align='center'>编修系统档案</h1>");
	printf("<p align='center'>");
	if(HAS_PERM(PERM_OBOARDS))
		printf("<a href='/cgi-bin/bbs/adminfile?mode=1'>推荐讨论区管理</a>&nbsp;<a href='/cgi-bin/bbs/adminfile?mode=18'>区长名单</a>&nbsp;");
	if(HAS_PERM(PERM_WELCOME))
		printf("<a href='/cgi-bin/bbs/adminfile?mode=16'>站内推荐</a>&nbsp;<a href='/cgi-bin/bbs/adminfile?mode=17'>校内公告</a>&nbsp;");
	if(HAS_PERM(PERM_ANNOUNCE))
		printf("<a href='/cgi-bin/bbs/adminfile?mode=19'>暂停上传</a>&nbsp;");
	printf("</p>");
	http_quit();
}
