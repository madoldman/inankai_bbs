/*$Id: bbsdict.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"

int check(unsigned char *word)
{
        int i, c;
        for(i=0; i<strlen(word); i++) {
                c=toupper(word[i]);
                if(c>='A' && c<='Z')
                        continue;
                if(c>=160 && c<=254)
                        continue;
                if(c=='.' || c=='*')
                        continue;
                http_fatal("����Ĳ���");
        }
}


int main()
{
        char word[80], buf[256];
        int type;
        int has=1;	//wzk
        init_all();
        modify_mode(u_info,DICT+20000);	//bluetent
        type=atoi(getparm("type"));
        strcpy(word, getparm("word"));
        printf("%s -- �ֵ��ѯ<hr color=green>\n", BBSNAME);
        if(type==0 || word[0]==0) {
                printf("<form name=dict method=post action=bbsdict?type=1><pre>\n");
                hprintf("���ֵ� [1;33mCDICT5.0 [m��¼���ʽ� 40000 , һ��ϳ����ĵ��ʺ���д������¼,\n");
                printf("��Ӣ���������, ֧��'*'��׺ͨ���.\n\n");
                printf("������Ҫ��ѯ����Ӣ�ĵ���: <input type=text name=word>\n");
                printf("<input type=submit>\n");
                printf("</pre></form><script language=javascript>dict.word.focus()</script>\n");
        } else {
                char file[80];
                FILE *fp;
                printf("��ѯ����: [%s]<br>\n", nohtml(word));
                sprintf(file, "tmp/%d.tmp", getpid());
                check(word);
                sprintf(buf, "bin/dict '%s' > %s", word, file);
                system(buf);
                fp=fopen(file, "r");
                if(fp==0)
                        http_fatal("�ڲ�����");

                printf("<table width=610 border=0><tr><td><pre>\n");
                fgets(buf, 255, fp);		//wzk
                printf("<br>");			//wzk
                while(1) {
                        if(fgets(buf, 255, fp)==0) {
                                if (has)
                                        printf("����\"%s\"û���ҵ�", word);
                                break;
                        }
                        has=0;		//wzk
                        hprintf("%s", buf);
                }
                printf("</pre></table>");
                unlink(file);
        }
        check_msg();//bluetent 2002.10.31
        http_quit();
}

