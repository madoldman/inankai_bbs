/*$Id: bbsman2recycle.c,v 1.2 2008-12-19 09:55:06 madoldman Exp $
*/

#include "BBSLIB.inc"
/*
 
删除标记的信件到回收站
 
 
*/
int main()
{
        int i, total=0,r=0;
        init_all();
        if(!loginok)
                http_fatal("请先登录");
        for(i=0; i<parm_num && i<40; i++) {
                if(!strncmp(parm_name[i], "box", 3)) {
                        total++;
                        r=do_del3(parm_name[i]+3);
                        if(r==1)
                                break;
                }
        }
        if(total<=0)
        {
                printf("请先选定信件<br>\n");
                http_quit();
        } else
                if(r==1)
                        http_quit();

        redirect("bbsmailrecycle");
        http_quit();
}

int do_del3(char *file)
{
        FILE *fp;
        int num=0;
        char path[256], dir[256],rdir[256];
        struct fileheader f;
        struct userec *u;
        sprintf(dir, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]), currentuser.userid);
        sprintf(rdir, "mail/%c/%s/.recycle", toupper(currentuser.userid[0]), currentuser.userid);
        fp=fopen(rdir, "r");

        /*if(!file_exist(rdir))
        {
             fp=fopen(rdir, "a+");
        fclose(fp);
        }

        */


        if(fp==0)
                http_fatal("错误的参数");
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {
                        del_record(rdir, sizeof(struct fileheader), num);
                        append_record(&f,sizeof(struct fileheader), dir);



                        u=getuser(f.owner);
                        fclose(fp);
                        return 0;
                }


                //  printf("%s  %s    %d <br>",f.filename,file, num );

                num++;
        }
        fclose(fp);
        printf("<tr><td><td>%s<td>文件不存在.\n", file);
        return 1;
}
