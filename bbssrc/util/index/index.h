#define HOME "/nkbbs/bbs/0Announce"
#define BLUENESS "[1;36m"
#define YELLOW   "[1;33m"
#define RED      "[1;31m"
#define NOCOLOR  "[m"
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

class Index
{
	fstream IndexFile;
public:
	void init(char *path);
	void finish(void);
	void run(char *path,int depth);
};
void Index::init(char *path)
{
	char fileName[256];
	time_t now;
	time(&now);
	sprintf(fileName,"%s%s%s",HOME,path,"/index");
	IndexFile.open(fileName,ios::out);
	sprintf(fileName,"%s%s",HOME,path);
	IndexFile<<"\t\tÎÒ°®ÄÏ¿ª¾«»ªÇøÏêÏ¸Ë÷Òý×Ô¶¯Éú³ÉÏµÍ³"<<endl;
        IndexFile<<"\t\t¸üÐÂÊ±¼ä£º"<<ctime(&now)<<endl;
        IndexFile<<"©Ð["<<RED<<"±¾°å¾«»ªÇøË÷Òý"<<NOCOLOR<<"]"<<endl;
	run(fileName,0);
}
void Index::finish(void)
{
        IndexFile<<"©¸["<<RED<<"±¾°å¾«»ªÇøË÷Òý"<<NOCOLOR<<"]"<<endl;
	IndexFile.close();
}
void Index::run(char *path,int depth)
{
	fstream NameFile;
	char fileName[256];
	char Line[256];
	char LineName[39];
	char LineFile[256];
	DIR *dir;
        int i;
	sprintf(fileName,"%s%s",path,"/.Names");
	NameFile.open(fileName,ios::in);
	NameFile.getline(Line,256);
	NameFile.getline(Line,256);
        NameFile.getline(Line,256);
	while(!NameFile.eof())
	{
		NameFile.getline(Line,256);
		if (Line[0]!='N')
			break;
		strcpy(LineName,Line+5);
		LineName[38]=0;
		NameFile.getline(Line,256);
		sprintf(LineFile,"%s%s",path,Line+6);
		if ((dir = opendir(LineFile)) != NULL&&strcmp(Line,"Path=~/"))
		{
			closedir(dir);
			for (i=0;i<depth;i++)
				IndexFile<<"©¦";
			IndexFile<<"©À©Ð["<<YELLOW<<"¨ŒÄ¿Â¼"<<NOCOLOR<<"] "<<LineName<<endl;
			run(LineFile,depth+1);
			for (i=0;i<depth;i++)
				IndexFile<<"©¦";
			IndexFile<<"©¦©¸["<<YELLOW<<"¡÷Ä¿Â¼"<<NOCOLOR<<"] "<<LineName<<endl;
		}
		else
		{
			for (i=0;i<depth;i++)
				IndexFile<<"©¦";
        		IndexFile<<"©À©¤["<<BLUENESS<<"ÎÄ¼þ"<<NOCOLOR<<"] "<<LineName<<endl;
		}
		NameFile.getline(Line,256);
		NameFile.getline(Line,256);
	}
}
