/*$Id: setting.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/
#include "BBSLIB.inc"
int main()
{
        NOTADDCSSFILE=1;
        init_all();
        if(!loginok)
        	http_fatal("����ֻ�е�¼�����ʹ�á�");

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
               <a href=bbsmain class=toplink>|��ҳ|</a>\
               <a href=rmd class=toplink>|������������|\
               <a  class=toplink href=bbs0an?path=/groups>|�鿴������|</a>\
               <a href=bbspc class=toplink>|�����ļ���|</a>\
               <a href=bbs0an?path=/abbs class=toplink>|����|</a>\
               <a href=bbs0an?path=/bbslist class=toplink>|���ڱ�վ|<a/>\
\
               <br><br><br> ");

        printf("</td><td bgcolor=336699>&nbsp</td></tr>\
               <tr><td  bgcolor=d8e8f3 width=80% valign=top>");
        printf("\
               <table><tr><td valign=top>\
               ����\
               <ul>\
               <li><a href=bbsinfo>�޸�ע������</a>\
               <br>�����ĸ����������ַ,�绰,�ʼ��ȱ��ʱ,����Ҫ��������\
               <li><a  href=bbsplan>�޸ĸ���˵����</a>\
               <br>���ڲ�ѯ����ʱ���Կ������˵ĸ���˵����\
               <li><a  href=bbssig>�޸�ǩ����</a>\
               <br>������������ʱ,ϵͳ�����Զ������ǩ������ӵ����º���,������Ϊ�����˵ı�־		          <li><a  href=bbsqmd>����ͼƬǩ����</a>\
               <br>ǩ����Ҳ���Ժ���ͼƬ\
               <li><a  href=bbsnick>��ʱ�ı��ǳ�</a>\
               </ul>\
\
               �ҵĺ���\
               <ul>   <li> <a  href=bbsfall>�趨��������</a>\
               <br>������,����Ϊ���ṩ��һ�����������Ϣ�ĵط�\
               </ul>\
               ��۷��\
               <ul>   <li> <a  href=homestyle>�趨��ҳ���</a>\
               <br>ѡ��һ����񣬻��ߵ�<a href=/cgi-bin/bbs/bbsdoc?board=ArtsDev>ArtsDev</a>���ύ��������Ʒ��������\
               </ul>\
               </td><td>\
\
               ��ȫ\
               <ul>\
               <li><a  href=bbsip>��¼��ַ����</a>\
               <br>���������������ʺ�ֻ���������õĻ�����ʹ��,���Է�ֹ�ʺŵ���\
               <li> <a  href=bbspwd>�޸�����</a>\
               <br>�û��������Ǽ�Ϊ���е���Ϣ,�õ�����Ӧ�ü������ֺ���ĸ,���һ���������벻�ٰ�ȫ�������������������\
               </ul>\
\
\
               �Ķ���������\
               <ul>\
               <li> <a  href=bbsparm>�޸ĸ��˲���</a>\
               <br>�󲿷ֲ�����ʹ����telnet��ʽ\
               <li> <a  href=bbsmywww>���˶���</a>\
               <br>�޸������ż�һҳ��ʾ����,����ʶ��ģʽ���û�ʹ�ú���ʾ��ص�����\
               </ul>\
\
               ����\
               <ul>\
               <li> <a  href=bbsclearall>���ȫվδ�����</a>\
               <br> ��һƪ������û�п���ʱ,���ǽ�Ϊ�����,���������Ķ�.��������������\
               <li><a  href=bbsstat>��ѯ����ȫվ����</a>\
               <li> <a href=bbsmsg>������Ϣ</a>\
               <br>���ﱣ����������ѵ�������Ϣ\
               <ul>\
               </td></tr></table>\
\
               ");

        printf("</td>\
               <td valign=top bgcolor=336699>");
        printf("<table align=center><font color=ffffff>\
               <tr><td><a href=bbsfriend class=link>����������</a></td></tr>\
               <tr><td><a href=bbsusr  class=link>�����ķ�</a></td></tr>\
               <tr><td><a href=bbsqry class=link>��ѯ����</a></td></tr>\
               <tr><td><a href=bbssendmsg  class=link>����ѶϢ</a></td></tr>\
               <tr><td><a href='javascript:openchat()'  class=link>������</a></td></tr>\
               <tr><td><a href=bbspstmail  class=link>�����ż�</a></td></tr>\
               ");
        char buf[256];
        sprintf(buf,"0Announce/PersonalCorpus/%c/%s",toupper(currentuser.userid[0]),currentuser.userid);
        if(file_exist(buf)){
                printf("<tr><td><a href=bbs0an?path=/PersonalCorpus/%c/%s  class=link>�����ҵ��ļ�</a></td></tr>",toupper(currentuser.userid[0]),currentuser.userid);
        printf("<br><br><br><br>");
        printf("<tr>&nbsp;</tr><tr>&nbsp;</tr><tr><td><a class=link  href=createpc?mode=2&id=%s onclick='return confirm(\"�ò���������ɾ�����ĸ����ļ�,������?ǿ�ҽ����� �����ر��������ļ�. http://202.113.16.117/person/%c/%s.html.tgz \")'>ɾ���ҵ��ļ�</a></td></tr>",currentuser.userid,toupper(currentuser.userid[0]),currentuser.userid);        


        }
	else
                printf("<tr><td><a href=createpc  class=link>�����ҵ��ļ�</a></td></tr>");

        printf("</font></table>");
        printf("</td></tr>");
        printf("<tr><td bgcolor=336699><br>\
               <center><font color=ffffff>%s</center></font><br><br>\
               </td><td bgcolor=336699> &nbsp</td></tr>\
               </table></body>",BBSNAME);

}
