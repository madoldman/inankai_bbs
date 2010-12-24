/*
 * c_express.c		-- express service side function and UI
 *	
 * A part of BBS Express Project
 *
 * Copyright (c) 1998, 1999, Edward Ping-Da Chuang <edwardc@edwardc.dhs.org>
 * of BBS Express Project, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * CVS: $Id: c_express.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
 */

#include "bbs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

char c_express_c[] = 
  "$Id: c_express.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $";
  
char *substr();
#define MMCODE

/*
        i think base64 will more reliable than uudecode, or i can 
        do a CRC check line by line ...
*/

#ifndef BBSHOME
  #define BBSHOME     "/service/bbs"
#endif

#define VERID   "BES 1.01"
#define VERSION "1.01 (standalone)"
#define RARAV

/*
        enable RARAV feel free, the rar for unix won't be check your
        register information
*/

struct taged {
        int             tag;    /* 0 is Y , 1 is N */
        char            status;
        char            filename[STRLEN];       /* the DIR files */
        char            owner[STRLEN];
        char            title[STRLEN];
        struct          fileheader fh;
};

struct bessruc {
        char            id[10]; /* VERID */
        int             compress;       /* 0 is tgz 1 is zip 2 is rar 3 ... */
        int             keepmail;       /* YEA is 要保留 NA 不保留 */
        int             mail;   /* 是否抓 mail (预设 是) */
        int             group;  /* default: none */
        int             maxget;
};

struct indexsrc {
        char            boardname[STRLEN];
        char            filename[STRLEN];
        char            owner[STRLEN];
        char            title[STRLEN];
        char            tag;
};

struct grpsrc {
        char            boardname[STRLEN];
        char            tag[10];
};


struct bessruc  bes;
struct taged    tag[1024];
struct grpsrc   grp[128];

FILE           *gfp;
static int             gnum = -1, grpnum = -1;
char           *msg, *cmd;

#define Limit

#ifdef Limit

int             last_size = -1; /* 如果 .INDEX 的大小相同, 就不允许再抓 */
time_t          last_use;
#define BETWEEN (30)            /* 三分钟後才准再抓 */

#endif

#define MAXGET	((int)300)

int
express(char *userid)
{
        b_main();
}

int
b_main()
{

        sprintf(genbuf, "200 Welcome to use Express Ver %s , %s . ", VERSION, currentuser.userid);
        outs(genbuf);
        sprintf(genbuf, "%s/home/%c/%s/express.config", BBSHOME, toupper(currentuser.userid[0]), currentuser.userid);
        gfp = fopen(genbuf, "r");
        if (gfp == NULL) {
                sprintf(genbuf, "501 cannot find configure file. you may create new one");
                outs(genbuf);
        } else {
                dumpsetting(0); /* 列出设定并载入 */
        }
}

int
dumpsetting(int recursive)
{
        if (recursive == 0)
                fread(&bes, sizeof(bes), 1, gfp);

        if (strcmp(bes.id, "") == 0) {
                outs("503 unknown configure file format, no id found.");
                Fatal();
        } else if (strcmp(bes.id, VERID) != 0) {
                sprintf(genbuf, "502 configure file is conflict with system version");
                outs(genbuf);
                Fatal();
        } else {
                readDIR();
        }

}

int
b_create()
{

        FILE           *fp;
        char            file[256];
        char            g1[5] = "";
        char            g2[5] = "";
        char            g3[5] = "";
        char            g4[5] = "";
        int             i = 0;

        if (strlen(msg) > 23) {
                outs("531 command is too long, fatal.");
                Fatal();
        }
        sscanf(msg, "%s %s %s %s", g1, g2, g3, g4);

        if (strcmp(g1, "") != 0)
                i++;
        if (strcmp(g2, "") != 0)
                i++;
        if (strcmp(g3, "") != 0)
                i++;
        if (strcmp(g4, "") != 0)
                i++;

        if (i != 4) {
                outs("512 not enough/correct prarmater number.");
                return NA;
        }
        sprintf(file, "%s/home/%c/%s/express.config", BBSHOME, toupper(currentuser.userid[0]), currentuser.userid);
        fp = fopen(file, "w");

        if (fp == NULL)
                outs("open failure");

        strcpy(bes.id, VERID);

        switch (g1[0]) {

        case '1':
                bes.compress = 1;
                break;
        case '2':
                bes.compress = 2;
                break;
        case '3':
                bes.compress = 3;
                break;
        default:
                outs("513 param 1 error, giving up.");
                fclose(fp);
                unlink(file);
                return -1;
        }

        switch (g2[0]) {

        case 'y':
        case 'Y':
                bes.keepmail = YEA;
                break;
        case 'N':
        case 'n':
                bes.keepmail = NA;
                break;
        default:
                outs("513 param 2 error, giving up.");
                fclose(fp);
                unlink(file);
                return -1;
        }

        switch (g3[0]) {

        case 'y':
        case 'Y':
                bes.mail = YEA;
                break;
        case 'N':
        case 'n':
                bes.mail = NA;
                break;
        default:
                outs("513 param 3 error, giving up.");
                fclose(fp);
                unlink(file);
                return -1;
        }

        switch (g4[0]) {

        case 'y':
        case 'Y':
                bes.group = YEA;
                break;
        case 'N':
        case 'n':
                bes.group = NA;
                break;
        default:
                outs("513 param 4 error, giving up.");
                fclose(fp);
                unlink(file);
                return -1;
        }

        fwrite(&bes, sizeof(bes), 1, fp);
        fclose(fp);
        outs("251 configure create/change successfully.");
        readDIR();              /* re-new it */
        return YEA;
}


int
readDIR()
{
        FILE           *dir;
        struct fileheader fh;
        int             num = 0;

        sprintf(genbuf, "%s/mail/%c/%s/.DIR", BBSHOME, toupper(currentuser.userid[0]), currentuser.userid);
        dir = fopen(genbuf, "r");

        while (fread(&fh, sizeof(fh), 1, dir) != NULL && !feof(dir)) {

                strcpy(tag[num].filename, fh.filename);
                tag[num].tag = 0;
                strcpy(tag[num].owner, fh.owner);
                strcpy(tag[num].title, fh.title);
                if (fh.accessed[0] & FILE_READ) {
                        if (fh.accessed[0] & FILE_MARKED)
                                tag[num].status = 'm';
                        else
                                tag[num].status = ' ';
                } else {
                        if (fh.accessed[0] & FILE_MARKED)
                                tag[num].status = 'M';
                        else
                                tag[num].status = 'N';
                }

                strcpy(tag[num].fh.filename, fh.filename);
                strcpy(tag[num].fh.owner, fh.owner);
                strcpy(tag[num].fh.title, fh.title);
                tag[num].fh.level = fh.level;
                strcpy(tag[num].fh.accessed, fh.accessed); 
                num++;

        }
        gnum = num;
        sprintf(genbuf, "111 %d data has been re-new", gnum);
        outs(genbuf);
}

int
dumplist(char *filename)
{
        int             i, match = 0;

        for (i = 0; i < gnum; i++) {
                sprintf(genbuf, "110 %s,%c,%d,%s,%s,%s", (tag[i].tag == 1) ? "T" : "x", ( tag[i].status == ' ' ) ? 'x' : tag[i].status , i + 1, tag[i].owner, tag[i].title,
                        tag[i].filename);
                outs(genbuf);
                match++;
        }

        /* edwardc.990129 minior bug fixed */
        sprintf(genbuf, "211 total %d data listed", match);
        outs(genbuf);
        outs("999 .");
}

int
tagmail()
{
        int             i, tag1, tag2;
        char            a[10], b[10];

        sscanf(msg, "%s %s", a, b);
        tag1 = atoi(a);
        tag2 = atoi(b);

        /* edwardc.981104 1~100 到底是 100 还是 99 ? */

        if (tag1 == tag2) {
                tag1--;
                tag[tag1].tag = 1;
                sprintf(genbuf, "200 mail number %d tagged", tag2);
                outs(genbuf);
        } else if (tag2 > tag1) {
                tag1--;
                for (i = tag1; i < tag2; i++)
                        tag[i].tag = 1;
                sprintf(genbuf, "200 mail number %d to %d tagged", tag1 + 1, tag2);
                outs(genbuf);
        } else if (tag1 > tag2) {
                tag2--;
                for (i = tag2; i < tag1; i++)
                        tag[i].tag = 1;
                sprintf(genbuf, "200 mail number %d to %d tagged", tag2 + 1, tag1);
                outs(genbuf);
        } else {
                outs("555 oops?");
        }

        return 0;
}

int
tagnew()
{
        int             i, match = 0;
        char		buf[20], buf2[10];

        if ( *cmd == 0 ) {
                /* no specify the lastread, will assume the N record */
        	strcpy(buf2, "0");
        	
                for (i = 0; i < gnum; i++) {
                        if (tag[i].status == 'N' || tag[i].status == 'M') {
                        	strcpy(buf2, substr(tag[i].filename, 2, 11));
                        	outs(tag[i].filename);
                                tag[i].tag = 1;
                                match++;
                        }
                }
        
        } else {
                long a;
                
                if ( strlen(cmd) > 9 ) {
                	outs("510 oops? you're so big.");
                	Fatal();
                }
                
		strcpy(buf2, cmd);
		                
                a = atol(cmd);
                for ( i = 0 ; i < gnum ; i++ ) {
                	strcpy(buf, substr(tag[i].filename, 2, 11));
                        if ( atol(buf) > (long) a ) {
				strcpy(buf2, substr(tag[i].filename, 2, 11));
                                tag[i].tag = 1;
                                match++;
                        }
                }
                        
        }
        
        sprintf(genbuf, "210 %d %s", match, buf2);
        outs(genbuf);
}

int
viewtagged()
{
        int             match = 0, i;

        for (i = 0; i < gnum; i++) {

                if (tag[i].tag == 1) {
                        sprintf(genbuf, "110 %s,%s,%d,%s,%s,%s", (tag[i].tag == 1) ? "T" : " ", tag[i].status, i + 1, tag[i].owner, tag[i].title,
                                tag[i].filename);
                        outs(genbuf);
                        match++;
                }
        }
        sprintf(genbuf, "211 total %d tagged");
        outs(genbuf);
}

int
compress()
{
        char            command[100], mtime[10], dir[100];
        char            compress[100], package[30];
        char            fullpath[200];
        int             i = 1, match = 0;
        FILE           *index, *pgm;
        struct indexsrc savebuf;
        struct stat     st;

	if ( gnum == 0 && grpnum < 0 ) {
		outs("320 nothing to pack");
		return;
	}

        outs("311 retr command accepted it's under process.");

        chdir(BBSHOME);
        sprintf(mtime, "%d", time(0));

        /* edwardc.990109 使用内建函数比较好用 ^_^ */

        outs("260 cleanning dir..");
        sprintf(command, "rm -fr tmp/express.%s.*", currentuser.userid);
        system(command);
        sprintf(dir, "tmp/express.%s.%s", currentuser.userid, mtime);
        mkdir(dir, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        strcpy(fullpath, dir);
        strcat(fullpath, "/");

        
        outs("261 copying mail.");
        for (i = 0; i < gnum; i++) {
                if (tag[i].tag == 1) {
                        sprintf(compress, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, tag[i].filename);
                        if (stat(compress, &st) >= 0 && st.st_size > (off_t) 0) {
                                sprintf(genbuf, " cp mail/%c/%s/%s %s/ ", toupper(currentuser.userid[0]), currentuser.userid, tag[i].filename, dir);
                                pgm = popen(genbuf, "r");
                                pclose(pgm);
                                match++;
                        }
                }
        }

        if (bes.group == 1 && grpnum != -1 ) {
                outs("264 copying group article.");
        
                for ( i = 0 ; i < grpnum+1 ; i++ ) {
                        if ( strcmp(grp[i].boardname, "EOF") == 0 )
                                break;
                        match = cppost(dir, grp[i].boardname, atol(grp[i].tag));
                        makeindex(grp[i].boardname, atol(grp[i].tag), i, dir);
                }
        
                sprintf(compress, "%s/M.LIST.A", dir);
                index = fopen(compress, "a");
                fprintf(index, "[General]\n");
                fprintf(index, "Count=%d\n", i - 1);
                fprintf(index, "Build=%d\n\n", time(0));
                fclose(index);
        }

        sprintf(genbuf, "%s/M.INDEX.A", dir);
        index = fopen(genbuf, "ab");

        i = 1;

        outs("262 making the index.");

        for (i = 0; i < gnum; i++) {
                if (tag[i].tag == 1) {
                        strcpy(savebuf.boardname, "_MAIL_");
                        strcpy(savebuf.filename, tag[i].filename);
                        strcpy(savebuf.owner, tag[i].owner);
                        strcpy(savebuf.title, tag[i].title);
                        savebuf.tag = tag[i].status;
                        fwrite(&savebuf, sizeof(savebuf), 1, index);
                }
        }

        fclose(index);
        
#ifdef Limit
        stat(genbuf, &st);

        if (last_size == (off_t) - 1) {
                last_size = st.st_size;
                last_use = time(0);
        } else {
                if (last_size == st.st_size && difftime(time(0), last_use) <= BETWEEN) {
                        outs("519 the same package .. transfeer denied.");
                        return -1;
                } else {
                        last_use = time(0);
                }
        }

#endif
        sprintf(genbuf, "%s/M.INDEX.A", dir);
        chmod(genbuf, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        sprintf(genbuf, "%s/M.LIST.A", dir);
        chmod(genbuf, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        sprintf(package, "%s.%s.", currentuser.userid, mtime);
        
/* edwardc.990206 just support RAR beco's client support it :) */

#if 0
        if (bes.compress == 1) {
                strcat(package, "tar.gz");
                sprintf(compress, "bin/tar cplfsz %s/%s %s/M.*.A %s/.INDEX", dir, package, dir, dir);
        } else if (bes.compress == 2) {
                strcat(package, "zip");
                sprintf(compress, "bin/zip -9 %s/%s %s/M.*.A %s/.INDEX ", dir, package, dir, dir);
        } else if (bes.compress == 3) {
#endif  

                strcat(package, "rar");
#ifdef RARAV
                sprintf(compress, "bin/rar a -m5 -av %s/%s %s/M.*.A ", dir, package, dir);
#else
                sprintf(compress, "bin/rar a -m5 %s/%s %s/%s/M.*.A ", dir, package, dir);
#endif

#if 0
        }
#endif

        strcat(fullpath, package);

        outs("263 compressing.");
        system(compress);

        /* edwardc.990115 mmencode(base64) implemention */

#ifdef MMCODE
        sprintf(command, "tmp/express.%s.%s/%s.uue", currentuser.userid, mtime, currentuser.userid);
        mm(1, fullpath, command);
#else
        sprintf(command, "%s %s %s > tmp/express.%s.%s/%s.uue", UUEN, fullpath, package, currentuser.userid, mtime, currentuser.userid);
        system(command);
#endif

        sprintf(command, "tmp/express.%s.%s/%s.uue", currentuser.userid, mtime, currentuser.userid);

        if (stat(command, &st) >= 0) {
                sprintf(compress, "300 %s %d ", package, st.st_size);
                outs(compress);
                pgm = fopen(command, "r");
#ifdef MMCODE
                while (fgets(fullpath, 74, pgm) != NULL) {
                        oouts(120, fullpath, 1);
                }
#else
                while (fgets(fullpath, 63, pgm) != NULL) {
                        oouts(120, fullpath, 1);
                }
#endif          
                fclose(pgm);
                outs("120 .");
                outs("240 Transfeer complete.");
        } else {
                outs("540 Cannot transfeer.");
        }

}

int
showmd5(char *filename)
{
        struct stat     st;

        if (stat(filename, &st) < 0)
                return -1;
        else {
                if (st.st_mtime != last_use) {
                        last_use = st.st_mtime;
                        return 0;
                } else
                        return 0;
        }

        return 0;

}

int
boardlist()
{
        FILE *fp;
        struct boardheader bh;
        struct stat st;
        char xxx[256], sun2[256];
        int i, ch;
        
        /* edwardc.990130 奇人异事 :p .. 
	   sun 在 solaris/SunOS 上面是个保留字 ..
           sun 这个名字对我来说蛮有纪念价值的, 我看他现在可能是个保留字 :pp
           (FreeBSD 底下没问题 -_- ..)
        */

        sprintf(genbuf,"%s/%s",BBSHOME,BOARDS);
        fp = fopen(genbuf, "r");
        
        if ( fp == NULL ) {
                outs("511 boards read error");
                return -1;
        }

        i = 1;
        
        stat(genbuf, &st);
        
        sprintf(xxx, "250 %d",st.st_mtime);
        outs(xxx);
        
        while ( fread(&bh, sizeof(bh), 1, fp) != NULL ) {
                xxx[0] = sun2[0] = '\0';
                if( !(bh.level & PERM_POSTMASK) && !HAS_PERM(bh.level) )
                        continue;
                /* 220 num,bn,scr,bm,outgoing */
                strcpy(sun2, substr(bh.title, 11, strlen(bh.title)));
                if ( strstr(bh.title, "●") || strstr(bh.title, "⊙") )
                        ch = 'Y';
                else
                        ch = 'N';
                        
                sprintf(xxx,"111 %d,%s,%s,%s,%c",i, bh.filename, sun2, ( bh.BM[0] == '\0' ) ? "N/A" : bh.BM, ch);
                outs(xxx);
                i++;
        }
        
        sprintf(xxx, "221 total %d data listed.", i-1);
        outs(xxx);
        
        fclose(fp);
        
}

char *
substr(char *str, int from, int to)
{
        char ch[3];
        static char buf[256];
        int i;
        
        if ( strlen(str) > sizeof(buf) )
                return "EOF";
        
        buf[0] = '\0';
        for ( i = from ; i < to ; i++ ) {
                sprintf(ch, "%c", str[i]);
                strcat(buf, ch);
        }
        
        return buf;
}

void
statblist()
{
        struct stat st;
        
        sprintf(genbuf,"%s/.BOARDS",BBSHOME);
        
        if ( stat(genbuf, &st) < 0 ) {
           outs("221 0");
        } else {
           sprintf(genbuf,"221 %d",st.st_mtime);
           outs(genbuf);
        }
}

int
makeindex(char *bname, long tag, int inic, char *dir)
{
        struct fileheader fh;
        struct indexsrc is;
        FILE           *fp, *fp2, *fp3;
        int             a = 0;
        char            xxx[10];

        sprintf(genbuf, "boards/%s/.DIR", bname);
        fp = fopen(genbuf, "r");
        sprintf(genbuf, "%s/M.INDEX.A",dir);
        fp2 = fopen(genbuf, "ab");

        while (fread(&fh, sizeof(fh), 1, fp) != 0 && !feof(fp)) {
                strcpy(xxx, substr(fh.filename, 2, 11));
                if (atol(xxx) > tag && fh.owner[0] != '-' && a < (int)MAXGET ) {
                        strcpy(is.boardname, bname);
                        strcpy(is.filename, fh.filename);
                        strcpy(is.owner, fh.owner);
                        strcpy(is.title, fh.title);

			/* edwardc.990206 因为之前有判断, 所以 tag 都是未读的 */
	
                        if (fh.accessed[0] & FILE_MARKED) {
                                if (fh.accessed[0] & FILE_DIGEST)
                                        is.tag = 'B';
                                else
                                        is.tag = 'M';
                        } else {
                                if (fh.accessed[0] & FILE_DIGEST)
                                        is.tag = 'G';
                                else
                                        is.tag = 'N';
                        }

                        fwrite(&is, sizeof(is), 1, fp2);
                        a++;
                }
        }

        fclose(fp);
        fclose(fp2);
        sprintf(genbuf,"%s/M.LIST.A",dir);
        fp3 = fopen(genbuf, "a");
        fprintf(fp3, "[Group%d]\n", inic);
        fprintf(fp3, "GroupName=%s\n", bname);
        fprintf(fp3, "PostNum=%d\n", a);
        fprintf(fp3, "LastFile=%s\n\n", substr(fh.filename, 2, 11));
        fclose(fp3);
        return a;

}

/* cppost(dir, grp[i].boardname, grp[i].tag); */

int
cppost(char *dir, char *boardname, long tag)
{
        FILE           *fp, *pgm;
        struct fileheader fh;
        struct stat     st;
        int             count = 0;
        char            xxx[256];

        sprintf(genbuf, "boards/%s/.DIR", boardname);
        fp = fopen(genbuf, "r");

        while (fread(&fh, sizeof(fh), 1, fp) != 0 && !feof(fp) ) {
                strcpy(xxx, substr(fh.filename, 2, 11));
                if ( atol(xxx) > (long)tag && count < (int)MAXGET ) {
                        sprintf(genbuf, "boards/%s/%s", boardname, fh.filename);
                        if (stat(genbuf, &st) >= 0) {
                                /* edwardc.980918 修改防止复制到被 cancel 的文章 */
                               if (st.st_size > (off_t) 0 && fh.owner[0] != '-' ) {
                                        sprintf(genbuf, "cp boards/%s/%s %s/ ", boardname, fh.filename, dir);
                                        pgm = popen(genbuf, "r");
                                        pclose(pgm);
                                        count++;
                                }
                        }
                }
        }
        fclose(fp);
        return count;
}

int
group()
{
        char *cmd1, *cmd2, *cmd3;
        int cleantag = 0, i;
        struct stat st;
        
        cmd1 = (char *)nextword(&msg);
        cmd2 = (char *)nextword(&msg);
        cmd3 = (char *)nextword(&msg);
        
        if ( search_group(&cmd1) == NA ) {
                outs("520 no such group avaliable.");
                return NA;
        }
        
        if ( atoi(cmd2) < 0 || strlen(cmd2) > 9 ) {
                outs("521 para2 error.");
                return NA;
        }
        
        if ( *cmd1 == 0 || *cmd2 == 0 ) {
                outs("522 paramters not enough.");
                return NA;
        }

        if ( *cmd3 == 0 )
                cleantag = 0;
        else
                cleantag = 1;
                        
        if ( grpnum+1 == 128 ) {
                outs("523 sorry, you just tag at least 128 groups.");
                return NA;
        }
        
        if ( grpnum == -1 ) {
                grpnum = 0;
                strcpy(grp[0].boardname, cmd1);
                strcpy(grp[0].tag, cmd2);
        } else {
                for ( i = 0 ; i < grpnum ; i++ ) {
                        if ( strcmp(grp[i].boardname, "EOF") == 0 ) {
                                break;
                        }
                        if ( strcasecmp(grp[i].boardname, cmd1) == 0 ) {
                                outs("524 you already specified it.");
                                return NA;
                        }
                }
                grpnum++;
                
                strcpy(grp[grpnum].boardname, cmd1);
                strcpy(grp[grpnum].tag, cmd2);
                strcpy(grp[grpnum+1].boardname, "EOF");
        }

        if ( cleantag == 1 ) {
                sprintf(genbuf,"%s/boards/%s/.DIR",BBSHOME,cmd1);
                if ( stat(genbuf, &st) < 0 ) {
                        outs("525 stat error!.");
                        return NA;
                } else {
                        sprintf(genbuf,"526 %d tag rested.", st.st_mtime);
                        outs(genbuf);
                        strcpy(grp[grpnum].boardname, "EOF");                   
                        return NA;
                }
        }

        sprintf(genbuf,"%s/boards/%s/.DIR",BBSHOME,cmd1);
        if ( stat(genbuf, &st) < 0 ) {
                sprintf(genbuf,"220 0 0 0");
                outs(genbuf);
                return NA;
        } else {
                if ( atol(cmd2) >= (long) st.st_mtime ) {
                        outs("220 0 0 0");
                        strcpy(grp[grpnum].boardname, "EOF");
                        /* discard specified */
                        return NA;
                } else {
                        newarticle(cmd1, atol(cmd2));
                }
        }
                
        return 0;       
}

int
search_group(char **grp)
{
        FILE *fp;
        struct boardheader bh;
        
        sprintf(genbuf,"%s/.BOARDS",BBSHOME);
        
        fp = fopen(genbuf, "r");
        
        if ( fp == NULL )
                return NA;
                
        while ( fread(&bh, sizeof(bh), 1, fp) != NULL ) {
                if( !(bh.level & PERM_POSTMASK) && !HAS_PERM(bh.level) )
                        continue;
                if ( strcasecmp(bh.filename, *grp) == 0 ) {
                        fclose(fp);
                        strcpy(*grp, bh.filename);
                        /* edwardc.990204 传回正确的讨论区名称 */
                        return YEA;
                }
        }

        fclose(fp);
        return NA;
                
}

int
newarticle(char *brd, long tag)
{
        FILE *fp;
        char xx[10];
        struct fileheader bh;
        struct stat st;
        int new=0, size = 0;
        
        sprintf(genbuf,"%s/boards/%s/.DIR",BBSHOME,brd);
        fp = fopen(genbuf, "r");
        
        if ( fp == NULL ) {
                outs("526 0 article.");
                return 0;
        }
        
        while ( fread(&bh, sizeof(bh), 1, fp) != NULL ) {
                strcpy(xx, substr(bh.filename, 2, 11));
                if ( atol(xx) > tag ) {
                        sprintf(genbuf,"%s/boards/%s/%s",BBSHOME,brd,bh.filename);
                        if ( stat(genbuf, &st) < 0 )
                                continue;
                        if ( st.st_size <= 0 )
                                continue;
                        if ( bh.owner[0] == '-' )
                                continue;
                        new++;
                        size += st.st_size;
                }
        }

        fclose(fp);
        
        sprintf(genbuf,"220 %d %d %s",new,size, substr(bh.filename, 2, 11));
        outs(genbuf);   
}

int
maxget()
{
	sprintf(genbuf,"220 MAXGET: %d",MAXGET);
	outs(genbuf);
}
