/*$Id: bbs2an.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"
/*改进措施:应该在传入文件的同时,传入文件的序号.
*/

int main()
{
        int i, total=0, mode;
        char board[80], *ptr, buf[80];
        struct shortfile *brd;
        init_all();
        if(!loginok)
                http_fatal("请先登录");
        strsncpy(board, getparm("board"), 60);
        if(!(brd=getbcache(board)))http_fatal("错误的讨论区");
        strcpy(board, brd->filename);
        digest(board);
        printf("<a href='javascript:history.go(-1);'>操作完成,返回</a>");
        http_quit();
}

int digest(char * board)
{
        char buf[PATHLEN],path[PATHLEN],cmd[PATHLEN+STRLEN];
        char file[80],title[80];
        FILE *fp;
        strsncpy(file,getparm("file"),60);
        int currboard=0;
        sethomefile(buf, currentuser.userid,".announcepath");
        if((fp = fopen(buf, "r")) == NULL ) {
                showinfo("您还没有设定丝路,请设定丝路");
                return 1;
        }
        fscanf(fp,"%s",path);
        fclose(fp);
        if(!file_exist(path)) {
                printf("无效的路径:%s.丝路已丢失,请重新设定.",path);
                return 1;
        }
        sprintf(buf,"/%s",board);
        if(strstr(path,buf)&&has_BM_perm(&currentuser, board))currboard=1;
        char dir[STRLEN],filepath[STRLEN];
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(cmd,"%s/%s",path,file);
        if(file_exist(cmd))
        {
        	printf("<br>%s 在丝路里已经存在该文件,跳过",file);
        	return 1;
        }
        sprintf(filepath, "boards/%s/%s", board, file);
        fp=fopen(dir, "r+");
        if(fp==0) {
                showinfo("错误的板面.");
                return 1;
        }
        struct fileheader f;
        int i=0;
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                i++;
                if(strcmp(f.filename, file))
                        continue;
                if((f.accessed[0] & FILE_VISIT) && currboard)
                	printf("<br>提醒:%s %s  以前被收入过精华区",file,f.title);
                if(currboard)
                {
                	f.accessed[0] |= FILE_VISIT;  // 将标志置位 
                	fseek (fp, -1 * sizeof (struct fileheader), SEEK_CUR);
                	fwrite (&f, sizeof (struct fileheader), 1, fp);
                }
                fclose (fp);
                sprintf(cmd,"%s/.Names",path);
                fp=fopen(cmd,"a+");
                if(fp==0) {
                        showinfo("不能打开.Names文件,放弃");
                        return 1;
                }
                fprintf(fp,"Name=%s\nPath=~/%s\nNumb=\n#\n",f.title,file);
                fclose(fp);
                sprintf(cmd,"cp %s %s",filepath,path);
                system(cmd);
                return 0;
        }
        fclose(fp);
        showinfo("文件不存在.\n");
        return 0;
}

