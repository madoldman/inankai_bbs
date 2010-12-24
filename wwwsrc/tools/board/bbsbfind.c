/*$Id: bbsbfind.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        int num=0, total=0, type, dt, m=0, g=0, og=0, u=0;
        char dir[80], title[80], title2[80], title3[80], board[80], userid[80], buf[80];
        struct shortfile *brd;
        struct fileheader x;
        init_all();
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        printf("<center>%s -- ������������<hr color=green><br>\n", BBSNAME);
        type=atoi(getparm("type"));
        strsncpy(board, getparm("board"), 30);
        if(type==0)
                return show_form(board);
        strsncpy(title, getparm("title"), 60);
        strsncpy(title2, getparm("title2"), 60);
        strsncpy(title3, getparm("title3"), 60);
        strsncpy(userid, getparm("userid"), 60);
        dt=atoi(getparm("dt"));
        if(!strcasecmp(getparm("m"), "on"))
                m=1;
        if(!strcasecmp(getparm("g"), "on"))
                g=1;
        if(!strcasecmp(getparm("og"), "on"))
                og=1;
        if(!strcasecmp(getparm("u"), "on"))
                u=1;
        if(dt<0)
                dt=0;
        if(dt>9999)
                dt=9999;
        brd=getbcache(board);
        if(brd==0)
                http_fatal("�����������");
        strcpy(board, brd->filename);
        if(!has_read_perm(&currentuser, board))
                http_fatal("���������������");
        sprintf(dir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("�����������û��Ŀǰ����");
        printf("����������'%s'��, ���⺬: '%s' ", board, nohtml(title));
        if(title2[0])
                printf("�� '%s' ", nohtml(title2));
        if(title3[0])
                printf("���� '%s' ", nohtml(title3));
        if(g&&m)
                strcpy(buf, "������ժ");
        else if(g)
                strcpy(buf, "��ժ");
        else if(m)
                strcpy(buf, "����");
        else
                strcpy(buf, "");
        printf("����Ϊ: '%s', '%d'�����ڵ�%s����%s.<br>\n",
               userid[0] ? userid_str(userid) : "��������", dt, buf, u?"(��������)":"");
        printf("<table width=610>\n");
        printf("<tr><td>���<td>���<td>����<td>����<td>����\n");
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)==0)
                        break;
                num++;
                if(title[0] && !strcasestr(x.title, title))
                        continue;
                if(title2[0] && !strcasestr(x.title, title2))
                        continue;
                if(userid[0] && strcasecmp(x.owner, userid))
                        continue;
                if(title3[0] && strcasestr(x.title, title3))
                        continue;
                if(abs(time(0)-atoi(x.filename+2))>dt*86400)
                        continue;
                if(m && !(x.accessed[0] & FILE_MARKED))
                        continue;
                if(g && !(x.accessed[0] & FILE_DIGEST))
                        continue;
                if(og && !strncmp(x.title, "Re: ", 4))
                        continue;
                if(u && x.filename[STRLEN-3]!='U')
                        continue;
                total++;
                printf("<tr><td>%d", num);
                printf("<td>%s", flag_str(x.accessed[0]));
                printf("<td>%s", userid_str(x.owner));
                printf("<td>%12.12s", 4+Ctime(atoi(x.filename+2)));
                printf("<td><a href=bbscon?board=%s&file=%s&num=%d>%46.46s </a>\n", board, x.filename, num-1, x.title);
                if(total>=999)
                        break;
        }
        fclose(fp);
        printf("</table>\n");
        printf("<br>���ҵ� %d ƪ���·�������", total);
        if(total>999)
                printf("(ƥ��������, ʡ�Ե�1000�Ժ�Ĳ�ѯ���)");
        printf("<br>\n");
        printf("[<a href=bbsdoc?board=%s>���ر�������</a>] [<a href='javascript:history.go(-1)'>������һҳ</a>]", board);
        http_quit();
}

int show_form(char *board)
{
        printf("<table><form action=bbsbfind?type=1 method=post>\n");
        printf("<tr><td>��������: <input type=text maxlength=24 size=24 name=board value='%s'><br>\n", board);
        printf("<tr><td>���⺬��: <input type=text maxlength=50 size=20 name=title> AND ");
        printf("<input type=text maxlength=50 size=20 name=title2>\n");
        printf("<tr><td>���ⲻ��: <input type=text maxlength=50 size=20 name=title3>\n");
        printf("<tr><td>�����ʺ�: <input type=text maxlength=12 size=12 name=userid><br>\n");
        printf("<tr><td>ʱ�䷶Χ: <input type=text maxlength=4  size=4  name=dt value=7> ������<br>\n");
        printf("<tr><td>��ժ����: <input type=checkbox name=g> ");
        printf("<tr><td>��������: <input type=checkbox name=m> ");
        printf("<tr><td>��������: <input type=checkbox name=u> ");
        printf("<tr><td>��������: <input type=checkbox name=og><br><br>\n");
        printf("<tr><td><input type=submit value=�ݽ���ѯ���>\n");
        printf("</form></table>");
        printf("[<a href='bbsdoc?board=%s'>������һҳ</a>] [<a href=bbsfind>ȫվ���²�ѯ</a>]", board);
        http_quit();
}
