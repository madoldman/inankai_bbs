#include <stdio.h>
#include "bbs.h"

struct perm_st
{
	unsigned int perm;
	char* perm_name;
};
struct perm_st perms[] = 
{
	{PERM_SYSOP, 		"系统维护管理员"},
	{PERM_ACCOUNTS,		"帐号管理员"},
	{PERM_OBOARDS,		"讨论区总管"},
	{PERM_DENYPOST,		"修改基本权限"},
	{PERM_ANNOUNCE,		"精华区总管"},
	{PERM_ACBOARD,		"活动看板总管"},
	{PERM_WELCOME,		"编辑进站画面"},
	{PERM_OVOTE,		"投票管理员"},
	{PERM_ACHATROOM,	"聊天室管理员"},
	{PERM_CHATCLOAK,	"本站智囊团"},
	{PERM_XEMPT,		"帐号永久保留"},
	{PERM_CLOAK,		"隐身术"},
	{PERM_SEECLOAK,		"看穿隐身术"},
	{PERM_FORCEPAGE,	"强制呼叫"},
	{PERM_EXT_IDLE, 	"延长发呆时间"},
	{PERM_MAILINFINITY,	"邮箱无上限"},
	{PERM_SEEUSERIP,	"看用户IP"},
	{PERM_LARGEMAIL,	"大信箱"}
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
	printf("\n有\033[1;33m%d\033[0m个具有\033[1;35m%s\033[0m权限的id\n\n",
			perm_count(id), perms[id].perm_name);
    if((fp = fopen(path, "r")) == NULL)
        return;
	while( fread(&ur, sizeof(struct userec), 1, fp) > 0)
	{
		if(ur.userlevel & perms[id].perm){
			if(perms[id].perm == PERM_SYSOP)
				printf("\033[1;31m◆\033[1;32m %s \033[1;31m◆\033[0m ", ur.userid);
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
	printf("\t\t\t\t\t\033[1;35m%s\033[0m 特殊权限统计\n", BBSNAME);
	for(i = 0; i < 18; i ++)
		print_perm(i);
	return 0;
}
