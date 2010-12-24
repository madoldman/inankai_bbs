/* $Id: jbbscon.c,v 1.3 2008-12-23 09:38:35 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "NEWLIB.inc"

/*bbscon?board=xx&file=xx&junk=*/
char board[80];
char file[512];

int main()
{
	FILE *fp;
	struct fileheader x;
	int i;
	int htmlmode=0;
	char dir[80];
	char buf[600];
	int boardmode=0;
	xml_init();
	strsncpy(board, getparm("board"), 32);
	strsncpy(file, getparm("file"), 32);
	htmlmode=atoi(getparm("htmlmode"));
	if((htmlmode&257)==257)printf("<?xml version=\"1.0\" encoding=\"gb18030\" ?>\n");
	if(htmlmode&1)printf("<div>");
	if(getbcache(board))//讨论区
	{
		strcpy(board, getbcache(board)->filename);
		boardmode=atoi(getparm("boardmode"));
		if((boardmode==1)&&(!(has_BM_perm(&currentuser, board)||HAS_PERM(PERM_FORCEPAGE))))
			boardmode=0;
		if(boardmode>2||boardmode<0)boardmode=0;
		sprintf(dir, "boards/%s/%s",board,BOARDINDEX[boardmode]);
		if(!*file)return 0;
		if(!(fp=fopen(dir, "r+")))return 0;
		if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2))return 0;
		if(strstr(file, "..") || strstr(file, "/"))return 0;
		//fseek(fp,0,SEEK_SET);
		for(;;)
		{
			if(fread(&x,sizeof(x),1,fp)<=0)
				break;
			if(!strcmp(x.filename,file))
			{
				if (postReadable(&x))
				{
					sprintf(buf,"boards/%s/%s",board,x.filename);
					j_show_file(buf,htmlmode);
				}
				break;
			}
		}
	}
	else//精华区
	{
		strsncpy(file, getparm("file"), 511);
		if(strstr(file, ".Search") || strstr(file, ".Names")|| strstr(file, "..")|| strstr(file, "SYSHome"))return 0;
		sprintf(buf, "0Announce%s", file);
		j_show_file(buf,htmlmode);
	}
	if(htmlmode&1)printf("</div>");
	fcloseall();
	return 0;
}

