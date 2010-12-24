#define BRC_MAXSIZE     80000
#define BRC_MAXNUM      60
#define BRC_STRLEN      20
#define BRC_ITEMSIZE    (BRC_STRLEN + 1 + BRC_MAXNUM * sizeof( int ))
char    brc_buf[BRC_MAXSIZE];
int     brc_size;
char    brc_name[BRC_STRLEN];
int     brc_list[BRC_MAXNUM], brc_num;

char *brc_getrecord(char *ptr, char *name, int *pnum, int *list) {
        int     num;
        char   *tmp;
        strncpy(name, ptr, BRC_STRLEN);
        ptr += BRC_STRLEN;
        num = (*ptr++) & 0xff;
        tmp = ptr + num * sizeof(int);
        if (num > BRC_MAXNUM) num = BRC_MAXNUM;
        *pnum = num;
        memcpy(list, ptr, num * sizeof(int));
        return tmp;
}

char *brc_putrecord(char *ptr, char *name, int num, int *list) {
        if (num > 0 ) {
                if (num > BRC_MAXNUM) {
                        num = BRC_MAXNUM;
                }
                strncpy(ptr, name, BRC_STRLEN);
                ptr += BRC_STRLEN;
                *ptr++ = num;
                memcpy(ptr, list, num * sizeof(int));
                ptr += num * sizeof(int);
        }
        return ptr;
}

void brc_update(char *userid, char *board) {
        char    dirfile[STRLEN], *ptr;
        char    tmp_buf[BRC_MAXSIZE - BRC_ITEMSIZE], *tmp;
        char    tmp_name[BRC_STRLEN];
        int     tmp_list[BRC_MAXNUM], tmp_num;
        int     fd, tmp_size;
        ptr = brc_buf;
        if (brc_num > 0) {
                ptr = brc_putrecord(ptr, brc_name, brc_num, brc_list);
        }
        if (1) {
                sethomefile(dirfile, userid, ".boardrc");
                if ((fd = open(dirfile, O_RDONLY)) != -1) {
                        tmp_size = read(fd, tmp_buf, sizeof(tmp_buf));
                        close(fd);
                } else {
                        tmp_size = 0;
                }
        }
        tmp = tmp_buf;
        while (tmp < &tmp_buf[tmp_size] && (*tmp >= ' ' && *tmp <= 'z')) {
                tmp = brc_getrecord(tmp, tmp_name, &tmp_num, tmp_list);
                if (strncmp(tmp_name, board, BRC_STRLEN) != 0) {
                        ptr = brc_putrecord(ptr, tmp_name, tmp_num, tmp_list);
                }
        }
        brc_size = (int) (ptr - brc_buf);
        if ((fd = open(dirfile, O_WRONLY | O_CREAT, 0644)) != -1) {
                ftruncate(fd, 0);
                write(fd, brc_buf, brc_size);
                close(fd);
        }
}

int brc_init(char *userid, char *boardname) {
	FILE *fp;
        char dirfile[STRLEN], *ptr;
	int tmpno=0;
        sethomefile(dirfile, userid, ".boardrc");
        if(fp=fopen(dirfile, "r")) {
                brc_size=fread(brc_buf, 1, BRC_MAXSIZE, fp);
                fclose(fp);
        } else {
                brc_size = 0;
        }
        ptr=brc_buf;
        while(ptr< &brc_buf[brc_size] && (*ptr >= ' ' && *ptr <= 'z')) {
                ptr=brc_getrecord(ptr, brc_name, &brc_num, brc_list);
                if (!strncasecmp(brc_name, boardname, BRC_STRLEN)) return brc_num;
        }
        strncpy(brc_name, boardname, BRC_STRLEN);
        brc_list[0] = 1;
        brc_num=1;
        return 0;
}

int brc_add_read(char *filename) {
        int     ftime, n, i;
        if(!loginok) return;
        ftime=atoi(&filename[2]);
        if(filename[0]!='M' && filename[0]!='G')  return;
        if(brc_num<=0) {
                brc_list[brc_num++] = ftime;
                return;
        }
        for (n = 0; n < brc_num; n++) {
                if (ftime == brc_list[n]) {
                        return;
                } else if (ftime > brc_list[n]) {
                        if (brc_num < BRC_MAXNUM)
                                brc_num++;
                        for (i = brc_num - 1; i > n; i--) {
                                brc_list[i] = brc_list[i - 1];
                        }
                        brc_list[n] = ftime;
                        return;
                }
        }
        if(brc_num<BRC_MAXNUM) brc_list[brc_num++] = ftime;
}

int brc_un_read(char *file) {
        int ftime, n;
        if(file[0]!='M' && file[0]!='G') return 0;
	ftime=atoi(file+2);
        if (brc_num <= 0)
                return 1;
        for (n=0; n<brc_num; n++) {
                if(ftime>brc_list[n]) return 1;
                if(ftime==brc_list[n]) return 0;
        }
        return 0;
}

int brc_clear() {
	int i;
	char filename[20];
	if(!loginok) return;
	for(i=0; i<60; i++) {
		sprintf(filename, "M.%d.A", time(0)-i);
		brc_add_read(filename);
	}
}

int brc_has_read(char *file) {
	return !brc_un_read(file);
}

//huangxu@070917:moved from ../board/bbssnd.c

void lcase(char* pstr)//转换成小写字符 add by bluetent
{
        while(*pstr)
        {
                if(*pstr>='A'&&*pstr<='Z')
                        *pstr+=32;
                pstr++;
        }
}


/* add by bluetent 关键字过滤(FILTER) */
int bad_words(char *words)
{
        FILE *fp;
        char which[100];
        char badstr[100]="~!@ #$.%^&*()[]{}_+-|=\\`;,:'\"?<>/\n\t\r";
        char *tmp,*tmp2;
        int i,l,j;
        l=strlen(badstr);
        i=strlen(words);
        if(i<=1)
                return 0;
        if ((fp = fopen("etc/.badwords", "r")) == NULL)
                return 0;
        tmp2=tmp=(char *)malloc(i+1);
        //分配内存失败
        if(tmp2==NULL) {
                fclose(fp);
                return 0;
        }
        //过滤掉一些字符
        for(;*words;words++) {
                for(j=0; j<l; j++) {
                        if(*words==badstr[j])
                                break;
                }
                if(j == l) {
                        *tmp=*words;
                        tmp++;
                }
        }
        *tmp=0;
        /* words内容都是badstr里的 */
        if(tmp2==NULL) {
                fclose(fp);
                return 0;
        }
        while(fgets(which,100,fp)!=NULL) {
                strtok(which,": \n\r\t");
                lcase(tmp2);
                lcase(which);
                if (strstr(tmp2, which))
                        //              if(strstr(which, tmp2)!=NULL)
                {
                        fclose(fp);
                        free(tmp2);
                        if (!strcmp (u_info->userid, "efanlee") )
                        {
                                http_fatal (which);
                        }
                        return 1;
                }
        }
        fclose(fp);
        free(tmp2);
        return 0;
        /* add end */

}

int cmpboard(struct boardheader *b1,struct boardheader *b2)
{
	if((b1->flag & ZONE_FLAG)&&!(b2->flag & ZONE_FLAG))
        {
                return -1;
        }
        if(!(b1->flag & ZONE_FLAG)&&(b2->flag & ZONE_FLAG))
        {
                return 1;
        }
        return strcasecmp(b1->filename, b2->filename);
}

int count_zone(char *filename,char *zonename)
{
        int num=0;
        FILE *fp;
        char tempbuf[STRLEN];
        struct boardheader myheader;
        if((fp=fopen(filename,"r"))==NULL)
        {
                return 0;
        }
        while(fread(&myheader,sizeof(struct boardheader),1,fp)>0)
        {
                if((myheader.flag & CHILDBOARD_FLAG) && strcmp(myheader.owner,zonename)==0)                {
                        if(!(myheader.filename[0]))
                                continue;
                        if (!(myheader.level & PERM_POSTMASK) && !HAS_PERM(myheader.level) && !(myheader.level & PERM_NOZAP))
                                continue;
#ifdef BOARD_CONTROL
                        if( !HAS_PERM(PERM_SYSOP))
                        {
                                setbfile(tempbuf, myheader.filename, "board.allow");
                                if(dashf(tempbuf)&&!seek_in_file(tempbuf,currentuser.userid))
                                continue;
                        }
#endif
                        num++;
                }
                if((myheader.flag & ZONE_FLAG) && strcmp(myheader.owner,zonename)==0)
                {
                        if(count_zone(BOARDS,myheader.filename)!=0 || HAS_PERM(PERM_SYSOP))                                num++;
                }

        }
        return num;
}

int filenum(char *board)
{
        char file[256];
        sprintf(file, "boards/%s/.DIR", board);
        return file_size(file)/sizeof(struct fileheader);
}

int board_read(char *board)
{
        char buf[256], path[256];
        FILE *fp;
        struct fileheader x;
        int total;
        if(!loginok)
                return 1;
        bzero(&x, sizeof(x));
        sprintf(buf, "boards/%s/.DIR", board);
        total=file_size(buf)/sizeof(struct fileheader);
        if(total<=0)
                return 1;
        if (fp=fopen(buf, "r+"))
	{
        	fseek(fp, (total-1)*sizeof(struct fileheader), SEEK_SET);
        	fread(&x, sizeof(x), 1, fp);
        	fclose(fp);
	}
        brc_init(currentuser.userid, board);
        return brc_has_read(x.filename);
}

int titleComp(char * title1, char * title2)
{
	char * t1, * t2;
	t1 = strncmp(title1, "Re:", 3) ? title1 : (title1 + 4);
	t2 = strncmp(title2, "Re:", 3) ? title2 : (title2 + 4);
	return strncmp(t1, t2, 39);
}

int boardModeDir(const char * board, int boardmode, char * dir, char * modeinfo)
{
	int isbm;
	isbm = has_BM_perm(&currentuser, board);
	*dir = 0;
	if (*board == '*')
	{
		boardmode = 0;
		if (!strcmp(board, WML_VB_MAIL))
		{
			strcpy (modeinfo, "信件");
			if (loginok)
			{
				sprintf (dir, "mail/%c/%s/.DIR", toupper(currentuser.userid[0]), currentuser.userid);
			}
			else
			{
				strcpy (dir, "/tmp/not-exists");
			}
		}
		return 0;
	}
	if (boardmode == 4)//JUNK
	{
		if (!isbm && !HAS_PERM(PERM_FORCEPAGE))
		{
			boardmode = 0;
		}
		else
		{
			sprintf(dir, "boards/%s/.junk", board);
			strcpy(modeinfo, "回收");
		}
	}
	if (boardmode == 1)
	{
		sprintf(dir, "boards/%s/.DIGEST", board);
		strcpy(modeinfo, "文摘");
	}
	else if (boardmode == 8)
	{
		sprintf(dir, "boards/%s/.top", board);
		strcpy(modeinfo, "置顶");
	}
	if (!*dir)
	{
		boardmode = 0;
		sprintf(dir, "boards/%s/.DIR", board);
		strcpy(modeinfo, "普通");
	}
	return 0;
}

int fhfromdir(const char * fn, const char * board, int boardmode, struct fileheader * fhbuf)
//nffromdir简化版
{
	FILE * fp;
	struct fileheader fh[WML_TOPIC_RANGE];
	int i, j, posstart, max, cnt, posend, tmp;
	char buf[80], *ptr;
	char dir[256];
	memset (fhbuf, 0, sizeof (struct fileheader));
	boardModeDir(board, boardmode, dir, buf);
	fp = fopen(dir, "rb");
	if (!fp)
	{
		return -4;
	}
	*buf = 0;
	posstart = 0;
	posend = 0;
	cnt = 0;
	for (;;)
	{
		posend += fread (fh + posend, sizeof (struct fileheader), WML_TOPIC_RANGE - posend, fp);
		for (i = posstart; i < posend; i++, cnt++)
		{
			if (!strcmp((fh + i)->filename, fn))
			{
				memcpy (fhbuf, (fh + i), sizeof(struct fileheader));
				fclose(fp);
				return cnt;
			}
		}
		if (posend < WML_TOPIC_RANGE)
		{
			break;
		}
		posend = 0;
		posstart = 0;
	}
	fclose (fp);
	return -5;//not found
}

int writeBackDir(const char * dir, struct fileheader * fh)
{
	FILE * fp;
	struct fileheader f;
	int status;
	fp = fopen(dir, "rb+");
	if (!fp)
	{
		return -1;
	}
	flock (fileno(fp), LOCK_EX);
	status = -2;
	while (fread(&f, sizeof(struct fileheader), 1, fp))
	{
		if (!strcmp(fh->filename, f.filename))
		{
			fseek (fp, -sizeof(struct fileheader), 1);//current-mode
			fwrite (fh, sizeof(struct fileheader), 1, fp);
			status = 0;
			break;
		}
	}
	flock (fileno(fp), LOCK_UN);
	fclose (fp);
	return status;
}

int fileContMove(FILE * fp, int dst, int src, int len)
{
	const int IOBUFLEN = 1048576;
	char buf[IOBUFLEN];
	int currpos, offset, rem, currlen;
	offset = dst - src;
	if (!fp || !offset)
	{
		return 0;
	}
	currlen = 0;
	for (rem = len; rem > 0; rem -= currlen)
	{
		currlen = (rem < IOBUFLEN) ? rem : IOBUFLEN;
		currpos = dst + (offset < 0 ? len - rem : rem - currlen);
		fseek (fp, currpos + src, 0);
		fread (buf, 1, currlen, fp);
		fseek (fp, currpos + dst, 0);
		fwrite (buf, 1, currlen, fp);
	}
	if (offset < 0)
	{
		ftruncate (fileno(fp), dst + len);
	}
	return 0;
}

int seekDel(const char * dir, struct fileheader * fh)
{
	FILE * fp;
	struct fileheader f;
	int status, i;
	fp = fopen(dir, "rb+");
	if (!fp)
	{
		return -1;
	}
	flock (fileno(fp), LOCK_EX);
	status = -2;
	while (fread(&f, sizeof(struct fileheader), 1, fp))
	{
		if (!strcmp(fh->filename, f.filename))
		{
			i = ftell(fp);
			fseek (fp, 0, 2);
			fileContMove(fp, i - sizeof(struct fileheader), i, ftell(fp) - i);
			status = 0;
			break;
		}
	}
	flock (fileno(fp), LOCK_UN);
	fclose (fp);
	return status;
}

