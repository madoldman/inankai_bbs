/*$Id: bbspc.c,v 1.3 2009-10-30 04:06:45 maybeonly Exp $
*/

#include "BBSLIB.inc"
#include "digest.h"
int main()
{
        FILE *fp;
        char  log[STRLEN],id[14];

        init_all();

        modify_mode(u_info,DIGEST+20000);	//bluetent
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        strsncpy(id, getparm("id"), 13);
        char *id2=trim(id);
        if(strcmp("",id2)) {//属于查找文集的
                struct userec *x;
                x=getuser(id2);
                if(x==0) {
                        showinfo("该id不存在");
                        return 1;
                }
                char buf[256];
                sprintf(buf,"0Announce/PersonalCorpus/%c/%s/.Names",toupper(x->userid[0]),x->userid);
                FILE *fp=fopen(buf,"r");
                if(fp==0) {
                        sprintf(buf,"%s尚未创建个人文集,<a href=bbspstmail?userid=%s&title=快去建个人文集吧!>发信让其快建啦</a>",x->userid,x->userid);
                        showinfo(buf);
                        return 1;
                }
                sprintf(buf,"bbs0an?path=/PersonalCorpus/%c/%s",toupper(x->userid[0]),x->userid);
                redirect(buf);
                return 0;
        }
        /*thunder 个人文集*/
        printf("<font size=3><center>%s个人文集库</center></font><br><hr>",BBSNAME);
        printf("展示自我,搭建属于自己的温馨网上家园,本站个人文集库为您提供无限动力!");
        if((currentuser.userlevel & PERM_LOGINOK) && !(currentuser.userlevel&PERM_PERSONAL))
                printf("<a href=createpc>创建我的文集</a><br>");
        printf("<br>如何建设,管理,宣传自己的文集,欢迎访问<a href=bbsdoc?board=PersonalCorpus>个人文集经验交流区</a>");
        printf("<center>");
        int i=0,j,c=0,a[]={10,16};
        for(i=0;i<2;i++) {
                printf("<br>");
                for(j=0;j<a[i];j++,c++)
                        printf("<a href=bbs0an?path=/PersonalCorpus/%c>  --%c-- </a> ",'A'+c,'A'+c);
        }
        printf("	<form action=bbspc  method=post>\
               查找<input  style='height:20px; border:1px solid #404040' type=text name=id maxlength=13 size=13 onmousemove='this.focus()'>的个人文集.</form>\
               </center>\
               ");

if (0 && HAS_PERM(PERM_POST))
{
 printf("<p><center><a href=bbsdoc?board=PersonalCorpus>精彩文集</a></center></p>");
 printf("<p style=\"text-align:center\">用下载包恢复文集</p>");
 printf("<p><center><iframe border=\" 0\"  scrolling=\" no\"  style=\" border:none; height:40px; width:500px; margin: 0px auto\"  src=\" bbsupload?board=*restorepc\" ></iframe></center></p>");
}
 /*        char buf[PATHLEN];
        char tmpfile[STRLEN];
        int index=1;
        fp=fopen("0Announce/PersonalCorpus/pctop","r");
        if(fp!=NULL) {
                fgets(buf, 128, fp);
                printf("访问量排行TOP50(访问量:人次),截至日期:%s<table><tr>",buf);
                int count=0;
                char pctitle[STRLEN];

                for(i=0;i<3;i++) {
                        printf("<td valign=top>");
                        for(j=0;j<20;j++) {
                                if(fgets(buf, 128, fp)==0)
                                        goto E;
                                sscanf(buf,"%d %s",&count,id);
                                getpctitle (id, pctitle);
                                printf("%2d&nbsp;&nbsp;<a href=bbs0an?path=/PersonalCorpus/%c/%s>%s</a> %s<font color=green>%d</font><br>",index,toupper(id[0]),id,pctitle,id,count);

                                index++;
                        }
                        printf("</td>");
                }
        }

E:
        if(j!=20)
                printf("</td>");
        printf("</tr></table>");

        sprintf(tmpfile,"tmp/%d.pc",getpid());
        sprintf(buf, "wc -l 0Announce/PersonalCorpus/Log > %s" ,tmpfile);
        system(buf);
        fp=fopen(tmpfile, "r");
        if(fp==0) {
                showinfo("个人文集创建日志不存在");
                return 1;
        }
        i=0;



        fscanf(fp," %d ",&i);

        printf("<br>本站现有文集用户总数:%d,最新建立文集的50位网友如下",i);
        fclose(fp);
        sprintf(buf, " tail -n 50  0Announce/PersonalCorpus/Log | tac > %s" ,tmpfile);
        system(buf);
        fp=fopen(tmpfile, "r");
        if(fp==0) {
                showinfo("个人文集创建日志不存在");
                return 1;
        }

        char user[14],date[14],name[40];
        index=0;
        printf("<table cellspacing=1 cellpadding=2  width=90% border=0 >\n");
        printf("<tr bgcolor=%s class=title>
               <td align=middle width=30>序号</td>
               <td align=middle width=*>文集名称</td>
               <td align=middle>作者</td>
               <td align=middle >创建日期</td>
               </tr>",mytheme.bar);
        while(1) {
                if(fgets(buf, 128, fp)==0)
                        break;
                sscanf(buf,"%s %s %s ",user, date,name);

                index++;

                printf("<tr>
                       <td align=middle width=30>%d</td>
                       <td><a href=bbs0an?path=/PersonalCorpus/%c/%s> %s</a></td>
                       <td align=middle><a href=bbsqry?userid=%s>%s</a></td>
                       <td align=middle>%s</td>
                       </tr>",index,toupper(user[0]),user,name,user,user,date);
                if(index==100)
                        break;
        }
        printf("</table>");
        fclose(fp);
        unlink(tmpfile);
        printf("<br>注:只显示最新建立50位网友的个人文集.");
*/
}

