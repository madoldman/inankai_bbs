/*$Id: bbsmdoc.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"
#include "showboard.h"
char bufnav[1024], buflink[1024];
int main()
{
        FILE *fp;
        char board[80], dir[80], *ptr, flag[80];
        struct shortfile *x1;
        struct fileheader x;
        int i, start, total, my_t_lines=0, chart=0, showhead=0;
        init_all();
        strsncpy(board, getparm("board"), 32);
        my_t_lines=atoi(getparm("my_t_lines"));
        if(my_t_lines<10 || my_t_lines>40)
                my_t_lines=20;
        x1=getbcache(board);
        if(x1==0)
                http_fatal("�����������");
        strcpy(board, x1->filename);
        board_ban_ip(board);
        if (!canenterboard (currentuser.userid, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal ("���Ѿ���ȡ�����뱾���Ȩ��");
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("�����������");
        if(!has_BM_perm(&currentuser, board))
                http_fatal("��û��Ȩ�޷��ʱ�ҳ");

        modify_mode(u_info,READING);	//bluetent
        sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("�����������Ŀ¼");
        total=file_size(dir)/sizeof(struct fileheader);
        start=atoi(getparm("start"));
        if(strlen(getparm("start"))==0 || start>total-my_t_lines)
                start=total-my_t_lines;
        if(start<0)
                start=0;
        printf("<nobr><center>\n");
        strcpy(u_info->board, board);
        /*	printf("%s -- [������: %s] ����[%s] ������[%d]<hr color=green>\n",
        		BBSNAME, board, userid_str(x1->BM), total);
        	if(total<=0) http_fatal("��������Ŀǰû������");
        	printf("<form name=form1 method=post action=bbsman>\n");
              	printf("<table width=613>\n");
              	printf("<tr><td>���<td>����<td>״̬<td>����<td>����<td>����\n");
        	fseek(fp, start*sizeof(struct fileheader), SEEK_SET);*/
        printf("<script language=JavaScript>\
               \
               function MM_jumpMenu(selObj){\
               eval(\"window.location='\"+selObj.options[selObj.selectedIndex].value+\"'\");\
       }\
               function SubmitFlag(flagmode){\
               document.form1.mode.value=flagmode;\
               document.form1.submit();\
       }\
       function selrange(x)\
       {if(event.shiftKey){var min=x;var max=document.getElementById('lastsel').value;if(min>max){var tmp=min;min=max;max=tmp;}var stt=document.getElementById('chk'+x).checked;for(var i=min;i<=max;i++)document.getElementById('chk'+i).checked=stt;}document.getElementById('lastsel').value=x;}\
               </script>\
               <script language=JavaScript src=/scrolldown.js></script>\
               <script language=JavaScript src=/js/common.js></script>");
        printf("<meta http-equiv=pragma content=no-cache>");
        printf("<body background=%s bgproperties=fixed leftmargin=0 topmargin=0><a name=top></a><form name=form1 action=bbsman method=post><table align=center width=600 border=0 cellpadding=1 cellspacing=1 bgcolor=#%s>\
               <tr align=left valign=center height=20>\
               <td width=*>����ģʽ: %s (%s) ����: %s</td>",mytheme.bgpath,mytheme.bar, board, x1->title+11, (!strcmp(userid_str(x1->BM), ""))?"��":userid_str(x1->BM));
        printf("<td align=right>    <a href=\"javascript:menu('board_info',0)\">���ڱ���</a></td>");
        printf("<td width=160>ģʽѡ��\
               <SELECT NAME=Navagation onChange=\"MM_jumpMenu(this)\">\
               <option value=bbsdoc?board=%s>������</option>\
               <option value=bbstdoc?board=%s>ͬ����ģʽ</option>\
               <option value=bbsgdoc?board=%s>��ժ��</option>\
               <option value=bbsmdoc?board=%s selected>����ģʽ</option>\
               <option value=bbsdoc?board=%s&junk=yes>����ģʽ</option>\
               <option value=bbsatt?board=%s>�ļ�չ��</option>\
               </SELECT>\
               </td>\
               </tr>\
               </table>\
               ", board, board, board, board,board,board);
        //huangxu@060407:����ģʽ
        if(total<=0)
                http_fatal("��������Ŀǰû������");

        showhead=atoi(getparm("my_showhead_mode"));

        showthesis(board);

        if(!showhead) {
                printf("<div id=board_info style='display:none'>");


                printf("<table align=center width=600 border=0 cellpadding=4><tr height=60><td align=left width=150><font color=#009966>��</font>  <a href=bbsnot?board=%s>���廭��</a><br><font color=#009966>��</font>  <a href=bbsbanshow?board=%s>����б�</a><br><font color=#009966>��</font>  <a href=bbsbrdchg?board=%s onclick='return confirm(\"�Ƿ��л�%s��İ����ղ�״̬?\")'>�л��ղ�״̬</a><br></td>", board, board, board, board);

                chart=atoi(getparm("chart"));
                if(chart==0) {
                        chart=chartpos(board);
                }

                /*	printf("<td align=left width=*>
                <font color=#009966>��</font> ������������<font color=red>%d</font><br>
                <font color=#009966>��</font> ������������<font color=red>%3d</font> (<a href=bbstopb10>�鿴ȫ������</a>)<br>
                <font color=#009966>��</font> ����Ϊ%sת�Ű��棬��������%s��������<br>
                </td>
                <td align=left width=150>
                <font color=#009966>��</font> <a href=\"javascript:void open('bbsadvice', '','width=300,height=200')\">��ϵͳ���ܵ�����뽨��</a><br>
                <font color=#009966>��</font> <a href=bbshelp?file=reading>��ҳ������Ϣ</a><br>
                </td>
                </tr>
                </table>
                ", total, chart, (x1->flag & OUT_FLAG)? "" : "��", junkboard(board)? "��" : "");*/
                printf("<td align=left width=*>\
                       <font color=#009966>��</font> ��������������<font color=red>%d</font><br>\
                       <font color=#009966>��</font> ������������<font color=red>%d</font>(<a href=bbstopb10>ȫ��</a>) �������ߣ�<font color=red>%d</font>(<a href=bbsinboard>����</a>)<br>\
                       <font color=#009966>��</font> ����Ϊ%sת�Ű��棬��������%s��������<br>\
                       </td>\
                       <td align=left width=150>\
                       <font color=#009966>��</font> <a href=\"javascript:void open('bbsadvice', '','width=300,height=200')\">��ϵͳ���ܵ�����뽨��</a><br>\
                       <font color=#009966>��</font> <a href=bbshelp?file=reading>��ҳ������Ϣ</a><br>\
                       </td>\
                       </tr>\
                       </table>\
                       ", total, chart, *((int*)(x1->filename+72)), (x1->flag & OUT_FLAG)? "" : "��", junkboard(board)? "��" : "");



                printf("</div>");

        }
        printf("<table align=center width=95% border=0 cellpadding=1 cellspacing=1>\
               <tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        shownav(board, start);
        printf(bufnav);
        printf("<font color=#009966>��</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr><td>");
        printf("<table width=100%%><tr><td align=center>");
        showlink(board, total, start, my_t_lines, chart);
        printf(buflink);
        printf("<td align=right>[<a href=#bottom>���л����ײ�</a>]</td></tr></table></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table>");
        printf("<table align=center width=95% border=0 cellpadding=1 cellspacing=1><tr bgColor=%s  height=1><td align=center width=30>���</td><td align=center width=30>����</td><td align=center width=30>״̬</td><td align=center width=80>����</td><td align=center width=80>����</td><td align=center width=*>����</td></tr>",mytheme.bar);
        if(!fp)
                http_fatal("error");


        adminshowtop(board);

        fseek(fp, start*sizeof(struct fileheader), SEEK_SET);
        for(i=0; i<my_t_lines; i++) {
                char filename[80];
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                strcpy(flag, "");
                ptr=flag_str2(x.accessed[0], brc_has_read(x.filename));
                if(ptr[0]=='N') {
                        sprintf(flag, "<font color=#909090 face=Arial>&nbsp;</font>");
                }
                if(ptr[0]=='I'||ptr[0]=='i') {
                        sprintf(flag, "<font color=red face=Arial>%c</font>", ptr[0]);
                }

                if(ptr[0]=='M'||ptr[0]=='m') {
                        sprintf(flag, "<font color=red face=Arial>m</font>");
                }
                if(ptr[0]=='G'||ptr[0]=='g') {
                        sprintf(flag, "<font color=#009933 face=Arial>g</font>");
                }
                if(ptr[0]=='B'||ptr[0]=='b') {
                        sprintf(flag, "<font color=#9966FF face=Arial>b</font>");
                }
                if(ptr[1]=='X'||ptr[1]=='x') {
                        if(ptr[0]=='N')
                                sprintf(flag, "<font color=blue face=Arial>x</font>");
                        else
                                sprintf(flag, "%s<font color=blue face=Arial>x</font>", flag);
                }
                sprintf(filename, "boards/%s/%s", board, x.filename);
                printf("<tr height=1><td bgColor=#%s align=right>%d", mytheme.con,start+i+1);
                printf("<td align=center><input style='height:18px' name=box%s type=checkbox id='chk%d' onclick='selrange(%d);' >", x.filename,i,i);
                printf("<td align=center>%s<td>%s", flag, userid_str(x.owner));
                printf("<td align=center>%12.12s", Ctime(atoi(x.filename+2))+4);
                printf("<td align=left><a href=newcon?board=%s&file=%s&num=%d&mode=3>%s", board, x.filename, start+i, strncmp(x.title, "Re: ", 4) ? "�� " : "");
                hprintf("%46.46s ", x.title);
                printf("</a>");
        }
        fclose(fp);
       	printf("<input type=hidden name=lastsel id=lastsel value=0 />");
        printf("<input type=hidden name=mode value='1'>\n");
        printf("<input type=hidden name=start value='%d'>\n", start);
        printf("<input type=hidden name=board value='%s'>\n", board);
        printf("\n");
        printf("</table><table align=center width=600 border=0 cellpadding=1 cellspacing=1><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr><td><table width=100%%><tr><td align=center>");
        printf(buflink);
        printf("</td><td align=right>[<a href=#top>���л�������</a>]</td></tr></table></td></tr>");
        printf("<tr align=center><td>�ö����ͣ�<input type=radio name=topstyle value='%d' checked>��ʾ&nbsp;\
        <input type=radio name=topstyle value='%d'>����&nbsp;\
        <input type=radio name=topstyle value='%d'>���&nbsp;<input type=radio name=topstyle value='%d'>�Ƽ�&nbsp;\
        <input type=radio name=topstyle value='%d'>����&nbsp;<input type=radio name=topstyle value='%d'>����&nbsp;",
        0,FILE_STYLE1,FILE_STYLE2,FILE_STYLE3,FILE_STYLE4,FILE_STYLE5);//huangxu@060407:������ɫ
        char cmode=0;//huangxu@070425:����
        char boardallow[80],boardwrite[80];
        sprintf(boardallow,"boards/%s/board.allow",board);
        sprintf(boardwrite,"boards/%s/board.write",board);
        if (file_exist(boardallow)) cmode |= 1;
        if (file_exist(boardwrite)) cmode |= 2;
        if(!(cmode & 2)&&(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_OBOARDS)||(cmode & 1)))
        {
        	printf ("<a href=\"/js/userlist.htm?file1=257&file2=%s&title=%s.board.allow\" target=\"_blank\">��Ա����</a>&nbsp;", board, board);
        }
        if(!(cmode & 1)&&(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_OBOARDS)||(cmode & 2)))
        {
        	printf ("<a href=\"/js/userlist.htm?file1=258&file2=%s&title=%s.board.write\" target=\"_blank\">��������</a>", board, board);
        }
        printf("</td></tr>");
        printf("<tr><td><table width=100% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        printf(bufnav);
        printf("<font color=#009966>��</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table></center><a name=bottom></a></form>");
        //huangxu@060407:����<form>�ķ�Χ
        printf("  <center>\
               <form action=bbsmdoc?board=%s  method=post>\
               ��Ҫ��ת���� <input    style='height:16px; border:1px solid #404040' type=text name=start maxlength=4 size=4 onmousemove='this.focus()'>ƪ����!</form>\
               <br>\
               </center>",board);
        //check_msg();//bluetent 2002.10.31
        http_quit();
}

/* ����������ӵ�bufnav */
int shownav(char *board, int start)
{
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbspst?board=%s>��������</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsbfind?board=%s>��������</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsclear?board=%s&start=%d>���δ�����</a>", bufnav, board, start);
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbs0an?path=%s>�������Ķ�</a>", bufnav, anno_path_of(board));
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsfdoc?board=%s>����������</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>��</font><a href=javascript:self.print()>��ӡ��ҳ</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsmnote?board=%s>���廭��</a>", bufnav, board);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsdenyall?board=%s>��������</a>", bufnav, board);
        //	printf("<input type=button value=ɾ�� onclick='document.form1.mode.value=1; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>��</font><a href='javascript:SubmitFlag(1)'>ɾ��</a>", bufnav);
        //	printf("<input type=button value=���/���M onclick='document.form1.mode.value=2; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>��</font><a href='javascript:SubmitFlag(2)'>�л�M</a>", bufnav);
        //	printf("<input type=button value=���/���G onclick='document.form1.mode.value=3; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>��</font><a href='javascript:SubmitFlag(3)'>�л�G</a>", bufnav);
        //	printf("<input type=button value=���/�������Re  onclick='document.form1.mode.value=4; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>��</font><a href='javascript:SubmitFlag(4)'>�л��ظ�����</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=javascript:location.reload()>ˢ��</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href='javascript:SubmitFlag(6)'>�Ƽ�����</a>", bufnav);

        sprintf(bufnav, "%s<font color=#009966>��</font><a href='javascript:SubmitFlag(7)'>�ö�</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href='javascript:SubmitFlag(8)'>���뾫���� </a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>��</font><a href=bbsdigest?mode=1>�鿴˿·</a>", bufnav);
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
        sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=0%s>��ҳ</a> ", buflink, board, chartbuf);
        if(start>9*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>ǰʮҳ</a> ", buflink, board, start-my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰʮҳ</font> ", buflink);
        if(start>4*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>ǰ��ҳ</a> ", buflink, board, start-my_t_lines*5, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰ��ҳ</font> ", buflink);
        if(start>1*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>ǰ��ҳ</a> ", buflink, board, start-my_t_lines*2, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰ��ҳ</font> ", buflink);
        if(start>0)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>ǰһҳ</a> ", buflink, board, start-my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>ǰһҳ</font> ", buflink);

        sprintf(buflink, "%s(<font color=red>%d</font> - <font color=red>%d</font>) ", buflink, start+1, start+my_t_lines);
        if(start<total-my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>��һҳ</a> ", buflink, board, start+my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>��һҳ</font> ", buflink);
        if(start<total-2*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>���ҳ</a> ", buflink, board, start+2*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>���ҳ</font> ", buflink);
        if(start<total-5*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>����ҳ</a> ", buflink, board, start+5*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>����ҳ</font> ", buflink);
        if(start<total-10*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>��ʮҳ</a> ", buflink, board, start+my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>��ʮҳ</font> ", buflink);
        sprintf(buflink, "%s<a href=bbsmdoc?board=%s%s>ĩҳ</a> ", buflink, board, chartbuf);

        return 1;
}

adminshowtop(char *board)
{

        char path[STRLEN], buf[STRLEN];
        struct fileheader f;
        //char topstr[]={"��ʾ","����","���","�Ƽ�","����","����"};

        sprintf(path, "boards/%s/.top", board);
        if(!file_exist(path))
                return;
        else {

                FILE *fp=fopen(path,"r");
                if(fp!=NULL) {
                        while(1) {
                                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                                        break;
                                printf("<tr><td><img src=/image/hot.gif></td>");

                                printf("<td align=center><a href=bbsmktop?board=%s&&file=%s>ȡ��</a></td>",board,f.filename);
                                printf("<td align=center>");
                                if (f.accessed[1] & FILE_STYLE1)
                                	printf("<font class=c31>����</font>");
                                else if (f.accessed[1] & FILE_STYLE2)
                                	printf("<font class=c35>���</font>");
                                else if (f.accessed[1] & FILE_STYLE3)
                                	printf("<font class=c32>�Ƽ�</font>");
                                else if (f.accessed[1] & FILE_STYLE4)
                                	printf("<font class=c34>����</font>");
                                else if (f.accessed[1] & FILE_STYLE5)
                                	printf("<font class=c36>����</font>");
                                else 
                                	printf("<font class=c33>��ʾ</font>");//huangxu@060407:����ö�
                                printf("</td><td align=left width=80>%s</td>", userid_str(f.owner));
                                printf("<td align=center width=80>%12.12s", Ctime(atoi(f.filename+2))+4);
                                printf("</td><td align=left width=*><a href=bbstcon?board=%s&file=%s>%s%46.46s </a></td>", board, f.filename,
                                       strncmp(f.title, "Re: ", 4) ? "�� " : "",f.title);


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
                                if(r.sign==0)
                                        printf("<a href=\"rmd?mode=1&&board=%s&&file=%s\"><ȡ��></a></td>",board,r.en);

                                if(i%2==1)
                                        printf("</tr>");
                                i++;
                        }  //while
                        printf("</table>");


                }//if fp
        } //elsee
}

