/*$Id: bbs0an.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
  ����mode=1Ϊ����ģʽ
  path��0AnnounceΪ��Ŀ¼
  ����/groups,û��/��β
*/
#include "BBSLIB.inc"
#include "digest.h"
int
main ()
{
        FILE *fp;
        int count, i, index = 0, total = 0;
        int isfile = 0;
        char *id;
        char *ptr, path[512], names[512], name[1024][80], file[1024][80], buf[512],
        title[256] = " ";

        init_all ();
        modify_mode (u_info, DIGEST + 20000);	//bluetent
        printf ("<script language=JavaScript src=/scrolldown.js></script>");
        printf ("<script>\
                function SubmitFlag(flagmode){\
                document.form1.mode.value=flagmode;\
                document.form1.submit();\
        }\
                function getvalue(info,mode) {\
                xx=prompt(info,'');\
                if(xx=='' || xx==null) return 0;\
                form1.a.value=xx;\
                SubmitFlag(mode);\
        }\
                </script>");
        printf ("<body background=%s bgproperties=fixed>", mytheme.bgpath);


        strsncpy (path, getparm ("path"), 511);
        if (strstr (path, "..") || strstr (path, "SYSHome"))
                http_fatal ("��Ŀ¼������");
        sprintf (names, "0Announce%s/.Names", path);
        if (!file_exist (names))
                http_fatal ("Ŀ¼������");

        char s[STRLEN];
        char buftitle[1024];
        int boardmode = 0, pcmode = 0;
        int mode = getfrompath (path, s);
        char pctitle[STRLEN];
        if (mode == 1)
                boardmode = 1;
        if (mode == 2) {
                pcmode = 1;
                getpctitle (s, pctitle);

        }

        if (boardmode)
                if (!has_read_perm (&currentuser, s)) {
                        showinfo ("���ڸ�������,��û�ж�Ȩ��.����,�޷��鿴�侫����.");
                        return 0;
                }

        int adminmode = atoi (getparm ("mode"));
        if (adminmode) {
                if (!hasperm (mode, s)) {
                        showinfo ("����Ȩ�鿴��ҳ!");
                        return 0;
                }

        }

        fp = fopen (names, "r");
        if (fp == 0) {
                showinfo ("$s�޷���", names);
                return 0;
        }

        while (1) {
                if (fgets (buf, 511, fp) == 0)
                        break;
                if (!strncmp (buf, "# Title=", 8))
                        strcpy (title, buf + 8);
                if (!strncmp (buf, "Name=", 5) && total < 1023) {
//                        if (!strncmp (buf+5, "<GUESTBOOK>", 11)) {
//                                sprintf (name[total], "|���Ա�|%s",trim (buf + 16));

//                                printf("");/*I add this,and all is right then.*/

//                        } else
                                strcpy (name[total], rtrim (buf + 5));
                        strcpy (file[total], "");
                        total++;
                }
                if (!strncmp (buf, "Path=~", 6) && total > 0) {
                        sprintf (file[total - 1], "%s", trim (buf + 6));
                }
        }

        if (strstr (title, "SYSOPS") && strstr (title, "SYSOP")
                        && !(currentuser.userlevel & PERM_SYSOP))
                http_fatal ("�����Ŀ¼");
        if (strstr (title, "BMS"))
                if (boardmode)
                        if (!has_BM_perm (&currentuser, s)) {
                                showinfo ("����Ȩ���ʱ�ҳ");
                                return 0;
                        }


        if (pcmode) {
                printf (" <font size=6>%s</font>   ��ӭ����<font color=green>%s</font>���ļ�", pctitle, s);
                printf ("<a href=bbspstmail?userid=%s>д���ʺ�</a>", s);
                printf("         ");
                printf ("<a href=/person/%c/%s.html.tar.gz>   ����  </a>",toupper(s[0]),s);
                printf("<a href=bbsdoc?board=PersonalCorpus>  �Ƽ� </a>");
                printf("<a href=bbsdread?path=/PersonalCorpus/%c/%s> ���� </a>",toupper(s[0]),s);
        } else
                printf ("<center>        <img src=/jinghua_b.gif>");
        char tmpbuf[16];
        printf (" <br><img src=/visit.gif> ��Ŀ¼�������: %d</center>\n",
                get_count (path));
        printf ("<a href=bbs0an?path=/abbs> ��վʹ�ð���</a>");
        if (adminmode == 0) {
                if (boardmode) {
                        strncpy (tmpbuf, path, 15);
                        printf
                        ("<a href=bbsdread?path=%s/%s> Ŀ¼����</a><a href=bbsdoc?board=%s> ��������",
                         tmpbuf, s, s);
                }
                if (hasperm (mode, s)) {
                        printf("<a href=bbs0an?mode=1&path=%s>   ����ģʽ</a>", path);

                        printf("");/*I add this,and all is right then.*/

                        printf("<a href=digestpst?mode=1&path=%s>  ��������</a>",path);
                        printf ("");
                }
                if(strstr(title,"<GUESTBOOK>"))
                        printf("<a href=digestpst?mode=2&path=%s>  ��Ҫ����</a>",path);
                char cn[1024], tmpbuf[256];
                sprintf (tmpbuf, "0Announce%s", path);
                cnpath (tmpbuf, cn);
                printf ("<br>%s", cn);
        } else {
                printf ("<br><a href=bbs0an?path=%s>��ͨģʽ</a>", path);
                printf
                ("<a href='javascript:SubmitFlag(2)'> �½�</a><a href='javascript:SubmitFlag(3)'> ����</a><a href='javascript:SubmitFlag(4)'> ճ��</a><a href=\"javascript:if(confirm('ȷ��Ҫɾ��������Щ�ļ���?ע��,��ֻ��ɾ����Ŀ¼.')) SubmitFlag(5)\">    ɾ��  </a> <a href=\"javascript:getvalue('������������:',6)\">������</a><a  href=\"javascript:getvalue('�������´���:',7)\"> �ƶ�����</a>");
//                if (boardmode)//huangxu@060715:Ϊ������ļ����У�
                        printf
                        ("<a href='javascript:SubmitFlag(1)'> �鿴˿·</a><a href='javascript:SubmitFlag(11)'> ��Ϊ˿·</a>");
        }

        printf ("<form method=post name=form1 action=bbsdigest?path=%s>", path);
        printf ("<input type=hidden name=mode>");
        printf ("<input type=hidden name=a>");
        printf
        ("<center><table cellspacing=1 cellpadding=2 border=0 width=610 align=center>\n");
        printf
        ("<tr bgcolor=#%s class=title><td align=middle width=30><font color=%s>���</font>",
         mytheme.bar,mytheme.lf);
        if (adminmode)
                printf ("<td>����");
        printf
        ("<td> <td align=middle width=*><font color=%s>����</font><td align=middle><font color=%s  width=100>%s</font><td align=middle><font color=%s width=60>����</font>",mytheme.lf,mytheme.lf,pcmode?"����":"����",mytheme.lf);
        int hidden = 0;
        for (i = 0; i < total; i++) {

                hidden = 0;
                if (strstr (name[i], "SYSOPS")
                                && strstr (title, "SYSOP") && !(currentuser.userlevel & PERM_SYSOP))
                        continue;
                if (strstr (name[i], "BMS"))
                        if (boardmode)
                                if (!has_BM_perm (&currentuser, s))
                                        continue;
                if (!strncmp (name[i], "<HIDE>", 6)) {
                        if (!hasperm (mode, s))
                                continue;
                        else
                                hidden = 1;
                }

                index++;
                if (strlen (name[i]) <= 39)
                        id = "";
                else {
                        name[i][38] = 0;
                        id = name[i] + 39;
                        if (!strncmp (id, "BM: ", 4))
                                id += 4;
                        ptr = strchr (id, ')');
                        if (ptr)
                                ptr[0] = 0;
                }

                printf ("<tr><td align=right>%d", index);
                sprintf (buf, "0Announce%s%s", path, file[i]);
                if (!file_exist (buf)) {
                        printf ("<td align=middle width=30>[����] <td>%s",
                                nohtml (name[i]));
                } else {
                        hsprintf2(buftitle,"%s",name[i]);//huangxu@061021
                        if (file_isdir (buf)) {
                                if (adminmode)
                                        printf ("<td><input  name=box%d type=checkbox>\n", i + 1);
                                printf ("<td align=middle width=20>\n");
                                printf
                                ("<img src=/folder.gif>\n<td><a href=bbs0an?%spath=%s%s>%s</a>\n",
                                 adminmode ? "mode=1&" : "", path, file[i], buftitle);
                        } else {
                                if (adminmode) {
                                        printf
                                        ("<td><input style='height:18px' name=box%d type=checkbox>\n",
                                         i + 1);
                                        //   printf("<a href=bbsedit?mode=2&path=%s%s>�޸�</a>",path,file[i]);
                                }
                                printf ("<td align=middle width=20>\n");
                                printf
                                ("<img src=/link0.gif><td><a href=bbsanc?%spath=%s%s>%s</a>",
                                 adminmode ? "mode=1&" : "", path, file[i], buftitle);
                        }
                }


                if (id[0]) {
                        printf ("<td>%s", userid_str (id));
                } else {
                        printf ("<td>&nbsp;");
                }
                printf ("<td>%6.6s %s", Ctime (file_time (buf)) + 4,
                        Ctime (file_time (buf)) + 20);
        }




        printf
        ("</table><center>");



        if (total <= 0)
                if(pcmode)
                        printf("<br><< ������ >><br>");
                else
                        printf("<br><< Ŀǰû������ >><br>");


        printf("<hr color=#65c8aa> </form>");
        printf("<br><a href='javascript:history.go(-1)'>������һҳ</a> ");
        if (boardmode)
                printf ("<a href=bbsdoc?board=%s>��������</a>", s);
        if(pcmode&&adminmode) {
                printf("<br><a href=bbsdoc?board=PersonalCorpus>��վ�����ļ���</a>,��ӭ���Ĺ���,�����ļ����辭��,����BUG");

        }
        http_quit ();
}
