/*$Id: bbsclearall.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"

int main()
{
        char board[80], start[80], buf[256];
        struct shortfile data[MAXBOARD], *x;


        int i, total=0;
        init_all();
        //strsncpy(board, getparm("board"), 32);
        //strsncpy(start, getparm("start"), 32);
        if(!loginok)
                http_fatal("�Ҵҹ����޷�ִ�д������, ���ȵ�¼");
        //	if(!has_read_perm(&currentuser, board)) http_fatal("�����������");
        //	printf("������������Ժ򡭡�<br>\n");
        for(i=0; i<MAXBOARD; i++) {
                x=&(shm_bcache->bcache[i]);
                if(x->filename[0]<=32 || x->filename[0]>'z')
                        continue;
                if(!has_read_perm(&currentuser, x->filename))
                        continue;
                memcpy(&data[total], x, sizeof(struct shortfile));
                total++;
        }

        //	printf("%d", total);
        for (i=0;i<total ;i++) {
                strsncpy(board, data[i].filename, 32);
                brc_init(currentuser.userid, board);
                brc_clear();
                brc_update(currentuser.userid, board);
                //sprintf(buf, "bbsdoc?board=%s&start=%s", board, start);
                //refreshto(buf, 0);
        }
        printf("ȫվδ������������<br><a href=javascript:history.back(1)>[����]</a>\n");
        http_quit();
}
