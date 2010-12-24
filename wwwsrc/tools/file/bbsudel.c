/*$Id: bbsudel.c,v 1.2 2008-12-19 09:55:10 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        struct dir x;
        struct shortfile *brd;
        char file[80], filename[80];
        int num=0;
        int start=0;
        init_all();
        if(!loginok)
                http_fatal("匆匆过客无法删除文件, 请先登录");
        strsncpy(file, getparm("file"), 40);
        start=atoi(getparm("start"));
        fp=fopen(UPLOAD_PATH"/.DIR", "r");
        if(fp==0)
                http_fatal("空目录");
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                if(!strncmp(x.showname, file, 36)) {
                        brd=getbcache(x.board);
                        if(brd==0)
                                http_fatal("内部错误10002");
                        if(!has_BM_perm(&currentuser, x.board) &&
                                        strcmp(x.userid, currentuser.userid) &&
                                        !(currentuser.userlevel & PERM_OBOARDS)) {
                                http_fatal("你无权删除本文件");
                        }
                        if(x.filename) {
                                sprintf(filename, UPLOAD_PATH"/%d", x.filename);
                                unlink(filename);
                        } else {
                                sprintf(filename, FILE_PATH"/%s/%s", x.board, x.showname);
                                unlink(filename);
                        }
                        del_record(UPLOAD_PATH"/.DIR", sizeof(struct dir), num);
                        sprintf(filename, "bbsfdoc?board=%s&start=%d", x.board, start);
                        redirect(filename);
                        http_quit();
                }
                num++;
        }
        fclose(fp);
        http_fatal("错误的文件名");
}
