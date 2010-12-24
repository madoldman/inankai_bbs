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
		http_fatal("Guest不能留言");
	}
	strncpy (cont, getparm("cont"), sizeof(cont));
	strncpy (ch, getparm("ch"), sizeof(ch));
	if (!CheckChannel(ch))
	{
		http_fatal("Channel Error");
	}
	if (AppendWish(ch, cont))
	{
		http_fatal("此ID暂时不能留言!");
	}
	printf ("<script type=\"text/javascript\">alert('留言成功！');location.href='%s'</script>", getsenv("HTTP_REFERER"));
	return 0;
}

