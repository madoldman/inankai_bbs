/*$Id: delfiles.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp,*fp2;
        struct shortfile *brd;
        struct fileheader f;
        struct userec *u;
        int num;
        char buf[80], dir[80], path[80], board[80], file[80], *id;
	char jdir[80];
        init_all();
        if (!(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_DENYPOST)&&HAS_PERM(PERM_OBOARDS)))
        	http_fatal("对不起，您无权执行此操作！");
        id=currentuser.userid;
        int count,c2;
        int i;
        char * pctmp;
        count=atoi(getparm("total"));
        c2=0;
        for (i=0;i<count;i++)
        {
        	sprintf(buf,"chk%d",i);
        	strsncpy(buf,getparm(buf),79);
        	if (!*buf) continue;
        	pctmp=strstr(buf,"板");
        	*pctmp=0;
        	strcpy(board,buf);
        	strcpy(file,pctmp+2);
        num=0;
        if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2)||(strstr(file, "..")))
        {
        	printf("<p>%d : 尝试删除%s板%s…… 失败（安全性保护）。</p>",i,board,file);continue;
        }
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/%s", board, file);
	sprintf(jdir, "boards/%s/.junk", board);
        fp=fopen(dir, "r");
        if(fp==0)
        {
        	printf("<p>%d : 尝试删除%s板%s…… 失败（文件无法打开）。</p>",i,board,file);continue;
        }
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                {
                	        printf("<p>%d : 尝试删除%s板%s…… 失败（文件未找到）。</p>",i,board,file);
        break;}
                if(!strcmp(f.filename, file)) {
		fp2=fopen(jdir, "a");
		fwrite(&f, sizeof(struct fileheader), 1, fp2);
		fclose(fp2);
			if(!HAS_PERM(PERM_SYSOP))
      				if(strstr(board,"syssecurity"))
        	{printf("<p>%d : 尝试删除%s板%s…… 失败（权限不够）。</p>",i,board,file);continue;}
			del_record(dir, sizeof(struct fileheader), num);
                        sprintf(buf, "\n※ %s 于 %s 删除·Web·批处理[FROM: %s]", currentuser.userid, Ctime(time(0))+4, fromhost);
                        f_append(path, buf);
                /*        if(!strcmp(f.owner, currentuser.userid)) {
                                post_article("junk", f.title, path, f.owner, "", fromhost, -1);
                        } else {
                                post_article("deleted", f.title, path, f.owner, "", fromhost, -1);
                        }*/
                        //unlink(path);
                       	printf("<p>%d : 尝试删除%s板%s…… 成功。</p>",i,board,file);
                       	c2++;
                        u=getuser(f.owner);
                        if(!junkboard(board) && u) {
                                if(u->numposts>0)
                                        u->numposts--;
                                save_user_data(u);
                        }
                        sprintf(buf, "%s %-12s bbsdel %s\n", Ctime(time(0))+4, id, board);
                        f_append("trace", buf);
                        break;
                }
                num++;
        }
        fclose(fp);
       }
       printf("<h3>成功删除%d篇文章。</h3>",c2);
       printf("<p><a href='bbsfind'>返回</a></p>");
       http_quit();
}
