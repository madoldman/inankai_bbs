#include "BBSLIB.inc"

int main()
{
	init_all();
	printf("<h1 align='center'>����ϵͳ����</h1>");
	printf("<p align='center'>");
	if(HAS_PERM(PERM_OBOARDS))
		printf("<a href='/cgi-bin/bbs/adminfile?mode=1'>�Ƽ�����������</a>&nbsp;<a href='/cgi-bin/bbs/adminfile?mode=18'>��������</a>&nbsp;");
	if(HAS_PERM(PERM_WELCOME))
		printf("<a href='/cgi-bin/bbs/adminfile?mode=16'>վ���Ƽ�</a>&nbsp;<a href='/cgi-bin/bbs/adminfile?mode=17'>У�ڹ���</a>&nbsp;");
	if(HAS_PERM(PERM_ANNOUNCE))
		printf("<a href='/cgi-bin/bbs/adminfile?mode=19'>��ͣ�ϴ�</a>&nbsp;");
	printf("</p>");
	http_quit();
}
