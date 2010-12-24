#include "bbs.h"
#include "bbsgopher.h"

#define MAXITEMS        1024
#define PATHLEN         256
//#define A_PAGESIZE      (t_lines - 5)
#define A_PAGESIZE      (t_lines - 4)
//modefied by yiyo

#define ADDITEM         0
#define ADDGROUP        1
#define ADDMAIL         2
#define ADDGOPHER       3

int     bmonly = 0;
void    a_menu();
char	currdigestdir[256];//add by bluetent for FavDigest
extern char BoardName[];
void a_prompt();		/* added by netty */
int y_lockscreen();
typedef struct
{
        char    title[72];
        char    fname[80];
        char   *host;
        int     port;
}
ITEM;

int  a_fmode = 0;

typedef struct
{
        ITEM   *item[MAXITEMS];
        char    mtitle[STRLEN];
        char   *path;
        int     num, page, now;
        int     level;
}
MENU;

/*
 * Cache the MENUs when searching
 * Add by macana<ml.macana@gmail.com> 2005.11.14
 */
#define SEARCHDEPTH 3

void a_showmenu(MENU *pm)
{
        struct stat st;
        struct tm *pt;
        char    title[STRLEN * 2], kind[20];
        char    fname[STRLEN];
        char    ch;
        char    buf[STRLEN], genbuf[STRLEN * 2];
        time_t  mtime;
        int     n;
        // Add by wujian
        char    counter[9];
        FILE    *fp;
        sprintf(fname,"%s/counter.person",pm->path);
        fp=fopen(fname,"r");
        if(fp) {
                fscanf(fp,"%s",counter);
                fclose(fp);
        } else
                strcpy(counter,"< none >");
        ch = strlen(counter);
        for(n=ch;n>=0;n--)
                counter[n+(9-ch)/2] = counter[n];
        for(n=(9-ch)/2;n>0;n--)
                counter[n-1] = ' ';
        // Add end.
        clear();
        if (chkmail()&&(mail_unread()>0)) {
                prints("[5m");
                sprintf(genbuf, "[%d·âĞÂĞÅ, °´ M ²é¿´]", mail_unread());
        } else
                strcpy(genbuf, pm->mtitle);
        sprintf(buf, "%*s", (80 - strlen(genbuf)) / 2, "");
        prints("[1;44m%s%s%s[m\n", buf, genbuf, buf);
        prints("           [1;32m F[37m ¼Ä»Ø×Ô¼ºµÄĞÅÏä  [32m¡ü¡ı[37m ÒÆ¶¯ [32m ¡ú <Enter> [37m¶ÁÈ¡ [32m ¡û,q[37m Àë¿ª[m\n");
        prints ("[1;44;37m ±àºÅ  %-20s[32m±¾Ä¿Â¼ÒÑ±»ä¯ÀÀ[33m%-9s[32m´Î[37m Õû  Àí           %8s [m",
                "[Àà±ğ] ±ê    Ìâ",counter,a_fmode? "µµ°¸Ãû³Æ" : "±à¼­ÈÕÆÚ");
        prints("\n");
        if (pm->num == 0) {
                prints("      << Ä¿Ç°Ã»ÓĞÎÄÕÂ >>\n");
        }
        for (n = pm->page; n < pm->page + A_PAGESIZE && n < pm->num; n++) {
                strcpy(title, pm->item[n]->title);
                sprintf(fname, "%s", pm->item[n]->fname);
                sprintf(genbuf, "%s/%s", pm->path, fname);
                ch = (dashd(genbuf) ? '/':' ');
                if (a_fmode &&(pm->level&PERM_BOARDS)!=0) {
                        fname[10] = '\0';
                } else {
                        if (dashf(genbuf) || ch == '/') {
                                if(ch=='/') {
                                        sprintf(genbuf,"%s/.Names",genbuf);
                                        stat(genbuf, &st);
                                        genbuf[strlen(genbuf)-7]='\0';
                                } else
                                        stat(genbuf, &st);
                                mtime = st.st_mtime;
                        } else
                                mtime = time(0);
                        pt = localtime(&mtime);
                        sprintf(fname, "[1m%04d[m.[1m%02d[m.[1m%02d[m",
                                pt->tm_year+1900, pt->tm_mon + 1, pt->tm_mday);//by yiyoÄêÊıËÄÎ»
                        ch = ' ';
                }
                if (pm->item[n]->host != NULL) {
                        if (pm->item[n]->fname[0] == '0')
                                strcpy(kind, "[[1;32mÁ¬ÎÄ[m]");
                        else
                                strcpy(kind, "[[1;33mÁ¬Ä¿[m]");
                } else if (dashl(genbuf)) {
                        strcpy(kind,"[[1;32mÁ´½Ó[m]");

                } else if (dashf(genbuf)) {
                        strcpy(kind, "[[1;36mÎÄ¼ş[m]");
                } else if (dashd(genbuf)) {
                        strcpy(kind, "[[1;37mÄ¿Â¼[m]");
                } else {
                        strcpy(kind, "[[1;32m´íÎó[m]");
                }
                if (strstr (genbuf, ".."))
                        strcpy (kind, "[\33[1;32m·Ç·¨\33[m]");
                if (  !strncmp(title, "[Ä¿Â¼] ", 7) || !strncmp(title, "[ÎÄ¼ş] ", 7)
                                || !strncmp(title, "[Á¬Ä¿] ", 7) || !strncmp(title, "[Á¬ÎÄ] ", 7))
                        sprintf(genbuf, "%-s %-55.55s%-s%c", kind, title + 7, fname, ch);
                else
                        sprintf(genbuf, "%-s %-55.55s%-s%c", kind, title, fname, ch);
                strncpy(title, genbuf, STRLEN * 2);
                title[STRLEN * 2 - 1] = '\0';
                prints("  %3d  %s\n", n + 1, title);
        }
        clrtobot();
        move(t_lines - 1, 0);
        prints("%s", (pm->level & PERM_BOARDS) ?
               "[1;31;44m[°å  Ö÷]  [33mËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ĞÂÔöÎÄÕÂ a ©¦ ĞÂÔöÄ¿Â¼ g ©¦ ±à¼­µµ°¸ E        [m" :
               "[1;31;44m[¹¦ÄÜ¼ü] [33m ËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ÒÆ¶¯ÓÎ±ê k,¡ü,j,¡ı ©¦ ¶ÁÈ¡×ÊÁÏ Rtn,¡ú         [m");
}

void a_additem(MENU *pm, char *title, char *fname, char *host, int port)
{
        ITEM   *newitem;
        if (pm->num < MAXITEMS) {
                newitem = (ITEM *) malloc(sizeof(ITEM));
                strcpy(newitem->title, title);
                if (host != NULL) {
                        newitem->host = (char *) malloc(sizeof(char) * (strlen(host) + 1));
                        strcpy(newitem->host, host);
                } else
                        newitem->host = host;
                newitem->port = port;
                strcpy(newitem->fname, fname);
                pm->item[(pm->num)++] = newitem;
        }
}

int get_corpus_owner(char *path,char *s)
{
        char buf[16];
        if(strncmp(path,"/PersonalCorpus",15))
                return 0;
        int size=strlen(path);
        int j=1,i=0,n=0;
        for(i=15;i<size;i++) {
                if(j==3)
                        if(path[i]!='/')
                                buf[n++]=path[i];
                if(path[i]=='/')
                        j++;
                if(j==4) {
                        buf[n]=0;
                        break;
                }
        }
        if(j>=3) {
                buf[n]=0;
                strcpy(s,buf);
                return 1;
        } else
                return 0;
}

int a_loadnames(MENU *pm)
{
        FILE   *fn, *fp;
        int     counter=0;
        ITEM    litem;
        char    buf[PATHLEN], *ptr;
        char    hostname[STRLEN], board[STRLEN];
        char * bmstr;

	strcpy(board, pm->path);
	ptr = strrchr(board, '/');
	if (ptr != NULL)
	{
		/* This section is so ugly :( */
		if (strstr(board, "0Announce/abbs/") == board || strstr(board,"0Announce/groups/oldbakup") == board)
			strcpy(currboard, "sysop");
		/*if (*(ptr - 2) == '_' || *(ptr - 3) == '_')*/
		if(!(strncmp(ptr-7,"GROUP_",6)&&strncmp(ptr-8,"GROUP_",6)))
		{
			strcpy(currboard, ptr + 1);
		}
	}

        memset(buf,0,STRLEN);
        strncpy(buf, pm->mtitle,STRLEN);
        bmstr = strstr(buf+38, "(BM:");
        if (bmstr != NULL) {
                if (chk_currBM2(bmstr + 4))
                        pm->level |= PERM_BOARDS;
                else
                        if (bmonly == 1 && !HAS_PERM(PERM_BOARDS))
                                return;
        }
        pm->num = 0;
        sprintf(buf, "%s/.Names", pm->path);
        if ((fn = fopen(buf, "r")) == NULL)
                return 0;
        // Add by wujian
        sprintf(buf, "%s/counter.person",pm->path);
        fp = fopen(buf,"r");
        if(fp) {
                fscanf(fp,"%d",&counter);
                fclose(fp);
        }
        fp=fopen(buf,"w+");
        if(fp) {
                flock(fileno(fp),LOCK_EX);
                fprintf(fp,"%d",counter+1);
                flock(fileno(fp),LOCK_UN);
                fclose(fp);
        }
        // Add end.
        hostname[0] = '\0';
        char owner[16];
        int pcmode=get_corpus_owner(pm->path+9,owner);
        while (fgets(buf, sizeof(buf), fn) != NULL) {
                if ((ptr = strchr(buf, '\n')) != NULL)
                        *ptr = '\0';
                if (strncmp(buf, "Name=", 5) == 0) {
                        strncpy(litem.title, buf + 5, 72);
                        litem.title[71] = '\0';
                } else if (strncmp(buf, "Path=", 5) == 0) {
                        if (strncmp(buf, "Path=~/", 7) == 0)
                                strncpy(litem.fname, buf + 7, 80);
                        else
                                strncpy(litem.fname, buf + 5, 80);
                        litem.fname[79] = '\0';
                        if ((!strstr(litem.title+38,"(BM: BMS)")||HAS_PERM(PERM_BOARDS))&&(!strstr(litem.title+38,"(BM: SYSOPS)")||HAS_PERM(PERM_SYSOP))&& ((strstr(litem.title, "<HIDE>")!= litem.title)||(pcmode&&!strcmp(owner,currentuser.userid))||(!pcmode&&(HAS_PERM(PERM_ANNOUNCE)||(pm->level & PERM_BOARDS) || check_bBM(currboard)))))
                                a_additem(pm,litem.title,litem.fname,(strlen(hostname) == 0) ? NULL : hostname, litem.port);
                        hostname[0] = '\0';
                } else if (strncmp(buf, "# Title=", 8) == 0) {
                        //if (pm->mtitle[0] == '\0')
                        strncpy(pm->mtitle, buf + 8, STRLEN);
                } else if (strncmp(buf, "Host=", 5) == 0) {
                        strncpy(hostname, buf + 5, STRLEN);
                } else if (strncmp(buf, "Port=", 5) == 0) {
                        litem.port = atoi(buf + 5);
                }
        }
        fclose(fn);
        return 1;
}

void a_savenames( MENU  *pm )
{
        FILE   *fn;
        ITEM   *item;
        char    fpath[PATHLEN];
        int     n;

        sprintf(fpath, "%s/.Names", pm->path);
        if ((fn = fopen(fpath, "w")) == NULL)
                return;
        fprintf(fn, "#\n");
        if (!strncmp(pm->mtitle,"[Ä¿Â¼] ",7) || !strncmp(pm->mtitle,"[ÎÄ¼ş] ",7)
                        || !strncmp(pm->mtitle,"[Á¬Ä¿] ",7) || !strncmp(pm->mtitle,"[Á¬ÎÄ] ",7)) {
                fprintf(fn, "# Title=%s\n", pm->mtitle + 7);
        } else {
                fprintf(fn, "# Title=%s\n", pm->mtitle);
        }
        fprintf(fn, "#\n");
        for (n = 0; n < pm->num; n++) {
                item = pm->item[n];
                if(!strncmp(item->title,"[Ä¿Â¼] ",7)||!strncmp(item->title,"[ÎÄ¼ş] ",7)
                                ||!strncmp(item->title,"[Á¬Ä¿] ",7)||!strncmp(item->title,"[Á¬ÎÄ] ",7)) {
                        fprintf(fn, "Name=%s\n", item->title + 7);
                } else
                        fprintf(fn, "Name=%s\n", item->title);
                if (item->host != NULL) {
                        fprintf(fn, "Host=%s\n", item->host);
                        fprintf(fn, "Port=%d\n", item->port);
                        fprintf(fn, "Type=1\n");
                        fprintf(fn, "Path=%s\n", item->fname);
                } else
                        fprintf(fn, "Path=~/%s\n", item->fname);
                fprintf(fn, "Numb=%d\n", n + 1);
                fprintf(fn, "#\n");
        }
        fclose(fn);
        chmod(fpath, 0644);
}

void a_prompt(int bot,char* pmt,char* buf,int len)
{
        saveline(t_lines + bot, 0);
        move(t_lines + bot, 0);
        clrtoeol();
        getdata(t_lines + bot, 0, pmt, buf, len, DOECHO, YEA);
        saveline(t_lines + bot, 1);
}

/* added by netty to handle post saving into (0)Announce */
int a_Save(char *path, char* key, struct fileheader * fileinfo, int nomsg)
{

        char    board[40];
        int     ans = NA;
        if (!nomsg) {
                sprintf(genbuf, "È·¶¨½« [%-.40s] ´æÈëÔİ´æµµÂğ", fileinfo->title);
                if (askyn(genbuf, NA, YEA) == NA)
                        return FULLUPDATE;
        }
        sprintf(board, "bm/%s", currentuser.userid);
        if (dashf(board)) {
                if (nomsg)
                        ans = YEA;
                else
                        ans = askyn("Òª¸½¼ÓÔÚ¾ÉÔİ´æµµÖ®ááÂğ", NA, YEA);
        }
        if (in_mail)
                sprintf(genbuf, "/bin/%s mail/%c/%s/%s %s %s", (ans) ? "cat" : "cp -r",
                        toupper(currentuser.userid[0]), currentuser.userid,
                        fileinfo->filename, (ans) ? ">>" : "", board);
        else
                sprintf(genbuf, "/bin/%s boards/%s/%s %s %s", (ans) ? "cat" : "cp -r",
                        key, fileinfo->filename, (ans) ? ">>" : "", board);
        system(genbuf);
        if (!nomsg)
                presskeyfor("ÒÑ½«¸ÃÎÄÕÂ´æÈëÔİ´æµµ, Çë°´<Enter>¼ÌĞø...",t_lines-1);
        return FULLUPDATE;
}
/*
int
a_Save(path, key, fileinfo, nomsg, full)
char   *path, *key;
struct fileheader *fileinfo;
int     nomsg, full;
{
 
   char    board[40];
   int     ans = NA;
   if (!nomsg) {
      sprintf(genbuf, "È·¶¨½« [%-.40s] ´æÈëÔİ´æµµÂğ", fileinfo->title);
      if (askyn(genbuf, NA, YEA) == NA)
         return FULLUPDATE;
   }
   sprintf(board, "bm/%s", currentuser.userid);
   if (dashf(board)) {
      if (nomsg) ans = YEA;
      else
         ans = askyn("Òª¸½¼ÓÔÚ¾ÉÔİ´æµµÖ®ááÂğ", NA, YEA);
   }
   if (in_mail)
      sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]),
         currentuser.userid, fileinfo->filename);
   else
      sprintf(genbuf, "boards/%s/%s", key, fileinfo->filename);
   if(full)f_cp(genbuf,board, ( ans ) ? O_APPEND : O_CREAT);
   else part_cp(genbuf,board, ( ans ) ? "a+" : "w+");
   if (!nomsg)
      presskeyfor("ÒÑ½«¸ÃÎÄÕÂ´æÈëÔİ´æµµ, Çë°´<Enter>¼ÌĞø...",t_lines-1);
   return FULLUPDATE;
}
*/
//modified by yiyo
/* added by netty to handle post saving into (0)Announce */
/*
int
a_Import(path, key, ent,fileinfo, direct, nomsg)
char   *path, *key;
int     ent;
struct fileheader *fileinfo;
char   *direct;
int     nomsg;
{
   char    fname[STRLEN], *ip, bname[PATHLEN], buf[ 256 ];
   int     ch;
   MENU    pm;
   FILE 	*fn;
 
   modify_user_mode(DIGEST);
   sethomefile(buf, currentuser.userid,".announcepath");
   if((fn = fopen(buf, "r")) == NULL ) {
      presskeyfor("¶Ô²»Æğ, ÄãÃ»ÓĞÉè¶¨Ë¿Â·. ÇëÏÈÉè¶¨Ë¿Â·.",t_lines-1);
      return 0;
   }
   fscanf(fn,"%s",buf);
   fclose(fn);
   if (!dashd(buf)) {
      presskeyfor("ÄãÉè¶¨µÄË¿Â·ÒÑ¶ªÊ§, ÇëÖØĞÂÉè¶¨.",t_lines-1);
      return 0;
   }
   pm.path = buf;
   strcpy(pm.mtitle, "");
   a_loadnames(&pm);
   strcpy(fname, fileinfo->filename);
   sprintf(bname, "%s/%s", pm.path, fname);
   ip = &fname[strlen(fname) - 1];
   while (dashf(bname)) {
      if (*ip == 'Z') ip++, *ip = 'A', *(ip + 1) = '\0';
      else (*ip)++;
      sprintf(bname, "%s/%s", pm.path, fname);
   }
   sprintf(genbuf, "%-38.38s %s ", fileinfo->title, currentuser.userid);//modified by yiyo ¾«»ªÇøÏÔÊ¾ÕûÀíÕß
   a_additem(&pm, genbuf, fname, NULL, 0);
   a_savenames(&pm);
   if (in_mail)
      sprintf(genbuf, "/bin/cp -r mail/%c/%s/%s %s", 
              toupper(currentuser.userid[0]), currentuser.userid, 
	      fileinfo->filename, bname);
   else
      sprintf(genbuf, "/bin/cp -r boards/%s/%s %s", 
              key, fileinfo->filename, bname);
   system(genbuf);
   if (!nomsg) {
      presskeyfor("ÒÑ½«¸ÃÎÄÕÂ·Å½ø¾«»ªÇø, Çë°´<Enter>¼ÌĞø...",t_lines-1);
   }
   for (ch = 0; ch < pm.num; ch++)
      free(pm.item[ch]);
   return 1;
}
 
// deardragon 0921
*/
void a_addtofav(char *path, char *mtitle)
{
        struct favdigest fd;
        char buf[256];
        strcpy(fd.path, path);
        strcpy(fd.title, mtitle);
        sprintf(buf, "home/%c/%s/digest", toupper(currentuser.userid[0]), currentuser.userid);
        append_record(buf, &fd, sizeof(struct favdigest));
        //	move(t_lines-2, 0);
        //	prints("ÒÑ·ÅÈë¾«»ªÇøÊÕ²Ø¼Ğ£¬°´ÈÎÒâ¼ü¼ÌĞø...");
        //	pressanykey();
        presskeyfor("ÒÑ½«¸Ãµµ°¸·Å½ø¾«»ªÇø, Çë°´ÈÎÒâ¼ü¼ÌĞø...",t_lines-1);
        move(t_lines - 1, 0);
        prints("%s", (HAS_PERM(PERM_BOARDS)) ?
               "[1;31;44m[°å  Ö÷]  [33mËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ĞÂÔöÎÄÕÂ a ©¦ ĞÂÔöÄ¿Â¼ g ©¦ ±à¼­µµ°¸ E        [m" :
               "[1;31;44m[¹¦ÄÜ¼ü] [33m ËµÃ÷ h ©¦ Àë¿ª q,¡û ©¦ ÒÆ¶¯ÓÎ±ê k,¡ü,j,¡ı ©¦ ¶ÁÈ¡×ÊÁÏ Rtn,¡ú         [m");
}

int a_Import(char *path,char* key, struct fileheader* fileinfo, char* direct, int nomsg)
{
        char    fname[STRLEN], *ip, bname[PATHLEN];
        int     ch;
        MENU    pm;
	char    trace[STRLEN];
        //FILE       *fn;

        modify_user_mode(DIGEST);
        pm.path = direct;
        strcpy(pm.mtitle, "");
        a_loadnames(&pm);
        strcpy(fname, fileinfo->filename);
        sprintf(bname, "%s/%s", pm.path, fname);
        ip = &fname[strlen(fname) - 1];
        while (dashf(bname)) {
                if (*ip == 'Z')
                        ip++, *ip = 'A', *(ip + 1) = '\0';
                else
                        (*ip)++;
                sprintf(bname, "%s/%s", pm.path, fname);
        }
        sprintf(genbuf, "%-38.38s %s ", fileinfo->title, currentuser.userid);
        a_additem(&pm, genbuf, fname, NULL, 0);
        a_savenames(&pm);
	sprintf(trace, "ANNIMP %s", genbuf);
        if (in_mail)
                sprintf(genbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]),
                        currentuser.userid, fileinfo->filename);
        else
                sprintf(genbuf, "boards/%s/%s", key, fileinfo->filename, bname);
        f_cp(genbuf, bname, O_CREAT);
        if (!nomsg) {
                presskeyfor("ÒÑ½«¸ÃÎÄÕÂ·Å½ø¾«»ªÇø, Çë°´<Enter>¼ÌĞø...",t_lines-1);
        }
        for (ch = 0; ch < pm.num; ch++)
                free(pm.item[ch]);
	report(trace);
        return 1;
}
int a_readdeny(char *key)
{
	FILE *fn;
	char buf[PATHLEN];
	int res = 0, i = 0;
	sprintf(buf, "0Announce/.denyread");
	if (key[0] != '\0' && (fn = fopen(buf, "r")) != NULL)
	{
		while (fgets(buf, sizeof(buf), fn) != NULL)
		{
			if (!strncasecmp(key, buf, strlen(buf) - 1))
			{
				res = 1;
				break;
			}
		}
		fclose(fn);
	}
	return res;
}
//modified by yiyo
int a_menusearch(char *path, char *key, char *found);
int a_menusearch_s(char *path, char *key, char *found)
{
//	return 0;
	FILE *fn;
	char bname[20], flag='0';
	char    buf[PATHLEN], *ptr;
        int     searchmode = 0;
        if (key == NULL) {
                key = bname;
                a_prompt(-1, "ÊäÈëÓûËÑÑ°Ö®ÌÖÂÛÇøÃû³Æ: ", key, 18);
		if (a_readdeny(key))
		{
			presskeyfor("²»ÄÜÍ¨¹ıËÑË÷·ÃÎÊ¸ÃÌÖÂÛÇø,°´<Enter>¼ÌĞø...", t_lines - 1);
			return 0;
		}
                searchmode = 1;
        }
	if (strstr(path, key) != NULL)
	{
		presskeyfor("²»ÄÜËÑË÷µ±Ç°ÌÖÂÛÇø,°´<Enter>¼ÌĞø...", t_lines - 1);
		return 0;
	}
        sprintf(buf, "0Announce/.Search");
        if (key[0] != '\0' && (fn = fopen(buf, "r")) != NULL) {
                while (fgets(buf, sizeof(buf), fn) != NULL) {
                        if (searchmode && !strstr(buf, "groups/"))
                                continue;
                        ptr = strchr(buf, ':');
                        if (!ptr) {
                                fclose(fn); /* add fclose by yiyo */
                                return 0;
                        } else {
                                *ptr = '\0';
                                ptr = strtok(ptr + 1, " \t\n");
                        }
                        if (!strcasecmp(buf, key)) {
                                sprintf(found, "0Announce/%s", ptr);
                                flag = '1';
                                break;
                        }
                }
                fclose(fn);
                if(flag == '0') {
                        presskeyfor("ÕÒ²»µ½ÄúËùÊäÈëµÄÌÖÂÛÇø, °´<Enter>¼ÌĞø...",t_lines-1);
                        return 0;
                }
                return 1;
        }
        return 0;
}

int a_menusearch(char *path, char* key, char* found)
{
	//return 0;
        FILE   *fn;
        char    bname[20], flag='0';
        char    buf[PATHLEN], *ptr;
        int     searchmode = 0;
        if (key == NULL) {
                key = bname;
                a_prompt(-1, "ÊäÈëÓûËÑÑ°Ö®ÌÖÂÛÇøÃû³Æ: ", key, 18);
		if (a_readdeny(key))
		{
	return 0;
			presskeyfor("²»ÄÜÍ¨¹ıËÑË÷·ÃÎÊ¸ÃÌÖÂÛÇø,°´<Enter>¼ÌĞø...", t_lines - 1);
			return 0;
		}
                searchmode = 1;
        }
        sprintf(buf, "0Announce/.Search");
        if (key[0] != '\0' && (fn = fopen(buf, "r")) != NULL) {
                while (fgets(buf, sizeof(buf), fn) != NULL) {
                        if (searchmode && !strstr(buf, "groups/"))
                                continue;
                        ptr = strchr(buf, ':');
                        if (!ptr) {
                                fclose(fn); /* add fclose by yiyo */
                                return 0;
                        } else {
                                *ptr = '\0';
                                ptr = strtok(ptr + 1, " \t\n");
                        }
                        if (!strcasecmp(buf, key)) {
                                sprintf(found, "0Announce/%s", ptr);
                                flag = '1';
                                break;
                        }
                }
                fclose(fn);
                if(flag == '0') {
                        //a_prompt(-1, "ÕÒ²»µ½ÄúËùÊäÈëµÄÌÖÂÛÇø, °´<Enter>¼ÌĞø...", buf, 2);
                        presskeyfor("ÕÒ²»µ½ÄúËùÊäÈëµÄÌÖÂÛÇø, °´<Enter>¼ÌĞø...",t_lines-1);
                        return 0;
                }
                return 1;
        }
        return 0;
}

#ifdef INTERNET_EMAIL
void a_forward(char* path,ITEM* pitem, int mode)
{
        struct shortfile fhdr;
        char    fname[PATHLEN], *mesg;
        sprintf(fname, "%s/%s", path, pitem->fname);
        if (dashf(fname)) {
                strncpy(fhdr.title, pitem->title, STRLEN);
                strncpy(fhdr.filename, pitem->fname, STRLEN);
                fhdr.title[STRLEN - 1] = '\0';
                fhdr.filename[STRLEN - 1] = '\0';
                switch (doforward(path, &fhdr, mode)) {
                case 0:
                        mesg = "ÎÄÕÂ×ª¼ÄÍê³É!\n";
                        break;
                case -1:
                        mesg = "System error!!.\n";
                        break;
                case -2:
                        mesg = "Invalid address.\n";
                        break;
                default:
                        mesg = "È¡Ïû×ª¼Ä¶¯×÷.\n";
                }
                prints(mesg);
        } else {
                move(t_lines - 1, 0);
                prints("ÎŞ·¨×ª¼Ä´ËÏîÄ¿.\n");
        }
        pressanykey();
}
#endif
 
void a_newitem(MENU *pm, int mode)
{
        char    uident[STRLEN];
        char    board[STRLEN], title[STRLEN];
        char    fname[STRLEN], fpath[PATHLEN], fpath2[PATHLEN];
        char   *mesg;
        FILE   *pn;
        long	edname;
        long	count;
        char    ans[10], head;
	char	trace[STRLEN];
        srand(time(0));
        pm->page = 9999;
        head = 'X';
        switch (mode) {
        case ADDITEM:
                head = 'A';	/* article */
                break;
        case ADDGROUP:
                head = 'D';	/* directory */
                break;
        case ADDMAIL:
                sprintf(board, "bm/%s", currentuser.userid);
                if (!dashf(board)) {
                        //a_prompt(-1, "ÇëÏÈÖÁ¸ÃÌÖÂÛÇø½«ÎÄÕÂ´æÈëÔİ´æµµ, °´<Enter>¼ÌĞø...", ans, 2);
                        presskeyfor("ÇëÏÈÖÁ¸ÃÌÖÂÛÇø½«ÎÄÕÂ´æÈëÔİ´æµµ, °´<Enter>¼ÌĞø...",t_lines-1);
                        return;
                }
                mesg = "ÇëÊäÈëµµ°¸Ö®Ó¢ÎÄÃû³Æ(¿Éº¬Êı×Ö)£º";
                break;
        case ADDGOPHER: {
                        int     gport;
                        char    ghost[STRLEN], gtitle[STRLEN], gfname[STRLEN];
                        a_prompt(-2, "Á¬ÏßµÄÎ»Ö·£º", ghost, STRLEN - 14);
                        if (ghost[0] == '\0')
                                return;

                        /* edwardc.990427 fix up .. @_@ terrible code */

                        a_prompt(-2, "Á¬ÏßµÄÄ¿Â¼£º(¿É°´ Enter Ô¤Éè£©", gfname, STRLEN - 14);
                        /* ÈÃÁ¬ÏßÄ¿Â¼Ô¤ÉèÎª¿Õ×Ö´®£¬´ó¶à¿ÉÁ¬½Ó */
                        if (gfname[0] == '\0')
                                gfname[0] = '\0';

                        a_prompt(-2, "Á¬ÏßµÄPort£º(Ô¤ÉèÎª 70£©", ans, 6);
                        if (ans[0] == '\0')
                                strcpy(ans, "70");

                        gport = atoi(ans);

                        a_prompt(-2, "±êÌâ£º£¨Ô¤ÉèÎªÁ¬ÏßÎ»Ö·)", gtitle, 70);
                        if (gtitle[0] == '\0')
                                strncpy(gtitle, ghost, STRLEN);

                        a_additem(pm, gtitle, gfname, ghost, gport);
                        a_savenames(pm);
                        return;
                }
        }
        /* edwardc.990320 system will assign a filename for you .. */
        edname = time (0) + getpid () + getppid () + rand ();
        sprintf (fname, "%c%X", head, edname);
        sprintf(fpath, "%s/%s", pm->path, fname);
        count = 0;
        while (1) {
                int fd;
                if (mode != ADDITEM)
                        break;
                fd = open (fpath, O_CREAT | O_EXCL | O_WRONLY, 0644);
                if (fd == -1) {
                        ++edname;
                        sprintf (fname, "%c%X", head, edname);
                        sprintf (fpath, "%s/%s", pm->path, fname);
                        if (count++ > MAX_POSTRETRY) {
                                return;
                        }
                } else {
                        fchmod (fd, 0644);
                        close (fd);
                        break;
                }
        }
        mesg = "ÇëÊäÈëÎÄ¼ş»òÄ¿Â¼Ö®ÖĞÎÄÃû³Æ£º ";
        a_prompt(-1, mesg, title, 35);
        if (*title == '\0')
                return;
        switch (mode) {
        case ADDITEM:
                if( vedit(fpath,0,YEA) == -1 ) {
                        unlink (fpath);
                        return ;
                }
                chmod(fpath, 0644);
		sprintf(trace,"ADDITEM %s -- %s", title, fpath);
                break;
        case ADDGROUP:
                if(strlen(fpath)>=230) {
                        //a_prompt(-1,"¶Ô²»Æğ, Ä¿Â¼²ã´ÎÌ«Éî, È¡Ïû²Ù×ö!",ans,2);
                        presskeyfor("¶Ô²»Æğ, Ä¿Â¼²ã´ÎÌ«Éî, È¡Ïû²Ù×ö!",t_lines-1);
                        return ;
                }
                while (mkdir(fpath, 0755) == -1) {
                        ++edname;
                        sprintf (fname, "%c%X", head, edname);
                        sprintf (fpath, "%s/%s", pm->path, fname);
                }
                chmod(fpath, 0755);
		sprintf(trace,"ADDGROUP %s -- %s",title, fpath);
                break;
        case ADDMAIL:
                rename(board, fpath);
		sprintf(trace, "ADDMAIL %s -- %s", title, fpath);
                break;
        }
        if (mode != ADDGROUP)
                sprintf(genbuf, "%-38.38s %s ", title, currentuser.userid);
        else {
                /*Add by SmallPig*/
                if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_ANNOUNCE)) {
                        move(1, 0);
                        clrtoeol();
                        getdata(1, 0, "°åÖ÷: ", uident, 35, DOECHO, YEA);
                        if (uident[0] != '\0')
                                sprintf(genbuf, "%-38.38s(BM: %s)", title, uident);
                        else
                                sprintf(genbuf, "%-38.38s", title);
                } else
                        sprintf(genbuf, "%-38.38s", title);
        }
        a_additem(pm, genbuf, fname, NULL, 0);
        a_savenames(pm);
        if (mode == ADDGROUP) {
                sprintf(fpath2, "%s/%s/.Names", pm->path, fname);
                if ((pn = fopen(fpath2, "w")) != NULL) {
                        fprintf(pn, "#\n");
                        fprintf(pn, "# Title=%s\n", genbuf);
                        fprintf(pn, "#\n");
                        fclose(pn);
                }
        }
	report(trace);
}

void a_moveitem(MENU *pm);
void a_moveitem_s(MENU *pm, MENU *oldpm)
{
	int i = 0, savenow = oldpm->now;
	while (i < oldpm->num)
	{
		if (pm->item[pm->now] == oldpm->item[i])
			break;
		i++;
	}
	if (i == oldpm->num)
		return;
	oldpm->now = i;
	a_moveitem(oldpm);
	oldpm->now = savenow;
}

void a_moveitem(MENU *pm)
{
        ITEM   *tmp;
        char    newnum[STRLEN];
        int     num, n;
        sprintf(genbuf, "ÇëÊäÈëµÚ %d ÏîµÄĞÂ´ÎĞò: ", pm->now + 1);
        a_prompt(-2, genbuf, newnum, 6);
        num = (newnum[0] == '$') ? 9999 : atoi(newnum) - 1;
        if (num >= pm->num)
                num = pm->num - 1;
        else if (num < 0)
                return;
        tmp = pm->item[pm->now];
        if (num > pm->now) {
                for (n = pm->now; n < num; n++)
                        pm->item[n] = pm->item[n + 1];
        } else {
                for (n = pm->now; n > num; n--)
                        pm->item[n] = pm->item[n - 1];
        }
        pm->item[num] = tmp;
        pm->now = num;
        a_savenames(pm);
}

void
a_copypaste(MENU *pm, int paste)
{
        static char title[STRLEN], filename[STRLEN], fpath[PATHLEN];
        ITEM   *item;
        char    newpath[PATHLEN]/*,ans[5]*/;
        FILE 	*fn;
	char   trace[STRLEN];
        move(t_lines - 1, 0);
        sethomefile(fpath,currentuser.userid,".copypaste");

        if (!paste) {
                fn = fopen(fpath,"w+");
                if( fn == NULL ) {
                        presskeyfor("²Ù×÷Ê§°Ü£¡Í¨ÖªÕ¾³¤£¡");
                        return;
                } /* add by yiyo */
                item = pm->item[pm->now];
                fwrite(item->title,sizeof(item->title),1,fn);
                fwrite(item->fname,sizeof(item->fname),1,fn);
                sprintf(genbuf, "%s/%s", pm->path, item->fname);
                strncpy(fpath, genbuf, PATHLEN);
                fpath[PATHLEN - 1] = '\0';
                fwrite(fpath,sizeof(fpath),1,fn);
                fclose(fn);
                //a_prompt(-1,"µµ°¸±êÊ¶Íê³É. (×¢Òâ! Õ³ÌùÎÄÕÂáá²ÅÄÜ½«ÎÄÕÂ delete!)",ans,2);
                presskeyfor("µµ°¸±êÊ¶Íê³É. (×¢Òâ! Õ³ÌùÎÄÕÂáá²ÅÄÜ½«ÎÄÕÂ delete!)",t_lines-1);
        } else {
                if ((fn = fopen(fpath /*genbuf*/, "r")) == NULL) {
                        //a_prompt(-1,"ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.",ans,2);
                        if(paste==2)
                                presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ link ÃüÁî.",t_lines-1);
                        else
                                presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.",t_lines-1);
                        return ;
                }
                fread(title,sizeof(item->title),1,fn);
                fread(filename,sizeof(item->fname),1,fn);
                fread(fpath,sizeof(fpath),1,fn);
                fclose(fn);
                if(paste==2) {
                        int  edname = time (0) + getpid () + getppid () + rand ();
                        sprintf (filename, "L%X", edname);
                        sprintf(newpath,"%s/%s",pm->path,filename);
                } else
                        sprintf(newpath, "%s/%s", pm->path, filename);
                if (*title == '\0' || *filename == '\0') {
                        if(paste==2)
                                presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ link ÃüÁî.",t_lines-1);
                        else
                                presskeyfor("ÇëÏÈÊ¹ÓÃ copy ÃüÁîÔÙÊ¹ÓÃ paste ÃüÁî.",t_lines-1);

                } else if (!(dashf(fpath) || dashd(fpath))) {
                        if(paste==1)
                                sprintf(genbuf,"Äã¿½±´µÄ%sµµ°¸/Ä¿Â¼²»´æÔÚ,¿ÉÄÜ±»É¾³ı,È¡ÏûÕ³Ìù.",filename);
                        else
                                sprintf(genbuf,"ÄãÁ´½ÓµÄ%sµµ°¸/Ä¿Â¼²»´æÔÚ,¿ÉÄÜ±»É¾³ı,È¡ÏûÁ´½Ó.",filename);
                        presskeyfor(genbuf,t_lines-1);
                } else if (dashf(newpath) || dashd(newpath)) {
                        sprintf(genbuf,"%s µµ°¸/Ä¿Â¼ÒÑ¾­´æÔÚ. ", filename);
                        presskeyfor(genbuf,t_lines-1);
                } else if (strstr(newpath, fpath) != NULL) {
                        presskeyfor("ÎŞ·¨½«Ä¿Â¼°á½ø×Ô¼ºµÄ×ÓÄ¿Â¼ÖĞ, »áÔì³ÉËÀ»ØÈ¦.",t_lines-1);
                } else {
                        if(paste==2)
                                sprintf(genbuf,"/bin/ln -s %s/%s %s/%s",BBSHOME,fpath,BBSHOME,newpath);
                        if(paste==1)
                                sprintf(genbuf, "/bin/cp -r %s %s", fpath, newpath);
                        if(paste==4)
                                sprintf(genbuf, "/bin/cp -rp %s %s", fpath, newpath);

                        system(genbuf);
                        a_additem(pm, title, filename, NULL, 0);
                        a_savenames(pm);
			sprintf(trace,"ANNCOPY %s --> %s", fpath, newpath);
			report(trace);
                }
        }
        pm->page = 9999;
}

void a_delete(MENU *pm)
{
        ITEM   *item;
        char    fpath[PATHLEN],ans[5];
        int     n;
        FILE 	*fn;
	char    trace[STRLEN];
        item = pm->item[pm->now];
        move(t_lines - 2, 0);
        prints("%5d  %-50s\n", pm->now + 1, item->title);
	sprintf(trace, "ANNDEL %s -- %s/%s", item->title, pm->path,item->fname);
        if (item->host == NULL) {
                sethomefile(fpath,currentuser.userid,".copypaste");
                if ((fn = fopen(fpath /*genbuf*/, "r")) != NULL) {
                        fread(genbuf,sizeof(item->title),1,fn);
                        fread(genbuf,sizeof(item->fname),1,fn);
                        fread(genbuf,sizeof(fpath),1,fn);
                        fclose(fn);
                }
                sprintf(fpath, "%s/%s", pm->path, item->fname);
                if(!strncmp(genbuf,fpath,sizeof(fpath)))
                        a_prompt(-1,"[1;5;31m¾¯¸æ[m: ¸Ãµµ°¸/Ä¿Â¼ÊÇ±»°åÖ÷×öÁË±ê¼Ç, [1;33mÈç¹ûÉ¾³ı, Ôò²»ÄÜÔÙÕ³Ìù¸ÃÎÄÕÂ!![m",ans,2);
                if (dashf(fpath)) {
                        /* Efan: Èç¹û¸ÃpathÖ¸ÏòËüµÄÉÏ¼¶Ä¿Â¼£¬ÔòÊÓÎª·Ç·¨£¬²»ÄÜÉ¾³ı	*/
                        if (strstr (fpath, ".."))
                                return;
                        if (askyn("É¾³ı´Ëµµ°¸, È·¶¨Âğ", NA, YEA) == NA)
                                return;
                        unlink(fpath);
                } else if (dashd(fpath)) {
                        if (askyn("É¾³ıÕû¸ö×ÓÄ¿Â¼, ±ğ¿ªÍæĞ¦Å¶, È·¶¨Âğ", NA, YEA) == NA)
                                return;
                        deltree(fpath);
                }
        } else {
                if (askyn("É¾³ıÁ¬ÏßÑ¡Ïî, È·¶¨Âğ", NA, YEA) == NA)
                        return;
        }
        free(item);
        (pm->num)--;
        for (n = pm->now; n < pm->num; n++)
                pm->item[n] = pm->item[n + 1];
        a_savenames(pm);
	report(trace);
}

void a_delete_s(MENU *pm, MENU *oldpm, int depth, MENU **smenus)
{
	int savenow = oldpm->now, i = 0;
	int savenum = oldpm->num;
	ITEM *pitem;
	while (i < oldpm->num)
	{
		if (oldpm->item[i] == pm->item[pm->now])
			break;
		i++;
	}
	if (i < oldpm->num)
	{
		oldpm->now = i;
		pitem = oldpm->item[i];
		a_delete(oldpm);
		if (oldpm->num < savenum)
		{
			while (depth >= 0 && (pm = smenus[depth]))
			{
				i = 0;
				while (i < pm->num)
				{
					if (pitem == pm->item[i])
						break;
					i++;
				}
				if (oldpm->num == oldpm->now)
					pm->item[pm->num - 1] = NULL;
				else for (; i + 1 < pm->num; i++)
					{
						pm->item[i] = pm->item[i + 1];
					}
				(pm->num)--;
				pm->now = 0;
				depth --;
			}
		}
	}
	oldpm->now = savenow;
}

void a_newname(MENU *pm)
{
        ITEM   *item;
        char    fname[STRLEN];
        char    fpath[PATHLEN];
        char   *mesg;
        item = pm->item[pm->now];
        a_prompt(-2, "ĞÂµµÃû: ", fname, 12);
        if (*fname == '\0')
                return;
        sprintf(fpath, "%s/%s", pm->path, fname);
        if (!valid_fname(fname)) {
                mesg = "²»ºÏ·¨µµ°¸Ãû³Æ.";
        } else if (dashf(fpath) || dashd(fpath)) {
                mesg = "ÏµÍ³ÖĞÒÑÓĞ´Ëµµ°¸´æÔÚÁË.";
        } else {
                sprintf(genbuf, "%s/%s", pm->path, item->fname);
                if (rename(genbuf, fpath) == 0) {
                        strcpy(item->fname, fname);
                        a_savenames(pm);
                        return;
                }
                mesg = "µµÃû¸ü¸ÄÊ§°Ü!!";
        }
        prints(mesg);
        egetch();
}

void a_findlosthide(MENU *pm)
{
	char cmd[STRLEN*2];
	char trace[STRLEN];
	sprintf(cmd,"%s/bin/findlosthide.sh %s/%s > /dev/null",BBSHOME,BBSHOME,pm->path);
	move(t_lines - 1, 0);
	if( askyn("ÄãÈ·¶¨ÒªÕÒ»Ø¶ªÊ§µÄÒş²ØÎÄ¼şÂğ?",NA,NA)==YEA)	
	{
		system(cmd);
		sprintf(trace,"ANNFIND %s",pm->path);
		report(trace);
		presskeyfor("ÕÒ»Ø¶ªÊ§ÎÄ¼ş³É¹¦",t_lines-1);
	}
	return;
}

void a_manager(MENU *pm, int ch)
{
        char    uident[STRLEN];
        ITEM   *item;
        MENU   xpm;
        char    fpath[PATHLEN], changed_T[STRLEN];//, ans[5];
        if (pm->num > 0) {
                item = pm->item[pm->now];
                sprintf(fpath, "%s/%s", pm->path, item->fname);
        }
        switch (ch) {
        case 'a':
                a_newitem(pm, ADDITEM);
                break;
        case 'g':
                a_newitem(pm, ADDGROUP);
                break;
        case 'i':
                a_newitem(pm, ADDMAIL);
                break;
        case 'G':
                if (HAS_PERM(PERM_SYSOP))
                        a_newitem(pm, ADDGOPHER);
                break;
        case 'p':
                a_copypaste(pm, 1);
                break;

        case 'P':
                a_copypaste(pm, 4);
                break;

        case 'l':
                a_copypaste(pm,2);
                break;
	case 'O':
		a_findlosthide(pm);
		break;
        case 'S': {
                        FILE *fn;
                        pm->page = 9999;
                        sethomefile(genbuf,currentuser.userid,".announcepath");
                        fn = fopen(genbuf,"r");
                        if(fn) {
                                char tmpath[1000];
                                fgets(tmpath,1000,fn);
                                fclose(fn);
                                sprintf(genbuf,"µ±Ç°Ë¿Â·: %s",tmpath);
                        } else
                                strcpy(genbuf,"Äú»¹Ã»ÓĞÓÃ f ÉèÖÃË¿Â·.");
                        presskeyfor(genbuf,t_lines-1);
                        break;
                }
        case 'f': {
                        FILE *fn;
                        pm->page = 9999;
                        sethomefile(genbuf,currentuser.userid,".announcepath");
                        fn = fopen(genbuf,"w+");
                        if(fn) {
                                fprintf(fn,"%s",pm->path);
                                fclose(fn);
                                presskeyfor("ÒÑ½«¸ÃÂ·¾¶ÉèÎªË¿Â·, Çë°´<Enter>¼ÌĞø...",t_lines-1);
                        }
                        break;
                }
        }
        if (pm->num <= 0)
                return;
        switch (ch) {
        case 's':
                a_fmode = !a_fmode;
                pm->page = 9999;
                break;
        case 'm':
                a_moveitem(pm);
                pm->page = 9999;
                break;
        case 'd':
        case 'D':
                a_delete(pm);
                pm->page = 9999;
                break;
        case 'V':
        case 'v':
	case 'w':
                if (HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_ANNOUNCE)) {
                        if (ch == 'v')
                                sprintf(fpath, "%s/.Names", pm->path);
                        else if (ch == 'V')
                                sprintf(fpath, "0Announce/.Search");
			else
				sprintf(fpath, "0Announce/.denyread");
                        if (dashf(fpath)) {
                                modify_user_mode(EDITANN);
                                vedit(fpath, 0, YEA);
                                modify_user_mode(DIGEST);
                        }
                        pm->page = 9999;
                }
                break;
        case 'T':
                strcpy(changed_T,item->title);
                changed_T[36]='\0';
                strcpy(changed_T, rtrim(changed_T));
                getdata(t_lines - 1, 0, "ĞÂ±êÌâ: ", changed_T, 35, DOECHO, NA);
                //         a_prompt(-2, "ĞÂ±êÌâ: ", changed_T, 35);
                if (*changed_T) {
                        if (dashf(fpath)) {
                                sprintf(genbuf, "%-38.38s %s ", changed_T, currentuser.userid);
                                strncpy(item->title, genbuf, 72);
                                item->title[71] = '\0';
                        } else if (dashd(fpath)) {
                                if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_ANNOUNCE)) {
                                        move(1, 0);
                                        clrtoeol();
                                        getdata(1, 0, "°åÖ÷: ", uident, 35, DOECHO, YEA);
                                        if (uident[0] != '\0')
                                                sprintf(genbuf, "%-38.38s(BM: %s)", changed_T, uident);
                                        else
                                                sprintf(genbuf, "%-38.38s", changed_T);
                                } else
                                        sprintf(genbuf, "%-38.38s", changed_T);
                                xpm.path=fpath;
                                a_loadnames(&xpm);
                                strcpy(xpm.mtitle, genbuf);
                                a_savenames(&xpm);
                                strncpy(item->title, genbuf, 72);
                        } else if (pm->item[pm->now]->host != NULL)
                                strncpy(item->title, changed_T, 72);
                        item->title[71] = '\0';
                        a_savenames(pm);
                }
                pm->page = 9999;
                break;
        case 'E':
                /* Efan: ±à¼­µÄpath²»ÄÜÊÇÔÚ¸ÃÄ¿Â¼Íâ	*/
                if (strstr (fpath, ".."))
                        break;
                if (dashf(fpath)) {
                        modify_user_mode(EDITANN);
                        vedit(fpath, 0, YEA);
                        modify_user_mode(DIGEST);
                }
                pm->page = 9999;
                break;
        case 'n':
                a_newname(pm);
                pm->page = 9999;
                break;
        case 'c':
                a_copypaste(pm, 0);
                break;
                /*	case Ctrl('K'):
                		y_lockscreen();
                		break;*/
        }
}

int a_loadnames_s(MENU *npm, MENU *opm, char *key)
{
	int i;
	npm->path = opm->path;
	npm->page = opm->page;
	npm->now = 0;
	npm->level = opm->level;
	npm->num = 0;
	for (i = 0; i < opm->num; i++)
	{
		if (strstr(opm->item[i]->title, key) == NULL)
		{
			continue;
		}
		npm->item[npm->num++] = opm->item[i];
	}
	return 0;
}

void a_manager_s(MENU *pm, MENU *oldpm, int ch, int depth/* 0-2 */, MENU **smenus)
{
	char uident[STRLEN];
	ITEM *item;
	MENU xpm;
	char fpath[PATHLEN], changed_T[STRLEN];
	if (pm->num > 0)
	{
		item = pm->item[pm->now];
		sprintf(fpath, "%s/%s", pm->path, item->fname);
	}
	switch (ch)
	{
		case 'a':
			a_newitem(oldpm, ADDITEM);
			pm->page = 9999;
			break;
		case 'g':
			a_newitem(oldpm, ADDGROUP);
			pm->page = 9999;
			break;
		case 'i':
			a_newitem(oldpm, ADDMAIL);
			pm->page = 9999;
			break;
		case 'G':
			if (HAS_PERM(PERM_SYSOP))
			{
				a_newitem(oldpm, ADDGOPHER);
				pm->page = 9999;
			}
			break;
		case 'p':
			a_copypaste(oldpm, 1);
			pm->page = 9999;
			break;
		case 'P':
			a_copypaste(oldpm, 4);
			pm->page = 9999;
			break;
		case 'l':
			a_copypaste(oldpm, 2);
			pm->page = 9999;
			break;
		case 'O':
			a_findlosthide(pm);
			break;
		case 'S':
			{
                        	FILE *fn;
                        	oldpm->page = 9999;
                        	sethomefile(genbuf,currentuser.userid,".announcepath");
                        	fn = fopen(genbuf,"r");
                        	if(fn) {
                        	        char tmpath[1000];
                        	        fgets(tmpath,1000,fn);
                        	        fclose(fn);
                        	        sprintf(genbuf,"µ±Ç°Ë¿Â·: %s",tmpath);
                        	} else
                        	        strcpy(genbuf,"Äú»¹Ã»ÓĞÓÃ f ÉèÖÃË¿Â·.");
                        	presskeyfor(genbuf,t_lines-1);
				pm->page = 9999;
                        	break;
                	}
		case 'f':
			{
                        	FILE *fn;
                        	oldpm->page = 9999;
                        	sethomefile(genbuf,currentuser.userid,".announcepath");
                        	fn = fopen(genbuf,"w+");
                        	if(fn) {
                        	        fprintf(fn,"%s",oldpm->path);
                        	        fclose(fn);
                        	        presskeyfor("ÒÑ½«¸ÃÂ·¾¶ÉèÎªË¿Â·, Çë°´<Enter>¼ÌĞø...",t_lines-1);
                        	}
				pm->page = 9999;
                        	break;
			}
	}
	if (oldpm->num <= 0 || pm->num <= 0)
		return;
	switch (ch)
	{
		case 's':
			a_fmode = !a_fmode;
			pm->page = 9999;
			break;
		case 'm':
			a_moveitem_s(pm, oldpm);
			pm->page = 9999;
			break;
		case 'd':
		case 'D':
			a_delete_s(pm, oldpm, depth, smenus);
			pm->page = 9999;
			break;
		case 'V':
		case 'v':
			if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_BLEVELS))
			{
				if (ch == 'v')
					sprintf(fpath, "%s/.Names", oldpm->path);
				else
					sprintf(fpath, "%s/.Search", oldpm->path);
				if (dashf(fpath))
				{
					modify_user_mode(EDITANN);
					vedit(fpath, 0, YEA);
					modify_user_mode(DIGEST);
				}
				pm->page = 9999;
			}
			break;
		case 'T':
                	strcpy(changed_T,item->title);
                	changed_T[36]='\0';
                	strcpy(changed_T, rtrim(changed_T));
                	getdata(t_lines - 1, 0, "ĞÂ±êÌâ: ", changed_T, 35, DOECHO, NA);
                	if (*changed_T) {
                        	if (dashf(fpath)) {
                        	        sprintf(genbuf, "%-38.38s %s ", changed_T, currentuser.userid);
                        	        strncpy(item->title, genbuf, 72);
                        	        item->title[71] = '\0';
                        	} else if (dashd(fpath)) {
                                	if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_ANNOUNCE)) {
                                	        move(1, 0);
                                	        clrtoeol();
                                	        getdata(1, 0, "°åÖ÷: ", uident, 35, DOECHO, YEA);
                                	        if (uident[0] != '\0')
                                	                sprintf(genbuf, "%-38.38s(BM: %s)", changed_T, uident);
                                	        else
							sprintf(genbuf, "%-38.38s", changed_T);
                                	} else
                                	        sprintf(genbuf, "%-38.38s", changed_T);
                                	xpm.path=fpath;
                                	a_loadnames(&xpm);
                                	strcpy(xpm.mtitle, genbuf);
                                	a_savenames(&xpm);
                                	strncpy(item->title, genbuf, 72);
                        	} else if (pm->item[pm->now]->host != NULL)
                                	strncpy(item->title, changed_T, 72);
				item->title[71] = '\0';
/*oldpm?*/			a_savenames(oldpm);
        	        }
                	pm->page = 9999;
                	break;
		case 'E':
			if (strstr (fpath, ".."))
				break;
			if (dashf(fpath))
			{
				modify_user_mode(EDITANN);
				vedit(fpath, 0, YEA);
				modify_user_mode(DIGEST);
			}
			pm->page = 9999;
			break;
		case 'n':
			a_newname(oldpm);
			pm->page = 9999;
			break;
		case 'c':
			a_copypaste(pm, 0);
			break;
	}
}

void a_search(char *maintitle, MENU *pme,int lastbmonly, int depth, MENU *oldpme, MENU **smenu_x /* NULL for depth 1*/)
{
	MENU 	me, **smenus;
	int bmonly, ch;
	char key[24], buf[STRLEN], tmp[STRLEN];
	char *bmstr;
	int number = 0, savemode;
	char fname[PATHLEN];
	int flag = 0;
	
	if (depth >= 4)
		return;

	if (depth == 1)
	{
		flag = 1;
		smenus = (MENU **)malloc(SEARCHDEPTH * sizeof(MENU *));
		smenus[0] = &me;
	}
	else
	{
		smenus = smenu_x;
		smenu_x[depth - 1] = &me;
	}

	a_prompt(-1, "ÇëÊäÈëÒªËÑË÷µÄ¹Ø¼ü×Ö: ", key, 20);
	if (key[0] == 0)
		return;
	sprintf(tmp, "%s Level: %d", maintitle, depth);
MACTEST_S_1:
	a_loadnames_s(&me, pme, key);
	
	modify_user_mode(DIGEST);
	me.path = pme->path;
	strcpy(me.mtitle, tmp);
	me.level = pme->level;
	bmonly = lastbmonly;
	/**/
	memset(buf,0,STRLEN);
        strncpy(buf, me.mtitle,STRLEN);
        bmstr = strstr(buf+38, "(BM:");
        if (bmstr != NULL) {
                if (chk_currBM2(bmstr + 4))
                        me.level |= PERM_BOARDS;
                else
                        if (bmonly == 1 && !HAS_PERM(PERM_BOARDS))
                                return;
        }
        if (strstr(me.mtitle, "(BM: BMS)")||strstr(me.mtitle, "(BM: SECRET)")
                        || strstr(me.mtitle, "(BM: SYSOPS)"))
                bmonly = 1;
        strcpy(buf, me.mtitle);
        bmstr = strstr(buf, "(BM:");

        me.page = 9999;
        me.now = 0;
MACTEST_S_2:
        while (1) {
                if (me.now >= me.num && me.num > 0) {
                        me.now = me.num - 1;
                } else if (me.now < 0) {
                        me.now = 0;
                }
                if (me.now < me.page || me.now >= me.page + A_PAGESIZE) {
                        me.page = me.now - (me.now % A_PAGESIZE);
                        a_showmenu(&me);
                }
                move(3 + me.now - me.page, 0);
                prints("->");
                ch = egetch();
                move(3 + me.now - me.page, 0);
                prints("  ");
                if (ch == 'Q' || ch == 'q' || ch == KEY_LEFT || ch == EOF)
                        break;
EXPRESS_S: /* Leeward 98.09.13 */
                switch (ch) {
		case 'e':
			if (depth == 3)
				break;
			a_search(maintitle, &me, lastbmonly, depth + 1, oldpme, (flag == 1? smenus: smenu_x));
			goto MACTEST_S_1;
			break;
                case KEY_UP:
                case 'K':
                case 'k':
                        if (--me.now < 0)
                                me.now = me.num - 1;
                        break;
                case KEY_DOWN:
                case 'J':
                case 'j':
                        if (++me.now >= me.num)
                                me.now = 0;
                        break;
                case Ctrl('K'):
                        y_lockscreen();
clear();
goto MACTEST_S_1;
                        break;
		case 'M':
			savemode = uinfo.mode;
			m_new();
			modify_user_mode(savemode);
			me.page = 9999;
			break;
                case 'h':
                        show_help("help/announcereadhelp");
                        me.page = 9999;
                        break;
                case '\n':
                case '\r':
                        if (number > 0) {
                                me.now = number - 1;
                                number = 0;
                                continue;
                        }
                case 'R':
                case 'r':
                case KEY_RIGHT:
                        if (me.now < me.num) {
                                if (me.item[me.now]->host != NULL) {
                                        if (me.item[me.now]->fname[0] == '0') {
                                                if (get_con(me.item[me.now]->host, me.item[me.now]->port) != -1) {
                                                        char    tmpfile[30];
                                                        GOPHER  tmp;
                                                        extern GOPHER *tmpitem;

                                                        tmpitem = &tmp;
                                                        strcpy(tmp.server, me.item[me.now]->host);
                                                        strcpy(tmp.file, me.item[me.now]->fname);
                                                        sprintf(tmp.title, "0%s", me.item[me.now]->title);
                                                        tmp.port = me.item[me.now]->port;
                                                        enterdir(me.item[me.now]->fname);
                                                        setuserfile(tmpfile, "gopher.tmp.s");
                                                        savetmpfile(tmpfile);
                                                        ansimore(tmpfile, YEA);
                                                        unlink(tmpfile);
                                                }
                                        } else {
                                                gopher(me.item[me.now]->host, me.item[me.now]->fname,
                                                       me.item[me.now]->port, me.item[me.now]->title);
                                        }
                                        me.page = 9999;
                                        break;
                                } else
                                        sprintf(fname, "%s/%s", me.path, me.item[me.now]->fname);
                                if (dashf(fname)) {
                                        /* Efan: Èç¹û¸ÃpathÖ¸ÏòÉÏ¼¶Ä¿Â¼£¬ÔòÊÓÎª ·Ç·¨	*/
                                        if (strstr (fname, ".."))
                                                break;
                                        /* Leeward 98.09:ÔÚ¾«»ªÇøÔÄ¶ÁÎÄÕÂµ½Ä©Î² Ê±£¬ÓÃÉÏ£¯ÏÂ¼ıÍ·Ö±½ÓÌø×ªµ½Ç°£¯ºóÒ»Ïî */
                                        if(ansimore(fname, NA)==KEY_UP) {
                                                if(--me.now < 0)
                                                        me.now = me.num -1;
                                                ch = KEY_RIGHT;
                                                goto EXPRESS_S;
                                        }
                                        prints("[1m[44m[31m[ÔÄ¶Á¾«»ªÇø×ÊÁÏ]  [33m½áÊøQ, ¡û ©¦ ÉÏÒ»Ïî×ÊÁÏ U,¡ü©¦ ÏÂÒ»Ïî×ÊÁÏ <Enter>,<Space>,¡ı [m");
                                        switch (ch = egetch()) {
                                        case KEY_DOWN:
                                        case ' ':
                                        case '\n':
                                                if (++ me.now >= me.num)
                                                        me.now = 0;
                                                ch = KEY_RIGHT;
                                                goto EXPRESS_S;
                                        case KEY_UP:
                                                //case 'u':
                                        case'U':
                                                if (-- me.now < 0)
                                                        me.now = me.num - 1;
                                                ch = KEY_RIGHT;
                                                goto EXPRESS_S;
                                        case 'h':
                                                goto EXPRESS_S;
                                        default:
                                                break;
                                        }
                                } else if (dashd(fname)) {
                                        a_menu(me.item[me.now]->title, fname, me.level, bmonly);
                                }
                                me.page = 9999;
                        }
                        break;
                /*case '/': {
                                char  found[256];
                                if (a_menusearch_s(me.path, NULL, found)) {
                                        a_menu("", found,
                                               ((HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP)
                                                 || HAS_PERM(PERM_OBOARDS)) ? PERM_BOARDS : 0),0);
                                        me.page = 9999;
                                }
                        }
                        break;*/
                case 'u':
                        modify_user_mode(QUERY);
                        t_query();
                        me.page = 9999;
                        break;

                case Ctrl('A'):
                        sprintf(fname, "%s/%s", me.path, me.item[me.now]->fname);
                        a_addtofav(fname, me.item[me.now]->title);
                        break;

#ifdef INTERNET_EMAIL

                case 'F':
                case 'U':
        if (me.now < me.num && HAS_PERM(PERM_BASIC)) {
                                a_forward(me.path, me.item[me.now], ch == 'U');
                                me.page = 9999;
                        }
                        break;
#endif

                case '!':
                        if (!Q_Goodbye())
                                break;	/* youzi leave */
                }
                if (ch >= '0' && ch <= '9') {
                        number = number * 10 + (ch - '0');
                        ch = '\0';
                } else {
                        number = 0;
                }
                if (me.level & PERM_BOARDS)
		{
			a_manager_s(&me, oldpme, ch, depth - 1, smenus);
			//a_manager(&me, ch);
			//me.page = 9999;
			goto MACTEST_S_2;
		}
                else if (ch == 'a' && HAS_PERM(PERM_POST)
                                && strstr(oldpme->mtitle, "<GUESTBOOK>") == oldpme->mtitle) {
                        a_newitem(oldpme, ADDITEM);
                } //add by yiyo ÁôÑÔ±¡
        }
	/**/
	if (flag == 0)
		smenu_x[depth - 1] = NULL;
	else
		free(smenus);
}

void a_menu(char *maintitle, char* path, int lastlevel, int lastbmonly)
{
        MENU    me;
        char    fname[PATHLEN], tmp[STRLEN];
        int     ch;
        char   *bmstr;
        char    buf[STRLEN];
        int     bmonly;
        int     number = 0;
        int	savemode;
        char    something[PATHLEN+20];//add by wujian

/* Only a test by macana*/

        modify_user_mode(DIGEST);
        strcpy(something,path);
        strcat(something,"/welcome");
        me.path = path;
        if (dashf (something))
                show_help(something);
        strcpy(me.mtitle, maintitle);
        me.level = lastlevel;
        bmonly = lastbmonly;
        sprintf (buf, "0Announce/PersonalCorpus/%c/%s", toupper (currentuser.userid[0]), currentuser.userid);
        int len=strlen(buf);       
        if (strstr(path, buf)){
                if(path[len]==0 || path[len]=='/')
                   me.level |= PERM_BOARDS;
        }
        a_loadnames(&me);

MACTEST:
        memset(buf,0,STRLEN);
        strncpy(buf, me.mtitle,STRLEN);
        bmstr = strstr(buf+38, "(BM:");
        if (bmstr != NULL) {
                if (chk_currBM2(bmstr + 4))
                        me.level |= PERM_BOARDS;
                else
                        if (bmonly == 1 && !HAS_PERM(PERM_BOARDS))
                                return;
        }
	if (bmstr != NULL && !HAS_PERM(PERM_ANNOUNCE) && HAS_PERM(PERM_OBOARDS) && strstr(path, "PersonalCorpus"))
	{
		char BMstr[STRLEN];
		char *ptr;
		strcpy(BMstr, bmstr + 4);
		ptr = strtok(BMstr, " ,:;|&()\0\n");
		if (strcmp(currentuser.userid, ptr))
			me.level &= ~PERM_BOARDS;
	}
        if (   strstr(me.mtitle, "(BM: BMS)")||strstr(me.mtitle, "(BM: SECRET)")
                        || strstr(me.mtitle, "(BM: SYSOPS)"))
                bmonly = 1;
        //   a_loadnames(&me);
        strcpy(buf, me.mtitle);
        bmstr = strstr(buf, "(BM:");

        me.page = 9999;
        me.now = 0;
        while (1) {
                if (me.now >= me.num && me.num > 0) {
                        me.now = me.num - 1;
                } else if (me.now < 0) {
                        me.now = 0;
                }
                if (me.now < me.page || me.now >= me.page + A_PAGESIZE) {
                        me.page = me.now - (me.now % A_PAGESIZE);
                        a_showmenu(&me);
                }
                move(3 + me.now - me.page, 0);
                prints("->");
                ch = egetch();
                move(3 + me.now - me.page, 0);
                prints("  ");
                if (ch == 'Q' || ch == 'q' || ch == KEY_LEFT || ch == EOF)
                        break;
EXPRESS: /* Leeward 98.09.13 */
                switch (ch) {
		case 'e':
			a_search("ËÑË÷½á¹ûä¯ÀÀ", &me, bmonly, 1, &me, NULL);
			goto MACTEST;
			break;
                case KEY_UP:
                case 'K':
                case 'k':
                        if (--me.now < 0)
                                me.now = me.num - 1;
                        break;
                case KEY_DOWN:
                case 'J':
                case 'j':
                        if (++me.now >= me.num)
                                me.now = 0;
                        break;
                case 'P':
                case KEY_PGUP:
                case Ctrl('B'):
                                                if (me.now >= A_PAGESIZE)
                                                        me.now -= A_PAGESIZE;
                                else if (me.now > 0)
                                        me.now = 0;
                                else
                                        me.now = me.num - 1;
                        break;
                case KEY_PGDN:
                case Ctrl('F'):
                                        case ' ':
                                                        if (me.now < me.num - A_PAGESIZE)
                                                                me.now += A_PAGESIZE;
                                        else if (me.now < me.num - 1)
                                                me.now = me.num - 1;
                                        else
                                                me.now = 0;
                        break;
                case Ctrl('K'):
                                                y_lockscreen();
clear();
goto MACTEST;
                        break; //tdhlshx ¾«»ªÇøÓ©Ä»Ëø¶¨
                case Ctrl('C'):
                                                if (me.num==0)
                                                        break;
                        set_safe_record();
                        if (!HAS_PERM(PERM_POST))
                                break;
                        sprintf(fname, "%s/%s", path, me.item[me.now]->fname);
                        if (!dashf(fname))
                                break;
        if (me.now < me.num) {
                                char    bname[30];
                                clear();
                                if (get_a_boardname(bname, "ÇëÊäÈëÒª×ªÌùµÄÌÖÂÛÇøÃû³Æ: ")) {
                                        move(1, 0);
                                        if( deny_me(bname) ) {
                                                prints("\n\nÄúÉĞÎŞÈ¨ÏŞÔÚ %s ·¢±íÎÄÕÂ.", bname);
                                                pressreturn();
                                                me.page = 9999;
                                                break;
                                        }
                                        sprintf(tmp, "ÄãÈ·¶¨Òª×ªÌùµ½ %s °åÂğ", bname);
                                        if (askyn(tmp, NA, NA) == 1) {
                                                Postfile(fname, bname, me.item[me.now]->title, 0);
                                                move(3, 0);
                                                sprintf(tmp, "[1mÒÑ¾­°ïÄã×ªÌùÖÁ %s °åÁË[m", bname);
                                                prints(tmp);
                                                refresh();
                                                sleep(1);
                                        }
                                }
                                me.page = 9999;
                        }
                        show_message(NULL);
                        break;
                case 'M':
                        savemode = uinfo.mode;
                        m_new();
                        modify_user_mode(savemode);
                        me.page = 9999;
                        break;
#if 0

                case 'L':
                        savemode = uinfo.mode;
                        m_read();
                        modify_user_mode(savemode);
                        me.page = 9999;
                        break;
#endif

                case 'h':
                        show_help("help/announcereadhelp");
                        me.page = 9999;
                        break;
                case '\n':
                case '\r':
                        if (number > 0) {
                                me.now = number - 1;
                                number = 0;
                                continue;
                        }
                case 'R':
                case 'r':
                case KEY_RIGHT:
                        if (me.now < me.num) {
                                if (me.item[me.now]->host != NULL) {
                                        if (me.item[me.now]->fname[0] == '0') {
                                                if (get_con(me.item[me.now]->host, me.item[me.now]->port) != -1) {
                                                        char    tmpfile[30];
                                                        GOPHER  tmp;
                                                        extern GOPHER *tmpitem;

                                                        tmpitem = &tmp;
                                                        strcpy(tmp.server, me.item[me.now]->host);
                                                        strcpy(tmp.file, me.item[me.now]->fname);
                                                        sprintf(tmp.title, "0%s", me.item[me.now]->title);
                                                        tmp.port = me.item[me.now]->port;
                                                        enterdir(me.item[me.now]->fname);
                                                        setuserfile(tmpfile, "gopher.tmp");
                                                        savetmpfile(tmpfile);
                                                        ansimore(tmpfile, YEA);
                                                        unlink(tmpfile);
                                                }
                                        } else {
                                                gopher(me.item[me.now]->host, me.item[me.now]->fname,
                                                       me.item[me.now]->port, me.item[me.now]->title);
                                        }
                                        me.page = 9999;
                                        break;
                                } else
                                        sprintf(fname, "%s/%s", path, me.item[me.now]->fname);
                                if( dashf( fname ) ) {
                                        /* Efan: Èç¹û¸ÃpathÖ¸ÏòÉÏ¼¶Ä¿Â¼£¬ÔòÊÓÎª ·Ç·¨	*/
                                        if (strstr (fname, ".."))
                                                break;
                                        /* Leeward 98.09:ÔÚ¾«»ªÇøÔÄ¶ÁÎÄÕÂµ½Ä©Î² Ê±£¬ÓÃÉÏ£¯ÏÂ¼ıÍ·Ö±½ÓÌø×ªµ½Ç°£¯ºóÒ»Ïî */
                                        if(ansimore( fname, NA )==KEY_UP) {
                                                if(--me.now < 0)
                                                        me.now = me.num -1;
                                                ch = KEY_RIGHT;
                                                goto EXPRESS;
                                        }
                                        prints("[1m[44m[31m[ÔÄ¶Á¾«»ªÇø×ÊÁÏ]  [33m½áÊøQ, ¡û ©¦ ÉÏÒ»Ïî×ÊÁÏ U,¡ü©¦ ÏÂÒ»Ïî×ÊÁÏ <Enter>,<Space>,¡ı [m");
                                        switch( ch = egetch() ) {
                                        case KEY_DOWN:
                                        case ' ':
                                        case '\n':
                                                if ( ++ me.now >= me.num )
                                                        me.now = 0;
                                                ch = KEY_RIGHT;
                                                goto EXPRESS;
                                        case KEY_UP:
                                                //case 'u':
                                        case'U':
                                                if (-- me.now < 0 )
                                                        me.now = me.num - 1;
                                                ch = KEY_RIGHT;
                                                goto EXPRESS;
                                        case 'h':
                                                goto EXPRESS;
                                        default:
                                                break;
                                        }
                                } else if (dashd(fname)) {
                                        a_menu(me.item[me.now]->title, fname, me.level, bmonly);
                                }
                                me.page = 9999;
                        }
                        break;
               /* case '/': {
                                char  found[256];
                                if (a_menusearch(path, NULL, found)) {
                                        a_menu("", found,
                                               ((HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP)
                                                 || HAS_PERM(PERM_OBOARDS)) ? PERM_BOARDS : 0),0);
                                        me.page = 9999;
                                }
                        }
                        break;*/
                case 'u':
                        modify_user_mode(QUERY);
                        t_query();
                        me.page = 9999;
                        break;

                case Ctrl('A'):
                                                //		if (pm->num > 0)
                                                //		{
                                                //item = pm->item[pm->now];
                                                //sprintf(fpath, "%s/%s", pm->path, item->fname);
                                                sprintf(fname, "%s/%s", path, me.item[me.now]->fname);
                        //		}
                        a_addtofav(fname, me.item[me.now]->title);
                        break;

#ifdef INTERNET_EMAIL

                case 'F':
                case 'U':
        if (me.now < me.num && HAS_PERM(PERM_BASIC)) {
                                a_forward(path, me.item[me.now], ch == 'U');
                                me.page = 9999;
                        }
                        break;
#endif

                case '!':
                        if (!Q_Goodbye())
                                break;	/* youzi leave */
                }
                if (ch >= '0' && ch <= '9') {
                        number = number * 10 + (ch - '0');
                        ch = '\0';
                } else {
                        number = 0;
                }
                if (me.level & PERM_BOARDS)
                        a_manager(&me, ch);
                else if (ch == 'a' && HAS_PERM(PERM_POST)
                                && strstr(me.mtitle, "<GUESTBOOK>") == me.mtitle) {
                        a_newitem(&me, ADDITEM);
                } //add by yiyo ÁôÑÔ±¡
        }
        for (ch = 0; ch < me.num; ch++)
                free(me.item[ch]);
}

int linkto(char *path, char *fname, char *title)
{
        MENU    pm;
        pm.path = path;

        //strcpy(pm.mtitle, f_title);
        a_loadnames(&pm);
        if( pm.mtitle[0] == '\0' )
                strcpy(pm.mtitle, title);
        a_additem(&pm, title, fname, NULL, 0);
        a_savenames(&pm);
}

int add_grp(char group[STRLEN],char gname[STRLEN],char bname[STRLEN],char title[STRLEN])
{
        FILE   *fn;
        char    buf[PATHLEN];
        char    searchname[STRLEN];
        char    gpath[STRLEN * 2];
        char    bpath[STRLEN * 2];
        sprintf(buf, "0Announce/.Search");
        sprintf(searchname, "%s: groups/%s/%s", bname, group, bname);
        sprintf(gpath, "0Announce/groups/%s", group);
        sprintf(bpath, "%s/%s", gpath, bname);
        if (!dashd("0Announce")) {
                mkdir("0Announce", 0755);
                chmod("0Announce", 0755);
                if ((fn = fopen("0Announce/.Names", "w")) == NULL)
                        return;
                fprintf(fn, "#\n");
                fprintf(fn, "# Title=%s ¾«»ªÇø¹«²¼À¸\n", BoardName);
                fprintf(fn, "#\n");
                fclose(fn);
        }
        if (!seek_in_file(buf, bname))
                add_to_file(buf, searchname);
        if (!dashd("0Announce/groups")) {
                mkdir("0Announce/groups", 0755);
                chmod("0Announce/groups", 0755);

                if( (fn = fopen( "0Announce/groups/.Names", "w" )) == NULL )
                        return;
                fprintf( fn, "#\n" );
                fprintf( fn, "# Title=ÌÖÂÛÇø¾«»ª\n");
                fprintf( fn, "#\n" );
                fclose(fn);

                //linkto("0Announce", "ÌÖÂÛÇø¾«»ª","groups", "ÌÖÂÛÇø¾«»ª");
                linkto("0Announce", "groups", "ÌÖÂÛÇø¾«»ª");
        }
        if (!dashd(gpath)) {
                mkdir(gpath, 0755);
                chmod(gpath, 0755);
                sprintf( buf, "%s/.Names", gpath );
                if( (fn = fopen( buf, "w" )) == NULL )
                        return;
                fprintf( fn, "#\n" );
                fprintf( fn, "# Title=%s\n", gname);
                fprintf( fn, "#\n" );
                fclose(fn);
                //linkto("0Announce/groups", gname, group, gname);
                linkto("0Announce/groups", group, gname);
        }
        if (!dashd(bpath)) {
                mkdir(bpath, 0755);
                chmod(bpath, 0755);
                //linkto(gpath, title, bname, title);
                linkto(gpath, bname, title);
                sprintf(buf, "%s/.Names", bpath);
                if ((fn = fopen(buf, "w")) == NULL) {
                        return -1;
                }
                fprintf(fn, "#\n");
                fprintf(fn, "# Title=%s\n", title);
                fprintf(fn, "#\n");
                fclose(fn);
        }
        return 1;

}

int del_grp(char grp[STRLEN],char bname[STRLEN], char title[STRLEN])
{
        char    buf[STRLEN], buf2[STRLEN], buf3[30];
        char    gpath[STRLEN * 2];
        char    bpath[STRLEN * 2];
        char    check[30];
        int     i, n;
        MENU    pm;
        strncpy(buf3, grp, 29);
        buf3[29] = '\0';
        sprintf(buf, "0Announce/.Search");
        sprintf(gpath, "0Announce/groups/%s", buf3);
        sprintf(bpath, "%s/%s", gpath, bname);
        deltree(bpath);

        pm.path = gpath;
        a_loadnames(&pm);
        for (i = 0; i < pm.num; i++) {
                strcpy(buf2, pm.item[i]->title);
                strcpy(check, strtok(pm.item[i]->fname, "/~\n\b"));
                if (strstr(buf2, title) && !strcmp(check, bname)) {
                        free(pm.item[i]);
                        (pm.num)--;
                        for (n = i; n < pm.num; n++)
                                pm.item[n] = pm.item[n + 1];
                        a_savenames(&pm);
                        break;
                }
        }
}

int edit_grp(char bname[STRLEN], char grp[STRLEN],char title[STRLEN],char newtitle[100])
{
        char    buf[STRLEN], buf2[STRLEN], buf3[30];
        char    gpath[STRLEN * 2];
        char    bpath[STRLEN * 2];
        int     i;
        MENU    pm;
        strncpy(buf3, grp, 29);
        buf3[29] = '\0';
        sprintf(buf, "0Announce/.Search");
        sprintf(gpath, "0Announce/groups/%s", buf3);
        sprintf(bpath, "%s/%s", gpath, bname);
        if (!seek_in_file(buf, bname))
                return 0;

        pm.path = gpath;
        a_loadnames(&pm);
        for (i = 0; i < pm.num; i++) {
                strncpy(buf2, pm.item[i]->title, STRLEN);
                buf2[STRLEN - 1] = '\0';
                if (strstr(buf2, title) && strstr(pm.item[i]->fname, bname)) {
                        strncpy(pm.item[i]->title, newtitle, 72);
			pm.item[i]->title[71] = '\0';
                        break;
                }
        }
        a_savenames(&pm);
        pm.path = bpath;
        a_loadnames(&pm);
        strncpy(pm.mtitle, newtitle, STRLEN);
	pm.mtitle[STRLEN - 1] = '\0';
        a_savenames(&pm);
}

int AddPCorpus()
{
        FILE *fn;
        //	char	digestpath[80] = "0Announce/groups/GROUP_0/PersonalCorpus";
        char    digestpath[80] = "0Announce/PersonalCorpus";
        char    personalpath[80], Log[200], title[200], ftitle[80];
        time_t	now;
        int     id;
        modify_user_mode(DIGEST);
        sprintf(Log,"%s/Log",digestpath);
        if (!check_systempasswd()) {
                return 1;
        }
        clear();
        stand_title("´´½¨¸öÈËÎÄ¼¯");

        move(4,0);
        if(!gettheuserid(1,"ÇëÊäÈëÊ¹ÓÃÕß´úºÅ: ",&id))
                return 1;

        sprintf(personalpath,"%s/%c/%s",digestpath,
                toupper(lookupuser.userid[0]),lookupuser.userid);
        if (dashd(personalpath)) {
                presskeyfor("¸ÃÓÃ»§µÄ¸öÈËÎÄ¼¯Ä¿Â¼ÒÑ´æÔÚ, °´ÈÎÒâ¼üÈ¡Ïû..",4);
                return 1;
        }

        move(4,0);
        if(askyn("È·¶¨ÒªÎª¸ÃÓÃ»§´´½¨Ò»¸ö¸öÈËÎÄ¼¯Âğ?",YEA,NA)==NA) {
                presskeyfor("ÄãÑ¡ÔñÈ¡Ïû´´½¨. °´ÈÎÒâ¼üÈ¡Ïû...",6);
                return 1;
        }
        mkdir(personalpath, 0755);
        chmod(personalpath, 0755);

        move(7,0);
        prints("[Ö±½Ó°´ ENTER ¼ü, Ôò±êÌâÈ±Ê¡Îª: [32m%s µÄ¸öÈËÎÄ¼¯[m]",lookupuser.userid);
        getdata(6, 0, "ÇëÊäÈë¸öÈËÎÄ¼¯Ö®±êÌâ: ", title, 39, DOECHO, YEA);
        if(title[0] == '\0')
                sprintf(title,"%s µÄ¸öÈËÎÄ¼¯",lookupuser.userid);
        sprintf(title, "%-38.38s(BM: %s)", title, lookupuser.userid);
        sprintf(digestpath,"%s/%c",digestpath,toupper(lookupuser.userid[0]));
        sprintf(ftitle,"¸öÈËÎÄ¼¯ £­£­ (Ê××ÖÄ¸ %c)", toupper(lookupuser.userid[0]));
        //linkto(digestpath, ftitle, lookupuser.userid, title);
        linkto(digestpath, lookupuser.userid, title);
        sprintf(personalpath, "%s/.Names", personalpath);
        if ((fn = fopen(personalpath, "w")) == NULL) {
                return -1;
        }
        fprintf(fn, "#\n");
        fprintf(fn, "# Title=%s\n", title);
        fprintf(fn, "#\n");
        fclose(fn);
        if(!(lookupuser.userlevel & PERM_PERSONAL)) {
                char    secu[STRLEN];
                lookupuser.userlevel |= PERM_PERSONAL;
                substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
                //                sprintf(secu, "½¨Á¢¸öÈËÎÄ¼¯, ¸øÓè %s °åÖ÷È¨ÏŞ", lookupuser.userid);
                sprintf(secu, "´´½¨¸öÈËÎÄ¼¯, ¸øÓè %s ÎÄ¼¯¹ÜÀíÈ¨ÏŞ", lookupuser.userid);
                securityreport(secu);
                move( 9,0);
                prints(secu);
                sethomefile(genbuf,lookupuser.userid,".announcepath");
                report(genbuf);
                if(fn = fopen(genbuf,"w+")) {
                        fprintf(fn,"%s/%s",digestpath,lookupuser.userid);
                        fclose(fn);
                }

        }
        now = time(NULL);
        /* thunder Îªwww·½±ã²»Ê¹ÓÃÑÕÉ«
        	getdatestring(now,NA);
        sprintf(genbuf,"[36m%-12.12s[m %14.14s [32m %.38s[m",
        		lookupuser.userid, datestring,title);
               */
        sprintf(genbuf,"%-12.12s    %14.14s    %.38s",
                lookupuser.userid, datestring,title);
        add_to_file(Log,genbuf);
        presskeyfor("ÒÑ¾­´´½¨¸öÈËÎÄ¼¯, Çë°´ÈÎÒâ¼ü¼ÌĞø...",12);
        return 0;
}
void Announce()
{
        sprintf(genbuf, "%s ¾«»ªÇø¹«²¼À¸", BoardName);
        a_menu(genbuf, "0Announce", (HAS_PERM(PERM_ANNOUNCE) || HAS_PERM(PERM_SYSOP)) ? PERM_BOARDS : 0, 0);
        clear();
}


void digesttitle()
{
        showtitle("¾«»ªÇøÊÕ²Ø  ", BoardName);
        prints("Àë¿ª[[1;32m¡û[m,[1;32me[m]  ÔÄ¶Á¾«»ª[[1;32m¡ú[m,[1;32mRtn[m]  É¾³ı¾«»ªÇøÁ´½Ó[[1;32md[m]  ĞŞ¸ÄÁ´½Ó±êÌâ[[1;32mT[m]  ÇóÖú[[1;32mh[m][m\n");
        prints("[1;44m ±àºÅ %-12s%-12s %-38s%8s[m\n", "      ", "°åÃæ/×÷Õß", "±ê  Ìâ","¸üĞÂÈÕÆÚ");
        clrtobot();
}

char   * digestdoent(int num, struct favdigest *ent)
{
        static char buf[512];
        char buf2[256];
        char path[256];
        char title[80];
        char type1[6], type2[6], owner[20];
        int iscorpus=0;
        int tmp=0;
      //  char *p;
        strcpy(path, ent->path);
        strcpy(title, ent->title);
        if(strstr(path, "PersonalCorpus"))
                strcpy(type1, "[0;37mÎÄ¼¯[1;37m");
        else
                strcpy(type1, "¾«»ª");

        time_t mtime;
        struct stat st;

        if(dashd(path))
        {
                strcpy(type2, "[33mÄ¿Â¼[37m");
                stat(path, &st);
                mtime = st.st_mtime;
        } else if(dashf(path))
        {
                strcpy(type2, "[36mÎÄ¼ş[37m");
                stat(path, &st);
                mtime = st.st_mtime;
        } else
        {
                mtime=time(0);
                strcpy(type2, "[32m´íÎó[37m");
        }
        if (strstr (path, ".."))
                strcpy (type2, "\33[32m·Ç·¨\33[32m");
        if(strstr(path, "0Announce/groups/"))
        {
                strcpy(buf2, path+24);
                tmp=0;
                while(buf2[0]!='/'&&tmp<10) {
                        strcpy(buf2, buf2+1);
                        tmp++;
                }
                strtok(buf2+1, "/");
                strcpy(owner, buf2+1);

        } else if (strstr(path, "PersonalCorpus")&&strlen(path)>28&&path[26]!='.')
        {
                iscorpus=1;

                strcpy(buf2, path+27);
               /* if(buf2[0]=='/')
                        strcpy(owner, buf2+1);
                else*/
                char *p=strstr(buf2,"/");
                if(p) *p='\0';
                strcpy(owner,buf2);
                 
               // p=strtok(owner,"/");


        } else
                strcpy(owner, " ");

        struct tm *pt;
        pt = localtime(&mtime);
        char ftime[16];
        sprintf(ftime, "%04d.%02d.%02d",pt->tm_year+1900, pt->tm_mon + 1, pt->tm_mday);
        ent->title[38]=0;
        sprintf(buf, " %4d [%s:%s] %s%-12s%s %-38s%8s", num, type1, type2, iscorpus?"[0;37m":"", owner,iscorpus?"[1;37m":"", ent->title,ftime);
        return buf;
}
int fav_read(int ent,struct favdigest *fd,char* direct)
{
        int     ch;
        if(dashf(fd->path)) {
                if (strstr (fd->path, ".."))
                        return 1;
                ansimore(fd->path);
                //		move(t_lines-1, 0);
                prints("[1m[44m[31m[ÔÄ¶Á¾«»ªÇø×ÊÁÏ]  [33m½áÊøQ, ¡û ©¦ ÉÏÒ»Ïî×ÊÁÏ U,¡ü©¦ ÏÂÒ»Ïî×ÊÁÏ <Enter>,<Space>,¡ı [m");
                ch=egetch();
                return 1;
        } else if(dashd(fd->path)) {
                a_menu(fd->title, fd->path, 0, 0);
        }
        return 1;
}

int fav_del(int ent,struct favdigest* fd,char* direct)
{
        char currdigestdir[256];
        sprintf(currdigestdir, "home/%c/%s/digest",  toupper(currentuser.userid[0]), currentuser.userid);
        if (askyn("È·¶¨É¾³ı", NA, YEA) == NA) {
                move(t_lines - 1, 0);
                prints("·ÅÆúÉ¾³ı...");
        } else {
                delete_record(currdigestdir, sizeof(struct favdigest), ent);
        }
        return 1;
}
int fav_changeT(int ent,struct favdigest* fd,char* direct)
{
        char buf[256];
        strcpy(buf,fd->title);
        strcpy(buf, rtrim(buf));
        getdata(t_lines - 1, 0, "ĞÂ±êÌâ: ", buf, 50, DOECHO, NA);
        //	if(!strcmp(buf,fd->title)) return 1;
        check_title(buf);
        strcpy(fd->title, buf);
        substitute_record(direct, fd, sizeof(struct favdigest), ent);
        return 1;
}

int fav_move(int ent,struct favdigest* fd,char* direct)
{
        char num[8], currdigestdir[256];
        int new_pos, fail;
        sprintf(currdigestdir, "home/%c/%s/digest",
                toupper(currentuser.userid[0]), currentuser.userid);
        move(t_lines-2,0);
        clrtoeol();
        move(t_lines-3,0);
        clrtoeol();
        getdata(t_lines-3,0,"ÒÆ¶¯ÖÁ±àºÅ: ",num,6,DOECHO,YEA) ;
        new_pos = atoi(num);
        if(new_pos <= 0) {
                move(t_lines-2,0);
                clrtoeol();
                prints("´íÎó±àºÅ!") ;
                pressreturn() ;
                return FULLUPDATE ;
        }
        if(new_pos == ent) {
                move(t_lines-2,0);
                clrtoeol();
                prints("±£³Ö´ÎĞò²»±ä!");
                pressreturn();
                return FULLUPDATE ;
        }
        move(t_lines-2,0);
        clrtoeol();
        if(askyn("È·¶¨ÒÆ¶¯?",NA,NA) == YEA) {
                fail = move_file(currdigestdir,sizeof(struct favdigest),ent,new_pos);
        }
        if(fail == -2) {
                move(t_lines-2,0);
                clrtoeol();
                prints("´íÎó±àºÅ!") ;
                pressreturn() ;
                return FULLUPDATE ;
        }
        if(fail == -1) {
                move(t_lines-2,0);
                clrtoeol();
                prints("ÒÆ¶¯Ê§°Ü!") ;
                pressreturn();
                clear();
        }

        return NEWDIRECT;
}

int fav_help()
{
        show_help("help/favdigesthelp");
        return FULLUPDATE;
}
struct one_key digest_comms[] =
        {
                KEY_RIGHT, fav_read,
                //	'\n', fav_read,
                'r', fav_read,
                'd', fav_del,
                'T', fav_changeT,
                'h', fav_help,
                '>', fav_move,
                Ctrl('K'), y_lockscreen,//tdhlshx add
                '\0', NULL
        };

int FavDigest()
{
        sprintf(currdigestdir, "home/%c/%s/digest", toupper(currentuser.userid[0]), currentuser.userid);
        if(!strcmp(currentuser.userid,"guest"))
                return 0;
	i_read(DIGEST, currdigestdir, digesttitle, digestdoent, digest_comms, sizeof(struct favdigest));
	return 0;
}

int DelPCorpus()
{
	char digestpath[80] = "0Announce/PersonalCorpus";
	char personalpath[80], Log[200], parentpath[80];
	MENU menu;
	int i;
	modify_user_mode(DIGEST);
	clear();
	move(4,0);
	sprintf(personalpath,"%s/%s/%c/%s",BBSHOME, digestpath,
			toupper(currentuser.userid[0]), currentuser.userid);
	if(!dashd(personalpath))
	{
		 presskeyfor("¸ÃÓÃ»§µÄ¸öÈËÎÄ¼¯Ä¿Â¼²»´æÔÚ, °´ÈÎÒâ¼üÈ¡Ïû..",4);
         return 1;
	}
	if(askyn("ÄúÈ·¶¨ÒªÉ¾³ıÎÄ¼¯Âğ£¿",NA,NA) == NA)
		return 1;
	clear();
	move(0,0);
	deltree(personalpath);
	sprintf(parentpath,"%s/%c",digestpath, toupper(currentuser.userid[0]));
	menu.path = parentpath;
	a_loadnames(&menu);
	for(i = 0;i < menu.num; i ++)
	{
		if(!strcmp(menu.item[i]->fname, currentuser.userid ))
		{
			menu.now = i;
			break;
		}
	}
	a_delete(&menu);
	a_savenames(&menu);
	if(currentuser.userlevel & PERM_PERSONAL)
		currentuser.userlevel &= ~PERM_PERSONAL;
	substitute_record(PASSFILE, &currentuser, sizeof(struct userec), usernum);
	sprintf(Log,"É¾³ı¸öÈËÎÄ¼¯£¬È¡Ïû%sµÄÎÄ¼¯¹ÜÀíÈ¨ÏŞ", currentuser.userid);
	securityreport(Log);
	return 0;
}

int ChangePCName()
{
	char digestpath[80] = "0Announce/PersonalCorpus";
	char personalpath[80], Log[200], parentpath[80], title[80];
	MENU parentmenu, menu;
	int i;
	modify_user_mode(DIGEST);
	clear();
	move(4,0);
	sprintf(personalpath,"%s/%c/%s",digestpath,
			toupper(currentuser.userid[0]), currentuser.userid);
	sprintf(genbuf, "%s/%s", BBSHOME, personalpath);
	if(!dashd(genbuf))
	{
		 presskeyfor("¸ÃÓÃ»§µÄ¸öÈËÎÄ¼¯Ä¿Â¼²»´æÔÚ, °´ÈÎÒâ¼üÈ¡Ïû..",4);
         return 1;
	}
	getdata(6, 0, "ÇëÊäÈë¸öÈËÎÄ¼¯Ö®±êÌâ: ", title, 39, DOECHO, YEA);
	if(title[0] == '\0')
		return 1;
	sprintf(parentpath,"%s/%c",digestpath, toupper(currentuser.userid[0]));
	parentmenu.path = parentpath;
	a_loadnames(&parentmenu);
	for(i = 0;i < parentmenu.num; i ++)
	{
		if(!strcmp(parentmenu.item[i]->fname, currentuser.userid ))
		{
			sprintf(parentmenu.item[i]->title,"%s", title);
			break;
		}
	}
	a_savenames(&parentmenu);
	menu.path = personalpath;
	a_loadnames(&menu);
	sprintf(menu.mtitle, "%-38.38s(BM: %s)", title, currentuser.userid);
	a_savenames(&menu);
	sprintf(Log,"%s ½«¸öÈËÎÄ¼¯¸ÄÃûÎª %s", currentuser.userid, title);
	securityreport(Log);
	return 0;
}

void dig_user_switchcase(char* newuserid){
        char digestpath[80] = "0Announce/PersonalCorpus";
        char Log[200], parentpath[80], personalpath[80],title[80];
        MENU parentmenu, menu;
        int i;
        sprintf(genbuf, "%s/%s/%c/%s", BBSHOME, digestpath,
                        toupper(currentuser.userid[0]),currentuser.userid);
        if(!dashd(genbuf))
                return;
        sprintf(genbuf, "mv -f %s/%s/%c/%s %s/%s/%c/%s",
                        BBSHOME, digestpath, toupper(currentuser.userid[0]), currentuser.userid,
                        BBSHOME, digestpath, toupper(newuserid[0]), newuserid);
        system(genbuf);
        sprintf(parentpath,"%s/%c",digestpath, toupper(currentuser.userid[0]));
        parentmenu.path = parentpath;
        a_loadnames(&parentmenu);
        for(i = 0;i < parentmenu.num; i ++)
        {    
                if(!strcmp(parentmenu.item[i]->fname, currentuser.userid))
                {    
                        strcpy(parentmenu.item[i]->fname, newuserid);
                        break;
                }    
        }    
        a_savenames(&parentmenu);
        sprintf(personalpath,"%s/%s", parentpath, newuserid);
        sprintf(title,"%sµÄ¸öÈËÎÄ¼¯", newuserid);
        menu.path = personalpath;
        a_loadnames(&menu);
        sprintf(menu.mtitle, "%-38.38s(BM: %s)", title, newuserid);
        a_savenames(&menu);
}
