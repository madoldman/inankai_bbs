/*$Id: bbsclear.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"

int main()
{
        char board[80], start[80], buf[256];
        init_all();
        strsncpy(board, getparm("board"), 32);
        strsncpy(start, getparm("start"), 32);
        if(!loginok)
                http_fatal("匆匆过客无法执行此项操作, 请先登录");
        if(!has_read_perm(&currentuser, board))
                http_fatal("错误的讨论区");
        brc_init(currentuser.userid, board);
        brc_clear();
        brc_update(currentuser.userid, board);
        sprintf(buf, "bbsdoc?board=%s&start=%s", board, start);
        refreshto(buf, 0);
        http_quit();
}
