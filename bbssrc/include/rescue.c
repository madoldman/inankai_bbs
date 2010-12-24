#include "bbs.h"

int mygetline (char * str, FILE *fp)
{
	int ch;
	while ((ch = fgetc (fp)) != EOF)
	{
		if (ch == '\n')
			break;
		*str++ = (char)ch;
	}
	*str = 0;
}

int main ()
{
	struct fileheader fh;
	time_t t;
	int i;
	FILE *fp1, *fp2, *fp3;
	char str [256];
	char str2 [256];
	char str3 [256];
	char str4 [256];
	fp1 = fopen ("/home/ok", "r");
	fp2 = fopen ("/home/bbs/boards/Military/dir2", "r+");
	fseek (fp2, 0, SEEK_END);
	if (!fp2)
	{
		perror ("fopen");
		return 1;
	}
	while (fscanf (fp1, "%s", str) != EOF)
	{
		sprintf (str2, "/home/bbs/boards/Military/%s", str);
		fp3 = fopen (str2, "r");
		if (fp3 == NULL)
			continue;
		memset (&fh, 0, sizeof (fh));
		mygetline (str3, fp3);
		if (strlen (str3) > 8)
		{
			strcpy (str4, str3 + 8);
			i = 0;
			while (str4 [i] != ' ')
				++i;
			str4 [i] = 0;
			strcpy (fh.owner, str4);
			mygetline (str3, fp3);
			if (strlen (str3) > 8)
			{
				strcpy (str4, str3 + 8);
				strcpy (fh.title, str4);
				strcpy (fh.filename, str);
				fh.filename[80-1] = 'L';
				fh.filename[80-2] = 'L';
				sscanf (str+2, "%d", &t);
				fwrite (&fh, sizeof (fh), 1, fp2);
			}
		}
	}
	fclose (fp1);
	fclose (fp2);
	return 0;
}
