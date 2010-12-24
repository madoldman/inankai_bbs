/*$Id: bbssnd.c,v 1.3 2010-07-01 10:13:47 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"
//struct file_to_appendix  a;
int main()
{
        FILE *fp;
        char filename[80], dir[80], board[80], title[80], buf[80], *content, vfrom[80];
	char tmpbuf [5], title2 [10];
        char redirfile[80];
        int r, i=0, length=0, sig, mode=0, accessed=0;
        struct fileheader x;
        struct shortfile *brd;
        init_all();
        if(!loginok)
                http_fatal("匆匆过客不能发表文章，请先登录");


        mode=atoi(getparm("mode"));
        if(mode)
                strsncpy(redirfile, getparm("redir"), 20);
        strsncpy(board, getparm("board"), 18);
        strsncpy(vfrom, getparm("vfrom"), 18);
		if (!*vfrom)
		{
			strcpy(vfrom, board);
		}

        if(dashf("NOPOST")&&!HAS_PERM(PERM_OBOARDS)
   		                && insec(1,board) 
          )
                http_fatal("对不起，系统进入只读状态，暂停发表文章.你先四处看看吧");

        /* efan: very faint	*/
        if (dashf ("NOPOST.2") && ! HAS_PERM (PERM_OBOARDS) && (
                                !strcmp (board, "VoiceofNankai") ||
                                !strcmp (board, "Military") ||
                                !strcmp (board, "TaiWan") ||
                                !strcmp (board, "Salon"))
           )
                http_fatal("对不起，本板进入只读状态，暂停发表文章");



        if(!(currentuser.userlevel & PERM_POST)&&strcmp(board, "appeal")&&strcmp(board, "sysop"))
                http_fatal("您尚未通过注册，暂无发表文章权限");

        if(check_post_limit(board)) return 0;

        strsncpy(title, getparm("title"), 50);
	strsncpy (tmpbuf, getparm ("title2"), 5);
	if (tmpbuf [0] != 0)
		sprintf (title2, "【%s】", tmpbuf);
	else
		title2[0] = 0;

	content=getparm("text");
        brd=getbcache(board);
        /* add by wzk for outgo post */
        //out=strlen(getparm("out"));
        //if (!(brd->flag & OUT_FLAG)) out=0;
        /* end */
        if(brd==0)
                http_fatal("错误的讨论区名称");
        strcpy(board, brd->filename);
	//huangxu@070217:no_word_filter flag
	int bd=0; 
        char temp[80];
        if(!(brd->flag2&NOFILTER_FLAG)&&(bad_words(title) || bad_words (tmpbuf) || bad_words(content) )){
                bd=1;		
                printf("您的文章可能包含不便显示的内容，已经提交审核。请返回并进行修改。");
   	        sprintf(temp,"TEMP-%s",board);
                strcpy(board,"TEMP");
        }else
                strcpy(temp,board);

        for(i=0; i<strlen(title); i++)
                if(title[i]<=27 && title[i]>=-1)
                        title[i]=' ';
        sig=atoi(getparm("signature"));

        if(title[0]==0)
                http_fatal("文章必须要有标题");
	if (title2 [0] == 0 && !strcmp (board, "Secondhand"))
		http_fatal ("在Secondhand板请说清楚您的需要!");
	if (tmpbuf [0] != ' ') {
		char tmptmpbuf [80];
		sprintf (tmptmpbuf, "%s%s", title2, title);
		strcpy (title, tmptmpbuf);
	}
        sprintf(dir, "boards/%s/.DIR", board);
        if(bd==0) 
      	  if(!has_post_perm(&currentuser, board))
             	   http_fatal("此讨论区是唯读的, 或是您尚无权限在此发表文章.");
        
	sprintf(filename, "boards/%s/deny_users", board);
        if(file_has_word(filename, currentuser.userid))
                http_fatal("很抱歉, 你被板务人员停止了本板的post权利.");
        if(abs(time(0) - *(int*)(u_info->from+36))<6) {
                *(int*)(u_info->from+36)=time(0);
                http_fatal("两次发文间隔过密, 请休息几秒后再试");
        }
        *(int*)(u_info->from+36)=time(0);
        sprintf(filename, "tmp/%d.%s.tmp", getpid(),currentuser.userid);
        f_append(filename, content);
        accessed=0;
        if(strlen(getparm("noreply"))>0)
                accessed=FILE_NOREPLY;

        int outgo=0;
        if(strlen(getparm("outgo"))>0)     
            outgo=1;

        int attach=atoi(getparm("attach"));
      
        r=0;
        r=post_article2(temp, title, filename, currentuser.userid, currentuser.username, fromhost, sig-1, accessed, attach,outgo);
        if(r<=0)
                http_fatal("内部错误，无法发文");
        char path[STRLEN];
  /*      while( appendix != NULL ) {
                bzero(&a,sizeof(struct appendix));
                strcpy(a.appendixname,appendix);
                a.filename=r;
                a.sign=0;
                strcpy(a.path,board);
                sprintf(path,"%s/file_appendix",UPLOAD_PATH);
                append_record(&a,sizeof(struct appendix),path);
                appendix = strtok( NULL, "," );
        }
*/
        sprintf(buf,"M.%d.A",r);
        brc_init(currentuser.userid, board);
        brc_add_read(buf);
        brc_update(currentuser.userid, board);
        unlink(filename);
        sprintf(buf, "posted WWW '%s' on '%s'", title, board);
        report(buf);
        if(mode==0)
                sprintf(buf, "bbsdoc?board=%s#bottom", vfrom);
        else if(mode==1)
                sprintf(buf, "bbstcon?board=%s&file=%s&vfrom=%s#bottom", board, redirfile, vfrom);
        else if(mode==2)
                sprintf(buf, "bbstdoc?board=%s#bottom", vfrom);
        if(!junkboard(board)) 
	{
                currentuser.numposts++;
                save_user_data(&currentuser);
               // write_posts(currentuser.userid, board, title);
        }
	 if(toptenboard(board))
        {
                 write_posts(currentuser.userid, board, title);
        }

	else if(!strcmp("Blessing",board)) write_posts(currentuser.userid, board, title);
        //  return 0;
        if(bd==0)        redirect(buf);

}

int write_posts(char *id, char *board, char *title)
{
        FILE *fp;
        struct post_log x;
        strcpy(x.author, id);
        strcpy(x.board, board);
        if(!strncmp(title, "Re: ", 4))
                title+=4;
        strsncpy(x.title, title, 61);
        if(title[0]==0)
                return;
        if(if_exist_title(title))
                return;
        x.date=time(0);
        x.number=1;
	//fp=fopen(".post", "a");
	if(strcmp("Blessing",board))
 	       fp=fopen(".post", "a");
	else 
		fp=fopen(".bless", "a");//loveni:十大祝福统计
        fwrite(&x, sizeof(struct post_log), 1, fp);
        fclose(fp);
}

int if_exist_title(char *title)
{
        static struct {
                int hash_ip;
                char title[64][60];
        }
        my_posts;
        char buf1[256];
        int n;
        FILE *fp;
        sethomefile(buf1, currentuser.userid, "my_posts");
        fp=fopen(buf1, "r+");
        if(fp==NULL)
                fp=fopen(buf1, "w+");
        fread(&my_posts, sizeof(my_posts), 1, fp);
        for(n=0; n<64; n++)
                if(!strncmp(my_posts.title[n], title, 50)) {
                        fclose(fp);
                        return 1;
                };
        my_posts.hash_ip = (my_posts.hash_ip+1) & 63;
        strncpy(my_posts.title[my_posts.hash_ip], title, 50);
        fseek(fp, 0, SEEK_SET);
        fwrite(&my_posts, sizeof(my_posts), 1, fp);
        fclose(fp);
        return 0;
}

