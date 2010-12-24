#include "convert.h"
void main(int argc,char* argv[])
{
	char FileLine[MAX_LINE_LEN];
	if (argc==1)
	{
		ifstream FileList;
		FileList.open(LISTFILE);
		while (!FileList.eof())
		{
			FileList.getline(FileLine,MAX_LINE_LEN);
			if(strlen(FileLine))
				make(FileLine);
		}
		FileList.close();
	}
	else
		make(argv[1]);
}
