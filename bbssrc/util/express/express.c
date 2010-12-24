/*
 * express.c		-- express built-in function and BBS UI implement
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
 * CVS: $Id: express.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
 */

#include "bbs.h"

char express_c[] =
  "$Id: express.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $";
  
#define SZ "bin/sz"

#ifndef SZ
#error "Where is ZMODEM (sz)?"
#endif

#define VERID   "BES 1.01"
#define RARAV

struct taged {
        int             tag;    /* 0 is Y , 1 is N */
        char            status;
        char            filename[STRLEN];       /* the DIR files */
        char            owner[STRLEN];
        char            title[STRLEN];
};

/* the express.config file */

struct bessruc {
        char            id[10]; /* VERID */
        int             compress;       /* 0 is tgz 1 is zip 2 is rar 3 ... */
        int             keepmail;       /* YEA is 要保留 NA 不保留 */
        int             mail;   /* 是否抓 mail (预设 是) */
        int             group;  /* default: none */
        int             maxget;
	long		lastget; /* last time stamp */
};

struct indexsrc {               /* .INDEX file structure v1.1 */
        char            boardname[STRLEN];
        char            filename[STRLEN];
        char            owner[STRLEN];
        char            title[STRLEN];
        char            tag;
};

struct bessruc  bes;
struct taged    tag[1024];

struct fileheader full[1024];

FILE           *gfp;
int             gnum = -1;

int
express()
{
        modify_user_mode(EXPRESS);
        bes_main();
}

int
bes_main()
{

        clear();
        move(1, 0);
        prints("欢迎 %s 使用快信系统 版本 1.01\n", currentuser.userid);
        sprintf(genbuf, "home/%c/%s/express.config", toupper(currentuser.userid[0]), currentuser.userid);
        gfp = fopen(genbuf, "r");
        if (gfp == NULL) {
                if (askyn("设定档不存在! 是否开启新的设定 ", YEA, NA) == NA) {
                        prints("如果您之前已有设定, 那麽请通知管理员. \n");
                        pressanykey();
                        clear();
                        return;
                }
                bes_create();
        } else {
                bes_dumpsetting(0);     /* 列出设定并载入 */
        }
}

int
bes_create()
{

        FILE           *fp;
        char            ans[3];
        char            file[256];
        char            buf[256];

        clear();
        stand_title("快信系统设定");
        move(2, 0);
        sprintf(file, "home/%c/%s/express.config", toupper(currentuser.userid[0]), currentuser.userid);
        fp = fopen(file, "w");
        sprintf(bes.id, "%s", VERID);
        
        /* 只有 rar :) */
        sprintf(buf, "请问你想要的压缩格式 (1) rar: ");
        getdata(3, 0, buf, ans, 2, DOECHO, YEA);

        if (atoi(ans) > 0 && atoi(ans) <= 3) {
                bes.compress = atoi(ans);
        } else {
                fclose(fp);
                unlink(file);
                return -1;
        }

        if (askyn("是否抓取信件 ?", YEA, NA) == NA) {
                bes.mail = 0;
                bes.keepmail = 0;
        } else {
                bes.mail = 1;
                if (askyn("是否保留信件 (即抓取信件後不删除) ", NA, NA) == NA) {
                        bes.keepmail = 0;
                } else {
                        bes.keepmail = 1;
                }

        }

        if (askyn("是否抓取看板文章 ", NA, NA) == NA) {
                bes.group = 0;
        } else {
                prints("信件限制最多 300 封\n");
                bes.group = 1;
                bes.maxget = 300;
        }

        if (askyn("以上设定是否正确 ", YEA, NA) == NA) {
                fclose(fp);
                unlink(file);
                return -1;
        }
        bes.lastget = 0;
        fwrite(&bes, sizeof(bes), 1, fp);
        fclose(fp);

        prints("设定完成 ..\n");
        pressanykey();

        return YEA;
}

int
bes_dumpsetting(int recursive)
{
        char            ans[3];

        if (recursive == 0)
                fread(&bes, sizeof(bes), 1, gfp);

        clear();
        sprintf(genbuf, "欢迎使用快信系统, 版本 1.01 (%s)", VERID);
        stand_title(genbuf);

        move(2, 0);

        if (strcmp(bes.id, VERID) != 0) {
                prints("\033[1;5;31m注意！ 你的设定档结构版本(%s)与系统内版本(%s)不符, 请通知管理员!\n", bes.id, VERID);
                pressanykey();
                return -1;
        }
        prints("压缩格式: ");

        switch (bes.compress) {

        case 1:
	default:
                prints("rar (generic)\n");
                break;
        }

        sprintf(genbuf, "是否抓取信件: %s ", (bes.mail == YEA) ? "是" : "否");
        prints("%s", genbuf);

        if (bes.mail == YEA) {
                sprintf(genbuf, "是否保留信件: %s \n", (bes.keepmail == 1) ? "是" : "否");
                prints("%s", genbuf);
        } else {
                prints("\n");
        }

        sprintf(genbuf, "抓取看板: %s \n", (bes.group == 1) ? "是" : "否");
        prints("%s", genbuf);
        prints("客户端下载请到: http://bbsexpress.ml.org/ \n\n");

        prints("        (1) 设定参数            (2) 信件列表 \n");
        prints("        (3) 设定看板            (4) 查询看板状态 \n");
        prints("        (5) 下传信包            (6) 离开快信系统 \n");

	strcpy(ans, "");
        getdata(10, 0, "请选择 [1-6] : ", ans, 2, DOECHO, YEA);

        switch (ans[0]) {

        case '1':
                bes_create();
                break;
        case '2':
                if (gnum == -1)
                        bes_readDIR();
                bes_maillist(0);
                break;
        case '3':
                bes_selectboard();
                break;

        case '4':
                bes_checkboard(1);
                break;

        case '5':
                bes_compress(0);
                break;
        case '6':
                fclose(gfp);
                pressanykey();
                return NA;
                break;

        default:
        }

        bes_dumpsetting(1);


        /* pressanykey();        */
}

int
bes_readDIR()
{
        FILE           *dir;
        struct fileheader fh;
        int             num = 0;

        sprintf(genbuf, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]), currentuser.userid);
        dir = fopen(genbuf, "r");

        while (fread(&fh, sizeof(fh), 1, dir) != 0 && !feof(dir)) {

                strcpy(tag[num].filename, fh.filename);
                tag[num].tag = 0;
                strcpy(tag[num].filename, fh.filename);
                strcpy(tag[num].owner, fh.owner);
                strcpy(tag[num].title, fh.title);
                full[num].level = fh.level;

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

                num++;

        }
        gnum = num;
        //tag[num + 1].tag == -1;

}

int
bes_maillist(int recursive)
{
        int             num = 0, line = 0, page = 0;
        int             i, loop = 0, loop2;
        char            ans[3], ans2[5], ans3[5];

        clear();
        stand_title("信件列表");

        line = 2;
        while (loop == 0) {
                if (line == 2) {
                        move(1, 0);

                        prints("\033[1;44m标记 编号 %-12s %-30s   %-15s\033[m\n", "发信者", "标  题", "档  名");
                }
                if (num != gnum)
                        prints("%2s %-c %4d %12s %-30s %15s\n", (tag[num].tag == 1) ? "T" : " ", tag[num].status, num + 1, tag[num].owner, tag[num].title, tag[num].filename);

                if (line != 21)
                        line = line++;
                else {
                        line = 2;
                        page++;
                        loop2 = 0;
                        while (loop2 == 0) {
                        	/* edwardc.990109 虽是小 bug 但很困扰使用者 .. */
                        	strcpy(ans, "");
                        	
                                getdata(23, 0, " t 标记/反标记信件 T 区域标记 n/N 标记所有未读信件 q/Q 离开 d/D 下一页 : ", ans, 2, DOECHO, YEA);

                                switch (ans[0]) {
                                case 't':
                                        sprintf(genbuf, "请输入要标记的信件号码 num[%d]: ", num + 1);
                                        getdata(22, 0, genbuf, ans2, 4, DOECHO, YEA);
                                        if (atoi(ans2) <= gnum && atoi(ans2) > 0) {
                                                if (tag[(atoi(ans2) - 1)].tag == 1) {
                                                        tag[(atoi(ans2) - 1)].tag = 0;
                                                        if (atoi(ans2) <= (num + 1) && atoi(ans2) >= (num - 19)) {

                                                                move(1 + (atoi(ans2) - (20 * (page - 1))), 1);
                                                                prints(" ");
                                                        }
                                                } else {
                                                        tag[(atoi(ans2) - 1)].tag = 1;
                                                        if (atoi(ans2) >= (num - 19) && atoi(ans2) <= (num + 1)) {
                                                                move(1 + (atoi(ans2) - (20 * (page - 1))), 1);
                                                                prints("T");
                                                        }
                                                }
                                        }
                                        line = 2;
                                        move(22, 0);
                                        prints("                                                                      ");
                                        break;

                                case 'T':
                                        getdata(22, 0, "请输入要标记的起点: ", ans2, 4, DOECHO, YEA);
                                        if (atoi(ans2) <= gnum + 1 && atoi(ans2) > 0)
                                                getdata(22, 30, "请输入要标记的终点: ", ans3, 4, DOECHO, YEA);
                                        if (atoi(ans3) <= gnum + 1 && atoi(ans3) > 0 && atoi(ans3) > atoi(ans2)) {
                                                for (i = (atoi(ans2) - 1); i < (atoi(ans3)); i++) {
                                                        tag[i].tag = 1;
                                                        if (atoi(ans2) >= (num - 19) && atoi(ans2) <= (num + 1) && atoi(ans3) >= (num - 19) && atoi(ans3) <= (num + 1)) {
                                                                //move(5 + (atoi(ans2) - (20 * page)) + (i - (atoi(ans2) - 1)), 1);
                                                                move(1 + (atoi(ans2) - (20 * (page - 1))) + (i - (atoi(ans2) - 1)), 1);
                                                                prints("T");
                                                        }
                                                }
                                        }
                                        line = 2;
                                        move(22, 0);
                                        prints("                                                                      ");
                                        break;

                                case 'n':
                                case 'N':

                                        bes_tagnew();
                                        break;

                                case 'Q':
                                case 'q':
                                        if (askyn("是否浏览已标记的信件 ", YEA, NA) == YEA) {
                                                bes_viewtagged();
                                        }
                                        return NA;
                                case 'd':
                                case 'D':
                                        line = 2;
                                        move(1, 0);
                                        clrtobot();
                                        loop2 = 1;
                                        break;

                                default:
                                        line = 2;
                                        move(1, 0);
                                        clrtobot();
                                        loop2 = 1;
                                }
                        }
                }
                if (num == gnum)
                        loop = 1;
                else
                        num++;
        }

        page = 0;
        line = 2;
        //num = 1;

        loop2 = 0;
        while (loop2 == 0) {
                getdata(23, 0, " t 标记/反标记信件 T 区域标记 n/N 标记所有未读信件 q/Q 离开 : ", ans, 2, DOECHO, YEA);
                switch (ans[0]) {
                case 't':
                        sprintf(genbuf, "请输入要标记的信件号码 num[%d]: ", num + 1);
                        getdata(22, 0, genbuf, ans2, 4, DOECHO, YEA);
                        if (atoi(ans2) <= gnum && atoi(ans2) > 0) {
                                if (tag[(atoi(ans2) - 1)].tag == 1) {
                                        tag[(atoi(ans2) - 1)].tag = 0;
                                        if (atoi(ans2) <= (num + 1) && atoi(ans2) >= (num - 19)) {
                                                move(5 + (atoi(ans2) - (20 * page)) - 8, 1);
                                                prints(" ");
                                        }
                                } else {
                                        tag[(atoi(ans2) - 1)].tag = 1;
                                        if (atoi(ans2) >= (num - 19) && atoi(ans2) <= (num + 1)) {
                                                move(5 + (atoi(ans2) - (20 * page)) - 8, 1);
                                                prints("T");
                                        }
                                }
                        }
                        //line = 2;
                        move(22, 0);
                        prints("                                                                      ");
                        break;

                case 'T':
                        getdata(22, 0, "请输入要标记的起点: ", ans2, 4, DOECHO, YEA);
                        if (atoi(ans2) <= gnum + 1 && atoi(ans2) > 0)
                                getdata(22, 30, "请输入要标记的终点: ", ans3, 4, DOECHO, YEA);
                        if (atoi(ans3) <= gnum + 1 && atoi(ans3) > 0 && atoi(ans3) > atoi(ans2)) {
                                for (i = (atoi(ans2) - 1); i < (atoi(ans3)); i++) {
                                        tag[i].tag = 1;
                                        if (atoi(ans2) >= (num - 19) && atoi(ans2) <= (num + 1) && atoi(ans3) >= (num - 19) && atoi(ans3) <= (num + 1)) {
                                                move((5 + (atoi(ans2) - (20 * page)) + (i - (atoi(ans2) - 1))) - 8, 1);
                                                prints("T");
                                        }
                                }
                        }
                        //line = 2;
                        move(22, 0);
                        prints("                                                                      ");
                        break;

                case 'n':
                case 'N':

                        bes_tagnew();
                        break;

                case 'q':
                case 'Q':
                        if (askyn("是否浏览已标记的信件 ", YEA, NA) == YEA) {
                                bes_viewtagged();
                        }
                        return YEA;
                        //loop2 = 1;
                default:
                        loop2 = 1;
                }
        }

        return NA;

}

int
bes_tagnew()
{
        int             i;

        for (i = 0; i < gnum; i++) {
                if (tag[i].status == 'N' || tag[i].status == 'M')
                        tag[i].tag = 1;
        }
}

int
bes_viewtagged()
{
        int             line = 0, match = 0, i;

        clear();
        stand_title("已标记信件列表");

        move(1, 0);

        prints("\033[1;44m标记 编号 %-12s %-30s   %-15s\033[m\n", "发信者", "标  题", "档  名");

        for (i = 0; i < gnum; i++) {

                if (tag[i].tag == 1) {
                        prints("%2s %-c %4d %12s %-30s %15s\n", (tag[i].tag == 1) ? "T" : " ", tag[i].status, i + 1, tag[i].owner, tag[i].title, tag[i].filename);
                        match++;
                        if (line != 21)
                                line++;
                        else {
                                line = 1;
                                pressanykey();
                                /* edwardc.990109 应该从 2 开始, 要不然会遮到 bar */
                                move(2, 0);
                                clrtobot();
                        }
                }
        }
        if (match == 0) {
                move(5, 15);
                prints("\033[1;31m没有任何已标记的信件 ! \n");
        }
        pressreturn();
}


int
bes_selectboard()
{
        char            besboard[STRLEN], tmp[3];
        char            bname[STRLEN], bpath[STRLEN];
        int             count, exit = NA;
        struct stat     st;

        if (bes.group == 0) {
                if (askyn("您并无开启抓取看板信件功\能, 现在开启吗", NA, NA) == 1) {
                        bes.group = 1;
                } else {
                        return NA;
                }
        }
        sethomefile(besboard, currentuser.userid, "express.boards");


        while (exit == NA) {

                clear();
                stand_title("设定看板 (最多十版) \n");
                count = listfilecontent(besboard);
                if (count > 10) {
                        move(2, 0);
                        prints("目前最多只允许\设定 10 个版\n");
                }
                getdata(1, 0, "(A) 增加 (D) 删除 (C) 清除所有设定 (Q) 离开? [Q] : ",
                        tmp, 2, DOECHO, YEA);


                switch (tmp[0]) {

                case 'A':
                case 'a':


                        move(0, 0);
                        clrtoeol();
                        prints("选择一个讨论区 (英文字母大小写皆可)\n");
                        prints("输入讨论区名 (按空白键自动搜寻): ");

                        clrtoeol();
                        make_blist();
                        namecomplete((char *) NULL, bname);

                        if (seek_in_file(besboard, bname)) {
                                move(2, 0);
                                prints("已经为抓取看板之一 \n");
                                break;
                        }
                        setbpath(bpath, bname);
                        if ((*bname == '\0') || (stat(bpath, &st) == -1)) {
                                move(2, 0);
                                prints("不正确的讨论区.\n");
                                break;
                        }
                        if (!(st.st_mode & S_IFDIR)) {
                                move(2, 0);
                                prints("不正确的讨论区.\n");
                                pressreturn();
                                break;
                        }
                        addtofile(besboard, bname);
                        count++;
                        break;

                case 'C':
                case 'c':

                        unlink(besboard);
                        count = 0;
                        break;

                case 'D':
                case 'd':

                        namecomplete("输入看板名称:", bname);
                        if (seek_in_file(besboard, bname)) {
                                sprintf(genbuf, "express.boards.%s", bname);
                                sethomefile(bpath, currentuser.userid, genbuf);
                                unlink(bpath);
                                del_from_file(besboard, bname);
                                count--;
                        }
                        break;

                case 'e':
                case 'E':
                case 'Q':
                case 'q':

                default:
                        exit = YEA;
                }
        }

        /*
         * if (count != 0) bes_freshBOARD();
         */
        return 0;
}

int
bes_compress()
{
        char            command[100], mtime[10], dir[100];
        char            compress[512], package[30], buf[50];
        char            fullpath[200];
        int             i = 1, match = 0, check = 0;
        FILE           *index, *pgm, *fp;
        struct indexsrc savebuf;
	struct stat st;

        sprintf(mtime, "%d", time(0));
        
        /* edwardc.990109 使用内建函数比较好用 ^_^ */        

        sprintf(command, "rm -fr tmp/express.%s.*", currentuser.userid);
        system(command);
        sprintf(dir, "tmp/express.%s.%s", currentuser.userid, mtime);
        mkdir(dir, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
        strcpy(fullpath, dir);
        strcat(fullpath, "/");

#if 0
        if (gnum == -1) {
                /* prints("请先标记信件後再使用本功\能.\n "); */
                prints("没有任何已标记信件.. \n");
                pressreturn();
                return NA;
        }
#endif

        if (bes.mail == 1) {
                prints("复制信件中.. ");


                for (i = 0; i < gnum; i++) {
                        if (tag[i].tag == 1) {
                                sprintf(compress, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, tag[i].filename);
                                if (stat(compress, &st) >= 0) {
                                        if (st.st_size > (off_t) 0) {
                                                sprintf(genbuf, " cp mail/%c/%s/%s %s/ ", toupper(currentuser.userid[0]), currentuser.userid, tag[i].filename, dir);
                                                pgm = popen(genbuf, "r");
                                                pclose(pgm);
                                                match++;
                                        }
                                }
                        }
                }

                if (match != 0) {
                        prints("done %d.\n", gnum);
                        check++;
                } else {
                        prints("没有任何以标记的信件. \n");
                        if ( bes.group != 1 ) {
                        	prints("放弃作业.. \n"); 
                        	pressreturn(); 
                        	return NA;
                        }
                }
        }

        if (bes.group == 1) {
                sethomefile(command, currentuser.userid, "express.boards");

                fp = fopen(command, "r");

                if (fp == NULL) {
                        prints("cannot open %s , quit.. \n", command);
                        pressanykey();
                        return NA;
                }
                i = 1;



                while (!feof(fp) && fscanf(fp, "%s", buf) != 0) {
                        if (!feof(fp)) {
                                prints("复制 %s 板新文章中.. ", buf);
                                match = bes_cppost(dir, currentuser.userid, buf);
                                if (match == 0)
                                        prints("没有新文章\n");
                                else {
                                        check++;
                                        prints("共复制 %d 篇文章  ", match);
                                        sprintf(command, "%s/M.INDEX.A", dir);
                                        sprintf(compress, "%s/M.LIST.A", dir);
                                        if (bes_makeindex(buf, command, i, compress) > 0) {
                                                prints("加入索引档\n");
                                                i++;
                                        } else
                                                prints("没有加入索引档\n");
                                }
                        }
                }
                index = fopen(compress, "a");
                fprintf(index, "[General]\n");
                fprintf(index, "Count=%d\n", i - 1);
                fprintf(index, "Build=%d\n\n", time(0));
                fclose(index);
        }

        if (bes.mail == YEA) {
                prints("产生信件索引档中.. ");

                sprintf(genbuf, "%s/M.INDEX.A", dir);

                index = fopen(genbuf, "ab");

                if (gnum == -1) {
                        prints("没有标记信件, 跳过.. ");
                } else {
                        for (match = 0; match < gnum; match++) {
                                if (tag[match].tag == 1) {
                                        strcpy(savebuf.boardname, "_MAIL_");
                                        strcpy(savebuf.filename, tag[match].filename);
                                        strcpy(savebuf.owner, tag[match].owner);
                                        strcpy(savebuf.title, tag[match].title);
                                        savebuf.tag = tag[match].status;
                                        fwrite(&savebuf, sizeof(savebuf), 1, index);
                                }
                        }
                }


                fclose(index);

        }
        if (check == 0) {
                prints("nothing to do ... exit..\n");
                return NA;
        }
        
        /* edwardc.981103 for some UnRAR's problem */
        /* edwardc.990109 使用 chmod() 代替 system() */
        
        sprintf(genbuf,"%s/M.INDEX.A",dir);
        chmod(genbuf, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        sprintf(genbuf,"%s/M.LIST.A",dir);
        chmod(genbuf, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        
        prints("压缩中.. ");
        sprintf(package, "%s.%s.", currentuser.userid, mtime);
     
        /* edwardc.990206 一律采用 rar */
           
#if 0
        if (bes.compress == 1) {
                strcat(package, "tar.gz");
                sprintf(compress, "bin/tar cplfsz %s/%s %s/M.*.A", dir, package, dir);
        } else if (bes.compress == 2) {
                strcat(package, "zip");
                sprintf(compress, "bin/zip -9 %s/%s %s/M.*.A", dir, package, dir);
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

        pgm = popen(compress, "r");
        pclose(pgm);

        prints("done.\n");
        prints("设定 History 纪录 .. done.\n");
        prints("请启动 client 端接收 zmodem (通常是自动开启)\n");
        sprintf(command, "准备送出信包 %s ", package);
        if (askyn(command, YEA, NA) == YEA) {
                sprintf(command, "%s %s", SZ, fullpath);
                system(command);
        }
	sprintf(command,"rm -fr tmp/express.%s.*",currentuser.userid);
	system(command);
        pressanykey();
}

int
bes_checkboard(int type)
{
        FILE           *fp;
        char            buf[100], besboard[STRLEN];

	/* TODO
	   做个像 viewtagged 一样的 UI */
	   
        if (bes.group == 0) {
                if (askyn("您并无开启抓取看板信件功\能, 现在开启吗", NA, NA) == 1) {
                        bes.group = 1;
                } else {
                        return NA;
                }
        }
        sethomefile(besboard, currentuser.userid, "express.boards");

        fp = fopen(besboard, "r");

        while (fscanf(fp, "%s", buf) != 0 && !feof(fp)) {

                prints("%15s 板 -=> %4d 篇未读 \n", buf, bes_reportboard(buf));
        }

        pressanykey();
}

int
bes_reportboard(char *boardname)
{
        /*
         * 此函数只用来显示各看板新文章统计, 并不负责标记. compress
         * 再做标记就可以了
         */

        FILE           *fp;
        struct fileheader fh;
        int             count = 0;

        brc_initial(boardname); /* 记得 initial 一下 */

        sprintf(genbuf, "boards/%s/.DIR", boardname);
        fp = fopen(genbuf, "r");

        while (fread(&fh, sizeof(fh), 1, fp) != 0 && !feof(fp)) {
                if (brc_unread(fh.filename))
                        count++;
        }
        return count;
}

int
bes_makeindex(char *bname, char *index, int inic, char *ini)
{
        struct fileheader fh;
        struct indexsrc is;
        FILE           *fp, *fp2, *fp3;
        char		xxx[20];
        int             a = 0;

        brc_initial(bname);

        sprintf(genbuf, "boards/%s/.DIR", bname);
        fp = fopen(genbuf, "r");
        fp2 = fopen(index, "ab");

        while (fread(&fh, sizeof(fh), 1, fp) != 0 && !feof(fp)) {
                if (brc_unread(fh.filename) && fh.owner[0] != '-' ) {
                        brc_addlist( fh.filename ) ;
                        strcpy(is.boardname, bname);
                        strcpy(is.filename, fh.filename);
                        strcpy(is.owner, fh.owner);
                        strcpy(is.title, fh.title);

                        if (fh.accessed[0] & FILE_MARKED) {
                                if (fh.accessed[0] & FILE_DIGEST)
                                        is.tag = 'b';
                                else
                                        is.tag = 'm';
                        } else {
                                if (fh.accessed[0] & FILE_DIGEST)
                                        is.tag = 'g';
                                else
                                        is.tag = ' ';
                        }

                        fwrite(&is, sizeof(is), 1, fp2);
                        a++;
                }
        }

        fclose(fp);
        fclose(fp2);
        fp3 = fopen(ini, "a");
        fprintf(fp3, "[Group%d]\n", inic);
        fprintf(fp3, "GroupName=%s\n", bname);
        fprintf(fp3, "PostNum=%d\n", a);
        sscanf(fh.filename,"M.%s.A",xxx);
	fprintf(fp3, "LastFile=%s\n\n",xxx);
        fclose(fp3);
        return a;

}

int
bes_cppost(char *dir, char *userid, char *boardname)
{
        FILE           *fp, *pgm;
        struct fileheader fh;
        struct stat     st;
        int             count = 0;

        brc_initial(boardname); /* 记得 initial 一下 */

        sprintf(genbuf, "boards/%s/.DIR", boardname);
        fp = fopen(genbuf, "r");

        while (fread(&fh, sizeof(fh), 1, fp) != 0 && !feof(fp) && count < bes.maxget ) {

                if (brc_unread(fh.filename)) {
                        sprintf(genbuf, "boards/%s/%s", boardname, fh.filename);
                        if (stat(genbuf, &st) >= 0) {
                        
                        	/* 980918 修改防止复制到被 cancel 的文章 */
                        	
                                if (st.st_size > (off_t) 0 && fh.owner[0] != '-' ) {
                                        sprintf(genbuf, "cp boards/%s/%s %s/", boardname, fh.filename, dir);
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
