#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];

int maxpage(int total,int size)
{
	int i=total/size;
	if(total%size)i++;
	return i-1;
}

struct SellerSum
{
	char id[16];
	float count,count2;
	int sum,sum2;
};

int getsellerp(char * id,struct SellerSum * sellers,int count)
{
	struct SellerSum * i;
	for(i=sellers;i<sellers+count;i++)
		if(!strcasecmp(id,i->id))
			return (i-sellers);
	return -1;
}

int main()
{
	int orderok=0;
	int i=0,j;
	int l;
	int x=0;
	int sum=0;
	float count=0;
	int sum2=0;
	float count2=0;
	int sc=0;
	struct SellerSum * pss,*pcs;
	init_all();
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	if(!(TShirtAdmin()&8))http_fatal("��û��Ȩ�޷��ʱ�ҳ");
	FILE *fp,*fps;
	struct Seller sl;
	if(!(fps=fopen(SELLERLIST,"r")))
		http_fatal("�޷����ļ�");
	fseek(fps,0,2);
	l=ftell(fps);
	fseek(fps,0,0);
	sc=l/sizeof(sl);
	if(!(pss=(struct SellerSum *)(malloc(sizeof(struct SellerSum)*sc))))
		http_fatal("һ�����");
	memset(pss,0,sizeof(struct SellerSum)*(sc));
	for(i=0;i<sc;i++)
	{
		fread(&sl,sizeof(sl),1,fps);
		strcpy((pss+i)->id,sl.userid);
		(pss+i)->sum=(pss+i)->sum2=0;
		(pss+i)->count=(pss+i)->count2=0;
	}
	fclose(fps);
	if(!(fp=fopen(gettshirtfile(""),"r")))
		http_fatal("�޷����ļ�");
	fseek(fp,0,0);
	for(i=0;;i++)
	{
		if(!(l=fread(&ts,sizeof(ts),1,fp)))break;
		float price=getprice(ts);
		j=(getsellerp(ts.approved,pss,sc));
		if(j<0)continue;
		pcs=pss+j;
		if(ts.tapprove)
		{
			pcs->sum+=ts.flag;
			sum+=ts.flag;
			pcs->count+=price;
			count+=price;
		}
		pcs->sum2+=ts.flag;
		sum2+=ts.flag;
		pcs->count2+=price;
		count2+=price;
	}
	fclose(fp);
	printf("<h3 align=center>����ҵ��(�Ƽ�):����=%d/%d</h3>",sum,sum2);
	printf("<table align=center border=0 width=700>");
	int max=0;
	for(i=0;i<sc;i++)
	{
		if((pss+i)->sum>max)max=(pss+i)->sum;
		if((pss+i)->sum2>max)max=(pss+i)->sum2;
	}
	for(i=0;i<sc;i++)
	{
		printf("<tr><td width=100><a href=\"bbsqry?userid=%s\">%s %d</a></td>",(pss+i)->id,(pss+i)->id,(pss+i)->sum);
		printf("<td width=600><img src=/bar2.gif height=20 width='%d' title=�ѽ���%d alt=�ѽ���%d></td></tr>",max?(pss+i)->sum*600/max:0,(pss+i)->sum,(pss+i)->sum);
		printf("<tr><td width=100><a href=\"tshirtsum?userid=%s\">�ܼ� %d</a></td>",(pss+i)->id,(pss+i)->sum2);
		printf("<td width=600><img src=/bar.gif height=20 width='%d' title=�ܼ�%d alt=�ܼ�%d></td></tr>",max?(pss+i)->sum2*600/max:0,(pss+i)->sum2,(pss+i)->sum2);
		printf("<tr><td></td></tr>");
	}
	printf("</table><p></p><p></p>");
	float fmax=0;
	for(i=0;i<sc;i++)
	{
		if((pss+i)->count>fmax)fmax=(pss+i)->count;
		if((pss+i)->count2>fmax)fmax=(pss+i)->count2;
	}
	printf("<h3 align=center>����ҵ��(�Ƽ�):����=%.2f/%.2f</h3>",count,count2);
	printf("<table align=center border=0 width=700>");
	for(i=0;i<sc;i++)
	{
		printf("<tr><td width=100><a href=\"bbsqry?userid=%s\">%s %.2f</a></td>",(pss+i)->id,(pss+i)->id,(pss+i)->count);
		printf("<td width=600><img src=/bar2.gif height=20 width='%d' title=�ѽ���%.2f alt=�ѽ���%.2f></td></tr>",fmax?(int)((pss+i)->count*600/fmax):0,(pss+i)->count,(pss+i)->count);
		printf("<tr><td width=100><a href=\"tshirtsum?userid=%s\">�ܼ� %.2f</a></td>",(pss+i)->id,(pss+i)->count2);
		printf("<td width=600><img src=/bar.gif height=20 width='%d' title=�ܼ�%.2f alt=�ܼ�%.2f></td></tr>",fmax?(int)((pss+i)->count2*600/fmax):0,(pss+i)->count2,(pss+i)->count2);
		printf("<tr><td></td></tr>");
	}
	printf("</table><p></p><p></p>");
	free(pss);
	http_quit();
}

