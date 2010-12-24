
#include "BBSLIB.inc"
#include "filemgr.inc"

struct shortfile *x;
char board[80];
char mode_bm=0;
struct fmg_attdir ad;
int start;
int pagesize=20;

int attact()
{
	int i;
	char buf[256];
	char buf2[80];
	int act;
	int cnt=0;
	int r;
	struct fmg_FileRec frec;
	act=atoi(getparm("act"));
	ad=fmg_openattdir(board,0);
	for(i=0;i<pagesize;i++)
	{
		sprintf(buf,"name%d",i);
		strsncpy(buf,getparm(buf),220);
		if(!*buf)continue;
		r=fmg_seekrec(buf,&ad);
		if(r<0)continue;
		fseek(ad.fp,r*sizeof(struct fmg_FileRec)+sizeof(struct fmg_FileHead),0);
		fread(&frec,sizeof(frec),1,ad.fp);
		sprintf(buf2,"info%d",i);
		strsncpy(frec.info,getparm(buf2),255);
		fseek(ad.fp,-sizeof(struct fmg_FileRec),1);
		fwrite(&frec,sizeof(frec),1,ad.fp);
		sprintf(buf2,"chk%d",i);
		if(!*(getparm(buf2)))continue;
		r=fmg_changelife(buf,act,&ad);
		if(r>0)cnt++;
		else
		{
			printf("<p align=\"center\"><font color=\"red\">处理文件 %s 时出错，此前已成功处理%d个文件。</font></p>",buf,cnt);
			switch(r)
			{
				case -1:
					strcpy(buf,"记录未找到");
					break;
				case -2:
					strcpy(buf,"文件打开错误");
					break;
				case -3:
					strcpy(buf,"空间不足");
					break;
				case -4:
					strcpy(buf,"操作错误");
					break;
				default:
					strcpy(buf,"未知原因");
					break;
			}
			printf("<p align=\"center\"><font color=\"yellow\">出错原因：%s。</font></p>",buf);
			cnt=-1;
			break;
		}
	}
	fmg_closeattdir(&ad);
	if(cnt>0)
		printf("<p align=\"center\"><font color=\"green\">已成功对%d个文件进行了操作。</font></p>",cnt);
	else if(cnt==0)
		printf("<p align=\"center\"><font color=\"yellow\">未选取任何文件。</font></p>");
	return 0;
}

int showpagetag(char * baseurl,int curr,int total)
{
	printf("<form action=\"%s\" method=\"post\">",baseurl);
	printf("<p align=\"right\"><a href=\"%sstart=1\">第一页</a>&nbsp;",baseurl);
	if(curr>=pagesize)
		printf("<a href=\"%sstart=%d\">前一页</a>&nbsp;",baseurl,curr-pagesize+1);
	else
		printf("<font color=\"#666666\">前一页</a>&nbsp;");
	printf("<font color=\"red\">转到起始序号</font>&nbsp;<input type=\"text\" size=\"4\" value=\"%d\" name=\"start\" />"
	"<input type=\"submit\" value=\"Go\" />&nbsp;",curr+1);
	if(total-curr>pagesize)
		printf("<a href=\"%sstart=%d\">后一页</a>&nbsp;",baseurl,curr+pagesize+1);
	else
		printf("<font color=\"#666666\">后一页</a>&nbsp;");
	printf("<a href=\"%sstart=%d\">最末页</a></p></form>",baseurl,total/pagesize*pagesize+1);
	return 0;
}

int showatt()
{
	char buf[128];
	char buf2[128];
	char keyw[256];
	strsncpy(buf, getsenv("HTTP_USER_AGENT"), 120);
	if (strcasestr(buf,"baiduspider"))return 0;
	char lifemode=7;
	int i,j;
	int sm=0;
	struct fmg_FileRec frec;
	if(!*(getparm("life")))lifemode=7;else lifemode=atoi(getparm("life"));
	if(lifemode<0||lifemode>8||!mode_bm)lifemode=7;
	printf("<h2 align=center>%s 附件展区</h2>",x->filename);
	if(ad.fh.maxsize[7]>0)sprintf(buf,"%dMB",ad.fh.maxsize[7]);else strcpy(buf,"无上限");
	fmg_sizestr_d(buf2,ad.fh.itemsize[7]);
	printf("<p align=right>永久附件：<b>%s/%s</b>&nbsp;",buf2,buf);
	if(ad.fh.maxtotal>0)sprintf(buf,"%dMB",ad.fh.maxtotal);else strcpy(buf,"无上限");
	fmg_sizestr_d(buf2,ad.fh.totalsize);
	printf("所有附件：<b>%s/%s</b>&nbsp;",buf2,buf);
	printf("<a href=\"/cgi-bin/bbs/bbsdoc?board=%s\">返回本板</a>&nbsp;",board);
	if(mode_bm)
	{
		if(lifemode<8)
			printf("<a href=\"bbsatt?board=%s&life=8\">所有附件</a>",board);
		else
			printf("<a href=\"bbsatt?board=%s&life=7\">永久附件</a>",board);
	}
	printf("</p>");
	if(mode_bm)
		printf("<form action=\"bbsatt?board=%s&start=%d&life=%d\" method=\"post\">",board,start+1,lifemode);
	printf("<table align=\"center\" width=\"90%\"><tr bgcolor=\"%s\"><td align=\"right\"><font color=\"%s\">序号%s</font></td>"
	"<td align=\"center\"><font color=\"%s\">文件名</font></td><td align=\"center\"><font color=\"%s\">上传者</font></td>"
	"<td align=\"center\"><font color=\"%s\">文件大小</font></td><td align=\"center\"><font color=\"%s\">上传时间</font></td>"
	"<td align=\"center\"><font color=\"%s\">生存期</font></td><td align=\"center\"><font color=\"%s\">文件说明</font></td></tr>",
	mytheme.bar,mytheme.lf,mode_bm?"<input type=\"checkbox\" name=\"chkall\" onClick=\"for (var i=0;i<form.elements.length;i++)"
	"{var e = form.elements[i];if (e.name != 'chkall')e.checked = form.chkall.checked;}\" />":"",
	mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf);
	fseek(ad.fp,sizeof(struct fmg_FileHead),0);
	strsncpy(keyw,getparm("keyword"),250);
	sm=atoi(getparm("field"));
	if(sm<0||sm>1)sm=0;
	i=0;
	if(*keyw)
	{
		for(;;)
		{
			if(!fread(&frec,sizeof(frec),1,ad.fp))break;
			if(!*(frec.filename))continue;
			if(lifemode<8&&lifemode!=(frec.flag&7))continue;
			if(i>=start+pagesize)
			{
				if(sm==0)
				{
					if(strcasestr(frec.filename,keyw))
					{
						fseek(ad.fp,-sizeof(frec),1);
						start=i;
						break;
					}
				}
				else if(sm==1)
				{
					if(strcasestr(frec.info,keyw))
					{
						fseek(ad.fp,-sizeof(frec),1);
						start=i;
						break;
					}
				}
			}
			i++;
		}
		if(start!=i)
		{
			i=0;
			fseek(ad.fp,sizeof(struct fmg_FileHead),0);
			printf("<tr><td colspan=\"7\" align=\"center\"><font color=\"red\"><i><b>您搜索的内容未找到。</b></i></font></td></tr>");
		}
	}
	for(;;)
	{
		if(!fread(&frec,sizeof(frec),1,ad.fp))break;
		if(!*(frec.filename))continue;
		if(lifemode<8&&lifemode!=(frec.flag&7))continue;
		if(i>=start&&i<start+pagesize)
		{
			printf("<tr><td align=\"right\">%d",i+1);
			if(mode_bm)printf("<input type=\"checkbox\" name=\"chk%d\" value=\"checked\"><input type=\"hidden\" name=\"name%d\" value=\"%s\">",
				i-start,i-start,frec.filename);
			printf("</td><td><a href=\"/up2/%s\">%s</a></td>",urlencode(frec.filename+16),frec.filename);
			if(strcmp(frec.uploader,"SYSOPs")&&!strchr(frec.uploader,'.'))
				printf("<td><a href=\"bbsqry?userid=%s\">%s</a></td>",frec.uploader,frec.uploader);
			else
				printf("<td>%s</td>",frec.uploader);
			fmg_sizestr_d(buf,frec.filesize);
			printf("<td align=\"right\">%s</td>",buf);
			getdatestring(frec.timeupload,NA);
			printf("<td>%s</td>",datestring);
			printf("<td align=\"right\">%s</td><td>",fmg_lifestr[frec.flag&7]);
			if(mode_bm)printf("<input type=\"text\" name=\"info%d\" value=\"",i-start);
			hprintf("%s",frec.info);
			if(mode_bm)printf("\">");
			printf("</td></tr>");
		}
		i++;
	}
	i--;
	printf("</table>");
	if(mode_bm)
	{
		printf("<p><select name=\"act\"><option value=\"8\">选择操作</option><option value=\"-1\">立即删除</option>");
		for(j = 0; j < 8; j++)
		{
			printf("<option value=\"%d\">设置生存期为 %s </option>", j, fmg_lifestr[j]);
		}
		printf("<input type=\"button\" value=\"提交\" onClick=\"{if(confirm('您确定执行的操作吗?')){disabled=true;submit();return true;}return false;}\" />"
			"&nbsp;<input type=\"submit\" value=\"重写\" />&nbsp;");
		if(HAS_PERM(PERM_ANNOUNCE)||HAS_PERM(PERM_OBOARDS)||HAS_PERM(PERM_SYSOP))
			printf("<a href=\"bbsattadmin?board=%s\">高级管理</a>",board);
		printf("</p></form>");
	}
	sprintf(buf,"bbsatt?board=%s&life=%d&",board,lifemode);
	showpagetag(buf,start,i);
	printf("<form action=\"bbsatt?board=%s&start=%d&life=%d\" method=\"post\">",board,start+1,lifemode);
	hsprintf(keyw,"%s",keyw);
	printf("<p align=\"center\">快速搜索：<input type=\"text\" name=\"keyword\" value=\"%s\" />&nbsp;"
	"<input type=\"radio\" name=\"field\" value=\"0\" %s />搜索文件名&nbsp;<input type=\"radio\" name=\"field\" value=\"1\" %s />"
	"搜索说明 <input type=\"submit\" value=\"搜索下一个\" /></p></form>",keyw,sm==0?"checked":"",sm==1?"checked":"");
	return 0;
}

int main()
{
	init_all();
	if(loginok)modify_mode(u_info,READING);
	strsncpy(board, getparm("board"), 32);
	if(!(x=getbcache(board)))http_fatal("错误的讨论区");
	strcpy(board,x->filename);
	if(!has_read_perm(&currentuser,board)&&!HAS_PERM(PERM_SYSOP))http_fatal("您无权浏览此讨论区");
	mode_bm=HAS_PERM(PERM_ANNOUNCE)||has_BM_perm(&currentuser,board)||has_ZM_perm(&currentuser,board);
	if((*(getparm("act")))&&mode_bm)attact();
	ad=fmg_openattdir(board,1);
	if(!(ad.fp))http_fatal("无法打开附件索引");
	start=atoi(getparm("start"))-1;
	if(start<0||start>=ad.fh.reccount)start=0;
	showatt();
	fmg_closeattdir(&ad);
	http_quit();
	return 0;
}
