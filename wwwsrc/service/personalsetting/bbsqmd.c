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
                http_fatal("匆匆过客无法执行本操作，请先登录");
        if(!(currentuser.userlevel & PERM_POST)) {
                showinfo("您还没有通过注册,不能设定图片签名档");
                return 0;
        }
        printf("<script>\
               function picsubmit() {\
               if(document.upload.up.value.length<4){alert('请选择文件'\
               ); return false;};\
               alert('点击 确定 开始上传,请耐心等候,上传完成将有成功提蔦
               �...'); return true;\
       }\
               </script>");

        modify_mode(u_info,QMD+20000);
        printf("<br><center><font size=5>设定图片签名档</font></center>");

        if(mode==0) {//主设置页面
                printf("<p>您是否经常担心签名档被删除?在这里,我们为您提供了一个可以永久保留签名档的空间.仅限图片一张,上限50k.");
                if(checkqmd()) {
                        printf("<br><br><br>您已经设定了一个签名档,要更改吗?");
                        printf("<ul><li><a href=bbsqmd?mode=1>是的,我想换一个<a>\
                               <li><a href='javascript:history.go(-1)'>不,还用这个吧</ul></a>");
                        return 0;
                } else {
                        printf("<center><p><a href=bbsqmd?mode=1>下一步</a></p></center>");
                        return 0;
                }
        }

        if(mode==1) {//上传页面
                printf("<form method=post name=upload action=bbsdoupload enctype='multipart/form-data'>\n");
                printf("<input type=file name=up>");
                printf("<input type=hidden name=MAX_FILE_SIZE value=%d>", maxsize);
                printf("<input type=submit class=btn  onclick='return picsubmit()' value='上   传'><br> \n");
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
