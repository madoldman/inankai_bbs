/*$Id: bbs0an.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
  参数mode=1为管理模式
  path以0Announce为主目录
  类似/groups,没有/结尾
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
                http_fatal ("此目录不存在");
        sprintf (names, "0Announce%s/.Names", path);
        if (!file_exist (names))
                http_fatal ("目录不存在");

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
                        showinfo ("对于该讨论区,您没有读权限.所以,无法查看其精华区.");
                        return 0;
                }

        int adminmode = atoi (getparm ("mode"));
        if (adminmode) {
                if (!hasperm (mode, s)) {
                        showinfo ("你无权查看该页!");
                        return 0;
                }

        }

        fp = fopen (names, "r");
        if (fp == 0) {
                showinfo ("$s无法打开", names);
                return 0;
        }

        while (1) {
                if (fgets (buf, 511, fp) == 0)
                        break;
                if (!strncmp (buf, "# Title=", 8))
                        strcpy (title, buf + 8);
                if (!strncmp (buf, "Name=", 5) && total < 1023) {
//                        if (!strncmp (buf+5, "<GUESTBOOK>", 11)) {
//                                sprintf (name[total], "|留言本|%s",trim (buf + 16));

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
                http_fatal ("错误的目录");
        if (strstr (title, "BMS"))
                if (boardmode)
                        if (!has_BM_perm (&currentuser, s)) {
                                showinfo ("你无权访问本页");
                                return 0;
                        }


        if (pcmode) {
                printf (" <font size=6>%s</font>   欢迎光临<font color=green>%s</font>的文集", pctitle, s);
                printf ("<a href=bbspstmail?userid=%s>写信问候</a>", s);
                printf("         ");
                printf ("<a href=/person/%c/%s.html.tar.gz>   下载  </a>",toupper(s[0]),s);
                printf("<a href=bbsdoc?board=PersonalCorpus>  推荐 </a>");
                printf("<a href=bbsdread?path=/PersonalCorpus/%c/%s> 导读 </a>",toupper(s[0]),s);
        } else
                printf ("<center>        <img src=/jinghua_b.gif>");
        char tmpbuf[16];
        printf (" <br><img src=/visit.gif> 本目录浏览次数: %d</center>\n",
                get_count (path));
        printf ("<a href=bbs0an?path=/abbs> 本站使用帮助</a>");
        if (adminmode == 0) {
                if (boardmode) {
                        strncpy (tmpbuf, path, 15);
                        printf
                        ("<a href=bbsdread?path=%s/%s> 目录导读</a><a href=bbsdoc?board=%s> 本讨论区",
                         tmpbuf, s, s);
                }
                if (hasperm (mode, s)) {
                        printf("<a href=bbs0an?mode=1&path=%s>   管理模式</a>", path);

                        printf("");/*I add this,and all is right then.*/

                        printf("<a href=digestpst?mode=1&path=%s>  发表文章</a>",path);
                        printf ("");
                }
                if(strstr(title,"<GUESTBOOK>"))
                        printf("<a href=digestpst?mode=2&path=%s>  我要留言</a>",path);
                char cn[1024], tmpbuf[256];
                sprintf (tmpbuf, "0Announce%s", path);
                cnpath (tmpbuf, cn);
                printf ("<br>%s", cn);
        } else {
                printf ("<br><a href=bbs0an?path=%s>普通模式</a>", path);
                printf
                ("<a href='javascript:SubmitFlag(2)'> 新建</a><a href='javascript:SubmitFlag(3)'> 复制</a><a href='javascript:SubmitFlag(4)'> 粘贴</a><a href=\"javascript:if(confirm('确定要删除所有这些文件吗?注意,您只能删除空目录.')) SubmitFlag(5)\">    删除  </a> <a href=\"javascript:getvalue('请输入新名字:',6)\">重命名</a><a  href=\"javascript:getvalue('请输入新次序:',7)\"> 移动次序</a>");
//                if (boardmode)//huangxu@060715:为嘛个人文集不行？
                        printf
                        ("<a href='javascript:SubmitFlag(1)'> 查看丝路</a><a href='javascript:SubmitFlag(11)'> 设为丝路</a>");
        }

        printf ("<form method=post name=form1 action=bbsdigest?path=%s>", path);
        printf ("<input type=hidden name=mode>");
        printf ("<input type=hidden name=a>");
        printf
        ("<center><table cellspacing=1 cellpadding=2 border=0 width=610 align=center>\n");
        printf
        ("<tr bgcolor=#%s class=title><td align=middle width=30><font color=%s>序号</font>",
         mytheme.bar,mytheme.lf);
        if (adminmode)
                printf ("<td>管理");
        printf
        ("<td> <td align=middle width=*><font color=%s>标题</font><td align=middle><font color=%s  width=100>%s</font><td align=middle><font color=%s width=60>日期</font>",mytheme.lf,mytheme.lf,pcmode?"作者":"整理",mytheme.lf);
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
                        printf ("<td align=middle width=30>[错误] <td>%s",
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
                                        //   printf("<a href=bbsedit?mode=2&path=%s%s>修改</a>",path,file[i]);
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
                        printf("<br><< 建设中 >><br>");
                else
                        printf("<br><< 目前没有文章 >><br>");


        printf("<hr color=#65c8aa> </form>");
        printf("<br><a href='javascript:history.go(-1)'>返回上一页</a> ");
        if (boardmode)
                printf ("<a href=bbsdoc?board=%s>本讨论区</a>", s);
        if(pcmode&&adminmode) {
                printf("<br><a href=bbsdoc?board=PersonalCorpus>本站个人文集区</a>,欢迎您的光临,交流文集建设经验,报告BUG");

        }
        http_quit ();
}
