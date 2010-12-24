/*$Id: bbsgdoc.c,v 1.3 2009-10-30 04:06:45 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "showboard.h"

char *eff_size(), buflink[512], bufnav[512], filename[80];
int my_t_lines;
struct shortfile *x1;

int main()
{
        FILE *fp;
        char board[80], dir[80], *ptr;
        struct fileheader x;
        int i, start, total, chart=0, showhead=0;
        init_all();
        modify_mode(u_info,READING);	//bluetent
        my_t_lines=atoi(getparm("my_t_lines"));
        strsncpy(board, getparm("board"), 32);
        x1=getbcache(board);

        if(x1==0)
                http_fatal("�����������");
        strcpy(board, x1->filename);
        board_ban_ip(board);
        if (!canenterboard (currentuser.userid, board))
                http_fatal ("���Ѿ���ȡ�����뱾���Ȩ��");
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("�����������");
        sprintf(dir, "boards/%s/.DIGEST", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("�����������Ŀ¼");
        total=file_size(dir)/sizeof(struct fileheader);
        start=atoi(getparm("start"));
        if(my_t_lines<10 || my_t_lines>40)
                my_t_lines=20;
        if(strlen(getparm("start"))==0 || start>total-my_t_lines)
                start=total-my_t_lines;
        if(start<0)
                start=0;
        strcpy(u_info->board, board);

        printf("<script language=JavaScript>\
               \
               function MM_jumpMenu(selObj){\
               eval(\"window.location='\"+selObj.options[selObj.selectedIndex].value+\"'\");\
       }\
               \
               </script>\
               <script language=JavaScript src=/scrolldown.js></script>\
               <script language=JavaScript src=/js/common.js></script>");
        printf("<meta http-equiv=pragma content=no-cache>");
        printf("<body background=%s  bgproperties=fixed leftmargin=0 topmargin=0><a name=top></a><table align=center width=600 border=0 cellpadding=1 cellspacing=1 bgcolor=#%s>\
               <tr align=left valign=center height=20>\
               <td width=*>��ժ��: %s (%s) ����: %s</td>",mytheme.bgpath,mytheme.bar, board, x1->title+11, (!strcmp(userid_str(x1->BM), ""))?"��":userid_str(x1->BM));


        printf("<td align=right><a href=\"javascript:menu('board_info','')\">���ڱ���</a></td>");
        printf("<td width=160>ģʽѡ��\
               <SELECT NAME=Navagation onChange=\"MM_jumpMenu(this)\">\
               <option value=bbsdoc?board=%s>������</option>\
               <option value=bbstdoc?board=%s>ͬ����ģʽ</option>\
               <option value=bbsgdoc?board=%s selected>��ժ��</option>\
               <option value=bbsmdoc?board=%s>����ģʽ</option>\
               </SELECT>\
               </td>\
               </tr>\
               </table>\
               ", board, board, board, board);
        if(total<=0)
                http_fatal("����ժ��Ŀǰû������");
        showhead=atoi(getparm("my_showhead_mode"));

        showthesis(board);

        if(!showhead) {

                printf("<div id=board_info style='display:none'>");
                printf("<table align=center width=600 border=0 cellpadding=4><tr height=60><td align=left width=150><font color=#009966>��</font>  <a href=bbsnot?board=%s>���廭��</a><br><font color=#009966>��</font>  <a href=bbsbanshow?board=%s>����б�</a><br><font color=#009966>��</font>  <a href=bbsbrdchg?board=%s onclick='return confirm(\"�Ƿ��л�%s��İ����ղ�״̬?\")'>�л��ղ�״̬</a><br></td>", board, board, board, board);

                chart=atoi(getparm("chart"));
                if(chart==0) {
                        chart=chartpos(board);
                }
                printf("<td align=left width=*>\
                       <font color=#009966>��</font> ��������������<font color=red>%d</font><br>\
                       <font color=#009966>��</font> ������������<font color=red>%d</font>(<a href=bbstopb10>ȫ��</a>) �������ߣ�<font color=red>%d</font>(<a href=bbsinboard>����</a>)<br>\
                       <font color=#009966>��</font> ����Ϊ%sת�Ű��棬��������%s��������<br>\
                       </td>\
                       <td align=left width=150>\
                       <font color=#009966>��</font> <a href=\"javascript:void open('bbsadvice', '','width=300,height=200')\">��ϵͳ���ܵ�����뽨��</a><br>\
                       <font color=#009966>��</font> <a href=bbshelp?file=digest>��ҳ������Ϣ</a><br>\
                       </td>\
                       </tr>\
                       </table>\
                       ", total, chart, *((int*)(x1->filename+72)), (x1->flag & OUT_FLAG)? "" : "��", junkboard(board)? "��" : "");

                printf("</div>");
        }
        printf("<table align=center width=600 border=0 cellpadding=1 cellspacing=1>\
               <tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        shownav(board, start);
        printf(bufnav);
        printf("<font color=#009966>��</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr><td>");
        printf("<table width=100%%><tr><td align=center>");
        showlink(board, total, start, my_t_lines, chart);
        printf(buflink);
        printf("<td align=right>[<a href=#bottom>���л����ײ�</a>]</td></tr></table></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table>");
        printf("<table align=center width=600 border=0 cellpadding=1 cellspacing=1><tr bgColor=#%s><td align=center width=30>���</td><td align=center width=80>����</td><td align=center width=80>����</td><td align=center width=*>����</td><td align=center width=30>����</td></tr>",mytheme.bar);

        showtop(board);


        if(fp) {
                fseek(fp, start*sizeof(struct fileheader), SEEK_SET);
                for(i=0; i<my_t_lines; i++) {
                        if(fread(&x, sizeof(x), 1, fp)<=0)
                                break;
                        sprintf(filename, "boards/%s/%s", board, x.filename);
                        printf("<tr height=1><td align=right width=30 bgColor=#%s>%d</td>",mytheme.con, start+i+1);
                        printf("<td align=left width=80>%s</td>", userid_str(x.owner));
                        printf("<td align=center width=80>%12.12s</td>", Ctime(atoi(x.filename+2))+4);
                        printf("<td align=left width=*><a href=\"newcon?board=%s&file=%s \">%s%46.46s </a></td>", board, x.filename, /*start+i,*/
                               strncmp(x.title, "Re: ", 4) ? "�� " : "",
                               void1(nohtml(x.title)));
                        printf("<td align=right width=30>%s</td>", eff_size(filename));
                }
                fclose(fp);
        }
        printf("</table><table align=center width=600 border=0 cellpadding=1 cellspacing=1><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr><td><table width=100%%><tr><td align=center>");
        printf(buflink);
        printf("</td><td align=right>[<a href=#top>���л�������</a>]</td></tr></table></td></tr>");
        printf("<tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        printf(bufnav);
        printf("<font color=#009966>��</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table></center><a name=bottom></a>");

        printf("\
               <center>\
               <form action=bbsgdoc?board=%s  method=post>\
               ��Ҫ��ת���� <input    style='height:16px; border:1px solid #404040' type=text name=start maxlength=4 size=4 onmousemove='this.focus()'>ƪ����!</form>\
               <br>\
               </center>\
               ",board);

        check_msg();//bluetent 2002.10.31
        http_quit();
}

/* ����������ӵ�bufnav */
int shownav(char *board, int start)
{
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbspst?board=%s>��������</a>", bufnav, board);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsbfind?board=%s>��������</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsclear?board=%s&start=%d>���δ�����</a>", bufnav, board, start);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbs0an?path=%s>������</a>", bufnav, anno_path_of(board));

        sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsdoc?board=%s>������</a>",bufnav, board);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbstdoc?board=%s>ͬ�����Ķ�</a>",bufnav, board);


        //sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsfdoc?board=%s>����������</a>", bufnav, board);
        if(x1->flag&VOTE_FLAG)
                sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsvote?board=%s><font color=red>�μ�ͶƱ</font></a>", bufnav, board);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=javascript:self.print()>��ӡ��ҳ</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=javascript:location.reload()>ˢ�±�ҳ</a>", bufnav);
        return 1;
}

/* ���ҳ�����ӵ�buflink */
int showlink(char *board, int total, int start, int my_t_lines, int chart)
{
        int i, curr, first;
        char chartbuf[16];
        if(chart>0)
                sprintf(chartbuf, "&chart=%d", chart);
        else
                strcpy(chartbuf, "");
        sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=0%s>��ҳ</a> ", buflink, board, chartbuf);
        if(start>9*my_t_lines)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>ǰʮҳ</a> ", buflink, board, start-my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰʮҳ</font> ", buflink);
        if(start>4*my_t_lines)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>ǰ��ҳ</a> ", buflink, board, start-my_t_lines*5, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰ��ҳ</font> ", buflink);
        if(start>1*my_t_lines)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>ǰ��ҳ</a> ", buflink, board, start-my_t_lines*2, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰ��ҳ</font> ", buflink);
        if(start>0)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>ǰһҳ</a> ", buflink, board, start-my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰһҳ</font> ", buflink);

        sprintf(buflink, "%s(<font color=red>%d</font> - <font color=red>%d</font>) ", buflink, start+1, start+my_t_lines);
        if(start<total-my_t_lines)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>��һҳ</a> ", buflink, board, start+my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>��һҳ</font> ", buflink);
        if(start<total-2*my_t_lines)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>���ҳ</a> ", buflink, board, start+2*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>���ҳ</font> ", buflink);
        if(start<total-5*my_t_lines)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>����ҳ</a> ", buflink, board, start+5*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>����ҳ</font> ", buflink);
        if(start<total-10*my_t_lines)
                sprintf(buflink, "%s<a href=bbsgdoc?board=%s&start=%d%s>��ʮҳ</a> ", buflink, board, start+my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>��ʮҳ</font> ", buflink);
        sprintf(buflink, "%s<a href=bbsgdoc?board=%s%s>ĩҳ</a> ", buflink, board, chartbuf);

        return 1;
}

showtop(char *board)
{
        char path[STRLEN], buf[STRLEN];
        struct fileheader f;

        sprintf(path, "boards/%s/.top", board);
        if(!file_exist(path))
                return;
        else {

                FILE *fp=fopen(path,"r");
                if(fp!=NULL) {
                        while(1) {
                                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                                        break;
                                printf("<tr><td><img src=/image/hot.gif>");
                                //printf("<td align=center width=30>%s</td>", "�ö�");
                                printf("<td align=left width=80>%s</td>", userid_str(f.owner));
                                printf("<td align=center width=80>%12.12s", Ctime(atoi(f.filename+2))+4);
                                printf("</td><td align=left width=*><a href=bbstcon?board=%s&file=%s>%s%46.46s </a></td>", board, f.filename,
                                       strncmp(f.title, "Re: ", 4) ? "�� " : "",f.title);
                                printf("<td align=right width=30>%s</td></tr>", eff_size(f.filename));

                        }  //while

                }//if fp
        } //elsee
}


showthesis(char *board)
{
        char path[STRLEN], buf[STRLEN];
        struct rmd r;

        sprintf(path, "boards/%s/.thesis", board);
        if(!file_exist(path)) {
                return;
        } else {

                FILE *fp=fopen(path,"r");
                if(fp!=NULL) {

                        printf("</center>�����Ƽ�<center>");
                        printf("<table width=100%>");


                        int i=0;
                        while(1) {
                                if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                        break;


                                if(i%2==0)
                                        printf("<tr>");

                                if(r.sign==1)

                                        printf("<td>��</td>");
                                else
                                        printf("<td>&nbsp</td>");

                                printf("<td align=left width=50%><a href=\"bbstcon?board=%s&file=%s\">%s%46.46s </a>", board, r.en,
                                       strncmp(r.cn, "Re: ", 4) ? "�� " : "",r.cn);

                                if(i%2==1)
                                        printf("</tr>");
                                i++;
                        }  //while


                        printf("</table>");


                }//if fp
        } //elsee
}
