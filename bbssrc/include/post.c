/* 
	没有检查合法性
	使用方法: post [BOARD] <Filename[实际上不使用]> [版面显示名字] > /dev/null
	Modified by macana<ml.macana@gmail.com> at 2005.12.21 15:52
*/
#include "bbs.h" 

int safewrite(int fd, char *buf, int size)
{
	int cc, sz = size, origsz = size;
	char *bp = buf;

	do {
		cc = write(fd, bp, sz);
		if ((cc < 0) && (errno != EINTR)) {
			//report("safewrite err!");
			return -1;
		}
		if (cc > 0) {
			bp += cc;
			sz -= cc;
		}
	} while (sz > 0);
	return origsz;
}

append_record(char *filename, char *record,int size)
{
        int     fd;
        if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1) {
         //     report("open file error in append_record()");
                return -1;
        }
        flock(fd, LOCK_EX);
        lseek(fd, 0, SEEK_END);
        if (safewrite(fd, record, size) == -1)
              //  report("apprec write err!");
        ;  
        flock(fd, LOCK_UN);
        close(fd);
        return 0;
}


int main(int argc, char *argv[])
{

        FILE *inf;
        if(argc!=4){
           printf("Usage: boardname filename title");
	   return 0;

	}
        inf=stdin;
 
 /*       if(inf==NULL) {
                printf("open failed");
		return;
        }

        int  i=fread(buf,2,1,inf);
        if(i!=1)
	{	
	
                printf("%d",i);
		fclose(inf);
		return ;
	         
	}*/
	rewind(inf);
	
        char buf[256];
	char board[STRLEN],file[STRLEN]; 
	struct fileheader postfile;
        setuid(9999);
        char    filepath[STRLEN], fname[STRLEN];
        int     fp, count;
        time_t now;
		 
	sprintf(buf, "/home/bbs/boards/%s/", argv[1]);
	if (chdir(buf)) {
        	printf("Unable to chdir to '%s'\n", buf);
  		fcloseall();
		return 1;
	}

	memset(&postfile, 0, sizeof(postfile));
        now = time(0);
        sprintf(fname, "M.%d.A", now);
		sprintf(filepath, "/home/bbs/boards/%s/%s", argv[1], fname);
	count = 0;

        while ((fp = open(filepath, O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1) {
                now++;
                sprintf(fname, "M.%d.A", now);
                sprintf(filepath, "/home/bbs/boards/%s/%s", argv[1], fname);
                if (count++ > 5) {
                        fcloseall();
                        return -1;
                }
        }
        close(fp);

        strcpy(postfile.filename, fname);
        strcpy(postfile.owner, BBSID);
        
      	strcpy(postfile.title, argv[3]);
        sprintf(filepath, "/home/bbs/boards/%s/%s", argv[1], ".DIR");
       
        printf("%s",filepath);
         
        if (append_record(filepath, &postfile, sizeof(postfile)) == -1) {
              sprintf(buf, "Posting '%s' on '%s': append_record failed!",
                                postfile.title, argv[1]);
       //      report(buf);
	}

	FILE   *of;
	now = time(0);
	sprintf(filepath, "/home/bbs/boards/%s/%s", argv[1], fname);
	of = fopen(filepath, "w");
	if (inf == NULL || of == NULL) {
		if( inf ) /* add fclose here and below by yiyo */
		{
			fclose(inf);
		}
		if( of )
		{
			fclose(of);
		}    
		// report("Cross Post error");
		return;
	}
	fprintf(of,"发信人: %s (自动发信系统), 信区: %s\n",BBSID,argv[1]);
	fprintf(of, "标  题: %s\n", argv[3]);


	fprintf(of, "发信站: %s自动发信系统 %s\n\n", BBSNAME, ctime(&now));
   
	while (fgets(buf, 256, inf) != NULL) {
		fprintf(of, "%s", buf);
	}
	fprintf(of,"\n--\n");
	fprintf(of, "※ 来源:·%s %s·[FROM: localhost ] ",	BBSNAME,BBSHOST);
	fclose(inf);
	fclose(of);
}
