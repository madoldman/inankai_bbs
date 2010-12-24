#define MY_CSS "/bbs.css"
#include "BBSLIB.inc"
#define  SUBMANU  "&nbsp&nbsp"

#include "tshirt.h"

int main()
{
	int i,j,i1;
	int orderok=0;
	int tperm=0;
	init_all();
	if(!loginok)http_fatal("您尚未登录, 请先登录");
	if(!HAS_PERM(PERM_POST))http_fatal("对不起，您无权使用本页！");
	FILE * fp;
	struct Seller sl;
	if(!(fp=fopen(SELLERLIST,"r")))
		if(!(fp=fopen(SELLERLIST,"w+")))
			http_fatal("无法打开文件");
	for(i=1;;i++)
	{
		if(fread(&sl,sizeof(sl),1,fp)>0)
		{
			if(!strcasecmp(sl.userid,currentuser.userid))
			{
				tperm|=2;
				break;
			}
		}
		else break;
	}
	fclose(fp);
	if(TShirtAdmin()&8)tperm|=8;
	if(HAS_PERM(PERM_SYSOP))tperm|=138;
	printf("<p><a href=/tshirt_show.htm target=mainFrame>站衫样式</a></p>");
	printf("<p><a href=tshirt target=mainFrame>站衫征订</a></p>");
	if(tperm&138)printf("<p><a href=tshirtseek target=mainFrame>订单查询</a></p>");
	if(tperm&2)printf("<p><a href=tshirtsum target=mainFrame>销售统计</a></p>");
	if(tperm&8)printf("<p><a href=tshirtadmin target=mainFrame>销售代表</a></p>");
	if(tperm&8)printf("<p><a href=tshirtseller target=mainFrame>销售业绩</a></p>");
	if(tperm&8)printf("<p><a href=tshirtadminsum target=mainFrame>全局统计</a></p>");
	if(tperm&10)printf("<p><a href=tshirtrecycle target=mainFrame>被砍掉的</a></p>");
	printf("<p>%s<br>站衫自动征订系统 By Huangxu</p>",BBSNAME);
	http_quit();
}
