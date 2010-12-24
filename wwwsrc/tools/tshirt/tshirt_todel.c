#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int confirmorder()
{
	FILE * fp;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("无法打开日志");
	ts.hash1=ts_genhash();
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	FILE * fpm;
	char path[80];
	char title[80];
	if(!(fp=fopen(gettshirtfile(currentuser.userid),"r+")))
		http_fatal("无法打开文件");
	sprintf(path, "tmp/%d.tmp", getpid());
	fpm=fopen(path, "w");
	ts_append_record(fp);
	if(ts.flag)
	{//更新订单
		ts_append_record(fp);
		printf("您的请求已经发送。销售代表将与您联系。");
		sprintf(title,"订单信使：%s要求砍单",ts.userid);
		order2file(fpm);
		fprintf(fpm,"\n砍单密钥: %s",ts_hash_src);
	}
	fclose(fp);
	fclose(fpm);
	post_mail(ts.approved,title , path, "TShirt", "TShirt信使", "自动发信系统", -1, 0);
	unlink(path);
	return 1;
}

char* getsellerinfo(char * sellerid)
{
	FILE * fp;
	int i;
	struct Seller sl;
	static char sellerinfo[240];
	int tmpint=0;
	if(fp=fopen(SELLERLIST,"rb"))
	{
		fseek(fp,0,0);
		for(i=1;;i++)
		{
			if(!fread(&sl,sizeof(sl),1,fp))break;
			if(strcmp(sellerid,sl.userid))continue;
			sprintf(sellerinfo,"%s %s %s %s",sl.userid,sl.name,sl.addr,sl.tel2);
			break;
		}
		fclose(fp);
	}
	return sellerinfo;
}

int main()
{
	int i,j,i1;
	char path[80];
	int orderok=0;
	FILE * fp;
	init_all();
	if(!loginok)http_fatal("您尚未登录, 请先登录");
	if(!HAS_PERM(PERM_POST))http_fatal("对不起，您无权使用本页！");
	sprintf(path,"%s",gettshirtfile(currentuser.userid));
	if(!(fp=fopen(path,"r+")))
		if(!(fp=fopen(path,"w+")))
			http_fatal("无法创建文件");
	printf("<title>%s的订单</title></head><body>",currentuser.userid);
	i=seekrec(fp,&ts,currentuser.userid);
	fclose(fp);
	if(i<0)http_fatal("您下过订单了……？");
	if(!ts.tapprove)http_fatal("错误！您的订单尚未结算。如有疑问，请与销售代表联系。");
	if(*(getparm("act")))
	{
		if(!confirmorder())http_fatal("系统出错，请与系统维护联系！");
		return 0;
	}
	ts_htmlenc(&ts);
	printf("<title>%s的订单</title></head><body>",ts.userid);
	printf("<h3 align=center> %s 的订单</h3>",ts.userid);
	printf("<p align=center><font color=red>请记下销售代表的资料以便与其联系。</font></p>");
	printf("<form action=tshirt_todel?act=confirm method=post>");
	printf("<table align=center border=0>");
	printf("<tr><td>您的ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td><font color=red>您的地址(40字节)(必填)</font></td><td>%s </td></tr>",ts.addr);
	printf("<tr><td><Font color=red>移动电话(20字节)(必填)</font></td><td>%s </td></tr>",ts.tel);
	printf("<tr><td>宿舍电话(20字节)</td><td>%s </td></tr>",ts.tel2);
	printf("<tr><td>备注(88字节)</td><td>%s </td></tr>",ts.memo);
	printf("<tr><td><font color=red>销售代表(必填)</font></td><td>%s</td></tr></table>",(ts.approved));
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
	printf("<h4 align=center>共计%d件，合计金额￥%.2f</h4>",ts.flag,getprice(ts));
	printf("<Table align=center border=0><tr><td>");
	printf("<input type=submit value=我要砍单 />");
	printf("</td><td><input type=reset value=重写 onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

