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
        int             keepmail;       /* YEA is Ҫ���� NA ������ */
        int             mail;   /* �Ƿ�ץ mail (Ԥ�� ��) */
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
        prints("��ӭ %s ʹ�ÿ���ϵͳ �汾 1.01\n", currentuser.userid);
        sprintf(genbuf, "home/%c/%s/express.config", toupper(currentuser.userid[0]), currentuser.userid);
        gfp = fopen(genbuf, "r");
        if (gfp == NULL) {
                if (askyn("�趨��������! �Ƿ����µ��趨 ", YEA, NA) == NA) {
                        prints("�����֮ǰ�����趨, ������֪ͨ����Ա. \n");
                        pressanykey();
                        clear();
                        return;
                }
                bes_create();
        } else {
                bes_dumpsetting(0);     /* �г��趨������ */
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
        stand_title("����ϵͳ�趨");
        move(2, 0);
        sprintf(file, "home/%c/%s/express.config", toupper(currentuser.userid[0]), currentuser.userid);
        fp = fopen(file, "w");
        sprintf(bes.id, "%s", VERID);
        
        /* ֻ�� rar :) */
        sprintf(buf, "��������Ҫ��ѹ����ʽ (1) rar: ");
        getdata(3, 0, buf, ans, 2, DOECHO, YEA);

        if (atoi(ans) > 0 && atoi(ans) <= 3) {
                bes.compress = atoi(ans);
        } else {
                fclose(fp);
                unlink(file);
                return -1;
        }

        if (askyn("�Ƿ�ץȡ�ż� ?", YEA, NA) == NA) {
                bes.mail = 0;
                bes.keepmail = 0;
        } else {
                bes.mail = 1;
                if (askyn("�Ƿ����ż� (��ץȡ�ż��᲻ɾ��) ", NA, NA) == NA) {
                        bes.keepmail = 0;
                } else {
                        bes.keepmail = 1;
                }

        }

        if (askyn("�Ƿ�ץȡ�������� ", NA, NA) == NA) {
                bes.group = 0;
        } else {
                prints("�ż�������� 300 ��\n");
                bes.group = 1;
                bes.maxget = 300;
        }

        if (askyn("�����趨�Ƿ���ȷ ", YEA, NA) == NA) {
                fclose(fp);
                unlink(file);
                return -1;
        }
        bes.lastget = 0;
        fwrite(&bes, sizeof(bes), 1, fp);
        fclose(fp);

        prints("�趨��� ..\n");
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
        sprintf(genbuf, "��ӭʹ�ÿ���ϵͳ, �汾 1.01 (%s)", VERID);
        stand_title(genbuf);

        move(2, 0);

        if (strcmp(bes.id, VERID) != 0) {
                prints("\033[1;5;31mע�⣡ ����趨���ṹ�汾(%s)��ϵͳ�ڰ汾(%s)����, ��֪ͨ����Ա!\n", bes.id, VERID);
                pressanykey();
                return -1;
        }
        prints("ѹ����ʽ: ");

        switch (bes.compress) {

        case 1:
	default:
                prints("rar (generic)\n");
                break;
        }

        sprintf(genbuf, "�Ƿ�ץȡ�ż�: %s ", (bes.mail == YEA) ? "��" : "��");
        prints("%s", genbuf);

        if (bes.mail == YEA) {
                sprintf(genbuf, "�Ƿ����ż�: %s \n", (bes.keepmail == 1) ? "��" : "��");
                prints("%s", genbuf);
        } else {
                prints("\n");
        }

        sprintf(genbuf, "ץȡ����: %s \n", (bes.group == 1) ? "��" : "��");
        prints("%s", genbuf);
        prints("�ͻ��������뵽: http://bbsexpress.ml.org/ \n\n");

        prints("        (1) �趨����            (2) �ż��б� \n");
        prints("        (3) �趨����            (4) ��ѯ����״̬ \n");
        prints("        (5) �´��Ű�            (6) �뿪����ϵͳ \n");

	strcpy(ans, "");
        getdata(10, 0, "��ѡ�� [1-6] : ", ans, 2, DOECHO, YEA);

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
        stand_title("�ż��б�");

        line = 2;
        while (loop == 0) {
                if (line == 2) {
                        move(1, 0);

                        prints("\033[1;44m��� ��� %-12s %-30s   %-15s\033[m\n", "������", "��  ��", "��  ��");
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
                        	/* edwardc.990109 ����С bug ��������ʹ���� .. */
                        	strcpy(ans, "");
                        	
                                getdata(23, 0, " t ���/������ż� T ������ n/N �������δ���ż� q/Q �뿪 d/D ��һҳ : ", ans, 2, DOECHO, YEA);

                                switch (ans[0]) {
                                case 't':
                                        sprintf(genbuf, "������Ҫ��ǵ��ż����� num[%d]: ", num + 1);
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
                                        getdata(22, 0, "������Ҫ��ǵ����: ", ans2, 4, DOECHO, YEA);
                                        if (atoi(ans2) <= gnum + 1 && atoi(ans2) > 0)
                                                getdata(22, 30, "������Ҫ��ǵ��յ�: ", ans3, 4, DOECHO, YEA);
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
                                        if (askyn("�Ƿ�����ѱ�ǵ��ż� ", YEA, NA) == YEA) {
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
                getdata(23, 0, " t ���/������ż� T ������ n/N �������δ���ż� q/Q �뿪 : ", ans, 2, DOECHO, YEA);
                switch (ans[0]) {
                case 't':
                        sprintf(genbuf, "������Ҫ��ǵ��ż����� num[%d]: ", num + 1);
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
                        getdata(22, 0, "������Ҫ��ǵ����: ", ans2, 4, DOECHO, YEA);
                        if (atoi(ans2) <= gnum + 1 && atoi(ans2) > 0)
                                getdata(22, 30, "������Ҫ��ǵ��յ�: ", ans3, 4, DOECHO, YEA);
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
                        if (askyn("�Ƿ�����ѱ�ǵ��ż� ", YEA, NA) == YEA) {
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
        stand_title("�ѱ���ż��б�");

        move(1, 0);

        prints("\033[1;44m��� ��� %-12s %-30s   %-15s\033[m\n", "������", "��  ��", "��  ��");

        for (i = 0; i < gnum; i++) {

                if (tag[i].tag == 1) {
                        prints("%2s %-c %4d %12s %-30s %15s\n", (tag[i].tag == 1) ? "T" : " ", tag[i].status, i + 1, tag[i].owner, tag[i].title, tag[i].filename);
                        match++;
                        if (line != 21)
                                line++;
                        else {
                                line = 1;
                                pressanykey();
                                /* edwardc.990109 Ӧ�ô� 2 ��ʼ, Ҫ��Ȼ���ڵ� bar */
                                move(2, 0);
                                clrtobot();
                        }
                }
        }
        if (match == 0) {
                move(5, 15);
                prints("\033[1;31mû���κ��ѱ�ǵ��ż� ! \n");
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
                if (askyn("�����޿���ץȡ�����ż���\��, ���ڿ�����", NA, NA) == 1) {
                        bes.group = 1;
                } else {
                        return NA;
                }
        }
        sethomefile(besboard, currentuser.userid, "express.boards");


        while (exit == NA) {

                clear();
                stand_title("�趨���� (���ʮ��) \n");
                count = listfilecontent(besboard);
                if (count > 10) {
                        move(2, 0);
                        prints("Ŀǰ���ֻ����\�趨 10 ����\n");
                }
                getdata(1, 0, "(A) ���� (D) ɾ�� (C) ��������趨 (Q) �뿪? [Q] : ",
                        tmp, 2, DOECHO, YEA);


                switch (tmp[0]) {

                case 'A':
                case 'a':


                        move(0, 0);
                        clrtoeol();
                        prints("ѡ��һ�������� (Ӣ����ĸ��Сд�Կ�)\n");
                        prints("������������ (���հ׼��Զ���Ѱ): ");

                        clrtoeol();
                        make_blist();
                        namecomplete((char *) NULL, bname);

                        if (seek_in_file(besboard, bname)) {
                                move(2, 0);
                                prints("�Ѿ�Ϊץȡ����֮һ \n");
                                break;
                        }
                        setbpath(bpath, bname);
                        if ((*bname == '\0') || (stat(bpath, &st) == -1)) {
                                move(2, 0);
                                prints("����ȷ��������.\n");
                                break;
                        }
                        if (!(st.st_mode & S_IFDIR)) {
                                move(2, 0);
                                prints("����ȷ��������.\n");
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

                        namecomplete("���뿴������:", bname);
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
        
        /* edwardc.990109 ʹ���ڽ������ȽϺ��� ^_^ */        

        sprintf(command, "rm -fr tmp/express.%s.*", currentuser.userid);
        system(command);
        sprintf(dir, "tmp/express.%s.%s", currentuser.userid, mtime);
        mkdir(dir, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
        strcpy(fullpath, dir);
        strcat(fullpath, "/");

#if 0
        if (gnum == -1) {
                /* prints("���ȱ���ż�����ʹ�ñ���\��.\n "); */
                prints("û���κ��ѱ���ż�.. \n");
                pressreturn();
                return NA;
        }
#endif

        if (bes.mail == 1) {
                prints("�����ż���.. ");


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
                        prints("û���κ��Ա�ǵ��ż�. \n");
                        if ( bes.group != 1 ) {
                        	prints("������ҵ.. \n"); 
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
                                prints("���� %s ����������.. ", buf);
                                match = bes_cppost(dir, currentuser.userid, buf);
                                if (match == 0)
                                        prints("û��������\n");
                                else {
                                        check++;
                                        prints("������ %d ƪ����  ", match);
                                        sprintf(command, "%s/M.INDEX.A", dir);
                                        sprintf(compress, "%s/M.LIST.A", dir);
                                        if (bes_makeindex(buf, command, i, compress) > 0) {
                                                prints("����������\n");
                                                i++;
                                        } else
                                                prints("û�м���������\n");
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
                prints("�����ż���������.. ");

                sprintf(genbuf, "%s/M.INDEX.A", dir);

                index = fopen(genbuf, "ab");

                if (gnum == -1) {
                        prints("û�б���ż�, ����.. ");
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
        /* edwardc.990109 ʹ�� chmod() ���� system() */
        
        sprintf(genbuf,"%s/M.INDEX.A",dir);
        chmod(genbuf, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        sprintf(genbuf,"%s/M.LIST.A",dir);
        chmod(genbuf, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        
        prints("ѹ����.. ");
        sprintf(package, "%s.%s.", currentuser.userid, mtime);
     
        /* edwardc.990206 һ�ɲ��� rar */
           
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
        prints("�趨 History ��¼ .. done.\n");
        prints("������ client �˽��� zmodem (ͨ�����Զ�����)\n");
        sprintf(command, "׼���ͳ��Ű� %s ", package);
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
	   ������ viewtagged һ���� UI */
	   
        if (bes.group == 0) {
                if (askyn("�����޿���ץȡ�����ż���\��, ���ڿ�����", NA, NA) == 1) {
                        bes.group = 1;
                } else {
                        return NA;
                }
        }
        sethomefile(besboard, currentuser.userid, "express.boards");

        fp = fopen(besboard, "r");

        while (fscanf(fp, "%s", buf) != 0 && !feof(fp)) {

                prints("%15s �� -=> %4d ƪδ�� \n", buf, bes_reportboard(buf));
        }

        pressanykey();
}

int
bes_reportboard(char *boardname)
{
        /*
         * �˺���ֻ������ʾ������������ͳ��, ����������. compress
         * ������ǾͿ�����
         */

        FILE           *fp;
        struct fileheader fh;
        int             count = 0;

        brc_initial(boardname); /* �ǵ� initial һ�� */

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

        brc_initial(boardname); /* �ǵ� initial һ�� */

        sprintf(genbuf, "boards/%s/.DIR", boardname);
        fp = fopen(genbuf, "r");

        while (fread(&fh, sizeof(fh), 1, fp) != 0 && !feof(fp) && count < bes.maxget ) {

                if (brc_unread(fh.filename)) {
                        sprintf(genbuf, "boards/%s/%s", boardname, fh.filename);
                        if (stat(genbuf, &st) >= 0) {
                        
                        	/* 980918 �޸ķ�ֹ���Ƶ��� cancel ������ */
                        	
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
