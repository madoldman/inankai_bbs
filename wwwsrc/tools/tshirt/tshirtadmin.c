#include "BBSLIB.inc"
#include "tshirt.h"

char path[80];
char boardallow[80],boardwrite[80];
char board[80];
void updateclub();

int updateseller()
{
	int l=atoi(getparm("count"));
	if(!*(getparm("count")))http_fatal("�����ݣ�");
	FILE * fp;
	struct userec *x;
	struct Seller sl;
	char strid[80];
	if(!(fp=fopen(SELLERLIST,"wb")))
		http_fatal("�޷����ļ�");
	int i;
	for(i=1;i<=l;i++)
	{
		memset(&sl,0,sizeof(sl));
		sprintf(strid,"id_%d",i);
		x=getuser(trim(getparm(strid)));
		if(!x)continue;
		strcpy(sl.userid,x->userid);
		sprintf(strid,"tel_%d",i);
		strncpy(sl.tel,getparm(strid),sizeof(sl.tel));
		sprintf(strid,"tel2_%d",i);
		strncpy(sl.tel2,getparm(strid),sizeof(sl.tel2));
		sprintf(strid,"addr_%d",i);
		strncpy(sl.addr,getparm(strid),sizeof(sl.addr));
		sprintf(strid,"name_%d",i);
		strncpy(sl.name,getparm(strid),sizeof(sl.name));
		sprintf(strid,"ipreg_%d",i);
		strncpy(sl.ipreg,getparm(strid),sizeof(sl.ipreg));
		fwrite(&sl,sizeof(sl),1,fp);
		printf("<p>�ɹ����%s ��</p>",x->userid);
	}
	printf("<p><a href=tshirtadmin>�鿴���</a></p>");
	fclose(fp);
	return 0;
}

int main()
{
	struct Seller sl;
	int i=0;
	init_all();
	if(!(TShirtAdmin()&8))http_fatal("��û��Ȩ�޷��ʱ�ҳ");
	if(strlen(getparm("act")))
	{
		updateseller();
		http_quit();
		return 0;
	}
	FILE *fp;
	printf("<script language=javascript src=/js/setseller.js></script>");
	printf("<body><form action=tshirtadmin?act=update method=post><h3 align=center>�趨���۴���</h3><p align=center>ע��ID��Сд����Ҫ����</p>");
	printf("<table align=center border=0><tbody id=tblmain><tr><td>ID</td><td>�ֻ�(20B)</td><td>�绰(20B)</td><td>��ַ(40B)</td><td>����(30B)</td>"
	"<td>ƥ��IP</td><td><a href=\"javascript:addseller();\">����</a></td></tr>");
	if(fp=fopen(SELLERLIST,"rb"))
	{
		for(i=1;;i++)
		{
			if(fread(&sl,sizeof(sl),1,fp)>0)
			{
				printf("<tr><td><input type=text name=id_%d value=\"%s\" /></a></td>",i,sl.userid);
				printf("<td><input type=text name=tel_%d value=\"%s\" /></td>",i,sl.tel);
				printf("<td><input type=text name=tel2_%d value=\"%s\" /></td>",i,sl.tel2);
				printf("<td><input type=text name=addr_%d value=\"%s\" /></td>",i,sl.addr);
				printf("<td><input type=text name=name_%d value=\"%s\" /></td>",i,sl.name);
				printf("<td><input type=text name=ipreg_%d value=\"%s\" /></td>", i, sl.ipreg);
				printf("<td><a href=\"javascript:delseller(%d);\">ɾ��</a></td></tr>",i);
			}
			else break;
		}
		i--;
		fclose(fp);
	}
	printf("</tbody></table>");
	printf("<p align=center><input type=submit value=�ύ />&nbsp;<input type=reset value=��д /></p>");
	printf("<input type=hidden name=act value=update />");
	printf("<input type=hidden name=count id=itemcount value=%d></form>",i);
	printf("</body></html>");
	return 0;
}
