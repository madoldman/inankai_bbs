#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

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
	if(!*userid)
	{
		printf("<form action=tshirtseek method=get>");
		printf("<p align=center>����ID��<input type=text name=userid><input type=submit value=�ύ></p>");
		printf("</form>");
		http_quit();
	}
	FILE *fp;
	if(!(fp=fopen(gettshirtfile(userid),"r")))
		http_fatal("�޷����ļ�");
	i=seekrec(fp,&ts,userid);
	if(strcasecmp(ts.approved,currentuser.userid)&&!(TShirtAdmin()&8))http_fatal("�˶�������������");
	fclose(fp);
	if(i<0)http_fatal("δ���ҵ��䶩����");
	printf("<title>%s�Ķ���</title></head><body>",ts.userid);
	printf("<h3 align=center> %s �Ķ���</h3>",ts.userid);
	printf("<form action=tshirtconfirm?userid=%s method=post>",ts.userid);
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
	printf("<h4 align=center>����%d�����ϼƽ�%.2f.</h4>",ts.flag,getprice(ts));
	if(!ts.tapprove)
	{
		printf("<p align=center>�ٴ������ܽ����㣺<input type=text name=sum /></p>");
		printf("<p align=center>�˿������û�IDǿ�п����˶�����<input type=text name=del /></p>");
	}
	printf("<Table align=center border=0><tr><td>");
	if(ts.tapprove)printf("�˶����Ѿ����㡣 ");
	printf("<input type=submit value=ȷ�� />");
	printf("</td><td><input type=reset value=���� onclick='history.go(-1);' /></td></tr></table></form>");
	http_quit();
}

