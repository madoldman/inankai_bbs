/*********************************************************
   write by: deardragon             date: 2000.11.09      
   fb2000.dhs.org                   [����������]
 *********************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

char dotNames[] = ".Names";
char dotHtml[]=".html";
char dotIndex[] = ".index";
char dotIndexNew[] = ".index.new";
/* visitnumFilename �Ǿ������������ļ������֣����û������ "" */
//char visitnumFilename[] = "";
char visitnumFilename[] = "counter.person";

FILE *fpIndexNew,*fpHtml;
int visited;
int ndir;
int nfile;
int dironly=0;
char currPath[2048], currFile[2048];
char numstr[512];
char visitnumstr[512];
int number=0;
char pc[128]="�����徫����������";
int corpus=0; 

int Isdir(char *fname)
{
   struct stat st;
   char tempfilename[2048];
   int result;
   sprintf(tempfilename,"%s/%s",currPath,fname);
   //���ӵĻ����ļ��� lionel
   result = (lstat(tempfilename, &st) == 0)
	   && S_ISDIR(st.st_mode)
	   && !S_ISLNK(st.st_mode);
   return (result);
}

void visitlog(char *fname)
{
   FILE *logfp;
   char tempfilename[2048],counter[9];
   if(visitnumFilename[0] == '\0'){
      visitnumstr[0] = '\0';
      return;
   }
   sprintf(tempfilename,"%s/%s/%s",currPath,fname,visitnumFilename);
   logfp=fopen(tempfilename,"r");
   if(logfp) {
      fscanf(logfp,"%s",counter);
      fclose(logfp);
   }else counter[0] = '\0';
   if(counter[0]!='\0'){
	number=atoi(counter);
	sprintf(visitnumstr,"\033[32m[%s]\033[m",counter);
   }
   else visitnumstr[0]='\0';
}

int AnnounceIndex(int level)
{
   FILE *fpNames;
   int dir=0;
   int isdir, count, HIDE;
   char NamesFile[2048],IndexFile[2048],IndexNewFile[2048];
   char buf[1024],*bufptr,*ptr,currentpath[2048], title[80];
   char save_numstr[1024];
   
   strcpy(currentpath, currPath);
   strcpy(NamesFile, currFile);

   if((fpNames = fopen(NamesFile, "r")) == NULL){
	  printf("err");
	  return -1;
   }
   if (!level) {
      struct tm *tm;
      char weeknum[7][3]={"��","һ","��","��","��","��","��"};
      time_t now = time(0);
      tm = localtime(&now);
      sprintf(IndexNewFile,"%s/%s", currentpath, dotIndexNew);
      if((fpIndexNew = fopen(IndexNewFile, "w")) == NULL){
         fclose(fpNames);
	 return -1; 
      }

      /* thunder */
      char HtmlFile[256];
      sprintf(HtmlFile, "%s/%s", currPath, dotHtml);
      if( (fpHtml = fopen(HtmlFile, "w+")) == NULL){
         fclose(fpNames);
         return -1;
      }
  
      fprintf(fpHtml," \
          <pre> \
          <br> ��������: %4d��%02d��%02d��%02d:%02d:%02d ����%2s\n",
         tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
         tm->tm_hour,tm->tm_min,tm->tm_sec,
         weeknum[tm->tm_wday]);
       
      fprintf(fpIndexNew,"\033[0;1;41;33m ---======%s[����ʱ��:%4d��%02d��%02d��%02d:%02d:%02d ����%2s] ======--- \033[m\n",pc,
	 tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
	 tm->tm_hour,tm->tm_min,tm->tm_sec,
	 weeknum[tm->tm_wday]);
      visited = ndir = nfile = 0;
   }//end if(!level){}
   isdir = count = HIDE = 0;
   while (fgets(buf, sizeof(buf), fpNames)){
      bufptr = buf + 5; 
      if (!memcmp(buf, "Name=", 5)){
	 strcpy(title,bufptr);
	 if(strstr(title + 38,"(BM: SYSOPS)") ||
			 strstr(title + 38,"(BM: BMS)")||
			 !memcmp(title,"<HIDE>",6))
		 HIDE = 1;
	 else
		 HIDE = 0;
	 ptr = strchr(title,'\n');
	 *ptr = '\0';
	 ptr--;
	 while(ptr&&*ptr==' ')
		 ptr--;
	 *(ptr+1) = '\0';
	 if(strlen(title)>34)
		 title[34] = '\0';
      }//end 'Names=' 
      if (!memcmp(buf, "Path=", 5)){
	 if(HIDE) continue;
	 //skip 2 chars '~/'
	 bufptr = bufptr + 2;
	 if (ptr = strchr(bufptr, '\n'))
		 *ptr = '\0';
	 if(strlen(bufptr) == 0) continue;
	 if (Isdir(bufptr)) {
	    ndir++;
	    isdir = 1; 
	 } else { 
	    nfile++; 
	 }
         if (!level && !memcmp(bufptr, dotIndex, 6)) {
	    visited = 1;
         } 
	 if(isdir){
            visitlog(bufptr);
            
             if(level==0)
                fprintf(fpHtml,"%d",dir++);
	     int ti;
             for( ti=0;ti<level;ti++)
		fprintf(fpHtml,"	");
	    
           
	    fprintf(fpHtml,"<a href=bbs0an?path=/%s/%s/%s>%s</a><font color=green>[%d]</font><br>"
            ,corpus?"PersonalCorpus":"groups",currPath,bufptr,title,number);
		

	    fprintf(fpIndexNew, "%s%2d.[\033[1;32mĿ¼\033[m]%s%s\033[m%s\n",
		 numstr,++count,dironly?"":"\033[33m",title,visitnumstr); 
	 } else if(!dironly){
	    fprintf(fpIndexNew, "%s%2d.[\033[1;36m�ļ�\033[m]%s\033[m\n", 
		numstr, ++count, title);
	 } else count++;
	 if (isdir) {
	    char tmp[80];
	    sprintf(currPath,"%s/%s",currentpath,bufptr);
	    sprintf(currFile,"%s/%s",currPath,dotNames);
   	    strcpy(save_numstr,numstr);
	    sprintf(tmp,"%2d. ",count);
	    strcat(numstr,tmp);
	    AnnounceIndex(level + 1);
	    strcpy(currPath,currentpath);
	    strcpy(currFile,NamesFile);
   	    strcpy(numstr,save_numstr);
	    isdir = 0;
	 }
      }
   } 
   if (!level) {
      fclose(fpIndexNew);
      fclose(fpHtml);
      sprintf(IndexFile,"%s/%s", currPath, dotIndex);
      rename(IndexNewFile, IndexFile); 
      if (!visited) {
         sprintf(IndexNewFile, "%s/%s.new",currPath,dotNames);
	 if (fpIndexNew = fopen(IndexNewFile, "w")) {
	    fseek(fpNames, (off_t) 0, SEEK_SET);
	    while (fgets(buf, sizeof(buf), fpNames)) {
	       if(buf[0] != '#') break;
	       fputs(buf, fpIndexNew);
	    } 
	    fprintf(fpIndexNew, 
	       "Name=%s\nPath=~/%s\nNumb=0\n#\n",pc,dotIndex);
	    fputs(buf,fpIndexNew);
	    while (fgets(buf, sizeof(buf), fpNames)) {
	       fputs(buf, fpIndexNew);
	    } 
	    fclose(fpNames);
	    fclose(fpIndexNew);
	    rename(IndexNewFile,NamesFile);
	    return 0;
	 }
      }
   } 
   fclose(fpNames);
}

int main(int argc, char *argv[])
{
   DIR *dirp, *bdirp;
   struct dirent *de, *bde; 
   int ch;
   char *fname, *bname;
   
   if( argc < 3 ){
      printf("  Useage: %s BBSHOME GROUPnum BoardName [-d]\n", argv[0]);
      printf("          %s BBSHOME all [-d]\n", argv[0]);
      printf("\n(1)��: %s /home/bbs 0 sysop\n", argv[0]);
      printf("       ������λ�� GROUP_0 ���� sysop ���������");
      printf("\n(2)��: %s /home/bbs all -d\n", argv[0]);
      printf("       ���������а�����д�Ŀ¼����");
      printf("\n(3)���� [-d] �ĺ��壺��������Ŀ¼����\n"); 
      printf("       ע��-d ��������У�����������\n\n");
      return 1;
   }   
   sprintf(currPath,"%s/0Announce/groups", argv[1]);
   if(chdir(currPath)==-1) {
      printf("Ŀ¼ [%s] ����ȷ��\n",currPath);
      return -1;
   }
   if(argv[argc-1][0] == '-' && argv[argc-1][1] == 'd') dironly = 1;
  
   
   if(!strcasecmp(argv[2],"CorpusOnly")){
       strcpy(pc,"��  ���ļ�����  ��");
       chdir("../PersonalCorpus");
       corpus=1;
   }else if (strcasecmp(argv[2],"all")) {
      sprintf(currPath,"GROUP_%s/%s",argv[2],argv[3]);
      sprintf(currFile,"%s/%s",currPath,dotNames);
      strcpy(numstr,"");
      AnnounceIndex(0);
      return 0;
   }
 
 
   strcpy(currPath,".");
   if (!(dirp = opendir(currPath))) {
      printf("Error! Unable to enter [Announce]\n");
      return -1;
   } 
   while (de = readdir(dirp)) {
      strcpy(currPath,".");
      fname = de->d_name;
      ch = fname[0];
      if (ch != '.') {
         if(!Isdir(fname)) continue;
         if(!(bdirp = opendir(fname))){
	    printf("Error! Unable to enter [%s]\n", fname);
	    continue;
	 }   
	 while(bde = readdir(bdirp)){
            strcpy(currPath,fname);
	    bname = bde->d_name;
	    ch = bname[0];
	    if(ch != '.'){
	       if(Isdir(bname)){
		  sprintf(currPath, "%s/%s", fname, bname);               
	          sprintf(currFile, "%s/%s", currPath, dotNames);
   		  strcpy(numstr,"");
		  printf("%s\n",currPath);
	          AnnounceIndex(0);
	       }
	    }
	 }   
	 closedir(bdirp);
      }	 
   }
   closedir(dirp);
   exit(0);
}
