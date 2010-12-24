/*$Id: bbsbrdchg.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"

//char mybrd[32][80];
char mybrd[GOOD_BRC_NUM][80];
char mybrdnew[GOOD_BRC_NUM][80];
int mybrdnum=0;
struct boardheader x;

int ismybrd(char *board)
{
        int n;
        for(n=0; n<mybrdnum; n++)
                if(!strcasecmp(mybrd[n], board))
                        return n;
        return -1;
}

int main()
{
        //	FILE *fp;
        char file[200], board[200];
        int i, j=0;
        init_all();
        strsncpy(board, getparm("board"), 32);
        if(!loginok)
                http_fatal("超时或未登录，请重新login");
        //	sprintf(file, "home/%c/%s/mybrds", toupper(currentuser.userid[0]), currentuser.userid);
        //	fp=fopen(file, "r");
        //	if(fp) {
        //		mybrdnum=fread(mybrd, sizeof(mybrd[0]), 30, fp);
        //		fclose(fp);
        //	}
        mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
        if(mybrdnum>=GOOD_BRC_NUM)
                http_fatal("您预定讨论区数目已达上限，不能增加预定");
        if(!has_read_perm(&currentuser, board))
                http_fatal("此讨论区不存在");
        if(ismybrd(board)<0) {
                strcpy(mybrd[mybrdnum], board);
                SaveMyBrdsToGoodBrd(currentuser.userid, mybrd, mybrdnum + 1);
                printf("<script>top.f2.location='bbsleft'</script>\n");
                printf("收藏讨论区成功<br><a href='javascript:history.go(-1)'>快速返回</a>");
        } else {
                for (i=0;i<mybrdnum;i++) {
                        if(strcmp(board, mybrd[i])) {
                                strcpy(mybrdnew[j], mybrd[i]);
                                j++;
                        }
                }
                SaveMyBrdsToGoodBrd(currentuser.userid, mybrdnew, mybrdnum + 1);
                printf("<script>top.f2.location='bbsleft'</script>\n");
                printf("取消收藏成功<br><a href='javascript:history.go(-1)'>快速返回</a>");
        }
        ;


        //	fp=fopen(file, "w");
        //	fwrite(mybrd, 80, mybrdnum+1, fp);
        //	fclose(fp);
        http_quit();
}
