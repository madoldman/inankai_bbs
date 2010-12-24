/*$Id: bbstdoc.c,v 1.5 2010-07-01 10:13:47 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "showboard.h"
#include "VBLIB.inc"


char *stat1();
char *eff_size(), buflink[512], bufnav[512], filename[80];
int my_t_lines;
struct shortfile *x1;

int main()
{
        FILE *fp;
		struct vb_manager * vb;
        char board[80], dir[80], filename[256];
        char *ptr, *font1="", *font2="";
        struct fileheader x;
        struct fileheader *data;
        int i, start, total2=0, total, sum=0, chart=0, showhead=0;
        init_all();
        modify_mode(u_info,READING);	//bluetent
        my_t_lines=atoi(getparm("my_t_lines"));
        strsncpy(board, getparm("board"), 32);
        x1=getbcache(board);
        if(x1==0)
                http_fatal("错误的讨论区");
        strcpy(board, x1->filename);

        board_ban_ip(board);
        if (!canenterboard (currentuser.userid, board))
                http_fatal ("您已经被取消进入本板的权限");
        if(!has_read_perm(&currentuser, board) /*&& !HAS_PERM(PERM_SYSOP)*/)
                http_fatal("错误的讨论区");
        board_usage(board);
		vb = vb_parseconfig(board);
        sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("错误的讨论区目录");
        total=file_size(dir)/sizeof(struct fileheader);
        data=calloc(sizeof(struct fileheader), total);
        if(data==0)
                http_fatal("内存溢出");
        total=fread(data, sizeof(struct fileheader), total, fp);
        fclose(fp);
        for(i=0; i<total; i++)
                if(strncmp(data[i].title, "Re:",3))
                        total2++;
        start=atoi(getparm("start"));

        if(my_t_lines<10 || my_t_lines>40)
                my_t_lines=20;
        if(strlen(getparm("start"))==0 || start>total2-my_t_lines)
                start=total2-my_t_lines;
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
        printf("<body background=%s  bgproperties=fixed leftmargin=0 topmargin=0><a name=top></a><table align=center width=600 border=0 cellpadding=1 cellspacing=1 bgcolor=%s>\
               <tr align=left valign=center height=20>\
               <td width=*>同主题模式: %s (%s) 板主: %s</td>", mytheme.bgpath,mytheme.bar ,board, x1->title+11, (!strcmp(userid_str(x1->BM), ""))?"无":userid_str(x1->BM));

        printf("<td align=right><a href=\"javascript:menu('board_info','')\">关于本板</a></td>");


        printf("<td width=160>模式选择：\
               <SELECT NAME=Navagation onChange=\"MM_jumpMenu(this)\">\
               <option value=bbsdoc?board=%s>讨论区</option>\
               <option value=bbstdoc?board=%s selected>同主题模式</option>\
			   ", board, board);
		if (!vb)
		printf("<option value=bbsgdoc?board=%s>文摘区</option>\
               <option value=bbsmdoc?board=%s>管理模式</option>\
               <option value=bbsdoc?board=%s&junk=yes>回收模式</option>\
               <option value=bbsatt?board=%s>文件展区</option>\
			   ", board, board, board, board);
		printf("</SELECT>\
               </td>\
               </tr>\
               </table>\
               ", board, board, board, board,board,board);
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
                printf("<td align=left width=*>\
                       <font color=#009966>⊙</font> 讨论区文章数：<font color=red>%d</font><br>\
                       <font color=#009966>⊙</font> 讨论区排名：<font color=red>%d</font>(<a href=bbstopb10>全部</a>) 板内在线：<font color=red>%d</font>(<a href=bbsinboard>排名</a>)<br>\
                       <font color=#009966>⊙</font> 本板为%s转信板面，发表文章%s算文章数<br>\
                       </td>\
                       <td align=left width=150>\
                       <font color=#009966>⊙</font> <a href=\"javascript:void open('bbsadvice', '','width=300,height=200')\">对系统功能的意见与建议</a><br>\
                       <font color=#009966>⊙</font> <a href=bbshelp?file=tdoc>本页帮助信息</a><br>\
                       </td>\
                       </tr>\
                       </table>\
                       ", total, chart, *((int*)(x1->filename+72)), (x1->flag & OUT_FLAG)? "" : "非", junkboard(board)? "不" : "");


                printf("</div>");
        }
        printf("<table align=center width=600 border=0 cellpadding=1 cellspacing=1>\
               <tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        shownav(board, start, vb);
        printf(bufnav);
        printf("<font color=#009966>·</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr><td>");
        printf("<table width=100%%><tr><td align=center>");
        showlink(board, total2, start, my_t_lines, chart);
        printf(buflink);
        printf("<td align=right>[<a href=#bottom>↓切换到底部</a>]</td></tr></table></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table>");
        printf("<table align=center width=600 border=0 cellpadding=1 cellspacing=1><tr bgColor=%s><td align=center width=30>序号</td><td align=center width=80>作者</td><td align=center width=80>日期</td><td align=center width=*>标题</td></tr>",mytheme.bar);


        showtop(board);
		char title_in_url[STRLEN * 4];

        for(i=0; i<total; i++) {
                if(!strncmp(data[i].title, "Re:", 3))
                        continue;
                sum++;
                if(sum-1<start)
                        continue;
                if(sum-1>=start+my_t_lines)
                        break;
                sprintf(filename, "boards/%s/%s", board, data[i].filename);
                printf("<tr><td align=right width=30 bgColor=#%s>%d</td>",mytheme.con, sum);
                printf("<td align=left width=80>%s</td>", userid_str(data[i].owner));
                printf("<td align=center width=80>%12.12s</td>", Ctime(atoi(data[i].filename+2))+4);
				kill_quotation_mark(title_in_url,data[i].title);
                printf("<td align=left width=*><a href=newcon?board=%s&title=%s&vfrom=%s>", vb ? vb_getboardfromfh(data + i) : board, title_in_url, board);
                hprintf("%46.46s ",(data[i].title));
                printf("</a></td>");
        }
        printf("</table><table align=center width=600 border=0 cellpadding=1 cellspacing=1><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr><td><table width=100%%><tr><td align=center>");
        printf(buflink);
        printf("</td><td align=right>[<a href=#top>↑切换到顶部</a>]</td></tr></table></td></tr>");
        printf("<tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr><tr valign=top><td>");
        printf(bufnav);
        printf("<font color=#009966>·</font></td></tr><tr><td><table width=100%% bgcolor=#65c8aa border=0 cellpadding=0 cellspacing=0 height=1><tr><td></table></td></tr></table></center><a name=bottom></a>");
        printf("\
               <center>\
               <form action=bbstdoc?board=%s  method=post>\
               我要跳转到第 <input    style='height:16px; border:1px solid #404040' type=text name=start maxlength=4 size=4 onmousemove='this.focus()'>篇文章!</form>\
               <br>\
               </center>\
               ",board);

        check_msg();//bluetent 2002.10.31
        http_quit();
}

char *stat1(struct fileheader *data, int from, int total)
{
        static char buf[256];
        char *ptr=data[from].title;
        int i, re=0, click=*(int*)(data[from].title+73);
        for(i=from; i<total; i++)
        {
                if(!strncmp(ptr, data[i].title+4, 40)) {
                        re++;
                        click+=*(int*)(data[i].title+73);
                }
        }
        sprintf(buf, "<font color=%s>%d</font>/<font color=%s>%d</font>",
                re>9 ? "red" : "black", re, click>499 ? "red" : "black", click);
        return buf;
}

/* 输出导航链接到bufnav */
int shownav(char *board, int start, struct vb_manager * vb)
{
      //if(file_has_word("etc/patternpost", board))
      if(boardForcePat(x1))
           sprintf(bufnav,"%s<a href=bbspattern?board=%s&vfrom=%s><font color=orange>发表文章</font></a>",bufnav, vb ? *vb->boardlist : board, board);
      else
           sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbspst?board=%s&mode=2&vfrom=%s>发表文章</a>", bufnav, vb ? *vb->boardlist : board, board);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsbfind?board=%s>搜索文章</a>", bufnav, board);
        //	sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsclear?board=%s&start=%d&type=1>清除未读</a>", bufnav, board, start);
		if (!vb)
		{
        	sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbs0an?path=%s>精华区</a>", bufnav, anno_path_of(board));
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsgdoc?board=%s>文摘区</a>",bufnav, board);
		}
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsdoc?board=%s>讨论区模式</a>",bufnav, board);
        // sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsfdoc?board=%s>本板上载区</a>", bufnav, board);
        if(x1->flag&VOTE_FLAG)
                sprintf(bufnav, "%s<font color=#009966>·</font><a href=bbsvote?board=%s><font color=red>参加投票</font></a>", bufnav, board);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=javascript:self.print()>打印本页</a>", bufnav);
        sprintf(bufnav, "%s<font color=#009966>·</font><a href=javascript:location.reload()>刷新本页</a>", bufnav);
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
        sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=0%s>首页</a> ", buflink, board, chartbuf);
        if(start>9*my_t_lines)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>前十页</a> ", buflink, board, start-my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前十页</font> ", buflink);
        if(start>4*my_t_lines)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>前五页</a> ", buflink, board, start-my_t_lines*5, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前五页</font> ", buflink);
        if(start>1*my_t_lines)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>前二页</a> ", buflink, board, start-my_t_lines*2, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前二页</font> ", buflink);
        if(start>0)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>前一页</a> ", buflink, board, start-my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>前一页</font> ", buflink);

        sprintf(buflink, "%s(<font color=red>%d</font> - <font color=red>%d</font>) ", buflink, start+1, start+my_t_lines);
        if(start<total-my_t_lines)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>后一页</a> ", buflink, board, start+my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后一页</font> ", buflink);
        if(start<total-2*my_t_lines)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>后二页</a> ", buflink, board, start+2*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后二页</font> ", buflink);
        if(start<total-5*my_t_lines)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>后五页</a> ", buflink, board, start+5*my_t_lines, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后五页</font> ", buflink);
        if(start<total-10*my_t_lines)
                sprintf(buflink, "%s<a href=bbstdoc?board=%s&start=%d%s>后十页</a> ", buflink, board, start+my_t_lines*10, chartbuf);
        else
                sprintf(buflink, "%s<font color=#909090>后十页</font> ", buflink);
        sprintf(buflink, "%s<a href=bbstdoc?board=%s%s>末页</a> ", buflink, board, chartbuf);

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
                                //printf("<td align=center width=30>%s</td>", "置顶");
                                printf("<td align=left width=80>%s</td>", userid_str(f.owner));
                                printf("<td align=center width=80>%12.12s", Ctime(atoi(f.filename+2))+4);
                                printf("</td><td align=left width=*><a href=newcon?board=%s&file=%s>%s%46.46s </a></td>", board, f.filename,
                                       strncmp(f.title, "Re: ", 4) ? "○ " : "",f.title);
                                //printf("<td align=right width=30>%s</td></tr>", eff_size(f.filename));

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

                                printf("<td align=left width=50%><a href=\"newcon?board=%s&file=%s\">%s%46.46s </a>", board, r.en,
                                       strncmp(r.cn, "Re: ", 4) ? "○ " : "",r.cn);

                                if(i%2==1)
                                        printf("</tr>");
                                i++;
                        }  //while


                        printf("</table>");


                }//if fp
        } //elsee
}
