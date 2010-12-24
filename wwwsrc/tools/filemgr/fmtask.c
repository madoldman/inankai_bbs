#include "BBSLIB.inc"

#define FMG_SHOWDETAIL 
char conmode=0;
//FMG_SHOWDETAIL和conmode联用

#include "filemgr.inc"

struct fmg_attdir ad;

int main()
{
	html_init();
	if(!*(getsenv("HTTP_HOST")))conmode=1;
	if(!conmode&&(!(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_ANNOUNCE))))http_fatal("您无权执行此操作");
	int i;
	struct shortfile * x;
	for(i=0;i<MAXBOARD;i++)
	{
		if(!(x=&(shm_bcache->bcache[i])))continue;
		if(x->filename[0]<=32||x->filename[0]>'z')continue;
		if(x->flag&ZONE_FLAG)continue;
		printf("Delete expired files of %s/%s...%s",x->filename,x->title+11,conmode?"\n":"<br>");
		fmg_expfiles(x->filename);
	}
	return 0;
}

