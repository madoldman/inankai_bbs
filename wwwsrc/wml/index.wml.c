//index.wml.c
//huangxu@nkbbs.org

//��¼ǰҳ��

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
	printf ("<card title=\"��ӭ�����ֻ�%s\">", BBSNAME);
	printf ("<p>�ɣģ�<input type=\"text\" title=\"�û���\" name=\"inp_id\" maxlength=\"%d\" /></p>", IDLEN);
	printf ("<p>���룺<input type=\"password\" title=\"����\" name=\"inp_pw\" maxlength=\"8\" /></p>");
	printf ("<p>��������д������ģ���<input title=\"����\" name=\"inp_pw2\" maxlength=\"8\" /></p>");
	printf ("<p><anchor>��¼\
<go href=\"login.wml\" method=\"post\">\
<postfield name=\"id\" value=\"$(inp_id)\"/>\
<postfield name=\"pw\" value=\"$(inp_pw)\"/>\
<postfield name=\"pw2\" value=\"$(inp_pw2)\"/>\
<postfield name=\"%s\" value=\"%s\"/>\
</go>\
</anchor>\
&#32;&#32;\
<anchor><go href=\"login.wml?id=guest\" />�ο�</anchor></p>", STR_ENCODING_NAME, STR_ENCODING_TEST);
	printf ("<p>��ʹ�����ļ��������%s��ע��ID��</p>", BBSNAME);
	printf ("<p>%s http://%s/ Copyright %d</p>", BBSNAME, BBSHOST, tm->tm_year + 1900);
	printf ("</card>");
	wml_tail();
	return 0;
}
