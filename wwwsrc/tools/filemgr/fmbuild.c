#include "BBSLIB.inc"

#define FMG_SHOWDETAIL 
char conmode=0;
//FMG_SHOWDETAIL和conmode联用

#include "filemgr.inc"

struct fmg_attdir ad;
char mode=0;

int main(int argc,char**argv)
{
	html_init();
	if(!*(getsenv("HTTP_HOST")))conmode=1;
	if(!conmode&&(!(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_ANNOUNCE))))http_fatal("您无权执行此操作");
	int i;
	if(argc>1&&!strcasecmp(argv[1],"clean"))
	{
		printf("Clean mode.%s",conmode?"\n":"<br>");
		mode=1;
	}
	struct shortfile *x;
	if(*(getparm("board")))
	{
		if(!(x=getbcache(getparm("board"))))http_fatal("找不到板面");
		printf("Buiding index of %s/%s...%s",x->filename,x->title+11,conmode?"\n":"<br>");
		BuildAttIndex(x->filename);
	}
	else
		for(i=0; i<MAXBOARD; i++)
		{
			if(!(x=&(shm_bcache->bcache[i])))continue;
			if(x->filename[0]<=32||x->filename[0]>'z')continue;
			if(x->flag&ZONE_FLAG)continue;
			printf("Buiding index of %s/%s...%s",x->filename,x->title+11,conmode?"\n":"<br>");
			BuildAttIndex(x->filename);
		}
	printf("done.%s",conmode?"\n":"<br>");
	if(conmode)return 0;else http_quit();
}

int BuildAttIndex(char * board)
{
	char homepath[128];
	int i;
	char buf[128];
	strtolower(buf,board);
	sprintf(homepath,"%s/f/%s",UPLOAD_PATH,buf);
	sprintf(buf,"boards/%s/.attdir",board);
	if(mode==1)unlink(buf);
	ad=fmg_openattdir(board,0);
	if(!(ad.fp))return 0;
	BuildAttIndex__rec(homepath);
	fmg_checkexist(&ad);
  fmg_sortrec(&ad);
	fmg_closeattdir(&ad);
	return 1;
}

int BuildAttIndex__rec(char * path)
{
	int i=0;
	DIR *dirp;
	struct dirent *direntp;
	char buf[256];
	printf("  %s%s",path,conmode?"\n":"<br>");
	if(!(dirp=opendir(path)))return 0;
	while(direntp=readdir(dirp))
	{
		if(!strcmp(direntp->d_name,".")||!strcmp(direntp->d_name,".."))continue;
		sprintf(buf,"%s/%s",path,direntp->d_name);
  	if(file_isfile(buf))fmg_addrec_n(buf,"Unknown.",ad.fh.defaultlife,&ad);
  	else if(file_isdir(buf))BuildAttIndex__rec(buf);
  }
	closedir(dirp);
	return i;
}

