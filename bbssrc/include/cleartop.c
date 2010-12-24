#include "bbs.h"

int main (int argc, char * argv [])
{
	int size = sizeof (struct fileheader);
	int nfh;
	struct fileheader fh;
	int fd;
	char buf [STRLEN];

	if (argc != 2)
	{
		printf ("usage: cleartop [boardname]\n");
		return 0;
	}
	sprintf (buf, "/home/bbs/boards/%s/.DIR", argv [1]);

	fd = open (buf, O_RDWR);
	if (fd == -1)
	{
		perror (buf);
		return -1;
	}

	while ((nfh = read (fd, &fh, size)) == size)
	{
		if (fh.accessed[1] & FILE_ONTOP)	/* Efan: .DIR文件不应该出现 FILE_ONTOP	*/
		{
			fh.accessed[1] &= ~FILE_ONTOP;
			lseek (fd, -size, SEEK_CUR);
			write (fd, &fh, size);
		}
	}
	return 0;
}
