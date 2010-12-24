/* $Id: telnetview.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"
#include "NEWLIB.inc"

void jsprint(char * );

int no_re=0;
/* 	telnetview?board=xx&file=xx&start=xx 	*/
char bufnav[2048];
int start;
int total;
int ads=0;
int main()
{
        FILE *fp;
        char userid[80], buf[512], board[80], dir[80], file[512], filename[80];
        char ch, tmp[80], *ptr;
        struct fileheader x;
        int i, found=0;
        init_all();
        strsncpy(board, getparm("board"), 32);
        strsncpy(file, getparm("file"), 32);
        //printf("<head>我爱南开站-Telnet样式</head>");
        printf("<script language='javascript' src='/js/telnetview.js'></script>");//Js支持
        printf("<body bgproperties=fixed>",mytheme.bgpath);
        if (*file!='/') //帖子
        {
	        modify_mode(u_info,READING);	//bluetent
	        start=atoi(getparm("num"));
	        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
	                http_fatal("错误的讨论区");
	        if (!canenterboard (currentuser.userid, board) && !HAS_PERM(PERM_SYSOP))
	                http_fatal ("您已经被取消进入本板的权限");
	        strcpy(board, getbcache(board)->filename);
	        if(loginok)
	                brc_init(currentuser.userid, board);
	        sprintf(dir, "boards/%s/.DIR", board);
	        total=file_size(dir)/sizeof(struct fileheader);
	        if(start>=total)
	                start=total-1;
	        if(start<0)
	                start=0;
	        fp=fopen(dir, "r+");
	        if(fp==0)
	                http_fatal("目录错误");
	          if(start==0){
	                   while(1) {
	                       if(fread(&x, sizeof(x), 1, fp)<=0){
				   printf("文章不存在!");
	                           break;
	                       }
	                       if(!strcmp(x.filename, file)) {
	                            show_file(board,&x);
	                            break;
	                       }
	                   }
	                   fcloseall();
	                   return 0;
	                }
	
	                fseek(fp,sizeof(struct fileheader)*start,SEEK_SET);
	                if(fread(&x, sizeof(x), 1, fp)<=0)
	                        http_fatal("读取错误.");
	                show_file(board,&x);
	                fcloseall();
	                return 0;
	      }//帖子结束
	      else {//精华区
	        FILE *fp;
	        modify_mode(u_info,DIGEST+20000);	//bluetent
	        strsncpy(file, getparm("file"), 511);
	        if(strstr(file, ".Search") || strstr(file, ".Names")|| strstr(file, "..")|| strstr(file, "SYSHome"))
	                http_fatal("错误的文件名");
	        sprintf(buf, "0Announce%s", file);
	        fp=fopen(buf, "r");
	        if(fp==0)
	                http_fatal("错误的文件名");
	printf("<table><tr><td id=\"c\" bgcolor=\"black\" style=\"font-size:16px;line-height:1\"></td></tr></table>");
	printf("<script language=\"javascript\">\n\r	init();\n\r");
	printf("document.getElementById('c').innerHTML=previewthis('");
	
	        while(1) {
	                if(fgets(buf, 256, fp)==0)
	                        break;
	                jsprint(void1(buf));
	        }
	printf("');\n\r</script>");
	        fclose(fp);
	        //check_msg();//bluetent 2002.10.31
	    	}
 
        http_quit();
}
int show_file(char *board, struct fileheader *x)
{
        char title[STRLEN];
        char *ptr=x->title;
        if(!strncmp(ptr, "Re:", 3))
          ptr+=4;
        strsncpy(title, ptr, 60);

        brc_init(currentuser.userid, board);
        brc_add_read(x->filename);
        brc_update(currentuser.userid, board);
        //printf("ok");return 0;
        FILE *fp;
        char path[80], buf[512];
        if(loginok)
                brc_add_read(x->filename);
        sprintf(path, "boards/%s/%s", board, x->filename);
        fp=fopen(path, "r");
        if(fp==0)
        {
		http_fatal("文章内容已被删除<br>\n");
		   return;
	}
        printf("<p>");

        if(fgets(buf, 256, fp)==0)
        {
                printf("读取文件出错!");
                fclose(fp);
                return 0;
        }
        char *id,*s;

        int w=80;
        if(ads){
           w=100;
        }
        int con=1,qmd=0,cite=0;
	printf("<table><tr><td id=\"c\" bgcolor=\"black\" style=\"font-size:16px;line-height:1\"></td></tr></table>");
	printf("<script language=\"javascript\">\n\r	init();\n\r");
	printf("document.getElementById('c').innerHTML=previewthis('");
				fseek (fp,0,0);
        while(1)
        {
                if(fgets(buf, 500, fp)==0)
                        break;
                jsprint(buf);
        }
	printf("');\n\r</script>");
        fclose(fp);
        if(ads) googleads();
        return 1;
}

