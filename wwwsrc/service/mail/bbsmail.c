/*$Id: bbsmail.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "mail.h"

char *eff_size(char *file)
{
        FILE *fp;
        static char buf[256];
        int size;
        size=file_size(file)-180;//blank letter is about 180 bytes


        /*
        fp=fopen(file, "r");
        if(fp==0) return "-";
        for(i=0; i<3; i++)
                if(fgets(buf, 255, fp)==0) break;
        while(1) {
                if(fgets(buf, 255, fp)==0) break;
                if(!strcmp(buf, "--\n")) break;
                if(!strncmp(buf, ": ", 2)) continue;
                if(!strncmp(buf, "【 在 ", 4)) continue;
                if(strstr(buf, "※ 来源:．")) continue;
                for(i=0; buf[i]; i++) if(buf[i]<0) size2++;
                size+=strlen(trim(buf));
        }
        fclose(fp);
        E:*/

        if(size<=0)
                size=0;
        sprintf(buf, "%3.1fK",size/1024.0);
        return buf;
}

int  fmtime(char *file)
{
        struct stat buf;
        bzero(&buf, sizeof(buf));
        if(stat(file, &buf)==-1)
                bzero(&buf, sizeof(buf));
        return buf.st_mtime;
}
/*
int check_maxmail(int *max, int *size)
{
        char filename[256],cmd[256],mailpath[256], maildir[256], tmpf[256];
        FILE *tempfile, *fp;
        *size=0;
        *max=getmailboxsize(currentuser);
        sprintf(mailpath, "mail/%c/%s/", toupper(currentuser.userid[0]), currentuser.userid);
        sprintf(tmpf, "tmp/%s.mailsize", currentuser.userid);
        sprintf(maildir, "%s.DIR", mailpath);
        if(fmtime(maildir)<fmtime(tmpf)) {
                fp=fopen(tmpf, "r");
                if(fp) {
                        fscanf(fp, "%d ", size);
                        fclose(fp);
                        return 0;
                }
                goto READERR;
        }
        sprintf(cmd,"/usr/bin/du %s > %s", mailpath, tmpf);
        system(cmd);
        if((fp=fopen(tmpf, "r"))==NULL)
                goto READERR;
        fscanf(fp, "%d ", size);
        fclose(fp);
        return 0;
READERR:
        printf("内部错误.");
        return 1;
}
*/


int getmailsize()
{

	struct stat DIRst, SIZEst;
	char sizefile[50],dirfile[256];
	FILE  *fp;
	int mailsize=-1;
       
        char *userid=currentuser.userid;


        sprintf(dirfile, "mail/%c/%s/.DIR", toupper(userid[0]), userid);

	sprintf(sizefile,"tmp/%s.mailsize",userid);
	if(stat(dirfile, &DIRst)==-1||DIRst.st_size==0)
		mailsize = 0;
	else
		if(stat(sizefile, &SIZEst)!=-1 && SIZEst.st_size!=0
		        && SIZEst.st_ctime >= DIRst.st_ctime)
		{
			fp = fopen(sizefile,"r");
			if(fp)
			{
				fscanf(fp,"%d",&mailsize);
				fclose(fp);
			}
		}

	if( mailsize != -1 )
		return mailsize;

	mailsize = 0;
	
	int fd, ssize = sizeof(struct fileheader);
	struct fileheader fcache;
	char mailfile[256];
	struct stat st;

	fd = open(dirfile, O_RDONLY);
	if(fd != -1) {
	   while(read(fd, &fcache, ssize) == ssize){
	      sprintf(mailfile,"mail/%c/%s/%s",
	  toupper(userid[0]),userid,fcache.filename);
	      if(stat(mailfile,&st)!=-1) {
           	  mailsize += (st.st_size/1024+1);
         	}   
	   }

          close(fd);
	}



 sprintf(dirfile, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, ".recycle");

        fd = open(dirfile, O_RDONLY);
        if(fd != -1) {
           while(read(fd, &fcache, ssize) == ssize){
              sprintf(mailfile,"mail/%c/%s/%s",
          toupper(userid[0]),userid,fcache.filename);
              if(stat(mailfile,&st)!=-1) {
                  mailsize += (st.st_size/1024+1);
                }
           }

          close(fd);
        }


	fp = fopen(sizefile,"w+");
	if(fp){
	   fprintf(fp,"%d",mailsize);
	   fclose(fp);
	}
	return mailsize;
}

int getmailboxsize(struct userec user)
{
        if(user.userlevel&(PERM_SYSOP))
                return 30000;
        if(user.userlevel&(PERM_LARGEMAIL))
                return 25000;
        if((user.userlevel&(PERM_BOARDS))&&user.stay>2000000)
                return 16000;
        if((user.userlevel&(PERM_BOARDS))||user.stay>2000000)
                return 12000;
        if(user.stay>1000000)
                return 8000;
        if(user.userlevel&(PERM_LOGINOK))
                return 4000;
        return 100;
}

int main()
{
        FILE *fp;
        int filetime, i, start, total;
        int ret, max=0, size=0;
        char *ptr, buf[512], dir[80];
	char userid_ex[STRLEN];
        struct fileheader *data;
        setreuid(BBSUID, BBSUID);
        init_all();
        modify_mode(u_info,MAIL+20000);	//bluetent
        if(!loginok)
                http_fatal("您尚未登录, 请先登录");
        strsncpy(buf, getparm("start"), 10);
        start=atoi(buf)-1;
        if(buf[0]==0)
                start=999999;

        printf("<META  content=no-cache>");
        printf("<script language=javascript>function SelectAll(){\n\
               for (i=0;i<document.form1.length;i++)\n\
       {\n\
               if (document.form1.elements[i].type==\"checkbox\")\n\
               if (! document.form1.elements[i].checked)\n\
       {\n\
               document.form1.elements[i].checked=true;\n\
       }\n\
       }\n\
       }</script>");

        printf("<center>\n");
        printf("<font size=3><font color=red><b>%s@%s</b></font>   我的收件箱 </font> \n",   currentuser.userid,BBSHOST);
        printf("<hr color=green width=650>\n");
        sprintf(dir, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]), currentuser.userid);
        total=file_size(dir)/sizeof(struct fileheader);

        max=getmailboxsize(currentuser);
        size=getmailsize();

        if(!file_exist(dir)) {
                char buf[80];
                sprintf(buf, "mail/%c/%s", toupper(currentuser.userid[0]), currentuser.userid);
                if(!file_exist(buf))
                        mkdir(buf, 0770);
                fp=fopen(dir, "a+");
                fclose(fp);
        }
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("dir error");
        printf("<form name=form1 topmargin=0 leftmargin=0 action=bbsman2 method=post>\n");
        float f=size;
        int portion=f/max*100;
	if (currentuser.userlevel & PERM_MAILINFINITY) {	//fixed by istef @ 2005-16-19
		printf("您的信箱容量为：无上限,目前已使用 %dK, 信件总数：%d封<br>",(int)f,total);
	} else {
	        if(max==100)
        	        printf("您的信箱容量为: %dK, 目前已使用: %d%, 信件总数: %d封<br> ", max,portion , total);
	        else
        	        printf("您的信箱容量为: %dM, 目前已使用: %d%, 信件总数: %d封<br> ", max/1000,portion , total);
	}
        printf("<a href=bbspstmail>我要写邮件</a>\n");
        int mylines=atoi(getparm("my_t_lines"));
        if(mylines==0)
                mylines=20;
        if(start>total-mylines)
                start=total-mylines;
        if(start<0)
                start=0;

        fseek(fp, sizeof(struct fileheader)*start, SEEK_SET);
        data=(struct fileheader *)calloc(total, sizeof(struct fileheader));
        if(data==NULL) {
                showinfo("内部错误.");
                return 1;
        }
        fread(data, sizeof(struct fileheader), mylines, fp);
        fclose(fp);
        if(start>0) {
                i=start-mylines;
                if(i<0)
                        i=0;
                printf("<a href=bbsmail?start=0>第一页</a> ");
                printf("<a href=bbsmail?start=%d>上一页</a> ", i);
        }
        if(start<total-mylines) {
                i=start+mylines;
                if(i>total-1)
                        i=total-1;
                printf("<a href=bbsmail?start=%d>下一页</a> ", i);
                printf("<a href=bbsmail>最后一页&nbsp</a>");
        }

        printf("<a href='javascript:SelectAll()'>选中全部</a>");
        printf("<a href=bbsmailrecycle> &nbsp去回收站看看</a>\
               <input type=button class=btn onclick=\"if(confirm('确定要清空吗?')) location.href='bbsmailclear'\"  value=清空回收站>");
        printf("<input class=btn onclick='return confirm(\"确实要删除所有选定的信件吗\")' type=submit value='删除选中的信件'>\n");
        printf("<br><table width=610 cellspacing=1 cellpadding=2 bgColor=%s>\n", color0);
        printf("<tr height=20><td align=middle bgColor=%s><font color=#FFFFFF>序号<td align=middle bgColor=%s><font color=#FFFFFF>删除<td align=middle bgColor=%s><font color=#FFFFFF>状态<td align=middle bgColor=%s><font color=#FFFFFF>发信人<td align=middle bgColor=%s><font color=#FFFFFF>日期<td align=middle bgColor=%s><font color=#FFFFFF>信件标题<td align=middle bgColor=%s><font color=#FFFFFF>大小\n", color0, color0, color0, color0, color0, color0, color0);
        int j=0;
        for(i=start; i<start+mylines && i<total; i++,j++) {
                int type='N', type2=' ';
                char filename[256];
                printf("<tr><td align=right bgColor=%s>%d", color1, i+1);
                printf("<td align=middle bgColor=%s><input style='height:14px' type=checkbox name=box%s>", tablec, data[j].filename);
                if(data[j].accessed[0] & FILE_READ)
                        type=' ';
                if(data[j].accessed[0] & FILE_MARKED)
                        type= (type=='N') ? 'M' : 'm';
                if(type!='N') {
                        printf("<td align=middle bgColor=%s>%c%c", tablec,  type, type2);
                } else {
                        printf("<td align=middle bgColor=%s><img src=/newmail.gif>", tablec);
                }
                ptr=data[j].owner;
		/*modify by brew */

		getsenderex(ptr,userid_ex);
		 if(strstr(userid_ex,"@")!=NULL){
       /*letter from internet*/
		 printf("<td bgColor=%s><a href=bbsmailcon?file=%s&num=%d&mode=0>%s</a>", tablec , data[j].filename, i,nohtmlex(ptr,15));
         }else{
	    printf("<td bgColor=%s><a href=bbsqry?userid=%s >%s</a>", tablec, userid_ex, nohtmlex(ptr,15));
   /*letter from our site*/
         }

/*         if(getsender(ptr)){
       /*letter from internet* /
      printf("<td bgColor=%s><a href=bbsmailcon?file=%s&num=%d&mode=0>%s</a>", tablec , data[j].filename, i,ptr);
         }else{
   printf("<td bgColor=%s><a href=bbsqry?userid=%s>%13.13s</a>", tablec, ptr, ptr);
   /*letter from our site* /

         }*/
             
                filetime=atoi(data[j].filename+2);
                printf("<td bgColor=%s>%12.12s", tablec, Ctime(filetime)+4);
                printf("<td bgColor=%s><a href=bbsmailcon?file=%s&num=%d&mode=0>", tablec , data[j].filename, i);
                if(strncmp("Re: ", data[j].title, 4))
                        printf("★ ");
                hprintf("%42.42s", void1(data[j].title));
                sprintf(filename, "mail/%c/%s/%s",
                        toupper(currentuser.userid[0]), currentuser.userid, data[j].filename);
                printf("</a><td bgColor=%s align=right>%s\n",tablec,  eff_size(filename));
        }
        free(data);
        printf("</table></form><br>");
        if(total>mylines)
                printf("<form >\
                       我要跳转到第 <input    style='height:16px; border:1px solid #404040' type=text name=start maxlength=4 size=4  onmousemove='this.focus()'>封信件!</form>");
        printf("<a href=/help/pop3.html target=_blank>如何用pop3收取本站信件,将信件下载到本地?</a>\n<br>");
        if(!(currentuser.userlevel & PERM_MAILINFINITY) && size>max) //fixed by istef@6/19/2005
                printf("<script>alert('您的信件过量,您将无法收取新信件. 请清理你的信箱,及时清空回收站!');</script>\n");
        http_quit();

}
