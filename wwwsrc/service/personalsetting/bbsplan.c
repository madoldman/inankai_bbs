/*$Id: bbsplan.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"
FILE *fp;

int main()
{
        FILE *fp;
        char *ptr, plan[256], buf[10000];
        init_all();
        printf("<center>\n");
        if(!loginok)
                http_fatal("匆匆过客不能设置说明档，请先登录");
        modify_mode(u_info,USERDEF+20000);	//bluetent
        sprintf(plan, "home/%c/%s/plans", toupper(currentuser.userid[0]), currentuser.userid);
        if(!strcasecmp(getparm("type"), "update"))
                save_plan(plan);
        printf("%s -- 设置个人说明档 [%s]<hr>\n", BBSNAME, currentuser.userid);
        printf("<form method=post action=bbsplan?type=update>\n");
        fp=fopen(plan, "r");
        if(fp) {
                fread(buf, 9999, 1, fp);
                ptr=strcasestr(buf, "<textarea>");
                if(ptr)
                        ptr[0]=0;
                fclose(fp);
        }
        printf("<table width=610 border=1><tr><td>");
        printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n");
        printf(void1(buf));
        printf("</textarea></table>\n");
        printf("<input type=submit value=存盘> ");
        printf("<input type=reset value=复原>\n");
        printf("<hr>\n");
        http_quit();
}

int save_plan(char *plan)
{
        char buf[10000];
        /* add by yiyo 过滤说明档多余字符
                char* p;
                int i;
         
                p = buf;
                i = 0;
                while (*p) {
                        if (13 != *p) {
                                buf[i] = *p;
                                i++;
                        }
                        p++;
                }
                buf[i] = 0;
        /* add end by yiyo */
        fp=fopen(plan, "w");
        strsncpy(buf, getparm("text"), 9999);
        fprintf(fp, "%s", buf);
        fclose(fp);
        printf("个人说明档修改成功。");
        http_quit();
}
