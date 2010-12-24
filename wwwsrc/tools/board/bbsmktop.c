/*$Id: bbsmktop.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main( )
{

        init_all();
        char board[STRLEN],file[STRLEN];
        struct shortfile *x1;
        strsncpy(board, getparm("board"), 32);
        strcpy(file,getparm("file"));
        x1=getbcache(board);
        if(x1==0)
                http_fatal("错误的讨论区");
        strcpy(board, x1->filename);

        if(!has_BM_perm(&currentuser, board))
                http_fatal("您没有权限访问本页");

        char path[STRLEN], dir[STRLEN];
        struct fileheader f;
        FILE *fp;
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/.top", board);

        if(!file_exist(path)) {
                printf(".top文件不存在,你还没有置顶吧");
                http_quit();
                return;

        }

        fp=fopen(path,"r");
        if(fp==NULL) {
                printf("放弃：不能打开.top文件.");
                http_quit();
                return;
        }


        int i=0;
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;


                if(!strcmp(f.filename,file)) {
                        del_record(path, sizeof(struct fileheader), i);
                        fclose(fp);
                        fp = fopen (dir, "r+");
                        if (fp) {
                                int nfh;
                                while ((nfh = fread (&f, sizeof (struct fileheader), 1, fp)) ==1) {
                                        if (!strcmp (f.filename, file) && f.accessed[1] & FILE_TIP)
                                                break;
                                }
                                if (nfh == 1) {
                                        f.accessed[1] &= ~FILE_TIP;
                                        fseek (fp, -1 * sizeof (struct fileheader), SEEK_CUR);
                                        fwrite (&f, sizeof (struct fileheader), 1, fp);
                                }
                                fclose (fp);
                        }
                        printf("操作成功,<a href='javascript:history.go(-1)'>返回</a> 记着刷新呀!");
                        http_quit();

                        return;
                }
                i++;

        }//while



        printf("在置顶文件内找不到该文件,<a href='javascript:history.go(-1)'>返回</a>");
        http_quit();

}
