/*$Id: bbsmailclear.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        struct fileheader f;
        char rpath[80],*id,path[80];
        init_all();
        if(loginok == 0)
                http_fatal("����δ��¼");
        id=currentuser.userid;
        sprintf(rpath, "mail/%c/%s/.recycle", toupper(id[0]), id);
        fp=fopen(rpath, "r");
        if(fp==0)
                http_fatal("����Ĳ���2");
        while(1) {
                if(fread(&f, sizeof(struct fileheader ), 1, fp)<=0)
                        break;
                sprintf(path, "mail/%c/%s/%s", toupper(id[0]), id, f.filename);
                unlink(path);
        }

        unlink(rpath);
        fp=fopen(rpath,"a+");
        if(fp!=0) {
                fclose(fp);
                redirect("bbsmailrecycle");
                //   printf("��ճɹ���<a href=bbsmailrecycle> ����</a>");
                http_quit();
        }
        fclose(fp);
        http_fatal("��ճ���");
}


