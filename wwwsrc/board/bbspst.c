/*$Id: bbspst.c,v 1.5 2010-07-01 10:13:47 maybeonly Exp $
*/

#include "BBSLIB.inc"
int main()
{
        FILE *fp;
        int i, mode=0;
        char userid[80], buf[512]="", path[512], file[512], board[512], title[80]="", vfrom [80];
	    char tmpbuf [10];
        char redirfile[80];//如果是同主题模式，那么发表文章后将转至此文件所在主题展开模式
        char redirstr[128];
        int noreply = 0;             //
        struct fileheader *fileinfo;//这三个新填加的
        struct shortfile *bp;//

        //add your veriable here
        char *tableBgcolor="";//"eeeee0";

        init_all();
        if(!loginok)
                redirect("/reginfo.htm");
        modify_mode(u_info,POSTING+20000);	//bluetent


        printf("<body background=%s  bgproperties=fixed>",mytheme.bgpath);
        mode=atoi(getparm("mode"));
        /* mode=1表示是从同主题模式下回复文章, mode=2表示是从同主题模式下发表文章 */
        if(mode) {
                strsncpy(redirfile, getparm("file"), 20);
        }
        strsncpy(board, getparm("board"), 20);
        strsncpy(vfrom, getparm("vfrom"), 20);
        if(!(currentuser.userlevel & PERM_POST)&&strcmp(board, "Appeal")&&strcmp(board, "sysop"))
                http_fatal("您尚未通过注册，暂无发表文章权限");
        strsncpy(file, getparm("file"), 20);
        strsncpy(title, getparm("title"), 50);
        if(title[0] && strncmp(title, "Re: ", 4))
                sprintf(title, "Re: %s", getparm("title"));
        strsncpy(userid, getparm("userid"), 40);
        if(file[0]!='M' && file[0])
                http_fatal("错误的文件名");
        if(!has_post_perm(&currentuser, board))
                http_fatal("错误的讨论区或者您无权在此讨论区发表文章");
 
	if(!isAllowedIp(fromhost) && !HAS_PERM (PERM_BOARDS))
		http_fatal("十分对不起，本站暂时不对外网用户提供发文服务。欢迎并感谢您光临本站，你不妨四处看看先^_^");


        if(check_post_limit(board)) return 0;

        if(dashf("NOPOST")&&!HAS_PERM(PERM_OBOARDS)
			            && insec(1,board) 
				    
                                    
          )
                http_fatal("对不起，系统进入只读状态，暂停发表文章.你先四处看看吧.");
        /* efan: very faint	*/

	 bp = getbcache(board);
        if(strlen(file)!=0) {
                fileinfo=get_file_ent(board, file);
                noreply = fileinfo->accessed[0] & FILE_NOREPLY ||bp->flag & NOREPLY_FLAG;
                if(!(!noreply || HAS_PERM(PERM_SYSOP) ||has_BM_perm(&currentuser, board)))
                        http_fatal("对不起, 该文章有不可 RE 属性, 你不能回复(RE) 这篇文章.");
        }
        strcpy(u_info->board, board);



        /*	printf("<body><center>\n"); error? <body> */

        printf("<center>\n");



        //printf("%s -- 发表文章 [使用者: %s]<hr color=green>", BBSNAME, currentuser.userid);
   	    shownote(board);
        printf("<table border=0  bgcolor=%s >\n",tableBgcolor);
       // printf("<tr><td>");
        //printf("对本站的发展或者系统功能有什么意见建议吗?<a href=bbsdoc?board=Advice>欢迎您到Advice版提出!</a>\n");
       // printf("<br><font color=green>开始您的大作吧,我们拭目以待!</font><a href=bbsdoc?board=GoodArticle> 欢迎精品原创 <a/><a href=bbsdoc?board=BBSHelp> 如何使用本站 </a> <a href=bbsdoc?board=sysop> 有事找站长</a> ");
        //printf("<font color=green><br>\n");
        //printf("文责自负!请慎重考虑文章内容是否适合在公众场合发表，请勿肆意灌水.<br>\n</font>");
        if(mode==1) {
                sprintf(redirstr, "&mode=1&redir=%s", redirfile);
        } else if(mode==2) {
                strcpy(redirstr, "&mode=2");
        }
        printf("<tr><td><form name=bbspstform method=post action=bbssnd?board=%s&vfrom=%s%s>\n", board, vfrom, mode?redirstr:"");
        hsprintf(buf, "%s", void1(title));
	tmpbuf [0] = 0;
        if(strlen(title)==0) {
		if (!strcmp (board, "Secondhand")) {
			printf ("您需要【<input type=text name=title2 size=4 maxlength=4 value='%s'>】", tmpbuf);
                	printf("使用标题: <input type=text name=title size=40 maxlength=38 value='%s'>", buf);
		}
                else
			printf("使用标题: <input type=text name=title size=40 maxlength=46 value='%s'>", buf);
	}
        else {
		if (!strcmp (board, "Secondhand"))
                	printf("使用标题: %s<br>\n<input type=hidden name=title size=40 maxlength=100 value=\"%s\"><input type=hidden name=title2 size=1 value=' '>", buf, buf);
                else
			printf("使用标题: %s<br>\n<input type=hidden name=title size=40 maxlength=100 value=\"%s\">", buf, buf);
	}
        printf(" 讨论区: [<a href=bbsdoc?board=%s>%s</a>]<br>\n",board,board);
        printf("作者：%s", currentuser.userid);
        printf("  使用签名档 ");
        printf("<input type=radio name=signature value=1 checked>1");
        printf("<input type=radio name=signature value=2>2");
        printf("<input type=radio name=signature value=3>3");
        printf("<input type=radio name=signature value=4>4");
        printf("<input type=radio name=signature value=5>5");
        printf("<input type=radio name=signature value=6>6");
        printf("<input type=radio name=signature value=0>0");
        printf(" [<a target=_blank href=bbssig>查看签名档</a>] ");
        printf("<input type=checkbox name=noreply>不可回复");

        if(bp->flag & OUT_FLAG)
           printf("<input type=checkbox checked name=outgo>转信");

        printf("<table><tr><td>");
        //printf("附件:<input type=button class=btn   onclick=\"javascript:void open('bbsupload?board=%s','','top=120 left=250 width=300,height=320')\" value=粘贴附件>",board);
        printf("</td><td width=35%><input type=submit class=btn value=立即发表 onclick=\"this.value='请稍候...';this.disabled=true;bbspstform.submit();\"> ");
        printf("<input type=button class=btn value=放弃编辑 onclick=javascript:history.go(-1)>");
        printf("</td></tr></table>");
        printf("<iframe scrolling=\"no\" src=\"bbsupload?board=%s\" height=\"40\" width=\"100%\" ></iframe>",board);


        printf("<br>\n<textarea   onkeypress='if(event.keyCode==10) return document.bbspstform.submit()' onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.bbspstform.submit(); return false;}' name=text rows=20 cols=78 wrap=hard>\n\n");
        if(file[0]) {
                int lines=0;
                printf("【 在 %s 的大作中提到: 】\n", userid);
                sprintf(path, "boards/%s/%s", board, file);
                fp=fopen(path, "r");
                if(fp) {
                        for(i=0; i<3; i++)
                                if(fgets(buf, 500, fp)==0)
                                        break;
                        while(1) {
                                if(fgets(buf, 500, fp)==0)
                                        break;
                                if(!strncmp(buf, ": 【", 4))
                                        continue;
                                if(!strncmp(buf, ": : ", 4))
                                        continue;
                                if(!strncmp(buf, "--\n", 3))
                                        break;
                                if(buf[0]=='\n')
                                        continue;
                                if(lines++>=10) {
                                        printf(": (以下引言省略...)\n");
                                        break;
                                }
                                if(!strcasestr(buf, "</textarea>"))
                                        printf(": %s", buf);
                        }
                        fclose(fp);
                }
        }
        printf("</textarea>\n");
        printf("<tr><td class=post align=center>");
        //printf("<input type=submit value=发表 onclick=\"this.value='文章提交中，请稍后......';this.disabled=true;bbspstform.submit();\"> ");
        //printf("<input type=button value=放弃编辑 onclick=javascript:history.go(-1)>");
        printf("<input type=hidden name=attach value=>");
        printf("</form>\n");
        /* 利用 javascript脚本将所有的＆转换为& */
        printf("<script language=JavaScript>\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('＆', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('＆', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('＆', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('＆', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('＆', '&');\
               </script>\
               ");



         printf("<tr><td>");
         printf("<a href=bbsdoc?board=Advice>提个建议 </a><a href=bbsdoc?board=BBSHelp> 求助 </a><a href=bbsdoc?board=sysop> 联系管理人员</a> ");
       

        printf("</table>\
               <script language=javascript>\
               document.bbspstform.%s.focus();\
               </script>\
               </body>", (strlen(title)!=0)?"text":"title");
        printf("  <br> <font color=336699 >提示: 在大多浏览器下,当光标位于编辑框中时可使用Ctrl+Enter或 Ctrl+w 键快速发表文章.</font><br>");

       
        http_quit();
}
int shownote(char *board)
{
        char filename[STRLEN],buf[512];
        sprintf(filename, "vote/%s/notes", board);

        FILE *fp=fopen(filename, "r");

        if(fp==NULL) {
                return;
        }
        printf("<center>");
        printf("<table border=0 align=center width=500><tr><td><pre>\n");
        while(1) {
                bzero(buf, 512);
                if(fgets(buf, 512, fp)==0)
                        break;
                hhprintf("%s", buf);
        }
        printf("</pre></td></tr></table></center>\n");
        fclose(fp);
        return 0;
}
