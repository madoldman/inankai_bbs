/*$Id: bbsleft.c,v 1.5 2009-12-29 06:41:51 maybeonly Exp $
*/

#define MY_CSS "/bbs.css"
#include "BBSLIB.inc"
#define  SUBMANU  "&nbsp&nbsp"

int main()
{
        char buf[1024], *ptr;
        char userid[20];//��ʾ���û���
        char fname[1024];//�ж�ͶƱ���ļ�
	char * voteboard;//ͶƱ�İ���-_-bb
	struct stat statbuf; 
	init_all();
        printf("\
               <script  src=/js/common.js>\
               </script>\
	       <script src=/js/ajax.js>\
	       </script>\
	       <script src=/js/recordtime.js>\
	       </script>\
               ",mytheme.lf,mytheme.lf,mytheme.lf,mytheme.nav);





        printf("<body id=\"bbsleft\"><div id=\"content\"><nobr>\n");
        if(strcmp(currentuser.userid, ""))
                strcpy(userid, currentuser.userid);
        else
                strcpy(userid, "guest");
        
            printf("<a target=f3 href=bbsqry?userid=%s><font  face='Verdana, Arial, Helvetica, sans-serif'>%s</font></a><br>",userid,userid);
        printf("<img src=/image/bbsleft/logout.gif>&nbsp;<a href=bbslogout target=_parent>ע���˴ε�¼</a>");




        printf("<hr   align=left WIDTH=100 SIZE=2>");

//printf("<a href=/tshirt.htm target=_blank %s><img border=0 src=/image/link.gif><font color=red>վ������</font></a><br>\n");
        //huangxu@060510:Tshirt����



       // printf("<a href=/prime/ target=f3><font color=red>�껪�ؿ�</font></a><br>\n");
// printf("<a href=/cgi-bin/bbs/bbstshirt target=f3><img src=/tshirt/t-pix.gif border=0></a><br>\n");
        showbar("�Ͽ�����", "/bbsmain.htm",0,"link");
	  showbar("��վվ��", "bbs0an?path=/abbs/D577C6C6A/D95696106",0,"12");
	  
        printf("<img src=/image/bbsleft/2.gif><a href=bbs0an target=f3>������</a><br />");     
		printf("<img src=/image/bbsleft/9.gif><a href=bbspc target=f3>�ļ���  </a><br />");
       // showbar("�껪�ؿ�","/prime/ target=f3",0);
		//printf("<img src=/image/link.gif><a href=http://nkblog.org/ target=f3>BLOG</a><a href=/wiki/ target=__blank> Wiki </a><a href=http://202.113.231.41:6969/ target=_blank>BT</a><br>");
	printf("<img src=\"/image/bbsleft/6.gif\"><a href=\"http://we.inankai.net\" target=\"_blank\">�Ͽ�Ȧ</a>&nbsp;<a href=\"http://me.inankai.net\" target=\"_blank\">�Ͽ�־</a> <br />");
        showbar("�Ӿ�����","bbsboa?3",0,"7");
        showbar("ʮ�����Ż���","/hottopic10.htm",0,"110");
	voteboard = "FameHall";
	 sprintf(fname,"vote/%s/control", voteboard);
	  if(stat(fname,&statbuf)!=-1) 
		if(statbuf.st_size>0)
		{
			sprintf(fname, "bbsvote?board=%s", voteboard);
			showbar("<font color=red>ȫվͶƱ</font>", fname, 0, "link");
		}
        showbar("����������","menu_group",1,"4");


        printf("<div id=menu_group style='display:none'>\
               <table><tr><td width=5%></td><td>\
               <a target=f3 href=bbsboa?0> ��վϵͳ</a><br>\
               <a target=f3 href=bbsboa?1> �Ͽ���ѧ</a><br>\
               <a target=f3 href=bbsboa?2> ���Լ���</a><br>\
               <a target=f3 href=bbsboa?3> �Ӿ�����</a><br>\
			   <a target=f3 href=bbsboa?4> ��ѧ����</a><br>\
               <a target=f3 href=bbsboa?5> �������</a><br>\
               <a target=f3 href=bbsboa?6> ��������</a><br>\
               <a target=f3 href=bbsboa?7> ��������</a><br>\
               <a target=f3 href=bbsboa?8> ֪�Ը���</a><br>\
               <a target=f3 href=bbsboa?9> �����˶�</a><br>\
               <a target=f3 href=bbsboa?10> ����Ⱥ��</a><br>\
               <a target=f3 href=bbsboa?11> ��Ϸ����</a><br>\
               <a target=f3 href=bbsboa?12> ��������</a><br>\
	       <a target=f3 href=bbsboa?13> �ĺ�һ��</a><br>\
               \
               </td></tr></table></div>\n");
        //showbar("ѡ��������", "bbssel", 0);
        if(loginok) {
                FILE *fp;
                int i, mybrdnum=0;
                char mybrd[GOOD_BRC_NUM][80];


                showbarex("�ҵ��ղؼ�",  "menu_fav","myfav","15");
                printf("<div id=menu_fav style='display:none'>\n\
                       <table><tr><td width=5%></td><td>");
                if (currentuser.userlevel & PERM_LOGINOK) {
                        mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
			struct shortfile *sf = NULL;
                        for(i=0; i<mybrdnum; i++){
				sf = getbcache(mybrd[i]);
				//added by lionel
				//�п��ܳ��ְ��������Ҳ��������
				//����������
				if(sf == NULL)
					continue;
                                printf("\
                                       <a target=f3 href=bbs%sdoc?board=%s>%s</a><br>\n", (currentuser.userdefine & DEF_THESIS)?"t":"", mybrd[i], sf ->title+11);
			}
                }
                printf(" <a target=f3 href=bbsmybrd>�ղؼй���</a><br>\n");
                printf("</td></tr></table></div>\n");
        }

        showbar("̸��˵����","menu_talk" ,1,"3");

        printf("<div id=menu_talk style='display:none'>\n\
               <table><tr><td width=5%></td><td>");
        if(loginok)
                printf("	  <a href=bbsfriend target=f3> ���ߺ���</a><br>\n");
        printf("	  <a href=bbsusr target=f3> �����ķ�</a><br>\n");
        printf("	  <a href=bbsqry target=f3> ��ѯ����</a><br>\n");
        if(currentuser.userlevel & PERM_PAGE) {
                //printf("         <a href=bbssendmsg target=f4> ����ѶϢ</a><br>\n");
                printf("	 <a href=bbsmsg target=f3> �鿴����ѶϢ</a><br>\n");
        }
        if(loginok)
                printf("<a href='javascript:openchat()'>����������</a><br>");
        ptr="";
        if(currentuser.userlevel & PERM_CLOAK)
        {
                printf("       <a target=f3 onclick='return confirm(\"ȷʵ�л�����״̬��?\")' href=bbscloak> �л�����</a><br>\n");
                printf("	 <a href=/js/userlist.htm?file1=513&title=mylover target=_blank>��������������</a><br>\n");
        }
        printf("</td></tr></table></div>\n");

        if(loginok) {


                showbar("�����ż���", "menu_mail",1,"8");
                //	showbar("�����ż���", "javascript:menu(menu_mail,5)", 5);
                //�ʼ�ѡ��begin
                printf("<div id=menu_mail style='display:none'><table><tr><td width=5%></td><td>\
                       <a target=f3 href=bbsnewmail>�������ʼ�</a><br>\
                       <a target=f3 href=bbsmail>�����ʼ�</a><br>\
                       <a target=f3 href=bbspstmail>�����ʼ�</a><br>\
                       <a target=f3 href=bbspstmail?mode=1>����Ⱥ���ż�</a><br>\
                       <a target=f3 href=bbsmailrecycle>�ż�����վ</a><br>\
                       <a target=_blank href=/js/userlist.htm?file1=2&title=denymail> �����ʼ�����<a><br>\
                       </td></tr></table></div>");
                //�ʼ�ѡ��end

        }


        showbar("��������", "menu_special", 1,"5");

        //�ر������ѡ��begin
        printf("<div id=menu_special style='display:none'>\n<table><tr><td width=5%></td><td>");

        //printf("	 <a target=f3 href=bbsftpsearch>������ftp����</a><br>\n");
        //printf("         <a target=f3 href=bbsadl>����������</a><br>\n");
        //printf("         <a target=f3 href=/tools.htm>�������</a><br>\n");
        printf("         <a target=f3 href=bbsdict>�ֵ��ѯ</a><br>\n");
        //printf("         <a target=f3 href=/bbsdev/>��������</a><br>\n");
        printf("<a target=f3 href=bbs0an?path=/bbslist>��������ͳ��</a><br>\n");
        printf("        <a target=f3 href=bbsgdoc?board=notepad>�����</a><br>\n");
        printf("        <a target=f3 href=bbsinboard>������������</a><br>\n");
  
      //printf("         <a target=f3 href=/prime/>�껪�ؿ�</a><br>\n");
        //   printf("         <a target=f3 href=bbsfdocall>ȫվ�����б�</a><br>\n");
       // //	showbar("ȫվ������ѯ", "bbsfind2?newmode=1", 0);
        //    printf("         <a target=f3 href=bbsfind2?newmode=1>������ѯ</a><br>\n");
        printf("</td></tr></table></div>\n");

        //�ر������ѡ��end






/*

        showbar("ϵͳ��Ѷ��", "menu_server",1);

        //	showbar("ϵͳ��Ѷ��", "javascript:menu(menu_server,7)", 7);
        //ϵͳ��Ѷ��ѡ��begin
        printf("<div id=menu_server style='display:none'>\n<table><tr><td width=5%></td><td>");
        printf("<a target=f3 href=bbs0an?path=/bbslist>��������ͳ��</a><br>\n");
        printf("	<a target=f3 href=bbsgdoc?board=notepad>�����</a><br>\n");
        printf("	<a target=f3 href=bbsinboard>������������</a><br>\n");
        //        printf("        <a target=f3 href=bbsload>ϵͳ����</a><br>\n");
        printf("</td></tr></table></div>\n");
        //ϵͳ��Ѷ��ѡ��end


*/
        if(HAS_PERM(PERM_ADMINMENU)) {
                showbar("������ѡ��", "menu_admin", 1,"g");

                //begin
                printf("<div id=menu_admin style='display:none'>\n<table><tr><td width=5%></td><td>");

                if(HAS_PERM(PERM_OBOARDS)) {//huangxu@060419

                        //printf("<a target=f3 href=bbsfadm>��������</a><br>\n");
                        printf("<a target=f3 href=createpc>���������ļ�</a><br>\n");
                        printf("<a target=f3 href=rmd>�Ƽ����¹���</a>\n<br>");
                        printf("<a target=f3 href=adminsysfile> ����ϵͳ����<a><br>\n");
                        printf("<a target=f3 href=bbsmainbuf>ˢ����ҳ</a><br>");
                }
                //huangxu@060513:WEB��ҳ
                if(HAS_PERM(PERM_WELCOME))
                	printf("<a target=f3 href=adminwebdefault>��վ����</a><br>");
                //huangxu@060520:mj
                if(HAS_PERM(PERM_ACCOUNTS))
                {
                	printf("<a target=f3 href=adminseekmj>�������</a><br>");
                }
                //huangxu@060720:attache
                if(HAS_PERM(PERM_ANNOUNCE))
                {
                	printf("<a target=f3 href=bbsattadmin>��������</a><br>");
                }

                printf("</td></tr></table></div>\n");
                //end


        }

        if(file_has_word("/home/bbs/.designer",userid)==1)
                showbar("ϵͳ������", "bbstheme", 0,"link");

        showbar("վ�����²�ѯ", "bbsfind", 0,"13");
      //  showbar("����������", "bbsall", 0);
        if(loginok){
                showbar("�ҵ���ҳ","setting",0,"14");
                printf("&nbsp;&nbsp;&nbsp;<a href=kiss target=_top>������</a>");
        }
        printf("<hr   align=left WIDTH=100 SIZE=2>");

        printf("<table><tr><form name=sel action=bbssel methord=post  target=f3><td>&nbsp;&nbsp;<input type=text name=board maxlength=20 size=9 value=ѡ�������� onclick='this.select()'></td></form></tr></table> \
\
               ");


        if(strcmp(currentuser.userid,"")!=0)
                showbar("�������濴��","homestyle",0,"10");

        if(!loginok)
                showbar("���û�ע��", "/chooseid.htm",0, "link");
        if(loginok) {
                if(HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_POST))
                        printf("<script>alert('���������ȫվ�������µ�Ȩ��, ��ο�Punishment�湫��, ��������sysop��������. ��������, ����appeal���������.')</script>\n");
        }
        if(loginok && !(currentuser.userlevel & PERM_LOGINOK) && !has_fill_form())
        {
                printf("&nbsp;&nbsp");
                showbar("��дע�ᵥ", "bbsform", 0,"link");
                printf("<script language=javascript>if(confirm(\"����û��ͨ����֤, ���ܷ�������,�����ż���ѶϢ.����Ҫ������дע�ᵥ��?\")) top.f3.location=\"bbsform\";</script>");



        }

        printf("<img src=/image/bbsleft/telnet.gif> <a href=telnet://%s>Telnet��¼</a><br />",BBSHOST);
	printf("<img src=\"/image/bbsleft/telnet.gif\"> <a href=\"telnet://%s:110\"><font color=\"red\">�ϰ���telnet</font></a>", BBSHOST);
        printf("</div>");
        http_quit();
}

int showbar(char *word, char *link, int hasmenu,char * img)
{
        static dir=0;
        if(hasmenu==0)
                printf("<a href=%s %s><img border=0 src=/image/bbsleft/%s.gif>%s</a><br>\n", link, "target=f3",img, word);
        else {
                printf("<a href=\"javascript:menui('%s','img%d')\"><img border=0 id=img%d src=/image/bbsleft/%s.gif>%s</a><br>",link,dir,dir,img,word);
                dir++;
        }
        return 1;
}

int showbarex(char *word, char *link, char *link2,char * img)
{
        static dir=256;
        printf("<a href=\"javascript:menui('%s','img%d')\"><img border=0 id=img%d src=/image/bbsleft/%s.gif></a><a target=f3 href=%s>%s</a><br>",link,dir,dir,img,link2,word);
        dir++;
        return 1;
}

