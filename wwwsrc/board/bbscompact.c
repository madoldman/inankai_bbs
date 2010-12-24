/*$Id: bbscompact.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $

*/

#include"BBSLIB.inc"
int main()
{
        init_all();
        struct compact p;
        char board[STRLEN],path[STRLEN];
        strsncpy(board, getparm("board"), 20);
        sprintf(path,"compact/%s/compact",board);
        int   total=get_num_records(path,sizeof(p));
        if(total==0) {
                showinfo("�ð����޹���.");
                return 1;
        }

        printf("<center>%s ������<br>",board);
        printf("----------------------------------------------------------");
        printf("<table width=80% boarder=0 align=center>");
        printf("<tr><td>���</td><td>����</td><td>��������</td><td>��������</td></tr>");
        FILE *fp=fopen(path,"r");
        if(fp==0) {
                printf("�ļ���ȡ����,������.");
                return 1;
        }
        int  i=1;
        while(1) {
                if(fread(&p, sizeof(p), 1, fp)<=0)
                        break;
                printf("<tr><td>%d</td><td><a href=bbsdoc?board=%s&&mode=c&&num=%d>%s</a></td><td>%d</td><td>%6.6s</td></tr>\
                       ",i,board,i,p.name,p.total,ctime(&p.time)+4);
                i++;
        }

        printf("</table>");
        fclose(fp);
        return 0;
}
