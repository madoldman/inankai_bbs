/*$Id: bbsmybrd.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"
struct shortfile * bptr;
char mybrd[GOOD_BRC_NUM][80];
int redirmode=0;
int mybrdnum=0;

int is_zone(char *filename,char *zonename)
{
        FILE *fp;
        struct boardheader myheader;
        if((fp=fopen(filename,"r"))==NULL)
        {
                return 0;
        }
        while(fread(&myheader,sizeof(struct boardheader),1,fp)>0)
        {
                if(strcmp(myheader.filename,zonename)==0 && (myheader.flag & ZONE_FLAG))
                {
                        return 1;
                }
        }
        return 0;
}

int main()
{
        int total=0,j, i,m,type;
        char path[200];
        char *brd;
  
        init_all();
        if(!loginok)
                http_fatal("尚未登录或者超时");
        redirmode=atoi(getparm("redirmode"));
        type=atoi(getparm("type"));
        if(type!=0) {
                read_submit();
                http_quit();
        }
	printf(
	"<title>收藏夹设置</title>"
	"<script language=\"javascript\" src=\"/js/drag.js\"></script>"
	"<script language=\"javascript\" src=\"/js/myfav.js\"></script>"
	"<script language=\"javascript\">"
	"function init()"
	"{"
	"	document.onmousemove = mouseMove;"
	"	document.onmousedown = mouseDown;"
	"	document.onmouseup = mouseUp;"
	"	dragHelper = document.createElement('DIV');"
	"	dragHelper.style.cssText = 'position:absolute;display:none;left:100px;top:100px;';"
	"	document.body.appendChild(dragHelper);"
	"	CreateDragContainer("
	);
	for(i=0;i<=13;i++)
		printf("document.getElementById('div_z%d_c'),",i);
	printf(
	"		document.getElementById('DragChecked')"
	"	);"
	"}"
	"</script>"
	"</head>"
	"<body onload=\"init();\">"
	);
	mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
	printf("<h3 align=\"center\">个人预定讨论区管理(您目前预定了%d个讨论区，最多可预定%d个)</h3><hr />\n", mybrdnum,GOOD_BRC_NUM);
	printf("<p align=\"center\">操作提示：鼠标拖动操作，预定一个板面则将其从左侧列表中拖放到右侧，取消预定则从右侧拖走。点击左侧加号可以展开。</p>");
	printf("<form action=\"bbsmybrd?type=1&confirm1=1\" method=\"post\">\n");
	printf("<input type=\"hidden\" name=\"result\" value=\"\">\n");
	printf("<table align=\"center\" boarder=\"0\"><tr><td width=\"200\" align=\"center\"><b>未预订的板面</b></td><td width=\"200\" align=\"center\" ><b>我的收藏夹</b></td></tr><tr><td valign=\"top\">");
	int cnt=0;
	for(j=0;j<=13;j++)
	{
		total=0;
		m=0;
		printf("<div id=\"div_z%d\" style=\"background-color:#cceeff;color:red;cursor:pointer;\" onclick=\"divshow(this);\">+ %s区</div>",j,trim( secname[j][0]));
		printf("<div id=\"div_z%d_c\" style=\"display:none;\">",j);
		for(i=0; i<MAXBOARD; i++) 
		{
			total++;
			cnt++;
			bptr=&shm_bcache->bcache[i];
			if(has_read_perm(&currentuser, bptr->filename) && !(bptr->flag & ZONE_FLAG))
				if(insec(j,bptr->filename))
					printf("<div id=\"brd.%s\" style=\"display:%s;cursor:move;\">%s/%s</div>",bptr->filename,ismyboard(bptr->filename)?"none":"",bptr->filename,bptr->title+11);
		}
		printf("</div>");
	}
	printf("</div></td><td valign=\"top\" id=\"DragChecked\">");
	for(i=0;i<MAXBOARD;i++)
	{
		bptr=&shm_bcache->bcache[i];
			if(has_read_perm(&currentuser, bptr->filename) && !(bptr->flag & ZONE_FLAG))
				if(ismyboard(bptr->filename))printf("<div id=\"%s\" style=\"cursor:move;background-color:#ffeecc;\">%s/%s</div>",bptr->filename,bptr->filename,bptr->title+11);
	}
	printf("</td></tr></table>");
        printf("<p align=\"center\"><input type=\"button\" value=\"确认预定\" onclick=\"result.value=getresult();submit();\"> <input type=\"button\" value=\"自动排序\" onclick=\"autoOrder();\"></p>\n");
        printf("</form>\n");
        http_quit();
}

int ismyboard(char *board)
{
        int i;
        for(i=0; i<mybrdnum; i++)
                if(!strcasecmp(board, mybrd[i]))
                        return 1;
        return 0;
}

int read_submit()
{
        int i, i1, i2, n;
        char buf1[200];
        char *brd;
        FILE *fp;
        int mybrdnum=0;
        char bufstr[4096];
        char * p,*pb;
        strncpy(bufstr,getparm("result"),4095);
	for(pb=p=bufstr;*p;p++) 
	{
		if(*p=='/')
		{
			*p=0;
			if(mybrdnum>=GOOD_BRC_NUM)
				http_fatal("您试图预定超过%d个讨论区",GOOD_BRC_NUM);
			if(!has_read_perm(&currentuser, pb)) {
				printf("警告: 无法预定'%s'讨论区<br>\n", pb);
			}
			else
			{
				strsncpy(mybrd[mybrdnum], pb, 80);
				mybrdnum++;
			}
			pb=p+1;
		}
	}
        SaveMyBrdsToGoodBrd(currentuser.userid, mybrd, mybrdnum);
        printf("<script>top.f2.location='bbsleft'</script>修改预定讨论区成功，您现在一共预定了%d个讨论区:<hr>\n", mybrdnum);
        printf("[<a href='javascript:history.go(%d)'>返回</a>]", redirmode?-1:-2);
}
