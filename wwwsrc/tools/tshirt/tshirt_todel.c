#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int confirmorder()
{
	FILE * fp;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("�޷�����־");
	ts.hash1=ts_genhash();
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	FILE * fpm;
	char path[80];
	char title[80];
	if(!(fp=fopen(gettshirtfile(currentuser.userid),"r+")))
		http_fatal("�޷����ļ�");
	sprintf(path, "tmp/%d.tmp", getpid());
	fpm=fopen(path, "w");
	ts_append_record(fp);
	if(ts.flag)
	{//���¶���
		ts_append_record(fp);
		printf("���������Ѿ����͡����۴���������ϵ��");
		sprintf(title,"������ʹ��%sҪ�󿳵�",ts.userid);
		order2file(fpm);
		fprintf(fpm,"\n������Կ: %s",ts_hash_src);
	}
	fclose(fp);
	fclose(fpm);
	post_mail(ts.approved,title , path, "TShirt", "TShirt��ʹ", "�Զ�����ϵͳ", -1, 0);
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
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	if(!HAS_PERM(PERM_POST))http_fatal("�Բ�������Ȩʹ�ñ�ҳ��");
	sprintf(path,"%s",gettshirtfile(currentuser.userid));
	if(!(fp=fopen(path,"r+")))
		if(!(fp=fopen(path,"w+")))
			http_fatal("�޷������ļ�");
	printf("<title>%s�Ķ���</title></head><body>",currentuser.userid);
	i=seekrec(fp,&ts,currentuser.userid);
	fclose(fp);
	if(i<0)http_fatal("���¹������ˡ�����");
	if(!ts.tapprove)http_fatal("�������Ķ�����δ���㡣�������ʣ��������۴�����ϵ��");
	if(*(getparm("act")))
	{
		if(!confirmorder())http_fatal("ϵͳ��������ϵͳά����ϵ��");
		return 0;
	}
	ts_htmlenc(&ts);
	printf("<title>%s�Ķ���</title></head><body>",ts.userid);
	printf("<h3 align=center> %s �Ķ���</h3>",ts.userid);
	printf("<p align=center><font color=red>��������۴���������Ա�������ϵ��</font></p>");
	printf("<form action=tshirt_todel?act=confirm method=post>");
	printf("<table align=center border=0>");
	printf("<tr><td>����ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td><font color=red>���ĵ�ַ(40�ֽ�)(����)</font></td><td>%s </td></tr>",ts.addr);
	printf("<tr><td><Font color=red>�ƶ��绰(20�ֽ�)(����)</font></td><td>%s </td></tr>",ts.tel);
	printf("<tr><td>����绰(20�ֽ�)</td><td>%s </td></tr>",ts.tel2);
	printf("<tr><td>��ע(88�ֽ�)</td><td>%s </td></tr>",ts.memo);
	printf("<tr><td><font color=red>���۴���(����)</font></td><td>%s</td></tr></table>",(ts.approved));
	printf("<input type=hidden name=addr value=\"%s\"><input type=hidden name=tel value=\"%s\">",ts.addr,ts.tel);
	printf("<input type=hidden name=tel2 value=\"%s\"><input type=hidden name=memo value=\"%s\">",ts.tel2,ts.memo);
	printf("<input type=hidden name=seller value=\"%d\">",atoi(getparm("seller")));
	printf("<table align=center border=0><tr><td>��ʽ</td><td>����</td><td>����</td></tr>");
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
	printf("<h4 align=center>����%d�����ϼƽ�%.2f</h4>",ts.flag,getprice(ts));
	printf("<Table align=center border=0><tr><td>");
	printf("<input type=submit value=��Ҫ���� />");
	printf("</td><td><input type=reset value=��д onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

