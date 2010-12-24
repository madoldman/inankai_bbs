//NEWLIB.inc

//htmlmode:
//bit0:HTML/Text
//bit1:No QMD
//bit2:No Ref
//bit3:No title
//bit8:XMLFile

//boardmode:
//0=normal
//1=junk
//2=digest

char * BOARDINDEX[]={".DIR",".junk",".DIGEST"};
char * CLRHL[]={"#808080","#FF0000","#00FF00","#FFFF00","#0000FF","#FF00FF","#00FFFF","#FFFFFF",""};//高亮
char * CLRLL[]={"#000000","#800000","#008000","#808000","#000080","#C00080","#008080","#C0C0C0",""};//低亮
char * ARRFONT[]={"","宋体","黑体","楷体_GB2312"};//字体，默认为宋体
char * CLRWEB[]={"#000000","#e00000","#00e000","#e0e000","#0000ff","d000d0","#00a0a0","#202020","#000000"};//WEB色调
char ESC=27;

int j_hsprintf(int act,char *s, char *fmt, ...) {
	static int studl,stfcl;//下划线，前景
	static char * stfnt;
	if(act==1)
	{
		stfnt=ARRFONT[0];
		stfcl=8;
		studl=0;
		return 1;
	}
	char src[512];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(src, 500, fmt, ap);
	va_end(ap);
	char stt[64];
	char stt2[64];
	char xmlchar[16];
	int pushi;//缓冲i
	int toolongi=16;//过长
	int intctl;//控制字符缓冲
	char strctl[16];//...
	int l=strlen(src);
	int lmp = 0;//last mark pos
	int i;
	char x,j;
	int charf = 0;
	int len;
	sprintf(stt,"color:%s;",CLRWEB[stfcl]);
	if (*stfnt)
	{
		sprintf(stt,"%sfont-family:%s;",stt,stfnt);
	}
	if(studl)
		strcat(stt,"border-bottom:solid+thin;");
	lmp = 0;
	sprintf(s,"<span style=\"%s\">",stt);
	*stt=0;
	len=strlen(s);
	for (i=0;i<l;i++)
	{
		x=*(src+i);
		if (x == 13 || x == 10)
		{
			;
		}
		else if (x == 27)
		{
		//case 10://换行
		//	s+=repeatspan(ste) + '<br>\n' + stt + '';
		//	break;
			if (src[i+1]==91)//'['
			{
				i+=2;
				pushi=i;
				intctl=0;
				for(;;i++)
				{
					j=src[i];
					if (j>47&&j<58)
						intctl=intctl*10+j-48;//数字，缓冲
					else if (j>64&&j<69)//移动光标，不支持
						break;
					else if (j==110||j==115||j==117||j==75||j==74)//6n，报告光标位置，不支持
						break;
					else if (j==73)//I
					{
						if (intctl<4)
							stfnt=ARRFONT[intctl];
						break;
					}
					//else if (j==58)//冒号命令，忽略
						//;
					else if (j==59||j==109)//m命令
					{
						if (intctl==0)
						{
							//stfnt='';//字体不变
							stfcl=8;//默认设置为8，也就是空字符串
							studl=0;
						}
						else if (intctl==2||intctl==4)
							studl=1;//下划线
						else if (intctl>29&&intctl<38)//3x，前景色
						{
							stfcl=intctl-30;
						}
						intctl=0;
						if (j==109)
							break;
					}
					else if ((j>64&&j<91)||(j>96&&j<123)||j==48)
						break;//字母和@退出
					if (i-pushi>toolongi)//太长的话……哼哼
					{
						i=pushi;
						break;
					}
				}
				if (len > 0 && *(s + len - 1) == '>')
				{
					len = lmp;
				}
				else
				{
				//处理样式
					strnncpy(s,&len,"</span>");
					lmp = len;
				}
				*stt=0;
				sprintf(stt,"color:%s;",CLRWEB[stfcl]);
				if (*stfnt)
				{
					sprintf(stt,"%sfont-family:%s;",stt,stfnt);
				}
				if(studl)
					strcat(stt,"border-bottom:solid+thin;");
				sprintf(stt2,"<span style=\"%s\">",stt);
				strnncpy(s,&len,stt2);
			}
		}
		else
		{
			if (charf || isalnum(x) || (x < 0 ))
			//GBK???!!!
			{
				*(s+len++)=x;
			}
			else
			{
				sprintf(xmlchar, "&#%d;", x);
				strnncpy(s, &len, xmlchar);
			}
			if (x < 0 )//是汉字
			{
				charf ^= 1;
			}
			else
			{
				charf = 0;
			}
		}
	}
	if (len > 0 && *(s + len - 1) == '>')
	{
		len = lmp;
	}
	else
	{
		strnncpy(s,&len,"</span>");
	}
	s[len]=0;
}


int j_hprintf(char *fmt, ...) {
	char buf[10000], buf2[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf2, 1023, fmt, ap);
	va_end(ap);
	j_hsprintf(0,buf, "%s", buf2);
	printf("%s", buf);
}

int j_hhprintf(char *fmt, ...) {
	char buf0[1024], buf[1024], *s, *getparm();
	int len=0;
	char currHOST[32];
	int my_link_mode;
        int highspeed=1;
	int my_localpic_mode;
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);
	buf[1023]=0;
	strncpy(currHOST,getsenv("SERVER_NAME"),30);
	//huangxu@060511:多IP
	s=buf;
	my_link_mode=atoi(getparm("my_link_mode"));
	char *h;
	h=getparm("highspeed");
	if(!strcmp(h,""))highspeed=1;else	highspeed=atoi(h);  
	my_localpic_mode=atoi(getparm("my_localpic_mode"));
	if(my_link_mode==1)return j_hprintf("%s", buf);
	while(*s)
	{
		if(!strncasecmp(s, "http://", 7) || !strncasecmp(s, "mailto:", 7) || !strncasecmp(s, "ftp://", 6)) 
		{
			char *href;
			if(len>0) {
				buf0[len]=0;
				j_hprintf("%s", buf0);
				len=0;
			}
			href=strtok(s, "\'\" \r\t)(,;\n");
			if(href==0) break;
			char *link=urlencode(href);                
			//huangxu@060331
			hostlink(link);
			if(strcasestr(href, ".gif")||strcasestr(href, ".png")||strcasestr(href, ".jpg")||strcasestr(href, ".jpe"))
			{
				if((my_localpic_mode==0)&&!strstr(href,BBSHOST)&&!strstr(href,"inankai.com") && !strstr(href,"inankai.net") &&!strstr(href,BBSIP))
				{
					printf("<a href=\"%s\">%s</a>",HOSTLINK,href);
				}
				else
					if(highspeed)printf("<br /><a href=\"%s\" target=\"_blank\"><img src=\"%s\"  border=\"0\" alt=\"点击看大图\"  "
					"onload=\"javascript:if(this.width>screen.width-333)this.width=screen.width-333;\" /></a><br />",HOSTLINK, HOSTLINK);
					else
						printf("<br /><a href=\"%s\">%s</a>",HOSTLINK,href);
					href=strtok(0, "");
					if(href==0) return;
					return j_hhprintf(href);
			}
			if(strcasestr(href, ".swf"))
			{
				printf("<br />Flash&nbsp;点击欣赏&nbsp;<a href=\"bbsflash?id=%s\" target=\"_blank\"> %s </a> <br />",HOSTLINK,nohtml(href));
				href=strtok(0, "");
				if(href==0) return;
				return j_hhprintf(href);
			}
			printf("<a target=\"_blank\" href=\"%s\">%s</a>", HOSTLINK, nohtml(href));
			href=strtok(0, "");
			if(href==0) return printf("\n");
			return j_hhprintf(href);
		}
		else
		{
			buf0[len]=s[0];
			if(len<1000) len++;
			s++;
		}
	}
	buf0[len]=0;
	j_hprintf("%s", buf0);
}

int j_show_file(char *path,int htmlmode)
{
	FILE *fp;
	char buf[512];
	if(!(fp=fopen(path, "r")))return 0;
	char *id,*s;
	int w=80;
	int con=1,qmd=0,cite=0,tit=1;
	if(htmlmode&1)//HTML
	{
		j_hsprintf(1,"","");
		printf("<pre>");
		for(;;)
		{
			if(fgets(buf, 500, fp)==0)break;
			if(strlen(buf)<2)
			{
				tit=0;
			}
			if(tit==1&&(htmlmode&8))continue;
			if(!strncmp(buf, ": ", 2) ||!strncmp(buf, "【 在 ", 4))
			{
				if(htmlmode&4)break;
				if(con==1&&qmd==0) 
				{
					con=0;
					cite=1;
				}
				j_hhprintf("%s", buf);
				printf("<br />");
			}
			else
			{
				if(!strncmp(buf, "--", 2)) 
				{
					con=0;
					qmd=1;
					if(htmlmode&2)break;
				}
				if(cite==1&&qmd==0) 
				{
					cite=0;
					con=1;
				}
				j_hhprintf("%s", buf);
				printf("<br />");
			}
		}
		printf("</pre>\n");
	}
	else
		for(;;)
		{
			if(fgets(buf, 500, fp)==0)break;
			if(!*buf)tit=0;
			if(tit==0||((htmlmode&8)==0))
				printf("%s",buf);
			if((htmlmode&4)&&(!strncmp(buf, ": ", 2)||!strncmp(buf, "【 在 ", 4)))break;
			if((htmlmode&2)&&(!strcmp(buf,"--")))break;
		}
	fclose(fp);
	return 1;
}

int utfconv(char * dest, char * src, int * plen)
{
	int r = code_convert ("utf-8", "gb18030", src, *plen, dest, 1024) < 0;
#ifdef UTFCONVTOURL
	memset (src, 0, *plen);
#endif
	*plen = 0;
	if (r < 0)
	{
		return 0;
	}
#ifdef UTFCONVTOURL
	strcpy (dest, urlencode(dest));
#endif
	strcpy (lastconv, dest);
	return strlen(dest);
}


