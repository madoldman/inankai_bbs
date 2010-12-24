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
                http_fatal("错误的讨论区");
        strcpy(board, x1->filename);
        board_ban_ip(board);
        if (!canenterboard (currentuser.userid, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal ("您已经被取消进入本板的权限");
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("错误的讨论区");
        if(!has_BM_perm(&currentuser, board))
                http_fatal("您没有权限访问本页");

        modify_mode(u_info,READING);	//bluetent
        sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("错误的讨论区目录");
        total=file_size(dir)/sizeof(struct fileheader);
        start=atoi(getparm("start"));
        if(strlen(getparm("start"))==0 || start>total-my_t_lines)
                start=total-my_t_lines;
        if(start<0)
                start=0;
        printf("<nobr><center>\n");
        strcpy(u_info->board, board);
        /*	printf("%s -- [讨论区: %s] 板主[%s] 文章数[%d]<hr color=green>\n",
        		BBSNAME, board, userid_str(x1->BM), total);
        	if(total<=0) http_fatal("本讨论区目前没有文章");
        	printf("<form name=form1 method=post action=bbsman>\n");
              	printf("<table width=613>\n");
              	printf("<tr><td>序号<td>管理<td>状态<td>作者<td>日期<td>标题\n");
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
               <td width=*>管理模式: %s (%s) 板主: %s</td>",mytheme.bgpath,mytheme.bar, board, x1->title+11, (!strcmp(userid_str(x1->BM), ""))?"无":userid_str(x1->BM));
        printf("<td align=right>    <a href=\"javascript:menu('board_info',0)\">关于本板</a></td>");
        printf("<td width=160>模式选择：\
               <SELECT NAME=Navagation onChange=\"MM_jumpMenu(this)\">\
               <option value=bbsdoc?board=%s>讨论区</option>\
               <option value=bbstdoc?board=%s>同主题模式</option>\
               <option value=bbsgdoc?board=%s>文摘区</option>\
               <option value=bbsmdoc?board=%s selected>管理模式</option>\
               <option value=bbsdoc?board=%s&junk=yes>回收模式</option>\
               <option value=bbsatt?board=%s>文件展区</option>\
               </SELECT>\
               </td>\
               </tr>\
               </table>\
               ", board, board, board, board,board,board);
        //huangxu@060407:回收模式
        if(total<=0)
                http_fatal("本讨论区目前没有文章");

        showhead=atoi(getparm("my_showhead_mode"));

        showthesis(board);

        if(!showhead) {
                printf("<div id=board_info style='display:none'>");


                printf("<table align=center width=600 border=0 cellpadding=4><tr height=60><td align=left width=150><font color=#009966>⊙</font>  <a href=bbsnot?board=%s>进板画面</a><br><font color=#009966>⊙</font>  <a href=bbsbanshow?board=%s>封禁列表</a><br><font color=#009966>⊙</font>  <a href=bbsbrdchg?board=%s onclick='return confirm(\"是否切换%s板的板面收藏状态?\")'>切换收藏状态</a><br></td>", board, board, board, board);

                chart=atoi(getparm("chart"));
                if(chart==0) {
                        chart=chartpos(board);
                }

                /*	printf("<td align=left width=*>
                <font color=#009966>⊙</font> 板面文章数：<font color=red>%d</font><br>
                <font color=#009966>⊙</font> 讨论区排名：<font color=red>%3d</font> (<a href=bbstopb10>查看全部排行</a>)<br>
                <font color=#009966>⊙</font> 本板为%s转信板面，发表文章%s算文章数<br>
                </td>
                <td align=left width=150>
                <font color=#009966>⊙</font> <a href=\"javascript:void open('bbsadvice', '','width=300,height=200')\">对系统功能的意见与建议</a><br>
                <font color=#009966>⊙</font> <a href=bbshelp?file=reading>本页帮助信息</a><br>
                </td>
                </tr>
                </table>
                ", total, chart, (x1->flag & OUT_FLAG)? "" : "非", junkboard(board)? "不" : "");*/
                printf("<td align=left width=*>\
                       <font color=#009966>⊙</font> 讨论区文章数：<font color=red>%d</font><br>\
                       <font color=#009966>⊙</font> 讨论区排名：<font color=red>%d</font>(<a href=bbstopb10>全部</a>) 板内在线：<font color=red>%d</font>(<a href=bbsinboard>排名</a>)<br>\
                       <font color=#009966>⊙</font> 本板为%s转信板面，发表文章%s算文章数<br>\
                       </td>\
                       <td align=left width=150>\
                       <font color=#009966>⊙</font> <a href=\"javascript:void open('bbsadvice', '','width=300,height=200')\">对系统功能的意见与建议</a><br>\
                       <font color=#009966>⊙</font> <a href=bbshelp?file=reading>本页帮助信息</a><br>\
                       </td>\
                       </tr>\
                       </table>\
                       ", total, chart, *((int*)(x1->filename+72)), (x1->flag & OUT_FLAG)? "" : "非", junkboard(board)? "不" : "");



                printf("</div>");

        }
        printf("<table align=center width=95% border=0 cellpadding=1 cellspacing=1>\
               <tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        shownav(board, start);
        printf(bufnav);
        printf("<font color=#009966>·</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr><td>");
        printf("<table width=100%%><tr><td align=center>");
        showlink(board, total, start, my_t_lines, chart);
        printf(buflink);
        printf("<td align=right>[<a href=#bottom>↓切换到底部</a>]</td></tr></table></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table>");
        printf("<table align=center width=95% border=0 cellpadding=1 cellspacing=1><tr bgColor=%s  height=1><td align=center width=30>序号</td><td align=center width=30>管理</td><td align=center width=30>状态</td><td align=center width=80>作者</td><td align=center width=80>日期</td><td align=center width=*>标题</td></tr>",mytheme.bar);
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
                printf("<td align=left><a href=newcon?board=%s&file=%s&num=%d&mode=3>%s", board, x.filename, start+i, strncmp(x.title, "Re: ", 4) ? "○ " : "");
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
        printf("</td><td align=right>[<a href=#top>↑切换到顶部</a>]</td></tr></table></td></tr>");
        printf("<tr align=center><td>置顶类型：<input type=radio name=topstyle value='%d' checked>提示&nbsp;\
        <input type=radio name=topstyle value='%d'>公告&nbsp;\
        <input type=radio name=topstyle value='%d'>板规&nbsp;<input type=radio name=topstyle value='%d'>推荐&nbsp;\
        <input type=radio name=topstyle value='%d'>精华&nbsp;<input type=radio name=topstyle value='%d'>其他&nbsp;",
        0,FILE_STYLE1,FILE_STYLE2,FILE_STYLE3,FILE_STYLE4,FILE_STYLE5);//huangxu@060407:多种颜色
        char cmode=0;//huangxu@070425:名单
        char boardallow[80],boardwrite[80];
        sprintf(boardallow,"boards/%s/board.allow",board);
        sprintf(boardwrite,"boards/%s/board.write",board);
        if (file_exist(boardallow)) cmode |= 1;
        if (file_exist(boardwrite)) cmode |= 2;
        if(!(cmode & 2)&&(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_OBOARDS)||(cmode & 1)))
        {
        	printf ("<a href=\"/js/userlist.htm?file1=257&file2=%s&title=%s.board.allow\" target=\"_blank\">成员名单</a>&nbsp;", board, board);
        }
        if(!(cmode & 1)&&(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_OBOARDS)||(cmode & 2)))
        {
        	printf ("<a href=\"/js/userlist.htm?file1=258&file2=%s&title=%s.board.write\" target=\"_blank\">发文名单</a>", board, board);
        }
        printf("</td></tr>");
        printf("<tr><td><table width=100% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        printf(bufnav);
        printf("<font color=#009966>·</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table></center><a name=bottom></a></form>");
        //huangxu@060407:调整<form>的范围
        printf("  <center>\
               <form action=bbsmdoc?board=%s  method=post>\
               我要跳转到第 <input    style='height:16px; border:1px solid #404040' type=text name=start maxlength=4 size=4 onmousemove='this.focus()'>篇文章!</form>\
               <br>\
               </center>",board);
        //check_msg();//bluetent 2002.10.31
        http_quit();
}

/* 输出导航链接到bufnav */
int shownav(char *board, int start)
{
        //	sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbspst?board=%s>发表文章</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsbfind?board=%s>搜索文章</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsclear?board=%s&start=%d>清除未读标记</a>", bufnav, board, start);
        //	sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbs0an?path=%s>精华区阅读</a>", bufnav, anno_path_of(board));
        //	sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsfdoc?board=%s>本板上载区</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>·</font><a href=javascript:self.print()>打印本页</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsmnote?board=%s>进板画面</a>", bufnav, board);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsdenyall?board=%s>封人名单</a>", bufnav, board);
        //	printf("<input type=button value=删除 onclick='document.form1.mode.value=1; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>·</font><a href='javascript:SubmitFlag(1)'>删除</a>", bufnav);
        //	printf("<input type=button value=标记/清除M onclick='document.form1.mode.value=2; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>·</font><a href='javascript:SubmitFlag(2)'>切换M</a>", bufnav);
        //	printf("<input type=button value=标记/清除G onclick='document.form1.mode.value=3; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>·</font><a href='javascript:SubmitFlag(3)'>切换G</a>", bufnav);
        //	printf("<input type=button value=标记/清除不可Re  onclick='document.form1.mode.value=4; document.form1.submit();'>\n");
        sprintf(bufnav, "%s<font color=#009966>·</font><a href='javascript:SubmitFlag(4)'>切换回复属性</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=javascript:location.reload()>刷新</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href='javascript:SubmitFlag(6)'>推荐文章</a>", bufnav);

        sprintf(bufnav, "%s<font color=#009966>·</font><a href='javascript:SubmitFlag(7)'>置顶</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href='javascript:SubmitFlag(8)'>收入精华区 </a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsdigest?mode=1>查看丝路</a>", bufnav);
        return 1;
}

/* 输出页面链接到buflink */
int showlink(char *board, int total, int start, int my_t_lines, int chart)
{
        int i, curr, first;
        char chartbuf[16];
        if(chart>0)
                sprintf(chartbuf, "&chart=%d", chart);
        else
                strcpy(chartbuf, "");
        sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=0%s>首页</a> ", buflink, board, chartbuf);
        if(start>9*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>前十页</a> ", buflink, board, start-my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前十页</font> ", buflink);
        if(start>4*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>前五页</a> ", buflink, board, start-my_t_lines*5, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前五页</font> ", buflink);
        if(start>1*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>前二页</a> ", buflink, board, start-my_t_lines*2, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前二页</font> ", buflink);
        if(start>0)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>前一页</a> ", buflink, board, start-my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前一页</font> ", buflink);

        sprintf(buflink, "%s(<font color=red>%d</font> - <font color=red>%d</font>) ", buflink, start+1, start+my_t_lines);
        if(start<total-my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>后一页</a> ", buflink, board, start+my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后一页</font> ", buflink);
        if(start<total-2*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>后二页</a> ", buflink, board, start+2*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后二页</font> ", buflink);
        if(start<total-5*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>后五页</a> ", buflink, board, start+5*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后五页</font> ", buflink);
        if(start<total-10*my_t_lines)
                sprintf(buflink, "%s<a href=bbsmdoc?board=%s&start=%d%s>后十页</a> ", buflink, board, start+my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后十页</font> ", buflink);
        sprintf(buflink, "%s<a href=bbsmdoc?board=%s%s>末页</a> ", buflink, board, chartbuf);

        return 1;
}

adminshowtop(char *board)
{

        char path[STRLEN], buf[STRLEN];
        struct fileheader f;
        //char topstr[]={"提示","公告","板规","推荐","精华","其他"};

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

                                printf("<td align=center><a href=bbsmktop?board=%s&&file=%s>取消</a></td>",board,f.filename);
                                printf("<td align=center>");
                                if (f.accessed[1] & FILE_STYLE1)
                                	printf("<font class=c31>公告</font>");
                                else if (f.accessed[1] & FILE_STYLE2)
                                	printf("<font class=c35>板规</font>");
                                else if (f.accessed[1] & FILE_STYLE3)
                                	printf("<font class=c32>推荐</font>");
                                else if (f.accessed[1] & FILE_STYLE4)
                                	printf("<font class=c34>精华</font>");
                                else if (f.accessed[1] & FILE_STYLE5)
                                	printf("<font class=c36>其他</font>");
                                else 
                                	printf("<font class=c33>提示</font>");//huangxu@060407:多彩置顶
                                printf("</td><td align=left width=80>%s</td>", userid_str(f.owner));
                                printf("<td align=center width=80>%12.12s", Ctime(atoi(f.filename+2))+4);
                                printf("</td><td align=left width=*><a href=bbstcon?board=%s&file=%s>%s%46.46s </a></td>", board, f.filename,
                                       strncmp(f.title, "Re: ", 4) ? "○ " : "",f.title);


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
                        printf("</center>今日推荐<center>");
                        printf("<table width=100%>");


                        int i=0;
                        while(1) {
                                if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                        break;


                                if(i%2==0)
                                        printf("<tr>");

                                if(r.sign==1)

                                        printf("<td>◆</td>");
                                else
                                        printf("<td>&nbsp</td>");

                                printf("<td align=left width=50%><a href=\"bbstcon?board=%s&file=%s\">%s%46.46s </a>", board, r.en,
                                       strncmp(r.cn, "Re: ", 4) ? "○ " : "",r.cn);
                                if(r.sign==0)
                                        printf("<a href=\"rmd?mode=1&&board=%s&&file=%s\"><取消></a></td>",board,r.en);

                                if(i%2==1)
                                        printf("</tr>");
                                i++;
                        }  //while
                        printf("</table>");


                }//if fp
        } //elsee
}

