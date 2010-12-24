#include "convert.h"
int main(int argc,char* argv[])
{
	char FileLine[MAX_LINE_LEN];
	if (argc==1||strlen(argv[1])<3)
	{
		cout<<"没有输入关键字或者关键字太短！"<<endl;
		return 0;
	}
	if (argc==2)
	{
		ifstream FileList;
		FileList.open(LISTFILE);
		while (!FileList.eof())
		{
			FileList.getline(FileLine,MAX_LINE_LEN);
			if (strlen(FileLine)<=5)
				break;
			search(FileLine,argv[1]);
		}
		FileList.close();
	}
	else
		search(argv[2],argv[1]);
	return 0;
}
