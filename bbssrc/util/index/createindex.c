#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define HOME "/nkbbs/bbs/0Announce"

int cr_index(char* str)
{
	int i=0,n;
	FILE* fname;
	char* pstr;
	char buf[256];
	char buf2[512];
	char temp[8];
	strcpy(temp,"");
	
	sprintf(buf,"%s%s%s",HOME,str,"/.Names");
	sprintf(buf2,"cp %s %s.bak",buf,buf);
	system(buf2);
	fname=fopen(buf,"r+");
	while(!feof(fname))
	{
		i=0;
		fgets(buf,256,fname);
		if(buf[0]=='#')
			continue;
		if(pstr=strstr(buf,"Numb="))
		{
			pstr+=5;
			while(isdigit(*pstr))
			{
				temp[i++]=*pstr;
				pstr++;
			}
			temp[i]=0;
		}
	}
	n=atoi(temp);
	fputs("Name=本板目录索引\n",fname);
	fputs("Path=~/dirindex\n",fname);
	sprintf(buf,"Numb=%d\n",++n);
	fputs(buf,fname);
	fputs("#\n",fname);
	fputs("Name=本板详细索引\n",fname);
	fputs("Path=~/index\n",fname);
	sprintf(buf,"Numb=%d\n",++n);
	fputs(buf,fname);
	fputs("#\n",fname);
	fclose(fname);
}

int main()
{
	char path[256];
	char search[256];
	char* ps;
	FILE* fsearch;

	sprintf(search,"%s%s",HOME,"/.Search");
	fsearch=fopen(search,"r");
	while(!feof(fsearch))
	{
		if(fgets(path,256,fsearch)==0)
			break;
		path[strlen(path)-1]=0;
		if((ps=strchr(path,':'))==NULL)
			continue;
		if(strlen(ps)<4)
			continue;
		while(ps[1]==' ')
			ps++;
		ps[0]='/';
		cr_index(ps);
		printf("%s\n",ps);
	}
	fclose(fsearch);
}


