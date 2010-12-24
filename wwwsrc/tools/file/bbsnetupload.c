/*$Id: bbsnetupload.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        char board[80];
        //	char closewindow[80];
        int maxsize=1024000;//网络硬盘上限
        //	int currexp;//用户的经验值
        //currexp=currentuser.numposts + currentuser.numlogins/5 + (time(0)-currentuser.firstlogin)/86400 + currentuser.stay/3600;
        //	currexp=(time(0)-currentuser.firstlogin)/86400;
        //currexp=currentuser.stay/3600;
        init_all();
        if(!loginok)
                http_fatal("匆匆过客无法上传附件");
        strsncpy(board, currentuser.userid, 30);//用board存储用户id了：）//bluetent
        //	strsncpy(closewindow, getparm("closewindow"), 10);//bluetent

        //	if(!has_read_perm(&currentuser, board)) http_fatal("错误的讨论区");
        //	if(!has_post_perm(&currentuser, board)) http_fatal("无权上载附件至该板");
        if(currentuser.stay/3600<100)
                http_fatal("上站累计时数未满100小时，无权使用本功能");
        if((time(0)-currentuser.firstlogin)/86400<60)
                http_fatal("注册帐号时间未超过60天，无权使用本功能");
        //strsncpy(board, getparm("board"), 80);
        //	printf("<center>%s -- 上传附件至%s讨论区 [使用者: %s] <hr color=green>\n",
        //		BBSNAME, board, currentuser.userid);
        printf("<center>网络硬盘 [使用者: %s] <hr color=green>\n", currentuser.userid);
        //	printf("注意，bbs服务器资源宝贵，为节省空间，请勿上传过大的文件。请勿上传与板面无关的文件。<br>\n");
        //	printf("目前单个上载文件大小限制为<font color=red> %d </font>字节.<br>\n", maxsize);
        //printf("%d,%d,%d,%d",currentuser.numposts,currentuser.numlogins/5,(time(0)-currentuser.firstlogin)/86400,currentuser.stay/3600);
        //	printf("请遵守国家法律，<font color=red>严禁上载非法资料和可能导致纠纷的资料</font>。<br>\n");
        printf("<form method=post action=bbsnetdoupload enctype='multipart/form-data'>\n");
        printf("<table>\n");
        printf("<tr><td>上载附件: <td><input type=file name=up>");
        printf("<input type=hidden name=MAX_FILE_SIZE value=%d>", maxsize);
        printf("<input type=hidden name=board value='%s'>", board);
        //	printf("<tr><td>备注: <td>");
        //	printf("<input type=radio name=level value=0 checked>1星 ");
        //	printf("<input type=radio name=level value=1>2星 ");
        //	printf("<input type=radio name=level value=2>3星 ");
        //	printf("<input type=radio name=level value=3>4星 ");
        //	printf("<input type=radio name=level value=4>5星 ");
        //	printf("<tr><td>希望保留的时间: <td><input name=live size=4 maxlength=4 value=180>(1-9999)天<br>\n");
        printf("<tr><td>附件的简要说明: <td><input name=exp size=60 maxlength=72>\n");
        //add by bluetent 2002.12.22
        /*	if (!strcmp("yes", closewindow))
                printf("<input type=hidden name=closewindows value=yes>");
            else 
                printf("<input type=hidden name=closewindows value=no>");*/
        //add end
        printf("</table>\n");
        printf("<hr color=green><br>\n");
        printf("<input type=submit value='开始上传文件'> \n");
        printf("<input type=button onclick='javascript:history.go(-1)' value='返回上一页'> \n");
        printf("</form>");
}
