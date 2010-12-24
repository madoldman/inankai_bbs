/*$Id: bbsnetupload2.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
static struct dir x;

int main()
{
        char file[80], name[80], fileurl[150];
        int i, level, pid;
        int maxsize=102400;//������ϴ�������С
        //	int currexp=0;//�û��ľ���ֵ
        //	currexp=countexp(&currentuser);
        init_all();
        if(!loginok)
                http_fatal("�Ҵҹ����޷�ִ�б����������ȵ�¼");
        if(currentuser.stay/3600<100)
                http_fatal("��վ�ۼ�ʱ��δ��100Сʱ����Ȩʹ�ñ�����");
        if((time(0)-currentuser.firstlogin)/86400<60)
                http_fatal("ע���ʺ�ʱ��δ����60�죬��Ȩʹ�ñ�����");
        x.active=1;
        x.level=atoi(getparm("level"));
        strsncpy(x.board, getparm("userid"), 20);
        strsncpy(x.exp, getparm("exp"), 72);
        strsncpy(x.userid, currentuser.userid, 13);
        strsncpy(x.showname, getparm("name"), 36);
        do_check(x.showname);
        x.date=time(0);
        strsncpy(x.type, getparm("type"), 30);
        x.live=30;
        for(i=0; x.showname[i]; i++) {
                int c=x.showname[i];
                if(c<-1)
                        continue;
                if(isalnum(c) || c=='.' || c=='-')
                        continue;
                x.showname[i]='_';
        }
        //	level=atoi(getparm("level"));
        pid=atoi(getparm("file"));
        sprintf(file, "tmp/%d.netsave", pid);
        strsncpy(name, getparm("name"), 30);
        //	if(!has_post_perm(&currentuser, x.board)) http_fatal("���������������Ȩ��������������");
        if(!file_exist(file)) {
                http_fatal("������ļ���");
        }
        x.size=file_size(file);
        //	if(x.size<=0)http_fatal("��û���ϴ��κ����ݣ��뷵��");
        {
                char buf[256];
                sprintf(buf, "%16.16s %s %d %s %s %s\n",
                        Ctime(time(0))+4, currentuser.userid, x.size, fromhost, x.board, x.showname);
                f_append("netsave.log", buf);
        }
        /*	if(x.size>maxsize) {
        		unlink(file);
        		http_fatal("�ļ�̫��");
        	}*/
        upload(file, &x);
        unlink(file);
        sprintf(fileurl, "http://%s/cgi-bin/bbs/netshowfile?name=%s",BBSHOST, x.showname);//bluetent
        printf("�������سɹ�, ");
        /*	printf("URLΪ <font color=green>%s </font><br>\n", fileurl);
        	printf("��URL�Ѿ��Զ�׷�ӵ����ڷ��������ĩβ.");
        	printf("(����URL(��ɫ����)�ɿ�����������ֱ������, ϵͳ���ж������Զ����ɳ�����.)<br>\n");
        	printf("�粻����, ��ֻ���ڰ������������������������.<hr color=green>\n");
        	printf("�����ȼ�: %d��<br>\n", x.level+1);
        	printf("����ʱ��: %d��<br>\n", x.live);
        */	
        printf("�ļ���С: %d�ֽ�<br>\n", x.size);
        printf("�ļ�����: %s<br>\n", x.type);
        printf("�ļ�����: %s<br>\n", x.showname);
        printf("������ID: %s<br>\n", x.userid);
        printf("����ʱ��: %s<br>\n", Ctime(time(0))+4);
        //	printf("���ذ���: %s<br>\n", x.board);
        printf("�ļ�˵��: %s<br>\n", nohtml(x.exp));
        //	printf("<hr color=green><a href='javascript:window.close()'>�رմ˴���</a>\n");
        printf("<hr color=green><a href=bbsnetfdoc>��������洢ѡ��</a>\n");
        //add by bluetent 2002.12.22
        /*	strsncpy(closewindow, getparm("closewindow"), 10);
                if (!strcmp("yes", closewindow))
                        printf("
        				<script language=javascript>
        		        opener.document.bbspstform.text.value += ' %s ';
        				</script>
                        ", fileurl);
         
        	//add end*/
        http_quit();
}

int badfile(char *filename)
{
        int i;
        for(i=0; filename[i]; i++)
                if(!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", filename[i]))
                        return 1;
        return 0;
}

int upload(char *file, struct dir *x)
{
        FILE *fp;
        int t=time(0);
        char buf[256], dir[256];
        x->filename=t;
        sprintf(buf, NETSAVE_PATH"/%d", t);
        link(file, buf);
        sprintf(dir, NETSAVE_PATH"/.DIR");
        fp=fopen(dir, "a");
        fwrite(x, sizeof(struct dir), 1, fp);
        fclose(fp);
        return 0;
}

int do_check(char *show)
{
        FILE *fp;
        struct dir x;
        show[30]=0;
        fp=fopen(NETSAVE_PATH"/.DIR", "r");
        if(fp==0)
                return;
        while(1) {
                if(fread(&x, sizeof(x), 1, fp)<=0)
                        break;
                if(!strcmp(x.showname, show)) {
                        sprintf(show, "%d%s", time(0), x.showname);
                        fclose(fp);
                        return;
                }
        }
        fclose(fp);
}
