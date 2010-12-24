/*$Id: bbsfwd.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        struct fileheader *x;
        char board[80], file[80], target[80];
        init_all();
        strsncpy(board, getparm("board"), 30);
        strsncpy(file, getparm("file"), 30);
        strsncpy(target, getparm("target"), 30);
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ��ܽ��б������");
        if(!has_read_perm(&currentuser, board))
                http_fatal("�����������");
        x=get_file_ent(board, file);
        if(x==0)
                http_fatal("������ļ���");
        printf("<center>%s -- ת��/�Ƽ������� [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        if(target[0]) {
                if(!strstr(target, "@")) {
                        if(!getuser(target))
                                http_fatal("�����ʹ�����ʺ�");
                        strcpy(target, getuser(target)->userid);
                }
                return do_fwd(x, board, target);
        }
        printf("<table><tr><td>\n");
        printf("���±���: %s<br>\n", nohtml(x->title));
        printf("��������: %s<br>\n", x->owner);
        printf("ԭ������: %s<br>\n", board);
        printf("<form action=bbsfwd method=post>\n");
        printf("<input type=hidden name=board value=%s>", board);
        printf("<input type=hidden name=file value=%s>", file);
        printf("������ת�ĸ� <input name=target size=30 maxlength=30 value=%s> (������Է���id��email��ַ). <br>\n",
               currentuser.email);
        printf("<input type=submit value=ȷ��ת��></form>");
}

int do_fwd(struct fileheader *x, char *board, char *target)
{
        FILE *fp, *fp2;
        char title[512], buf[512], path[200], i;
        sprintf(path, "boards/%s/%s", board, x->filename);
        if(!file_exist(path))
                http_fatal("�ļ������Ѷ�ʧ, �޷�ת��");
        sprintf(title, "[ת��] %s", x->title);
        title[60]=0;
        post_mail(target, title, path, currentuser.userid, currentuser.username, fromhost, -1,1);
        printf("������ת�ĸ�'%s'<br>\n", nohtml(target));
        printf("[<a href='javascript:history.go(-2)'>����</a>]");
}
