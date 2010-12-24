/*$Id: bbsnot.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char buf[512], board[80], filename[80];
        init_all();
        strsncpy(board, getparm("board"), 32);
        if(!has_read_perm(&currentuser, board))
                http_fatal("����İ���");
        printf("<center>\n");
        printf("%s -- ����¼ [������: %s]<hr color=green>\n", BBSNAME, board);
        sprintf(filename, "vote/%s/notes", board);
        fp=fopen(filename, "r");
        if(fp==0) {
                printf("<br>�����������ޡ����滭�桹��\n");
                http_quit();
        }
        printf("<table border=0 align=center width=500><tr><td><pre>\n");
        while(1) {
                char *s;
                bzero(buf, 512);
                if(fgets(buf, 512, fp)==0)
                        break;
                while(1) {
                        int i;
                        s=strstr(buf, "$userid");
                        if(s==0)
                                break;
                        for(i=0; i<7; i++)
                                s[i]=32;
                        for(i=0; i<strlen(currentuser.userid); i++)
                                s[i]=currentuser.userid[i];
                }
                hhprintf("%s", buf);
        }
        fclose(fp);
        printf("</pre></table><hr>\n");
        printf("[<a href=bbsdoc?board=%s>��������</a>] ", board);
        if(has_BM_perm(&currentuser, board))
                printf("[<a href=bbsmnote?board=%s>�༭���滭��</a>]", board);
        printf("</center>\n");
        http_quit();
}
