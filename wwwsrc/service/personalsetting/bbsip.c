/*$Id: bbsip.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"
FILE *fp;

int main()
{
        FILE *fp;
        char *ptr, path[256], buf[10000];
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ������õ�¼��ַ���ƣ����ȵ�¼");
        modify_mode(u_info,USERDEF+20000);	//bluetent
        printf("<center>%s -- ���õ�¼��ַ���� [ʹ����: %s]<hr>\n",
               BBSNAME, currentuser.userid);
        sprintf(path, "home/%c/%s/badhost",
                toupper(currentuser.userid[0]), currentuser.userid);
        if(!strcasecmp(getparm("type"), "1"))
                save_sig(path);
        printf("<form method=post action=bbsip?type=1>\n");
        fp=fopen(path, "r");
        bzero(buf, 10000);
        if(fp) {
                fread(buf, 9999, 1, fp);
                ptr=strcasestr(buf, "<textarea>");
                if(ptr)
                        ptr[0]=0;
                fclose(fp);
        }
        printf("ÿ������һ��IP, �����ö��IP.<table width=610 border=1><tr><td>");
        printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n");
        printf("%s", void1(buf));
        printf("</textarea></table>\n");
        printf("<input type=submit value=����> ");
        printf("<input type=reset value=��ԭ>\n");
        printf("</form><hr>\n");
        http_quit();
}

int save_sig(char *path)
{
        char *buf;
        fp=fopen(path, "w");
        buf=getparm("text");
        fprintf(fp, "%s", buf);
        fclose(fp);
        printf("��¼��ַ�����޸ĳɹ���");
        http_quit();
}
