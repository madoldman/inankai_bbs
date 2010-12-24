#include "bbs.h"
int file_has_word(char *file, char *word) {
        FILE *fp;
        char buf[256], buf2[256];
        fp=fopen(file, "r");
        if(fp==0) return 0;
        while(1) {
                bzero(buf, 256);
                if(fgets(buf, 255, fp)==0) break;
                sscanf(buf, "%s", buf2);
                if(!strcasecmp(buf2, word)) {
                        fclose(fp);
                        return 1;
                }
        }
        fclose(fp);
        return 0;
}

int main (void)
{
	FILE *fp,*fp1;
	fp = fopen ("/home/bbs/.BOARDS","r");
	fp1 = fopen("/home/bbs/newboard","w");
	struct boardheader x;
	char genbuf[80];
	while(fread(&x,sizeof(x),1,fp))
	{
		if(file_has_word("/home/bbs/etc/junkboards",x.filename))
		{
			x.flag2|= JUNK_FLAG;
			x.flag2|= NOTOPTEN_FLAG;
			printf("%s\n",x.filename);
		}
		fwrite(&x,sizeof(x),1,fp1);

	
	}
}
	
