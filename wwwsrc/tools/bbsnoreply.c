/*$Id: bbsnoreply.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"
int main()
{
        FILE *fp;
        int i, total=0, mode;
        char board[80], file[80];
        char path[256], dir[256], buf[256];
        struct fileheader f;
        init_all();
        if(!loginok)
                http_fatal("请先登录");
        strsncpy(board, getparm("board"), 60);
        strsncpy(file, getparm("file"), 60);
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/%s", board, file);
        fp=fopen(path, "r+");
        if(fp==0)
                http_fatal("错误的参数");
        fclose(fp);
        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("错误的参数");
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {
                        if(strcmp(f.owner, currentuser.userid))
                                http_fatal("您无权访问本页");
                        if (f.accessed[0] & FILE_NOREPLY) {
                                f.accessed[0] &= ~FILE_NOREPLY;
                        } else {
                                f.accessed[0] |= FILE_NOREPLY;
                        }
                        fseek(fp, -1*sizeof(struct fileheader), SEEK_CUR);
                        fwrite(&f, sizeof(struct fileheader), 1, fp);
                        break;
                }
        }
        fclose(fp);
        redirect("javascript:history.go(-1)");
        return 1;
}
