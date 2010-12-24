/*$Id: bbsleft.c,v 1.5 2009-12-29 06:41:51 maybeonly Exp $
*/

#define MY_CSS "/bbs.css"
#include "BBSLIB.inc"
#define  SUBMANU  "&nbsp&nbsp"

int main()
{
        char buf[1024], *ptr;
        char userid[20];//显示的用户名
        char fname[1024];//判断投票的文件
	char * voteboard;//投票的板面-_-bb
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
        printf("<img src=/image/bbsleft/logout.gif>&nbsp;<a href=bbslogout target=_parent>注销此次登录</a>");




        printf("<hr   align=left WIDTH=100 SIZE=2>");

//printf("<a href=/tshirt.htm target=_blank %s><img border=0 src=/image/link.gif><font color=red>站衫征订</font></a><br>\n");
        //huangxu@060510:Tshirt链接



       // printf("<a href=/prime/ target=f3><font color=red>年华特刊</font></a><br>\n");
// printf("<a href=/cgi-bin/bbs/bbstshirt target=f3><img src=/tshirt/t-pix.gif border=0></a><br>\n");
        showbar("南开导读", "/bbsmain.htm",0,"link");
	  showbar("本站站规", "bbs0an?path=/abbs/D577C6C6A/D95696106",0,"12");
	  
        printf("<img src=/image/bbsleft/2.gif><a href=bbs0an target=f3>精华区</a><br />");     
		printf("<img src=/image/bbsleft/9.gif><a href=bbspc target=f3>文集区  </a><br />");
       // showbar("年华特刊","/prime/ target=f3",0);
		//printf("<img src=/image/link.gif><a href=http://nkblog.org/ target=f3>BLOG</a><a href=/wiki/ target=__blank> Wiki </a><a href=http://202.113.231.41:6969/ target=_blank>BT</a><br>");
	printf("<img src=\"/image/bbsleft/6.gif\"><a href=\"http://we.inankai.net\" target=\"_blank\">南开圈</a>&nbsp;<a href=\"http://me.inankai.net\" target=\"_blank\">南开志</a> <br />");
        showbar("视觉联盟","bbsboa?3",0,"7");
        showbar("十大热门话题","/hottopic10.htm",0,"110");
	voteboard = "FameHall";
	 sprintf(fname,"vote/%s/control", voteboard);
	  if(stat(fname,&statbuf)!=-1) 
		if(statbuf.st_size>0)
		{
			sprintf(fname, "bbsvote?board=%s", voteboard);
			showbar("<font color=red>全站投票</font>", fname, 0, "link");
		}
        showbar("分类讨论区","menu_group",1,"4");


        printf("<div id=menu_group style='display:none'>\
               <table><tr><td width=5%></td><td>\
               <a target=f3 href=bbsboa?0> 本站系统</a><br>\
               <a target=f3 href=bbsboa?1> 南开大学</a><br>\
               <a target=f3 href=bbsboa?2> 电脑技术</a><br>\
               <a target=f3 href=bbsboa?3> 视觉联盟</a><br>\
			   <a target=f3 href=bbsboa?4> 文学艺术</a><br>\
               <a target=f3 href=bbsboa?5> 人文社会</a><br>\
               <a target=f3 href=bbsboa?6> 服务特区</a><br>\
               <a target=f3 href=bbsboa?7> 休闲娱乐</a><br>\
               <a target=f3 href=bbsboa?8> 知性感性</a><br>\
               <a target=f3 href=bbsboa?9> 体育运动</a><br>\
               <a target=f3 href=bbsboa?10> 社团群体</a><br>\
               <a target=f3 href=bbsboa?11> 游戏天堂</a><br>\
               <a target=f3 href=bbsboa?12> 新讨论区</a><br>\
	       <a target=f3 href=bbsboa?13> 四海一家</a><br>\
               \
               </td></tr></table></div>\n");
        //showbar("选择讨论区", "bbssel", 0);
        if(loginok) {
                FILE *fp;
                int i, mybrdnum=0;
                char mybrd[GOOD_BRC_NUM][80];


                showbarex("我的收藏夹",  "menu_fav","myfav","15");
                printf("<div id=menu_fav style='display:none'>\n\
                       <table><tr><td width=5%></td><td>");
                if (currentuser.userlevel & PERM_LOGINOK) {
                        mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
			struct shortfile *sf = NULL;
                        for(i=0; i<mybrdnum; i++){
				sf = getbcache(mybrd[i]);
				//added by lionel
				//有可能出现板面名称找不到的情况
				//例如板面更名
				if(sf == NULL)
					continue;
                                printf("\
                                       <a target=f3 href=bbs%sdoc?board=%s>%s</a><br>\n", (currentuser.userdefine & DEF_THESIS)?"t":"", mybrd[i], sf ->title+11);
			}
                }
                printf(" <a target=f3 href=bbsmybrd>收藏夹管理</a><br>\n");
                printf("</td></tr></table></div>\n");
        }

        showbar("谈天说地区","menu_talk" ,1,"3");

        printf("<div id=menu_talk style='display:none'>\n\
               <table><tr><td width=5%></td><td>");
        if(loginok)
                printf("	  <a href=bbsfriend target=f3> 在线好友</a><br>\n");
        printf("	  <a href=bbsusr target=f3> 环顾四方</a><br>\n");
        printf("	  <a href=bbsqry target=f3> 查询网友</a><br>\n");
        if(currentuser.userlevel & PERM_PAGE) {
                //printf("         <a href=bbssendmsg target=f4> 发送讯息</a><br>\n");
                printf("	 <a href=bbsmsg target=f3> 查看所有讯息</a><br>\n");
        }
        if(loginok)
                printf("<a href='javascript:openchat()'>快意聊天室</a><br>");
        ptr="";
        if(currentuser.userlevel & PERM_CLOAK)
        {
                printf("       <a target=f3 onclick='return confirm(\"确实切换隐身状态吗?\")' href=bbscloak> 切换隐身</a><br>\n");
                printf("	 <a href=/js/userlist.htm?file1=513&title=mylover target=_blank>看穿我隐身名单</a><br>\n");
        }
        printf("</td></tr></table></div>\n");

        if(loginok) {


                showbar("处理信件区", "menu_mail",1,"8");
                //	showbar("处理信件区", "javascript:menu(menu_mail,5)", 5);
                //邮件选单begin
                printf("<div id=menu_mail style='display:none'><table><tr><td width=5%></td><td>\
                       <a target=f3 href=bbsnewmail>阅览新邮件</a><br>\
                       <a target=f3 href=bbsmail>所有邮件</a><br>\
                       <a target=f3 href=bbspstmail>发送邮件</a><br>\
                       <a target=f3 href=bbspstmail?mode=1>发送群体信件</a><br>\
                       <a target=f3 href=bbsmailrecycle>信件回收站</a><br>\
                       <a target=_blank href=/js/userlist.htm?file1=2&title=denymail> 拒收邮件名单<a><br>\
                       </td></tr></table></div>");
                //邮件选单end

        }


        showbar("其他东东", "menu_special", 1,"5");

        //特别服务区选单begin
        printf("<div id=menu_special style='display:none'>\n<table><tr><td width=5%></td><td>");

        //printf("	 <a target=f3 href=bbsftpsearch>局域网ftp搜索</a><br>\n");
        //printf("         <a target=f3 href=bbsadl>精华区下载</a><br>\n");
        //printf("         <a target=f3 href=/tools.htm>软件下载</a><br>\n");
        printf("         <a target=f3 href=bbsdict>字典查询</a><br>\n");
        //printf("         <a target=f3 href=/bbsdev/>代码下载</a><br>\n");
        printf("<a target=f3 href=bbs0an?path=/bbslist>各类数据统计</a><br>\n");
        printf("        <a target=f3 href=bbsgdoc?board=notepad>活动看板</a><br>\n");
        printf("        <a target=f3 href=bbsinboard>板面在线排行</a><br>\n");
  
      //printf("         <a target=f3 href=/prime/>年华特刊</a><br>\n");
        //   printf("         <a target=f3 href=bbsfdocall>全站附件列表</a><br>\n");
       // //	showbar("全站附件查询", "bbsfind2?newmode=1", 0);
        //    printf("         <a target=f3 href=bbsfind2?newmode=1>附件查询</a><br>\n");
        printf("</td></tr></table></div>\n");

        //特别服务区选单end






/*

        showbar("系统资讯区", "menu_server",1);

        //	showbar("系统资讯区", "javascript:menu(menu_server,7)", 7);
        //系统资讯区选单begin
        printf("<div id=menu_server style='display:none'>\n<table><tr><td width=5%></td><td>");
        printf("<a target=f3 href=bbs0an?path=/bbslist>各类数据统计</a><br>\n");
        printf("	<a target=f3 href=bbsgdoc?board=notepad>活动看板</a><br>\n");
        printf("	<a target=f3 href=bbsinboard>板面在线排行</a><br>\n");
        //        printf("        <a target=f3 href=bbsload>系统负载</a><br>\n");
        printf("</td></tr></table></div>\n");
        //系统资讯区选单end


*/
        if(HAS_PERM(PERM_ADMINMENU)) {
                showbar("管理者选单", "menu_admin", 1,"g");

                //begin
                printf("<div id=menu_admin style='display:none'>\n<table><tr><td width=5%></td><td>");

                if(HAS_PERM(PERM_OBOARDS)) {//huangxu@060419

                        //printf("<a target=f3 href=bbsfadm>附件管理</a><br>\n");
                        printf("<a target=f3 href=createpc>创建个人文集</a><br>\n");
                        printf("<a target=f3 href=rmd>推荐文章管理</a>\n<br>");
                        printf("<a target=f3 href=adminsysfile> 编修系统档案<a><br>\n");
                        printf("<a target=f3 href=bbsmainbuf>刷新首页</a><br>");
                }
                //huangxu@060513:WEB首页
                if(HAS_PERM(PERM_WELCOME))
                	printf("<a target=f3 href=adminwebdefault>进站画面</a><br>");
                //huangxu@060520:mj
                if(HAS_PERM(PERM_ACCOUNTS))
                {
                	printf("<a target=f3 href=adminseekmj>马甲搜索</a><br>");
                }
                //huangxu@060720:attache
                if(HAS_PERM(PERM_ANNOUNCE))
                {
                	printf("<a target=f3 href=bbsattadmin>附件管理</a><br>");
                }

                printf("</td></tr></table></div>\n");
                //end


        }

        if(file_has_word("/home/bbs/.designer",userid)==1)
                showbar("系统风格管理", "bbstheme", 0,"link");

        showbar("站内文章查询", "bbsfind", 0,"13");
      //  showbar("所有讨论区", "bbsall", 0);
        if(loginok){
                showbar("我的首页","setting",0,"14");
                printf("&nbsp;&nbsp;&nbsp;<a href=kiss target=_top>聊天室</a>");
        }
        printf("<hr   align=left WIDTH=100 SIZE=2>");

        printf("<table><tr><form name=sel action=bbssel methord=post  target=f3><td>&nbsp;&nbsp;<input type=text name=board maxlength=20 size=9 value=选择讨论区 onclick='this.select()'></td></form></tr></table> \
\
               ");


        if(strcmp(currentuser.userid,"")!=0)
                showbar("换个界面看看","homestyle",0,"10");

        if(!loginok)
                showbar("新用户注册", "/chooseid.htm",0, "link");
        if(loginok) {
                if(HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_POST))
                        printf("<script>alert('您被封禁了全站发表文章的权限, 请参看Punishment版公告, 期满后在sysop版申请解封. 如有异议, 可在appeal版提出申诉.')</script>\n");
        }
        if(loginok && !(currentuser.userlevel & PERM_LOGINOK) && !has_fill_form())
        {
                printf("&nbsp;&nbsp");
                showbar("填写注册单", "bbsform", 0,"link");
                printf("<script language=javascript>if(confirm(\"您还没有通过认证, 不能发表文章,发送信件和讯息.现在要重新填写注册单吗?\")) top.f3.location=\"bbsform\";</script>");



        }

        printf("<img src=/image/bbsleft/telnet.gif> <a href=telnet://%s>Telnet登录</a><br />",BBSHOST);
	printf("<img src=\"/image/bbsleft/telnet.gif\"> <a href=\"telnet://%s:110\"><font color=\"red\">上班用telnet</font></a>", BBSHOST);
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

