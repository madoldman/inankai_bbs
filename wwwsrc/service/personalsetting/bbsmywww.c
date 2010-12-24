/*$Id: bbsmywww.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char *ptr, path[256], buf[256], buf1[256], buf2[256];
        int t_lines=20, link_mode=0, def_mode=0, localpic_mode=0, type,highspeed=1;
        int showhead_mode=0;
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ��Ͳ��ܶ��ƽ���");
        modify_mode(u_info,USERDEF+20000);	//bluetent
        sprintf(path, "home/%c/%s/.mywww", toupper(currentuser.userid[0]), currentuser.userid);
        fp=fopen(path, "r");
        if(fp) {
                while(1) {
                        if(fgets(buf, 80, fp)==0)
                                break;
                        if(sscanf(buf, "%80s %80s", buf1, buf2)!=2)
                                continue;
                        if(!strcmp(buf1, "t_lines"))
                                t_lines=atoi(buf2);
                        if(!strcmp(buf1, "link_mode"))
                                link_mode=atoi(buf2);
                        if(!strcmp(buf1, "def_mode"))
                                def_mode=atoi(buf2);
                        if(!strcmp(buf1, "localpic_mode"))
                                localpic_mode=atoi(buf2);
                        if(!strcmp(buf1, "showhead_mode"))
                                showhead_mode=atoi(buf2);
                        if(!strcmp(buf1, "highspeed"))
                                highspeed=atoi(buf2);

                }
                fclose(fp);
        }
        type=atoi(getparm("type"));
        ptr=getparm("t_lines");
        if(ptr[0])
                t_lines=atoi(ptr);
        ptr=getparm("link_mode");
        if(ptr[0])
                link_mode=atoi(ptr);
        ptr=getparm("def_mode");
        if(ptr[0])
                def_mode=atoi(ptr);
        ptr=getparm("localpic_mode");
        if(ptr[0])
                localpic_mode=atoi(ptr);
        ptr=getparm("showhead_mode");
        if(ptr[0])
                showhead_mode=atoi(ptr);
        ptr=getparm("highspeed");
        if(ptr[0])
                highspeed=atoi(ptr);


        printf("<center>%s -- ���˶��� [ʹ����: %s]<hr color=green>", BBSNAME, currentuser.userid);
        if(type>0)
                return save_set(path, t_lines, link_mode, def_mode, localpic_mode, showhead_mode,highspeed);
        printf("<table>\n");
        if(t_lines<10 || t_lines>40)
                t_lines=20;
        if(link_mode<0 || link_mode>1)
                link_mode=0;
        printf("<tr><td><form action=bbsmywww>\n");
        printf("<input type=hidden name=type value=1>");
        printf("��������: <input name=t_lines size=8 value=%d><br> ������ʱһ����ʾ����������10-40 \n", t_lines);
        printf("<br><br>����ʶ��: <input name=link_mode size=8 value=%d><br>�������������е����ӣ�1��ʶ�����ı���ʽ��ʾ���ӣ� 0ʶ��������ͨ����������ӷ���������ҳ��\n", link_mode);
        //	printf("ȱʡģʽ (0һ��, 1����): &nbsp;&nbsp;<input name=def_mode size=8 value=%d><br>\n", def_mode);
        printf("<br><br>ͼƬ��ʾ: <input name=localpic_mode size=8 value=%d><br> 0��ʶ��Ǳ�վͼƬ����;1ʶ��", localpic_mode);
        //printf("��ʾ��������Ϣ�� (0��ʾ, 1����ʾ): &nbsp;&nbsp;<input name=showhead_mode size=8 value=%d><br>\n", showhead_mode);
        printf("<br><br>��������: <input name=highspeed size=8 value=%d><br> 0�������磬���е�ͼƬ�������ӵķ�ʽ��ʾ�� ���ں����ر��ͼƬ��ҳ�棬���Լӿ�����ٶȣi<br> 1�������磬ֱ����ʾ����ͼƬ��\n", highspeed);
        printf("<tr><td align=center><input type=submit value=ȷ��> <input type=button onclick=\"javascript:history.go(-1)\" value=����>");
        printf("</form>\n");
}

int save_set(char *path, int t_lines, int link_mode, int def_mode, int localpic_mode, int showhead_mode,int highspeed)
{
        FILE *fp;
        char buf[80];
        if(t_lines<10 || t_lines>40)
                http_fatal("���������");
        if(link_mode<0 || link_mode>1)
                http_fatal("���������ʶ�����");
        if(def_mode<0 || def_mode>1)
                http_fatal("�����ȱʡģʽ");
        if(localpic_mode<0 || localpic_mode>1)
                http_fatal("�����ʶ��ģʽ");
        if(showhead_mode<0 || showhead_mode>1)
                http_fatal("�����ʶ��ģʽ");
        if(highspeed<0 || highspeed>1)
                http_fatal("���������ģʽ");

        fp=fopen(path, "w");
        fprintf(fp, "t_lines %d\n", t_lines);
        fprintf(fp, "link_mode %d\n", link_mode);
        fprintf(fp, "def_mode %d\n", def_mode);
        fprintf(fp, "localpic_mode %d\n", localpic_mode);
        fprintf(fp, "showhead_mode %d\n", showhead_mode);
        fprintf(fp, "highspeed %d\n", highspeed);
        fclose(fp);
        sprintf(buf, "%d", t_lines);
        setcookie("my_t_lines", buf);
        sprintf(buf, "%d", link_mode);
        setcookie("my_link_mode", buf);
        sprintf(buf, "%d", def_mode);
        setcookie("my_def_mode", buf);
        sprintf(buf, "%d", localpic_mode);
        setcookie("my_localpic_mode", buf);
        sprintf(buf, "%d", showhead_mode);
        setcookie("my_showhead_mode", buf);
        sprintf(buf, "%d", highspeed);
        setcookie("highspeed", buf);
        showinfo("���Ʋ����趨�ɹ�.<br>\n");
}
