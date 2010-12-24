#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];
int flag=0;//0=用户1=销售代表

int clearorder()
{
	FILE * fp;
	time(&(ts.tapprove));
	ts.payed=ts.flag;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("无法打开日志");
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	ts.hash1=0;//huangxu@07:a bug!
	FILE * fpm;
	char path[80];
	char title[80];
	if(!(fp=fopen(gettshirtfile(ts.userid),"r+")))
		http_fatal("无法打开文件");
	sprintf(path, "tmp/%d.tmp", getpid());
	fpm=fopen(path, "w");
	ts_append_record(fp);
	sprintf(title,"订单信使：%s的订单已经结算",ts.userid);
	order2file(fpm);
	if(flag) fprintf(fpm,"您的订单是由销售代表结算的。\n");
	fclose(fp);
	fclose(fpm);
	post_mail(ts.approved,title , path, "TShirt", "TShirt信使", "自动发信系统", -1, 0);
	post_mail(ts.userid,title , path, "TShirt", "TShirt信使", "自动发信系统", -1, 0);
	unlink(path);
	return 1;
}

int delorder()
{
	FILE * fp;
	int i;
	ts.payed=-1;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("无法打开日志");
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	FILE * fpm;
	char path[80];
	char title[80];
	if(!(fp=fopen(gettshirtfile(ts.userid),"r+")))
		http_fatal("无法打开文件");
	if(!(fpm=fopen(TSHIRTRECYCLE,"r+")))
		if(!(fpm=fopen(TSHIRTRECYCLE,"w+")))
			http_fatal("无法删除订单");
	ts_append_record(fpm);
	fclose(fpm);
	sprintf(path, "tmp/%d.tmp", getpid());
	fpm=fopen(path, "w");
	ts_del_record(fp);
	printf("您的订单已经被砍掉。");
	sprintf(title,"订单信使：%s的订单已经被砍掉",ts.userid);
	order2file(fpm);
	if(flag) fprintf(fpm,"您的订单是由销售代表砍掉的。\n");
	fclose(fp);
	fclose(fpm);
	post_mail(ts.approved ,title , path, "TShirt", "TShirt信使", "自动发信系统", -1, 0);
	post_mail(ts.userid ,title , path, "TShirt", "TShirt信使", "自动发信系统", -1, 0);
	unlink(path);
	return 1;
}

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
	if(*userid)flag=1; else strcpy(userid,currentuser.userid);
	FILE *fp;
	if(!(fp=fopen(gettshirtfile(userid),"r+")))
		http_fatal("无法打开文件");
	i=seekrec(fp,&ts,userid);
	fclose(fp);
	if(i<0)http_fatal("未能找到其订单。");
	if((int)(getprice(ts))!=atoi(getparm("sum"))&&strcasecmp(userid,getparm("del")))http_fatal("金额错误，请仔细核对！");
	if(flag==0)
	{
		char hashbuf[16];
		strncpy(hashbuf,getparm("txtHASH"),16);
		if(strlen(hashbuf)!=8)http_fatal("格式不正确");
		char *p;
		for(p=hashbuf;*p;p++)
		{
			if((*p>='a')&&(*p<='f'))
				(*p)-=32;
			else if((*p>='A')&&(*p<='F'))
				;
			else if((*p>='0')&&(*p<='9'))
				;
			else
				http_fatal("格式错误");
		}
		if(ts_comparehash(ts.hash1,hashbuf))
		{
			if(!ts.tapprove)
				ts_report("结算码错误");
			http_fatal("验证码不正确");
		}
		if(ts.tapprove)
		{
			delorder();
			return;
		}
		ts.hash1=0;
	}
	else
	{
		if(strcasecmp(ts.approved,currentuser.userid))
			http_fatal("此订单不归您管理。");
	}
	if(ts.tapprove)http_fatal("此订单已经结算！");
	if(!strcasecmp(getparm("del"),ts.userid))delorder();else clearorder();
	printf("<title>%s的订单</title></head><body>",ts.userid);
	printf("<h3 align=center> %s 的订单</h3>",ts.userid);
	printf("<form action=tshirtconfirm method=post>");
	printf("<table align=center border=0>");
	printf("<tr><td>您的ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td>您的地址(40字节)</td><td>%s </td></tr>",ts.addr);
	printf("<tr><td>移动电话(20字节)</td><td>%s </td></tr>",ts.tel);
	printf("<tr><td>宿舍电话(20字节)</td><td>%s </td></tr>",ts.tel2);
	printf("<tr><td>备注(88字节)</td><td>%s </td></tr>",ts.memo);
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
			printf("</td></tr>");
		}
		printf("<input type=hidden name=style_%d value=%d>",i,ts.t[i].style);
		printf("<input type=hidden name=size_%d value=%d>",i,ts.t[i].size);
		printf("<input type=hidden name=count_%d value=%d>",i,ts.t[i].count);
	}
	printf("</table>");
	printf("<h4 align=center>共计%d件，合计金额￥%.2f,操作完成！</h4>",ts.flag,getprice(ts));
	printf("<Table align=center border=0><tr><td>");
	printf("</td><td><input type=reset value=返回 onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

