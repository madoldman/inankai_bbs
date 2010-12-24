#ifdef MUST_LOGIN
#undef MUST_LOGIN
#endif
#include "BBSLIB.inc"

int main()
{
	char buf[1024];
	char type[16];
	page_init("text/javascript");
	struct userec *x;
	strncpy(buf, getparm("check"), sizeof(buf));
	strncpy(type, getparm("type"), sizeof(type));
	if (!strcmp(type, "id"))
	{
		sleep(1);
		x = getuser(buf);
		if (x)
		{
			printf ("showResult('userid', 2, '该ID已经被使用');");
		}
		else if (is_bad_id(buf))
		{
			printf ("showResult('userid', 2, '该ID禁止注册');");
		}
		else
		{
			printf ("showResult('userid', 1, '');");
		}
	}
	else if (!strcmp(type, "inv"))
	{
		if (check_uuid(buf, ""))
		{
			printf ("showResult('invitation', 1, '');");
		}
		else
		{
			printf ("showResult('invitation', 2, '这个邀请码无效');");
		}
	}
	return 0;

}

