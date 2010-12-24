/*$Id: bbs2an.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"
/*�Ľ���ʩ:Ӧ���ڴ����ļ���ͬʱ,�����ļ������.
*/

int main()
{
        int i, total=0, mode;
        char board[80], *ptr, buf[80];
        struct shortfile *brd;
        init_all();
        if(!loginok)
                http_fatal("���ȵ�¼");
        strsncpy(board, getparm("board"), 60);
        if(!(brd=getbcache(board)))http_fatal("�����������");
        strcpy(board, brd->filename);
        digest(board);
        printf("<a href='javascript:history.go(-1);'>�������,����</a>");
        http_quit();
}

int digest(char * board)
{
        char buf[PATHLEN],path[PATHLEN],cmd[PATHLEN+STRLEN];
        char file[80],title[80];
        FILE *fp;
        strsncpy(file,getparm("file"),60);
        int currboard=0;
        sethomefile(buf, currentuser.userid,".announcepath");
        if((fp = fopen(buf, "r")) == NULL ) {
                showinfo("����û���趨˿·,���趨˿·");
                return 1;
        }
        fscanf(fp,"%s",path);
        fclose(fp);
        if(!file_exist(path)) {
                printf("��Ч��·��:%s.˿·�Ѷ�ʧ,�������趨.",path);
                return 1;
        }
        sprintf(buf,"/%s",board);
        if(strstr(path,buf)&&has_BM_perm(&currentuser, board))currboard=1;
        char dir[STRLEN],filepath[STRLEN];
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(cmd,"%s/%s",path,file);
        if(file_exist(cmd))
        {
        	printf("<br>%s ��˿·���Ѿ����ڸ��ļ�,����",file);
        	return 1;
        }
        sprintf(filepath, "boards/%s/%s", board, file);
        fp=fopen(dir, "r+");
        if(fp==0) {
                showinfo("����İ���.");
                return 1;
        }
        struct fileheader f;
        int i=0;
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                i++;
                if(strcmp(f.filename, file))
                        continue;
                if((f.accessed[0] & FILE_VISIT) && currboard)
                	printf("<br>����:%s %s  ��ǰ�������������",file,f.title);
                if(currboard)
                {
                	f.accessed[0] |= FILE_VISIT;  // ����־��λ 
                	fseek (fp, -1 * sizeof (struct fileheader), SEEK_CUR);
                	fwrite (&f, sizeof (struct fileheader), 1, fp);
                }
                fclose (fp);
                sprintf(cmd,"%s/.Names",path);
                fp=fopen(cmd,"a+");
                if(fp==0) {
                        showinfo("���ܴ�.Names�ļ�,����");
                        return 1;
                }
                fprintf(fp,"Name=%s\nPath=~/%s\nNumb=\n#\n",f.title,file);
                fclose(fp);
                sprintf(cmd,"cp %s %s",filepath,path);
                system(cmd);
                return 0;
        }
        fclose(fp);
        showinfo("�ļ�������.\n");
        return 0;
}

