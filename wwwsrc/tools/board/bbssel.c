/*$Id: bbssel.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        char *board, buf[80], *board1, *title;
        int i, total=0;
        init_all();
        modify_mode(u_info,SELECT+20000);	//bluetent
        board=nohtml(getparm("board"));
        if(board[0]==0) {
                printf("%s -- ѡ��������<hr color=green>\n", BBSNAME);
                printf("<form name=sel action=bbssel>\n");
                printf("����������: <input type=text name=board>");
                printf(" <input type=submit value=ȷ��><br><br> �������һ���ְ������ƣ�ϵͳ��Ϊ���Զ�ѡ��");
                printf("</form><script language=javascript>sel.board.focus()</script>\n");
                http_quit();
        } else {
                for(i=0; i<MAXBOARD; i++) {
                        board1=shm_bcache->bcache[i].filename;
                        if(!has_read_perm(&currentuser, board1))
                                continue;
                        if(!strcasecmp(board, board1) && !(shm_bcache->bcache[i].flag & ZONE_FLAG)) {
                                sprintf(buf, "bbs%sdoc?board=%s", (currentuser.userdefine & DEF_THESIS)?"t":"", board1);
                                redirect(buf);
                                http_quit();
                        }
                }
                printf("%s -- ѡ��������<hr color=green>\n", BBSNAME);
                //		printf("�Ҳ������������, ", board);
                printf("�����к���'%s'����������: <br><br>\n", board);
                printf("<table>");
                for(i=0; i<MAXBOARD; i++) {
                        board1=shm_bcache->bcache[i].filename;
                        title=shm_bcache->bcache[i].title;
                        if(!has_read_perm(&currentuser, board1))
                                continue;
                        if(!(shm_bcache->bcache[i].flag & ZONE_FLAG) && (strcasestr(board1, board) || strcasestr(title, board))) {
                                total++;
                                printf("<tr><td>%d", total);
                                printf("<td><a href=bbs%sdoc?board=%s>%s</a><td>%s<br>\n", (currentuser.userdefine & DEF_THESIS)?"t":"",
                                       board1, board1, title+7);
                                sprintf(buf, "bbs%sdoc?board=%s", (currentuser.userdefine & DEF_THESIS)?"t":"", board1);
                        }
                }
                if (total==1)
                        redirect(buf);
                printf("</table><br>\n");
                printf("���ҵ�%d������������������.\n", total);
        }
}
