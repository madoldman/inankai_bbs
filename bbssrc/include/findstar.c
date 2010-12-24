#include "bbs.h"

int main (void)
{
	int fd;
	struct userec fh;
	int nfh;
	fd = open ("/home/bbs/.PASSWDS", O_RDONLY);
	if (fd != -1)
	{
		while ((nfh = read (fd, &fh, sizeof (fh)) == sizeof (fh)))
			if (fh.userlevel & PERM_LOOKADMIN)
				printf ("%16.16s\n", fh.userid);
		close (fd);
	}
	else
		perror ("PASSWDS");
	return 0;
}
