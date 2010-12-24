/*$Id: adminseekmj.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
*/
#include "BBSLIB.inc"

#define MAXPUBIP (100)

struct IPRange
{
	unsigned int ip;
	char len;
};

struct IPUser
{
	char id[16];
	int count;
	struct IPUser * next;
};

struct IPAddr
{
	unsigned int ip;
	struct IPAddr * next;
	struct IPUser * user;
};

unsigned int MASK[]={0x0,0x1,0x3,0x7,0xf,0x1f,0x3f,0x7f,0xff,
	0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff,0xffff,
	0x1ffff,0x3ffff,0x7ffff,0xfffff,0x1fffff,0x3fffff,0x7fffff,0xffffff,
	0x1ffffff,0x3ffffff,0x7ffffff,0xfffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};
struct IPRange pubip[MAXPUBIP];
int cntpubip=0;
struct IPAddr * ipahead=0;
int cntipa=0;

unsigned int str2ip(char * ip)
{
	unsigned int r=0;
	int b=0;
	char * p;
	for(p=ip;;p++)
	{
		if(isdigit(*p))
		{
			b=b*10+(*p)-48;
		}
		else
		{
			r=((r&0xffffff)<<8)|(b&0xff);
			b=0;
			if(*p!='.')
				return r;
		}
	}
	return r;
}

char *ip2str(unsigned int ip)
{
	static char ipstr[20];
	sprintf(ipstr,"%d.%d.%d.%d",(ip>>24)&0xff,(ip>>16)&0xff,(ip>>8)&0xff,(ip)&0xff);
	return ipstr;
}

int anapublicip()
{
	FILE * fp;
	char * pbuf;
	char *p;
	int i,l;
	int b=0;
	unsigned int r=0;
	char ipl=0;
	char flag=0;
	if(!(fp=fopen("/home/bbs/etc/publicip","r")))
		if(!(fp=fopen("/home/bbs/etc/publicip","w")))
			http_fatal("公用IP设置错误");
	fseek(fp,0,2);
	l=ftell(fp);
	fseek(fp,0,0);
	if(!(pbuf=(char*)(malloc(l+1))))
		http_fatal("一般错误");
	fread(pbuf,1,l,fp);
	*(pbuf+l+1)=0;
	fclose(fp);
	for(i=0;i<l;i++)
	{
		if(isdigit(*(pbuf+i)))
		{
			if(flag)
				ipl=ipl*10+*(pbuf+i)-48;
			else
			{
				b=b*10+*(pbuf+i)-48;
			}
		}
		else if(*(pbuf+i)=='.')
		{
			if(!flag)
			{
				r=((r&0xffffff)<<8)|(b&0xff);
				b=0;
			}
		}
		else if(*(pbuf+i)=='/')
		{
			flag=1;
			ipl=0;
		}
		else
		{
			r=((r&0xffffff)<<8)|(b&0xff);
			b=0;
			if(cntpubip>=MAXPUBIP-1)break;
			if(!flag&&!r)continue;
			if(ipl==0)ipl=32;
			if(ipl>32)ipl=32;
			pubip[cntpubip].ip=r;
			pubip[cntpubip].len=ipl;
			cntpubip++;
			r=0;
			ipl=0;
			flag=0;
		}
	}
	free(pbuf);
	return 1;
}

int ispublicip(unsigned int ip)
{
	int i;
	if(ip==0)return 1;
	for(i=0;i<cntpubip;i++)
	{
		if((pubip[i].ip&MASK[pubip[i].len])==ip)
			return 1;
	}
	return 0;
}

int addipuser(struct IPAddr * ipa,char * userid)
{
	struct IPUser usr,*pusr,**p2usr;
	p2usr=&(ipa->user);
	for(pusr=ipa->user;pusr;pusr=*p2usr)
	{
		p2usr=&(pusr->next);
		if(!strcasecmp(userid,pusr->id))
		{
			return pusr->count++;
		}
	}
	if(!pusr)
	{
		if(!(pusr=(struct IPUser*)(malloc(sizeof(struct IPUser)))))
			return 0;
		pusr->next=0;
		pusr->count=1;
		strcpy(pusr->id,userid);
		*p2usr=pusr;
	}
	return 1;
}

int addip(unsigned int ip, char * userid)
{
	if(ispublicip(ip))return 0;
	struct IPAddr ipa,*pipa,**p2ipa;
	int i;
	p2ipa=&ipahead;
	for(pipa=ipahead;pipa;pipa=*p2ipa)
	{
		p2ipa=&(pipa->next);
		if(pipa->ip==ip)
		{
			addipuser(pipa,userid);
			break;
		}
	}
	if(!pipa)
	{
		if(!(pipa=(struct IPAddr*)(malloc(sizeof(struct IPAddr)))))
			return 0;
		pipa->next=0;
		pipa->ip=ip;
		pipa->user=0;
		*p2ipa=pipa;
		addipuser(pipa,userid);
	}
	return 1;
}

int anauser(char * userid)
{
	if(!*userid)return 0;
	FILE * fp;
	char ip[60];//50B
	char path[80];
	struct userec * x;
	if(!(x=getuser(userid)))
		return 0;
	strcpy(userid,x->userid);
	sethomefile(path,userid,"ipfile");
	if(!(fp=fopen(path,"r")))
		return 0;
	for(;fread(ip,1,50,fp);)
		addip(str2ip(ip),userid);
	fclose(fp);
	return 1;
}

int main()
{
	init_all();
	char id[20];
	int idl=0;
	int i;
	printf("<body background=%s ><font color=%s>",mytheme.bgpath,mytheme.mf);
	if(!HAS_PERM(PERM_ACCOUNTS))http_fatal("您无权使用此页面");
	printf("<h2 align=center>IP/马甲搜索器</h2>");
	printf("<h4 align=center><a href=adminfile?mode=5>点这里设置公共ip</a></h4>");
	anapublicip();
	char * pbuf=getparm("idlist");
	memset(id,0,sizeof(id));
	for(;*pbuf;pbuf++)
	{
		if(isalnum(*pbuf)&&idl<14)
		{
			*(id+idl)=*pbuf;
			idl++;
		}
		else
		{
			anauser(id);
			memset(id,0,sizeof(id));
			idl=0;
		}
	}
	anauser(id);
	struct IPAddr *ipa,*ipa2;
	struct IPUser *ipu,*ipu2;
	char * ipstr;
	for(ipa=ipahead;ipa;)
	{
		if(ipa->user->next)
		{
			ipstr=ip2str(ipa->ip);
			printf("<p>&nbsp;</p><p>以下用户从<b>%s</b>登录过:</p>",ipstr);
			printf("<table border=0>");
			i=0;
			for(ipu=ipa->user;ipu;)
			{
				if((i&3)==3)
					printf("<td><a href=bbsqry?userid=%s>%s</a> (<b>%d</b>次)</td></tr>",ipu->id,ipu->id,ipu->count);
				else if((i&3)==0)
					printf("<tr><td><a href=bbsqry?userid=%s>%s</a> (<b>%d</b>次)</td>",ipu->id,ipu->id,ipu->count);
				else
					printf("<td><a href=bbsqry?userid=%s>%s</a> (<b>%d</b>次)</td>",ipu->id,ipu->id,ipu->count);
				ipu2=ipu->next;
				free(ipu);
				ipu=ipu2;
				i++;
			}
			printf("</table>");
		}
		else
		{
			free(ipa->user);
		}
		ipa2=ipa->next;
		free(ipa);
		ipa=ipa2;
	}
	printf("<form action=adminseekmj method=post><p align=\"center\"><textarea name=idlist cols=32 rows=30></textarea><br /><input type=submit value=\"开始\" ><input type=reset value=\"取消\"></p></form>");
	printf("<p align=\"center\">注：此操作可能需要较长的时间，请耐心等待。</p>");
	return 0;
}

