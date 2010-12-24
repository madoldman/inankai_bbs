/*$Id: bbsdelmail.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        struct fileheader f;
        char path[80], file[80], *id ,rpath[80];
        int num=0,mode;
        init_all();
        if(loginok == 0)
                http_fatal("您尚未登录");
        id=currentuser.userid;

        mode=atoi(getparm("mode"));

        strsncpy(file, getparm("file"), 20);
        if(strncmp(file, "M.", 2) || strstr(file, ".."))
                http_fatal("错误的参数");
        if(mode==0) {
                sprintf(path, "mail/%c/%s/.DIR", toupper(id[0]), id);
                sprintf(rpath, "mail/%c/%s/.recycle", toupper(id[0]), id);
                if(!file_exist(rpath)) {
                        fp=fopen(rpath, "a+");
                        fclose(fp);
                }

        } else
        {
                sprintf(rpath, "mail/%c/%s/.DIR", toupper(id[0]), id);
                sprintf(path, "mail/%c/%s/.recycle", toupper(id[0]), id);
        }

        fp=fopen(path, "r");

        if(fp==0)
                http_fatal("错误的参数2");
        while(1) {
                if(fread(&f, sizeof(f), 1, fp)<=0)
                        break;
                num++;
                if(!strcmp(f.filename, file)) {
                        fclose(fp);

                        del_record(path, sizeof(struct fileheader), num-1);
                        append_record(&f,sizeof(struct fileheader),rpath);
                        if(mode==0)
                                redirect("bbsmail");
                        else
                                redirect("bbsmailrecycle");
                        http_quit();
                }
        }
        fclose(fp);
        if(mode==0)
                http_fatal("信件不存在, 无法删除");
        else
                http_fatal("信件不存在, 无法还原");

}


