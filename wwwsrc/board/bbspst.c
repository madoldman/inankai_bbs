/*$Id: bbspst.c,v 1.5 2010-07-01 10:13:47 maybeonly Exp $
*/

#include "BBSLIB.inc"
int main()
{
        FILE *fp;
        int i, mode=0;
        char userid[80], buf[512]="", path[512], file[512], board[512], title[80]="", vfrom [80];
	    char tmpbuf [10];
        char redirfile[80];//�����ͬ����ģʽ����ô�������º�ת�����ļ���������չ��ģʽ
        char redirstr[128];
        int noreply = 0;             //
        struct fileheader *fileinfo;//����������ӵ�
        struct shortfile *bp;//

        //add your veriable here
        char *tableBgcolor="";//"eeeee0";

        init_all();
        if(!loginok)
                redirect("/reginfo.htm");
        modify_mode(u_info,POSTING+20000);	//bluetent


        printf("<body background=%s  bgproperties=fixed>",mytheme.bgpath);
        mode=atoi(getparm("mode"));
        /* mode=1��ʾ�Ǵ�ͬ����ģʽ�»ظ�����, mode=2��ʾ�Ǵ�ͬ����ģʽ�·������� */
        if(mode) {
                strsncpy(redirfile, getparm("file"), 20);
        }
        strsncpy(board, getparm("board"), 20);
        strsncpy(vfrom, getparm("vfrom"), 20);
        if(!(currentuser.userlevel & PERM_POST)&&strcmp(board, "Appeal")&&strcmp(board, "sysop"))
                http_fatal("����δͨ��ע�ᣬ���޷�������Ȩ��");
        strsncpy(file, getparm("file"), 20);
        strsncpy(title, getparm("title"), 50);
        if(title[0] && strncmp(title, "Re: ", 4))
                sprintf(title, "Re: %s", getparm("title"));
        strsncpy(userid, getparm("userid"), 40);
        if(file[0]!='M' && file[0])
                http_fatal("������ļ���");
        if(!has_post_perm(&currentuser, board))
                http_fatal("�������������������Ȩ�ڴ���������������");
 
	if(!isAllowedIp(fromhost) && !HAS_PERM (PERM_BOARDS))
		http_fatal("ʮ�ֶԲ��𣬱�վ��ʱ���������û��ṩ���ķ��񡣻�ӭ����л�����ٱ�վ���㲻���Ĵ�������^_^");


        if(check_post_limit(board)) return 0;

        if(dashf("NOPOST")&&!HAS_PERM(PERM_OBOARDS)
			            && insec(1,board) 
				    
                                    
          )
                http_fatal("�Բ���ϵͳ����ֻ��״̬����ͣ��������.�����Ĵ�������.");
        /* efan: very faint	*/

	 bp = getbcache(board);
        if(strlen(file)!=0) {
                fileinfo=get_file_ent(board, file);
                noreply = fileinfo->accessed[0] & FILE_NOREPLY ||bp->flag & NOREPLY_FLAG;
                if(!(!noreply || HAS_PERM(PERM_SYSOP) ||has_BM_perm(&currentuser, board)))
                        http_fatal("�Բ���, �������в��� RE ����, �㲻�ܻظ�(RE) ��ƪ����.");
        }
        strcpy(u_info->board, board);



        /*	printf("<body><center>\n"); error? <body> */

        printf("<center>\n");



        //printf("%s -- �������� [ʹ����: %s]<hr color=green>", BBSNAME, currentuser.userid);
   	    shownote(board);
        printf("<table border=0  bgcolor=%s >\n",tableBgcolor);
       // printf("<tr><td>");
        //printf("�Ա�վ�ķ�չ����ϵͳ������ʲô���������?<a href=bbsdoc?board=Advice>��ӭ����Advice�����!</a>\n");
       // printf("<br><font color=green>��ʼ���Ĵ�����,������Ŀ�Դ�!</font><a href=bbsdoc?board=GoodArticle> ��ӭ��Ʒԭ�� <a/><a href=bbsdoc?board=BBSHelp> ���ʹ�ñ�վ </a> <a href=bbsdoc?board=sysop> ������վ��</a> ");
        //printf("<font color=green><br>\n");
        //printf("�����Ը�!�����ؿ������������Ƿ��ʺ��ڹ��ڳ��Ϸ������������ˮ.<br>\n</font>");
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
			printf ("����Ҫ��<input type=text name=title2 size=4 maxlength=4 value='%s'>��", tmpbuf);
                	printf("ʹ�ñ���: <input type=text name=title size=40 maxlength=38 value='%s'>", buf);
		}
                else
			printf("ʹ�ñ���: <input type=text name=title size=40 maxlength=46 value='%s'>", buf);
	}
        else {
		if (!strcmp (board, "Secondhand"))
                	printf("ʹ�ñ���: %s<br>\n<input type=hidden name=title size=40 maxlength=100 value=\"%s\"><input type=hidden name=title2 size=1 value=' '>", buf, buf);
                else
			printf("ʹ�ñ���: %s<br>\n<input type=hidden name=title size=40 maxlength=100 value=\"%s\">", buf, buf);
	}
        printf(" ������: [<a href=bbsdoc?board=%s>%s</a>]<br>\n",board,board);
        printf("���ߣ�%s", currentuser.userid);
        printf("  ʹ��ǩ���� ");
        printf("<input type=radio name=signature value=1 checked>1");
        printf("<input type=radio name=signature value=2>2");
        printf("<input type=radio name=signature value=3>3");
        printf("<input type=radio name=signature value=4>4");
        printf("<input type=radio name=signature value=5>5");
        printf("<input type=radio name=signature value=6>6");
        printf("<input type=radio name=signature value=0>0");
        printf(" [<a target=_blank href=bbssig>�鿴ǩ����</a>] ");
        printf("<input type=checkbox name=noreply>���ɻظ�");

        if(bp->flag & OUT_FLAG)
           printf("<input type=checkbox checked name=outgo>ת��");

        printf("<table><tr><td>");
        //printf("����:<input type=button class=btn   onclick=\"javascript:void open('bbsupload?board=%s','','top=120 left=250 width=300,height=320')\" value=ճ������>",board);
        printf("</td><td width=35%><input type=submit class=btn value=�������� onclick=\"this.value='���Ժ�...';this.disabled=true;bbspstform.submit();\"> ");
        printf("<input type=button class=btn value=�����༭ onclick=javascript:history.go(-1)>");
        printf("</td></tr></table>");
        printf("<iframe scrolling=\"no\" src=\"bbsupload?board=%s\" height=\"40\" width=\"100%\" ></iframe>",board);


        printf("<br>\n<textarea   onkeypress='if(event.keyCode==10) return document.bbspstform.submit()' onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.bbspstform.submit(); return false;}' name=text rows=20 cols=78 wrap=hard>\n\n");
        if(file[0]) {
                int lines=0;
                printf("�� �� %s �Ĵ������ᵽ: ��\n", userid);
                sprintf(path, "boards/%s/%s", board, file);
                fp=fopen(path, "r");
                if(fp) {
                        for(i=0; i<3; i++)
                                if(fgets(buf, 500, fp)==0)
                                        break;
                        while(1) {
                                if(fgets(buf, 500, fp)==0)
                                        break;
                                if(!strncmp(buf, ": ��", 4))
                                        continue;
                                if(!strncmp(buf, ": : ", 4))
                                        continue;
                                if(!strncmp(buf, "--\n", 3))
                                        break;
                                if(buf[0]=='\n')
                                        continue;
                                if(lines++>=10) {
                                        printf(": (��������ʡ��...)\n");
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
        //printf("<input type=submit value=���� onclick=\"this.value='�����ύ�У����Ժ�......';this.disabled=true;bbspstform.submit();\"> ");
        //printf("<input type=button value=�����༭ onclick=javascript:history.go(-1)>");
        printf("<input type=hidden name=attach value=>");
        printf("</form>\n");
        /* ���� javascript�ű������еģ�ת��Ϊ& */
        printf("<script language=JavaScript>\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('��', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('��', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('��', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('��', '&');\
               document.bbspstform.title.value=document.bbspstform.title.value.replace('��', '&');\
               </script>\
               ");



         printf("<tr><td>");
         printf("<a href=bbsdoc?board=Advice>������� </a><a href=bbsdoc?board=BBSHelp> ���� </a><a href=bbsdoc?board=sysop> ��ϵ������Ա</a> ");
       

        printf("</table>\
               <script language=javascript>\
               document.bbspstform.%s.focus();\
               </script>\
               </body>", (strlen(title)!=0)?"text":"title");
        printf("  <br> <font color=336699 >��ʾ: �ڴ���������,�����λ�ڱ༭����ʱ��ʹ��Ctrl+Enter�� Ctrl+w �����ٷ�������.</font><br>");

       
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
