/*$Id: bbsparm.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"

char *defines[] = {
                          "�������ر�ʱ���ú��Ѻ���",     /* DEF_FRIENDCALL */
                          "���������˵�ѶϢ",             /* DEF_ALLMSG */
                          "���ܺ��ѵ�ѶϢ",               /* DEF_FRIENDMSG */
                          "�յ�ѶϢ��������",             /* DEF_SOUNDMSG */
                          "��վ����ʾ�����¼",		/* DEF_NOTSHOWMSG */
                          "��ʾ�����",             	/* DEF_ACBOARD */
                          "��ʾѡ����ѶϢ��",             /* DEF_ENDLINE */
                          "�༭ʱ��ʾ״̬��",     	/* DEF_EDITMSG */
                          "ѶϢ������һ��/����ģʽ",	/* DEF_NOTMSGFRIEND */
                          "ѡ������һ��/����ģʽ",	/* DEF_NORMALSCR */
                          "������������ New ��ʾ",	/* DEF_NEWPOST */
                          "�Ķ������Ƿ�ʹ���ƾ�ѡ��",   	/* DEF_CIRCLE */
                          "�Ķ������α�ͣ춵�һƪδ��",	/* DEF_FIRSTNEW */
                          "��վʱ��ʾ��������",   	/* DEF_LOGFRIEND */
                          "������վ֪ͨ",                 /* DEF_LOGINFROM */
                          "�ۿ����԰�",                   /* DEF_NOTEPAD*/
                          "��Ҫ�ͳ���վ֪ͨ������",       /* DEF_NOLOGINSEND */
                          "����ʽ����",                   /* DEF_THESIS */
                          "�յ�ѶϢ�Ⱥ��Ӧ�����",       /* DEF_MSGGETKEY */
                          "��վʱ�ۿ���վ�˴�ͼ",         /* DEF_GRAPH */
                          "��վʱ�ۿ�ʮ�����а�",         /* DEF_TOP10 */
                          "ʹ������ǩ����",		/* DEF_RANDSIGN */
                          "��ʾ����",			/* DEF_S_HOROSCOPE */
                          "������������",			/* DEF_FRIEND_INFO */
                          "ʹ��\'+\'���δ������",	/* DEF_NOT_N_MASK */
                          "��������ɾ��",                 /* DEF_DELDBLCHAR */
                          "�Զ��Ű���Ԥ��Ϊ 78 ��",	/* DEF_AUTOWRAP */
                          "ʹ��GB���Ķ�",                 /* DEF_USEGB KCN 99.09.03 */
                          "�������Լ��� IP",		/* DEF_NOTHIDEIP */
                          "���±��ʹ�ò�ɫ",		/* DEF_COLORMARK */
                          "��ʾ��̬����",		/* DEF_SYSMSG*/
                          "��������ʱ��ʱ����ѶϢ",       /* DEF_POSTNOMSG */
                          NULL
                  };
int main()
{
        int i, perm=1, type;
        init_all();
        type=atoi(getparm("type"));
        printf("<center>%s -- �޸ĸ��˲��� [ʹ����: %s]<hr color=green>\n", BBSNAME, currentuser.userid);
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ����趨����");
        modify_mode(u_info,USERDEF+20000);	//bluetent
        if(type)
                return read_form();
        printf("<form action=bbsparm?type=1 method=post>\n");
        printf("<table width=610>\n");
        for(i=0; defines[i]; i++) {
                char *ptr="";
                if(i%2==0)
                        printf("<tr>\n");
                if(currentuser.userdefine & perm)
                        ptr=" checked";
                printf("<td><input type=checkbox name=perm%d%s><td>%s", i, ptr, defines[i]);
                perm=perm*2;
        }
        printf("</table>");
        printf("<input type=submit value=ȷ���޸�></form><br>���ϲ���������telnet��ʽ�²�������\n");
        http_quit();
}

int read_form()
{
        int i, perm=1, def=0;
        char var[100];
        for(i=0; i<32; i++) {
                sprintf(var, "perm%d", i);
                if(strlen(getparm(var))==2)
                        def+=perm;
                perm=perm*2;
        }
        currentuser.userdefine=def;
        save_user_data(&currentuser);
        printf("���˲������óɹ�.<br><a href=bbsparm>���ظ��˲�������ѡ��</a>");
}
