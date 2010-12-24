/*$Id: bbsdel.c,v 1.2 2008-12-19 09:55:03 madoldman Exp $
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
        int num=0;
        init_all();
        if(!loginok)
                http_fatal("���ȵ�¼");
        id=currentuser.userid;
        strsncpy(board, getparm("board"), 60);
        strsncpy(file, getparm("file"), 20);
        brd=getbcache(board);
        if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                http_fatal("����Ĳ���");
        if(strstr(file, ".."))
                http_fatal("����Ĳ���");
        if(brd==0)
                http_fatal("�������");
        if(!has_post_perm(&currentuser, board))
                http_fatal("�����������");
        sprintf(dir, "boards/%s/.DIR", board);
        sprintf(path, "boards/%s/%s", board, file);
	sprintf(jdir, "boards/%s/.junk", board);
        fp=fopen(dir, "r");
        if(fp==0)
                http_fatal("����Ĳ���");
        while(1) {
                if(fread(&f, sizeof(struct fileheader), 1, fp)<=0)
                        break;
                if(!strcmp(f.filename, file)) {//huangxu@060715:web��ɾ��m����
                        if((strcasecmp(id, f.owner)||(f.accessed[0]&FILE_MARKED)) && !has_BM_perm(&currentuser, board))
                                http_fatal("����Ȩɾ������");
		fp2=fopen(jdir, "a");
		fwrite(&f, sizeof(struct fileheader), 1, fp2);
		fclose(fp2);
			if(!HAS_PERM(PERM_SYSOP))//huangxu@060406:deleted��junk�Ѿ�����
      				if(strstr(board,"syssecurity")||(!strcmp(board,"Punishment")&&!HAS_PERM(PERM_BLEVELS)))
					http_fatal("����Ȩɾ������");
			del_record(dir, sizeof(struct fileheader), num);
                        sprintf(buf, "\n�� %s �� %s ɾ����Web�E[FROM: %s]", currentuser.userid, Ctime(time(0))+4, fromhost);
                        f_append(path, buf);
                /*        if(!strcmp(f.owner, currentuser.userid)) {
                                post_article("junk", f.title, path, f.owner, "", fromhost, -1);
                        } else {
                                post_article("deleted", f.title, path, f.owner, "", fromhost, -1);
                        }*/
                        //unlink(path);
                        printf("ɾ���ɹ�.<br><a href='bbsdoc?board=%s'>���ر�������</a>", board);
                        u=getuser(f.owner);
                        if(!junkboard(board) && u) {
                                if(u->numposts>0)
                                        u->numposts--;
                                save_user_data(u);
                        }
                        sprintf(buf, "%s %-12s bbsdel %s\n", Ctime(time(0))+4, id, board);
                        f_append("trace", buf);
                        http_quit();
                }
                num++;
        }
        fclose(fp);
        printf("�ļ�������, ɾ��ʧ��.<br>\n");
        printf("<a href='bbsdoc?board=%s'>���ر�������</a>", board);
        http_quit();
}
