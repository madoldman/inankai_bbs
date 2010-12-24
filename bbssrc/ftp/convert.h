#define MAX_FILE_LEN  100
#define MAX_PATH_LEN  150
#define MAX_LINE_LEN  256
#define FILE_TIME_LINE 14
#define LISTFILE "/home/bbssrc/ftp/FTPFileList"
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
struct FileInfo
{
	char FileName[MAX_FILE_LEN];
	char PathName[MAX_PATH_LEN];
	int  FileSize;
	char FileTime[FILE_TIME_LINE];
	char IsDir;
};
FileInfo convert(char* const Line,char* const ThisPath)
{
	struct FileInfo ThisLine;
	int FieldStart[9];
	int FieldEnd[9];
	int Field=0;
	char LineBak[MAX_LINE_LEN];
	strcpy(LineBak,Line);
	FieldStart[0]=0;
	for (int i=0;i<(int)strlen(Line);i++)
	{
		if (Line[i]==' '&&Line[i-1]!=' ')
		{
			FieldEnd[Field]=i-1;
			Field++;
		}
		if (Line[i]==' '&&Line[i+1]!=' ')
		{
			FieldStart[Field]=i+1;
			if (Field==8)
				break;
		}
	}
	FieldEnd[8]=strlen(Line)-1;
	strcpy(ThisLine.FileName,(LineBak+FieldStart[8]));
	ThisLine.FileName[MAX_FILE_LEN-1]=0;
	strcpy(ThisLine.PathName,ThisPath);
	ThisLine.PathName[MAX_PATH_LEN-1]=0;
	LineBak[FieldEnd[7]+1]=0;
	strcpy(ThisLine.FileTime,(LineBak+FieldStart[5]));
	LineBak[FieldEnd[4]+1]=0;
	ThisLine.FileSize=atoi(LineBak+FieldStart[4]);
	ThisLine.IsDir=LineBak[0];
	return ThisLine;
}
void make(char* const FileLine)
{
	cout<<"Now converting file "<<FileLine<<endl;
	char command[255];
	sprintf(command,"ncftp -u nk -p nk %s <ftp.txt> %s",FileLine,FileLine);
	system(command);
	ifstream FtpLine;
	FILE* OutFile;
	FtpLine.open(FileLine);
	OutFile=fopen(strcat(FileLine,".dat"),"wb");
	char Line[MAX_LINE_LEN];
	char ThisPath[MAX_PATH_LEN]="/";
	while (!FtpLine.eof())
	{
		FtpLine.getline(Line,MAX_LINE_LEN);
		if (Line[strlen(Line)-1]=='\n'||Line[strlen(Line)-1]=='\r')
			Line[strlen(Line)-1]=0;
		if (Line[strlen(Line)-1]=='\n'||Line[strlen(Line)-1]=='\r')
			Line[strlen(Line)-1]=0;
		if (Line[strlen(Line)-1]!=':'&&Line[0]!='-'&&Line[0]!='d'&&Line[0]!='l')
			continue;
		if (Line[strlen(Line)-1]==':')
		{
			Line[MAX_PATH_LEN]=0;
			if (Line[0]=='/')
				strcpy(ThisPath,Line);
			else
			{
				strcpy(ThisPath,"/");
				strcat(ThisPath,Line);
			}
			ThisPath[strlen(ThisPath)-1]='/';
		}
		else
		{
			FileInfo record;
			record=convert(Line,ThisPath);
			fwrite(&record,sizeof(record),1,OutFile);
		}
	}
	fclose(OutFile);
	FtpLine.close();
}
void search(char* FileLine,char* keyword)
{
	FILE* SearchFile;
	char FileName[MAX_FILE_LEN];
	strcpy(FileName,FileLine);
	strcat(FileName,".dat");
	SearchFile=fopen(FileName,"rb");
	FileInfo record;
	while(fread(&record,sizeof(record),1,SearchFile)!=0)
	{
		if (strstr(record.FileName,keyword))
		{
			cout<<"ftp://"<<FileLine<<record.PathName<<record.FileName<<endl;
			cout<<"["<<(record.IsDir=='d'?"目录：":"文件：")<<record.FileSize<<" byte  时间："<<record.FileTime<<"]"<<endl;
		}
	}
	fclose(SearchFile);
}
