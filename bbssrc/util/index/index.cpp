#include "index.h"
void main(void)
{
	fstream boards;
	char boardsFile[256];
	char boardLine[256];
	Index index;
	sprintf(boardsFile,"%s%s",HOME,"/.Search");
	boards.open(boardsFile,ios::in);
	while(!boards.eof())
	{
		boards.getline(boardLine,256,':');
		boards.getline(boardLine,256);
		if (strlen(boardLine)<4)
			continue;
		boardLine[0]='/';
		printf("%s\n",boardLine);
		index.init(boardLine);
		index.finish();
	}
	boards.close();
}
