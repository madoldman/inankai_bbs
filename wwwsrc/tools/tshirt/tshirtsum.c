#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int main()
{
	int orderok=0;
	init_all();
	if(!loginok)http_fatal("您尚未登录, 请先登录");
	//memset(&ts,0,sizeof(ts));
	int i=0,j;
	int l;
	int x=0;
	int sum=0;
	int count=0;
	int sum2=0;
	int count2=0;
	char userid[16];
	int typecount[16][16];
	int stylecount[16];
	int sizecount[16];
	if(!HAS_PERM(PERM_POST))http_fatal("对不起，您无权使用本页！");
	memset(typecount,0,sizeof(typecount));
	memset(stylecount,0,sizeof(stylecount));
	memset(sizecount,0,sizeof(sizecount));
	strcpy(userid,getparm("userid"));
	if(!(TShirtAdmin()&8))strcpy(userid,currentuser.userid);
	if(!*userid)
	{
		printf("<form action=tshirtsum method=get>");
		printf("<p align=center>输入销售代表ID：<input type=text name=userid value=%s>&nbsp;<input type=submit value=提交></p>",currentuser.userid);
		printf("</form>");
		http_quit();
	}
	FILE *fp;
	if(!(fp=fopen(gettshirtfile(""),"r")))
		http_fatal("无法打开文件");
	fseek(fp,0,0);
	printf("<h3 align=center>销售代表%s的订单情况</h3>",userid);
	printf("<table align=center border=0><tr><td>用户</td><td>计件</td><td>计价</td><td>订单日期</td><td>结算状态</td><td>销售代表</td><td>查看</td></tr>");
	for(;;i++)
	{
		if(!(l=fread(&ts,sizeof(ts),1,fp)))break;
		if(*(ts.userid)&&!strcasecmp(ts.approved,userid))
		{
		getdatestring(ts.torder,NA);
		printf("<tr><td><a href=bbsqry?userid=%s>%s</a></td><td>%d</td><td>%.2f</td><td>%s</td>",
		ts.userid,ts.userid,ts.flag,getprice(ts),datestring);
		getdatestring(ts.tapprove,NA);
		printf("<td>%s</td>",ts.tapprove?datestring:"未结算");
		printf("<td><a href=bbsqry?userid=%s>%s</a></td><td><a href=tshirtseek?userid=%s>查看</a></td></tr>",ts.approved,ts.approved,ts.userid);
			if(ts.tapprove)
			{
				sum+=ts.flag;
				count+=getprice(ts);
				for(j=0;j<10;j++)
					typecount[ts.t[j].size][ts.t[j].style]+=ts.t[j].count;
			}
			sum2+=ts.flag;
			count2+=getprice(ts);
		}
	}
	fclose(fp);
	printf("</table>");
	printf("<table align=center border=1><tr><td>%s</td><td>计件</td><td>计价</td></tr>",userid);
	printf("<tr><td>订单总计</td><td>%d</td><td>%d</td></tr>",sum2,count2);
	printf("<tr><td>已结算的</td><td>%d</td><td>%d</td></tr></table>",sum,count);
	printf("<h4 align=center>已结算订单统计</h4>");
	printf("<table align=center border=1><tr><td>尺码</td>");
	for(i=1;i<TSHIRT_STYLE_COUNT;i++)
		printf("<td>%s</td>",TSHIRT_STYLE[i]);
	printf("<td>小计</td></tr>");
	for(i=1;i<TSHIRT_SIZE_COUNT;i++)
	{
		printf("<tr><td>%s</td>",TSHIRT_SIZE[i]);
		for(j=1;j<TSHIRT_STYLE_COUNT;j++)
		{
			stylecount[j]+=typecount[i][j];
			sizecount[i]+=typecount[i][j];
			printf("<td>%d</td>",typecount[i][j]);
		}
		printf("<td>%d</td></tr>",sizecount[i]);
	}
	printf("<tr><td>小计</td>");
	for(i=1;i<TSHIRT_STYLE_COUNT;i++)
		printf("<td>%d</td>",stylecount[i]);
	printf("<td>%d</td></tr></table>",sum);
	http_quit();
}

