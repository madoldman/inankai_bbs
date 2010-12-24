#include "bbs.h"
int main (int argc, char * args[])
{
	FILE *fp1, *fp2;
	struct userec ur1, ur2;
	char buf [256];
	if (argc != 3)
		return 0;
	sprintf (buf, "/home/bak/passwd/%s", args[1]);
	fp1 = fopen (buf, "r");
	if (fp1 == NULL)
		return -1;
	while (fread (& ur1, sizeof (ur1), 1, fp1) == 1)
	{
		if (!strcmp (ur1.userid, "Anya"))
		{
			printf ("numlogins : %d\n", ur1.numlogins);
			printf ("numposts : %d\n", ur1.numposts);
			break;
		}
	}
	fclose (fp1);
	fp2 = fopen (args [2], "r+");
	if (fp2 == NULL)
	{
		perror (args [2]);
		return -1;
	}
	while (fread (& ur2, sizeof (ur2), 1, fp2) == 1)
	{
		if (!strcmp (ur2.userid, "Anya"))
		{
			fseek (fp2, - sizeof (ur2), SEEK_CUR);
			if (fwrite (& ur1, sizeof (ur1), 1, fp2) != 1)
			{
				perror (args [2]);
				fclose (fp2);
				return -1;
			}
			break;
		}
	}
	fclose (fp2);
	return 0;
}
