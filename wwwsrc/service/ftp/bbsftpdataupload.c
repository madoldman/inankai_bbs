/*$Id: bbsftpdataupload.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "ftpsearch.h"
int main()
{
        FILE *fp;
        char file[80], tmpfilename[80], target[256];
        char *buf, *p, *filename, buf1[256], ID[280], tmp[256];
        int n, m, i, nn=0;
        chdir(BBSHOME);
        printf("Content-type: text/html; charset=gb2312\n\n");
        n=atoi(getsenv("CONTENT_LENGTH"));
        strsncpy(fromhost, getsenv("REMOTE_ADDR"), 32);
        if(n<0)
                n=0;
        buf=malloc(n);
        if(buf==0)
                http_fatal("bbsupload: out of memory");
        fgets(ID, 200, stdin);
        fgets(buf1, 256, stdin);
        nn+=strlen(ID)+strlen(buf1);
        filename=strcasestr(buf1, "filename=\"");
        if(strstr(filename, "\"\""))
                http_fatal("请输入文件名");
        while(1) {
                p=strchr(filename, '\\');
                if(!p)
                        break;
                filename=p+1;
        }
        p=strstr(filename, "\"");
        if(p)
                p[0]=0;
        fgets(buf1, 256, stdin);
        nn+=strlen(buf1);
        fgets(buf1, 256, stdin);
        nn+=strlen(buf1);
        n-=nn;
        if(n<0)
                n=0;
        m=fread(buf, 1, n, stdin);
        buf[m]=0;
        if(m>=0) {
                sprintf(tmpfilename, "%s/%s", LISTDIR, fromhost);
                fp=fopen(tmpfilename, "w");
                fwrite(buf, 1, m-50, fp);
                fclose(fp);
                printf("资料上传完毕.");
                sprintf(tmp, "UPDATE %d bytes", m-50);
                ftplog(ADMINLOG, tmp, fromhost);
                printf("<p>您刚刚上传的数据将在1小时内更新至服务器端.");
                printf("<p><a href=bbsmyftp>完成</a>");
        }
        http_quit();
}
int memstr(char *s, int len, char *s2)
{
        int i, len2=strlen(s2);
        for(i=0; i<=len-len2; i++)
                if(!strncmp(s+i, s2, len2))
                        return i;
        return -1;
}
