#include "bbs.h"

int main (void)
{
	int fd;
	struct userec ur;
	fd = open ("/home/bbs/.PASSWDS", O_RDONLY);
	if (fd == -1)
	{
		perror ("open");
		return -1;
	}
	while (read (fd, & ur, sizeof (ur)) == sizeof(ur))
	{
		if (ur.userlevel & PERM_LOOKADMIN)
			printf ("%s\n", ur.userid);
	}
	close (fd);
	return 0;
}
