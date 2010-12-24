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
                http_fatal("´Ò´Ò¹ı¿ÍÎŞ·¨Ö´ĞĞ±¾²Ù×÷£¬ÇëÏÈµÇÂ¼");
        if(!(currentuser.userlevel & PERM_POST)) {
                showinfo("Äú»¹Ã»ÓĞÍ¨¹ı×¢²á,²»ÄÜÉè¶¨Í¼Æ¬Ç©Ãûµµ");
                return 0;
        }
        printf("<script>\
               function picsubmit() {\
               if(document.upload.up.value.length<4){alert('ÇëÑ¡ÔñÎÄ¼ş'\
               ); return false;};\
               alert('µã»÷ È·¶¨ ¿ªÊ¼ÉÏ´«,ÇëÄÍĞÄµÈºò,ÉÏ´«Íê³É½«ÓĞ³É¹¦ÌáÊ\
               ¾...'); return true;\
       }\
               </script>");

        modify_mode(u_info,QMD+20000);
        printf("<br><center><font size=5>Éè¶¨Í¼Æ¬Ç©Ãûµµ</font></center>");

        if(mode==0) {//Ö÷ÉèÖÃÒ³Ãæ
                printf("<p>ÄúÊÇ·ñ¾­³£µ£ĞÄÇ©Ãûµµ±»É¾³ı?ÔÚÕâÀï,ÎÒÃÇÎªÄúÌá¹©ÁËÒ»¸ö¿ÉÒÔÓÀ¾Ã±£ÁôÇ©ÃûµµµÄ¿Õ¼ä.½öÏŞÍ¼Æ¬Ò»ÕÅ,ÉÏÏŞ50k.");
                if(checkqmd()) {
                        printf("<br><br><br>ÄúÒÑ¾­Éè¶¨ÁËÒ»¸öÇ©Ãûµµ,Òª¸ü¸ÄÂğ?");
                        printf("<ul><li><a href=bbsqmd?mode=1>ÊÇµÄ,ÎÒÏë»»Ò»¸ö<a>\
                               <li><a href='javascript:history.go(-1)'>²»,»¹ÓÃÕâ¸ö°É</ul></a>");
                        return 0;
                } else {
                        printf("<center><p><a href=bbsqmd?mode=1>ÏÂÒ»²½</a></p></center>");
                        return 0;
                }
        }

        if(mode==1) {//ÉÏ´«Ò³Ãæ
                printf("<form method=post name=upload action=bbsdoupload enctype='multipart/form-data'>\n");
                printf("<input type=file name=up>");
                printf("<input type=hidden name=MAX_FILE_SIZE value=%d>", maxsize);
                printf("<input type=submit class=btn  onclick='return picsubmit()' value='ÉÏ   ´«'><br> \n");
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
