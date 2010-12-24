/*$Id: bbsedit.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"

struct shortfile * brd;

int main()
{
        FILE *fp;
        int type=0, i, num, l;
        char userid[80], buf[512], path[512], file[512], board[512], title[80]="";
        //struct shortfile *brd;
        struct fileheader  *x;
        char c;//huangxu@060331
        init_all();
        modify_mode(u_info,EDIT+20000);	//bluetent
        if(!loginok)
                http_fatal("´Ò´Ò¹ı¿Í²»ÄÜĞŞ¸ÄÎÄÕÂ£¬ÇëÏÈµÇÂ¼");
        strsncpy(board, getparm("board"), 20);
        type=atoi(getparm("type"));
        brd=getbcache(board);
        if(brd==0)
                http_fatal("´íÎóµÄÌÖÂÛÇø!");
        strcpy(board, brd->filename);
        strsncpy(file, getparm("file"), 20);
        if(!has_post_perm(&currentuser, board))
                http_fatal("´íÎóµÄÌÖÂÛÇø»òÕßÄúÎŞÈ¨ÔÚ´ËÌÖÂÛÇø·¢±íÎÄÕÂ");
        x=get_file_ent(board, file);
        if(strstr(file, "..") || strstr(file, "/"))
                http_fatal("´íÎóµÄ²ÎÊı");
        if(x==0)
                http_fatal("´íÎóµÄ²ÎÊı");
        if(strcmp(x->owner, currentuser.userid))
                if(!has_BM_perm(&currentuser, board))
                        http_fatal("ÄãÎŞÈ¨ĞŞ¸Ä´ËÎÄÕÂ");

        printf("<center>%s -- ĞŞ¸ÄÎÄÕÂ [Ê¹ÓÃÕß: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        if(type!=0)
                return update_form(board, file);
        printf("<table border=1>\n");
        printf("<tr><td>");
        printf("<tr><td><form method=post action=bbsedit>\n");
        hsprintf(buf, "%s", x->title);
        printf("Ê¹ÓÃ±êÌâ: %s ÌÖÂÛÇø: %s<br>\n", buf, board);
        printf("±¾ÎÄ×÷Õß£º%s<br>\n", currentuser.userid);
        sprintf(path, "boards/%s/%s", board, file);
        fp=fopen(path, "r");
        if(fp==0)
                http_fatal("ÎÄ¼ş¶ªÊ§");
        fseek(fp,0,2);
        l=ftell(fp);
        fseek(fp,0,0);
        for (i=4;l>0&&i>0;l--)//huangxu@060331
        {
        	c=fgetc(fp);
       		if (c==0x0a)
        		i--;
        }
        if (l<=0)  http_fatal("´íÎóµÄÎÄ¼ş");

        printf("<textarea name=text rows=20 cols=80 wrap=physicle>");
        while(1) {
                if(fgets(buf, 500, fp)==0)
                        break;
                        //ÎªÊ²Ã´ÕâĞ©´úÂëÎŞ·¨¹¤×÷£¿
                /*for (;i>0;i--)
                {
                	printf("CHAR::%d::",*(buf+44));
                	pbuf++;
                	pbuf=strchr(pbuf,0x0a);
                	if (!pbuf) 
                	{
                		printf("NOTFOUND%d",i);
                		break;
                		}
                		printf("FOUND:%d,Length:%d,str=%s::",pbuf-buf,strlen(buf),buf);
                }*/
                if(!strcasestr(buf, "</textarea>"))
                        printf("%s", buf);
        }
        fclose(fp);
        printf("</textarea>\n");
        printf("<tr><td class=post align=center>\n");
        printf("<input type=hidden name=type value=1>\n");
        printf("<input type=hidden name=board value=%s>\n", board);
        printf("<input type=hidden name=file value=%s>\n", file);
        printf("<input type=submit value=´æÅÌ> \n");
        printf("<input type=button value=·ÅÆú±à¼­ onclick=javascript:history.go(-1)></form>\n");
        printf("</table>");
        http_quit();
}

int update_form(char *board, char *file)
{
        FILE *fp;
        char *buf=getparm("text"), path[80];
        int i,l;//huangxu@060331
        //ÕâÀïÎªÁË°²È«
        char * pbuf,c;
        char rbuf[512];
        if(!(brd->flag2&NOFILTER_FLAG)&&(bad_words(buf)))
	{
                printf("<script language=\"javascript\">alert('ÄúµÄÎÄÕÂ¿ÉÄÜ°üº¬²»±ãÏÔÊ¾µÄÄÚÈİ£¬ÒÑ¾­Ìá½»ÉóºË¡£Çë·µ»Ø²¢½øĞĞĞŞ¸Ä¡£');history.go(-1);</script>");
		return 1;
	}
        sprintf(path, "boards/%s/%s", board, file);
        if (!(fp=fopen(path,"r")))
        	http_fatal("´íÎóµÄÎÄ¼ş!");
        fseek(fp,0,2);
        l=ftell(fp);
        fseek(fp,0,0);
        pbuf=rbuf;
        for (i=4;l>0&&i>0;l--)//huangxu@060331
        {
        	*(pbuf++)=c=fgetc(fp);
       		if (c==0x0a)
        		i--;
        }
        fclose(fp);
        if (l<=0)  http_fatal("´íÎóµÄÎÄ¼ş!");
        fp=fopen(path, "w");
        if(fp==0)
                http_fatal("ÎŞ·¨´æÅÌ");
       	fwrite(rbuf,1,pbuf-rbuf,fp);
        fprintf(fp, "%s", buf);
        //huangxu@060331
				fprintf(fp,
				        "\n[1;36m¡ù ĞŞ¸Ä:¡¤%s ÓÚ %16.16s ĞŞ¸Ä±¾ÎÄ¡¤Web[FROM: %-.20s][m\n",
				        currentuser.userid, Ctime(time(0))+ 4, fromhost);
        //fprintf(fp, "\n¡ù ĞŞ¸Ä:¡¤%s ì¶ %s ĞŞ¸Ä±¾ÎÄ¡¤%s Web[FROM: %s] ", currentuser.userid, Ctime(time(0))+4,BBSHOST, (!(currentuser.userdefine & DEF_NOTHIDEIP)&&isdormip(fromhost))?"ÄÏ¿ªËŞÉáÄÚÍø":fromhost);
        fclose(fp);
        printf("ĞŞ¸ÄÎÄÕÂ³É¹¦.<br><a href=bbsdoc?board=%s>·µ»Ø±¾ÌÖÂÛÇø</a>", board);
}
