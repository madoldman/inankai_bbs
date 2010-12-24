#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];
int flag=0;//0=�û�1=���۴���

int clearorder()
{
	FILE * fp;
	time(&(ts.tapprove));
	ts.payed=ts.flag;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("�޷�����־");
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	ts.hash1=0;//huangxu@07:a bug!
	FILE * fpm;
	char path[80];
	char title[80];
	if(!(fp=fopen(gettshirtfile(ts.userid),"r+")))
		http_fatal("�޷����ļ�");
	sprintf(path, "tmp/%d.tmp", getpid());
	fpm=fopen(path, "w");
	ts_append_record(fp);
	sprintf(title,"������ʹ��%s�Ķ����Ѿ�����",ts.userid);
	order2file(fpm);
	if(flag) fprintf(fpm,"���Ķ����������۴������ġ�\n");
	fclose(fp);
	fclose(fpm);
	post_mail(ts.approved,title , path, "TShirt", "TShirt��ʹ", "�Զ�����ϵͳ", -1, 0);
	post_mail(ts.userid,title , path, "TShirt", "TShirt��ʹ", "�Զ�����ϵͳ", -1, 0);
	unlink(path);
	return 1;
}

int delorder()
{
	FILE * fp;
	int i;
	ts.payed=-1;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("�޷�����־");
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	FILE * fpm;
	char path[80];
	char title[80];
	if(!(fp=fopen(gettshirtfile(ts.userid),"r+")))
		http_fatal("�޷����ļ�");
	if(!(fpm=fopen(TSHIRTRECYCLE,"r+")))
		if(!(fpm=fopen(TSHIRTRECYCLE,"w+")))
			http_fatal("�޷�ɾ������");
	ts_append_record(fpm);
	fclose(fpm);
	sprintf(path, "tmp/%d.tmp", getpid());
	fpm=fopen(path, "w");
	ts_del_record(fp);
	printf("���Ķ����Ѿ���������");
	sprintf(title,"������ʹ��%s�Ķ����Ѿ�������",ts.userid);
	order2file(fpm);
	if(flag) fprintf(fpm,"���Ķ����������۴������ġ�\n");
	fclose(fp);
	fclose(fpm);
	post_mail(ts.approved ,title , path, "TShirt", "TShirt��ʹ", "�Զ�����ϵͳ", -1, 0);
	post_mail(ts.userid ,title , path, "TShirt", "TShirt��ʹ", "�Զ�����ϵͳ", -1, 0);
	unlink(path);
	return 1;
}

int main()
{
	int i,j,i1;
	int orderok=0;
	init_all();
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	if(!HAS_PERM(PERM_POST))http_fatal("�Բ�������Ȩʹ�ñ�ҳ��");
	//memset(&ts,0,sizeof(ts));
	char userid[16];
	strncpy(userid,getparm("userid"),16);
	if(*userid)flag=1; else strcpy(userid,currentuser.userid);
	FILE *fp;
	if(!(fp=fopen(gettshirtfile(userid),"r+")))
		http_fatal("�޷����ļ�");
	i=seekrec(fp,&ts,userid);
	fclose(fp);
	if(i<0)http_fatal("δ���ҵ��䶩����");
	if((int)(getprice(ts))!=atoi(getparm("sum"))&&strcasecmp(userid,getparm("del")))http_fatal("����������ϸ�˶ԣ�");
	if(flag==0)
	{
		char hashbuf[16];
		strncpy(hashbuf,getparm("txtHASH"),16);
		if(strlen(hashbuf)!=8)http_fatal("��ʽ����ȷ");
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
				http_fatal("��ʽ����");
		}
		if(ts_comparehash(ts.hash1,hashbuf))
		{
			if(!ts.tapprove)
				ts_report("���������");
			http_fatal("��֤�벻��ȷ");
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
			http_fatal("�˶�������������");
	}
	if(ts.tapprove)http_fatal("�˶����Ѿ����㣡");
	if(!strcasecmp(getparm("del"),ts.userid))delorder();else clearorder();
	printf("<title>%s�Ķ���</title></head><body>",ts.userid);
	printf("<h3 align=center> %s �Ķ���</h3>",ts.userid);
	printf("<form action=tshirtconfirm method=post>");
	printf("<table align=center border=0>");
	printf("<tr><td>����ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td>���ĵ�ַ(40�ֽ�)</td><td>%s </td></tr>",ts.addr);
	printf("<tr><td>�ƶ��绰(20�ֽ�)</td><td>%s </td></tr>",ts.tel);
	printf("<tr><td>����绰(20�ֽ�)</td><td>%s </td></tr>",ts.tel2);
	printf("<tr><td>��ע(88�ֽ�)</td><td>%s </td></tr>",ts.memo);
	printf("<tr><td>���۴���</td><td>%s</td></tr></table>",ts.approved);
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
	printf("<h4 align=center>����%d�����ϼƽ�%.2f,������ɣ�</h4>",ts.flag,getprice(ts));
	printf("<Table align=center border=0><tr><td>");
	printf("</td><td><input type=reset value=���� onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

