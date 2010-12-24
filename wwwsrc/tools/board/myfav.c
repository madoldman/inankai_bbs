#include "BBSLIB.inc"
#include "boardrc.inc"

int main()
{
        struct shortfile data[GOOD_BRC_NUM+1], *x;
        int i, total=0, sec1;
        char *cgi="bbsdoc", *ptr, *my_sec;
        init_all();
        if(!loginok)http_fatal("您无法使用该页。");
        printf("<style type=text/css>A {color: #0000f0}</style>");
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        if(currentuser.userdefine & DEF_THESIS)
                cgi="bbstdoc";
        int mybrdnum=0;
        char mybrd[GOOD_BRC_NUM][80];
        mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
        for(i=0; i<mybrdnum; i++) 
	{
                if(!(x=getbcache(mybrd[i])))continue;
                if(x->filename[0]<=32 || x->filename[0]>'z')
                        continue;
                if(!has_read_perm(&currentuser, x->filename))
                        continue;
		if(x->flag & ZONE_FLAG)continue;
                data[total++]=*x;
        }
        //qsort(data, total, sizeof(struct shortfile), cmpboard);
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        printf("<p align='center'><a href=bbsmybrd><img src=/myfav.gif title='设置收藏夹' border=0 width=400/></a></p><hr color=green>");
        printf("<table width=610 align=center>\n");
        printf("<tr><td>序号<td>未<td>讨论区名称<td>更新时间<td>类别<td><td>中文描述<td>板主<td>文章数\n");
        for(i=0; i<total; i++) {
                char buf[100];
			sprintf(buf, "boards/%s/.DIR", data[i].filename);
                	printf("<tr><td>%d<td>%s",i+1, board_read(data[i].filename) ? "◇" : "◆");
                	printf("<td><a href=%s?board=%s>%s</a>", cgi, data[i].filename, data[i].filename);
                	printf("<td>%12.12s", 4+Ctime(file_time(buf)));
                	printf("<td>%6.6s", data[i].title+1);
                	if(data[i].flag&VOTE_FLAG) 
			{
                        	printf("<td><a href=bbsvote?board=%s><font color=red>V</font></a></td>", data[i].filename);
                	} 
			else
                        	printf("<td></td>");

                	printf("<td><a href=%s?board=%s>%s</a>", cgi, data[i].filename, data[i].title+7);
                	ptr=strtok(data[i].BM, " ,;");
                	if(ptr==0)
                        	ptr="无";
                	if(!strcmp(ptr, "SYSOPs")||!strcmp(ptr, "无"))
                        	printf("<td>%s</td>", ptr);
                	else
                        	printf("<td><a href=bbsqry?userid=%s>%s</a></td>", ptr, ptr);
                	printf("<td>%d\n", filenum(data[i].filename));
        }
        printf("</table><hr>\n");
        check_msg();//bluetent 2002.10.31
        http_quit();
}

