#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int main()
{
	int i,j,i1;
	int orderok=0;
	init_all();
	if(!loginok)http_fatal("您尚未登录, 请先登录");
	if(!HAS_PERM(PERM_POST))http_fatal("对不起，您无权使用本页！");
	//memset(&ts,0,sizeof(ts));
	char userid[16];
	strncpy(userid,getparm("userid"),16);
	if(!*userid)
	{
		printf("<form action=tshirtseek method=get>");
		printf("<p align=center>输入ID：<input type=text name=userid><input type=submit value=提交></p>");
		printf("</form>");
		http_quit();
	}
	FILE *fp;
	if(!(fp=fopen(gettshirtfile(userid),"r")))
		http_fatal("无法打开文件");
	i=seekrec(fp,&ts,userid);
	if(strcasecmp(ts.approved,currentuser.userid)&&!(TShirtAdmin()&8))http_fatal("此订单不归您管理。");
	fclose(fp);
	if(i<0)http_fatal("未能找到其订单。");
	printf("<title>%s的订单</title></head><body>",ts.userid);
	printf("<h3 align=center> %s 的订单</h3>",ts.userid);
	printf("<form action=tshirtconfirm?userid=%s method=post>",ts.userid);
	printf("<table align=center border=0>");
	printf("<tr><td>您的ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td>您的地址(40字节)</td><td>%s </td></tr>",ts.addr);
	printf("<tr><td>移动电话(20字节)</td><td>%s </td></tr>",ts.tel);
	printf("<tr><td>宿舍电话(20字节)</td><td>%s </td></tr>",ts.tel2);
	printf("<tr><td>备注(80字节)</td><td>%s </td></tr>",ts.memo);
	printf("<tr><td>销售代表</td><td>%s</td></tr></table>",ts.approved);
	printf("<input type=hidden name=addr value=\"%s\"><input type=hidden name=tel value=\"%s\">",ts.addr,ts.tel);
	printf("<input type=hidden name=tel2 value=\"%s\"><input type=hidden name=memo value=\"%s\">",ts.tel2,ts.memo);
	printf("<input type=hidden name=seller value=\"%d\">",atoi(getparm("seller")));
	printf("<table align=center border=0><tr><td>样式</td><td>尺码</td><td>数量</td></tr>");
	for(i=0;i<10;i++)
	{
		if(ts.t[i].count)
		{
			printf("<tr><td>%s</td>",TSHIRT_STYLE[ts.t[i].style]);
			printf("</td><td>%s",TSHIRT_SIZE[ts.t[i].size]);
			printf("</td><td>%d",ts.t[i].count);
		}
		printf("</td></tr>");
		printf("<input type=hidden name=style_%d value=%d>",i,ts.t[i].style);
		printf("<input type=hidden name=size_%d value=%d>",i,ts.t[i].size);
		printf("<input type=hidden name=count_%d value=%d>",i,ts.t[i].count);
	}
	printf("</table>");
	printf("<h4 align=center>共计%d件，合计金额￥%.2f.</h4>",ts.flag,getprice(ts));
	if(!ts.tapprove)
	{
		printf("<p align=center>再次输入总金额结算：<input type=text name=sum /></p>");
		printf("<p align=center>此框输入用户ID强行砍掉此订单：<input type=text name=del /></p>");
	}
	printf("<Table align=center border=0><tr><td>");
	if(ts.tapprove)printf("此订单已经结算。 ");
	printf("<input type=submit value=确定 />");
	printf("</td><td><input type=reset value=返回 onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

