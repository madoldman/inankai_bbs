/* $Id: bbsmainbuf.c,v 1.8 2009-12-29 06:41:51 maybeonly Exp $
*/

#include "BBSLIB.inc"
#define BS 512
char enname[32][32];
char cnname[32][32];
int newnum=0;
char bbsmain[]="/home/www/html/bbsmain.htm";
char hottopic10[]="/home/www/html/hottopic10.htm";
FILE * hfp;
char zm[SECNUM+1][128];

int publicboard(struct shortfile *x) {  
  if(!teamofboard ("guest",x->filename))return 0;
	if(x->level==0) return 1;
	if(x->level & (PERM_POSTMASK | PERM_NOZAP)) return 1;
	return 0;
}
int zonemaster()
{
	char *i,*j;
	char *p;
	int c=0;
	char src[BS];
	char buf[30];
	FILE *fp=fopen("etc/zonemaster","r");
	if(fp==NULL)return 0;
	j=buf;
	if(!fread(src,1,BS,fp))return 0;
	for(i=src;c<SECNUM;i++)
	{
		if(*i==0x0d)continue;
		if(*i==0x0a||*i==0)
		{
			*j=0;
			if(*buf)
				sprintf(zm[c],"<a href=\"/cgi-bin/bbs/bbsqry?userid=%s\">%s</a> ",buf,buf);
			else
				strcpy(zm[c],"无");
     	c++;
     	//if(!(*i))break;
     	j=buf;
    }
    else
    {
    	*j=*i;
    	j++;
    }
  }
  fclose(fp);
  return c;
}

int rmd()
{
	FILE *fp=fopen (PATHRMD,"r");
	if(fp==NULL)
	return 1;
	fprintf(hfp,"<DIV id= \"jingcaituijian\"><h3>精彩推荐</h3><ul>");
	struct rmd r;
	for(;;)
	{
		if(fread(&r, sizeof(struct rmd), 1, fp)<=0)break;
		if(r.sign==1)
			fprintf(hfp,"	<li>◇<a href=\"/cgi-bin/bbs/newcon?board=%s&amp;file=%s&amp;mode=3\">%s</a> "
			"<a href=\"/cgi-bin/bbs/bbsdoc?board=%s\">|%s|</a></li>",r.board,r.en,r.cn,r.board,r.board);
	}
	fprintf(hfp,"<li><div align=\"right\"><a href=\"/cgi-bin/bbs/rmd?mode=0\">更多精彩推荐...</a></div></li>");
	fprintf(hfp,"</ul></div>");
	fclose(fp);
	return 0;
}

int rmdboards()
	{
	FILE *fp;
	char ZoneLetter[]="0123456789CGNF";
	int rmdcnt=0;
	char buf[256];
	int i,j;
	char board[80];
	struct shortfile *x;
	char * bm;
	int z;
	char path[80];
	struct rmd r;
	zonemaster();
fprintf(hfp,
"<DIV id=\"botcontainer\" class=\"mainbox\">"
"<DIV class=\"notice_border\">"
"<DIV id=\"tuijianzone\">"
"<DIV id=\"patop\"><h3>讨论区推荐文章</h3></DIV>"
"<DIV id=\"patabs\">"
);
	for (i=0;i<SECNUM;i++)
	{
		rmdcnt=0;
		//if(i)strcpy(buf,"区长");else strcpy(buf,"站长");
		//sprintf(buf,"%s: %s",buf,zm[i]);
		//fprintf(hfp,"<div class=\"ma\"><div class=\"maheader\"><a href=\"/cgi-bin/bbs/rmd?mode=2&amp;&amp;group=%d\">%c区导读</a>&nbsp;"
		//"<a href=\"/cgi-bin/bbs/bbsboa?%d\"><strong>%s</strong></a> [%s]</div>",
		//i,ZoneLetter[i],i,secname[i],buf);
		if(i==0){
			fprintf(hfp,
"<ul id=\"tabs1\" class=\"patabs first\">"
"<li id=\"z0\" class=\"first\">"
"<div>"
"<h4><a id=\"paz0\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?0\">"
"<span class=\"icon\">本站系统</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z1\">"
"<div>"
"<h4><a id=\"paz1\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?1\">"
"<span class=\"icon\">南开大学</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z2\">"
"<div>"
"<h4><a id=\"paz2\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?2\">"
"<span class=\"icon\">电脑技术</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z3\">"
"<div>"
"<h4><a id=\"paz3\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?3\">"
"<span class=\"icon\">视觉联盟</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z4\">"
"<div>"
"<h4><a id=\"paz4\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?4\">"
"<span class=\"icon\">文学艺术</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z5\">"
"<div>"
"<h4><a id=\"paz5\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?5\">"
"<span class=\"icon\">人文社会</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z6\" class=\"last\">"
"<div>"
"<h4><a id=\"paz6\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?6\">"
"<span class=\"icon\">服务特区</span> </a></h4>"
"</div>"
"</li>"
"</ul><div id=\"tabs1previewdiv\" class=\"papreviewdiv\">"
);			
		}else if(i==7){
fprintf(hfp,
"</div><ul id=\"tabs2\" class=\"patabs last\">"
"<li id=\"z7\" class=\"first\">"
"<div>"
"<h4><a id=\"paz7\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?7\">"
"<span class=\"icon\">休闲娱乐</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z8\">"
"<div>"
"<h4><a id=\"paz8\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?8\">"
"<span class=\"icon\">知性感性</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"z9\">"
"<div>"
"<h4><a id=\"paz9\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?9\">"
"<span class=\"icon\">体育运动</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"zc\">"
"<div>"
"<h4><a id=\"pazC\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?10\">"
"<span class=\"icon\">社团群体</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"zg\">"
"<div>"
"<h4><a id=\"pazG\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?11\">"
"<span class=\"icon\">游戏天堂</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"zn\">"
"<div>"
"<h4><a id=\"pazN\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?12\">"
"<span class=\"icon\">新开板面</span> </a></h4>"
"</div>"
"</li>"
"<li id=\"zf\" class=\"last\">"
"<div>"
"<h4><a id=\"pazF\" class=\"tab_item\" name=\"tabitem\" href=\"http://all.inankai.net/cgi-bin/bbs/bbsboa?13\">"
"<span class=\"icon\">四海一家</span> </a></h4>"
"</div>"
"</li>"
"</ul><div id=\"tabs2previewdiv\" class=\"papreviewdiv\">"
);	
		}
		
		fprintf(hfp,"<table id=\"table_z%c\"class=\"tbltuijian\"><tbody>",ZoneLetter[i]);
		for(j=0; j<MAXBOARD; j++)
		{
			if(!(x=&(shm_bcache->bcache[j])))continue;
			if(x->filename[0]<=32||x->filename[0]>'z')continue;
			if(!strchr(seccode[i], x->title[0]))continue;
			if(!publicboard(x))continue;
			sprintf(path,"boards/%s/.thesis",x->filename);
			if(fp=fopen(path,"r"))
			{
				int total=file_size(path)/(sizeof(struct rmd));
				if(total==0)
				{
					fclose(fp);
					continue;
				}
				for(;;)
				{
					if(fread(&r, sizeof(struct rmd), 1, fp)<=0)break;
					if(rmdcnt&1)fprintf(hfp,"<td width=\"50%\">");
						else fprintf(hfp,"<tr><td width=\"50%\">");
					fprintf(hfp,"<a href=\"/cgi-bin/bbs/newcon?board=%s&file=%s&mode=3\"><font class=\"links\">%s%46.46s </font></a> "
					"<a href=\"/cgi-bin/bbs/bbsqry?userid=%s\"><font class=\"username\">%s</font></a> <a href=\"/cgi-bin/bbs/bbsdoc?board=%s\">"
					"%s</a>", x->filename, r.en,strncmp(r.cn, "Re: ", 4) ? "○ " : "",r.cn,r.owner,r.owner,x->filename,x->filename);
					if(rmdcnt&1)fprintf(hfp,"</td></tr>");
						else fprintf(hfp,"</td>");
					rmdcnt++;
				}
				fclose(fp);
			}
		}
	if(rmdcnt&1)fprintf(hfp,"</td>");
	fprintf(hfp,"</tbody></table>");
	}
	fprintf(hfp,"</div></DIV></DIV></DIV></DIV>");
	return 1;
}

int announce()
{
	FILE * fp;
	char * buf[BS];
	int i=0;
	int begin=0;
	char *p;
	fprintf(hfp,"<div id=\"top10\"><h3>近期热点</h3><ul>");
	if(fp=fopen("/home/www/html/news/bbsannounce","r"))
	{
		i=begin=0;
		while (fgets(buf,BS,fp)>0)
		{
			p=trim(buf);
			if(strcmp(p,""))
			{
				fprintf(hfp,"%s",p);
			}
		}
		fclose(fp);
	}
	fprintf(hfp,"</ul><hr class= \"stlhr\" /><UL>");
	if(fp=fopen("/home/www/html/news/campusannounce","r"))
	{
		i=begin=0;
		while (fgets(buf,BS,fp)>0)
		{
			p=trim(buf);
			if(strcmp(p,""))
			{
				fprintf(hfp,"%s",p);
			}
		}
		fclose(fp);
	}
	fprintf(hfp,"</ul></DIV><div class= \"hack\">&nbsp;</div>");
	return 0;
}
int showTop10(){
	FILE * fp;
	char * buf[BS];
	int i=0;
	int begin=0;
	char *p;
	fprintf(hfp,"<div id=\"top10\"><h3>十大话题</h3><ul>");
	if(fp=fopen("etc/posts/day","r"))
	{
		i=begin=0;
		while (fgets(buf,BS,fp)>0)
		{
			p=trim(buf);
			if(strcmp(p,""))
			{
				fprintf(hfp,"%s",p);
			}
		}
		fclose(fp);
	}
	fprintf(hfp,"</DIV>");
	return 0;
}

int showboard(char *name,char *path)
{
	char en[STRLEN],cn[STRLEN],buf[STRLEN];
	int r=0;
	int i=0;
	FILE *fp=fopen(path,"r");
	if(fp==NULL)
	return 1;
	while (fgets(buf,BS,fp)!=NULL)
	{
		if(strcmp(trim(buf),""))
		{
			r=sscanf(buf, "%s %s",en,cn);
			if(r==2)
			fprintf(hfp,"<a href=/cgi-bin/bbs/bbsdoc?board=%s>%s</a>\n",en,cn);
		}
	}
	fclose(fp);
}

int showboard2(char *name,char *path)
{
	char en[STRLEN],cn[STRLEN],buf[STRLEN];
	int r=0;
	int i=0;
	FILE *fp=fopen(path,"r");
	if(fp==NULL)
	return 1;
	while (fgets(buf,BS,fp)!=NULL)
	{
		if(strcmp(trim(buf),""))
		{
			r=sscanf(buf, "%s %s",en,cn);
			if(r==2)
			fprintf(hfp,"<a name=\"linkTJB\" class=\"boardLink\" href=/cgi-bin/bbs/bbsdoc?board=%s>%s</a>\n",en,cn);
		}
	}
	fclose(fp);
}
int newboard()
{
	int i;
	struct shortfile *x;
	for(i=0; i<MAXBOARD; i++)
	{
		x=&(shm_bcache->bcache[i]);
		if(x->filename[0]<=32 || x->filename[0]>'z') continue;
		if(!strchr(seccode[12], x->title[0]))   continue;
		if(!publicboard(x)) continue;
		strcpy(enname[newnum],x->filename);
		strcpy(cnname[newnum++],x->title+11);
	}
	if(newnum)
	{
		int i=0;
		//fprintf(hfp,"<DIV id= \"shikai\">");
		for(i=0;i<newnum;i++)
			fprintf(hfp,"<a name=\"linkTJB\" class=\"boardLink\" href=\"/cgi-bin/bbs/bbsdoc?board=%s\">%s</a> ",enname[i],cnname[i]);
		//fprintf(hfp,"</div>");
	}
	return 0;
}

void trimend(char* src,int lenth)
{
	char * p;
	for (p=src+lenth-1;*p==0x20||(!*p);p--)
		*p=0;
	return;
}

int hottopics()
{
	FILE *fp,*fpb;
	int n;
	char s1[256],s2[256];
	char brd[256],id[16], title[256];
	char title_in_url[240];
	fp=fopen("etc/posts/day", "r");
	if(fp==NULL)
		return 1;
	fpb=fopen("etc/bless/day", "r");
	if(fpb==NULL)
		return 1;
	fgets(s1, 255, fp);
	fgets(s1, 255, fp);
	fgets(s1, 255, fpb);
	fgets(s1, 255, fpb);
	fprintf(hfp,
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
"<HTML><HEAD><TITLE>十大话题</TITLE>"
"<META http-equiv=content-type content=\"text/html; charset=gbk\">"
"<META http-equiv=Pragma content=no-cache>"
"<META content=\"%s top10bless\" name=%s>"
"<STYLE type=text/css>"
"BODY {"
"	PADDING-RIGHT: 0px; PADDING-LEFT: 0px; PADDING-BOTTOM: 0px; FONT: 100.01% \"Trebuchet MS\",Verdana,Arial,sans-serif; PADDING-TOP: 0px; BACKGROUND-COLOR: #ffffff"
"}"
"DIV #nifty {"
"	BACKGROUND: white; MARGIN: auto; WIDTH: 640px; TEXT-ALIGN: center"
"}"
"B.rtop {"
"	DISPLAY: block; BACKGROUND: #fff"
"}"
"B.rbottom {"
"	DISPLAY: block; BACKGROUND: #fff"
"}"
"B.rtop B {"
"	DISPLAY: block; BACKGROUND: #0000cc; OVERFLOW: hidden; HEIGHT: 1px"
"}"
"B.rbottom B {"
"	DISPLAY: block; BACKGROUND: #0000cc; OVERFLOW: hidden; HEIGHT: 1px"
"}"
"B.rtop B.r4 {"
"	MARGIN: 0px 1px; HEIGHT: 2px"
"}"
"B.rbottom B.r4 {"
"	MARGIN: 0px 1px; HEIGHT: 2px"
"}"
".top10title {"
"BACKGROUND-COLOR: #ecf2ff; width:300px;"
"}"
".top10title2{"
"FONT-SIZE: 12px; COLOR: #003c95; BACKGROUND-COLOR: #ecf2ff; Text-Decoration:None;"
"}"
".top10id {"
"	FONT-SIZE: 12px; COLOR: #888888; FONT-FAMILY: \"Arial\", \"Helvetica\", \"sans-serif\"; BACKGROUND-COLOR: #ecf2ff;  TEXT-DECORATION: none;"
"}"
".top10board {"
"	FONT-WEIGHT: bold; FONT-SIZE: 12px; COLOR: #59ACFF; FONT-STYLE: italic; FONT-FAMILY: \"Courier New\", \"Courier\", \"mono\"; BACKGROUND:url(/image/ecf2ff.jpg); TEXT-ALIGN: center; TEXT-DECORATION: none;width:200px;"
"}"
".top10 {"
"	FONT-WEIGHT: bold; FONT-SIZE: 18pt; COLOR: #0080FF; FONT-FAMILY: \"楷体_GB2312\", \"宋体\", \"Fixedsys\"; TEXT-ALIGN: center; TEXT-DECORATION: none"
"}"
".top10blesstitlebless {"
"	FONT-SIZE: 12px; COLOR: #cf3f1c; BACKGROUND-COLOR:#FFEEE6; TEXT-ALIGN: right; TEXT-DECORATION: none; width:300px;"
"}"
".top10blessidbless {"
"	FONT-SIZE: 12px; COLOR: #C1BDBD; FONT-FAMILY: \"Arial\", \"Helvetica\", \"sans-serif\"; BACKGROUND:url(/image/FFEEE6.jpg); TEXT-ALIGN: center; TEXT-DECORATION: none;width:200px;"
"}"
".top10bless {"
"	FONT-WEIGHT: bold; FONT-SIZE: 18pt; COLOR: #ffffff; FONT-FAMILY: \"新宋体\", \"宋体\", \"Fixedsys\"; BACKGROUND-COLOR: #cc3300; TEXT-ALIGN: center; TEXT-DECORATION: none"
"}"
".top10blessboardblesslink {"
"	FONT-WEIGHT: bold; FONT-SIZE: 14px; COLOR: #ffffff; FONT-FAMILY: \"楷体_GB2312\"; BACKGROUND-COLOR: #cc3300; TEXT-ALIGN: center; TEXT-DECORATION: none"
"}"
".star {"
"	FONT-WEIGHT: bold; COLOR: #ff9966"
"}"
"#main tr{height:22px;}"
"</STYLE>"
"<META content=\"MSHTML 6.00.2900.2180\" name=GENERATOR></HEAD>"
"<BODY>"
"<table width=\"100%\" height=\"100%\">   "
"  <tr><td> "
"<TABLE id= \"main\" cellSpacing=0 cellPadding=0 border=0 align=\"center\">"
""
"  <TR>"
"    <TD class=top10 colSpan=4>★&nbsp&nbsp&nbsp<font color= #3333ff>热门话题钟点播报</font>&nbsp&nbsp&nbsp★<font color= #ff3300>&nbsp&nbsp&nbsp本日十大衷心祝福</font>&nbsp&nbsp&nbsp★</TD>"
"</TR>"
"  <TR height=20><td></td></TR>",BBSID,BBSID
);
	for(n=1; n<=10; n++) {
		if(fgets(s1, 255, fp)<=0)
			memset(s1,0,sizeof(s1));
		sscanf(s1+45, "%s", brd);
		sscanf(s1+122, "%s", id);
		if(fgets(s1, 255, fp)<=0)
			memset(s1,0,sizeof(s1));
		strsncpy(title, s1+27, 60);
		trimend(title,60);
		kill_quotation_mark(title_in_url,title);
		fprintf(hfp,
"<TR><TD class=top10title><A class=top10title2 href=\"/cgi-bin/bbs/newcon?board=%s&title=%s\">&nbsp;○%s </A>"
"<A class=top10id href=\"/cgi-bin/bbs/bbsqry?userid=%s\">%s</A></TD>"
"<TD class=top10board><A class=top10board href=\"/cgi-bin/bbs/bbs%sdoc?board=%s\">%s</A></TD><td>　</td></tr>"
,brd,title_in_url,title,id,id,(currentuser.userdefine & DEF_THESIS)?"t":"",brd,brd
);
		if(fgets(s2, 255, fpb)<=0)
			memset(s2,0,sizeof(s2));
		sscanf(s2+45, "%s", brd);
		sscanf(s2+122, "%s", id);
		if(fgets(s2, 255, fpb)<=0)
			memset(s2,0,sizeof(s2));
		strsncpy(title, s2+27, 60);
		if(!*title)
		{
			fprintf(hfp,
"<TR><td>　</td>"
"<TD class=top10blessidbless>&nbsp;"
"</TD><TD class=top10blesstitlebless>&nbsp;"
"</td></tr>"
);
			continue;
		}
		trimend(title,60);
		kill_quotation_mark(title_in_url,title);
		fprintf(hfp,
"<TR><td>　</td>"
"<TD class=top10blessidbless><A class=top10blessidbless href=\"/cgi-bin/bbs/bbsqry?userid=%s\">%s</A>"
"</TD><TD class=top10blesstitlebless><A class=top10blesstitlebless href=\"/cgi-bin/bbs/newcon?board=Blessing&title=%s\">"
"<SPAN class=star>★&nbsp;</SPAN>%s </A></td></tr>",id,id,title_in_url,title
);
	}
	fclose(fp);
	fclose(fpb);
	fprintf(hfp,
"</TABLE></td></tr></table></body><html>"
);
	return 0;
}

int main()
{
	html_init();
	int conmode=0;
	if(!*(getsenv("HTTP_HOST")))conmode=1;
	if(!conmode&&(!(HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_BOARDS))))http_fatal("您无权执行此操作");
	if(!(hfp=fopen(bbsmain,"w")))
		http_fatal("无法打开文件%s",bbsmain);
	printf("打开文件12……%s",conmode?"\n":"<br /> ");
	fprintf(hfp,
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"
"<HTML xml:lang=\"gbk\" xmlns=\"http://www.w3.org/1999/xhtml\">"
"<HEAD>"
"<TITLE>欢迎光临%s</TITLE>"
"<META http-equiv=Content-Type content=\"text/html; charset=gbk\">"
"<META http-equiv=Pragma content=no-cache>"
"<META content=\"MSHTML 6.00.2900.2963\" name=GENERATOR>"
""
"<META name=\"author\" content=\"%s\"> "
"<META NAME=\"description\" CONTENT=\"nankai-bbs\"> "
"<META NAME=\"keywords\" CONTENT=\"南开\"> "
"<LINK href=\"/css/bbsmain.css\" type=\"text/css\" rel=\"stylesheet\" id=\"csssheet\">\n"
"<script language=\"javascript\" src=\"/js/home_style.js\"></script>\n"
"<script language=\"javascript\" src=\"/js/bbsguide.js\"></script>\n"
"</HEAD>"
""
"<BODY bgcolor=\"#FFF\" style=\"text-align:center\" onload=\"init();\">"
""
"<DIV id=\"illicontainer\" class=\"mainbox\">"
"<DIV id=\"illi\" align=\"center\"></DIV>"
"</DIV>"
//,BBSNAME,BBSID,DEFSTYLE);//sillyhan
,BBSNAME,BBSID);
	printf("输出文件头……%s",conmode?"\n":"<br />");
	fprintf(hfp,
"<DIV id=\"illicontainer\" class=\"mainbox\">"
"<DIV id=\"nav\" class=\"clearfix\">"
"<UL>"
"<LI><A href=\"http://www.nklog.org/\" target=_blank>南开志</A></LI>"
"<LI><A href=\"http://we.inankai.net/\" target=\"_blank\">南开圈</A></LI>"
"</UL>"
"</DIV>"
"</DIV>"
);
	printf("顶部链接……%s",conmode?"\n":"<br />");
	fprintf(hfp,
"<DIV id=\"topcontainer\" class=\"mainbox\">"
"<DIV class=\"notice_border\" style=\"text-align: left; vertical-align: top;\">"
"<table width=\"100%\" border=\"0\"><tr><td id=\"top_left_container\" class=\"container\">"
);
	rmd();
	fprintf(hfp,"</td>");
	printf("顶部推荐文章……%s",conmode?"\n":"<br />");
	fprintf(hfp,"<td id=\"top_center_container\" class=\"container\"><div id=\"tj_new_boards\"><h3>推荐版面</h3>");
	showboard2("推荐讨论区",RMDBOARDS);
	fprintf(hfp,"<hr class=\"stlhr\" />"
"<h3>新开版面</h3>");
	newboard();
	fprintf(hfp,"</DIV>"
"<script language=\"javascript\">"
"var aryTempTJB = document.getElementsByName(\"linkTJB\");"
"for(var numI=0;numI<aryTempTJB.length;numI++){"
"aryTempTJB[numI].style.fontSize = (Math.floor(Math.random()*5)/5+1)+\"em\";"
"}</script>"
"</td><td id=\"top_right_container\" class=\"container\">");
	printf("推荐和新开讨论区……%s",conmode?"\n":"<br />");
	announce();
	printf("顶部公告……%s",conmode?"\n":"<br />");
	fprintf(hfp,"</td></tr></table></DIV></DIV>");
	//fprintf(hfp,"<DIV id= \"remen\"><div id=\"con\">");
	//showboard("热门讨论区",HOTBOARDS);
	rmdboards();
	time_t now;
	struct tm *tm;
	time(&now);
	tm = localtime(&now);
	printf("所有推荐文章……%s",conmode?"\n":"<br />");
	fprintf(hfp,"<div id='bottom_p' class=\"mainbox\"><p class=\"links\">欢迎光临!如果你对如何使用本站还不是很熟悉,建议您先去"
	"<a href=\"/cgi-bin/bbs/bbsdoc?board=BBSHelp\">BBSHelp</a>板看看.如果您在使用过程中遇到了解决不了的问题,你可以到"
	"<a href=\"/cgi-bin/bbs/bbsdoc?board=sysop\">SYSOP</a>反映.我们会以最快的速度给您满意的答复.<br />"
	"<b>发表文章,超大附件发送,收发邮件,个性化风格设定,个人文集自主建立</b>等等,<b>通过注册</b>,成为本站正式用户,立即拥有!一切就这么简单!"
	"<br /><br /><center><b>%s %s %4d</b></center></p><p></p></div></body></html>",BBSNAME,BBSHOST,tm->tm_year+1900);
	printf("底部提示信息……%s",conmode?"\n":"<br />");
	fclose(hfp);
	if(!(hfp=fopen(hottopic10,"w")))
		http_fatal("无法打开文件%s",hottopic10);
	hottopics();
	printf("生成十大……%s",conmode?"\n":"<br />");
	fclose(hfp);
	printf("关闭文件……%s",conmode?"\n":"<br />");
	if (conmode)
	{
		printf("生成首页成功！\n");
		return 0;
	}
	else
	{
		printf("<A href=/bbsmain.htm>生成首页成功！<a>");
		http_quit();
	}
}
