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
			printf ("showResult('userid', 2, '��ID�Ѿ���ʹ��');");
		}
		else if (is_bad_id(buf))
		{
			printf ("showResult('userid', 2, '��ID��ֹע��');");
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
			printf ("showResult('invitation', 2, '�����������Ч');");
		}
	}
	return 0;

}

