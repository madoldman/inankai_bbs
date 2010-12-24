/*$Id: bbsrst.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp,*fp2;
        struct shortfile *brd;
        struct fileheader f;
        struct userec *u;
        char buf[80], dir[80], path[80], board[80], file[80], *id;
	char jdir[80];
			char readbuf[512];
			char * pbuf,*pbuf2, *pbufi;
        int num=0;
        init_all();
        if(!loginok)
                http_fatal("���ȵ�¼");
        id=currentuser.userid;
        strsncpy(board, getparm("board"), 60);
        strsncpy(file, getparm("file"), 20);
        brd=getbcache(board);
        if(!has_BM_perm(&currentuser, board))
        	http_fatal("����Ȩ�ָ�����");
        if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                http_fatal("����Ĳ���");
        if(strstr(file, ".."))
                http_fatal("����Ĳ���");
        if(brd==0)
                http_fatal("�������");
        if(!has_post_perm(&currentuser, board))
                http_fatal("�����������");
        sprintf(dir, "boards/%s/.junk", board);
        sprintf(path, "boards/%s/%s", board, file);
	sprintf(jdir, "boards/%s/.DIR", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("����Ĳ���");
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {
		fp2=fopen(jdir, "a");
		sprintf(readbuf,"boards/%s/%s",board,f.filename);
		FILE * fp3=fopen(readbuf,"r");
		if (fp3)
		{
			*readbuf=0;
			fread(readbuf,1,500,fp3);
			pbuf=0;
			for (pbufi=readbuf;pbufi<readbuf+500;pbufi++)
			{
				if (*pbufi==0x0a)
					if (!strncmp(pbufi,"\x0a\xb1\xea\x20\x20\xcc\xe2\x3a",8))
						{
							pbuf=pbufi+9;
							break;
						}
			}
			if ((pbuf)&&(pbuf2=strchr(pbuf,0x0a)))
			{
				*pbuf2=0;
				strncpy(f.title,pbuf,STRLEN-1);
				*(f.title+STRLEN)=0;
			}
			fclose(fp3);
		}
		fwrite(&f, sizeof(struct fileheader), 1, fp2);
		fclose(fp2);
			del_record(dir, sizeof(struct fileheader), num);
                        f_append(path, buf);
                        printf("�ָ��ɹ�.<br><a href='bbsdoc?board=%s'>���ر�������</a>", board);
                        printf("<a href='bbsdoc?board=%s&junk=yes'>����վ</a>", board);
                        http_quit();
                }
                num++;
        }
        fclose(fp);
        printf("�ļ�������, �ָ�ʧ��.<br>\n");
        printf("<a href='bbsdoc?board=%s'>���ر�������</a>", board);
        printf("<a href='bbsdoc?board=%s&junk=yes'>����վ</a>", board);
        http_quit();
}
