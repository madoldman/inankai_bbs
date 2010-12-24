#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int delorder(int id)
{
	FILE * fp;
	int i;
	ts.payed=-2;
	if(!(fp=fopen(TSHIRTTRACE,"ab+")))
		http_fatal("�޷�����־");
	fwrite(&ts,sizeof(ts),1,fp);
	fclose(fp);
	if(!(fp=fopen(TSHIRTRECYCLE,"r+")))
		http_fatal("�޷����ļ�");
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
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	//memset(&ts,0,sizeof(ts));
	char userid[16];
	FILE *fp;
	strncpy(userid,getparm("userid"),16);
	if(!(fp=fopen(TSHIRTRECYCLE,"r+")))
		http_fatal("�޷����ļ�");
	i=id=atoi(getparm("id"));
	if(!HAS_PERM(PERM_SYSOP))http_fatal("����Ȩʹ�ô�ҳ��");
	if(i<0)http_fatal("δ���ҵ��䶩����");
	fseek(fp,i*sizeof(ts),0);
	if(!fread(&ts,sizeof(ts),1,fp))http_fatal("�޷��ҵ�����!");
	if(strcasecmp(ts.userid,getparm("userid")))http_fatal("��֤����");
	fclose(fp);
	delorder(id);
	printf("<title>%s�Ķ���</title></head><body>",ts.userid);
	printf("<h3 align=center> %s �Ķ����Ѿ���������ˡ�������ٿ�һ�۰�</h3>",ts.userid);
	printf("<form action=tshirtconfirm method=post>");
	printf("<table align=center border=0>");
	printf("<tr><td>����ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td>���ĵ�ַ(40�ֽ�)</td><td>%s </td></tr>",ts.addr);
	printf("<tr><td>�ƶ��绰(20�ֽ�)</td><td>%s </td></tr>",ts.tel);
	printf("<tr><td>����绰(20�ֽ�)</td><td>%s </td></tr>",ts.tel2);
	printf("<tr><td>��ע(80�ֽ�)</td><td>%s </td></tr>",ts.memo);
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
		}
		printf("</td></tr>");
		printf("<input type=hidden name=style_%d value=%d>",i,ts.t[i].style);
		printf("<input type=hidden name=size_%d value=%d>",i,ts.t[i].size);
		printf("<input type=hidden name=count_%d value=%d>",i,ts.t[i].count);
	}
	printf("</table>");
	printf("<h4 align=center>����%d�����ϼƽ�%.2f</h4>",ts.flag,getprice(ts));
	printf("<Table align=center border=0><tr><td>");
	printf("</td><td><input type=reset value=���� onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

