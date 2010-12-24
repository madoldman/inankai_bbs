/* ��������������ļ� 
   KCN 2001.3.21
*/


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef AIX
# include <sys/mode.h>
#else
# ifdef LINUX
#  include <stdlib.h>
# endif /*LINUX*/
#endif /*AIX*/
#include <errno.h>
#include <time.h>

#define DOTNAMES ".Names"
#define INDEXHTML "index.htm"
#define HEADER "�Ұ��Ͽ�BBSվ�þ�����"
#define FOOTER "�Ұ��Ͽ�BBSվ�þ�����"
#define MAXPATH    1024 
#define MAXLINELEN 1024
int  iscorpus=0;
char pbuf[80];
#ifdef AIX
#define GNUTAR "tar"
#else
//#define GNUTAR "/usr/local/bin/tar"
#define GNUTAR "tar"
#endif

typedef struct __tagDIR_DATA {
  char dir[MAXPATH];
  struct __tagDIR_DATA* next;
} DIR_DATA,*LP_DIR_DATA;

LP_DIR_DATA task_head,task_tail;
char WorkDir[MAXPATH]="/home/bbs/tmp";
char OutDir[MAXPATH]="/backup/digest";
extern int errno;

char prevprevHtml[MAXPATH];
char prevHtml[MAXPATH];
char nextHtml[MAXPATH];
FILE *pdstFile;
FILE *chmcontentFile;

int DealParameters(int argc,char** argv)
{
  extern char *optarg;
  extern int optind, opterr, optopt;

  char c;
  int flag=1;
  chmcontentFile=NULL;
  while (flag) {
    int this_option_optind = optind ? optind : 1;
    int option_index;
    
    c = getopt(argc, argv, "w:d:o:c:");
    if (c==-1) break;
    switch (c)
    {
      case 'd':
        printf("change to directory %s\n",optarg);
        chdir(optarg);
        break;
      case 'w':
        printf("Set Work Directory:%s\n",optarg);
        strncpy(WorkDir,optarg,MAXPATH-1);
        break;
      case 'o':
	printf("Set OutputDirectory:%s\n",optarg);
	strncpy(OutDir,optarg,MAXPATH-1);
	if (OutDir[strlen(OutDir)-1]=='/')
		OutDir[strlen(OutDir)-1]=0;
	break;
      case 'c':
        if (chmcontentFile) {
	    printf("Too many -c!\n");
	    return -1;
        }
        chmcontentFile=fopen(optarg,"wt");
        if (!chmcontentFile) {
            printf("Can't open %s:%s",optarg,strerror(errno));
            return -1;
        }
	printf("Set CHM content file:%s\n",optarg);
        break;
      default:
	flag= 0 ;
    }
  }
  
  for ( ; optind < argc; optind++)
  {
    char* p;

    if (task_head) {
      printf("too many argument!\n");
      return -1;
    }
    task_head = (LP_DIR_DATA)malloc(sizeof(DIR_DATA));
 
    p = strrchr(argv[optind],'/');
    if (p)
      if (*(p+1)=='/') {
        *p=0;
        p = strrchr(argv[optind],'/');
      };
    if (p) {
      *p=0;
      chdir(argv[optind]);
      if(strstr(argv[optind],"PersonalCorpus"))
 		iscorpus=1;
      printf("change to Directory:%s\n",argv[optind]);
      p++;
    }
    else 
      p=argv[optind];

    strncpy(task_head->dir,p,MAXPATH-1);
    task_head->next = NULL;
  }
  if (!task_head) {
    printf("usage:%s -o out_directory -d announce_dir directory_need_deal_with\n",argv[0]);
    return -1;
  }
  return 0;
}

char* DealLink(char* directory,char* Link,int index,int* isDir,char* date,char* title)
{
  static char filename[MAXPATH];
  struct stat st;
  struct tm *tmstruct;
  
  sprintf(filename,"%s/%s",directory,Link);
  if (lstat(filename,&st)==-1) {
    printf("Directory or File %s is not exist!\n",filename);
    return NULL;
  }


  tmstruct = localtime(&st.st_mtime);
  sprintf(date,"%04d.%02d.%02d",tmstruct->tm_year+1900,tmstruct->tm_mon+1,tmstruct->tm_mday);
  if (S_ISDIR(st.st_mode)) {
    LP_DIR_DATA data=(LP_DIR_DATA)malloc(sizeof(DIR_DATA));
    data->next = NULL;
    strncpy(data->dir,filename,MAXPATH);
    if (task_tail) {
      task_tail->next=data;
      task_tail = data;
    } else {
      task_head = data;
      task_tail = data;
    }

    sprintf(filename,"%s/index.htm",Link);
    *isDir = 1;
    return filename;
  } else {
  /* ����һ��ľ������ļ� */
    FILE *psrcFile;
    FILE *pBBSFile;
    int i,j,k;

    char srcLine[MAXLINELEN] , dstLine[MAXLINELEN*20];
    char Buf2[MAXLINELEN*4] = "";
    char* ptr;
  
    if (NULL == (psrcFile = fopen(filename, "rt")))
    {
      printf("Unexpected error: Can not open file \"%s\"\n", filename);
      return NULL;
    }

    sprintf(filename,"%s/%s/%08d.htm",WorkDir,directory,index);
    
    if (pdstFile) {
      /* �ر���һ��HTML�ļ� */
/*      fputs("</CENTER>\n",pdstFile);*/
      if (prevprevHtml[0]) {
        fprintf(pdstFile,"<A HREF=\"%s\">��һƪ</A>\n",prevprevHtml);
      }
      fputs("<A HREF='javascript:history.go(-1)'>������һҳ</A>\n",pdstFile);
      fputs("<A HREF=\"index.htm\">�ص�Ŀ¼</A>\n",pdstFile);
      fputs("<A HREF=\"#top\">�ص�ҳ��</A>\n",pdstFile);
      fprintf(pdstFile,"<A HREF=\"%08d.htm\">��һƪ</A>\n",index);
      fputs("</H1></CENTER>\n", pdstFile);
      fputs("<CENTER><H1>", pdstFile);
     
      if(!iscorpus)
           fputs(FOOTER, pdstFile);

      //fputs(FOOTER, pdstFile);
      fputs("</H1></CENTER>\n", pdstFile);
      fputs("</BODY></HTML>", pdstFile);
      fclose(pdstFile);
      pdstFile=NULL;
    }
    if (NULL == (pdstFile = fopen(filename, "wt")))
    {
      printf("Unexpected error: Can not write file \"%s\"\n", filename);
      fclose(pdstFile);
      return NULL;
    }                                 
  
    fputs("<HTML>\n<HEAD>\n<TITLE>", pdstFile);
    fputs(title, pdstFile);
    fputs("</TITLE>\n</HEAD>\n<BODY>\n<CENTER><H1>", pdstFile);        
    if(iscorpus)
	fputs(pbuf,pdstFile);
    else
	fputs(HEADER, pdstFile);
    
    fputs("</H1></CENTER>\n", pdstFile);        
    fputs("<A Name=\"top\"></a>\n", pdstFile);        
/*    fputs("<CENTER>\n",pdstFile); */
  
    sprintf(filename,"%s/bbs/%s/%s",WorkDir,directory,Link);
    if (NULL == (pBBSFile = fopen(filename, "wt")))
      printf("Unexpected error: Can not open file \"%s\"\n", filename);
  
    while (!feof(psrcFile))
    {
      if (fgets(srcLine, MAXLINELEN, psrcFile)==0) break;
      if (fputs(srcLine,pBBSFile)==EOF)
	perror("fputs error bbs file:");
    
      if ('\n' == srcLine[strlen(srcLine) - 1])
        srcLine[strlen(srcLine) - 1] = ' ';
  
      for (j = 0; srcLine[j]; j ++)
      {
        if (ptr = strchr(srcLine + j, '@'))
        {
          j = ptr - srcLine;
          if (strchr(ptr, '.'))
          {
            if (strchr(ptr, ' ') - strchr(ptr, '.') > 0)
            {
              for (k = j - 1; k >= 0; k --)
                if (!(  (srcLine[k] >= '0' && srcLine[k] <= '9')
                      ||(srcLine[k] >= 'A' && srcLine[k] <= 'Z')
                      ||(srcLine[k] >= 'a' && srcLine[k] <= 'z')
                      || '.' == srcLine[k])  )
                  break;
  
              strcpy(Buf2, srcLine + k + 1);
              sprintf(srcLine + k + 1, "mailto:%s", Buf2);
              ptr += 7; /* strlen("mailto:") */
              j = strchr(ptr, ' ') - srcLine - 1;
            } /* End if (strchr(ptr, ' ') - strchr(ptr, '.') > 0) */
          } /* End if (strchr(ptr, '.')) */
        } /* End if (ptr = strchr(srcLine + j, '@')) */
      } /* for (j = 0; srcLine[j]; j ++) */
  
      for (j = Buf2[0] = 0; srcLine[j]; j ++)
      {
        switch (srcLine[j])
        {
          case '>':
            strcat(Buf2, "&gt;");
          break;
  
          case '<':
            strcat(Buf2, "&lt;");
          break;
  
          case '&':
            strcat(Buf2, "&amp;");
          break;
  
          case '"':
            strcat(Buf2, "&quot;");
          break;
  
          case ' ':
            strcat(Buf2, "&nbsp;");
          break;
  
          case 27:
            ptr = strchr(srcLine + j, 'm');
            if (ptr)
              j = ptr - srcLine;
          break;
  
          case 'h':
          case 'H':
          case 'f':
          case 'F':
          case 'n':
          case 'N':
          case 'm':
          case 'M':
            if (!strncasecmp(srcLine + j, "http://", 7)
            ||  !strncasecmp(srcLine + j, "ftp://",  6)
            ||  !strncasecmp(srcLine + j, "news://", 7)
            ||  !strncasecmp(srcLine + j, "mailto:", 7))
            {
              ptr = strchr(srcLine + j, ' ');
  
              if (ptr)
              {
                *ptr = 0;
                k = strlen(Buf2);
                sprintf(Buf2 + k, "<A HREF=\"%s\">%s</A>", srcLine + j, srcLine + j + 7 * (!strncasecmp(srcLine + j, "mailto:", 7)));
                *ptr = ' ';
                j += ptr - (srcLine + j) - 1;
                break;
              }
            }
            /* no break here ! */
  
          default:
            Buf2[k = strlen(Buf2)] = srcLine[j];
            Buf2[k + 1] = 0;
        }
      }
  
      if (':' == srcLine[0])
        sprintf(dstLine, "��<I>%s</I><BR>\n", Buf2 + 1);
      else if ('>' == srcLine[0])
        sprintf(dstLine, "��<I>%s</I><BR>\n", Buf2 + 4);
      else
        sprintf(dstLine, "%s<BR>\n", Buf2);
  
      fputs(dstLine, pdstFile);
    }
  
    fclose(psrcFile); 
    if (pBBSFile) fclose(pBBSFile); 
    sprintf(filename,"%08d.htm",index);
    strcpy(prevprevHtml,prevHtml);
    strcpy(prevHtml,filename);
  }
  *isDir = 0;
  return filename;
}

void DealDirectory(char* directory)
{
  FILE* IndexHtmlFile;
  FILE* DotFile;
  FILE* BBSDotFile;
  char filename[MAXPATH];
  int index;
  
  printf("Dealing Directory %s\n",directory);
  if(strstr(directory, "//")) return ;// add by bluetent
  if (strstr(directory,"install")) {
	prevHtml[1]=1;
  }
  prevHtml[0] = 0;
  prevprevHtml[0] = 0;

  sprintf(filename,"%s/%s",WorkDir,directory);
  if (mkdir(filename,0700)==-1) {
    if (errno!=EEXIST) {
      fprintf(stderr,"Create Directory %s failed:%s",filename,strerror(errno));
      return;
    }
  }
  sprintf(filename,"%s/bbs/%s",WorkDir,directory);
  if (mkdir(filename,0700)==-1) {
    if (errno!=EEXIST) 
      fprintf(stderr,"Create Directory %s failed:%s",filename,strerror(errno));
  };

  sprintf(filename,"%s/%s",directory,DOTNAMES);
  if ((DotFile=fopen(filename,"rt"))==NULL) {
    fprintf(stderr,"can't open %s file",filename);
    return;
  }
  sprintf(filename,"%s/bbs/%s/%s",WorkDir,directory,DOTNAMES);
  if ((BBSDotFile=fopen(filename,"wt"))==NULL) {
    fprintf(stderr,"can't open %s file",filename);
    return;
  }

  sprintf(filename,"%s/%s/%s",WorkDir,directory,INDEXHTML);
  if ((IndexHtmlFile=fopen(filename,"wt"))==NULL) {
    fclose(DotFile);
    if (BBSDotFile) fclose(BBSDotFile);
    perror("can't open Index Html file");
    return;
  }
  
  index = 0;
  while (!feof(DotFile)) {
        char Buf[MAXLINELEN];
        char anchor[MAXLINELEN] = "";
        char* ptr;
        
        if (fgets(Buf, MAXLINELEN, DotFile)==0) break;
  	if (BBSDotFile) 
           if (!(strstr(Buf, "Name=")&&(strstr(Buf,"(BM: BMS)")||strstr(Buf,"(BM: SYSOPS)"))))
		if (fputs(Buf,BBSDotFile)==EOF)
			perror("fputs bbs .Name:");
        Buf[strlen(Buf) - 1] = 0;
        if (ptr = strstr(Buf, "Title="))
        {
          fputs("<HTML>\n\n<HEAD>\n  <TITLE>", IndexHtmlFile);
/*          fputs("<meta http-equiv='Content-Type' content='text/html; charset=gb2312'>");
          fputs("<link rel=stylesheet type=text/css href='/bbs.css'>");
*/
          fputs(ptr + 6, IndexHtmlFile);
          fputs("</TITLE>\n</HEAD>\n\n<BODY>\n\n<CENTER><H1>", IndexHtmlFile);
          if(iscorpus)
               fputs(pbuf,IndexHtmlFile);
          else
               fputs(HEADER, IndexHtmlFile);

	  //fputs(HEADER, IndexHtmlFile);
          fputs("</H1></CENTER>\n\n", IndexHtmlFile);
          fputs("<CENTER>\n",IndexHtmlFile);
          fputs("<table>",IndexHtmlFile);
          fputs("<tr><th>���<th>���<th class=body>����<th class=body>�༭����</tr>",IndexHtmlFile);
        } else 
        if (ptr = strstr(Buf, "Name="))
        {
	  char Name[256];
      	  if ( strstr(Buf,"(BM: BMS)")) continue;
      	  if ( strstr(Buf,"(BM: SYSOPS)")) continue;
	  
	/* add by bluetent */
	if(!strncmp(Buf+5, "<HIDE>", 6)) continue;
	/* add end */
	strcpy(Name,ptr+5);
          
          strcpy(anchor, ptr + 5);
          while (1) 
          {
            fgets(Buf, MAXLINELEN, DotFile);
            if (feof(DotFile))
            {
              printf("Unexpected error: Incorrect format in \"%s\" file\n\tName not match path", DOTNAMES);
              break;
            }
            else {
  	      if (BBSDotFile) fputs(Buf,BBSDotFile);
	      if (ptr = strstr(Buf, "Path=~/"))
              {
              char* herfname;
              char datestr[25];
              int isDir;
              
              ptr[strlen(ptr) - 1] = 0;
              herfname = DealLink(directory,ptr+7,index,&isDir,datestr,Name);
              if (herfname) {
                index++;
                fprintf(IndexHtmlFile,"<tr><td>%d</td><td>%s</td>",index,isDir?"Ŀ¼":"�ļ�");
                fprintf(IndexHtmlFile,"<td><A HREF=\"%s\">%s</A></td>", herfname,anchor);
                fprintf(IndexHtmlFile,"<td>%s</td></tr>\n", datestr);
              }
              break;
	      }
            } /* feof */
          } /* while (1) */
        } /* if Buf has "Name" */
  }; /* while feof(DotFile) */
  fclose(DotFile);
  if (BBSDotFile) fclose(BBSDotFile);

  if (pdstFile) {
/*    fputs("</CENTER>\n",pdstFile);*/
    if (prevHtml[0]) {
      fprintf(pdstFile,"<A HREF=\"%s\">��һƪ</A>\n",prevHtml);
    }
    fputs("<A HREF='javascript:history.go(-1)'>������һҳ</A>\n",pdstFile);
    fputs("<A HREF=\"index.htm\">�ص�Ŀ¼</A>\n",pdstFile);
    fputs("<A HREF=\"#top\">�ص�ҳ��</A>\n",pdstFile);
    fputs("</CENTER>\n", pdstFile);
    fputs("<CENTER><H1>", pdstFile);
    if(!iscorpus)
	 fputs(FOOTER, pdstFile);
    fputs("</H1></CENTER>\n", pdstFile);
    fputs("</BODY></HTML>", pdstFile);
    fclose(pdstFile);
    pdstFile=NULL;
  }
  fputs("</table>\n", IndexHtmlFile);

  fputs("<A HREF='javascript:history.go(-1)'>������һҳ</A>\n",IndexHtmlFile);
  fputs("<A HREF=\"../index.htm\">�ص���һ��Ŀ¼</A>\n",IndexHtmlFile);
  fputs("</CENTER>\n", IndexHtmlFile);
  fputs("<CENTER><H1>", IndexHtmlFile);
  if(!iscorpus)
	fputs(FOOTER, IndexHtmlFile);
  fputs("</H1></CENTER>\n", IndexHtmlFile);
  fputs("</BODY></HTML>", IndexHtmlFile);

  fclose(IndexHtmlFile);
}

int 
main(int argc, char **argv)
{
  char maindir[MAXPATH];
  char Buf[MAXPATH];
  task_head = NULL;
  WorkDir[0]=0;
  strcpy(maindir, argv[0]);
  if (DealParameters(argc,argv)!=0)
    return -1;
    
  if (chmcontentFile) {
    fputs("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n",chmcontentFile);
    fputs("<HTML>\n<HEAD>\n<meta name=\"GENERATOR\" content=\"Microsoft&reg; HTML Help Workshop 4.1\">\n",chmcontentFile);
    fputs("<!-- Sitemap 1.0 -->\n</HEAD><BODY>\n<UL>\n",chmcontentFile);
  }
  if (WorkDir[0]==0) {
    sprintf(WorkDir,"%s.AIX",task_head->dir);
  } else {
    strcat(WorkDir,task_head->dir);
    strcat(WorkDir,".AIX");
  }

  if (OutDir[0]==0) {
    strcpy(OutDir,getcwd(NULL,0));
  }


  pdstFile=NULL;
  strcpy(maindir,task_head->dir);
  if (mkdir(WorkDir,0700)==-1) {
	fprintf(stderr,"mkdir %s:%s",WorkDir,strerror(errno));
	return 0;
  }
  sprintf(Buf,"%s/bbs",WorkDir);
  if (mkdir(Buf,0700)==-1) {
	fprintf(stderr,"mkdir %s/bbs:%s",WorkDir,strerror(errno));
	return 0;
  }
  printf("Begine %s\n",task_head->dir);
  if(iscorpus)
 	 sprintf(pbuf,"<font size=4>%s</font>���ļ�",task_head->dir);
  task_tail = task_head;
  while (task_head) {
    LP_DIR_DATA dirdata= task_head;
    task_head = task_head->next;
   if (!task_head) task_tail=NULL;
   DealDirectory(dirdata->dir);
    free(dirdata);
  }
  free(task_head);

  printf("Finished creating HTML files...\n");

  if(!iscorpus){
  printf("Compressing BBS files...\n");
  printf("Calling \"gnu tar\"...\n");


  sprintf(Buf, "%s/%s.bbs.tar.gz", OutDir, maindir);
  unlink(Buf);
  sprintf(Buf, "cd %s/bbs; %s zcf %s/%s.bbs.tar.gz %s", WorkDir , GNUTAR, OutDir,maindir, maindir); 
  printf("%s",Buf);
  system(Buf);
  }

  printf("Compressing HTML files...\n");
  printf("Calling \"gnu tar\"...\n");
  sprintf(Buf, "%s/%s.html.tar.gz", OutDir, maindir);
  unlink(Buf);
  sprintf(Buf, "cd %s; %s zcf %s/%s.html.tar.gz %s", WorkDir , GNUTAR, OutDir,maindir, maindir); 
  printf("%s",Buf);
  system(Buf);

  printf("Cleaning working directory/data...\n");
  sprintf(Buf, "rm -rf %s", WorkDir);
 printf("%d :%s",system(Buf),WorkDir);
  //system(Buf);
  printf("Finished Xhtml: %s/%s.html.tar.gz\n", OutDir,maindir);
  return 0;
}

