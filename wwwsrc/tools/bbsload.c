/*$Id: bbsload.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"
int main()
{
        FILE *fp;
        char path[64]="/home/www/html/image/load";
        char filename[128]="/home/www/html/image/load/today";

        init_all();
        printf("<font color=336699 ><center>%sϵͳ����ͳ��<hr ></font><br>\n",BBSNAME);
        fp=fopen(filename,"r");
        if(fp==0) {
                printf("�ļ�������,�������Ա��ϵ");
                http_quit();
        }

        fscanf(fp,"%s",filename);
        printf("<img  src=/image/load/%s>",filename);
        printf("<br><br>��ǰ��¼");




        DIR *dp;
        struct dirent *entry;
        dp=opendir(path);
        if(dp==NULL) {
                printf("���ܴ�·��,�������Ա��ϵ");
                http_quit();
        }
        //   chdir(path);
        entry=readdir(dp);
        entry=readdir(dp);
        entry=readdir(dp);
        // entry=readdir(dp);
        int i=0;
        while(  entry!=NULL  ) {
                if(i%4==0)
                        printf("<br>");
                i++;
                //  if(strcmp("today",entry->d_name)==0)
                //  continue;

                printf("<a href=/image/load/%s>",entry->d_name);
                //  int i= subload(entry->d_name,'.');
                printf("&nbsp&nbsp%s</a>",entry->d_name);
                entry=readdir(dp);
        }
        http_quit();
}




int  subload(char *s,char ch)

{
        int i;
        for(i=0;i<strlen(s);i++) {
                if (*s==ch)
                        break;
        }
        *s=0;
        return 0;
}
