/*$Id: bbsqmd.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
 */
#include "BBSLIB.inc"
int main()
{
        char file[80], name[80];
        int maxsize=1024*50;
        init_all();
        int mode=atoi(getparm("mode"));
        if(!loginok)
                http_fatal("�Ҵҹ����޷�ִ�б����������ȵ�¼");
        if(!(currentuser.userlevel & PERM_POST)) {
                showinfo("����û��ͨ��ע��,�����趨ͼƬǩ����");
                return 0;
        }
        printf("<script>\
               function picsubmit() {\
               if(document.upload.up.value.length<4){alert('��ѡ���ļ�'\
               ); return false;};\
               alert('��� ȷ�� ��ʼ�ϴ�,�����ĵȺ�,�ϴ���ɽ��гɹ����\
               �...'); return true;\
       }\
               </script>");

        modify_mode(u_info,QMD+20000);
        printf("<br><center><font size=5>�趨ͼƬǩ����</font></center>");

        if(mode==0) {//������ҳ��
                printf("<p>���Ƿ񾭳�����ǩ������ɾ��?������,����Ϊ���ṩ��һ���������ñ���ǩ�����Ŀռ�.����ͼƬһ��,����50k.");
                if(checkqmd()) {
                        printf("<br><br><br>���Ѿ��趨��һ��ǩ����,Ҫ������?");
                        printf("<ul><li><a href=bbsqmd?mode=1>�ǵ�,���뻻һ��<a>\
                               <li><a href='javascript:history.go(-1)'>��,���������</ul></a>");
                        return 0;
                } else {
                        printf("<center><p><a href=bbsqmd?mode=1>��һ��</a></p></center>");
                        return 0;
                }
        }

        if(mode==1) {//�ϴ�ҳ��
                printf("<form method=post name=upload action=bbsdoupload enctype='multipart/form-data'>\n");
                printf("<input type=file name=up>");
                printf("<input type=hidden name=MAX_FILE_SIZE value=%d>", maxsize);
                printf("<input type=submit class=btn  onclick='return picsubmit()' value='��   ��'><br> \n");
                printf("</form>");
                return 0;
        }
}

int checkqmd()
{

        char *dotname[]={".jpg",".gif",".jpe",".jpeg",".png"},path[STRLEN];
        int i=0;
        for(;i<5;i++) {
                sprintf(path,"%s/qmd/%c/%s%s",UPLOAD_PATH,toupper(currentuser.userid[0]),currentuser.userid,dotname[i]);
                if(file_exist(path)) {
                        printf("<br><img src=/upload/qmd/%c/%s%s>",toupper(currentuser.userid[0]),currentuser.userid,dotname[i]);
                        return 1;
                }
        }
        return 0;
}
