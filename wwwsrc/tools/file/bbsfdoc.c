/*$Id: bbsfdoc.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char *ptr, board[256];
        struct dir x[10000], xx;
        int i, start, total=0;//,totalclick=0;
        init_all();
        strsncpy(board, getparm("board"), 30);
        if(!has_read_perm(&currentuser, board))
                http_fatal("�����������");
        fp=fopen(UPLOAD_PATH"/.DIR", "r");
        if(fp==0)
                http_fatal("û���ҵ��ļ�");
        while(total<10000) {
                if(fread(&xx, sizeof(xx), 1, fp)<=0)
                        break;
                if(!xx.active)
                        continue;
                if(strcasecmp(xx.board, board))
                        continue;
                x[total]=xx;
                //		totalclick+=x[total].click;
                total++;

        }
        ptr=getparm("start");
        if(ptr[0]==0) {
                start=total-19;
        } else {
                start=atoi(ptr);
        }
        if(start>total-19)
                start=total-19;
        if(start<0)
                start=0;
        printf("<script language=JavaScript src=/scrolldown.js></script>");
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        //	printf("<center> %s -- ���ڸ������� [������: %s] �ܵ��������%d��ƽ�����������%5.2f<hr color=green>\n", BBSNAME, board,totalclick,totalclick*1.0/total);
        printf("<center> %s -- ���ڸ������� [������: %s] \n", BBSNAME, board);
        printf("<table width=620 border=0><tr><td>���<td>�ļ���<td>������<td>ʱ��<td>��С<td>˵��<td>��ע<td>����\n");
        for(i=start; i<start+20 && i<total; i++) {
                printf("<tr><td>%d", i+1);
                if(x[i].filename) {
                        printf("<td><a href=/cgi-bin/bbs/showfile?name=%s target=_blank>%s</a>", nohtml(x[i].showname), nohtml(x[i].showname));
                } else {
                        printf("<td><a href=/file/%s/%s>%s</a>", x[i].board, nohtml(x[i].showname), nohtml(x[i].showname));
                }
                printf("<td>%s", userid_str(x[i].userid));
                printf("<td>%6.6s", Ctime(x[i].date)+4);
                printf("<td>%d", x[i].size);
                //		printf("<td> "); //%d", x[i].click);
                //		printf("<td>%d", x[i].click);
                printf("<td><a href=bbsfexp?name=%s>more </a>\n", nohtml(x[i].showname));
                printf("<td width=150>%s ", nohtml(x[i].exp));
                printf("<td><a onclick='return confirm(\"ȷ��ɾ����?\")' href=bbsudel?file=%s&start=%d>ɾ</a>\n", x[i].showname, start);
        }
        printf("</table><br>\n");
        printf("[<a href=bbsfdoc?board=%s>����</a>] [<a href=bbsdoc?board=%s>��������</a>] ", board, board);
        if(start>0)
                printf("[<a href=bbsfdoc?board=%s&start=%d>��һҳ</a>] ", board, start-19);
        if(start<total-20)
                printf("[<a href=bbsfdoc?board=%s&start=%d>��һҳ</a>] ", board, start+19);
}
