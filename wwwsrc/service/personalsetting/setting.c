/*$Id: setting.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/
#include "BBSLIB.inc"
int main()
{
        NOTADDCSSFILE=1;
        init_all();
        if(!loginok)
        	http_fatal("错误！只有登录后才能使用。");

        printf("<style>\
\
\
               .link{color:ffffff;FONT-SIZE: 9pt;}\
               .link:hover\
       {\
               TEXT-DECORATION: underline\
       }\
\
               .toplink{color:ffffff;font-family:Arial;FONT-SIZE: 12px;}\
               .toplink:hover{\
               TEXT-DECORATION: underline\
       }\
\
               A {TEXT-DECORATION: none;}\
\
\
               TABLE,TH,TD,P,INPUT,TEXTAREA,SELECT {FONT-SIZE:\
               9pt;}\
\
               </style>\
               <script  src=/js/common.js>\
               </script>\
\
               ");

        printf("<body leftmargin=0 topmargin=0>");
        printf("<table   leftmargin=0 topmargin=0 cellpadding=1 cellspacing=0  width=100%>");
        printf("<tr><td width=80%  bgcolor=336699>");
        printf(" <font size=4 color=ffffff face=\"Arial Black\" > %s - Personal Service Center</font><br>",currentuser.userid);
        printf("<br>\
               <a href=bbsmain class=toplink>|首页|</a>\
               <a href=rmd class=toplink>|阅览精彩文章|\
               <a  class=toplink href=bbs0an?path=/groups>|查看精华区|</a>\
               <a href=bbspc class=toplink>|个人文集区|</a>\
               <a href=bbs0an?path=/abbs class=toplink>|帮助|</a>\
               <a href=bbs0an?path=/bbslist class=toplink>|关于本站|<a/>\
\
               <br><br><br> ");

        printf("</td><td bgcolor=336699>&nbsp</td></tr>\
               <tr><td  bgcolor=d8e8f3 width=80% valign=top>");
        printf("\
               <table><tr><td valign=top>\
               资料\
               <ul>\
               <li><a href=bbsinfo>修改注册资料</a>\
               <br>当您的个人资料如地址,电话,邮件等变更时,您需要更新这里\
               <li><a  href=bbsplan>修改个人说明档</a>\
               <br>你在查询网友时可以看到别人的个人说明档\
               <li><a  href=bbssig>修改签名档</a>\
               <br>在您发表文章时,系统可以自动将你的签名档添加到文章后面,可以作为您个人的标志		          <li><a  href=bbsqmd>设置图片签名档</a>\
               <br>签名档也可以含有图片\
               <li><a  href=bbsnick>临时改变昵称</a>\
               </ul>\
\
               我的好友\
               <ul>   <li> <a  href=bbsfall>设定好友名单</a>\
               <br>在这里,我们为您提供了一个保存好友信息的地方\
               </ul>\
               外观风格\
               <ul>   <li> <a  href=homestyle>设定首页风格</a>\
               <br>选择一个风格，或者到<a href=/cgi-bin/bbs/bbsdoc?board=ArtsDev>ArtsDev</a>板提交，优秀作品奖励奖章\
               </ul>\
               </td><td>\
\
               安全\
               <ul>\
               <li><a  href=bbsip>登录地址限制</a>\
               <br>您可以设置您的帐号只能在您常用的机器上使用,可以防止帐号盗用\
               <li> <a  href=bbspwd>修改密码</a>\
               <br>用户的密码是极为敏感的信息,好的密码应该兼有数字和字母,如果一旦您的密码不再安全，请您在这里更改密码\
               </ul>\
\
\
               阅读参数定制\
               <ul>\
               <li> <a  href=bbsparm>修改个人参数</a>\
               <br>大部分参数仅使用于telnet方式\
               <li> <a  href=bbsmywww>个人定制</a>\
               <br>修改文章信件一页显示行数,链接识别模式等用户使用和显示相关的设置\
               </ul>\
\
               其他\
               <ul>\
               <li> <a  href=bbsclearall>清除全站未读标记</a>\
               <br> 当一篇文章您没有看过时,我们将为您标出,方便您的阅读.你可以清除这个标记\
               <li><a  href=bbsstat>查询个人全站排名</a>\
               <li> <a href=bbsmsg>聊天信息</a>\
               <br>这里保存着您与好友的聊天信息\
               <ul>\
               </td></tr></table>\
\
               ");

        printf("</td>\
               <td valign=top bgcolor=336699>");
        printf("<table align=center><font color=ffffff>\
               <tr><td><a href=bbsfriend class=link>好友在线吗</a></td></tr>\
               <tr><td><a href=bbsusr  class=link>环顾四方</a></td></tr>\
               <tr><td><a href=bbsqry class=link>查询网友</a></td></tr>\
               <tr><td><a href=bbssendmsg  class=link>发送讯息</a></td></tr>\
               <tr><td><a href='javascript:openchat()'  class=link>聊天室</a></td></tr>\
               <tr><td><a href=bbspstmail  class=link>发送信件</a></td></tr>\
               ");
        char buf[256];
        sprintf(buf,"0Announce/PersonalCorpus/%c/%s",toupper(currentuser.userid[0]),currentuser.userid);
        if(file_exist(buf)){
                printf("<tr><td><a href=bbs0an?path=/PersonalCorpus/%c/%s  class=link>管理我的文集</a></td></tr>",toupper(currentuser.userid[0]),currentuser.userid);
        printf("<br><br><br><br>");
        printf("<tr>&nbsp;</tr><tr>&nbsp;</tr><tr><td><a class=link  href=createpc?mode=2&id=%s onclick='return confirm(\"该操作将彻底删除您的个人文集,继续吗?强烈建议您 先下载保存您的文集. http://202.113.16.117/person/%c/%s.html.tgz \")'>删除我的文集</a></td></tr>",currentuser.userid,toupper(currentuser.userid[0]),currentuser.userid);        


        }
	else
                printf("<tr><td><a href=createpc  class=link>创建我的文集</a></td></tr>");

        printf("</font></table>");
        printf("</td></tr>");
        printf("<tr><td bgcolor=336699><br>\
               <center><font color=ffffff>%s</center></font><br><br>\
               </td><td bgcolor=336699> &nbsp</td></tr>\
               </table></body>",BBSNAME);

}
