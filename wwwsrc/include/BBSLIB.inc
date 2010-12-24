#include "../../bbssrc/include/bbs.h"
#include "sys/ipc.h"
#include "sys/shm.h"
#include "stdarg.h"
#include "crypt.inc"
#include "myconfig.h"

#include <iconv.h>

#define DEFSTYLE "violet_spirit"

#define WMLS_POST_PER_PAGE 20
#define WMLS_LINE_PER_PAGE 100
#define WML_MODE_OFFSET 20000
#define WML_TOPIC_RANGE 100
#define WMLP_TPOST_PER_PAGE (WMLS_POST_PER_PAGE * 2)
#define WMLS_FRIEND_PER_PAGE 20
#define WMLS_USER_PER_PAGE 20
#define WML_VB_MAIL "*mail"
#define WML_VB_ANN "*ann"
#define WML_VB_SMD "*smd"
#define WML_VB_HEXIE "*hexie"
#define WML_VB_DENIED "*denied"

//#define PROXY_X_FORWARD
//#define MASK_NOT_MUST_LOGIN
#ifndef MASK_NOT_MUST_LOGIN
#define MUST_LOGIN
#endif

#define MUST_EXISTING_USER

#ifdef PROXY_X_FORWARD
#define SENVFROM "HTTP_X_FORWARDED_FOR"
#else
#define SENVFROM "REMOTE_ADDR"
#endif

//#include "showboard.h"

/* thunder  2003.5.27*/
struct theme{
                char en[16];
                char cn[16];
                char nav[8],bg[8],bar[8],con[8],lf[8],mf[8];
                char bgpath[80];
                char css[40];
                char lbg[40]; 
		
}mytheme;

struct file_to_appendix{
       int  sign;//0ÎªÎÄÕÂ,1ÎªÓÊ¼þ
       char path[20];
       int filename;
       char appendixname[STRLEN];
};
struct appendix {
       char name[STRLEN];
       int  size;
       char user[20];
       time_t time;
};
 /* ÖÃ¶¥ */


char* ZONEADMIN[]={"","FirstZone","SecondZone","ThirdZone","FourthZone","FifthZone","SixthZone","SeventhZone","EighthZone",
	"NinethZone","ClubZone","GameZone","","FamilyZone"};

char *urlencode(char *s);
int loginok =0;
int tmpmode=0;
struct userec currentuser;
struct user_info *u_info=0;
struct user_info guest;
struct UTMPFILE *shm_utmp;
struct BCACHE *shm_bcache;
struct UCACHE *shm_ucache;
char fromhost[256];

struct shortfile *getbcache();
struct userec *getuser();
char *strcasestr();
char *ModeType();
char *anno_path_of();
char *getparm(char *);



int msger(char *parm,char *link)
{
printf("<a  href=\"javascript: void open('%s','','top=100,left=150,width=350,height=150')\">%s</a>",parm,link);

}

int showinfo(char *fmt, ...)
{
      char buf[1024];
      va_list ap;
      va_start(ap, fmt);
      vsnprintf(buf, 1023, fmt, ap);
      va_end(ap);
      buf[1023]=0;
      printf("<p>&nbsp;</p><table width=40% border=0 cellspacing=0 cellpadding=0 align=center height=120 bgcolor=eeeee><tr><td bgcolor=#84ACFF height=25><font color=white> &nbsp;ÌáÊ¾</font></td></tr><tr><td><p align=center> %s</p><p  align=center><a href=javascript:history.go(-1) >µã»÷·µ»Ø</a> </p></td></tr><tr><td height=20>&nbsp;</td></tr></table>",buf);
     
}
/* end */

int junkboard(char *board) {
        // Çë×Ô¶¨Òåjunkboard.
        // return 0;
  //huangxu@070217:new junk flag
	struct shortfile *x;
	if (!(x=getbcache(board))) return 0;
	return x->flag2&JUNK_FLAG;
}
int toptenboard(char * board)
{
        struct shortfile *x;
        if (!(x=getbcache(board))) return 0;
        return !(x->flag2&NOTOPTEN_FLAG);
}

int boardForcePat(struct shortfile * x)
{
	return (x->flag2&TEMPLATE_FLAG)?1:0;
}        

struct post_log {
	char	author[IDLEN+1];
	char	board[18];
	char	title[66];
	time_t	date;
	int	number;
};

int file_has_word(char *file, char *word) {
	FILE *fp;
	char buf[256], buf2[256];
	fp=fopen(file, "r");
	if(fp==0) return 0;
	while(1) {
		bzero(buf, 256);
		if(fgets(buf, 255, fp)==0) break;
		sscanf(buf, "%s", buf2);
		if(!strcasecmp(buf2, word)) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

int f_append(char *file, char *buf) {
	FILE *fp;
	fp=fopen(file, "a");
	if(fp==0) return;
	fprintf(fp, "%s", buf);
	fclose(fp);
}
int fs_append(char *file, char *buf) {
        FILE *fp;
        fp=fopen(file, "a");
        if(fp==0) return 0;
      	flock(fileno(fp),LOCK_EX);
        fprintf(fp, "%s", buf);
	fflush(fp);
      	flock(fileno(fp),LOCK_UN);
	fclose(fp);
	return 1;
}

struct stat *f_stat(char *file) {
	static struct stat buf;
	bzero(&buf, sizeof(buf));
	if(stat(file, &buf)==-1) bzero(&buf, sizeof(buf));
	return &buf;
}

#define file_size(x) f_stat(x)->st_size
#define file_time(x) f_stat(x)->st_mtime
#define file_rtime(x) f_stat(x)->st_atime
#define file_exist(x) (file_time(x)!=0)
#define file_isdir(x) ((f_stat(x)->st_mode & S_IFDIR)!=0)
#define file_isfile(x) ((f_stat(x)->st_mode & S_IFREG)!=0)



void setdftstyle()
//huangxu@061130:new style!
{
	FILE * fp;
	char *p;
	char buf[8192];
	char style[32];
	char path[256],path2[256];
	strncpy(style,getparm("home_style"),30);
	//printf("<!--::%s::-->",style);
	*(buf+8191)=0;
	*(style+30)=0;
	for(p=style;*p;p++)
	{
		if(!strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_",*p))
		{
			*p=0;
			break;
		}
	}
	//if(!strcmp(style,".mytheme")&&(*(currentuser.userid)))
	//{
		//sprintf(path, "home/%c/%s/.www.css", toupper(currentuser.userid[0]), currentuser.userid);
		//sprintf(path2, "home/%c/%s/.www.theme", toupper(currentuser.userid[0]), currentuser.userid);
	//}
	//else
	//{
		if(!*style)
			strcpy(style,DEFSTYLE);
		//sprintf(path, "/css/%s/bbsguide4.css", style);
		sprintf(path2, "/home/www/html/css/%s/style", style);
	//}
	//if(fp=fopen(path,"rt"))
	//{
		//fread(buf,1,8000,fp);
		//fclose(fp);
	//}
	if(fp=fopen(path2,"rb"))
	{
		fread(&mytheme,1,sizeof(struct theme),fp);
		fclose(fp);
	}
	printf("<LINK href=\"/css/%s/bbsguide4.css\" type=\"text/css\" rel=\"stylesheet\" id=\"csssheet\">\n",style);
	//printf("<style>%s</style>",buf);
	//printf("<!--%s,%s-->",path,path2);
	return;
}


int setbfile(char *buf,char *board,char *filename)
{
      if(strcmp(filename,""))
          sprintf(buf,"boards/%s/%s",board,filename);
      else 
          strcpy(buf,"");
}

int get_num_records(char  *filename, int size)
{
        struct stat st;
        if (stat(filename, &st) == -1)
                return 0;
        return (st.st_size / size);
}

int get_record(void *buf, int size, int num, char *file) {
	FILE *fp;
	if(size<1 || size>4096) return 0;
	if(num<0 || num>1000000) return 0;
	bzero(buf, size);
	fp=fopen(file, "r");
	if(fp==0) return 0;
	fseek(fp, num*size, SEEK_SET);
	if(fread(buf, size, 1, fp)<=0) {
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

int put_record(void *buf, int size, int num, char *file) {
	FILE *fp;
	if(size<1 || size>4096) return 0;
	if(num<0 || num>1000000) return 0;
	fp=fopen(file, "r+");
	if(fp==0) return 0;
	fseek(fp, num*size, SEEK_SET);
	fwrite(buf, size, 1, fp);
	fclose(fp);
	return 1;
}



int append_record(void *buf, int size,  char *file) {
        FILE *fp;
        if(size<1 || size>4096) return 0;
        fp=fopen(file, "a+");
        if(fp==0) return 0;
        fwrite(buf, size, 1, fp);
        fclose(fp);
        return 1;
}


int del_record(char *file, int size, int num) {
	FILE *fp;
	int total, i;
	char buf[4096];
	if(size<1 || size>4096) return 0;
	total=file_size(file)/size;
	if(total<1 || total>1000000) return 0;
	fp=fopen(file, "r+");
	if(fp==0) return 0;
	flock(fileno(fp), LOCK_EX);
	for(i=num+1; i<=total-1; i++) {
		fseek(fp, i*size, SEEK_SET);
		if(fread(buf, size, 1, fp)<=0) break;
		fseek(fp, (i-1)*size, SEEK_SET);
		fwrite(buf, size, 1, fp);
	}
	ftruncate(fileno(fp), (total-1)*size);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	return 1;
}

char *Ctime(time_t t) {
	static char s[80];
	sprintf(s, "%24.24s", ctime(&t));
	return s;
}

char *noansi(char *s) {
	static char buf[1024];
	int i=0, mode=0;
	while(s[0] && i<1023) {
		if(mode==0) { 
			if(s[0]==27) {
				mode=1;
			} else {
				buf[i]=s[0];
				i++;
			}
		} else {
			if(!strchr(";[0123456789", s[0])) mode=0;
		}
		s++;
	}
	buf[i]=0;
	return buf;
}
char *nohtml(char *s) {
	char *buf=calloc(strlen(s)+1, 1);
	int i=0, mode=0;
	while(s[0] && i<1023) {
		if(mode==0) {
			if(s[0]=='<') {
				mode=1;
			} else {
				buf[i]=s[0];
				i++;
			}
		} else {
			if(s[0]=='>') mode=0;
		}
		s++;
	}
	buf[i]=0;
	return buf;
}
/*add by brew 2005.6.24*/
char *nohtmlex(char *s,int len)
{
        int i;
	char *buf=nohtml(s);
        i=strlen(buf);
        if(i>=len)
        {
		i=0;
		while(i<len)
		{
			if(buf[i]<0)
			{
				if(i==len-1)
				{
				  buf[i]=0;
				  return buf;
				}
				i++;
			}
			i++;
		}
		/*if(i==len)
			buf[i-1]=0;
	else
                {
			buf[len-1]=0;
			buf[len-2]=0;
                }*/
	//	if(buf[i-1]<0)
	//	  buf[i-1]=0;			  
		buf[i]=0;
        }
        return buf;
}

char *strright(char *s, int len) {
	int l=strlen(s);
	if(len<=0) return "";
	if(len>=l) return s;
	return s+(l-len);
}

char *strcasestr(char *s1, char *s2) {
	int l;
	l=strlen(s2);
	while(s1[0]) {
		if(!strncasecmp(s1, s2, l)) return s1;
		s1++;
	}
	return 0;
}

int strsncpy(char *s1, char *s2, int n) {
	int l=strlen(s2);
	if(n<0) return;
	if(n>l+1) n=l+1;
	strncpy(s1, s2, n-1);
	s1[n-1]=0;
}

char *ltrim(char *s) {
	char *s2=s;
	if(s[0]==0) return s;
	while(s2[0] && strchr(" \t\r\n", s2[0])) s2++;
	return s2;
}

char *rtrim(char *s) {
	static char t[1024], *t2;
	if(s[0]==0) return s;
	strsncpy(t, s, 1024);
	t2=t+strlen(s)-1;
	while(strchr(" \t\r\n", t2[0]) && t2>t) t2--;
	t2[1]=0;
	return t;
}

int rtrims(char * str)
//ÐÞ¸ÄÁËÔ­´®
{
	char * p;
	for (p = str + strlen(str) - 1; p >= str && *p <= ' ' && *p >= 0; p--);
	if (p + 1 >= str)
	{
		*(p + 1) = 0;
	}
	return p - str + 1;
}

#define trim(s) ltrim(rtrim(s))

char *get_new_shm(int key, int size) {
        int id;
        id=shmget(key, size, IPC_CREAT | IPC_EXCL | 0640);
        if(id<0) return 0;
        return shmat(id, NULL, 0);
}

char *get_old_shm(int key, int size) {
        int id;
        id=shmget(key, size, 0);
        if(id<0) return 0;
        return shmat(id, NULL, 0);
}

char *get_shm(int key, int size) {
        int id;
        id=shmget(key, size, IPC_CREAT | 0640);
        if(id<0) return 0;
        return shmat(id, NULL, 0);
}

#define setcookie(a, b)	printf("<script>document.cookie='%s=%s'</script>\n", a, b)
#define redirect(x)	printf("<meta http-equiv='Refresh' content='0; url=%s'>\n", x)
#define refreshto(x, t)	printf("<meta http-equiv='Refresh' content='%d; url=%s'>\n", t, x)
#define cgi_head()	printf("Content-type: text/html; charset=%s\n\n", CHARSET)


char *getsenv(char *s) {
        char *t=getenv(s);
        if(t) return t;
        return "";
}

int http_quit() {
	printf("\n</html>\n");
	exit(0);
}

int http_fatal(char *fmt, ...) {
        char buf[1024];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, 1023, fmt, ap);
        va_end(ap);
	buf[1023]=0;
 	printf("´íÎó! %s! <br><br>\n", buf);
	printf("<a href=javascript:history.go(-1)>¿ìËÙ·µ»Ø</a>");
	http_quit();
}

int strnncpy(char *s, int *l, char *s2) {
	strncpy(s+(*l), s2, strlen(s2));
	(*l)+=strlen(s2);
}

int hsprintf(char *s, char *fmt, ...) {
	char buf[1024], ansibuf[80], buf2[80];
	char *tmp;
	int c, bold, m, i, l, len;
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);
	s[0]=0;
	l=strlen(buf);
	len=0;
	bold=0;
	for(i=0; i<l; i++) {
		c=buf[i];
		if(c=='&') {
			strnncpy(s, &len, "&amp;");
		} else if(c=='<') {
			strnncpy(s, &len, "&lt;");
		} else if(c=='>') {
			strnncpy(s, &len, "&gt;");
		} else if(c=='\"') {//huangxu@060714
			strnncpy(s, &len, "&quot;");
		} else if(c==27) {
			if(buf[i+1]!='[') continue;
			for(m=i+2; m<l && m<i+24; m++)
				if(strchr("0123456789;", buf[m])==0) break;
			strsncpy(ansibuf, &buf[i+2], m-(i+2)+1);
			i=m;
			if(buf[i]!='m') continue;
			if(strlen(ansibuf)==0) {
				bold=0;
				strnncpy(s, &len, "<font class=c37>");
			}
			tmp=strtok(ansibuf, ";");
			while(tmp) {
				c=atoi(tmp);
				tmp=strtok(0, ";");
				if(c==0) {
					strnncpy(s, &len, "<font class=c37>");
					bold=0;
				}
				if(c>=30 && c<=37) {
					if(bold==1) sprintf(buf2, "<font class=d%d>", c);
					if(bold==0) sprintf(buf2, "<font class=c%d>", c);
					strnncpy(s, &len, buf2);
				}
			}
		} else {
			s[len]=c;
			len++;
		}
	}
	s[len]=0;
}

int hsprintf2(char *s, char *fmt, ...) {
	char buf[1024], ansibuf[80], buf2[80];
	char *tmp;
	int c, bold, m, i, l, len;
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);
	strcpy(s,"<span>");
	l=strlen(buf);
	len=strlen(s);
	bold=0;
	for(i=0; i<l; i++) {
		c=buf[i];
		if(c=='&') {
			strnncpy(s, &len, "&amp;");
		} else if(c=='<') {
			strnncpy(s, &len, "&lt;");
		} else if(c=='>') {
			strnncpy(s, &len, "&gt;");
		} else if(c=='\"') {//huangxu@060714
			strnncpy(s, &len, "&quot;");
		} else if(c==' ') {
			strnncpy(s, &len, "&nbsp;");
		} else if(c==27) {
			if(buf[i+1]!='[') continue;
			for(m=i+2; m<l && m<i+24; m++)
				if(strchr("0123456789;", buf[m])==0) break;
			strsncpy(ansibuf, &buf[i+2], m-(i+2)+1);
			i=m;
			if(buf[i]!='m') continue;
			if(strlen(ansibuf)==0) {
				bold=0;
				strnncpy(s, &len, "</span><span class=c37>");
			}
			tmp=strtok(ansibuf, ";");
			while(tmp) {
				c=atoi(tmp);
				tmp=strtok(0, ";");
				if(c==0) {
					strnncpy(s, &len, "</span><span class=c37>");
					bold=0;
				}
				if(c>=30 && c<=37) {
					if(bold==1) sprintf(buf2, "</span><span class=d%d>", c);
					if(bold==0) sprintf(buf2, "</span><span class=c%d>", c);
					strnncpy(s, &len, buf2);
				}
			}
		} else {
			s[len]=c;
			len++;
		}
	}
	s[len]=0;
	strcat(s,"</span>");
}

int hprintf(char *fmt, ...) {
	char buf[8096], buf2[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf2, 1023, fmt, ap);
	va_end(ap);
	hsprintf(buf, "%s", buf2);
	printf("%s", buf);
}

int strStartWith(char* src,char* cmp)
{
	return (strcasestr(src,cmp)==src)?1:0;
}

static char HOSTLINK[1024];
char * hostlink(char *link)//huangxu@060511:¶àIPÁ´½Ó
{
	char currHOST[40];
	char buf[80];
	strncpy(buf,getparm("currhost"),30);
	if(!*buf||!strcmp(buf,"127.0.0.1"))strcpy(buf,BBSHOST);
	sprintf(currHOST,"http://%s/",buf);
	//http://nkbbs.org/123456
	//01234567891123456789212
	char *HOSTS[]={BBSHOST/*,"202.113.16.117","bbs.nankai.edu.cn","cnc.nkbbs.org","wbbs.nankai.edu.cn"*/};
	int cntHOSTS=1;
	int i;
	for (i=0;i<cntHOSTS;i++)
	{
		sprintf(buf,"http://%s/",HOSTS[i]);
		if(strStartWith(link,buf))
		{
			sprintf(HOSTLINK,"%s",link+strlen(buf)-1);
			return HOSTLINK;
		}
	}
	strcpy(HOSTLINK,link);
	return HOSTLINK;
}

/* Ê¶±ðÁ´½Ó,ÇëÓÃÀà¿Õ¸ñ×Ö·û½«Æä¸ô¿ª*/
 int hhprintf(char *fmt, ...) {
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
	//huangxu@060511:¶àIP
	s=buf;
	my_link_mode=atoi(getparm("my_link_mode"));
        
	char *h;
        h=getparm("highspeed");
        if(!strcmp(h,""))
                highspeed=1;
        else
        	highspeed=atoi(h);  
        	
	my_localpic_mode=atoi(getparm("my_localpic_mode"));
	if(my_link_mode==1)  return hprintf("%s", buf);

        
	while(s[0]) {
		if(!strncasecmp(s, "http://", 7) || !strncasecmp(s, "mailto:", 7) || !strncasecmp(s, "ftp://", 6)) {
			char *href;
			if(len>0) {
				buf0[len]=0;
				hprintf("%s", buf0);
				len=0;
			}
			href=strtok(s, "\'\" \r\t)(,;\n");
			if(href==0) break;

                        char *link=urlencode(href);                

            
      //huangxu@060331
      hostlink(link);
      if (strcasestr(href, ".mp3")||strcasestr(href, ".wma")||strcasestr(href, ".mid")||strcasestr(href, ".wav"))//Music type
         {printf("<a href=\"/cgi-bin/bbs/dl?%s\">%s</a>",HOSTLINK,href);
					href=strtok(0, "");
					if(href==0) return printf("\n");
					return hhprintf(href);
			}

			if(strcasestr(href, ".gif") 
                           ||strcasestr(href, ".png")
                           ||strcasestr(href, ".jpg") 
                           ||strcasestr(href, ".jpe")){
                       
                           if((my_localpic_mode==0)
 			 	 &&!strstr(href,BBSHOST)
				 &&!strstr(href,"bbs.nankai.edu.cn")
				 &&!strstr(href,BBSIP)) 
				 {
				 printf("<a href=%s>%s</a>",HOSTLINK,href);
				}
                             else
                                if(highspeed) printf("<br><a href='%s' target=_blank><IMG SRC='%s'  border=0, alt='µã»÷¿´´óÍ¼'  onload=\"javascript:if(this.width>screen.width-333)this.width=screen.width-333\"><br></a>",HOSTLINK, HOSTLINK);
                                 else     printf("<br><a href='%s'>%s</a>",HOSTLINK,href);

					href=strtok(0, "");
					if(href==0) return;
					return hhprintf(href);
			}

           if(strcasestr(href, ".swf")){                            
                 printf("<br>Flash&nbsp µã»÷ÐÀÉÍ&nbsp<a href=bbsflash?id=%s target=_blank> %s </a> <br>",HOSTLINK,nohtml(href));
                                        href=strtok(0, "");
                                        if(href==0) return;
                                        return hhprintf(href);
            }
			  
		
            printf("<a target=_blank href='%s'>%s</a>", HOSTLINK, nohtml(href));
			href=strtok(0, "");
			if(href==0) return printf("\n");
			return hhprintf(href);
		} else {
			buf0[len]=s[0];
			if(len<1000) len++;
			s++;
		}

	}
          buf0[len]=0;
          hprintf("%s", buf0);



}

char lastconv[1024];

int code_convert(char * from_charset, char * to_charset, char * inbuf, int inlen, char *outbuf, int outlen)
{   
	iconv_t cd;
	int rc;   
	char **pin = &inbuf; 
	char **pout = &outbuf;
	cd = iconv_open(to_charset,from_charset);
	if (cd==0)
	{
		return -1;
	}
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
	{
		return -1;
	}
	iconv_close(cd);
	return 0;
}

#define STR_ENCODING_COOKIES "paraencoding"
#define STR_ENCODING_NAME "ziteng"
#define STR_ENCODING_TEST "×ÏÌÙ»¨¿ª"
#define INT_ENCCONV_SIZE 4096
//4KµÄ»º³åÇø£¨¶¯Ì¬£©
char parm_name[256][80], *parm_val[256];
int parm_num=0;

int paraencoding()
//Ê¹ÓÃÈ«¾Ö±äÁ¿´¦Àí×Ö·û±àÂë
{
	int i, l, r;
	int ebuflen;
	char buf[256];
	char * ebuf;
	ebuflen = INT_ENCCONV_SIZE;
	strncpy(buf, getparm(STR_ENCODING_COOKIES), 10);
	if (strcasecmp(buf, "utf8") && strcasecmp(buf, "utf-8"))
	{
		return 0;//ÎÞÐè×ª»»¡£´ó²¿·ÖÇé¿öÏÂÊÇÕâÑùµÄ¡£
	}
	for (i = 0; i < parm_num; i++)
	//»ù±¾ÉÏÀ´Ëµ£¬°´ÕÕgb2312:utf8 = 2:3µÄ±ÈÀýÀ´¡£ÕâÀïÎªÁË°²È«£¬È¡1:2¡£
	{
		l = strlen(parm_val[i]);
		ebuflen = l * 2;
		//ÖØÐÂ·ÖÅä¿©£º£©
		ebuf = malloc(ebuflen);
		if (!ebuf)
		{
			http_fatal("memory overflow -_-bb");
		}
		r = code_convert("utf8", CHARSET, parm_val[i], l, ebuf, ebuflen);
		if (r >= 0)
		//³É¹¦£¬Ìæ»»µô°É
		{
			//ÕâÀï¿ªÊ¼ÎªÁË½ÚÔ¼ÄÚ´æ£¡ÓÐ±ØÒªÃ´£¿
			l = strlen(ebuf);
			if (l < ebuflen)
			//¼¸ºõÊÇ±ØÈ»µÄ
			{
				ebuf = realloc(ebuf, l + 1);
				if (!ebuf)
				{
					http_fatal("memory overflow -_-bb");
				}
			}
			free(parm_val[i]);
			parm_val[i] = ebuf;
		}
		else
		//Ê§°Ü£¬Ïú»Ùebuf
		{
			free(ebuf);
		}
	}
	//ok ²»ÓÃÊÍ·Åebuf
	return i;
}

int parm_add(char *name, char *val) {
	int len=strlen(val);
	if(parm_num>=255) http_fatal("too many parms.");
	parm_val[parm_num]=calloc(len+1, 1);
	if(parm_val[parm_num]==0) http_fatal("memory overflow2 %d %d", len, parm_num);
	strsncpy(parm_name[parm_num], name, 78);
	strsncpy(parm_val[parm_num], val, len+1);
	parm_num++;
}

int http_init() {
	char *buf, buf2[1024], *t2, *t3;
	int n;
#ifndef SILENCE
	printf("Content-type: text/html; charset=%s\n\n\n", CHARSET);
	printf("<html>\n");
	printf("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\">\n", CHARSET);
#endif
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
	strsncpy(fromhost, getsenv(SENVFROM), 32);
	//strsncpy(fromhost, getsenv("HTTP_X_FORWARDED_FOR"), 32);
}

int __to16(char c) {
	if(c>='a'&&c<='f') return c-'a'+10;
	if(c>='A'&&c<='F') return c-'A'+10;
	if(c>='0'&&c<='9') return c-'0';
	return 0;
}

inline char myto16(char c)//huangxu@060405:+inline
{
        if(c>9&&c<16)
                return c=c+'A'-10;
        if(c>=0&&c<=9)
                return c=c+'0';
        return 0;
}


int __unhcode(char *s) {
	int m, n;
	for(m=0, n=0; s[m]!=0; m++, n++) {
		if(s[m]=='+') {
			s[n]=' ';
			continue;
		}
		if(s[m]=='%') {
			s[n]=__to16(s[m+1])*16+__to16(s[m+2]);
			m+=2;
			continue;
		}
		s[n]=s[m];
	}
	s[n]=0;
}

char *urlencode(char *s){ 
   static char buf[1024];
   int i,j=0;
   char a,b;

   for( i=0;s[i];i++) {
          if(s[i]==' ') {
                  buf[j++]='+';
                  continue;
          }
          if(s[i]&0x80) {
                  a=(s[i]&0xf0)>>4;
                  b=s[i]&0x0f;
                  buf[j++]='%';
                  buf[j++]=myto16(a);
                  buf[j++]=myto16(b);
                  continue;
          } else{
                  buf[j++]=s[i];
         }
  }
 buf[j]=0;
 return buf;

}


//huangxu@060402:Éî²ãurl±àÂë
char *urlencode2(const char *s){ 
   static char buf[4096];
   int i,j=0;
   char a,b;

   for( i=0;s[i];i++) {
          if(s[i]==' ') {
                  buf[j++]='+';
                  continue;
          }
          if((s[i]>=0 && s[i]<' ') || s[i]&0x80||strchr("\x7E\x21\x40\x23\x24\x25\x5E\x26\x2A\x28\x29\x5F\x2B\x60\x2D\x3D\x5B\x5D\x7B\x7D\x5C\x5C\x7C\x3A\x3B\x5C\x27\x5C\x22\x2C\x2E\x2F\x3C\x3E\x3F\x20",s[i])) {
                  a=(s[i]&0xf0)>>4;
                  b=s[i]&0x0f;
                  buf[j++]='%';
                  buf[j++]=myto16(a);
                  buf[j++]=myto16(b);
                  continue;
          } else{
                  buf[j++]=s[i];
         }
  }
 buf[j]=0;
 return buf;

}

char *getparm(char *var) {
	int n;
	for(n=0; n<parm_num; n++) 
		if(!strcasecmp(parm_name[n], var)) return parm_val[n];
	return "";
}

int get_shmkey(char *s) {
        int n=0;
        while(shmkeys[n].key!=0) {
                if(!strcasecmp(shmkeys[n].key, s)) return shmkeys[n].value;
                n++;
        }
        return 0;
}

int shm_init() {
	shm_utmp= (struct UTMPFILE *) get_old_shm(UTMP_SHMKEY, sizeof(struct UTMPFILE));
	shm_bcache= (struct BCACHE *) get_old_shm(BCACHE_SHMKEY, sizeof(struct BCACHE));
	shm_ucache= (struct UCACHE *) get_old_shm(UCACHE_SHMKEY, sizeof(struct UCACHE));
	if(shm_utmp==0) http_fatal("shm_utmp error");
	if(shm_bcache==0) http_fatal("shm_bcache error");
	if(shm_ucache==0) http_fatal("shm_ucache error");
}

int user_init(struct userec *x, struct user_info **y) {
	struct userec *x2;
	char id[20], num[20];
	int i, uid, key;
	strsncpy(id, getparm("utmpuserid"), 13);
	strsncpy(num, getparm("utmpnum"), 12);
	key=atoi(getparm("utmpkey"));
	if(key==0) return 0;//bluetent
	i=atoi(num);
	if(i<0 || i>=MAXACTIVE) return 0;
	(*y)=&(shm_utmp->uinfo[i]);
	if(strncmp((*y)->from, fromhost, 24)) return 0;
	if((*y)->utmpkey != key) return 0;
	if((*y)->active==0) return 0;
	if((*y)->userid[0]==0) return 0;
	strcpy((*y)->board, "-");
	if(!strcasecmp((*y)->userid, "new"))return 0;
	x2=getuser((*y)->userid);
	if(x2==0) return 0;
	if(strcmp(x2->userid, id)) return 0;
	memcpy(x, x2, sizeof(*x));
	return 1;
}

int removeInvisibleChars(char * str, char x)
//È¥µô²»¿É¼ûµÄ×Ö·û£¬0x0-0x19£¨º¬£©£¬Ìæ»»Îªx
{
	char * p;
	for (p = str; *p; p++)
	{
		if (!(*p & 224))
		{
			*p = x;
		}
	}
	return p - str;
}

int post_mail_d(char *userid, char *s_title, char *file, char *id, char *nickname, char *ip, int sig,int candeny, char * status) 
{
	FILE *fp, *fp2;
	char buf3[256], dir[256];
	struct fileheader header;
	int t, i;
	char title[256];
	strcpy(title, s_title);
	removeInvisibleChars(title, ' ');

        /* add by yiyo for deny user's mail */
        if (deny_m_user(userid) && candeny ) return -3;
        /* end */

	if(strstr(userid, "@")) 
		return  post_imail(userid, title, file, id, nickname, ip, sig);
                 
	bzero(&header, sizeof(header));
	strcpy(header.owner, id);
	for(i=0; i<100; i++) {
		t=time(0)+i;
		sprintf(buf3, "mail/%c/%s/M.%d.A", toupper(userid[0]), userid, i+time(0));
		if(!file_exist(buf3)) break;
	}
	if(i>=99) return -1;
	sprintf(header.filename, "M.%d.A", t);
	strsncpy(header.title, title, 60);
	fp=fopen(buf3, "w");
	if(fp==0) return -2;
	fp2=fopen(file, "r");
	fprintf(fp, "¼ÄÐÅÈË: %s (%s)\n", id, nickname);
	fprintf(fp, "±ê  Ìâ: %s\n", title);
	fprintf(fp, "·¢ÐÅÕ¾: %s (%s)\n", BBSNAME, Ctime(time(0)));
	fprintf(fp, "À´  Ô´: %s\n\n", ip);
	if(fp2) {
		while(1) {
            bzero(buf3,256);
			if(fgets(buf3, 256, fp2)<=0) break;
			fprintf2(fp, buf3);
		}
		fclose(fp2);
	}
	fprintf(fp, "\n--\n");
	sig_append(fp, id, sig);
	fprintf(fp, "\n\n[1;%dm¡ù À´Ô´:¡¤%s %s ¡¤%s[FROM: %.20s][m\n", 31+rand()%7, BBSNAME,BBSHOST, status,
	#ifdef DEF_HIDE_IP_ENABLE
	(!(currentuser.userdefine & DEF_NOTHIDEIP)&&isdormip(ip))?"ÄÏ¿ªËÞÉáÄÚÍø":ip);
	#else
	ip);
	#endif
	fclose(fp);
    sprintf(dir, "mail/%c/%s/.DIR", toupper(userid[0]), userid);
    fp=fopen(dir, "a");
	if(fp==0) return -1;
        fwrite(&header, sizeof(header), 1, fp);
        fclose(fp);
		return t;
}

int post_mail(char *userid, char *title, char *file, char *id, char *nickname, char *ip, int sig,int candeny) {
	return post_mail_d(userid, title, file, id, nickname, ip, sig, candeny, "Web");
}

int post_imail_d(char *userid, char *title, char *file, char *id, char *nickname, char *ip, int sig, char * status) {
        FILE *fp1, *fp2;
        char buf[256];
	if(strstr(userid, ";") || strstr(userid, "`") || strstr(userid, "/")) http_fatal("´íÎóµÄÊÕÐÅÈËµØÖ·");
	sprintf(buf, "/usr/sbin/sendmail -f %s@%s '%s'", id, BBSHOST, userid);
        fp2=popen(buf, "w");
        fp1=fopen(file, "r");
        if(fp1==0 || fp2==0) return -1;
	if(strlen (nickname)>0)
        fprintf(fp2, "From: %s <%s@%s>\n",nickname, id, BBSHOST);
	else
	fprintf(fp2,"From: %s@%s\n",id,BBSHOST);
        fprintf(fp2, "To: %s\n", userid);
        fprintf(fp2, "Subject: %s\n\n", title);
        while(1) {
                bzero(buf,256);
		if(fgets(buf, 255, fp1)==0) break;
		if(buf[0]=='.' && buf[1]=='\n') continue;
                fprintf(fp2, "%s", buf);
        }
	fprintf(fp2, "\n--\n");
 	sig_append(fp2, id, sig);
 	fprintf(fp2, "\n\n[1;%dm¡ù À´Ô´:¡¤ %s %s ¡¤%s[FROM: %.20s][m\n", 31+rand()%7, BBSNAME,BBSHOST, status,
#ifdef DEF_HIDE_IP_ENABLE
(!(currentuser.userdefine & DEF_NOTHIDEIP)&&isdormip(ip))?"ÄÏ¿ªËÞÉáÄÚÍø":ip);
#else
	ip);
#endif
        fprintf(fp2, "¡¤\n");
        fclose(fp1);
        pclose(fp2);
        return time(0);
}

int post_imail(char *userid, char *title, char *file, char *id, char *nickname, char *ip, int sig) {
	return post_imail_d(userid, title, file, id, nickname, ip, sig, "Web");
}

int post_article_d(char *board, char *s_title, char *file, char *id, char *nickname, char *ip, int sig, char * status) {
	FILE *fp, *fp2;
	char buf3[1024];
	struct fileheader header;
	int t, i;
	char title[256];
	strcpy(title, s_title);
	removeInvisibleChars(title, ' ');
	bzero(&header, sizeof(header));
	strcpy(header.owner, id);
	for(i=0; i<100; i++) {
		t=time(0)+i;
		sprintf(buf3, "boards/%s/M.%d.A", board, t);
		if(!file_exist(buf3)) break;
	}
	if(i>=99) return -1;
	sprintf(header.filename, "M.%d.A", t);
	strsncpy(header.title, title, 60);
	fp=fopen(buf3, "w");
	fp2=fopen(file, "r");
	getdatestring(time(0), NA);
	fprintf(fp, "·¢ÐÅÈË: %s (%s), ÐÅÇø: %s\n±ê  Ìâ: %s\n·¢ÐÅÕ¾: %s (%s)\n\n",
		id, nickname, board, title, BBSNAME, datestring);
	if(fp2!=0) {
		while(1) {
			if(fgets(buf3, 1000, fp2)<=0) break;
			fprintf2(fp, buf3);
		}
		fclose(fp2);
	}
	fprintf(fp, "\n--\n");
	sig_append(fp, id, sig);
	fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s ¡¤%s[FROM: %.20s][m\n", 31+rand()%7, BBSNAME,BBSHOST , status, 
#ifdef DEF_HIDE_IP_ENABLE
 (!(currentuser.userdefine & DEF_NOTHIDEIP)&&isdormip(ip))?"ÄÏ¿ªËÞÉáÄÚÍø":ip);
#else
	ip);
#endif
	fclose(fp);
	sprintf(buf3, "boards/%s/.DIR", board);
	fp=fopen(buf3, "a");
	fwrite(&header, sizeof(header), 1, fp);
	fclose(fp);
	return t;
}

int post_article(char *board, char *title, char *file, char *id, char *nickname, char *ip, int sig) {
	return post_article_d(board, title, file, id, nickname, ip, sig, "Web");
}

/* add by bluetent for posting articles with accessed */
int post_article2_d(char *board, char *s_title, char *file, char *id, char *nickname, char *ip, int sig, int accessed, int attach,int outgo, const char * status) {
        int temp=0;
        if(!strncmp(board,"TEMP-",5)){
              board=board+5;
              temp=1;
        }
	FILE *fp, *fp2;
	char buf3[1024];
	struct fileheader header;
	int t, i;
	char title[256];
	strcpy(title, s_title);
	removeInvisibleChars(title, ' ');
	bzero(&header, sizeof(header));
	strcpy(header.owner, id);
	for(i=0; i<100; i++) {
		t=time(0)+i;
		if(temp==0)
		    sprintf(buf3, "boards/%s/M.%d.A", board, t);
		else
		    sprintf(buf3, "boards/TEMP/M.%d.A",  t);
		if(!file_exist(buf3)) break;
	}
	if(i>=99) return -1;
	sprintf(header.filename, "M.%d.A", t);
	header.accessed[0]=accessed;
	if(attach)
	{
		header.filename[STRLEN-3]='U';
		header.filename[STRLEN-4]='U';
	}
	strsncpy(header.title, title, 60);
	fp=fopen(buf3, "w");
	fp2=fopen(file, "r");
	getdatestring(time(0), NA);
	fprintf(fp, "·¢ÐÅÈË: %s (%s), ÐÅÇø: %s\n±ê  Ìâ: %s\n·¢ÐÅÕ¾: %s (%s)\n\n",
		id, nickname, board, title, BBSNAME, datestring);
	if(fp2!=0) {
		while(1) {
			if(fgets(buf3, 1000, fp2)<=0) break;
			fprintf2(fp, buf3);
		}
		fclose(fp2);
	}
	fprintf(fp, "\n--\n");
	sig_append(fp, id, sig);
	fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s ¡¤%s[FROM: %.20s][m\n", 31+rand()%7, BBSNAME, BBSHOST, status,
#ifdef DEF_HIDE_IP_ENABLE
(!(currentuser.userdefine & DEF_NOTHIDEIP)&&isdormip(ip))?"ÄÏ¿ªËÞÉáÄÚÍø":ip);
#else
	ip);
#endif
	fclose(fp);

        if (!outgo) {    

             header.filename[STRLEN - 1] = 'L';            
             header.filename[STRLEN - 2] = 'L';            
        } else {                                                
             header.filename[STRLEN - 1] = 'S';            
             header.filename[STRLEN - 2] = 'S';            
             outgo_post(&header, board);               
        }                                                       

if(temp==1)
sprintf(buf3, "boards/TEMP/.DIR");
else
	sprintf(buf3, "boards/%s/.DIR", board);
	fp=fopen(buf3, "a");
        fwrite(&header, sizeof(header), 1, fp);
	fclose(fp);
	return t;
}

int post_article2(char *board, char *title, char *file, char *id, char *nickname, char *ip, int sig, int accessed, int attach,int outgo)
{
	return post_article2_d(board, title, file, id, nickname, ip, sig, accessed, attach, outgo, "Web");
}

int sig_append(FILE *fp, char *id, int sig) {
	FILE *fp2;
	char path[256];
	char buf[100][256];
	int i, total;
	struct userec *x;
	if(sig<0 || sig>10) return;
	x=getuser(id);
	if(x==0) return;
	sprintf(path, "home/%c/%s/signatures", toupper(id[0]), id);
	fp2=fopen(path, "r");
	if(fp2==0) return;
	for(total=0; total<100; total++)
		if(fgets(buf[total], 255, fp2)==0) break;
	fclose(fp2);
	for(i=sig*6; i<sig*6+6; i++) {
		if(i>=total) break;
		fprintf(fp, "%s", buf[i]);
	}
}

char* anno_path_of(char *board) {
	FILE *fp;
	static char buf[256], buf1[80], buf2[80];
	fp=fopen("0Announce/.Search", "r");
	if(fp==0) return "";
	while(1) {
		if(fscanf(fp, "%s %s", buf1, buf2)<=0) break;
		buf1[strlen(buf1)-1]=0;
		if(!strcasecmp(buf1, board)) {
			sprintf(buf, "/%s", buf2);
			return buf;
		}
	}
	fclose(fp);
	return "";
}

int has_BM_perm(struct userec *user, char *board) {
	struct shortfile *x;
	char buf[256], *bm;
	x=getbcache(board);
	if(x==0) return 0;
	if(user_perm(user, PERM_BLEVELS)) return 1;
	if(!user_perm(user, PERM_BOARDS)) return 0;
	strcpy(buf, x->BM);
	if(check_ZM_perm(user,board))
	{
		return 1;
	}
	bm=strtok(buf, ",: ;&()\n");
	while(bm) {
		if(!strcasecmp(bm, user->userid)) return 1;
		bm=strtok(0, ",: ;&()\n");
	}
	return 0;
}
int check_ZM_perm(struct userec *temp_user,char *temp_board)
{
        struct shortfile *b_children;
        struct shortfile *b_father;
        char temp_BM[BM_LEN - 1];
        char *ptr;
        b_children = getbcache(temp_board);
	if(b_children==0) 
	{
		return 0;
	}
        if(b_children->flag & CHILDBOARD_FLAG)
        {
                b_father=getbcache(b_children->owner);
                memcpy(temp_BM,b_father->BM,BM_LEN - 1);
                ptr = strtok(temp_BM, ",: ;|&()\0\n");
                while(1)
                {
                        if (ptr == NULL)
                        {
                                return 0;
                        }
                        if (!strcasecmp(ptr,temp_user->userid))
                        {
                                return 1;
                        }
                        ptr = strtok(NULL, ",: ;|&()\0\n");
                }
        }
        return 0;
}

int has_read_perm(struct userec *user, char *board) {  
	struct shortfile *x;	/* °åÃæ²»´æÔÚ·µ»Ø0, pºÍz°åÃæ·µ»Ø1, ÓÐÈ¨ÏÞ°åÃæ·µ »Ø1. */
	if(board[0]<=32) return 0;
	x=getbcache(board);
	if(x==0) return 0;
	//huangxu@060406:SÈ¨ÏÞÊÇÍòÄÜµÎ¡­¡­
	if((user->userlevel & PERM_SYSOP) && 
		(!file_has_word("/home/bbs/etc/deny_s_board", board) || 
			teamofboard (u_info->userid, board))) return 1;

        if(!teamofboard (u_info->userid, board) ){
               // printf("¸Ã°æÎªÍÅÌå°åÃæ£¬¶øÄú²»ÊôÓÚ¸ÃÍÅÌå");
                return 0;
        }

	if(x->level==0) return 1;
	if(x->level & (PERM_POSTMASK | PERM_NOZAP)) return 1;
	if(!user_perm(user, PERM_BASIC)) return 0;
	if(user_perm(user, PERM_RECLUSION)) return 0;
	if(user_perm(user, x->level)) return 1;
	return 0;
}

//huangxu@060407:Web×Ô¶¯½â·â
int freeuser(struct userec *user,char* board)
{
	FILE *fp;
	char * pbuf;
	char * wbuf;
	char buf[256];
	int uln;
	int fln;
	char useridbuf[21];
	sprintf(useridbuf,"%s ",user->userid);
	uln=strlen(useridbuf);
	sprintf(buf,"boards/%s/deny_users",board);
	if (!(fp=fopen(buf,"r")))
	{
		printf("´ò¿ªÎÄ¼þ´íÎó.\n");
		return 1;//error
	}
	fseek(fp,0,2);
	fln=ftell(fp);
	fseek(fp,0,0);
	if (!(pbuf=malloc(fln)))
	{
		printf("¶ÁÎÄ¼þ´íÎó.\n");
		fclose(fp);
		return 1;
	}
	if(!fread(pbuf,1,fln,fp))
	{
		printf("ÔØÈëÎÄ¼þÊ±³ö´í.\n");
		fclose(fp);
		return 1;
	}
	fclose(fp);
	if (!(fp=fopen(buf,"w")))
	{
		printf("Ð´ÎÄ¼þ´íÎó.\n");
		return 1;
	}
	for(wbuf=pbuf;wbuf<pbuf+fln;wbuf+=73)
	{
		if (!strncasecmp(useridbuf,wbuf,uln))
			continue;
		if(fwrite(wbuf,1,73,fp)<73)
		{
			printf("Ð´ÎÄ¼þ³ö´í.\n");
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	free(pbuf);
	inform(board,user->userid);
	return 0;
}

int inform(char *board, char *user)
{
        int info_type=1;
        char msgbuf[200];
        FILE *fp;
        char path[80], title[80];
        sprintf(title, "»Ö¸´%sÔÚ%s°åµÄ·¢ÎÄÈ¨ÏÞ", user, board);
        sprintf(path, "tmp/%d.tmp", getpid());
        fp=fopen(path, "w");
        sprintf(msgbuf, "\n  %s ÍøÓÑ£º\n\n\tÒò·â½ûÊ±¼äÒÑ¹ý£¬ÏÖ»Ö¸´ÄúÔÚ [%s] °åµÄ¡º%s¡»È¨Á¦¡£\n\n",
                user, board,info_type==1?"·¢±íÎÄÕÂ":"½øÈë±¾°å");
        if (!strcasecmp(currentuser.userid,user))
		sprintf(msgbuf,"\tÏ£ÍûÄú½ñºóÄÜ×Ô¾õ×ñÊØÕ¾¹æ,%s¸ÐÐ»ÄúµÄÖ§³ÖÓëºÏ×÷.",BBSNAME);
        fprintf(fp, msgbuf);
        fclose(fp);
        post_article(board, title, path, BBSID, "×Ô¶¯·¢ÐÅÏµÍ³", "×Ô¶¯·¢ÐÅÏµÍ³", -1);
        post_mail(user, title, path, currentuser.userid, currentuser.username, "×Ô¶¯·¢ÐÅÏµÍ³", -1, 0);
        sprintf(path, "tmp/%d.tmp", getpid());
        fp=fopen(path, "w");
        fprintf(fp, msgbuf);
        fclose(fp);
        post_article("Punishment", title, path, currentuser.userid, currentuser.username, "×Ô¶¯·¢ÐÅÏµÍ³", -1);
        unlink(path);
        fp=fopen(path, "w");
        getuinfo(fp);
        fclose(fp);
        post_article("syssecurity3", title, path, currentuser.userid, currentuser.username, "×Ô¶¯·¢ÐÅÏµÍ³", -1);
        unlink(path);
        printf("ÒÑ¾­½â³ý%sÔÚ%s°åµÄÏÞÖÆ.<br>\n", user,board);
}

int boardpostdenied(struct userec *user,char* board)
{
	FILE *fp;
	char buf[256];
	int i;
	int tmp;
	int uln;
	char useridbuf[21];
	time_t now;
	sprintf(useridbuf,"%s ",user->userid);
	uln=strlen(useridbuf);
	//deny_user,73B/rec
	//id           no                                       2006Äê04ÔÂ08ÈÕ½â·â\x0a
	//0123456789112345678921234567893123456789412345678951234567896123456789712
	sprintf(buf,"boards/%s/deny_users",board);
	if (!(fp=fopen(buf,"r")))
		return 0;
	time(&now);
	getdatestring(now,NA);
	for(;;)
	{
		if (tmp=fread(buf,1,73,fp)<73)
			break;
		if (!strncasecmp(useridbuf,buf,uln))
		{
			if(strncmp(buf+54,datestring,14)<=0)
			{
				fclose(fp);
				return freeuser(user,board);
			}
			else;
				return 1;
		}
	}
	fclose(fp);
	return 0;
}

int has_post_perm(struct userec *user, char *board) {
	FILE *fp;
	char buf3[256];
	struct shortfile *x;
	if(!has_read_perm(user, board)) return 0;
	x=getbcache(board);
	if(x==0) return 0;
	if(!loginok) return 0;
	if(user_perm(user,PERM_SYSOP)) return 1;//huangxu@060407:SÈ¨ÏÞÊÇÍòÄÜµÄ!
	//È«Õ¾Ö»¶Á
	if(file_exist("NOPOST")&&!user_perm(user,PERM_CHATCLOAK))return 0;

        //sprintf(buf3, "boards/%s/deny_users", x->filename);
        //huangxu@060407:Web×Ô¶¯½â·â
        if(boardpostdenied(user,x->filename)) return 0;
        //if(file_has_word(buf3, user->userid)) return 0;        
        if(!teamofboard (u_info->userid, board) ){
                printf("¸Ã°æÎªÍÅÌå°åÃæ£¬¶øÄú²»ÊôÓÚ¸ÃÍÅÌå");
                return 0;
        }
	if (dashf ("NOPOST.2") && ! HAS_PERM (PERM_OBOARDS) && 	( !strcmp (board, "VoiceofNankai") ||
		!strcmp (board, "TaiWan") || !strcmp (board, "Military") || !strcmp (board, "Salon")))
	return 0;
	if(!teamofwrite (u_info->userid, board) )
	{
                printf("¸Ã°æÎª¾ãÀÖ²¿°åÃæ£¬¶øÄú²»ÊôÓÚÕâ¸ö¾ãÀÖ²¿.Èç¹ûÏë·¢ÎÄÇëÓë°åÎñÁªÏµ");
                return 0;
        }
	if (! canenterboard (u_info->userid, board)) {
		printf ("ÄúÒÑ¾­±»½ûÖ¹ÔÚ±¾°å·¢ÎÄµÄÈ¨ÏÞ");
		return 0;
	}
	if(!strcmp(board, "sysop")) return 1;
        if(user_perm(user, PERM_RECLUSION)) return 0;
	if(!user_perm(user, PERM_BASIC)) return 0;
	if(!user_perm(user, PERM_POST)) return 0;
	if(!(x->level & PERM_NOZAP) && x->level && !user_perm(user, (x->level & ~PERM_POSTMASK))) {
		return 0;
	}
	return 1;
}

struct shortfile *getbcache(char *board) {
        int i;
	if(board[0]==0) return 0;
	for(i=0; i<MAXBOARD; i++){
   	        if(!strcasecmp(board, shm_bcache->bcache[i].filename)) return &shm_bcache->bcache[i];
        }
	return 0;
}

int count_mails(char *id, int *total, int *unread) {
        struct fileheader x1;
	char buf[256];
        int n;
        FILE *fp;
	*total=0;
	*unread=0;
        if(getuser(id)==0) return 0;
        sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME, toupper(id[0]), id);
        fp=fopen(buf, "r");
        if(fp==0) return;
        *total=file_size(buf)/sizeof(x1);
        int i=0;
        if(*total>20)
            i=*total-20;
        fseek(fp, sizeof(x1)*i, SEEK_SET);
        while(fread(&x1, sizeof(x1), 1, fp)>0) {
               
                if(!(x1.accessed[0] & FILE_READ)) (*unread)++;
        }
        fclose(fp);
}

int findnextutmp(char *id, int from) {
	int i;
	if(from<0) from=0;
	for(i=from; i<MAXACTIVE; i++) 
		if(shm_utmp->uinfo[i].active)
			if(!strcasecmp(shm_utmp->uinfo[i].userid, id)) return i;
	return -1;
}

int sethomefile(char *buf, char *id, char *file) {
	sprintf(buf, "home/%c/%s/%s", toupper(id[0]), id, file);
}

int send_msg(char *myuserid, int mypid, char *touserid, int topid, char msg[256]) {
	char msgbuf[256], buf3[256];
	int i;
	for(i=0; i<strlen(msg); i++)
		if((0<msg[i] && msg[i]<=27 )|| msg[i]==-1) msg[i]=32;
	if(mypid<=0) return -1;
	sprintf(msgbuf, "[0;1;44;36m%-12.12s[33m([36m%-5.5s[33m):[37m%-54.54s[31m(^Z»Ø)[m[%05dm\n",
 		myuserid, Ctime(time(0)) + 11, msg, mypid);
	sethomefile(buf3, touserid, "msgfile");
	f_append(buf3, msgbuf);
	sethomefile(buf3, touserid, "msgfile.me");
	f_append(buf3, msgbuf);
	sethomefile(buf3, myuserid, "msgfile.me");
	sprintf(msgbuf, "[1;32;40mTo [1;33;40m%-12.12s[m (%-5.5s):%-55.55s\n",
		touserid, Ctime(time(0)) + 11, msg);
	f_append(buf3, msgbuf);
	if(topid<=0) return -1;
	kill(topid, SIGTTOU);
	kill(topid, SIGUSR2);
	return 0;
}

char *horoscope(int month, int day) {
        int date=month*100+day;
        if(month<1 || month>12 || day<1 || day>31) return "²»Ïê";
        if(date<121 || date>=1222) return "Ä¦ôÉ";
        if(date<219) return "Ë®Æ¿";
        if(date<321) return "Ë«Óã";
        if(date<421) return "ÄµÑò";
        if(date<521) return "½ðÅ£";
        if(date<622) return "Ë«×Ó";
        if(date<723) return "¾ÞÐ·";
        if(date<823) return "Ê¨×Ó";
        if(date<923) return "´¦Å®";
        if(date<1024) return "Ìì³Ó";
        if(date<1123) return "ÌìÐ«";
        if(date<1222) return "ÉäÊÖ";
}

char *ModeType(int mode) {
	switch (mode) {
	case IDLE:
		return "";
	case NEW:
		return "ÐÂÕ¾ÓÑ×¢²á";
	case LOGIN:
		return "½øÈë±¾Õ¾";
	case DIGEST:
		return "ä¯ÀÀ¾«»ªÇø";
	case MMENU:
		return "×ª»»ÕóµØÖÐ";
	case ADMIN:
		return "¹ÜÀíÕßÑ¡µ¥";
	case SELECT:
		return "Ñ¡ÔñÌÖÂÛÇø";
	case READBRD:
		return "Ò»ÀÀÖÚÉ½Ð¡";
	case READNEW:
		return "ÔÄÀÀÐÂÎÄÕÂ";
	case READING:
		return "Æ·Î¶ÎÄÕÂ";
	case POSTING:
		return "ÎÄºÀ»Ó±Ê";
	case MAIL:
		return "´¦ÀíÐÅ¼ã";
	case SMAIL:
		return "¼ÄÓïÐÅ¸ë";
	case RMAIL:
		return "ÔÄÀÀÐÅ¼ã";
	case TMENU:
		return "ÁÄÌìÑ¡µ¥";
	case LUSERS:
		return "»·¹ËËÄ·½";
	case FRIEND:
		return "Ñ°ÕÒºÃÓÑ";
	case MONITOR:
		return "Ì½ÊÓÃñÇé";
	case QUERY:
		return "²éÑ¯ÍøÓÑ";
	case TALK:
		return "ÈµÇÅÏ¸Óï";
	case PAGE:
		return "ºô½ÐÖÐ...";
	case CHAT1:
		return "¹ú¼Ê»áÒéÌü";
	case CHAT2:
		return "¹«¹²ÁÄÌìÊÒ";
	case IRCCHAT:
		return "»áÌ¸IRC";
	case LAUSERS:
		return "Ì½ÊÓÍøÓÑ";
	case XMENU:
		return "ÏµÍ³×ÊÑ¶";
	case VOTING:
		return "Í¶Æ±ÖÐ...";
	case BBSNET:
		return "BBSNET";
	case EDITWELC:
		return "±à¼­Welc";
	case EDITUFILE:
		return "±à¼­¸öÈËµµ";
	case EDITSFILE:
		return "±àÐÞÏµÍ³µµ";
	case ZAP:
		return "¶©ÔÄÌÖÂÛÇø";
	case GAME:
		return "ÄÔÁ¦¼¤ÌÀ";
	case SYSINFO:
		return "¼ì²éÏµÍ³";
	case FTPSEARCH:
		return "FTPËÑË÷";
	case DICT:
		return "·­²é×Öµä";
	case LOCKSCREEN:
		return "Ó©Ä»Ëø¶¨";
	case TYPEN:
		return "·¢¹«¸æÖÐ";
	case NOTEPAD:
		return "ÁôÑÔ°å";
	case GMENU:
		return "¹¤¾ßÏä";
	case MSG:
		return "ËÍÑ¶Ï¢";
	case USERDEF:
		return "×Ô¶©²ÎÊý";
	case EDIT:
		return "ÐÞ¸ÄÎÄÕÂ";
	case OFFLINE:
		return "×ÔÉ±ÖÐ..";
	case EDITANN:
		return "±àÐÞ¾«»ª";
	case HYTELNET:
		return "Hytelnet";
	case CCUGOPHER:
		return "ËûÕ¾¾«»ª";
	case LOOKMSGS:
		return "²ì¿´Ñ¶Ï¢";
	case WFRIEND:
		return "Ñ°ÈËÃû²á";
	case WNOTEPAD:
		return "Óû×ß»¹Áô";
	case BBSPAGER:
		return "ÍøÂ·´«ºô";
	case 10001:
		return "Web ä¯ÀÀ";
	case ADDRESSBOOK:
		return "²ì¿´Í¨Ñ¶Â¼";
	case M_BLACKJACK:
		return "¡ïºÚ¼×¿Ë¡ï";
	case M_XAXB:
		return "¡ï²ÂÊý×Ö¡ï";
	case M_DICE:
		return "¡ïÎ÷°ËÀ­¡ï";
	case M_GP:
		return "½ðÆË¿ËËó¹þ";
	case M_NINE:
		return "ÌìµØ¾Å¾Å";
        case WINMINE:
                return "¼üÅÌÉ¨À×";
	case WINMINE2:
		return "¸ÐÓ¦É¨À×";
	case M_BINGO:
		return "±ö¹û±ö¹û";
        case FIVE:
		return "¾öÕ½Îå×ÓÆå";
	case MARKET:	
		return "½»Ò×ÊÐ³¡";
        case PAGE_FIVE:
		return "ÑûÇëÏÂÆå";
	case CHICK:
		return "µç×ÓÐ¡¼¦";
	case MARY:
		return "³¬¼¶ÂêÀö"; 
	case CHICKEN:
		return "ÐÇ¿ÕÕ½¶·¼¦";
	case GOODWISH:
		return "¸øÅóÓÑ×£¸£";
	case SAID:
		return "ÕÒÓÃ»§ÎÄÕÂ";
	case SAAR:
		return "±êÌâËÑË÷";
	case QUERYIP:
		return "²éÑ¯ÉÏÏßIP";
	case FREEIP:
		return "IPµØÖ·²éÑ¯";
	case TT:
		return "´ò×ÖÁ·Ï°";
	case TETRIS:
		return "¶íÂÞË¹·½¿é";
	case LUNAR:
		return "µç×ÓÍòÄêÀú";
	case FRIENDTEST:
		return "ÓÑÒê²âÊÔ";
	case M_2NDHAND:
		return "ÌøÔéÊÐ³¡";
	case YBQH:
		return "²éÓÊ±àÇøºÅ";
	case RECLUSION:
		return "¹éÒþÖÐ...";
	case WORKER:
		return "ÍÆÏä×Ó";
	case XFIVE:
		return "ÈË»úÎå×ÓÆå";
	case QMD:
	        return  "Éè¶¨Í¼Æ¬Ç©Ãûµµ";
//	case CALC:
//		return "ÃüÁî¼ÆËãÆ÷";
	default:
		return "È¥ÁËÄÄÀï!?";
        }
}

//char   *cexp(int usrexp) {//huangxu@060405:exp=>usrexp£¬warningºÃÐÄ·³
const char * expstr(struct userec *x)
{
	int usrexp = countexp(x);
	if(usrexp==-9999) 	return "Ã»µÈ¼¶";
        if(usrexp<=100)  	return "ÐÂÊÖÉÏÂ·";
        if(usrexp<=450)	return "Ò»°ãÕ¾ÓÑ";
        if(usrexp<=850)	return "ÖÐ¼¶Õ¾ÓÑ";
        if(usrexp<=1500)	return "¸ß¼¶Õ¾ÓÑ";
        if(usrexp<=2500)	return "ÀÏÕ¾ÓÑ";
        if(usrexp<=3000)	return "³¤ÀÏ¼¶";
        if(usrexp<=5000)	return "±¾Õ¾ÔªÀÏ";
        if(usrexp<=10000)	return "¿ª¹ú´óÀÏ";
        if(usrexp<=20000)	return "ÉñÏÉ?Ñý¹Ö?";
        return "ÈçÀ´?¹ÛÒô?";
}

#ifdef  ALLOW_MEDALS

char *
cmedal (num)
unsigned int num;
{
	//mod by brew..
		switch (num) {
	case 1: return GLY_NEW_MEDAL1;
	case 2: return GLY_NEW_MEDAL2;
	case 3: return GLY_NEW_MEDAL3;
	case 4: return GLY_NEW_MEDAL4;
	case 5: return GLY_NEW_MEDAL5;
	case 6: return GLY_NEW_MEDAL6;
	case 7: return GLY_NEW_MEDAL7;
//	case 8: return GLY_NEW_MEDAL8;
	}
	return GLY_NEW_MEDAL0;
/*
        if (num <= 0)
                return GLY_MEDAL0;
        if (num <= 1)
                return GLY_MEDAL1;
        if (num <= 2)
                return GLY_MEDAL2;
        if (num <= 3)
                return GLY_MEDAL3;
        if (num <= 5)
                return GLY_MEDAL4;
        if (num <= 7)
                return GLY_MEDAL5;
        if (num <= 9)
                return GLY_MEDAL6;
        if (num <= 100)
		return GLY_MEDAL7;
	if (num <= 200) //ÀëÈÎÕ¾Îñ
		return GLY_NEW_MEDAL1;
	if (num <= 300) //Ô­´´Ð´ÊÖ
		return GLY_NEW_MEDAL2;
	if (num <= 400) //ÓÅÐã°åÎñ
		return GLY_NEW_MEDAL3;
	if (num <= 500) //ÓÅÐã½¨Òé
		return GLY_NEW_MEDAL4;
	if (num <= 600) //×¥³æÄÜÊÖ
		return GLY_NEW_MEDAL5;
	if (num <= 700) //ÓÅÐãÍøÓÑ
		return GLY_NEW_MEDAL6;
	if (num <= 800) //ÌØÊâ¹±Ï×
		return GLY_NEW_MEDAL7;
	return "ÄÇÃ´¶à½±ÕÂ-_-b";
*/
}

#endif

//char   *cperf(int perf) {
const char * perfstr(struct userec * x)
{
	int perf = countperf(x);
	if(perf==-9999)	return "Ã»µÈ¼¶";
        if(perf<=5)	return "¸Ï¿ì¼ÓÓÍ";
        if(perf<=12)	return "Å¬Á¦ÖÐ";
        if(perf<=35)	return "»¹²»´í";
        if(perf<=50)	return "ºÜºÃ";
        if(perf<=90)	return "ÓÅµÈÉú";
        if(perf<=140)	return "Ì«ÓÅÐãÁË";
        if(perf<=200)	return "±¾Õ¾Ö§Öù";
	return "Éñ¡«¡«";
}

int count_life_value(struct userec *urec) {
	int i;
	i=(time(0) - urec->lastlogin)/60;
	if((urec->userlevel & PERM_XEMPT)||!strcasecmp(urec->userid, "guest"))
        	return 999;
	if(urec->numlogins<=1)
		return (15 * 1440 - i) / 1440;
	//	return 100;


	if(!(urec->userlevel & PERM_LOGINOK))
			return (30 * 1440 - i) / 1440;
		//return 100;


     int exp=countexp(urec);
     int posts=urec->numposts;
     int days=(time(0)-(urec->firstlogin))/86400;
     
     if(days>=365*3 && exp>1500 && posts>300)return 666;//Huangxu@060714:666life
     char genbuf[128];
     sprintf(genbuf, "home/%c/%s/life666", toupper(urec->userid[0]), urec->userid);
	   if(dashf(genbuf)) return 666;


    int life=200;
    if (urec->stay > 1000000){
        life=365;
        if (exp > 3000 &&posts>300 )     life=465;
        if (exp > 5000 && posts>800 )     life=565;
    }
	
	if((life * 1440 - i) / 1440 < 1)
		return 1;
	else
    	return (life * 1440 -i ) / 1440;

/*


	if (urec->stay > 1000000){
		if ((365 * 1440 -i) / 1440 < 100)
       			 return 100;
		else
			return (365 * 1440 - i) / 1440;
	}
	if ((200 * 1440 -i) / 1440 < 100)
        	return 100;
	else
		return (200 * 1440 - i) / 1440;
	*/
}

int countexp(struct userec *x) {
        int tmp;
	if(!strcasecmp(x->userid, "guest")) return -9999;
        tmp=x->numposts + x->numlogins/5 + (time(0)-x->firstlogin)/86400 + x->stay/3600;
	if(tmp<0) tmp=0;
        return tmp;
}

int countperf(struct userec *x) {
	int day, logins, posts;
        if(!strcasecmp(x->userid, "guest")) return -9999;
	day=(time(0) - x->firstlogin)/86400+1;
	logins=x->numlogins;
	posts=x->numposts;
	if(day<=0 || logins<=0 || posts<0) return 0;
        return (10*posts/logins+10*logins/day);
}

int show_special(char *id2)
{
        FILE *fp;
        char  id1[80], name[80];
        fp=fopen("etc/sysops", "r");
        if(fp==0)
                return;
        while(1) {
                id1[0]=0;
                name[0]=0;
                if(fscanf(fp, "%s %s", id1, name)<=0)
                        break;
                if(!strcmp(id1, id2))
                        hprintf("[1;37m[[0;35m%s[1;37m][m", name);
        }
        fclose(fp);
}

int
uleveltochar(buf, lvl)
char   *buf;
unsigned int lvl;
//ÇëºóÈÎÎ¬»¤Îñ±Ø×öµ½ÊµÊÂÇóÊÇ¡£
{
        if (!(lvl & PERM_BASIC)) {
                strcpy(buf, "--------- ");
                return 0;
        }
        buf[10]= '\0';
        buf[0] = (lvl & (PERM_CLOAK)   ) ? 'C':' ';//ÒþÉí
        buf[1] = (lvl & (PERM_SEECLOAK)) ? '#':' ';//¿´ÒþÉí
        buf[2] = (lvl & (PERM_BOARDS  )) ? 'B':' ';//°åÖ÷
        buf[3] = (lvl & (PERM_XEMPT   )) ? 'X':' ';//ÓÀ¾ÃÕÊºÅ
        buf[4] = (lvl & (PERM_CHATCLOAK))? 'M':' ';//ÖÇÄÒÍÅ
        buf[5] = (lvl & (PERM_ACCOUNTS)) ? 'A':' ';//ÕÊºÅ¹ÜÀí
        buf[6] = (lvl & (PERM_OBOARDS )) ? 'O':' ';//ÌÖÂÛÇø¹ÜÀí
        buf[7] = (lvl & (PERM_DENYPOST)) ? 'D':' ';//ÐÞ¸Ä»ù±¾È¨ÏÞ
        buf[8] = (lvl & (PERM_SYSOP   )) ? 'S':' ';//ÏµÍ³Î¬»¤
        buf[9] = (lvl & (PERM_POST    )) ? ' ':'p';//±»·â½û
        return 1;
}

//char *
//cnumposts(num)
//int num;
const char * numpostsstr(struct userec * x)
{
	int num = x->numposts;

        if(num== 0 )
                return GLY_CPOST0;
        if(num>0&&num<=500)
                return GLY_CPOST1;
        if(num>500&&num<=1500)
                return GLY_CPOST2;
        if(num>1500&&num<=4000)
                return GLY_CPOST3;
        if(num>4000&&num<=8000)
                return GLY_CPOST4;
        if(num>8000&&num<=15000)
                return GLY_CPOST5;
        if(num>15000)
                return GLY_CPOST6;
}

int modify_mode(struct user_info *x, int newmode) {
	if(x==0) return;
	if(newmode<20000) newmode+=20000;
	x->mode=newmode;
}

int save_user_data(struct userec *x) {
	FILE *fp;
	int n;
	//±¾Õ¾½¨Õ¾Ê±¼äÎª2008Äê11ÔÂ21ÈÕ21:50:02,ÆäÊ±¼ä´ÁÎª1227275402
	if(x->stay > MULTI_LOGINS * 2 * (time(0) - 1227275402) ||
		x->userid[0] == '\0')
		return 0;
	n=getusernum(x->userid);
	if(n<0 || n>1000000) return 0;
	fp=fopen(".PASSWDS", "r+");
	if(fp==0) return 0;
	fseek(fp, n*sizeof(struct userec), SEEK_SET);
	fwrite(x, sizeof(struct userec), 1, fp);
	fclose(fp);
	return 1;
}

int is_bansite(char *ip) {
	FILE *fp;
	char buf3[256];
	fp=fopen(".bansite", "r");
	if(fp==0) return 0;
	while(fscanf(fp, "%s", buf3)>0)
		if(!strcasecmp(buf3, ip)) return 1;
	fclose(fp);
	return 0;
}

int user_perm(struct userec *x, int level) {
	return (level? (x->userlevel & level) : 1);
}

int getusernum(char *id) {

	int i;
	if(id[0]==0) return -1;
	for(i=0; i<MAXUSERS; i++) {
		if(!strcasecmp(shm_ucache->userid[i], id)) return i;
	}
	return -1;
}

struct userec *getuser(char *id)
{
	static struct userec userec1;
	int uid;

	FILE *fp;
	uid=getusernum(id);
	if(uid<0) return 0;
	fp=fopen(".PASSWDS", "r");
	fseek(fp, uid*sizeof(userec1), SEEK_SET);
	bzero(&userec1, sizeof(userec1));
	fread(&userec1, sizeof(userec1), 1, fp);
	fclose(fp);	
	return &userec1;
}

int checkpasswd(char *pw_crypted, char *pw_try) {
	return !strcmp(crypt1(pw_try, pw_crypted), pw_crypted);
}

int checkuser(char *id, char *pw) {
	int i;
	struct userec *x;
	x=getuser(id);
	if(x==0) return 0;
	return checkpasswd(x->passwd, pw);
}

int count_id_num(char *id) {
	int i, total=0;
	for(i=0; i<MAXACTIVE; i++)
		if(shm_utmp->uinfo[i].active && !strcasecmp(shm_utmp->uinfo[i].userid, id)) total++;
	return total;
}

int count_online() {
	int i, total=0;
	for(i=0; i<MAXACTIVE; i++) 
		if(shm_utmp->uinfo[i].active) total++;
	return total;
}

int count_online2() {
	int i, total=0;
	for(i=0; i<MAXACTIVE; i++)
		if((shm_utmp->uinfo[i].active) && (shm_utmp->uinfo[i].invisible==0||!strcasecmp(currentuser.userid,shm_utmp->uinfo[i].userid))) total++;
	return total;
}

struct override fff[300];
struct override fff2;
int friendnum=0;
int friendnum2=0;
int
cmpfuid(a, b)
unsigned short *a, *b;
{
	return *a - *b;
}

int loadfriend(char *id) {
        FILE *fp;
        char file[256];
/*
        if(!loginok) return -1;
*/
	friendnum=0;
        sprintf(file, "/home/bbs/home/%c/%s/friends", toupper(id[0]), id);
        fp=fopen(file, "r");
        if(fp) {
                friendnum=fread(fff, sizeof(fff[0]), 300, fp);
                fclose(fp);
	}
/*
	fp=fopen(file, "r");
	if(fp) 
	{
		friendnum2=0;
		while(1)
		{
			if(fread(&fff2, sizeof(fff2), 1, fp)<=0) break;
			u_info->friend[friendnum2]=getusernum(fff2.id)+1;
			friendnum2++;	
			
		}
                fclose(fp);
		qsort(&u_info->friend, friendnum2, sizeof(&u_info->friend[0]), cmpfuid);
	}
*/
	for(friendnum2=0; friendnum2<friendnum; friendnum2++)
	{
		u_info->friend[friendnum2]=getusernum(fff[friendnum2].id) +1;
/*
		qsort(&u_info->friend, friendnum2, sizeof(u_info->friend[0]), cmpfuid);
*/
	}
	u_info->fnum = friendnum;
	qsort (&u_info->friend, friendnum, sizeof (u_info->friend[0]), cmpfuid);
	return 0;
}


int isfriend(char *id) {
	static inited=0;
        int n;
	if(!inited) {
		loadfriend(currentuser.userid);
		inited=1;
	}
        for(n=0; n<friendnum; n++)
                if(!strcasecmp(id, fff[n].id)) return 1;
        return 0;
}

struct override bbb[MAXREJECTS];
int badnum=0;

int loadbad(char *id) {
        FILE *fp;
        char file[256];
        if(!loginok) return;
        sprintf(file, "home/%c/%s/rejects", toupper(id[0]), id);
        fp=fopen(file, "r");
        if(fp) {
                badnum=fread(fff, sizeof(fff[0]), MAXREJECTS, fp);
                fclose(fp);
        }
}

int isbad(char *id) {
        static inited=0;
        int n;
        if(!inited) {
                loadbad(currentuser.userid);
                inited=1;
        }
        for(n=0; n<badnum; n++)
                if(!strcasecmp(id, bbb[n].id)) return 1;
        return 0;
}

int init_all() {
	srand(time(0)*2+getpid());
	chdir(BBSHOME);

	http_init();
	seteuid(BBSUID);
	if(geteuid()!=BBSUID) http_fatal("uid error.");
	shm_init();
	loginok=user_init(&currentuser, &u_info);
        if(u_info==0){
             u_info=&guest;
     	}
#ifdef MUST_LOGIN
		if (*(getsenv("HTTP_HOST")) && !loginok && !strstr(getsenv("SCRIPT_NAME"), "log") && !strstr(getsenv("SCRIPT_NAME"), "reg"))
		{
			printf ("<script>alert('Ö»ÓÐµÇÂ¼ºó²Å¿ÉÒÔ·ÃÎÊ£¡');location.href='/';</script>");
			exit (0);
		}
#endif  
 	if(!NOTADDCSSFILE)       setdftstyle();
}

int init_no_http() {
	srand(time(0)+getpid());
	chdir(BBSHOME);
	shm_init();
}

#ifdef MUST_EXISTING_USER
int isPosterExists(struct fileheader * x)
{
	if (!strcmp(x->owner, BBSID)||strchr(x->owner,'.'))
	{
		return 1;
	}
	struct userec *u;
	u = getuser(x->owner);
	return u&&(!!(u->userlevel & PERM_LOGINOK));//sillyhan@2008/01/10 ÓÃ»§²»´æÔÚÊ±£¬uÎªnull
	//return !!(getuser(x->owner));
}

int postReadable(struct fileheader * x)
{
	if (HAS_PERM(PERM_SYSOP | PERM_OBOARDS))
	{
		return 1;
	}
	return isPosterExists(x);
}

int http_postReadable(struct fileheader * x)
{
	if (!postReadable(x))
	{
		printf ("¶Ô²»Æð,¸ÃÓÃ»§ÉÐÎ´×¢²á!");
		exit (0);
		return 0;
	}
	return 1;
}
#endif

int page_init(const char * CONTENTTYPE) 
//huangxu@080107:¸÷ÖÖÒ³Ãæ³õÊ¼»¯
{
	char *buf, buf2[1024], *t2, *t3;
	int n;
	srand(time(0)*2+getpid());
	chdir(BBSHOME);
	printf ("Cache-Control: no-cache, must-revalidate\nPragma: no-cache\n");
	if (CONTENTTYPE)
	{
		printf("Content-type: %s; charset=%s\n\n", CONTENTTYPE, CHARSET);
	}
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
	strsncpy(fromhost, getsenv(SENVFROM), 32);
	//ÕâÀï´¦Àí×Ö·û±àÂë
	paraencoding();
	seteuid(BBSUID);
	if(geteuid()!=BBSUID) http_fatal("uid error.");
	shm_init();
	loginok=user_init(&currentuser, &u_info);
        if(u_info==0){
             u_info=&guest;
        }
  return;
}

int xml_init() 
{
	int i = page_init("text/xml");
#ifdef MUST_LOGIN
	if (*(getsenv("HTTP_HOST")) && !loginok)
	{
		exit (0);
	}
#endif
	return i;
}

int html_init() 
{
	int i = page_init("text/html");
#ifdef MUST_LOGIN
	if (*(getsenv("HTTP_HOST")) && !loginok)
	{
		printf ("<script>alert('Ö»ÓÐµÇÂ¼ºó²Å¿ÉÒÔ·ÃÎÊ£¡');location.href='/';</script>");
		exit (0);
	}
#endif
	return i;
}

char *void1(unsigned char *s) {
        int i;
        int flag=0;
        for(i=0; s[i]; i++) {
                if(flag==0) {
                        if(s[i]>=128) flag=1;
                        continue;
                }
                flag=0;
                if(s[i]<32) s[i-1]=32;
        }
        if(flag) s[strlen(s)-1]=0;
        return s;
}

char *sec(char c) {
	int i;
	for(i=0; i<SECNUM; i++) {
		if(strchr(seccode[i], c)) return secname[i][0];
	}
	return "(unknown.)";
}

char *flag_str(int access) {
	static char buf[80];
	char *flag2="";
	strcpy(buf, "  ");
	if(access & FILE_DIGEST) flag2="G";
	if(access & FILE_MARKED) flag2="M";
	if((access & FILE_MARKED) && (access & FILE_DIGEST)) flag2="B";
	if(access & FILE_NOREPLY) buf[0]='X';
	sprintf(buf, "%s", flag2);
	return buf;
}

char *flag_str2(int access, int has_read) {
        static char buf[80];
	strcpy(buf, "   ");
        if(loginok) strcpy(buf, "N  ");
        if(access & FILE_DIGEST) buf[0]='G';
        if(access & FILE_MARKED) buf[0]='M';
        if((access & FILE_MARKED) && (access & FILE_DIGEST)) buf[0]='B';
	if(access & FILE_NOREPLY) buf[1]='X';
        if(access & FILE_VISIT)  buf[0]='I';
	if(has_read)
	{
		buf[0]=tolower(buf[0]);
		buf[1]=tolower(buf[1]);
	}
	if(buf[0]=='n') buf[0]=' ';
        return buf;
}

char *userid_str(char *s) {
	static char buf[512];
	char buf2[256], tmp[256], *ptr, *ptr2;
	strsncpy(tmp, s, 255);
	buf[0]=0;
	ptr=strtok(tmp, " ,();\r\n\t");
	while(ptr && strlen(buf)<400) {
		if(ptr2=strchr(ptr, '.')) {
			ptr2[1]=0;
			strcat(buf, ptr);
			strcat(buf, " ");
		} else {
			ptr=nohtml(ptr);
			if(strcmp(ptr, "SYSOPs"))
				sprintf(buf2, "<a href=bbsqry?userid=%s>%s</a> ", ptr, ptr);
			else
				sprintf(buf2, "%s ", ptr);
			strcat(buf, buf2);
		}
		ptr=strtok(0, " ,();\r\n\t");
	}
	return buf;
}

int fprintf2(FILE *fp, char *s) {
        fprintf(fp, "%s", s);
        /*
           ´æ´¢Ê±½øÐÐ¶ÏÐÐ´¦Àí
           Í¨¹ý¶ÔtextµÄwrapÊôÐÔÉèÖÃ,ie¿ÉÒÔºÜºÃµÄ´¦Àí¶ÏÐÐ.²»¹ýËÆºõmozillaÓÐÎÊÌâ
           Õâ¶Î´úÂë¿ÉÄÜ²úÉúÂÒÂëÎÊÌâ,¹Ê×¢µôÁË
         */
        /*
	int i, tail=0, sum=0;
	if(s[0]==':' && s[1]==' ' && strlen(s)>79) {
		sprintf(s+76, "..\n");
		fprintf(fp, "%s", s);
		return;
	}
	for(i=0; s[i]!=0; i++) {
                if(s[i]==13){
             		fprintf(fp, "\n");
			return ;
                }
                else
			fprintf(fp,"%c",s[i]);
		sum++;
		if(tail!=0) {
			tail=0;
		} else if(s[i]<0) {
			tail=s[i];
		}
		if(sum>=78 && tail==0) {
			if(s[i+1]!=13) fprintf(fp, "\n");
			sum=0;
		}
	}*/
}

struct fileheader *get_file_ent(char *board, char *file) {
	FILE *fp;
	char dir[80];
	static struct fileheader x;
	int num=0;
	sprintf(dir, "boards/%s/.DIR", board);
	fp=fopen(dir, "r");
	while(1) {
		if(fread(&x, sizeof(x), 1, fp)<=0) break;
		if(!strcmp(x.filename, file)) {
			fclose(fp);
			return &x;
		}
		num++;
	}
	fclose(fp);
	return 0;
}


/*path±ØÐë´æÔÚ*/
int getfrompath(char *path,char *s) 
{
        char buf[32];
        strcpy(buf,"");
        strcpy(s,"");
      	if(!strncmp(path,"/groups/GROUP_",14)){      //return 0;//²»ÊÇÌÖÂÛÇø¾«»ª
	      int size=strlen(path);
  	      int j=2,i=0,n=0;
	      for(i=14;i<size;i++){
                  if(j==3)
                    if(path[i]!='/')  buf[n++]=path[i];
   	          if(path[i]=='/') j++;
  	          if(j==4){
       		      buf[n]=0;
       		      break;
    	          }
	      }
 	     if(j>=3){
                 buf[n]=0;
                 strcpy(s,buf); 
                 return 1; //sÊÇÏàÓ¦µÄ°åÃæÃû³Æ
             }
      	     else   return 0; 
        }
       
       if(!strncmp(path,"/PersonalCorpus",15)){     
           int size=strlen(path);
           int j=1,i=0,n=0;
           for(i=15;i<size;i++){
              if(j==3)
                    if(path[i]!='/')  buf[n++]=path[i];
              if(path[i]=='/') j++;
              if(j==4){
                 buf[n]=0;
                 break;
              }
           }
          if(j>=3){
             buf[n]=0;
             strcpy(s,buf);
             return 2; //sÊÇ¸öÈËÎÄ¼¯µÄËùÓÐÕß
          }
          else   return 0; 
      }
     
      return 0;//ÆÕÍ¨¾«»ªÇø

}


int set_my_cookie() {
	FILE *fp;
	char path[256], buf[256], buf1[256], buf2[256];
	int my_t_lines=30, my_link_mode=0, my_def_mode=0, my_localpic_mode=0;
	int my_showhead_mode=0;
        int highspeed=1;
	sprintf(path, "home/%c/%s/.mywww", toupper(currentuser.userid[0]), currentuser.userid);
 	fp=fopen(path, "r");
 	if(fp) {
 		while(1) {
 			if(fgets(buf, 80, fp)==0) break;
 			if(sscanf(buf, "%80s %80s", buf1, buf2)!=2) continue;
 			if(!strcmp(buf1, "t_lines")) my_t_lines=atoi(buf2);
 			if(!strcmp(buf1, "link_mode")) my_link_mode=atoi(buf2);
			if(!strcmp(buf1, "def_mode")) my_def_mode=atoi(buf2);
			if(!strcmp(buf1, "localpic_mode"))
			my_localpic_mode=atoi(buf2);
			if(!strcmp(buf1, "showhead_mode"))
			my_showhead_mode=atoi(buf2);
			if(!strcmp(buf1, "highspeed"))
                              highspeed=atoi(buf2);

 		}
 		fclose(fp);
		sprintf(buf, "%d", my_t_lines);
		setcookie("my_t_lines", buf);
		sprintf(buf, "%d", my_link_mode);
		setcookie("my_link_mode", buf);
	        sprintf(buf, "%d", my_def_mode);
            	setcookie("my_def_mode", buf);
		sprintf(buf, "%d", my_localpic_mode);
		setcookie("my_localpic_mode", buf);
		sprintf(buf, "%d", my_showhead_mode);
		setcookie("my_showhead_mode", buf);
	        sprintf(buf, "%d", highspeed);
                setcookie("highspeed", buf);

 	}
 	sprintf(path, "home/%c/%s/.www.style", toupper(currentuser.userid[0]), currentuser.userid);
 	memset(buf,0,sizeof(buf));
	if(fp=fopen(path, "rb"))
	{
		fgets(buf,80,fp);
		if(!*buf)
			strcpy(buf,DEFSTYLE);
		sprintf(path,"/home/www/html/css/%s/bbsguide4.css",buf);
		if(!file_exist(path))
			strcpy(path,DEFSTYLE);
		printf("<script>document.cookie='home_style=%s; path=/';</script>",buf);
		fclose(fp);
	}
}

int has_fill_form() {
        FILE *fp;
        int r;
        char userid[256], tmp[256], buf[256], *ptr;
        fp=fopen("new_register", "r");
        if(fp==0) return 0;
        while(1) {
                if(fgets(buf, 100, fp)==0) break;
                r=sscanf(buf, "%s %s", tmp, userid);
                if(r==2) {
                        if(!strcasecmp(tmp, "userid:") && !strcasecmp(userid, currentuser.userid)) {
                                fclose(fp);
                                return 1;
                        }
                }
        }
        fclose(fp);
        return 0;
}

struct dir {
        char board[20];
        char userid[14];
        char showname[40];
        char exp[80];
        char type[30];
        int filename;
        int date;
	int level;
        int size;
        int live;
	int click;
        int active;
	int accessed;
};

//2001.09.04 Haishion add from convert GoodBrd 2 MyBrds
//#define GOOD_BRC_NUM	40
char fname2[STRLEN];
int LoadMyBrdsFromGoodBrd(char userid[IDLEN + 2], char mybrd[GOOD_BRC_NUM][80])
{
	FILE	*fp;
	char	fname[STRLEN];
	int	i=0;
	sprintf(fname, "home/%c/%s/.goodbrd", toupper(userid[0]), userid);
//add by lionel
//ÏÂÃæÕâÁ½ÐÐ×¢ÊÍµô°É,.goodbrd²»´æÔÚÁË»¹ÓÐfavÄØ
//	sprintf(fname2, "touch home/%c/%s/.goodbrd", toupper(userid[0]), userid);
//	system(fname2);//add by bluetent Èç¹û.goodbrdÎÄ¼þ²»´æÔÚ»á³öÏÖÂÒÂë½Ø¶ÏÊÕ²Ø¼ÐÒÔÏÂµÄ¹¦ÄÜÏÔÊ¾

	if (fp = fopen(fname, "r")) {
		for (i = 0; i< GOOD_BRC_NUM; i++) {
			if (EOF == fscanf(fp, "%s\n", mybrd[i]))
				break;
		}	
		fclose(fp);
	}
        if(i>0) return i;
        if (fp = fopen("etc/fav", "r")) {
              for (i = 0; i< GOOD_BRC_NUM; i++) {
                  if (EOF == fscanf(fp, "%s\n", mybrd[i])) break;
              }
              fclose(fp);
              if(i>0) return i;
        }
        strcpy(mybrd[0],"sysop");          
        return 1;
}

void SaveMyBrdsToGoodBrd(char userid[IDLEN + 2], char mybrd[GOOD_BRC_NUM][80], int mybrdnum)
{
        FILE    *fp;
        char    fname[STRLEN];
        int     i;
        sprintf(fname, "home/%c/%s/.goodbrd", toupper(userid[0]), userid);
//	sprintf(fname2, "touch home/%c/%s/.goodbrd", toupper(userid[0]), userid);
//	system(fname2);//add by bluetent Èç¹û.goodbrdÎÄ¼þ²»´æÔÚ»á³öÏÖÂÒÂë½Ø¶ÏÊÕ²Ø¼ÐÒÔÏÂµÄ¹¦ÄÜÏÔÊ¾
        if (fp = fopen(fname, "w+")) {
                for (i = 0; i< mybrdnum; i++) 
			fprintf(fp, "%s\n", mybrd[i]);
                fclose(fp);
        }

}
//2001.09.04 Haishion add from convert GoodBrd 2 MyBrds

char datestring[30];
int getdatestring( time_t now, int Chang)
{
	struct tm *tm;
	char weeknum[7][3]={"Ìì","Ò»","¶þ","Èý","ËÄ","Îå","Áù"};

	tm = localtime(&now);
	sprintf(datestring,"%4dÄê%02dÔÂ%02dÈÕ%02d:%02d:%02d ÐÇÆÚ%2s",
		tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
		tm->tm_hour,tm->tm_min,tm->tm_sec,
		weeknum[tm->tm_wday]);
        return (tm->tm_sec % 10);
}

//add by bluetent 2002.10.30
int has_new_msg()
{
	char buf[256];
	if(!loginok)return 0;
	sprintf(buf, "home/%c/%s/wwwmsg", toupper(currentuser.userid[0]), currentuser.userid);
	if(file_size(buf)>0)return 1;
	return 0;
}

int check_msg()
{
    //	if(has_new_msg())printf("<script>if(!top.fmsg.inmsg)top.fmsg.location.reload();</script>\n");
}

/* add by yiyo for deny user's mail */
int deny_m_user( char * user)
{
 char filepath[STRLEN];
 char userid[40], *ptr;
 FILE *fp;
 if (user_perm(&currentuser, PERM_SYSOP)) return 0;
 sprintf(filepath, "home/%c/%s/deny_user", toupper(user[0]), user);
 if ((fp = fopen(filepath, "r")) == NULL) return 0;
 while (fgets(userid, 30, fp)) {
        ptr = strtok(userid, " \t\n\r");
        if ( ptr != NULL && *ptr != '#' ) {
                if (!strcasecmp(ptr, currentuser.userid)) {
                fclose(fp);
                return 1;
                        }
                }
        }
        fclose(fp);
        return 0;
}
/* add end by yiyo */

int
seek_in_file(filename, seekstr)
char    *filename, *seekstr;
{
	FILE   *fp;
	char    buf[80];
	char   *namep;
	if ((fp = fopen(filename, "r")) == NULL)
		return 0;
	while (fgets(buf, 80, fp) != NULL) {
		namep = (char *) strtok(buf, ": \n\r\t");
		if (namep != NULL && !strcmp(namep, seekstr)) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

void
do_report(filename,s)
char   *s,*filename;
{
	char    buf[512];
	time_t  dtime;
	time(&dtime);
	getdatestring(dtime,NA);
// modified by bluetent 2002.12.09
	sprintf(buf, "%s %22.22s %s %s\n", currentuser.userid, datestring, s, fromhost);
	f_append(filename, buf);
}
void 
report(s)
char	*s;
{
	do_report("trace",s);
	do_report("tracetoday",s);//bluetent 2003.1.1
}

int isdormip(char *ip)
{
	if(strstr(ip, "10.10.")==ip || strstr(ip,"10.22.")==ip || strstr(ip,"10.21.")==ip || strstr(ip,"10.13.")== ip) return 1;
	return 0;
}
/*
int isAllowedIp(char *ip)
{
	if(strstr(ip,"211.102.114.")==ip || strstr(ip,"211.102.112.")==ip || strstr(ip,"61.181.245.238")==ip || strstr(ip,"211.102.110.")==ip || strstr(ip,"10.")==ip || strstr(ip,"202.113.16.")==ip || strstr(ip,"202.113.17.")==ip || strstr(ip,"202.113.18.")==ip || strstr(ip,"202.113.19.")==ip || strstr(ip,"202.113.20.")==ip || strstr(ip,"202.113.21.")==ip || strstr(ip,"202.113.22.")==ip || strstr(ip,"202.113.23.")==ip || strstr(ip,"202.113.24.")==ip || strstr(ip,"202.113.25.")==ip || strstr(ip,"202.113.26.")==ip || strstr(ip,"202.113.27.")==ip || strstr(ip,"202.113.28.")==ip|| strstr(ip,"202.113.29.")==ip || strstr(ip,"202.113.30.")==ip || strstr(ip,"202.113.31.")==ip || strstr(ip,"202.113.224.")==ip || strstr(ip,"202.113.225.")==ip || strstr(ip,"202.113.226.")==ip || strstr(ip,"202.113.227.")==ip || strstr(ip,"202.113.228.")==ip || strstr(ip,"202.113.229.")==ip || strstr(ip,"202.113.230.")==ip || strstr(ip,"202.113.231.")==ip || strstr(ip,"202.113.232.")==ip || strstr(ip,"202.113.233.")==ip || strstr(ip,"202.113.234.")==ip || strstr(ip,"202.113.235.")==ip || strstr(ip,"202.113.236.")==ip || strstr(ip,"202.113.237.")==ip || strstr(ip,"202.113.238.")==ip || strstr(ip,"202.113.239.")==ip || strstr(ip,"222.30.48.")==ip || strstr(ip,"222.30.49.")==ip || strstr(ip,"222.30.50.")==ip || strstr(ip,"222.30.51.")==ip || strstr(ip,"222.30.52.")==ip || strstr(ip,"222.30.53.")==ip)
		return 1;
	else
		return 0;
}*/





int isAllowedIp(char *ip)
{
 char tmpbuf[STRLEN];
 int life;   
 if(HAS_PERM(PERM_XEMPT)) return 1;
 if(HAS_PERM(PERM_CLOAK)) return 1;

 if(file_has_word("vipuser",currentuser.userid)) return 1;

//add by istef @ 7/1/2005
//ÍâÍøÒÑÈÏÖ¤ÓÃ»§·¢ÎÄ

 sprintf(tmpbuf,"%s/home/%c/%s/postAnywhere.allow",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
 if(file_exist(tmpbuf)) return 1;

	sprintf(tmpbuf,"%s/home/%c/%s/.webauth.done",BBSHOME,toupper(currentuser.userid[0]),currentuser.userid);
 if(file_exist(tmpbuf)) return 1;

//int days=(time(0) - currentuser.firstlogin)/86400;
 //if(days>1096) return 1;

//add by brew 050706
   //ÓÀ6ºÍÓÀ9
   life=count_life_value(&currentuser);
   if(life==666||life==999)
        return 1;


    FILE *fp=fopen("ip.allow","r");
    if(fp==NULL) return 1;
    char buf[80];
    char *p;
    while(1){
        if(fgets(buf, 80, fp)<=0) break;
        p=strtok(buf, " \n\t\r");
        if(p==NULL||*p=='#') continue;
        if(strstr(ip,p)==ip||( strstr(p,"/") && ipzone(ip,p)))
	{
           fclose(fp);
           return 1;
        }
    }
    fclose(fp);
    return 0;
}
int ipzone(char *ip, char *zone)
{
    int start,end,i;
    char s[80];
    char *p=index(zone,'/');
    if(p==NULL)  return 1;
    end=atoi(p+1);
    *p=0;
    p=rindex(zone,'.');
    if(p==NULL)  return 1;
    start=atoi(p+1);
    *p=0;

    for(i=start;i<=end;i++){
       sprintf(s,"%s.%d",zone,i);
       if(strstr(ip,s)==ip) return 1;
    }

    return 0;
}
int efantest (thename, theprompt)
char * thename;
char * theprompt;
{
	FILE *fp;
	char buf [50];
	if (strcmp (currentuser.userid, thename))
		return 0;
	sprintf (buf, "/tmp/%s.log", thename);
	fp = fopen (buf, "w");
	fprintf (fp, "%s\n", theprompt);
	fclose (fp);
	return 0;
}

int dashf (fname)
char * fname;
{
	struct stat st;
	return (stat (fname, &st) ==0 && S_ISREG (st.st_mode) );
}

int teamofboard (theuser, board)
char * theuser;
char * board;
{
	char buf [256];
	FILE *fp;
	sprintf (buf, "%s/boards/%s/board.allow", BBSHOME, board);
	if (!dashf (buf))
		return 1;
	fp = fopen (buf, "r");
	if (!fp)
		return 1;
	while (fscanf (fp, "%s", buf) != EOF)
		if (!strcasecmp (buf, theuser) )
		{
			fclose (fp);
			return 1;
		}
	fclose (fp);
	return 0;
}

int teamofwrite (char * theuser, char * board)
{
	char buf [256];
	FILE *fp;
	sprintf (buf, "%s/boards/%s/board.write", BBSHOME, board);	if (!dashf (buf))	return 1;
	
	fp = fopen (buf, "r");
	if (!fp)
		return 1;
	while (fscanf (fp, "%s", buf) != EOF)
		if (!strcmp (buf, theuser) )
		{
			fclose (fp);
			return 1;
		}
	fclose (fp);
	return 0;
}

int canenterboard (theuser, board)
char * theuser;
char * board;
{
	char buf [256];
	FILE *fp;
	sprintf (buf, "%s/boards/%s/board.deny", BBSHOME, board);
	if (!dashf (buf))
		return 1;
	fp = fopen (buf, "r");
	if (!fp)
		return 1;
	while (fscanf (fp, "%s", buf) != EOF)
		if (!strcmp (buf, theuser) )
		{
			fclose (fp);
			return 0;
		}
	fclose (fp);
	return 1;
}

void strtolower( char *dst, char *src )
{
    for( ; *src; src++ )
       *dst++ = tolower( *src );
    *dst = '\0';
}
int insec(int n,char *board) 
{ 
    
       struct shortfile *bp=getbcache(board); 
       if(strchr(seccode[n], bp->title[0]))
           return 1; // ÔÚ¸ÃÇø
       else 
           return 0; 
} 
                                                                       
int outgo_post(struct fileheader *fh, char *board)                                                           
{                                                                               
        char    buf[256];                                                       
        sprintf(buf, "%s\t%s\t%s\t%s\t%s\n", board,                             
                fh->filename, currentuser.userid,currentuser.username,fh->title);                                            
        f_append("innd/out.bntp", buf);                                      
}                                                                               

int googleads()
{


printf("<script type=\"text/javascript\"><!--\
google_ad_client = \"pub-7020572115244230\";\
google_alternate_color = \"FFFFFF\";\
google_ad_width = 160;\
google_ad_height = 600;\
google_ad_format = \"160x600_as\";\
google_ad_channel =\"0174831605\";\
google_ad_type = \"text\";\
google_page_url = document.location;\
google_color_border = \"FFFFFF\";\
google_color_bg = \"FFFFFF\";\
google_color_link = \"0000FF\";\
google_color_url = \"0000FF\";\
google_color_text = \"000000\";\
//--></script>\
<script type=\"text/javascript\"\
  src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">\
</script>"
);

}
/*
*kill_quotation_mark()¹ýÂËÁ´½Ó(URL)ÖÐµÄµ¥ÒýºÅ£¬Ë«ÒýºÅ
*·ÀÖ¹±êÌâÖÐÒýºÅÔì³ÉµÄ»ìÂÒ
*--by zhangchn 2004.12.22
*/

//huangxu@060402:²»½ö½öÊÇÒýºÅ
void kill_quotation_mark(char *filtered, char *input)
{
	char *ptr=input;
	char *pft=filtered;
	while(*ptr)
	{
          if((*ptr)&0x80||strchr("\x7E\x21\x40\x23\x24\x25\x5E\x26\x2A\x28\x29\x5F\x2B\x60\x2D\x3D\x5B\x5D\x7B\x7D\x5C\x5C\x7C\x3A\x3B\x5C\x27\x5C\x22\x2C\x2E\x2F\x3C\x3E\x3F\x20",*ptr)) {
                  *(pft++)='%';
                  *(pft++)=myto16(((*ptr)&0xf0)>>4);
                  *(pft++)=myto16((*ptr)&0x0f);
					}
					else
									*(pft++)=*ptr;
					ptr++;
	}
	*pft=0;
	return;
}

int strlower(char *s)
{
 char *p=s;
  while(*p){
   *p=tolower(*p);
   p++;
  }
}

int check_limit(struct userec *x,char *file, int value, int limit)
{
        FILE *fp;
        char buf[100];
        int num;
        struct stat buf2;
        if ( strcasecmp(x->userid, "guest") == 0 || HAS_PERM(PERM_SYSOP))
                return 0;
        sethomefile(buf, x->userid, file);
        if((fp = fopen(buf,"r")) == NULL){
                fp = fopen(buf,"w");
                fprintf(fp, "%d", value);
                fclose(fp);
                return 0;
        }
        //ÔÝÊ±½â¾ölogincheck 0×Ö½ÚµÄÎÊÌâ
        fseek(fp, 0, SEEK_END);
        if(ftell(fp) == 0){
                fclose(fp);
                unlink(buf);
                return 0;
        }
        fseek(fp, 0, SEEK_SET);
        fscanf(fp,"%d",&num);
        fclose(fp);
        stat(buf, &buf2);
        if(abs(buf2.st_mtime-time(0))>86400)
        {
                unlink(buf);
                return 0;
        }
     
        time_t tt=buf2.st_mtime+86400;

        if(abs(value-num)>limit)
               return tt;
        return 0;
}

int check_post_limit(char *board)
{
     return 0;
     time_t tt=check_limit(&currentuser,"postcheck", currentuser.numposts, 100);

     if(tt!=0) //ÏÞÖÆÃ¿¸öÈËÃ¿ÌìÖ»¿ÉÒ»·¢ÎÄ¡¢×ªÔØ72Æª
     {
           if(junkboard(board)) return 0;
           getdatestring(tt, NA);
           printf("»Ö¸´Ê±¼ä£º%s<br>", datestring);
           printf("´ËÕÊºÅ24Ð¡Ê±ÄÚ·¢ÎÄ´ÎÊý¹ý¶à, ÇëÐªÒ»Ðª£¬¹ýÒ»¶ÎÊ±¼äÔÙÀ´, ÇëÎðÊ¹ÓÃ¹àË®»ú.\n");
           return 1;
     }
     return 0;
}

int check_login_limit(struct userec *x )
{

     if(check_limit(x,"logincheck", x->numlogins, 100)) 
     {
           //printf("´ËÕÊºÅ24Ð¡Ê±ÉÏÕ¾´ÎÊý¹ý¶à, ÇëÐªÒ»Ðª£¬¹ýÒ»¶ÎÊ±¼äÔÙÀ´, ÇëÎðÊ¹ÓÃÉÏÕ¾»ú.\n");
           return 1;
     }
    return 0;
}

//huangxu@060408:ÓÃ»§×ÊÁÏ
char * sprintgetuinfo(struct userec u)
{
	static char struinfo[512];
	int     num;
	char    buf[40];
	char    usex[3];
	if (u.gender=='M'||u.gender=='m')
		strcpy(usex,"Ëû");
	else
		strcpy(usex,"Ëý");
	sprintf(struinfo,"\n\n%sµÄ´úºÅ     : %s\n", usex, u.userid);
	sprintf(struinfo,"%s%sµÄêÇ³Æ     : %s\n",struinfo, usex, u.username);
	sprintf(struinfo,"%sÕæÊµÐÕÃû     : %s\n",struinfo, u.realname);
	sprintf(struinfo,"%s¾Ó×¡×¡Ö·     : %s\n",struinfo, u.address);
	sprintf(struinfo,"%sµç×ÓÓÊ¼þÐÅÏä : %s\n",struinfo, u.email);
	sprintf(struinfo,"%sÕæÊµ E-mail  : %s\n",struinfo, u.reginfo);
	sprintf(struinfo,"%sÕÊºÅ×¢²áµØÖ· : %s\n",struinfo, u.ident);
	getdatestring(u.firstlogin,NA);
	sprintf(struinfo,"%sÕÊºÅ½¨Á¢ÈÕÆÚ : %s\n",struinfo, datestring);
	getdatestring(u.lastlogin,NA);
	sprintf(struinfo,"%s×î½ü¹âÁÙÈÕÆÚ : %s\n",struinfo, datestring);
	sprintf(struinfo,"%s×î½ü¹âÁÙ»úÆ÷ : %s\n",struinfo, u.lasthost);
	sprintf(struinfo,"%sÉÏÕ¾´ÎÊý     : %d ´Î\n",struinfo, u.numlogins);
	sprintf(struinfo,"%sÎÄÕÂÊýÄ¿     : %d\n",struinfo, u.numposts);
	sprintf(struinfo,"%sÉÏÕ¾×ÜÊ±Êý   : %d Ð¡Ê± %d ·ÖÖÓ\n",
	        struinfo,u.stay / 3600, (u.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F0s2345678");
	for (num = 0; num < 30; num++)
		if (!(u.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	sprintf(struinfo,"%sÊ¹ÓÃÕßÈ¨ÏÞ   : %s\n\n",struinfo, buf);
	return struinfo;
}

int getuinfo(FILE * fn)
{
	int     num;
	char    buf[40];
	char    usex[3];
	if (currentuser.gender=='M'||currentuser.gender=='m')
		strcpy(usex,"Ëû");
	else
		strcpy(usex,"Ëý");
	fprintf(fn, "\n\n%sµÄ´úºÅ     : %s\n",usex, currentuser.userid);
	fprintf(fn, "%sµÄêÇ³Æ     : %s\n",usex, currentuser.username);
	fprintf(fn, "ÕæÊµÐÕÃû     : %s\n", currentuser.realname);
	fprintf(fn, "¾Ó×¡×¡Ö·     : %s\n", currentuser.address);
	fprintf(fn, "µç×ÓÓÊ¼þÐÅÏä : %s\n", currentuser.email);
	fprintf(fn, "ÕæÊµ E-mail  : %s\n", currentuser.reginfo);
	fprintf(fn, "ÕÊºÅ×¢²áµØÖ· : %s\n", currentuser.ident);
	getdatestring(currentuser.firstlogin,NA);
	fprintf(fn, "ÕÊºÅ½¨Á¢ÈÕÆÚ : %s\n", datestring);
	getdatestring(currentuser.lastlogin,NA);
	fprintf(fn, "×î½ü¹âÁÙÈÕÆÚ : %s\n", datestring);
	fprintf(fn, "×î½ü¹âÁÙ»úÆ÷ : %s\n", currentuser.lasthost);
	fprintf(fn, "ÉÏÕ¾´ÎÊý     : %d ´Î\n", currentuser.numlogins);
	fprintf(fn, "ÎÄÕÂÊýÄ¿     : %d\n", currentuser.numposts);
	fprintf(fn, "ÉÏÕ¾×ÜÊ±Êý   : %d Ð¡Ê± %d ·ÖÖÓ\n",
	        currentuser.stay / 3600, (currentuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(currentuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, "Ê¹ÓÃÕßÈ¨ÏÞ   : %s\n\n", buf);
}

int canseeme(char * id)
{
	char genbuf[128];
	sprintf(genbuf, "home/%c/%s/mylover", toupper(*id), id);
	//if(dashf(genbuf))//huangxu:it is a bug
		return seek_in_file(genbuf,currentuser.userid);
}//loveni:ÌØ¶¨Ãûµ¥¿´´©ÒþÉí

int has_ZM_perm(struct userec *user,char* board)
//huangxu@060720:ÇøÎñ
{
	int i;
	struct shortfile *x;
	if(!(x=getbcache(board)))
		return 0;
	for(i=0;i<SECNUM;i++)
	{
		if(strchr(seccode[i],x->title[0]))
		{
			return has_BM_perm(user,ZONEADMIN[i]);
		}
	}
	return 0;
}

int endswith(char* str, char* tail)
{
	char * p;
	p = strstr(str, tail);
	if (p)
	{
		if (strlen(p) == strlen(tail))
		{
			return YEA;
		}
	}
	return NA;
}

int wwwlogin(struct userec *user, int * putmpnum, int * putmpkey)
{
        FILE *fp;
        char buf[80],buf2[256],ipfile[256];
        int pid, n, tmp;
        struct user_info *u;
        //add by yiyo
        struct user_enter
        {
                char   loginfrom[20];
                char   logintime[30];
        }
        rduser;
        //add end
        if(!(currentuser.userlevel & PERM_LOGINOK))
        {
                char file[256];
                sprintf(file, "home/%c/%s/register",
                        toupper(currentuser.userid[0]), currentuser.userid);
                if(file_exist(file)) {
                        currentuser.userlevel |=PERM_DEFAULT;
                        save_user_data(&currentuser);
                }
        }
        fp=fopen(".UTMP.lock", "a");
        flock(fileno(fp), LOCK_EX);


        for(n=0; n<MAXACTIVE; n++)
        {
                if(shm_utmp->uinfo[n].active == 0) {
                        u=&(shm_utmp->uinfo[n]);
                        bzero(u, sizeof(struct user_info));

                        u->active=1;
                        u->uid=getusernum(user->userid)+1;
                        u->pid=getpid();
                        u->mode=LOGIN+20000;
                        if(user_perm(&currentuser, PERM_LOGINCLOAK) &&
                                        (currentuser.flags[0] & CLOAK_FLAG))


                        u->invisible = YEA;
                        u->pager = 0;


                        if(currentuser.userdefine & DEF_FRIENDCALL)
                                u->pager|=FRIEND_PAGER;
                        if(currentuser.flags[0] & PAGER_FLAG) {
                                u->pager|=ALL_PAGER;
                                u->pager|=FRIEND_PAGER;
                        }
                        if(currentuser.userdefine & DEF_FRIENDMSG)
                                u->pager|=FRIENDMSG_PAGER;
                        if(currentuser.userdefine & DEF_ALLMSG) {
                                u->pager|=ALLMSG_PAGER;
                                u->pager|=FRIENDMSG_PAGER;
                        }

                        strsncpy(u->from, fromhost, 24);
                        //modified by bluetent 2003.01.01
                        if(strcasecmp(user->userid,"guest"))
                                u->from[22] = 'H';

			//Modified by byang, to implement user online time accounting work correctly even if user close the browser without logout first. I add a new field in user_info to record the last time the online time is record.
                        *(int*)(u->from+30)=u->lastrecord=time(0);
                        u->idle_time=time(0);
                        strsncpy(u->username, user->username, 20);
                        strsncpy(u->userid, user->userid, 13);
                        tmp=rand()%100000000;
 
                        u->utmpkey=tmp;
                        //sprintf(buf, "%d", n);
                        //setcookie("utmpnum", buf);
                        //sprintf(buf, "%d", tmp);
                        //setcookie("utmpkey", buf);
                        //setcookie("utmpuserid", currentuser.userid);
                        //set_my_cookie();
                        *putmpnum = n;
                        *putmpkey = tmp;
 
                        flock(fileno(fp), LOCK_UN);
                        fclose(fp);
			/* add by brew 2005.6.17 */
                        /* just to set the global var "u_info" 
			   used in loadfriend();
			  */
			u_info=u;


                        //add by bluetent 2003.02.23 /* ¶ÁÈ¡ºÃÓÑÁÐ±í */
                        loadfriend(currentuser.userid);

                        /* Ð´Èëipfile bluetent 2003.01.18 */
                        {
                                FILE *fp;
                                //	time_t now;
                                struct user_enter rduser;

                                //	now=time(0);

                                bzero(&rduser,sizeof(rduser));
                                snprintf(rduser.loginfrom,18,"%s",fromhost);
                                snprintf(rduser.logintime,28,"%s\n",Ctime(time(0)));
                                bzero(ipfile,256);
                                sprintf(ipfile,"home/%c/%s/ipfile",toupper(currentuser.userid[0]),currentuser.userid);
                                if(NULL == (fp=fopen(ipfile,"r")))
                                {
                                        creat(ipfile,0644);
                                }
                                if(NULL == (fp=fopen(ipfile,"r+")))
                                {
                                        return;
                                }
                                fseek(fp,-0L,SEEK_END);
                                fwrite(&rduser,sizeof(rduser),1,fp);
                                fclose(fp) ;
                        }
                       mytruncate(ipfile,sizeof(struct user_enter),120);

                       // printf("%d %s %d",n,u->userid,u->active);
                        return 0;//succeed
                }
        }
        flock(fileno(fp), LOCK_UN);
        fclose(fp);
        return -1;//fail
        //http_fatal("±§Ç¸£¬Ä¿Ç°ÔÚÏßÓÃ»§ÊýÒÑ´ïÉÏÏÞ£¬ÎÞ·¨µÇÂ¼¡£ÇëÉÔºóÔÙÀ´¡£");
}

int check_multi_d(struct userec *x, struct user_info ** u, int kick)
{
	int i, total;
	int uc;
	int opid;
	FILE * fp;
	total = 0;
	if(x->userlevel & PERM_MULTILOG)
	{
		return 0;
	}
	for(i = 0; i < MAXACTIVE; i++)
	{
		if(shm_utmp->uinfo[i].active == 0)
		{
			continue;
		}
		if(!strcasecmp(shm_utmp->uinfo[i].userid, x->userid))
		{
			if (kick == total)
			{
				opid = shm_utmp->uinfo[i].pid;
				if (shm_utmp->uinfo[i].pid && shm_utmp->uinfo[i].mode < 20000 && kill(shm_utmp->uinfo[i].pid, 0) == 0)
				{
					sleep (1);
					if (shm_utmp->uinfo[i].active && shm_utmp->uinfo[i].pid)
					//»¹Ã»ËÀ£¿Ð¡Ñù¶ùµÄ£¬ÆþËÀÄãÑ¾µÄ£¡
					{
						kill(shm_utmp->uinfo[i].pid, 9);
					}
				}
				else
				{
					sleep(1);
					if (shm_utmp->uinfo[i].mode < 20000 && shm_utmp->uinfo[i].active && shm_utmp->uinfo[i].pid)
					{
						kill(shm_utmp->uinfo[i].pid, 9);
					}
				}
				fp = fopen(".UTMP.lock", "a");
				flock(fileno(fp), LOCK_EX);
				if (shm_utmp->uinfo[i].pid == opid)
				{
					shm_utmp->uinfo[i].active = 0;
				}
				flock(fileno(fp), LOCK_UN);
				fclose(fp);
				kick = -1;
				continue;
			}
			if (u)
			{
				u[total] = &shm_utmp->uinfo[i];
			}
			total++;
			if (total >= MULTI_LOGINS)
			{
				return -1;
			}
		}
	}
	return 0;
}

int check_multi(struct userec *x)
{
	return check_multi_d(x, NULL, -1);
}

int mytruncate(char * path , size_t size, int num)
{

  char tmppath[255];
  sprintf(tmppath,"%s.webiptmp",path);
  FILE *fp=fopen(tmppath,"w+");
  FILE *src=fopen(path,"r");
  if(src==NULL || fp==NULL) return -1;

  fseek(src,-size*num,SEEK_END);
  void *buf=(void *)malloc(size);
  while(1){
      bzero(buf,size);
      if(fread(buf,size,1,src)<=0) break;
      fwrite(buf,size,1,fp);
  }
  fclose(src);
  fclose(fp);
  rename(tmppath,path);
  return 0;
}

/* add end */
/* ²éÑ¯ÉÏÏßµØÖ·ÊÇ·ñ±»ÓÃ»§ÉèÖÃ·â½û bluetent 2003.01.15 */
int bad_host(char *name, char *buf2)
{
        FILE   *list;
        char    buf[40], *ptr;
        int allow=0;
        int stat=0;
        if (list = fopen(buf2, "r")) {
                while (fgets(buf, 40, list)) {
                        ptr = strtok(buf, " \n\t\r");
                        if (ptr != NULL && *ptr != '#') {
                                if(!strcmp(ptr, "allow")) {
                                        allow=1;
                                        stat=1;
                                }
                                if(!strcmp(ptr, name))
                                        stat=(allow?0:1);
                                if(ptr[0] == '#')
                                        continue;
                                if(ptr[0] == '-' && !strcmp(name,&ptr[1]))
                                        stat=(allow?1:0);
                                if(ptr[strlen(ptr)-1]=='.'&&!strncmp(ptr,name,strlen(ptr)-1))
                                        stat=(allow?0:1);
                                if(ptr[0]=='.'&&strlen(ptr)<strlen(name)&&
                                                !strcmp(ptr,name+strlen(name)-strlen(ptr)))
                                        stat=(allow?0:1);
                        }
                }
                fclose(list);
        }
        return stat;
}

int headerCookie(const char * key, const char * value)
//±ØÐëÔÚÃ»ÓÐÊä³öÍ·Ö®Ç°
{
	//Set-Cookie: JSESSIONID=12345; HttpOnly
	printf ("Set-Cookie: %s=%s; \n", key, urlencode2(value));
	return 0;
}

int file_append(char *file, void *buf, int len, int lock) 
{
	FILE * fp;
	int r;
	fp = fopen(file, "ab");
	if (!fp)
	{
		fp = fopen(file, "wb");
		if (!fp)
		{
			return -1;
		}
	}
	if (lock)
	{
		flock(fileno(fp), LOCK_EX);
	}
	r = fwrite (buf, 1, len, fp);
	fflush(fp);
	if (lock)
	{
		flock(fileno(fp), LOCK_UN);
	}
	fclose(fp);
	return r;
}

int is_bad_id(char *userid)
{
        FILE   *fp;
        char    buf[STRLEN], ptr2[IDLEN + 2],*ptr, ch;
        ptr = userid;
        while ((ch = *ptr++) != '\0') {
                if (!isalnum(ch) && ch != '_')
                        return 1;
        }
        if( !strcasecmp(userid,BBSID) )
                return 1;
        if ((fp = fopen(".badname", "r")) != NULL) {
                strtolower(ptr2, userid);
                while (fgets(buf, STRLEN, fp) != NULL) {
                        if(!strcmp(buf,"\n")) continue;
                        ptr = strtok(buf, " \n\t\r");
                        strtolower(ptr,ptr); //add by quickmouse
                        if (ptr != NULL && *ptr != '#') {
                                if(  (ptr[0] == '*' && strstr(ptr2, &ptr[1]) != NULL)
                                                ||(ptr[0] != '*' && !strcmp(ptr2,ptr)) ) {
                                        fclose(fp);
                                        return 1;
                                }
                        }
                }
                fclose(fp);
        }
        return 0;
}

int check_uuid(char * uuid, char * userid)
{
	FILE *fp;
	char buf[256];
	struct uuid_st uu;
	int result = 0;
	sprintf(buf, "%s/%s", BBSHOME, UUIDS);
	sleep (2);
	fp = fopen(buf, (*userid) ? "rb+" : "rb");
	if (!fp)
	{
		return 0;
	}
	while (fread(&uu, sizeof(uu), 1, fp))
	{
		if (!strcmp(uu.uuid, uuid))
		{
			if (*uu.reger /*&& strcasecmp(uu.reger, userid)*/)
			{
				fclose(fp);
				return 0;
			}
			if (*userid)
			{
				strncpy(uu.reger, userid, sizeof(uu.reger));
				fseek(fp, -sizeof(uu), 1);
				fwrite(&uu, sizeof(uu), 1, fp);
			}
			result = 1;
			break;
		}
	}
	if (!result && *userid)
	{
		strncpy(uu.reger, userid, sizeof(uu.reger));
		strcpy(uu.owner, "iNankai");
		strncpy(uu.uuid, uuid, sizeof(uu.uuid));
		fseek(fp, 0, 2);
		fwrite(&uu, sizeof(uu), 1, fp);
	}
	fclose(fp);
	return 1;
}

void jsprint(char * srcstr)
{
	char * i;
	register char st=1;//Õû×ÖÅÐ¶Ï:1ÎªÕû×Ö
	for (i=srcstr;*i;i++)
	{
		if (st&&strchr("\\\"'\x0d\x0a",*i))
			printf("\\x%c%c",myto16(((*i)>>4)&0xf),myto16((*i)&0xf));
		else
			printf("%c",*i);
		if ((*i)&0x80)
			st^=1;
		else
			st=1;
	}
}


int js_redirect(char * url, char * tip, int sec)
{
	printf ("<script type=\"text/javascript\" >\n");
	if (tip)
	{
		printf ("document.write('");
		jsprint (tip);
		printf ("');\n");
	}
	if (sec)
	{
		printf ("setTimeout(\"");
	}
	printf ("location.href='");
	jsprint (url);
	printf ("';");
	if (sec)
	{
		printf ("\", %d", sec);
	}
	printf ("\n</script>\n");
	return 0;
}

