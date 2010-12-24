/*$Id: bbsgcon.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        FILE *fp;
        char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
        struct fileheader x;
        int num, tmp, total;
        init_all();
        modify_mode(u_info,READING+20000);	//bluetent
        strsncpy(board, getparm("board"), 32);
        strsncpy(file, getparm("file"), 32);
        num=atoi(getparm("num"));
        printf("<center>\n");
        if(!has_read_perm(&currentuser, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal("�����������");
        if (!canenterboard (currentuser.userid, board) && !HAS_PERM(PERM_SYSOP))
                http_fatal ("���Ѿ���ȡ�����뱾���Ȩ��");
        strcpy(board, getbcache(board)->filename);
        printf("%s -- �����Ķ� [������: %s]<hr>", BBSNAME, board);
        if(strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                http_fatal("����Ĳ���1");
        if(strstr(file, "..") || strstr(file, "/"))
                http_fatal("����Ĳ���2");
        sprintf(dir, "boards/%s/.DIGEST", board);
        total=file_size(dir)/sizeof(x);
        if(total<=0)
                http_fatal("�������������ڻ���Ϊ��");
        printf("<table width=610 border=0>\n");
        printf("<tr><td>\n<pre>");
        sprintf(filename, "boards/%s/%s", board, file);
        fp=fopen(filename, "r");
        if(fp==0)
                http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
        while(1) {
                if(fgets(buf, 512, fp)==0)
                        break;
                hhprintf("%s", void1(buf));
        }
        fclose(fp);
        printf("</pre>\n</table><hr>\n");
        printf("[<a href=bbssec>����������</a>]");
        printf("[<a href=bbsall>ȫ��������</a>]");
        fp=fopen(dir, "r+");
        if(fp==0)
                http_fatal("dir error2");
        if(num>0) {
                fseek(fp, sizeof(x)*(num-1), SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                printf("[<a href=bbscon?board=%s&file=%s&num=%d>��һƪ</a>]", board, x.filename, num-1);
        }
        printf("[<a href=bbsdoc?board=%s>��������</a>]", board);
        if(num<total-1) {
                fseek(fp, sizeof(x)*(num+1), SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                printf("[<a href=bbscon?board=%s&file=%s&num=%d>��һƪ</a>]", board, x.filename, num+1);
        }
        if(num>0 && num<=total) {
                fseek(fp, sizeof(x)*num, SEEK_SET);
                fread(&x, sizeof(x), 1, fp);
                //		(*(int*)(x.title+72))++;
                //		fseek(fp, sizeof(x)*num, SEEK_SET);
                //		fwrite(&x, sizeof(x), 1, fp);
        }
        fclose(fp);
#ifdef MUST_EXISTING_USER
	if(!http_postReadable(&x))http_fatal("���û�δע��");
#endif

ptr=x.title;
        if(!strncmp(ptr, "Re: ", 4))
                ptr+=4;
        printf("[<a href='bbstfind?board=%s&title=%s'>ͬ�����Ķ�</a>]\n", board, void1(ptr));
        printf("</center>\n");
        http_quit();
}
