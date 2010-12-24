#include "BBSLIB.inc"
#include "NEWLIB.inc"
#include "wish.inc"

#define CNTWISHSHOW 20

int main()
{
	page_init("text/javascript");
	char ch[16];
	struct wishrec w[CNTWISHSHOW];
	int i, l, s;
	strncpy(ch, getparm("ch"), sizeof(ch));
	s = atoi(getparm("s"));
	if (s <= 0 || s > CNTWISHSHOW)
	{
		s = CNTWISHSHOW;
	}
	if (!CheckChannel(ch))
	{
		return 0;
	}
	memset(w, 0, sizeof(w));
	l = LastWishes(ch, s, w);
	printf ("createWishes([");
	for (i = 0; i < l; i++)
	{
		if (i)
		{
			printf (",");
		}
		getdatestring((w+i)->time, NA);
		printf ("{i:'%s',t:'%s',s:%d,c:'", (w + i)->id, datestring, (w + i)->seq);
		jsprint((w + i)->cont);
		printf ("'}");
	}
	printf ("]);\n");
	return 0;
}


