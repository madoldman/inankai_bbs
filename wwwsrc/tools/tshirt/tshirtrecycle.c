#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int main()
{
	int orderok=0;
	int typecount[16][16];
	int stylecount[16];
	int sizecount[16];
	init_all();
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	FILE *fp;
	if(!(fp=fopen(TSHIRTRECYCLE,"r")))
		http_fatal("�޷����ļ�");
	int i=0,j;
	int l;
	int x=0;
	int sum=0;
	float count=0;
	int sum2=0;
	float count2=0;
	memset(typecount,0,sizeof(typecount));
	memset(stylecount,0,sizeof(stylecount));
	memset(sizecount,0,sizeof(sizecount));
	fseek(fp,0,0);
	printf("<h3 align=center>�ѱ������������</h3>");
	printf("<table align=center border=0><tr><td>�û�</td><td>�Ƽ�</td><td>�Ƽ�</td><td>��������</td><td>����״̬</td><Td>���۴���</td><td>�鿴</td></tr>");
	for(;;i++)
	{
		if(!(l=fread(&ts,sizeof(ts),1,fp)))break;
		if(!(TShirtAdmin()&8)&&strcasecmp(ts.approved,currentuser.userid))continue;
		getdatestring(ts.torder,NA);
		printf("<tr><td><a href=bbsqry?userid=%s>%s</a></td><td>%d</td><td>%.2f</td><td>%s</td>",
		ts.userid,ts.userid,ts.flag,getprice(ts),datestring);
		getdatestring(ts.tapprove,NA);
		printf("<td>%s</td>",ts.tapprove?datestring:"δ����");
		printf("<td><a href=bbsqry?userid=%s>%s</a></td><td><a href=tshirtrecycledseek?id=%d>�鿴</a></td></tr>",ts.approved,ts.approved,i);
		if(ts.tapprove)
		{
			sum+=ts.flag;
			count+=getprice(ts);
			for(j=0;j<10;j++)
				typecount[ts.t[j].size][ts.t[j].style]+=ts.t[j].count;
		}
		sum2+=ts.flag;
		count2+=getprice(ts);
	}
	fclose(fp);
	printf("</table>");
	printf("<table align=center border=1><tr><td>&nbsp;</td><td>�Ƽ�</td><td>�Ƽ�</td></tr>");
	printf("<tr><td>�����ܼ�</td><td>%d</td><td>%.2f</td></tr>",sum2,count2);
	printf("<tr><td>�ѽ����</td><td>%d</td><td>%.2f</td></tr></table>",sum,count);
	/*printf("<h4 align=center>�ѿ����ѽ��㶩��ͳ��</h4>");
	printf("<table align=center border=1><tr><td>����</td>");
	for(i=1;i<TSHIRT_STYLE_COUNT;i++)
		printf("<td>%s</td>",TSHIRT_STYLE[i]);
	printf("<td>С��</td></tr>");
	for(i=1;i<TSHIRT_SIZE_COUNT;i++)
	{
		printf("<tr><td>%s</td>",TSHIRT_SIZE[i]);
		for(j=1;j<TSHIRT_STYLE_COUNT;j++)
		{
			stylecount[j]+=typecount[i][j];
			sizecount[i]+=typecount[i][j];
			printf("<td>%d</td>",typecount[i][j]);
		}
		printf("<td>%d</td></tr>",sizecount[i]);
	}
	printf("<tr><td>С��</td>");
	for(i=1;i<TSHIRT_STYLE_COUNT;i++)
		printf("<td>%d</td>",stylecount[i]);
	printf("<td>%d</td></tr></table>",sum);
	*/
	http_quit();
}

