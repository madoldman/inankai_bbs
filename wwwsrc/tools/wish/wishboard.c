#include "BBSLIB.inc"
#include "NEWLIB.inc"
#include "wish.inc"

int main()
{
	char buf[2048], cont[1024], id[16], ch[16];
	char fn[256], *p;
	int i, j;
	FILE * fp;
	init_all();
	if (!loginok)
	{
		http_fatal("Guest��������");
	}
	strncpy (cont, getparm("cont"), sizeof(cont));
	strncpy (ch, getparm("ch"), sizeof(ch));
	if (!CheckChannel(ch))
	{
		http_fatal("Channel Error");
	}
	if (AppendWish(ch, cont))
	{
		http_fatal("��ID��ʱ��������!");
	}
	printf ("<script type=\"text/javascript\">alert('���Գɹ���');location.href='%s'</script>", getsenv("HTTP_REFERER"));
	return 0;
}

