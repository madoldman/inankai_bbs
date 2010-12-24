/*$Id: bbsccc.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        struct fileheader *x;
        char board[80], *board1, file[80], target[80], boardsearch[80];
        int i,total=0;
        init_all();
        strsncpy(board, getparm("board"), 30);
        strsncpy(file, getparm("file"), 30);
        strsncpy(target, getparm("target"), 30);
        if(!loginok)
	{
                http_fatal("´Ò´Ò¹ı¿Í²»ÄÜ½øĞĞ±¾Ïî²Ù×÷");
	}
	if(strcmp(board,"Announce")==0&&!HAS_PERM(PERM_SYSOP)&&!HAS_PERM(PERM_ADMINMENU))
	{
		http_fatal("·ÇÕ¾ÎñÈËÔ±²»ÄÜ´ÓAnnounce°å×ªÔØÎÄÕÂ");
	}
        if(strcmp(target,"")) {
                for(i=0; i<MAXBOARD; i++) {
                        board1=shm_bcache->bcache[i].filename;
                        if(!has_read_perm(&currentuser, board1))
                                continue;
                        if(!strcasecmp(board1, target)) {
                                total++;
                                strcpy(boardsearch,board1);
                        }
                }
                if (total!=1)
                        http_fatal("´íÎóµÄÌÖÂÛÇø");
                else
                        strcpy (target,boardsearch);
        }
        if(!has_read_perm(&currentuser, board)) {
                printf("%s",board);
                http_fatal("´íÎóµÄÌÖÂÛÇø2");
        }

        x=get_file_ent(board, file);
        if(x==0)
                http_fatal("´íÎóµÄÎÄ¼şÃû");

        if(check_post_limit(board)) return 0;

        printf("<center>%s -- ×ªÔØÎÄÕÂ [Ê¹ÓÃÕß: %s]<hr color=green></center>\n", BBSNAME, currentuser.userid);
        if(target[0]) {
                if(!has_post_perm(&currentuser, target))
                        http_fatal("´íÎóµÄÌÖÂÛÇøÃû³Æ»òÄãÃ»ÓĞÔÚ¸Ã°æ·¢ÎÄµÄÈ¨ÏŞ");
                if(fopen("NOPOST","r")!=NULL&&!HAS_PERM(PERM_OBOARDS))
                        http_fatal("¶Ô²»Æğ£¬ÏµÍ³½øÈëÖ»¶Á×´Ì¬£¬ÔİÍ£×ªÔØÎÄÕÂ");

                return do_ccc(x, board, target);
        }
        printf("<table><tr><td>\n");
        printf("<font color=red>×ªÌù·¢ÎÄ×¢ÒâÊÂÏî:<br>\n");
        printf("±¾Õ¾¹æ¶¨Í¬ÑùÄÚÈİµÄÎÄÕÂÑÏ½ûÔÚ 5 ¸öÒÔÉÏÌÖÂÛÇøÄÚÖØ¸´·¢±í¡£");
        printf("Î¥Õß½«±»·â½ûÔÚ±¾Õ¾·¢ÎÄµÄÈ¨Àû<br><br></font>\n");
        printf("ÎÄÕÂ±êÌâ: %s<br>\n", nohtml(x->title));
        printf("ÎÄÕÂ×÷Õß: %s<br>\n", x->owner);
        printf("Ô­ÌÖÂÛÇø: %s<br>\n", board);
        printf("<form action=bbsccc method=post>\n");
        printf("<input type=hidden name=board value=%s>", board);
        printf("<input type=hidden name=file value=%s>", file);
        printf("×ªÔØµ½ <input name=target size=30 maxlength=30> ÌÖÂÛÇø. ");
        printf("<input type=submit value=È·¶¨></form></td></tr></table>");
}

int do_ccc(struct fileheader *x, char *board, char *board2)
{
        FILE *fp, *fp2;
        char title[512], buf[512], path[200], path2[200], i;
        sprintf(path, "boards/%s/%s", board, x->filename);
        fp=fopen(path, "r");
        if(fp==0)
                http_fatal("ÎÄ¼şÄÚÈİÒÑ¶ªÊ§, ÎŞ·¨×ªÔØ");
        sprintf(path2, "tmp/%d.tmp", getpid());
        fp2=fopen(path2, "w");
        fprintf(fp2, "\n[37;1m¡¾ ÒÔÏÂÎÄ×Ö×ªÔØ×Ô [32m%s [37mÌÖÂÛÇø ¡¿\n", board);
        while(1)
        {
                if(fgets(buf, 256, fp)==0)
                        break;
                fprintf(fp2, "%s", buf);
        }
        fclose(fp);
        fclose(fp2);
        if(!strncmp(x->title, "[×ªÔØ]", 6))
        {
                sprintf(title, x->title);
        } else
        {
                sprintf(title, "%.55s(×ªÔØ)", x->title);
        }
        post_article(board2, title, path2, currentuser.userid, currentuser.username, fromhost, -1);
        unlink(path2);
        printf("'%s' ÒÑ×ªÌùµ½ %s °å.<br>\n", nohtml(title), board2);
        printf("<a href='javascript:history.go(-2)'>       ·µ»Ø        </a>");
}
