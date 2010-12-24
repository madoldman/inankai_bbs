/*$Id: bbsboa.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"


int main()
{
        struct shortfile data[MAXBOARD], *x;
        int i, total=0, sec1;
        char *cgi="bbsdoc", *ptr, *my_sec;
        init_all();
        printf("<style type=text/css>A {color: #0000f0}</style>");
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        sec1=atoi(getsenv("QUERY_STRING"));
        if(sec1<0 || sec1>=SECNUM)
                http_fatal("错误的参数");
        //	if(atoi(getparm("my_def_mode"))!=0) cgi="bbstdoc";
        if(currentuser.userdefine & DEF_THESIS)
                cgi="bbstdoc";
	for(i=0; i<MAXBOARD; i++) 
	{
                x=&(shm_bcache->bcache[i]);
                if(x->filename[0]<=32 || x->filename[0]>'z')
                        continue;
                if(!has_read_perm(&currentuser, x->filename))
                        continue;
	///	printf("name:%s\n",x->title);
                if(!strchr(seccode[sec1], x->title[0]))
                        continue;
		if(x->flag & CHILDBOARD_FLAG)
			continue;
		if((x->flag & ZONE_FLAG) && count_zone(BOARDS,x->filename)==0 && !HAS_PERM(PERM_SYSOP))
			continue;
		if(strcmp(x->owner,"")!=0)
			continue;
                memcpy(&data[total], x, sizeof(struct shortfile));
                total++;
        }
        qsort(data, total, sizeof(struct shortfile), cmpboard);
	printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
	printf("<center>\n");
        //	printf("%s -- 分类讨论区 [%s区]<hr color=green>", BBSNAME, secname[sec1]);
        printf("<img src=/fenlei_b.gif><br>%s<hr color=green>", 
		secname[sec1],total);
        printf("<table width=610>\n");
        printf("<tr><td>序号<td>未<td>讨论区名称<td>更新时间<td>类别<td><td>中文描述<td>板主<td>文章数\n");
        for(i=0; i<total; i++) {
                char buf[100];
		if(!(data[i].flag & ZONE_FLAG))
                {
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
		else
		{
                	printf("<tr><td>%d<td>%s",i+1, "＋");
                	printf("<td><a href=bbszone?zonename=%s>%s</a>", data[i].filename, data[i].filename);
                	printf("<td>%12.12s", "");
                	printf("<td>%6.6s", data[i].title+1);
                	printf("<td></td>");
                	printf("<td><a href=bbszone?zonename=%s>%s</a>", data[i].filename, data[i].title+7);
			printf("<td></td>");
                	printf("<td>%d\n", count_zone(BOARDS,data[i].filename));
		}
        }
        printf("</table><hr></center>\n");
        //check_msg();//bluetent 2002.10.31
        http_quit();
}

