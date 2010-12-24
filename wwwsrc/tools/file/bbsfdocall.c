/*$Id: bbsfdocall.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char *ptr, board[256];
        struct dir x[10000], xx;
        int i, start, total=0;
        int totalsize=0;//附件占服务器总容量
        init_all();
        //	strsncpy(board, getparm("board"), 30);
        //	if(!has_read_perm(&currentuser, board)) http_fatal("错误的讨论区");
        fp=fopen(UPLOAD_PATH"/.DIR", "r");
        if(fp==0)
                http_fatal("没有找到文件");
        while(total<10000) {
                if(fread(&xx, sizeof(xx), 1, fp)<=0)
                        break;
                if(!xx.active)
                        continue;
                //		if(strcasecmp(xx.board, board)) continue;
                x[total]=xx;
                totalsize+=xx.size;
                total++;
        }
        ptr=getparm("start");
        if(ptr[0]==0) {
                start=total-19;
        } else {
                start=atoi(ptr);
        }
        ptr=getparm("startno");
        if(ptr[0]!=0) {
                start=atoi(ptr)-1;
        }
        if(start>total-19)
                start=total-19;
        if(start<0)
                start=0;
        printf("<center>全站附件列表 [附件数：%d，已占用硬盘空间：%d字节，平均每个附件占用：%d字节]<hr color=green>\n",  total, totalsize, totalsize/total);
        printf("<table width=610 border=1><tr><td>序号<td>文件名<td>上载者<td>板面<td width=50>时间<td>大小<td>下载次数<td>说明<td>备注\n");
        for(i=start; i<start+20 && i<total; i++) {
                printf("<tr><td>%d", i+1);
                //		printf("<td><a href=/cgi-bin/bbs/showfile?name=%s target=_blank>%s</a>", nohtml(x[i].showname), nohtml(x[i].showname));
                if(x[i].filename) {
                        printf("<td><a href=/cgi-bin/bbs/showfile?name=%s target=_blank>%s</a>", nohtml(x[i].showname), nohtml(x[i].showname));
                } else {
                        printf("<td><a href=/file/%s/%s>%s</a>", x[i].board, nohtml(x[i].showname), nohtml(x[i].showname));
                }

                printf("<td>%s", userid_str(x[i].userid));
                printf("<td>%s", x[i].board);
                printf("<td width=50>%6.6s", Ctime(x[i].date)+4);
                printf("<td>%d", x[i].size);
                //		printf("<td> "); //%d", x[i].click);
                printf("<td>%d", x[i].click);
                printf("<td><a href=bbsfexp?name=%s>more </a>\n", nohtml(x[i].showname));
                printf("<td width=150>%s ", strlen(x[i].exp)<=0?"&nbsp;":nohtml(x[i].exp));
                //		printf("<td><a onclick='return confirm(\"确定删除吗?\")' href=bbsudel?file=%s>删</a>\n", x[i].showname);
        }
        printf("</table><br>\n");
        //	printf("[<a href=bbsfdoc?board=%s>刷新</a>] [<a href=bbsdoc?board=%s>本讨论区</a>] [<a href=bbsupload?board=%s>上载文件</a>]", board, board, board);
        printf("[<a href=bbsfdocall?start=0>首页</a>]");
        if(start>0)
                printf("[<a href=bbsfdocall?start=%d>上一页</a>] ", start-19);
        if(start<total-20)
                printf("[<a href=bbsfdocall?start=%d>下一页</a>] ", start+19);
        printf("[<a href=bbsfdocall>尾页</a>]");
        printf("[<a href=bbsfind2?newmode=1>全站附件查询</a>]");
        printf("<form><input type=text name=startno><input type=submit value=转到指定序号的附件></form>");
}
