#include <sys/types.h>
#include <regex.h>

#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int sellermatch(struct Seller * psl)
//IP匹配
{
	regex_t reg;
	regmatch_t match[10];
	int r;
	char buf[1024];
	if (regcomp(&reg, psl->ipreg, REG_EXTENDED))
	{
		return 0;
	}
	r = regexec(&reg, fromhost, sizeof(match) / sizeof(regmatch_t), match, 0);
	if (r)
	{
		r = 0;
	}
	else
	{
		r = !!match->rm_eo;
	}
	regfree(&reg);
	return r;
}

void showseller()
{
	printf("<script language=\"javascript\">var sellerinfo=Array(");
	FILE * fp;
	int i, t;
	struct Seller sl;
	char sellerbuf[256];
	if(fp=fopen(SELLERLIST,"rb"))
	{
		for(i=0;;i++)
		{
			if(!fread(&sl,sizeof(sl),1,fp))break;
			sprintf(sellerbuf,"<a target=_blank href=bbsqry?userid=%s>%s %s %s</a>",sl.userid,sl.userid,sl.addr,sl.tel2);
			printf("%s\"",i?",":"");
			jsprint(sellerbuf);
			printf("\"");
		}
	}
	int tmpint=0;
	printf(");function showinfo(i){document.getElementById('spninfo').innerHTML=(i>0)?sellerinfo[i-1]:'请选择一个销售代表';}</script>");
	printf("<p align=center>请选择销售代表：<select name=seller onchange=\"showinfo(this.value);\"><option value=0>请选择</option>");
	if(fp)
	{
		fseek(fp,0,0);
		for(i=1;;i++)
		{
			if(!fread(&sl,sizeof(sl),1,fp))break;
			printf("<option value=%d ",i);
			if (*ts.approved)//有效
			{
				t = !strcasecmp(sl.userid,ts.approved);
			}
			else
			{
				//新建的
				t = sellermatch(&sl);
			}
			if (t)
			{
				tmpint = i;
				printf("selected");
			}
			printf(">%s %s</option>", sl.addr,sl.userid);
		}
		fclose(fp);
	}
	printf("</select></p><p align=center><span id=spninfo></span></p>");
	printf("<script language=javascript>showinfo(%d);</script>",tmpint);
	return;
}

int main()
{
	int i,j,i1;
	init_all();
	FILE * fp;
	if(!loginok)http_fatal("您尚未登录, 请先登录");
	if(!HAS_PERM(PERM_POST))http_fatal("对不起，您无权使用本页！");
/*	sprintf(path,"%sbak",gettshirtfile(currentuser.userid));
	if(!(fp=fopen(path,"r+")))
		if(!(fp=fopen(path,"w+")))
			http_fatal("无法创建文件");
	fclose(fp);*/
	sprintf(path,"%s",gettshirtfile(currentuser.userid));
	if(!(fp=fopen(path,"r+")))
		if(!(fp=fopen(path,"w+")))
			http_fatal("无法创建文件");
	printf("<title>%s的订单</title></head><body>",currentuser.userid);
	i=seekrec(fp,&ts,currentuser.userid);
	if(i<0)
	{//没有订单
		#ifdef TSHIRTCLOSED 
			if(strcasestr(currentuser.userid,"tshirt")!=currentuser.userid)	http_fatal("征订已经结束,谢谢您的关心");
		#endif
		memset(&ts,0,sizeof(ts));
		strcpy(ts.userid,currentuser.userid);
		*(ts.addr)=0;
		//strcpy(ts.addr,currentuser.address);
		*(ts.tel)=*(ts.tel2)=0;
		time(&(ts.torder));
		*(ts.approved)=0;
		memset(&ts.tapprove,sizeof(time_t),0);
		*(ts.memo)=0;
		ts.flag=0;
	}
	printf("<h3 align=center> %s 的订单</h3>",ts.userid);
	printf("<form action=tshirtorder method=post>");
	printf("<table align=center border=0>");
	printf("<tr><td>您的ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td>您的地址(40字节)*</td><td><input type=text name=addr value=\"%s\" /></td></tr>",ts.addr);
	printf("<tr><td>移动电话(20字节)*</td><td><input type=text name=tel value=\"%s\" /></td></tr>",ts.tel);
	printf("<tr><td>宿舍电话(20字节) </td><td><input type=text name=tel2 value=\"%s\" /></td></tr>",ts.tel2);
	printf("<tr><td>备注(80字节)</td><td><input type=text name=memo value=\"%s\" /></td></tr></table>",ts.memo);
	if(ts.tapprove)
		printf("<p align=center><a href=bbsqry?userid=%s>%s</a></p>",ts.approved,ts.approved);
		else showseller();
	//
	printf("<p align=center><a href=/tshirt_show.htm target=_blank>点这里查看样式</a></p>");
	//
	printf("<table align=center border=0><tr><td>样式</td><td>尺码</td><td>数量</td></tr>");
	for(j=0;j<10;j++)
	{
		printf("<tr><td><select name=style_%d>",j);
		for(i1=0;i1<TSHIRT_STYLE_COUNT;i1++)
			printf("<option value=%d %s>%s</option>",i1,(i1==ts.t[j].style)?"selected":"",TSHIRT_STYLE[i1]);
		printf("</select></td><td><select name=size_%d>",j);
		for(i1=0;i1<TSHIRT_SIZE_COUNT;i1++)
			printf("<option value=%d %s>%s</option>",i1,(i1==ts.t[j].size)?"selected":"",TSHIRT_SIZE[i1]);
		printf("</select></td><td><select name=count_%d>",j);
		for(i1=0;i1<16;i1++)
			printf("<option value=%d %s>%d</option>",i1,(i1==ts.t[j].count)?"selected":"",i1);
		printf("</select></td></tr>");
	}
	printf("</table>");
	printf("<h4 align=center>共计%d件，合计金额￥%.2f</h4>",ts.flag,getprice(ts));
	printf("<p align=center>");
	if(ts.tapprove)
		printf("您的订单已经结算，如有疑问，请与销售代表联系。<a href=tshirt_todel><font color=red>请求砍单</font></a>");
	else
		printf("<input type=submit value=提交 />");
	printf("</td><td><input type=reset value=重写 /></td></tr></table></form>");
	printf("</p>");
	if(ts.hash1)
	{
		printf("<form action=tshirtconfirm method=post>");
		printf("<p align=center>从销售代表处获得密码后在此输入"
		"<input type=text name=txtHASH /> 金额：<input type=text name=sum /><input type=submit value=%s></p>",
		(ts.tapprove)?"砍单":"结算");
		printf("</form>");
	}
//	printf("<Table align=center border=0><tr><td>");
	fclose(fp);
	http_quit();
}

