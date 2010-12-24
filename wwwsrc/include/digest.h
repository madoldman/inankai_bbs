/*$Id: digest.h,v 1.2 2008-12-19 09:55:05 madoldman Exp $

*/
int gettitle(char *path,char *file,char *title)
{
       char fpath[PATHLEN],buf[STRLEN],b[STRLEN];
       if(!strcmp(path,"")){
             sprintf(title,"<a href=bbs0an>Ê×Ò³</a>");
             return 0;
       }
      sprintf(fpath,"%s/.Names",path);
       FILE *fp=fopen(fpath,"r");
       if(fp==0) return 0;
       int sign=0;
       int i=0;
       int len=strlen(file);
       while(1){
           if(fgets(buf,256,fp)<=0) break;
           if(strstr(buf,"Name=")){
                strcpy(b,buf+5);
                i++;
           }
            
          if( strstr(buf,"Path=") && !strncmp(buf+7,file, len) ){   
              sign=1;
              break;
           }
      }
      if(!sign) return 0;
      b[38]=0;
      sprintf(title,"%s/<a href=bbs0an?path=%s/%s>%s</a>",title,path+9,file,trim(b));
      return i;
}
int cnpath(char *path,char *cnpath)
{
    char subpath[256],title[1024];
    char *p=strtok(path,"/");
    strcpy(subpath,"");
    strcpy(title,"");

    while(p){
         gettitle(subpath,p,title);
         if(!strcmp(subpath,"")) strcpy(subpath,p);
         else
         sprintf(subpath,"%s/%s",subpath,p);
         p=strtok(0,"/");

    }
    strcpy(cnpath,title);
}
int hasperm(int mode,char *s)
{
        if(mode==1)//ÌÖÂÛÇø°åÃæ¹ÜÀíÄ£Ê½
		   if(!has_BM_perm(&currentuser, s))  return 0;
		   else return 1;
        if(mode==2)
		   if(strcmp(s,currentuser.userid)) return 0;
		   else return 1;
        if(HAS_PERM(PERM_OBOARDS) )  return 1;
		else return 0;
}
int getpctitle(char *userid,char *title)
  {
	 strcpy(title,"");
	 char buf[256];
     sprintf(buf,"0Announce/PersonalCorpus/%c/%s/.Names",toupper(userid[0]),userid);
     FILE *fp=fopen(buf,"r");
	 if(fp==0) return 1;
	 while(1) {
		if(fgets(buf, 256, fp)==0) break;
		if(!strncmp(buf, "# Title=", 8)){
			buf[38]=0;
			strcpy(title, trim(buf+8));
			break;
		}
	}
	return 0;
}

int get_count(char *path)
{
	FILE *fp;
	char buf[256];
	int counts=0;
	sprintf(buf, "0Announce%s/counter.person", path);
	if(!file_exist(buf)) {
		fp=fopen(buf, "w+");
	} else {
		fp=fopen(buf, "r+");
	}
	flock(fileno(fp), LOCK_EX);
	fscanf(fp, "%d", &counts);
	counts++;
	fseek(fp, 0, SEEK_SET);
	fprintf(fp, "%d\n", counts);
	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	return counts;
}
int isguestbook(char *path)
{
        char buf[PATHLEN];
        sprintf(buf, "0Announce%s/.Names", path);
        FILE *fp=fopen(buf,"r");
        if(fp==0)
               return 0;
        while(1) {
                if(fgets(buf, 256, fp)<=0) break;
                if(strstr(buf, "Title=<GUESTBOOK>")) return 1;
        }
        return 0;
}

int digestpost(char *path,char *title, char *tmpfile,char *id,char *nickname,int sig,int r,char *s)
{    
     int t,i;
     char buf[PATHLEN];
     for(i=0; i<100; i++) {
        t=time(0)+i;
        sprintf(buf, "0Announce%s/M.%d.A",path,t);
        if(!file_exist(buf)) break;
     }
     if(i>=99) return 1;
     
     FILE * fp=fopen(buf, "w+");
     FILE * fp2=fopen(tmpfile, "r");
     getdatestring(time(0), NA);
     strcpy(buf,"¾«»ªÇø");
     if(r==2) strcpy(buf,"¸öÈËÎÄ¼¯");
     fprintf(fp, "·¢ÐÅÈË: %s (%s), ÐÅÇø: %s%s\n±ê  Ìâ: %s\n·¢ÐÅÕ¾: %s (%s)\n\n",
         id, nickname, s,buf, title, BBSNAME, datestring);
     if(fp2!=0) {
         while(1) {
                 if(fgets(buf, 256, fp2)<=0) break;
                 fprintf2(fp, buf);
         }
         fclose(fp2);
     }
     fprintf(fp, "\n--\n");
     sig_append(fp, id, sig);
	 fprintf(fp, "\n[1;%dm¡ù À´Ô´:¡¤%s %s ¡¤Web [FROM: %.20s][m\n", 31+rand()%7, BBSNAME,BBSHOST, 
#ifdef DEF_HIDE_IP_ENABLE
(!(currentuser.userdefine & DEF_NOTHIDEIP)&&isdormip(ip))?"ÄÏ¿ªËÞÉáÄÚÍø":fromhost);
#else
       fromhost);
#endif
      fclose(fp);
      char buf2[PATHLEN];
      sprintf(buf, "0Announce%s/.Names", path);
      sprintf(buf2,"Name=%-39s%s\nPath=~/M.%d.A\nNumb=\n#\n",title,id,t);
      f_append(buf,buf2);
      return 0;
}

int denyrobot()
{

      char buf[128];
      sethomefile(buf,currentuser.userid,".ptime");
      FILE *fp;
      int t=0;
      if(!file_exist(buf)) {
              fp=fopen(buf, "w+");
      } else {
              fp=fopen(buf, "r+");
      }
      if(fp==NULL){
         showinfo("²»ÄÜ´´½¨ÎÄ¼þ");
         exit(1);
      }

      flock(fileno(fp), LOCK_EX);
      fscanf(fp, "%d", &t);
      int now=time(NULL);
      fseek(fp, 0, SEEK_SET);
      fprintf(fp, "%d\n", now);
      flock(fileno(fp), LOCK_UN);
      fclose(fp);
      if(abs(now-t)<15){
           showinfo("ÄúÌ«ÀÛÁË£¬ÐªÒ»»á¶ù°É");
           exit(1);
      }
      return 0;
}
