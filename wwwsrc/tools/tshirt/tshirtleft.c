#define MY_CSS "/bbs.css"
#include "BBSLIB.inc"
#define  SUBMANU  "&nbsp&nbsp"

#include "tshirt.h"

int main()
{
	int i,j,i1;
	int orderok=0;
	int tperm=0;
	init_all();
	if(!loginok)http_fatal("����δ��¼, ���ȵ�¼");
	if(!HAS_PERM(PERM_POST))http_fatal("�Բ�������Ȩʹ�ñ�ҳ��");
	FILE * fp;
	struct Seller sl;
	if(!(fp=fopen(SELLERLIST,"r")))
		if(!(fp=fopen(SELLERLIST,"w+")))
			http_fatal("�޷����ļ�");
	for(i=1;;i++)
	{
		if(fread(&sl,sizeof(sl),1,fp)>0)
		{
			if(!strcasecmp(sl.userid,currentuser.userid))
			{
				tperm|=2;
				break;
			}
		}
		else break;
	}
	fclose(fp);
	if(TShirtAdmin()&8)tperm|=8;
	if(HAS_PERM(PERM_SYSOP))tperm|=138;
	printf("<p><a href=/tshirt_show.htm target=mainFrame>վ����ʽ</a></p>");
	printf("<p><a href=tshirt target=mainFrame>վ������</a></p>");
	if(tperm&138)printf("<p><a href=tshirtseek target=mainFrame>������ѯ</a></p>");
	if(tperm&2)printf("<p><a href=tshirtsum target=mainFrame>����ͳ��</a></p>");
	if(tperm&8)printf("<p><a href=tshirtadmin target=mainFrame>���۴���</a></p>");
	if(tperm&8)printf("<p><a href=tshirtseller target=mainFrame>����ҵ��</a></p>");
	if(tperm&8)printf("<p><a href=tshirtadminsum target=mainFrame>ȫ��ͳ��</a></p>");
	if(tperm&10)printf("<p><a href=tshirtrecycle target=mainFrame>��������</a></p>");
	printf("<p>%s<br>վ���Զ�����ϵͳ By Huangxu</p>",BBSNAME);
	http_quit();
}
