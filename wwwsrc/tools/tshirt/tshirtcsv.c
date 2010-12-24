#include "BBSLIB.inc"
#include "tshirt.h"

#define tsoutput(x) TSoutput((x),sizeof(x))

void TSoutput(char * src,int length)
{
	char * p=src;
	printf("\"");
	for(p=src;!*p||p<src+length;p++)
	{
		printf("%c",*p);
		if(*p=='\"')printf("\"");
	}
	printf("\"");
	return;
}

int attach_init() {
	char *buf, buf2[1024], *t2, *t3;
	int n;
	srand(time(0)*2+getpid());
	chdir(BBSHOME);
	printf("Content-Type: application/octet-stream \n");
	printf("Content-Disposition: attachment; filename=tshirt.csv \n\n", CHARSET);
	n=atoi(getsenv("CONTENT_LENGTH"));
	if(n>5000000) n=5000000;
	buf=calloc(n+1, 1);
	if(buf==0) http_fatal("memory overflow");
	fread(buf, 1, n, stdin);
	buf[n]=0;
	t2=strtok(buf, "&");
	while(t2) {
		t3=strchr(t2, '=');
		if(t3!=0) {
			t3[0]=0;
			t3++;
			__unhcode(t3);
			parm_add(trim(t2), t3);
		}
		t2=strtok(0, "&");
	}
	strsncpy(buf2, getsenv("QUERY_STRING"), 1024);
	t2=strtok(buf2, "&");
	while(t2) {
		t3=strchr(t2, '=');
		if(t3!=0) {
			t3[0]=0;
			t3++;
			__unhcode(t3);
			parm_add(trim(t2), t3);
		}
		t2=strtok(0, "&");
	}
	strsncpy(buf2, getsenv("HTTP_COOKIE"), 1024);
	t2=strtok(buf2, ";");
	while(t2) {
		t3=strchr(t2, '=');
		if(t3!=0) {
			t3[0]=0;
			t3++;
			parm_add(trim(t2), t3);
		}
		t2=strtok(0, ";");
	}
	strsncpy(fromhost, getsenv("REMOTE_ADDR"), 32);
	seteuid(BBSUID);
	if(geteuid()!=BBSUID) http_fatal("uid error.");
	shm_init();
	loginok=user_init(&currentuser, &u_info);
        if(u_info==0){
             u_info=&guest;
        }
  return;
}

int main()
{
	attach_init();
	if(!loginok)return 0;
	if(!HAS_PERM(PERM_CHATCLOAK))return 0;
	FILE *fp;
	char buf[80];
	int i,j,l;
	if(!(fp=fopen(gettshirtfile(""),"r")))
		http_fatal("无法打开文件");
	fseek(fp,0,0);
	printf("\"订单编号\",\"用户\",\"订单日期\",\"结算日期\",\"销售代表\",\"地址\",\"电话1\",\"电话2\",\"备注\",");
	for(i=0;i<10;i++)
		printf("\"样式%d\",\"尺码%d\",\"数量%d\",",i+1,i+1,i+1);
	printf("\"计件\",\"计价\"\x0d\x0a");
	for(i=0;;i++)
	{
		if(!(fread(&ts,sizeof(ts),1,fp)))break;
		getdatestring(ts.torder,NA);
		strcpy(buf,datestring);
		*(buf+22)=0;
		printf("\"%d\",\"%s\",\"%s\",",i,ts.userid,buf);
		getdatestring(ts.tapprove,NA);
		strcpy(buf,datestring);
		*(buf+22)=0;
		printf("\"%s\",\"%s\",",(ts.tapprove)?buf:"",ts.approved);
		tsoutput(ts.addr);
		printf(",");
		tsoutput(ts.tel);
		printf(",");
		tsoutput(ts.tel2);
		printf(",");
		tsoutput(ts.memo);
		printf(",");
		for(j=0;j<10;j++)
			printf("\"%d\",\"%d\",\"%d\",",ts.t[j].style,ts.t[j].size,ts.t[j].count);
		printf("\"%d\",\"%.2f\"\x0d\x0a",ts.flag,getprice(ts));
	}
	fclose(fp);
	return 0;
}
