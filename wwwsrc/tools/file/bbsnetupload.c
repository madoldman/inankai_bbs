/*$Id: bbsnetupload.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        char board[80];
        //	char closewindow[80];
        int maxsize=1024000;//����Ӳ������
        //	int currexp;//�û��ľ���ֵ
        //currexp=currentuser.numposts + currentuser.numlogins/5 + (time(0)-currentuser.firstlogin)/86400 + currentuser.stay/3600;
        //	currexp=(time(0)-currentuser.firstlogin)/86400;
        //currexp=currentuser.stay/3600;
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ����޷��ϴ�����");
        strsncpy(board, currentuser.userid, 30);//��board�洢�û�id�ˣ���//bluetent
        //	strsncpy(closewindow, getparm("closewindow"), 10);//bluetent

        //	if(!has_read_perm(&currentuser, board)) http_fatal("�����������");
        //	if(!has_post_perm(&currentuser, board)) http_fatal("��Ȩ���ظ������ð�");
        if(currentuser.stay/3600<100)
                http_fatal("��վ�ۼ�ʱ��δ��100Сʱ����Ȩʹ�ñ�����");
        if((time(0)-currentuser.firstlogin)/86400<60)
                http_fatal("ע���ʺ�ʱ��δ����60�죬��Ȩʹ�ñ�����");
        //strsncpy(board, getparm("board"), 80);
        //	printf("<center>%s -- �ϴ�������%s������ [ʹ����: %s] <hr color=green>\n",
        //		BBSNAME, board, currentuser.userid);
        printf("<center>����Ӳ�� [ʹ����: %s] <hr color=green>\n", currentuser.userid);
        //	printf("ע�⣬bbs��������Դ����Ϊ��ʡ�ռ䣬�����ϴ�������ļ��������ϴ�������޹ص��ļ���<br>\n");
        //	printf("Ŀǰ���������ļ���С����Ϊ<font color=red> %d </font>�ֽ�.<br>\n", maxsize);
        //printf("%d,%d,%d,%d",currentuser.numposts,currentuser.numlogins/5,(time(0)-currentuser.firstlogin)/86400,currentuser.stay/3600);
        //	printf("�����ع��ҷ��ɣ�<font color=red>�Ͻ����طǷ����ϺͿ��ܵ��¾��׵�����</font>��<br>\n");
        printf("<form method=post action=bbsnetdoupload enctype='multipart/form-data'>\n");
        printf("<table>\n");
        printf("<tr><td>���ظ���: <td><input type=file name=up>");
        printf("<input type=hidden name=MAX_FILE_SIZE value=%d>", maxsize);
        printf("<input type=hidden name=board value='%s'>", board);
        //	printf("<tr><td>��ע: <td>");
        //	printf("<input type=radio name=level value=0 checked>1�� ");
        //	printf("<input type=radio name=level value=1>2�� ");
        //	printf("<input type=radio name=level value=2>3�� ");
        //	printf("<input type=radio name=level value=3>4�� ");
        //	printf("<input type=radio name=level value=4>5�� ");
        //	printf("<tr><td>ϣ��������ʱ��: <td><input name=live size=4 maxlength=4 value=180>(1-9999)��<br>\n");
        printf("<tr><td>�����ļ�Ҫ˵��: <td><input name=exp size=60 maxlength=72>\n");
        //add by bluetent 2002.12.22
        /*	if (!strcmp("yes", closewindow))
                printf("<input type=hidden name=closewindows value=yes>");
            else 
                printf("<input type=hidden name=closewindows value=no>");*/
        //add end
        printf("</table>\n");
        printf("<hr color=green><br>\n");
        printf("<input type=submit value='��ʼ�ϴ��ļ�'> \n");
        printf("<input type=button onclick='javascript:history.go(-1)' value='������һҳ'> \n");
        printf("</form>");
}
