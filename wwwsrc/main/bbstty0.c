/*$Id: bbstty0.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#define MY_CSS  ""
#include "BBSLIB.inc"
int  main()
{
        init_all();
        printf("\
               <style type=text/css>\
               A {color: #%s}\
               BODY {BACKGROUND-COLOR: #%s; FONT-SIZE: 12px;}\
               </style>\
               ",mytheme.lf,mytheme.nav);

        printf("<meta http-equiv=\"pragma\" content=\"no-cache\">");

        printf("<body topmargin=4 leftmargin=4 background=%s  >",mytheme.lbg);
        printf("\
               <script>\
               function check(x) {\
               x.cmd.value=x.cmd2.value;\
               x.cmd2.value='';\
               if(x.cmd.value=='a') {\
               top.f3.scrollBy(0, -400);\
               return false;\
       }\
               if(x.cmd.value=='x') {\
               top.f3.scrollBy(0, 400);\
               return false;\
       }\
               if(x.cmd.value=='r') {\
               top.f3.location=top.f3.location;\
               return false;\
       }\
               if(x.cmd.value=='close') {\
               parent.content.rows='*,0';\
               return false;\
       }\
               return 1;\
       }\
               </script>\
               <form onsubmit='return check(this)' name=form2 action=/cgi-bin/bbs/bbstty target=f3 method=post>\
               <nobr>\
               <input type=hidden name=cmd>\
               <a href=/cgi-bin/bbs/bbstty?cmd=/h target=f3>¿ØÖÆÌ¨<b>:</b></a>\
               <input style='height:16px; border:1px solid #404040' type=text name=cmd2 maxlength=50 size=9 onmousemove='this.focus()'>\
               </form>\
               </body>\
               ");
        if(loginok && (currentuser.userlevel & PERM_PAGE) ) {
                if(has_new_msg()) {
                        printf("<script language=javascript> open('bbssendmsg?s=1', '', 'top=100,left=150,width=350,height=150 scrollbars=yes');</script>");
                        refreshto("bbstty0", 10);
                } else
                        refreshto("bbstty0", 20);
        }
}
