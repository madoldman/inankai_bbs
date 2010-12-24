#include "bbs.h"
#include <dirent.h>

char tmp_users [MAXUSERS][IDLEN +2];
int num;

int found (char ur [IDLEN + 2])
{
	int p, q, i, r;
	p = 0;
	q = num;
	while (p <= q)
	{
		i = p + ((q - p) >> 1);
		r = strcmp (tmp_users [i], ur);
		if (r == 0)
			return 1;
		if (r > 0)
			q = i - 1;
		else
			p = i + 1;
	}
	return 0;
}


int main (void)
{
	DIR *dp;
	struct dirent *dirp;
	struct userec ur;
	FILE *fp;
	int nur;
	int i, j;
	char ch;
	char direction [256];
	char dname [IDLEN + 2];

	fp = fopen ("allusers", "r");
	if (fp == NULL)
		return -1;
	num = 0;
	while (fscanf (fp, "%s", tmp_users [num++]) != EOF)
		;
	fclose (fp);
	num--;
	/*
		printf ("number of users: %d\n", num);
		for (i = 0; i < num - 1; ++i) {
			int tmp = i;
			for (j = i + 1; j < num; ++j)
				if (strcmp (tmp_users [tmp], tmp_users [j]) > 0)
					tmp = j;
			if (tmp != i) {
				char sw_usr [IDLEN + 2];
				strcpy (sw_usr, tmp_users [tmp]);
				strcpy (tmp_users [tmp], tmp_users [i]);
				strcpy (tmp_users [i], sw_usr);
			}
		}
		for (i = 0; i < num; ++i)
			printf ("%s\n", tmp_users [i]);
	*/
	for (ch = 'A'; ch <= 'Z'; ++ch)
	{
		sprintf (direction, "/home/bbs/home/%c", ch);
		dp = opendir (direction);
		while ((dirp = readdir (dp)) != NULL)
		{
			if (!strncmp (dirp->d_name, ".", 1))
				continue;
			strcpy (dname, dirp->d_name);
			{
				char * p = dname;
				while (*p)
				{
					if (*p >= 'a' && *p <= 'z')
						*p += 'A' - 'a';
					p++;
				}
			}
			if (!found (dname))
			{
				//				printf ("[%s]\n", dname);
				char source [256], dest [256], command [256];
				sprintf (source, "%s/%s", direction, dirp->d_name);
				sprintf (dest, "/home/bak/home/%s", dirp->d_name);
				sprintf (command, "mv %s %s", source, dest);
				//				printf ("%s", command);
				system (command);
				sprintf (source, "/home/bbs/mail/%c/%s", ch, dirp->d_name);
				sprintf (dest, "/home/bak/mail/%s", dirp->d_name);
				sprintf (command, "mv %s %s", source, dest);
				//				printf ("%s", command);
				system (command);
			}
		}
	}
	return 0;
}
