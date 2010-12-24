/* $Id: bbsmain.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"
#define BS 512
char enname[32][32];
char cnname[32][32];
int newnum=0;

int  recommanded()
{
        FILE *fp=fopen (PATHRMD,"r");
        if(fp==NULL)
                return 1;
        
        printf("<table><tr><td><b><i>�����Ƽ�</b></i></td></tr>");
        
        struct rmd r;
        while(1) {
                if(fread(&r, sizeof(struct rmd), 1, fp)<=0)
                        break;
                if(r.sign==1)
                        printf("<tr><td>��<a href=newcon?board=%s&file=%s&mode=3>%s</a>  <a href=bbsdoc?board=%s>|%s|</a> </tr></td>",r.board,r.en,r.cn,                  /*  r.owner,r.owner,*/ r.board,r.board);
        }//while
        printf("<tr><td align=right><a href=rmd?mode=0>���ྫ���Ƽ�...</a></td></tr>");
        printf("</table>");
        fclose(fp);
        return 0;
}

int newboard()
{
        int i;
        struct shortfile *x;
        for(i=0; i<MAXBOARD; i++) {
                x=&(shm_bcache->bcache[i]);
                if(x->filename[0]<=32 || x->filename[0]>'z') continue;
                if(!strchr(seccode[12], x->title[0]))   continue;
                if(!has_read_perm(&currentuser, x->filename)) continue;
                strcpy(enname[newnum],x->filename);
                strcpy(cnname[newnum++],x->title+11);
        }

        if(newnum) {
                int i=0;
                printf("<table cellpadding=2 cellspacing=2 align=center width=80%  ><tr><td><font color=red><b>�Կ�����</b></font></td><td>");
                for(i=0;i<newnum;i++) {
                        printf("<a href=bbsdoc?board=%s>%s</a>\n",enname[i],cnname[i]);
                }
                printf("</td></tr></table>");
        }
}

int showboard(char *name,char *path)
{
        char en[STRLEN],cn[STRLEN],buf[STRLEN];
        int r=0;
        int i=0;
        FILE *fp=fopen(path,"r");
        if(fp==NULL)
                return 1;
        printf("<table cellpadding=2 cellspacing=2 border=1>");
        printf("<tr><td align=center><i><b>%s</b></i></td></tr><tr><td>",name);
        while (fgets(buf,BS,fp)!=NULL) {
                if(strcmp(trim(buf),"")) {
                        r=sscanf(buf, "%s %s",en,cn);
                        if(r==2)
                                printf("<a href=bbsdoc?board=%s>%s</a>\n",en,cn);
                }
        }
        printf("</td></tr></table>");
        fclose(fp);
}

//huangxu@060417:+ף��
void trimend(char* src,int lenth)
{
	char * p;
	for (p=src+lenth;*p==0x20||(!*p);p--)
		*p=0;
	return;
}

int hottopics(char *path)
{
        FILE *fp;
        int n;
        char s1[256];
        char brd[256],id[16], title[256];
        char title_in_url[240];
        fp=fopen(path, "r");
        if(fp==NULL)
                return 1;
        fgets(s1, 255, fp);
        fgets(s1, 255, fp);
   
        printf("<center><b><font size=5>���Ż����ӵ㲥��</font></b><br>");
        printf("<table cellpadding=1 cellspacing=0 border=0 width=80% >");
        for(n=1; n<=10; n++) {
                if(fgets(s1, 255, fp)<=0)
                        break;
                sscanf(s1+45, "%s", brd);
                sscanf(s1+122, "%s", id);
                if(fgets(s1, 255, fp)<=0)
                        break;
                strsncpy(title, s1+27, 60);
                trimend(title,60);
                kill_quotation_mark(title_in_url,title);
                printf("<p>��<a href='newcon?board=%s&title=%s'>%s</a> <a href=bbsqry?userid=%s><font color=green>%s </font></a><a href=bbs%sdoc?board=%s><font color=gray>%s</font></a></p> ",brd,title_in_url,title,id,id,(currentuser.userdefine & DEF_THESIS)?"t":"",brd,brd);
        }
        printf("</table>");
        fclose(fp);
        fp=fopen("etc/bless/day", "r");
        if(fp==NULL)
                return 1;
        fgets(s1, 255, fp);
        fgets(s1, 255, fp);
        printf("<p>&nbsp;</p><p>&nbsp;</p>");
        printf("<center><b><font size=5>����ʮ������ף��</font></b><br>");
        printf("<p align=center><a href=bbstdoc?board=Blessing><font color=red>��ӭ����Blessing��������Ը</font></a></p>");
        printf("<table cellpadding=1 cellspacing=0 border=0 width=80% >");
        for(n=1; n<=10; n++) {
                if(fgets(s1, 255, fp)<=0)
                        break;
                sscanf(s1+45, "%s", brd);
                sscanf(s1+122, "%s", id);
                if(fgets(s1, 255, fp)<=0)
                        break;
                strsncpy(title, s1+27, 60);
                trimend(title,60);
                kill_quotation_mark(title_in_url,title);
                printf("<p>��<a href='newcon?board=Blessing&title=%s'>%s</a> <a href=bbsqry?userid=%s><font color=green>%s </font></a></p> ",title_in_url,title,id,id,(currentuser.userdefine & DEF_THESIS)?"t":"");
        }
        printf("</table>");
        fclose(fp);

        return 0;
}

int  allboards()
{
        FILE *fp,*fp2;
        int i, j,n;
        char buf[256], tmp[80], name[80], cname[80],sig;
        int s=0;
        printf("<br><b><i>�Ƽ�����</i></b><br>");
        fp=fopen("0Announce/bbslist/mactodayboard2", "r");
        if(fp==NULL) 
                return 1;


        struct rmd r;
        for(i=0; i<SECNUM; i++) {
                switch(i) {
                case 10:
                        sig='U';
                        break;
                case 11:
                        sig='O';
                        break;
                case 12:
                        sig='G';
                        break;
                case 13:
                        sig='N';
                        break;
		case 14:
			sig='Z';
			break;
                default:
                        sig=i+'0';
                }

                printf("<br><center><table width=100%>\n");

                printf("<tr><td><b><a href=rmd?mode=2&&group=%d><font color=blue>%c������</font></a>&nbsp;<a href=bbsboa?%d><font color=blue>%s</font></b></a></td></tr><tr><td><table><tr>",i,sig,i,secname[i][0]);

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
                                printf("<td><a href=\"newcon?board=%s&file=%s&mode=3\"><font color=navy>%46.46s</font></a><a href=bbsqry?userid=%s>����:<font color=green>%s</font></a>����:<a href=bbsdoc?board=%s>%s</a></td>",name, r.en,r.cn,r.owner,r.owner,name,name);
                                n++;
                                if(n%2==0)
                                        printf("</tr><tr>");
                        }
                        fclose(fp2);
                  }
                }//forÿ��
                printf("</tr></table></td></tr></table></center>");
        }        
        fcloseall();
        return 0;
}

int allboardbuf()
{
   char buf[BS];
   FILE *fp=fopen("etc/allboardbuf","r");
   if(fp==NULL)
           return -1;
   while (1){ 
       bzero(buf,BS);
       if(fgets(buf,BS,fp)<=0) break;
       printf("%s",buf);   
    }
   fclose(fp);

}


int  announce(char *path)
{
        char buf[BS];
        FILE *fp=fopen(path,"r");
        if(fp==NULL)
                return -1;

        int i=0;
        int begin=0;
        char *p;
        while (fgets(buf,BS,fp)>0) {
                if(i==0) {
                        printf("<table><b><i>�����ȵ�</i></b><br>");
                        i=1;
                }
                p=trim(buf);
                if(strcmp(p,"")){
                        /*if(strstr(p,"----")){ 
				begin=1;
				printf("<br>");
				continue;
			}
                        if(begin)*/
                            printf("<tr><td>%s</td></tr>\n",p);
                }
        }
        printf("</table>");
        fclose(fp);
}

/*  �ж���������ǲ���λ�ڶ�Ӧ�� */
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


int showright()
{
        printf("<p><center><b> %s %s  2006</b>\
               <hr color=%s width=80%>\
               </center>\
               </p>",BBSNAME,BBSHOST, mytheme.con);
}

int showlink()
{
        printf("<a href=\"/cgi-bin/bbs/bbsdoc?board=GoodArticle\">|����ԭ��|</a>\
        <a href=\"http://www.nankai.edu.cn/\" target=_blank>|�Ͽ���ѧ|</a>\
		  <a href=\"http://news.nankai.edu.cn/\" target=_blank>|����|</a>\
               <a href=\"http://jw.nankai.edu.cn\" target=_blank>|����|</a>\
               <a href=\"http://202.113.20.150/\" target=_blank>|ͼ���|</a>\
               <a href=\"http://202.113.16.61/\" target=_blank>|�����й�|</a>\
               <a href=\"http://movie.nankai.edu.cn/\" target=_blank>|Ӱ��|</a>\
               <a href=\"http://soft.nankai.edu.cn/\" target=_blank>|���|</a>\
	         <a href=\"http://music.nankai.edu.cn/\" target=_blank>|����|</a>\
               <a href=\"/cgi-bin/bbs/bbs0an?path=/weather\">|����|</a>\
               <a href=\"/cgi-bin/bbs/bbsnot?board=NetMedia\">|��̨|</a>\
               <a href=\"/bbsnet.html\">|��������|</a>\
               <a href=\"/cgi-bin/bbs/bbs0an?path=/abbs/n\">|����|</a>");
}

int shownewcomer(char *path)
{
        struct fileheader x[10];
        FILE *fp=fopen(path,"r");
        if(fp==NULL)
                return 1;
        int total=file_size(path)/sizeof(struct fileheader);
        int i=0;
        if(total<10)
                i=0;
        else
                i=total-10;
        printf("<a href=bbsdoc?board=newcomers><b><i>����ע��ID</i></b></a><br>");
        fseek(fp, sizeof(struct fileheader)*i, SEEK_SET);
        if(fread(&x, sizeof(struct fileheader),10, fp)>0) {
                for(i=9;i>=0;i--)
                        printf("<a href=bbsqry?userid=%s> %s<%5.5s> </a>\n",x[i].owner, x[i].owner,Ctime(atoi(x[i].filename+2))+11);
        }
        fclose(fp);
}

int main()
{
        init_all();
        int i=atoi(getsenv("QUERY_STRING"));
		if(i==10){
		   hottopics("etc/posts/day");
		   return 0;
		}
        printf("<body background=\"%s\" bgproperties=fixed leftmargin=10 topmargin=3>",mytheme.bgpath);
        showlink();
        printf("<table width=100% border=0><tr><td valign=top width=50%>");
        //hottopics("etc/posts/day");
        recommanded();
        printf("</td><td valign=top>");
        announce("etc/wwwannounce");
        printf("</td></tr></table>");
      
        newboard();
        printf("<table width=90% border=0 align=center><tr><td valign=top>");     
        showboard("����������",HOTBOARDS);
        printf("</td><td valign=top>");
        //showboard("��Ʒ������",RMDBOARDS);
 	  showboard("�Ƽ�������",RMDBOARDS);
        printf("</td></tr></table>");
        allboardbuf();
       // shownewcomer("boards/newcomers/.DIR");
        printf("<p>��ӭ����!���������ʹ�ñ�վ�����Ǻ���Ϥ,��������ȥ<a href=bbsdoc?board=BBSHelp>BBSHelp</a>�濴��.�������ʹ�ù����������˽�����˵�����,����Ե�<a  href=bbsdoc?board=sysop>SYSOP</a>��ӳ.���ǻ��������ٶȸ�������Ĵ�.\
               <br>\
               <b>��������,���󸽼�����,�շ��ʼ�,���Ի�����趨,�����ļ���������</b>�ȵ�,<b>ͨ��ע��</b>,��Ϊ��վ��ʽ�û�,����ӵ��!һ�о���ô��!\
               </p>");

        showright();
        http_quit();
}
