#define SELLERLIST "etc/tshirt/seller"
#define TSHIRTTRACE "etc/tshirt/trace"
#define TSHIRTRECYCLE "etc/tshirt/recycle"

//#define TSHIRTCLOSED

struct T_Shirt//4B
{
	char size;
	char style;
	char count;
	char flag;
};

struct TShirt//256B
{
	char userid[16];//16B
	char addr[40];//40B
	char tel[20];//20B
	char tel2[20];//20B
	time_t torder;//4B
	char approved[16];//16B
	int flag;//4B:���4λ��ʾ������
	int payed;//4B:�Ѿ�����
	time_t tapprove;//4B
	char memo[80];//80B
	int hash1;//4B
	int hash2;//4B
	struct T_Shirt t[10];//40B
};

struct Seller//256B
{
	char userid[16];
	char addr[40];
	char tel[20];
	char tel2[20];
	char name[32];
	char ipreg[128];
};

struct TShirt ts;


int TSHIRT_STYLE_COUNT=7;
int TSHIRT_SIZE_COUNT=12;

static char * TSHIRT_STYLE[]={"��ѡ��","��ʽ1","��ʽ2","��ʽ3","��ʽ4","��ʽ5","��ʽ6"};
static char * TSHIRT_SIZE[]={"��ѡ��","Ů��S","�п�S","Ů��M","�п�M","Ů��L","�п�L","Ů��XL","�п�XL","Ů��XXL","�п�XXL","�п�XXXL"};
static float TSHIRT_PRICE[]={0,25,25,25,25,25,25};


int TShirtAdmin()
{
	int r=0;
	if (HAS_PERM(PERM_CHATCLOAK))
		r|=8;
	if (HAS_PERM(PERM_SYSOP))
		r=-1;
	return r;
}

char * gettshirtfile(char * uid)
{
	static char tsp[80];
	sprintf(tsp,"etc/tshirt/orderlist");//����һ���ˣ�
	return tsp;
}

int seekrec(FILE * fp,struct TShirt * tst,char * userid)
//<0��ʾʧ��
//-1-x��ʾ��X��λ���п�λ
{
	if(!fp)return -1;
	int i=0;
	int l;
	int x=0;
	fseek(fp,0,0);
	for(;;i++)
	{
		if(!(l=fread(tst,sizeof(ts),1,fp)))
			return x?x:(-1-i);
		if(!(*(tst->userid)))
			x=-1-i;
		if(!strcasecmp(tst->userid,userid))
			return i;
	}
}

char * getseller(int i)
{
	FILE * fp;
	static char seller[16];
	struct Seller sl;
	*seller=0;
	if(i<=0)return seller;
	if(!(fp=fopen(SELLERLIST,"rb")))
		return seller;
	fseek(fp,(i-1)*sizeof(struct Seller),0);
	fread(&sl,sizeof(sl),1,fp);
	strcpy(seller,sl.userid);
	fclose(fp);
	return seller;
}

int getorderinfo(struct TShirt * tshirt)
{
	int r=1;
	int i,f=0;
	int t=0;
	char tmpstr[80];
	struct T_Shirt ts2;
	memset(tshirt,0,sizeof(struct TShirt));
	strcpy(tshirt->userid,currentuser.userid);
	strncpy(tshirt->addr,getparm("addr"),sizeof(tshirt->addr));
	strncpy(tshirt->tel,getparm("tel"),sizeof(tshirt->tel));
	strncpy(tshirt->tel2,getparm("tel2"),sizeof(tshirt->tel2));
	strncpy(tshirt->memo,getparm("memo"),sizeof(tshirt->memo));
	time(&(tshirt->torder));
	strcpy(tshirt->approved,getseller(atoi(getparm("seller"))));
	for(i=0;i<10;i++)
	{
		sprintf(tmpstr,"style_%d",i);
		ts2.style=(atoi(getparm(tmpstr)))&0xf;
		sprintf(tmpstr,"size_%d",i);
		ts2.size=(atoi(getparm(tmpstr)))&0xf;
		sprintf(tmpstr,"count_%d",i);
		ts2.count=(atoi(getparm(tmpstr)))&0xf;
		if(ts2.size&&ts2.style&&ts2.count)
		{
			tshirt->t[f++]=ts2;
			t+=ts2.count;
		}
	}
	tshirt->flag=t;
	if(t&&(!*(tshirt->approved)||!*(tshirt->tel)||!*(tshirt->addr)))r=0;
	return r;
}

int ts_append_record(FILE *fp) {
	if(fp==0) return 0;
	struct TShirt tmpts;
	flock(fileno(fp), LOCK_EX);
	int i=seekrec(fp,&tmpts,ts.userid);
	if(i<0)
		fseek(fp,0,2);
	else
		fseek(fp,sizeof(ts)*i,0);
	fwrite(&ts, sizeof(ts), 1, fp);
	flock(fileno(fp), LOCK_UN);
	return 1;
}

int ts_del_record(FILE *fp) {
	int total, i;
	char buf[4096];
	if(!fp) return 0;
	flock(fileno(fp), LOCK_EX);
	fseek(fp,0,2);
	total=ftell(fp)/sizeof(ts);
	struct TShirt tmpts;
	int num=seekrec(fp,&tmpts,ts.userid);
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
	return 1;
}

float getprice(struct TShirt tst)
{
	float r=0;
	int i;
	for(i=0;i<10;i++)
	{
		if(tst.t[i].style<0||tst.t[i].style>=TSHIRT_STYLE_COUNT)
			tst.t[i].style=0;
		r+=TSHIRT_PRICE[tst.t[i].style]*tst.t[i].count;
	}
	return r;
}

int order2file(FILE * fp)
{
	int i;
	fprintf(fp,"%s�Ķ���\n",ts.userid);
	fprintf(fp,"�û�ID��%s\n",ts.userid);
	fprintf(fp,"���ĵ�ַ��%s \n",ts.addr);
	fprintf(fp,"�ƶ��绰��%s \n",ts.tel);
	fprintf(fp,"����绰��%s \n",ts.tel2);
	fprintf(fp,"��ע��%s \n",ts.memo);
	fprintf(fp,"���۴���%s \n",ts.approved);
	fprintf(fp,"��ʽ\t����\t����\n");
	for(i=0;i<10;i++)
		if(ts.t[i].count)
		{
			fprintf(fp,"%s\t",TSHIRT_STYLE[ts.t[i].style]);
			fprintf(fp,"%s\t",TSHIRT_SIZE[ts.t[i].size]);
			fprintf(fp,"%d\n",ts.t[i].count);
		}
	fprintf(fp,"�Ƽ���%d \n",ts.flag);
	fprintf(fp,"�ܼۣ���%.2f \n\n",getprice(ts));
	getdatestring(ts.torder,NA);
	fprintf(fp,"����ʱ�䣺%s\n",datestring);
	return 1;
}

int ts_htmlenc(struct TShirt * tst)
{
	char buf[1024];
	hsprintf(buf,tst->addr);
	strncpy(tst->addr,buf,sizeof(tst->addr));
	hsprintf(buf,tst->tel);
	strncpy(tst->tel,buf,sizeof(tst->tel));
	hsprintf(buf,tst->tel2);
	strncpy(tst->tel2,buf,sizeof(tst->tel2));
	hsprintf(buf,tst->memo);
	strncpy(tst->memo,buf,sizeof(tst->memo));
	return 0;
}

char ts_hash_src[10];

int ts_genhash()
{
	int i;
	int hash=0;
	char BASEchar[]="0123456789ABCDEF";
	for(;!hash;)
	{
		for(i=0;i<8;i++)
		{
			ts_hash_src[i]=BASEchar[((random())&0xf)];
		}
		ts_hash_src[8]=0;
		memcpy(&hash,crypt1(ts_hash_src,ts_hash_src),sizeof(int));
		break;
	}
	return hash;
}

int ts_comparehash(int srchash,char * ts_srchash)
{
	int i;
	int hash=0;
	memcpy(&hash,crypt1(ts_srchash,ts_srchash),sizeof(hash));
	if(hash==srchash)return 0; else return 1;
}

void ts_report(char * title2)
{
	int info_type=1;
	char msgbuf[200];
	FILE *fp;
	char path[80], title[80];
	strcpy (title,title2);
	sprintf(path, "tmp/%d.tmp", getpid());
	fp=fopen(path, "w");
	getuinfo(fp);
	fclose(fp);
	post_article("syssecurity4", title, path, currentuser.userid, currentuser.username, "�Զ�����ϵͳ", -1);
	unlink(path);
	return;
}

/*
��ȷ��һ�������
һ�����ߵ�����ϵͳ��Ҫ������id��¼���ṩסַ���绰����������Ϣ��
ѡ��վ�����͡���С�ͼ�����ÿ�಻����9����
�ܹ�������99�������д�ģ�����ô��ƽṹ������
��û��ͨ������Ա��˵�ʱ�򣬿���������ʱ�޸ĺ�ȡ��������
һ��ͨ������Ա��ˣ��ö����������޸ġ�ͬʱȷ�Ϻ�Ķ����ᷴӳ��ͳ��ҳ�С�
����ǰ̨��

��̨����־Ը�ߺ͹���Ա��
־Ը���ܿ���������¥�����ж�����������յ�Ǯ֮�󣬿���ͨ������ҳ��
ȷ�Ͼ����ÿ�ݶ�����
�����վ����Կ����ж��������ȷ�ϵȡ�

����Ա���Ը������־Ը�ߺ�վ��ӹ���Ȩ�ޡ��������ҳ��Ҳ�У�����ô���㡣


��չ�Ĺ��ܱ���־Ը��ȷ�϶�������Զ����Ÿ��û����Ա��ٴ�ȷ�ϡ�
һ��ͨ������Ա��ˣ��ö����������޸ġ�ͬʱȷ�Ϻ�Ķ����ᷴӳ��ͳ��ҳ�С�
����ǰ̨��

��̨����־Ը�ߺ͹���Ա��
־Ը���ܿ���������¥�����ж�����������յ�Ǯ֮�󣬿���ͨ������ҳ��
ȷ�Ͼ����ÿ�ݶ�����
�����վ����Կ����ж��������ȷ�ϵȡ�

����Ա���Ը������־Ը�ߺ�վ��ӹ���Ȩ�ޡ��������ҳ��Ҳ�У�����ô���㡣


��չ�Ĺ��ܱ���־Ը��ȷ�϶�������Զ����Ÿ��û����Ա��ٴ�ȷ�ϡ�
�û����궩����ȷ�Ϻ�������¶���������ܹ����ܳ����û��ܶ�����

�����ܷ�ʵ�ֺ���û����©
*/
