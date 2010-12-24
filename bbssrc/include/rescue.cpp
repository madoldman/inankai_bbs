#include <cstring>
#include <cstdio>
#include <fstream>
#define STRLEN 80
using namespace std;
struct fileheader {             /* This structure is used to hold data in */
        char filename[STRLEN];     /* the DIR files */
        char owner[STRLEN];
        char title[STRLEN];
        unsigned level;
        unsigned char accessed[ 12 ];   /* struct size = 256 bytes */
} ;

int main ()
{
	struct fileheader fh;
	time_t t;
	int i;
	FILE *fp1, *fp2;
	char str [256];
	char str2 [256];
	char str3 [256];
	char str4 [256];
	fp1 = fopen ("/home/ok", "r");
	fp2 = fopen ("/home/bbs/boards/Military/dir2", "w");
	if (!fp2) {
		perror ("fopen");
		return 1;
	}
	while (fscanf (fp1, "%s", str) != EOF) {
		sprintf (str2, "/home/bbs/boards/Military/%s", str);
		ifstream fin (str);
		memset (&fh, 0, sizeof (fh));
		fin.getline (str3, 256);
		if (strlen (str3) > 8) {
			strcpy (str4, str3 + 8);
			i = 0;
			while (str4 [i] != ' ') ++i;
			str4 [i] = 0;
			strcpy (fh.owner, str4);
			fin.getline (str3, 256);
			if (strlen (str3) > 8) {
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
