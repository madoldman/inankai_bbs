#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int maxpage(int total,int size)
{
	int i=total/size;
	if(total%size)i++;
	return i-1;
}

int main()
{
	int orderok=0;
	int typecount[16][16];
	int stylecount[16];
	int sizecount[16];
	int typecount2[16][16];
	int stylecount2[16];
	int sizecount2[16];
	int start=0;
	int perpage=20;
	int pagestat=0;
	int currpage=0;
	init_all();
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	if(!(TShirtAdmin()&8))http_fatal("��û��Ȩ�޷��ʱ�ҳ");
	FILE *fp;
	if(!(fp=fopen(gettshirtfile(""),"r")))
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
	memset(typecount2,0,sizeof(typecount2));
	memset(stylecount2,0,sizeof(stylecount2));
	memset(sizecount2,0,sizeof(sizecount2));
	fseek(fp,0,0);
	currpage=atoi(getparm("page"))-1;
	if(currpage<0)currpage=0;
	start=currpage*perpage;
	printf("<h3 align=center><a href=tshirtcsv >�������۴���Ķ������</a></h3>");
	printf("<table align=center border=0><tr><td>�û�</td><td>�Ƽ�</td><td>�Ƽ�</td><td>��������</td><td>����״̬</td><Td>���۴���</td><td>�鿴</td></tr>");
	for(;;i++)
	{
		if(!(l=fread(&ts,sizeof(ts),1,fp)))break;
		if(i>=start&&i<start+perpage)
		{
			getdatestring(ts.torder,NA);
			printf("<tr><td><a href=bbsqry?userid=%s>%s</a></td><td>%d</td><td>%.2f</td><td>%s</td>",
			ts.userid,ts.userid,ts.flag,getprice(ts),datestring);
			getdatestring(ts.tapprove,NA);
			printf("<td><a href=tshirtsum?userid=%s title=�鿴�����۴�����������>%s</a></td>",ts.approved,ts.tapprove?datestring:"δ����");
			printf("<td><a href=bbsqry?userid=%s>%s</a></td><td><a href=tshirtseek?userid=%s>%d</a></td></tr>",ts.approved,ts.approved,ts.userid,i+1);
		}
		if(ts.tapprove)
		{
			sum+=ts.flag;
			count+=getprice(ts);
			for(j=0;j<10;j++)
				typecount[ts.t[j].size][ts.t[j].style]+=ts.t[j].count;
		}
		sum2+=ts.flag;
		count2+=getprice(ts);
		for(j=0;j<10;j++)
			typecount2[ts.t[j].size][ts.t[j].style]+=ts.t[j].count;
	}
	pagestat=i;
	fclose(fp);
	printf("</table>");
	printf("<form action=tshirtadminsum method=get><p align=right>");
	printf("<a href=tshirtadminsum>��һҳ</a>&nbsp;");
	if(currpage>0)
		printf("<a href=tshirtadminsum?page=%d>ǰһҳ</a>&nbsp;",currpage);
	else
		printf("<font color=#666666>ǰһҳ</font>&nbsp;");
	printf("<font color=red>��ǰҳ�룺 %d/%d</font>&nbsp;",currpage+1,maxpage(pagestat,perpage)+1);
	printf("<font color=green>���� %d �ݶ���</font>&nbsp;",pagestat);
	printf("��ת����<input type=text name=page value=%d size=4>ҳ<input type=submit value=Go>&nbsp;",currpage+1);
	if(pagestat>start+perpage)
		printf("<a href=tshirtadminsum?page=%d>��һҳ</a>&nbsp;",currpage+2);
	else
		printf("<font color=#666666>��һҳ</font>&nbsp;");
	printf("<a href=tshirtadminsum?page=%d>��ĩҳ</a>&nbsp;",maxpage(pagestat,perpage)+1);
	printf("</p></form>");
	printf("<table align=center border=1><tr><td>&nbsp;</td><td>�Ƽ�</td><td>�Ƽ�</td></tr>");
	printf("<tr><td>�����ܼ�</td><td>%d</td><td>%.2f</td></tr>",sum2,count2);
	printf("<tr><td>�ѽ����</td><td>%d</td><td>%.2f</td></tr></table>",sum,count);
	printf("<h4 align=center>���ж���ͳ��</h4>");
	printf("<table align=center border=1><tr><td>����</td>");
	for(i=1;i<TSHIRT_STYLE_COUNT;i++)
		printf("<td>%s</td>",TSHIRT_STYLE[i]);
	printf("<td>С��</td></tr>");
	for(i=1;i<TSHIRT_SIZE_COUNT;i++)
	{
		printf("<tr><td>%s</td>",TSHIRT_SIZE[i]);
		for(j=1;j<TSHIRT_STYLE_COUNT;j++)
		{
			stylecount2[j]+=typecount2[i][j];
			sizecount2[i]+=typecount2[i][j];
			printf("<td>%d</td>",typecount2[i][j]);
		}
		printf("<td>%d</td></tr>",sizecount2[i]);
	}
	printf("<tr><td>С��</td>");
	for(i=1;i<TSHIRT_STYLE_COUNT;i++)
		printf("<td>%d</td>",stylecount2[i]);
	printf("<td>%d</td></tr></table>",sum2);
	printf("<p>&nbsp;</p>");
	printf("<h4 align=center>�ѽ��㶩��ͳ��</h4>");
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
	http_quit();
}

