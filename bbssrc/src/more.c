// deardragon 2000.08.28  over
#include "bbs.h"
time_t  calltime = 0;
void    R_monitor();
struct shortfile *getbcache();

struct ACSHM
{
	char    data[ACBOARD_MAXLINE][ACBOARD_BUFSIZE];
	int     movielines;
	int     movieitems;
	time_t  update;
};

struct ACSHM *movieshm = NULL;
int     nnline = 0, xxxline = 0;
char    more_buf[MORE_BUFSIZE];
int     more_size, more_num;
int strsncpy(char *s1, char *s2, int n) 
{
	int l=strlen(s2);
	int i;
	if(n<0) return;
	if(n>l+1) n=l+1;
	for(i=n-1;i>=0;i--)
	{
		if(s2[i]==32 || s2[i]==10 || s2[i]==16)
		{
			s2[i]=0;
			n--;
		}
		else
		{
			break;
		}
	}
	strncpy(s1, s2, n);
        s1[n]=0;
}

void ActiveBoard_Init( void )
{
	struct fileheader fh;
	FILE   *fp;
	char   *ptr;
	char    buf[1024], buf2[1024];
	struct stat st;
	int     max = 0, i = 0, j = 0, x, y = 0;
	int     flag; /* flag = 1 ¼´Îª¹ýÂÇµô "--\n" ÒÔááÖ®ÈÎºÎÄÚÈÝ */

	if( movieshm == NULL )
		movieshm = (void *) attach_shm("ACBOARD_SHMKEY", 4123, sizeof(*movieshm));

	if (stat("boards/notepad/.DIGEST", &st) < 0)
	{
		empty_movie(1);
		return ;
	}
	if (movieshm->update > st.st_mtime)
		return ;

	for (i = 0; i < ACBOARD_MAXLINE; i++)
		movieshm->data[i][0] = 0;

	max = get_num_records("boards/notepad/.DIGEST", sizeof(fh));

	i = 1;
	j = 0;
	while (i <= max && j < ACBOARD_MAXLINE)
	{
		get_record("boards/notepad/.DIGEST", &fh, sizeof(fh), i++);
		sprintf(buf, "boards/notepad/%s", fh.filename);

		fp = fopen(buf, "r");
		if (fp == NULL)
			continue;

		y++;		/* record how many files have been append */

		if (fh.title[0] == '$')
			flag = (int) (fh.title[1] - '0');
		else
			flag = 4;
		for (x = 0; x < flag; x++)  // Ìø¹ýÍ·²¿ÐÅÏ¢
			fgets(buf, 1024, fp);

		flag = 0;
		for (x = 0; x < MAXMOVIE - 1 && j < ACBOARD_MAXLINE; x++)
		{
			if (fgets(buf, 1024, fp) != 0)
			{
				buf[ACBOARD_BUFSIZE-4] = '\0';
				if (flag == 1 || strcmp(buf, "--\n") == 0)
				{
					strcpy(buf2, "[K");
					flag = 1;
				}
				ptr = movieshm->data[j];
				if (flag == 0)
				{
					strcpy(buf2, "[K");
					strcat(buf2, buf);
				}
				buf2[ACBOARD_BUFSIZE-1] = '\0';
				memcpy(ptr, buf2, ACBOARD_BUFSIZE);
			}
			else
			{ /* no data handling */
				strcpy(movieshm->data[j], "[K");
			}
			j++;
		}
		fclose(fp);
	}
	if (j == 0)
	{
		empty_movie(3);
		return ;
	}
	movieshm->movielines = j;
	movieshm->movieitems = y;
	movieshm->update = time(0);

	sprintf(buf, "»î¶¯¿´°å¸üÐÂ, ¹² %d ÐÐ, %d ²¿·Ý.", j, y);
	report(buf);
	return ;
}

int empty_movie(int x)
{
	sprintf(genbuf, "Empty Movie!!! (error = %d)", x);
	report(genbuf);

	strcpy(movieshm->data[2], "[K      ** ÉÐÎ´Éè¶¨»î¶¯¿´°å ** ");
	strcpy(movieshm->data[3], "[K         ÇëÏê¼û°²×°ËµÃ÷Êé Firebird-2000 ");
	strcpy(movieshm->data[4], "[K         Éè¶¨ notepad °å");

	movieshm->movielines = MAXMOVIE;
	movieshm->movieitems = 1;
	movieshm->update = time(0);

}

void setcalltime( void )
{
	char    ans[6];
	int     ttt;
	move(1, 0);
	clrtoeol();
	getdata(1, 0, "¼¸·ÖÖÓºóÒªÏµÍ³ÌáÐÑÄã: ", ans, 3, DOECHO, YEA);
	ttt = atoi(ans);
	if (ttt <= 0)
		return;
	calltime = time(0) + ttt * 60;
}

int readln(int fd, char *buf)
{
	int     len, bytes, in_esc, ch,c;
	len = bytes = in_esc = c = 0;
	while (1)
	{
		if (more_num >= more_size)
		{
			more_size = read(fd, more_buf, MORE_BUFSIZE);
			if (more_size == 0)
				break;
			more_num = 0;
		}
		ch = more_buf[more_num++];
		bytes++;
		if (ch == '\n' || bytes > 255)
			break;
		else if (ch == '\t')
		{
			c = 0;
			do
			{
				len++, *buf++ = ' ';
			}
			while ((len % 4) != 0);
		}
		else if (ch == '')
		{
			c = 0;
			if (showansi)
				*buf++ = ch;
			in_esc = 1;
		}
		else if (in_esc)
		{
			if (showansi)
				*buf++ = ch;
			if (strchr("[0123456789;,", ch) == NULL)
				in_esc = 0;
		}
		else if (isprint2(ch))
		{
			if(ch & 0x80 )
				c ++;
			else
				c = 0;
			if ((len==79&&c%2==1)||len > 79)
				break;
			len++;
			*buf++ = ch;
		}
	}
	if(ch != '\n'&&more_num>0)
		more_num --;
	*buf++ = '\n';
	*buf = '\0';
	return bytes;
}

int morekey( void )
{
	int     ch;
	while (1)
	{
		switch (ch = egetch())
		{
#if 0
		case '$':
		case KEY_TAB:
		case 'G':
		case Ctrl('S'):
						return KEY_TAB;
		case Ctrl('A'):
					case Ctrl('T'):
							return Ctrl('T');
#endif

		case 'q':
		case KEY_LEFT:
		case EOF:
			return KEY_LEFT;
		case ' ':
		case KEY_RIGHT:
		case KEY_PGDN:
		case Ctrl('F'):
						return KEY_RIGHT;
		case KEY_PGUP:
		case Ctrl('B'):
						return KEY_PGUP;
		case '\r':
		case '\n':
		case KEY_DOWN:
		case 'j':
			return KEY_DOWN;
		case 'k':
		case KEY_UP:
			return KEY_UP;
		case 'h':
		case 'H':
		case '?':
			return 'H';
		case '/':
			return '/';//add by tdhlshx for ËÑË÷×Ö·û´® 03.12.13
		case 'd':
		case 'D':
			return 'D';
			//add by yiyo ÔÄ¶ÁÎÄÕÂÊ±²é×Öµä
		case 'y':
		case 'Y':
		case 'n':
		case 'N':
		case 'r':
		case 'R':
		case 'c':
		case 'C':
		case 'm':
		case 'M':
		case KEY_TAB:
			return toupper(ch);
		default:
			;
		}
	}
}

int seek_nth_line(int fd, int no)  // ´ÓÎÄ¼þÍ·¶Áµ½µÚ no ÐÐ( ¶¨Î»µ½ no ÐÐ )
{
	int     n_read, line_count, viewed;
	int	len, in_esc, bytes,c;
	char   *p, *end, ch;
	lseek(fd, 0, SEEK_SET);
	line_count = viewed = 0;
	in_esc = len = bytes = c = 0;
	more_num = MORE_BUFSIZE + 1;
	if (no == 0)
		return 0;
	while (1)
	{
		do
		{
			n_read = read(fd, more_buf, MORE_BUFSIZE);
		}
		while(n_read < 0 && errno == EINTR);
		if(n_read == 0)
			break;
		p = more_buf;
		end = p + n_read;
		for (; p < end && (line_count<no||no<0); p++)
		{
			bytes ++;
			ch = *p;
			if (ch == '\n'|| bytes > 255)
			{
				line_count++;
				bytes = 0;
				len = 0;
				c = 0;
			}
			else if (ch == '\t')
			{
				len += 4;
				c = 0;
			}
			else if (ch == '')
			{
				in_esc = 1;
				c = 0;
			}
			else if (in_esc)
			{
				if (strchr("[0123456789;,", ch) == NULL)
					in_esc = 0;
			}
			else if (isprint2(ch))
			{
				if(ch & 0x80)
					c ++ ;
				else
					c =0;
				if ((len == 79 && c !=0 && c%2 ==0)||len > 79)
				{
					line_count ++;
					len = 0;
					bytes = 0;
					c = 0;
				}
				else
				{
					len++;
				}
			}
		}
		if (no > 0 && line_count >= no)
		{
			viewed += (p - more_buf);
			lseek(fd, (off_t) viewed, SEEK_SET);
			break;
		}
		else
		{
			viewed += n_read;
		}
	}
	//more_num = MORE_BUFSIZE + 1;	/* invalidate the readln()'s buffer */
	if(no < 0)
		return line_count;
	return viewed;
}
/*Add by SmallPig*/
int countln(char *fname)
{
	FILE   *fp;
	char    tmp[256];
	int     count = 0;

	if ((fp = fopen(fname, "r")) == NULL)
		return 0;

	while (fgets(tmp, sizeof(tmp), fp) != NULL)
		count++;
	fclose(fp);
	return count;
}

/* below added by netty  *//* Rewrite by SmallPig */
void netty_more()
{
	char    buf[256];
	int     ne_row = 1;
	int     x, y;
	time_t  thetime = time(0);

	getyx(&y, &x);
	update_endline();
	if (!DEFINE(DEF_ACBOARD))
		return;
	nnline = (thetime / 10 % movieshm->movieitems) * (MAXMOVIE - 1);

	while ((nnline < movieshm->movielines))
	{
#ifdef BIGGER_MOVIE
		move(1 + ne_row, 0);
#else

		move(2 + ne_row, 0);
#endif

		clrtoeol();
		strcpy(buf, movieshm->data[nnline]);
		showstuff(buf/*, 0*/);
		nnline = nnline + 1;
		ne_row = ne_row + 1;
		if (nnline == movieshm->movielines)
		{
			nnline = 0;
			break;
		}
		if (ne_row > MAXMOVIE - 1)
		{
			break;
		}
	}
	move(y, x);
}

void printacbar()
{
#ifndef BIGGER_MOVIE
	struct shortfile *bp;
	int x,y;

	getyx(&y,&x);
	bp = getbcache(DEFAULTBOARD);
	move(2,0);
	if(bp->flag&VOTE_FLAG)
		prints(" [1;36m©°¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©È[37mÏµÍ³Í¶Æ±ÖÐ [ Config->Vote ] [36m©À¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©´ [m\n");
	else
		prints(" [1;36m©°¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©È[37m»î  ¶¯  ¿´  °å[36m©À¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©´ [m\n");
	move(2+MAXMOVIE,0);
	prints(" [1;36m©¸¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©¼[m\n");
	move (y,x);
#endif

	refresh();
}

void check_calltime()
{
	int     line;
	if ( calltime != 0 && time(0) >= calltime )
	{
		if (uinfo.mode == TALK)
			line = t_lines / 2 - 1;
		else
			line = 0;

		saveline(line, 0);	/* restore line */
		bell();
		bell();
		bell();
		move(line, 0);
		clrtoeol();
		prints("[1;44;32mÏµÍ³Í¨¸æ: [37m%-65s[m", "ÏµÍ³ÄÖÖÓ Áå¡«¡«¡«¡«¡«¡«");
		igetkey();
		move(line, 0);
		clrtoeol();
		saveline(line, 1);
		calltime = 0;
	}
}

void R_monitor()
{
	if (uinfo.mode != MMENU)
		return ;
	if (!DEFINE(DEF_ACBOARD) && !DEFINE(DEF_ENDLINE))
		return;

	alarm(0);
	signal(SIGALRM, R_monitor);
	netty_more();
	printacbar();
	if (!DEFINE(DEF_ACBOARD))
		alarm(55);
	else
		alarm(10);
}

#ifdef BELL_DELAY_FILTER
void bell_delay_filter(char *buf)
{
	int i;
	char *t;
	t = strstr(buf,"\033");
	if(t == NULL)
		return ;
	for(i = 1;strchr("[0123456789; ",t[i]);i++)
		;
	if(t[i] == '\107' || t[i] == '\115' )
		*t ='\052';
	bell_delay_filter(t+1);
}
#endif

/*   modified by yiyo ÔöÇ¿ÔÄ¶ÁÎÄÕÂ¹¦ÄÜ¼ü   */
/*int
mem_printbotline(int l1, int l2, int read, int size)
{
	extern int t_lines;
	static int n = 0;
	char *s[4]= {
	                "½áÊø ¡û q | ¡ü ¡ý PgUp PgDn ÒÆ¶¯",
	                "s ¿ªÍ· | e Ä©Î² | b f ÏòÇ°ºó·­Ò³",
	                "g Ìøµ½Ö¸¶¨ÐÐ | ? / ÏòÉÏÏÂËÑË÷×Ö·û´®",
	                "nÏÂÒ»Æª | lÉÏÒ»Æª | R »ØÎÄ | d ²é×Öµä"
	            };
	n++;
	if (uinfo.mode == READING)
		n %= 4;
	else
		n %= 3;
	move(t_lines - 1, 0);
	clrtoeol();
	prints("\033[1;44;32mÏÂÃæ»¹ÓÐà¸ (%d%%) µÚ(%d-%d)ÐÐ \033[33m| %s | h ¸¨ÖúËµÃ÷\033[m",100 * read / size,l1,l2,s[n]);
}*/

/*rawmore2() ansimore2() Add by SmaLLPig*/
int rawmore(char *filename, int promptend, int row, int numlines, int stuffmode)
{
	extern int t_lines;
	struct stat st;
	int     fd, tsize;
	char    buf[256];
	int     i, ch, viewed, pos, isin = NA, titleshow = NA;
	int     numbytes;
	int     curr_row = row;
	int     linesread = 0;
	if ((fd = open(filename, O_RDONLY)) == -1)
	{
		return -1;
	}
	if (fstat(fd, &st))
	{
		close(fd); /* add by yiyo maybe bug */
		return -1;
	}

	tsize = st.st_size;
	more_size = more_num = 0;
	clrtobot();
	i = pos = viewed = 0;
	numbytes = readln(fd, buf);
	curr_row++;
	linesread++;
	while (numbytes)
	{
		if (linesread <= numlines || numlines == 0)
		{
			viewed += numbytes;
#ifdef BELL_DELAY_FILTER

			bell_delay_filter(buf);
#endif

			if (     !titleshow && (!strncmp(buf, "¡õ ÒýÓÃ", 7))
			         ||(!strncmp(buf, "==>", 3))
			         || (!strncmp(buf, "¡¾ ÔÚ", 5))
			         ||(!strncmp(buf, "¡ù ÒýÊö", 7)))
			{
				prints("[1;33m%s[m", buf);
				titleshow = YEA;
			}
			else if (buf[0] != ':' && buf[0] != '>' || buf[1] != ' ')
			{
				if (isin == YEA)
				{
					isin = NA;
					//prints("[37m");
					prints("[m");
				}
				if (check_stuffmode() || stuffmode == YEA)
					showstuff(buf/*, 0*/);
				else
					prints("%s", buf);
			}
			else
			{
				prints("[36m");
				if (check_stuffmode() || stuffmode == YEA)
					showstuff(buf/*, 0*/);
				else
					prints("%s", buf);
				isin = YEA;
			}
			i++;
			pos++;
			if (pos == t_lines)
			{
				scroll();
				pos--;
			}
			numbytes = readln(fd, buf);
			curr_row++;
			linesread++;
			if (numbytes == 0)
				break;
			if (i == t_lines - 1)
			{
				if (showansi)
				{
					move(t_lines - 1, 0);
					prints("[0m[m");
					refresh();
				}
				move(t_lines - 1, 0);
				clrtoeol();
				prints("[1;44;32mÏÂÃæ»¹ÓÐà¸ (%d%%)[33m   ©¦ ½áÊø ¡û <q> ©¦ ¡ü/¡ý/PgUp/PgDn ÒÆ¶¯ ©¦ ? ¸¨ÖúËµÃ÷ ©¦     [m", (viewed * 100) / tsize);
				//	    mem_printbotline(curr_row-23, curr_row-1,viewed, tsize);//modified by yiyo
				ch = morekey();
				move(t_lines - 1, 0);
				clrtoeol();
				refresh();
				if (ch == KEY_LEFT)
				{
					close(fd);
					return ch;
				}
				else if (ch == KEY_RIGHT)
				{
					i = 1;
				}
				else if (ch == KEY_DOWN)
				{
					i = t_lines - 2;
#if 0

				}
				else if (ch == Ctrl('T'))
				{
					i = pos = 0;
					clear();
					curr_row = 0;
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row ++;
				}
				else if (ch == KEY_TAB)
				{
					clear();
					i = pos = 0;
					curr_row = seek_nth_line(fd,-1) - t_lines + 2;
					if(curr_row < 0)
						curr_row = 0;
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row++;
#endif

				}
				else if (ch == KEY_PGUP || ch == KEY_UP)
				{
					clear();
					i = pos = 0;
					curr_row -= (ch == KEY_PGUP) ? (2 * t_lines - 2) : (t_lines + 1);
					if (curr_row < 0)
					{
						close(fd);
						return ch;
					}
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row++;

				}
	
				else if (ch == 'D')
				{
					x_dict();
					i = pos = 0;
					curr_row -= (t_lines);
					if (curr_row < 0)
						curr_row = 0;
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row++;
					//add by yiyo ÔÄ¶ÁÎÄÕÂÊ±²é×Öµä

				}
				else if (ch == 'H')
				{
					show_help("help/morehelp");
					i = pos = 0;
					curr_row -= (t_lines);
					if (curr_row < 0)
						curr_row = 0;
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row++;
				}
			}
		}
		else
			break;	/* More Than Want */
	}
	close(fd);
	if (promptend)
	{
		pressanykey();
	}
	return 0;
}


int rawmore_topten(char *filename, int promptend, int row, int numlines, int stuffmode)
{
	extern int t_lines;
	struct stat st;
	FILE	*fp;
	int     fd, tsize;
	char    buf[256];
	char	tempbuf[256];
	char	brd[256],id[16],title[256];
	char	s1[256];
	int     i, ch, viewed, pos, isin = NA, titleshow = NA;
	int     numbytes;
	int     curr_row = row;
	int     linesread = 0;
	int 	index;
	if ((fd = open(filename, O_RDONLY)) == -1)
	{
		return -1;
	}
	if (fstat(fd, &st))
	{
		close(fd); /* add by yiyo maybe bug */
		return -1;
	}
	tsize = st.st_size;
	more_size = more_num = 0;
	clrtobot();
	i = pos = viewed = 0;
	numbytes = readln(fd, buf);
	curr_row++;
	linesread++;
	while (numbytes)
	{
		if (linesread <= numlines || numlines == 0)
		{
			viewed += numbytes;
#ifdef BELL_DELAY_FILTER
			bell_delay_filter(buf);
#endif
			if (     !titleshow && (!strncmp(buf, "¡õ ÒýÓÃ", 7))
			         ||(!strncmp(buf, "==>", 3))
			         || (!strncmp(buf, "¡¾ ÔÚ", 5))
			         ||(!strncmp(buf, "¡ù ÒýÊö", 7)))
			{
				prints("[1;33m%s[m", buf);
				titleshow = YEA;
			}
			else if (buf[0] != ':' && buf[0] != '>' || buf[1] != ' ')
			{
				if (isin == YEA)
				{
					isin = NA;
					//prints("[37m");
					prints("[m");
				}
				if (check_stuffmode() || stuffmode == YEA)
					showstuff(buf/*, 0*/);
				else
					prints("%s", buf);
			}
			else
			{
				prints("[36m");
				if (check_stuffmode() || stuffmode == YEA)
					showstuff(buf/*, 0*/);
				else
					prints("%s", buf);
				isin = YEA;
			}
			i++;
			pos++;
			if (pos == t_lines)
			{
				scroll();
				pos--;
			}
			numbytes = readln(fd, buf);
			curr_row++;
			linesread++;
			if (numbytes == 0)
				break;
		}
		else
			break;	/* More Than Want */
	}
	close(fd);
	if (promptend)
	{
		sprintf(tempbuf,"%s","\033[33m                 °´ÏàÓ¦Êý×Ö£¨1£­9£­0£©½øÈëÏàÓ¦°åÃæ£¬°´ÆäËûÈÎÒâ¼ü·µ»Ø\033[0m");
		move(t_lines - 1,0);
		prints(tempbuf);
		ch=egetch();
		if(ch>='0'&&ch<='9')
		{
			if((fp=fopen(filename,"r"))==NULL)
			{
				return 0;
			}
			fgets(s1,255,fp);
			fgets(s1,255,fp);
			index=ch-'0';
			if(index==0)
			{
				index=10;
			}
			for(i=1;i<index;i++)
			{
				if(fgets(s1,255,fp)<0)
				{
					break;
				}	
				if(fgets(s1,255,fp)<0)
				{
					break;
				}
			}
			fgets(s1, 255, fp);
                	sscanf(s1+45, "%s", brd);
                	sscanf(s1+122, "%s", id);
                	fgets(s1, 255, fp);
                	strsncpy(title, s1+27, 60);
			fclose(fp);
			do_select_topten(brd,id,title);
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

int mesgmore(char *filename, int promptend, int row, int numlines, int mode)
{
	extern int t_lines;
	struct stat st;
	int     fd, tsize;
	char    buf[256];
	char    oldfilename[STRLEN];
        char    cmd[2*STRLEN];
	int     i, ch, viewed, pos, isin = NA;
	int     numbytes;
	int     curr_row = row;
	int     linesread = 0;
	time_t  now;
	char    title[256];

	if ((fd = open(filename, O_RDONLY)) == -1)
		return -1;
	if (fstat(fd, &st))
		return -1;

	tsize = st.st_size;
	more_size = more_num = 0;

	clrtobot();
	i = pos = viewed = 0;
	numbytes = readln(fd, buf);
	curr_row++;
	linesread++;
	while (numbytes)
	{
		if (linesread <= numlines || numlines == 0)
		{
			viewed += numbytes;
			prints("[37m");
			if (check_stuffmode())
				showstuff(buf/*, 0*/);
			else
				prints("%s", buf);
			isin = YEA;
			i++;
			pos++;
			if (pos == t_lines)
			{
				scroll();
				pos--;
			}
			numbytes = readln(fd, buf);
			curr_row++;
			linesread++;
			if (numbytes == 0)
				break;
			if (i == t_lines - 1)
			{
				if (showansi)
				{
					move(t_lines - 1, 0);
					prints("[0m[m");
					refresh();
				}
				move(t_lines - 1, 0);
				clrtoeol();
				prints("[0m[1;44;32m(%d%%) ÊÇ·ñ¼ÌÐø [[1;37mY/n[1;32m]   ±£Áô <[1;37mr[32m>    Çå³ý <[1;37mc[1;32m>   ¼Ä»ØÐÅÏä <[1;37mm[1;32m>                      [m", (viewed * 100) / tsize);
				ch = morekey();
				move(t_lines - 1, 0);
				clrtoeol();
				refresh();
				if (ch == KEY_LEFT)
				{
					close(fd);
					return ch;
				}
				else if (ch == KEY_RIGHT)
				{
					i = 1;
				}
				else if (ch == KEY_DOWN)
				{
					i = t_lines - 2;
#if 0

				}
				else if (ch == Ctrl('T'))
				{
					i = pos = 0;
					clear();
					curr_row = 0;
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row ++;
				}
				else if (ch == KEY_TAB)
				{
					clear();
					i = pos = 0;
					curr_row = seek_nth_line(fd,-1) - t_lines + 2;
					if(curr_row < 0)
						curr_row = 0;
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row++;
#endif

				}
				else if (ch == KEY_PGUP || ch == KEY_UP)
				{
					clear();
					i = pos = 0;
					curr_row -= (ch == KEY_PGUP)?(2*t_lines - 2) : (t_lines + 1);
					if (curr_row < 0)
					{
						close(fd);
						return ch;
					}
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row++;
				}
				else if (ch == 'H')
				{
					show_help("help/msghelp");
					i = pos = 0;
					curr_row -= (t_lines);
					if (curr_row < 0)
						curr_row = 0;
					viewed = seek_nth_line(fd, curr_row);
					numbytes = readln(fd, buf);
					curr_row++;
				}
				else if (ch == 'C')
				{
					if(mode==YEA)
                                        {
                                                close(fd);
	                                        strcpy(oldfilename,filename);
						strcat(oldfilename,".old");
                                                strcpy(cmd,"mv ");
                                                strcat(cmd,filename);
                                                strcat(cmd," ");
                                                strcat(cmd,oldfilename);
                                                system(cmd);
                                        }
                                        else
                                        {
                                                close(fd);
                                                unlink(filename);
                                        }
					return ch;
				}
				else if (ch == 'M')
				{
					close(fd);
					now = time(0);
					getdatestring(now,NA);
					sprintf(title, "[%s] ËùÓÐÑ¶Ï¢±¸·Ý", datestring);
					mail_file(filename, currentuser.userid, title);
					unlink(filename);
					return ch;
				}
				else if (ch == 'N' || ch == 'R')
				{
					close(fd);
					return ch;
				}
				else if (ch == 'Y')
				{
					i = 1;
				}
			}
		}
		else
			break;	/* More Than Want */
	}

	close(fd);
	if (promptend == YEA || pos < t_lines)
		msgmorebar(filename,mode);
	return ch;
}

int ansimore(char *filename, int promptend)
{
	int     ch;
	clear();
	ch = rawmore(filename, promptend, 0, 0, NA);
	move(t_lines - 1, 0);
	prints("[0m[m");
	refresh();
	return ch;
}
int ansimore_topten(char *filename,int promptend)
{
	int     ch;
	clear();
	ch = rawmore_topten(filename, promptend, 0, 0, NA);
	move(t_lines - 1, 0);
	prints("[0m[m");
	refresh();
	return ch;
}

int ansimore2(char *filename, int promptend, int row, int numlines)
{
	int     ch;
	ch = rawmore(filename, promptend, row, numlines, NA);
	refresh();
	return ch;
}
/* edwardc.990624 ÏÈÔÝÊ±ÓÃ ansimore3() ´úÌæ ... */

int ansimore3(char *filename, int promptend)
{
	int     ch;
	clear();
	ch = rawmore(filename, promptend, 0, 0, YEA);
	move(t_lines - 1, 0);
	prints("[0m[m");
	refresh();
	return ch;
}

// deardragon 2000.08.28  over
