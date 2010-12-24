//index.wml.c
//huangxu@nkbbs.org

//登录前页面

#include "BBSLIB.inc"
#include "WMLLIB.inc"

int main()
{
	wml_init();
	wml_head();
	struct tm *tm;
	time_t now;
	now = time(NULL);
	tm = localtime(&now);
	printf ("<card title=\"欢迎访问手机%s\">", BBSNAME);
	printf ("<p>ＩＤ：<input type=\"text\" title=\"用户名\" name=\"inp_id\" maxlength=\"%d\" /></p>", IDLEN);
	printf ("<p>密码：<input type=\"password\" title=\"密码\" name=\"inp_pw\" maxlength=\"8\" /></p>");
	printf ("<p>或者密码写这里（明文）：<input title=\"密码\" name=\"inp_pw2\" maxlength=\"8\" /></p>");
	printf ("<p><anchor>登录\
<go href=\"login.wml\" method=\"post\">\
<postfield name=\"id\" value=\"$(inp_id)\"/>\
<postfield name=\"pw\" value=\"$(inp_pw)\"/>\
<postfield name=\"pw2\" value=\"$(inp_pw2)\"/>\
<postfield name=\"%s\" value=\"%s\"/>\
</go>\
</anchor>\
&#32;&#32;\
<anchor><go href=\"login.wml?id=guest\" />游客</anchor></p>", STR_ENCODING_NAME, STR_ENCODING_TEST);
	printf ("<p>请使用您的计算机访问%s以注册ID。</p>", BBSNAME);
	printf ("<p>%s http://%s/ Copyright %d</p>", BBSNAME, BBSHOST, tm->tm_year + 1900);
	printf ("</card>");
	wml_tail();
	return 0;
}
