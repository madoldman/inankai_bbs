/* $Id: bbsdoc.c,v 1.4 2010-07-01 10:13:47 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"
#include "showboard.h"
#include "VBLIB.inc"

int my_t_lines;
char buflink[1024], bufnav[1024];
struct shortfile *x1;
char *timestring(char *s);
char flgjunk=0;//huangxu@060406:回收站文章

int main()
{
        FILE *fp;
		struct vb_manager * vb;
        char  dir[80], *ptr, buf[256], vboard[80];
        static char  board[STRLEN];
        struct fileheader x[40];
        int i, start, total, chart=0, maxvisit=0;
        init_all();
        if(loginok) modify_mode(u_info,READING);	//bluetent
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
        if (strlen(getparm("junk"))&&(has_BM_perm(&currentuser, board)||HAS_PERM(PERM_FORCEPAGE)))
        	flgjunk=1;//huangxu@060406:junk模式
         board_usage(board);
		 vb = vb_parseconfig(board);
			 strcpy(vboard, vb ? vb->vboard : board);
        char path[STRLEN];
        char mode[8];
        int iscompact=0,num=0;
        struct compact p;
        strncpy(mode,getparm("mode"),8);
        if(!strcmp(mode,"c")) {//过刊模式
                iscompact=1;
                num=atoi(getparm("num"));
                if(num==0)
                        num=1;
                sprintf(path,"compact/%s/compact",board);
                get_record(&p,sizeof(p),num-1,path);
               	sprintf(dir,"compact/%s/%d/.DIR",board,p.dir);
        } else
                if (flgjunk)//huangxu@060406:回收站
                	sprintf(dir,"boards/%s/.junk",board);
                else
                	sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0) {
                showinfo("文件打开失败!");
                return -1;
        }
        total=file_size(dir)/sizeof(struct fileheader);

        start=atoi(getparm("start"));
        my_t_lines=atoi(getparm("my_t_lines"));
        if(my_t_lines<10 || my_t_lines>40)
                my_t_lines=30;
        if(start==0 || start>total-my_t_lines)
                start=total-my_t_lines+1;
        start-=1;
        if(start<0)
                start=0;

        fseek(fp,sizeof(struct fileheader)*start,SEEK_SET);
        int n=fread(x,sizeof(struct fileheader),my_t_lines,fp);
        fclose(fp);
        brc_init(currentuser.userid, board);

        strcpy(u_info->board, board);
        printf("<script language=JavaScript>\
               function MM_jumpMenu(selObj){\ 
               eval(\"window.location='\"+selObj.options[selObj.selectedIndex].value+\"'\");\
               }\
               </script>\
               <script language=JavaScript src=/scrolldown.js></script>\
               <script language=JavaScript src=/js/common.js></script>");
        printf("<meta http-equiv=pragma content=no-cache>");

printf("<link rel=\"alternate\" type=\"application/rss+xml\" title=\"RSS\" href=\"http://%s/cgi-bin/rss?board=%s\" />",BBSHOST,board);

printf("<title>%s-%s 文章列表</title>",BBSNAME,board);
        printf("</head><body background=\"%s\"   bgproperties=fixed leftmargin=10 topmargin=5><a name=top></a>",mytheme.bgpath);
		if (!vb)
        printf("\
               <a href=bbs0an?path=%s>精华区</a>\
               <a href=bbsgdoc?board=%s>文摘区</a>\
               <a href=bbsatt?board=%s>文件展区</a>\
               ", anno_path_of(board),board,board);

        sprintf(buf,"compact/%s/compact",board);
        if(!vb && get_num_records(buf,sizeof(struct compact)))
                printf("<a href=bbscompact?board=%s>过刊区</a>",board);
        if(!vb && has_BM_perm(&currentuser, board))
                printf("<a href=bbsmdoc?board=%s&&start=%d>管理模式</a>",board,start);
        if(x1->flag&VOTE_FLAG)
                printf("<a href=bbsvote?board=%s><font color=red>参加投票</font></a>",board);

        printf("<a href=bbsbrdchg?board=%s> 收藏本板</a>",board);
        if (flgjunk)//huangxu@060406
              printf(" <a href=bbsdoc?board=%s>正常模式</a>",board);
        else
        if (!vb && (has_BM_perm(&currentuser, board)||HAS_PERM(PERM_FORCEPAGE)))
                printf(" <a href=bbsdoc?board=%s&&junk=yes>回收模式</a>",board);
        	


 printf("&nbsp;&nbsp;&nbsp;<A style=\"BORDER-RIGHT: 1px solid #663300; PADDING-RIGHT: 3px; BORDER-TOP: 1px solid #ffcc99; PADDING-LEFT: 3px; FONT-WEIGHT: bold; FONT-SIZE: 11px; BACKGROUND: #ff6600; PADDING-BOTTOM: 0px; TEXT-TRANSFORM: uppercase; BORDER-LEFT: 1px solid #ff9966; COLOR: #ffffff; PADDING-TOP: 0px; BORDER-BOTTOM: 1px solid #000000; TEXT-DECORATION: none; font-family: Arial,Verdana,Sans-Serif;\" href=\"/cgi-bin/rss?board=%s\">xml</A>",board);

        printf("<br><font size=5 face=arial> <center><b>%s(%s)</b></font></center>",board, x1->title+11);
        if(flgjunk)//huangxu@060406
        	printf("<center><font size=3>回收模式</font></center>");


        chart=atoi(getparm("chart"));
        if(chart==0)
                chart=chartpos(board);
        printf("<table width=84% border=0 cellspacing=0 cellpadding=0 align=center>\
               <tr><td >\
               板主:<font face='Verdana, Arial, Helvetica, sans-serif'><b><font color=orange>%s</b></font>\
               </font><br>\
               在线:%d人 <a href=bbstopb10>排名:%d</a> 文章数:%d %s %s</td>",(!strcmp(userid_str(x1->BM), ""))?"无":userid_str(x1->BM),*((int*)(x1->filename+72)),chart,total, (x1->flag & OUT_FLAG)? "转信" : "", junkboard(board)? "本板不计文章数":"");
        printf("<td >");
        setbfile(buf,board,"pattern");
        int forcepattern=0;
        //if(file_has_word("etc/patternpost", board))
        //huangxu@070415
        forcepattern=boardForcePat(x1);
        if(get_num_records(buf,sizeof(struct pattern))){
               if(!forcepattern)
                  sprintf(bufnav,"<a href=bbspst?board=%s&vfrom=%s>发表文章 </a>",vb ? *vb->boardlist : board, board);
               sprintf(bufnav,"%s<a href=bbspattern?board=%s&vfrom=%s><font color=orange>%s</font></a>",bufnav,vboard,board,   forcepattern?"发表文章":"模板发文");   
        } 
        else             
               sprintf(bufnav,"<a href=bbspst?board=%s&vfrom=%s> 发表文章 </a>",vb ? *vb->boardlist : board,board);
        sprintf(bufnav,"%s<a href=bbstdoc?board=%s>同主题阅读</a>\
                <a href=bbsbfind?board=%s>板内查询</a>\
                <a href=bbsclear?board=%s&start=%d>清除未读</a>\
                <a href=javascript:location.reload()>刷新</a>\
                <a href=javascript:self.print()>打印</a>",bufnav,board,board,board,start);
        printf("%s</td>\
               </tr>\
               </table>",bufnav);



        showlink(board, total,  start,  my_t_lines,  chart);
        printf(buflink);

        printf("[<a href=#bottom>↓底部</a>]");
        printf("<table align=center width=90%  border=0 cellpadding=1 cellspacing=1><tr bgColor=%s><td align=center width=30><font color=%s>序号</font></td><td align=center width=30><font color=%s>状态</font></td><td align=center width=80><font color=%s>作者</font></td><td align=center><font color=%s>\
                       日期</font></td> <td align=center width=*><font color=%s>标题</font></td></tr>",mytheme.bar,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf);


        if(total<=0)
                http_fatal("本讨论区目前没有文章");

        showtop(board);



        /*增强阅读提示*/
        int sign[40],re=0,j;
        for(i=0;i<n;i++) {
                if(strncmp(x[i].title, "Re: ", 4))
                        sign[i]=1;
                else {
                        re++;
                        sign[i]=1;
                        for(j=1;j<=i;j++) {
                                if(i-j<0)
                                        break;
                                if(isre(x[i-j].title,x[i].title)) {
                                        sign[i]=0;
                                        break;
                                }
                        }
                }//else
        }

        if(re<6)
                for(i=0;i<n;i++) {
                        sign[i]=0;
                        if(i%8==0)
                                sign[i]=2;
                }
        else
                for(i=0;i<n;i++) {
                        if(sign[i]==0) {
                                sign[i]=2;
                                i+=8;
                        }
                }


        char *s[]={"","red","black"};
        char *s1[]={"&nbsp;", "T","+"};
        for(i=0; i<n; i++) {
                char filename[80];
                char *ptr, flag[80];
                ptr=flag_str2(x[i].accessed[0], brc_has_read(x[i].filename));
                strcpy(flag, "");
                if(ptr[0]=='N') {
                        sprintf(flag, "<font color=#909090 face=Arial>%c</font>", ptr[0]);
                }
                if(ptr[0]=='I'||ptr[0]=='i') {
                        sprintf(flag, "<font color=red face=Arial>%c</font>", ptr[0]);
                }

                if(ptr[0]=='M'||ptr[0]=='m') {
                        sprintf(flag, "<font color=red face=Arial>%c</font>", ptr[0]);
                }
                if(ptr[0]=='G'||ptr[0]=='g') {
                        sprintf(flag, "<font color=#009933 face=Arial>%c</font>", ptr[0]);
                }
                if(ptr[0]=='B'||ptr[0]=='b') {
                        sprintf(flag, "<font color=#9966FF face=Arial>%c</font>", ptr[0]);
                }
                if(ptr[1]=='X'||ptr[1]=='x') {
                        if(ptr[0]=='N')
                                sprintf(flag, "<font color=blue face=Arial>%c</font>", ptr[1]);
                        else
                                sprintf(flag, "%s<font color=blue face=Arial>%c</font>", flag, ptr[1]);
                }
                if(x[i].filename[STRLEN-3]=='U') {
                        sprintf(flag, "%s<font color=red face=Arial>@</font>",flag);
                }
                sprintf(filename, "boards/%s/%s", board, x[i].filename);
                hsprintf(buf, "%s", strncmp(x[i].title, "Re: ", 4)?x[i].title:x[i].title+4);
                printf("<tr height=1><td align=right width=30 bgColor=%s>%d</td>", mytheme.con,start+i+1);
                printf("<td align=center width=30>%s</td>", flag);
                printf("<td align=left width=80>%s</td>", userid_str(x[i].owner));
                printf("<td align=center width=80>%s</td>",timestring(x[i].filename+2));

                //			printf("  ", (*(int*)(x.title+72)/maxvisit)<=1?(*(int*)(x.title+72)*100/maxvisit):0, (*(int*)(x.title+72)/maxvisit)<=1?(100-*(int*)(x.title+72)*100/maxvisit):0);

                printf("<td align=left><a href=newcon?file=%s&mode=%d&board=%s&vfrom=%s&num=%d%s><font color=%s> %s%s</font></a>",x[i].filename,sign[i],vb ? vb_getboardfromfh(x + i) : board, board,start+i,flgjunk?"&junk=yes":"",s[sign[i]],s1[sign[i]],strncmp(x[i].title, "Re: ", 4) ? "○ ":"Re: ");

                if(!iscompact )
                        printf("<a href=newcon?board=%s&vfrom=%s&file=%s&_num=%d&compact=%d%s>%46.46s </a></td>", vb ? vb_getboardfromfh(x + i) : board,board, x[i].filename, start+i,num,flgjunk?"&junk=yes":"",buf);
                else
                        printf("<a href=newcon?board=%s&vfrom=%s&file=%s&_num=%d&compact=%d%s>%46.46s</a></td>", vb ? vb_getboardfromfh(x + i) : board, board, x[i].filename, start+i,num,flgjunk?"&junk=yes":"",buf);

                //printf("%s", eff_size(filename));
                //			printf("%d", (*(int*)(x.title+72)>=100&&*(int*)(x.title+72)<=10000)?"red":"#000000", ((*(int*)(x.title+72))>0&&*(int*)(x.title+72)<=10000)?(*(int*)(x.title+72)):0);

        }

        printf("</table>");
        printf("<form action=bbsdoc?board=%s  method=post><table width=90%><tr><td>",board);
        printf(buflink);
        printf("</td><td align=right>[<a href=#top>↑顶部</a>]</td></tr></table>\
               <a name=bottom></a>");
        printf("<table align=center width=90%><tr><td>%s<a href=/digest/%s.html.tar.gz> 下载精华区  </a></td<td>",bufnav,board);
        printf("第<input style='height:12px; border:1px solid #404040' type=text name=start maxlength=5 size=5 onmousemove='this.focus()' onkeypress='if(event.keyCode==10) this.value-=1'>篇\
               </td></tr></table></form>\
               ");

        showthesis(board);
        printf("<br><br>");
        http_quit();
}


/*  输出页面链接到buflink */
//huangxu@060406:回收模式
int showlink(char *board, int total, int start, int my_t_lines, int chart)
{
        int i, curr, first;
        char chartbuf[16];
        if(chart>0)
                sprintf(chartbuf, "&chart=%d", chart);
        else
                strcpy(chartbuf, "");
        sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=1%s%s>首页</a> ", buflink, board, chartbuf,flgjunk?"&junk=yes":"");
        if(start>9*my_t_lines)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>前十页</a> ", buflink, board, start-my_t_lines*10, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>前十页</font> ", buflink);
        if(start>4*my_t_lines)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>前五页</a> ", buflink, board, start-my_t_lines*5, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>前五页</font> ", buflink);
        if(start>1*my_t_lines)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>前二页</a> ", buflink, board, start-my_t_lines*2, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>前二页</font> ", buflink);
        if(start>0)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>前一页</a> ", buflink, board, start-my_t_lines, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>前一页</font> ", buflink);

        sprintf(buflink, "%s(<font color=red>%d</font> - <font color=red>%d</font>) ", buflink, start+1, start+my_t_lines);
        if(start<total-my_t_lines)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>后一页</a> ", buflink, board, start+my_t_lines, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>后一页</font> ", buflink);
        if(start<total-2*my_t_lines)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>后二页</a> ", buflink, board, start+2*my_t_lines, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>后二页</font> ", buflink);
        if(start<total-5*my_t_lines)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>后五页</a> ", buflink, board, start+5*my_t_lines, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>后五页</font> ", buflink);
        if(start<total-10*my_t_lines)
                sprintf(buflink, "%s<a href=bbsdoc?board=%s&start=%d%s%s>后十页</a> ", buflink, board, start+my_t_lines*10, chartbuf,flgjunk?"&junk=yes":"");
        else
                sprintf(buflink, "%s<font color=#909090>后十页</font> ", buflink);
        sprintf(buflink, "%s<a href=bbsdoc?board=%s%s%s>末页</a> ", buflink, board, chartbuf,flgjunk?"&junk=yes":"");

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
                                printf("<tr><td><img src=/image/hot.gif></td>");
                                printf("<td align=center width=30>");
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
                                printf("<td align=center width=80>%s",timestring(f.filename+2));
                                printf("</td><td align=left width=*><a href=\"newcon?board=%s&file=%s&mode=3\">%s%46.46s </a></td>", board, f.filename,
                                       strncmp(f.title, "Re: ", 4) ? "○ " : "",f.title);
                                //printf("%s", eff_size(f.filename));

                        }  //while

                }//if fp
        } //elsee
}

showthesis(char *board)
{
        char path[STRLEN], buf[STRLEN];
        struct rmd r;
        sprintf(path, "boards/%s/.thesis", board);
        if(!file_exist(path))
                return;
        FILE *fp=fopen(path,"r");
        if(fp!=NULL) {
                printf ("<table align=center width=100%>");

                int i=0;
                while(1) {
                        if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                                break;

                        if (i == 0)
                                printf ("<tr><td><font color=blue><i><b>推荐文章</b></i></font></td></tr>");

                        printf("<tr><td>");
                        if(r.sign==1)
                                printf("◆");
                        else
                                printf("◇");
                        printf("<a href=\"newcon?board=%s&file=%s&mode=3\">%s%46.46s </a>  作者:%s %12.12s", board,r.en,strncmp(r.cn, "Re: ", 4) ? "○ " : "",r.cn,userid_str(r.owner),Ctime(r.psttime)+4);
                        printf("</td></tr>");
                        i++;
                }  //while
                printf("</table>");
        }//if fp
}

char *timestring(char *s)
{
        time_t t;
        struct tm *time;
        static  char buf[16];
        t=atoi(s);
        time=localtime(&t);
        sprintf(buf,"%02d/%02d %02d:%02d",time->tm_mon+1,time->tm_mday,time->tm_hour,time->tm_min);
        return buf;
}
isre(char *s1,char *s2)
{
        //	printf("%s,%s",s1,s2);
        //exit( 0);
        if(!strncmp(s1, s2, 40)||(!strncmp(s1, s2+4, 40) && !strncmp(s2, "Re: ", 4)))
                return 1;
        else
                return 0;

}
