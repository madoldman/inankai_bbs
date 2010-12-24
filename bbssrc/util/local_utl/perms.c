#include <stdio.h>
#include "bbs.h"

struct perm_st
{
	unsigned int perm;
	char* perm_name;
};
struct perm_st perms[] = 
{
	{PERM_SYSOP, 		"ϵͳά������Ա"},
	{PERM_ACCOUNTS,		"�ʺŹ���Ա"},
	{PERM_OBOARDS,		"�������ܹ�"},
	{PERM_DENYPOST,		"�޸Ļ���Ȩ��"},
	{PERM_ANNOUNCE,		"�������ܹ�"},
	{PERM_ACBOARD,		"������ܹ�"},
	{PERM_WELCOME,		"�༭��վ����"},
	{PERM_OVOTE,		"ͶƱ����Ա"},
	{PERM_ACHATROOM,	"�����ҹ���Ա"},
	{PERM_CHATCLOAK,	"��վ������"},
	{PERM_XEMPT,		"�ʺ����ñ���"},
	{PERM_CLOAK,		"������"},
	{PERM_SEECLOAK,		"����������"},
	{PERM_FORCEPAGE,	"ǿ�ƺ���"},
	{PERM_EXT_IDLE, 	"�ӳ�����ʱ��"},
	{PERM_MAILINFINITY,	"����������"},
	{PERM_SEEUSERIP,	"���û�IP"},
	{PERM_LARGEMAIL,	"������"}
};

int perm_count(int id)
{
	FILE * fp;
	char path[STRLEN];
	struct userec ur;
	int count = 0;
	sprintf(path, "%s/%s", BBSHOME, PASSFILE);
	if((fp = fopen(path, "r")) == NULL)	
		return -1;
	while( fread(&ur, sizeof(struct userec), 1, fp) > 0)
	{
		if(ur.userlevel & perms[id].perm)
			count ++;
	}
	fclose(fp);
	return count;
}

void print_perm(int id)
{
	FILE * fp;
    char path[STRLEN];
    struct userec ur;
    int count = 0;
    sprintf(path, "%s/%s", BBSHOME, PASSFILE);
	printf("\n��\033[1;33m%d\033[0m������\033[1;35m%s\033[0mȨ�޵�id\n\n",
			perm_count(id), perms[id].perm_name);
    if((fp = fopen(path, "r")) == NULL)
        return;
	while( fread(&ur, sizeof(struct userec), 1, fp) > 0)
	{
		if(ur.userlevel & perms[id].perm){
			if(perms[id].perm == PERM_SYSOP)
				printf("\033[1;31m��\033[1;32m %s \033[1;31m��\033[0m ", ur.userid);
			else
				printf("%-12s",ur.userid);
			count ++;
			if(count % 4 == 0)
				printf("\n");
			else
				printf("\t");
		}
	}
	printf("\n");
	fclose(fp);
}

int main()
{
	int i;
	printf("\t\t\t\t\t\033[1;35m%s\033[0m ����Ȩ��ͳ��\n", BBSNAME);
	for(i = 0; i < 18; i ++)
		print_perm(i);
	return 0;
}
