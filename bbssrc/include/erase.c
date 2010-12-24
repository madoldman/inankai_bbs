#include "bbs.h"
int main (void)
{
	struct boardheader bhp;
	int fd, fd2;
	fd = open ("/home/bbs/.BOARDS", O_RDWR);
	fd2 = open ("/home/bbs/boards2", O_RDWR);
	if (fd != -1 && fd2 != -1)
	{
		while (read (fd, &bhp, sizeof (bhp)) > 0)
		{
			if (strcmp (bhp.filename, "Swimming"))
				write (fd2, &bhp, sizeof(bhp));
		}
		close (fd);
		close (fd2);
	}
	return 0;
}
