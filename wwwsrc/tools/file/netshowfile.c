/*$Id: netshowfile.c,v 1.2 2008-12-19 09:55:10 madoldman Exp $
*/

#define SILENCE
#include "BBSLIB.inc"
static int time1;
int main()
{
        FILE *fp;
        struct dir x;
        char name[256], redirbuf[256];
        time1=times(0);
        init_all();
        strsncpy(name, getparm("name"), 40);

        fp=fopen(NETSAVE_PATH"/.DIR", "r+");
        if(fp==0) {
                cgi_head();
                http_fatal("文件不存在1");
        }
        fseek(fp, -1000*sizeof(x), SEEK_END);
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)==0)
                        break;
                if(!strcmp(x.showname, name)) {
                        x.click++;
                        fseek(fp, -1*sizeof(x), SEEK_CUR);
                        fwrite(&x, sizeof(x), 1, fp);
                        fclose(fp);
                        cat(x.filename);
                        exit(0);
                }
        }
        rewind(fp);
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)==0)
                        break;
                if(!strcmp(x.showname, name)) {
                        x.click++;
                        fseek(fp, -1*sizeof(x), SEEK_CUR);
                        fwrite(&x, sizeof(x), 1, fp);
                        fclose(fp);
                        cat(x.filename);
                        exit(0);
                }
        }
        cgi_head();
        printf("name=%s\n", name);
        http_fatal("文件不存在3");
}

int cat(int fid)
{
        char file[80], buf[4000];
        FILE *fp;
        int r;
        sprintf(file, NETSAVE_PATH"/%d", fid);
        fp=fopen(file, "r");
        if(fp==0) {
                cgi_head();
                http_fatal("文件不存在(error_code 4)");
        }
        printf("Content-type: application/octet-stream;\n\n");
        while(1) {
                r=fread(buf, 1, 4000, fp);
                if(r<=0)
                        break;
                fwrite(buf, 1, r, stdout);
        }
}
