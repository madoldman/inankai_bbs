#define SILENCE
#include "BBSLIB.inc"
#include "boardrc.inc"
#define MAILTO  "flypigs"
#define  N    20

struct shortfile *bd;
struct fileheader fh[N];
int num=0;
struct fileheader x;
char buf[256];

char bigbuf[1024];
char * xmlsafestr(char *s)
{
        bzero(bigbuf,1024);
        char *p=s;
        while(*p) {
                switch(*p) {

                case '<':
                        strcat(bigbuf,"&lt;");
                        break;
                case '>':
                        strcat(bigbuf,"&gt;");
                        break;
                case '&':
                        strcat(bigbuf,"&amp;");
                        break;
                case '"':
                        strcat(bigbuf,"&quot;");
                        break;

                case '\'':
                        strcat(bigbuf,"&apos;");
                        break;
                default:
                        sprintf(bigbuf,"%s%c",bigbuf,*p);
                }
                p++;

        }


        return bigbuf;

}

char * url(struct fileheader *p)
{
        bzero(buf,256);
        sprintf(buf,"http://%s/cgi-bin/bbs/newcon?board=%s&file=%s&mode=3",BBSHOST,bd->filename,p->filename);
        return xmlsafestr(buf);
}




int rdfheader()
{
printf("<?xml version=\"1.0\" encoding=\"gb18030\"?>\
<rdf:RDF\
xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\
xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\
xmlns:sy=\"http://purl.org/rss/1.0/modules/syndication/\"\
xmlns=\"http://purl.org/rss/1.0/\"\
xmlns:admin=\"http://webns.net/mvcb/\"\
xmlns:annotate=\"http://purl.org/rss/1.0/modules/annotate/\"\
xmlns:dcterms=\"http://purl.org/dc/terms/\"\
xmlns:cc=\"http://web.resource.org/cc/\"\
xmlns:content=\"http://purl.org/rss/1.0/modules/content/\"\
xmlns:foaf=\"http://xmlns.com/foaf/0.1/\"\
xmlns:trackback=\"http://madskills.com/public/xml/rss/module/trackback/\">\n");
}

int channelstart()
{
        dcdate(time(0));
        printf("<channel rdf:about=\"http://%s/\">\
<title>%s-%s(%s)</title>\
<link>http://%s/cgi-bin/bbs/bbsdoc?board=%s</link>\
\
<image rdf:resource=\"http://%s/logo88x31.gif\" />\
<description></description>\
<dc:language>zh-cn</dc:language>\
<dc:creator>%s</dc:creator>\
<dc:date>%s</dc:date>\
\
<admin:generatorAgent rdf:resource=\"http://%s/rss\" />\
<admin:errorReportsTo rdf:resource=\"mailto:%s@%s\"/>\
\
<cc:license rdf:resource=\"http://creativecommons.org/licenses/by-nd-nc/1.0/\" />\n",
               BBSHOST,BBSNAME,bd->filename,bd->title+11,
               BBSHOST,bd->filename,
               BBSHOST,
               BBSHOST,
               datestring,BBSHOST,MAILTO,BBSHOST
              );

}

int channelclose()
{
        printf("</channel>\n");
}

int image()
{
        printf("<image rdf:about=\"http://%s/logo88x31.gif\">\
<title>%s</title>\
<link>http://%s/</link>\
<url>http://%s/logo88x31.gif</url>\
</image>\n",BBSHOST,BBSNAME,BBSHOST,BBSHOST);
}

int items()
{
        int size=sizeof(struct fileheader),i;
        bzero(&fh,N*size);
        char filepath[256];
        sprintf(filepath, "boards/%s/.DIR", bd->filename);
        FILE *fp=fopen(filepath, "r");
        if(fp==0)
                return -1;

        int total=file_size(filepath)/size,j=0;

        i=0;
        fseek(fp, -size, SEEK_END);
        for(j=0;j<total;j++) {
                fread(&x,size,1,fp);
                fseek(fp, -2*size, SEEK_CUR);
                if(!strncmp(x.title, "Re: ", 4))
                        continue;
                memcpy(&fh[i],&x,size);
                if(++i==N)
                        break;
        }

        printf("<items>\n<rdf:Seq>\n");
        for(j=0;j<i;j++)
                printf("<rdf:li rdf:resource=\"%s\" />\n",url(&fh[j]));
        printf("</rdf:Seq>\n</items>\n");
        num=i;
        return 0;
}


int rdfclose()
{
        printf("</rdf:RDF>\n");
}

/*
mode=0 text
mode=1 html
*/
int morefile(char *file ,int mode)
{
        int i;
        char path[256];
        sprintf(path, "boards/%s/%s", bd->filename, file);
        FILE *fp=fopen(path, "r");
        if(fp==0)
                return 0;
        if(mode==1){
       	     while(1){
                if(fgets(buf, 256, fp)==0)   break;
                hhprintf("%s", noansi(buf));
                printf("<br />");
             }
        }else{
          for(i=0;i<8;i++){
              if(fgets(buf, 256, fp)==0) break; 
              if(!strcmp(buf,"--\n")) break;
              if(i>3){
                 printf("%s",xmlsafestr(noansi(buf)));
                 printf("<br />");
             }
          }
        }

        out:
        fclose(fp);
        return 0;
}

int dcdate(time_t t)
{
	struct tm *tm;
	tm = localtime(&t);
	sprintf(datestring,"%4d-%02d-%02dT%02d:%02d:%02d+08:00",
		tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
		tm->tm_hour,tm->tm_min,tm->tm_sec);
}

int content()
{
        int j;
        for(j=0;j<num;j++) {
                printf("<item rdf:about=\"%s\">\n",url(&fh[j]));
                printf("<title>%s</title>\n",xmlsafestr(fh[j].title));
                printf("<link>%s</link>\n",url(&fh[j]));
                printf("<description>\n");
                morefile(fh[j].filename,0);
                printf("\n</description>\n");

                dcdate(atoi(fh[j].filename+2));
                printf("<dc:creator>%s</dc:creator>\n<dc:date>%s</dc:date>\n",fh[j].owner,datestring);

                printf("<content:encoded>\n<![CDATA[");
                morefile(fh[j].filename,1);
                printf("]]>\n</content:encoded></item>\n");
        }

}

int error(char *s)
{
        printf("<?xml version=\"1.0\" encoding=\"gb18030\" ?>\
<response>\
<error>1</error>\
<message>%s</message>\
</response>\n",s);
       exit(0);
}


int main()
{
        NOTADDCSSFILE=1;
        char id[STRLEN];

        init_all();

        printf("Content-type: text/html; charset=gb18030\n\n" );

        strsncpy(id, getparm("board"), 32);
        bd=getbcache(id);
        if(bd==0)
                error("The board does't exsits.");
        if(!has_read_perm(&currentuser, id))
                error("Not previldge to read the board .");
        rdfheader();
        channelstart();
        items();
        channelclose();
        image();

        content();

        rdfclose();
}
