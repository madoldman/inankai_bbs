#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int main()
{
	int i,j,i1;
	int id;
	int orderok=0;
	init_all();
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	//memset(&ts,0,sizeof(ts));
	char userid[16];
	strncpy(userid,getparm("userid"),16);
	FILE *fp;
	if(!(fp=fopen(TSHIRTRECYCLE,"r")))
		http_fatal("�޷����ļ�");
	i=atoi(getparm("id"));
	id=i;
	fseek(fp,sizeof(ts)*i,0);
	if(!fread(&ts,sizeof(ts),1,fp))http_fatal("��Ŵ���");
	if(!HAS_PERM(PERM_CHATCLOAK)&&strcasecmp(ts.approved,currentuser.userid))http_fatal("�˶�������������");
	fclose(fp);
	printf("<title>%s�Ķ���(�Ѿ�������)</title></head><body>",ts.userid);
	printf("<h3 align=center> %s �Ķ���(�Ѿ�������)</h3>",ts.userid);
	printf("<form action=tshirtrecycledel?id=%d method=post>",id);
	printf("<table align=center border=0>");
	printf("<tr><td>����ID</td><td> %s </td></tr>",ts.userid);
	printf("<tr><td>���ĵ�ַ(40�ֽ�)</td><td>%s </td></tr>",ts.addr);
	printf("<tr><td>�ƶ��绰(20�ֽ�)</td><td>%s </td></tr>",ts.tel);
	printf("<tr><td>����绰(20�ֽ�)</td><td>%s </td></tr>",ts.tel2);
	printf("<tr><td>��ע(80�ֽ�)</td><td>%s </td></tr>",ts.memo);
	printf("<tr><td>���۴���</td><td>%s</td></tr></table>",ts.approved);
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
	if(HAS_PERM(PERM_SYSOP))printf("<p align=center>�����û�ID�ӻ���վ��ɾ����<input type=text name=userid /></p>");
	printf("<Table align=center border=0><tr><td>");
	if(ts.tapprove)printf("�˶����Ѿ����㡣 ");
	if(HAS_PERM(PERM_SYSOP))printf("<input type=submit value=ȷ��>");
	printf("</td><td><input type=reset value=���� onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

