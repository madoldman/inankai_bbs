/* $Id: allpost.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#define SILENCE
#include "BBSLIB.inc"
#define BS 512
char enname[32][32];
char cnname[32][32];
int newnum=0;

int  allboards()
{
        FILE *fp,*fp2;
        int i, j,n;
        char buf[256], tmp[80], name[80], cname[80],sig;
        int s=0;
        printf("<br><b><i>推荐文章</i></b><br>");
        fp=fopen("0Announce/bbslist/mactodayboard2", "r");
        if(fp==NULL)
                return 1;


        struct rmd r;
        for(i=0; i<SECNUM; i++) {
                switch(i) {
                case 10:
                        sig='C';
                        break;
                case 11:
                        sig='G';
                        break;
                case 12:
                        sig='N';
                        break;
                case 13:
                        sig='F';
                        break;
                default:
                        sig=i+'0';
                }

                printf("<br><center><table width=100%>\n");

                printf("<tr><td><b><a href=rmd?mode=2&&group=%d><font color=blue>%c区导读</font></a>&nbsp;<a href=bbsboa?%d><font color=blue>%s</font></b></a></td></tr><tr><td><table><tr>",i,sig,i,secname[i][0]);

                newnum=0;n=0;
                fgets(buf, 150, fp);
                rewind(fp);
                for(j=0; j<MAXBOARD; j++) {
                        if(fgets(buf, 150, fp)<=0)
                                break;
                        newnum=sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp, cname, tmp);
                        if(newnum==6&&isthiskind(name, i)) {
                        sprintf(buf, "boards/%s/.thesis", name);
                        fp2=fopen(buf,"r");
                        if(fp2==NULL)
                                continue;
                        while(1) {
                                if(fread(&r, sizeof(struct rmd), 1, fp2)<=0)
                                        break;
                                printf("<td><a href=\"newcon?board=%s&file=%s&mode=3\"><font color=navy>%46.46s</font></a><a href=bbsqry?userid=%s> <font color=green>%s</font></a> <a href=bbsdoc?board=%s>%s</a></td>",name, r.en,r.cn,r.owner,r.owner,name,name);
                                n++;
                                if(n%2==0)
                                        printf("</tr><tr>");
                        }
                        fclose(fp2);
                  }
                }//for每区
                printf("</tr></table></td></tr></table></center>");
        }        
        fcloseall();
        return 0;
}


/*  判断这个板面是不是位于对应区 */
int isthiskind(char *board, int sec)
{
        struct shortfile *x;
        int i;
        for(i=0; i<MAXBOARD; i++) {
                x=&(shm_bcache->bcache[i]);
                if(!strcmp(x->filename, board))
                        break;
        }
        if(x->filename[0]<=32 || x->filename[0]>'z')
                return 0;
        if(!has_read_perm(&currentuser, x->filename))
                return 0;
        if(!strchr(seccode[sec], x->title[0]))
                return 0;
        return 1;
}


int main()
{
        init_all();
        allboards();
}
