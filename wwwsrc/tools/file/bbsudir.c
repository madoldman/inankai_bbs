/*$Id: bbsudir.c,v 1.2 2008-12-19 09:55:10 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char path[100];
        struct dir x;
        init_all();
        fp=fopen("upload/.DIR", "r");
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                printf("%s, %s, %s, %d, %s, %d, %d <br>\n",
                       x.board, x.type, x.userid, x.filename, x.showname, x.size, x.date);
        }
        fclose(fp);
}
