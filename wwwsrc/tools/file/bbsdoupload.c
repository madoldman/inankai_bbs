/*$Id: bbsdoupload.c,v 1.5 2009-12-29 06:41:52 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "filemgr.inc"

int memstr(char *s, int len, char *s2)
{
        int i, len2=strlen(s2);
        for(i=0; i<=len-len2; i++)
                if(!strncmp(s+i, s2, len2))
                        return i;
        return -1;
}
int badfilename(char *s)
{

		char *p=s;
		while(*p){
          if(*p>='A'&&*p<='Z')
                 *p+=32;
          p++;
        }

		char *badname[]={".php",".pl",".phtml","..",".exe",".mp3",".wma"};
        int i=0,j=0;
        for(;i<sizeof(badname) / sizeof(*badname);i++)
                if(strstr(s, badname[i]))
                        return 1;
        
        for(i=0;s[i]; i++) {
                if( s[i] ==' ' )
                        continue;
                s[j++]=s[i];
        }
        s[j]=0;
        return 0;
}
int main()
{
        int m, n, nn=0, i;
        char *buf, *p, *filename, buf1[256], ID[280];

        FILE *fp;
        int autoresize=0;

        int maxsize=INT_SYSMAXUPFILESIZE+2056;//20M

        chdir(BBSHOME);
        printf("Content-type: text/html; charset=gb18030\n\n");
        //huangxu@061229
        if(file_exist("/home/www/html/news/noupload.html"))
        {
        	redirect("/news/noupload.html");
        }

 	char board[STRLEN];
        strncpy(board,getsenv("QUERY_STRING"),32);
        if((p=strchr(board,'*')) && p!=board)
        {
        	*p=0;
        	autoresize=1;
        }
        if(*board && *board != '*'){
        struct shortfile *x1;
        shm_init();
        x1=getbcache(board);
        if(x1==0)   http_fatal("错误的讨论区");
        }
 
        n=atoi(getsenv("CONTENT_LENGTH"));

        if(n<0)
                n=0;
        if(n>maxsize)
                n=maxsize;
        buf=malloc(n);
        if(buf==0)
                http_fatal("bbsupload: out of memory");
        fgets(ID, 200, stdin);
        fgets(buf1, 256, stdin);
        nn+=strlen(ID)+strlen(buf1);
        //filename=strcasestr(buf1, "filename=\"");
        filename=strstr (buf1, "filename=\"");
        if(filename==0)
                http_fatal("bbsupload: filename unknown");
        filename+=10;
        while(1) {
                p=strchr(filename, '\\');
                if(!p)
                        break;
                filename=p+1;
        }
        while(1) {
                p = strchr (filename, '/');
                if (!p)
                        break;
                filename=p +1;
        }
        p=strstr(filename, "\"");
        if (p)
                p [0] = 0;
        //huangxu@0610119:filename no ()
        for (p=filename;*p;p++)
        {
        	if (strchr("()[]{}/\\<>|`~!@#$%^&*?\'\";:",*p))

        		*p='_';
        }
        if(badfilename(filename)) {
                http_fatal("本站不支持上传.php,.pl文件,谢谢合作!");
                return 1;
        }
        fgets(buf1, 256, stdin);
        nn+=strlen(buf1);
        fgets(buf1, 256, stdin);
        nn+=strlen(buf1);
        n-=nn;
        if(n<0)
                n=0;
        m=fread(buf, 1, n, stdin);
        buf[m]=0;
        i=memstr(buf, m, ID);
        if(i>=0) {
                int ii;
                char tmpfilename[100], tmp2[100];
                sprintf(tmpfilename, "tmp/%d.upload", getpid());

                fp=fopen(tmpfilename, "w");
                fwrite(buf, 1, i-2, fp);
                fclose(fp);
               // if(autoresize&&((strstr(filename,".bmp"))||(strstr(filename,".jpg"))||
               // 	(strstr(filename,".jpeg"))||(strstr(filename,"png"))))//gif不做了，有多帧问题……
               // 	sprintf(buf,"/js/php/picresize.php?tmpfile=%d&filename=%s&board=%s",getpid(),urlencode(filename),board);
               // else
                	sprintf(buf,"bbsupload?tmpfile=%d&filename=%s&board=%s",getpid(),urlencode(filename),board);
                redirect(buf);
        } else {
                http_fatal("upload format error");
        }
}
