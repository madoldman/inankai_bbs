#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int delorder(int id)
{
	FILE * fp;
	int i;
	ts.payed=-2;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("无法打开日志");
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	if(!(fp=fopen(TSHIRTRECYCLE,"r+")))
		http_fatal("无法打开文件");
	int total;
	char buf[4096];
	if(!fp) return 0;
	flock(fileno(fp), LOCK_EX);
	fseek(fp,0,2);
	total=ftell(fp)/sizeof(ts);
	fseek(fp,sizeof(fp)*id,0);
	struct TShirt tmpts;
	int num=id;
	if(num<0) return 0;
	int size=sizeof(ts);
	for(i=num+1; i<=total-1; i++) {
		fseek(fp, i*size, SEEK_SET);
		if(fread(buf, size, 1, fp)<=0) break;
		fseek(fp, (i-1)*size, SEEK_SET);
		fwrite(buf, size, 1, fp);
	}
	ftruncate(fileno(fp), (total-1)*size);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	return 1;
}

int main()
{
	int i,j,i1,id;
	int orderok=0;
	init_all();
	if(!loginok)http_fatal("您尚未登录, 请先登录");
	//memset(&ts,0,sizeof(ts));
	char userid[16];
	FILE *fp;
	strncpy(userid,getparm("userid"),16);
	if(!(fp=fopen(TSHIRTRECYCLE,"r+")))
		http_fatal("无法打开文件");
	i=id=atoi(getparm("id"));
	if(!HAS_PERM(PERM_SYSOP))http_fatal("您无权使用此页。");
	if(i<0)http_fatal("未能找到其订单。");
	fseek(fp,i*sizeof(ts),0);
	if(!fread(&ts,sizeof(ts),1,fp))http_fatal("无法找到订单!");
	if(strcasecmp(ts.userid,getparm("userid")))http_fatal("验证错误");
	fclose(fp);
	delorder(id);
	printf("<title>%s的订单</title></head><body>",ts.userid);
	printf("<h3 align=center> %s 的订单已经彻底灭绝了……最后再看一眼吧</h3>",ts.userid);
	printf("<form action=tshirtconfirm method=post>");
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
	printf("<h4 align=center>共计%d件，合计金额￥%.2f</h4>",ts.flag,getprice(ts));
	printf("<Table align=center border=0><tr><td>");
	printf("</td><td><input type=reset value=返回 onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

