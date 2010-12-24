//userindex.wml.c
//huangxu@nkbbs.org

//ÒÑ·ÏÆú£º£¨

#include "BBSLIB.inc"
#include "WMLLIB.inc"

int userindex_act(char * info)
{
	*info = 0;
	return -1;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (userindex_act(buf))
	{
		printf ("<card title=\"´íÎó\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />·µ»Ø</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}

