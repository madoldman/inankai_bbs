/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
$Id: read.c,v 1.5 2010-07-05 04:58:44 madoldman Exp $
*/

#include "bbs.h"

#define PUTCURS   move(3+locmem->crs_line-locmem->top_line,0);prints(">");
#define RMVCURS   move(3+locmem->crs_line-locmem->top_line,0);prints(" ");

struct fileheader SR_fptr;
int     SR_BMDELFLAG = NA;
char   *pnt;
int	my_get_records (char *, char *, int, int, int);
int	get_records ();
int	kaka_pic (void);
int editpattern();
extern int	noreply;
extern int 	temphotinfonumber;
extern int local_article;
extern int curr_ent;//add by bluetent 2003.4.19
extern int digestmode;//added by tdhlshx 2004.3.7
struct keeploc
{
	char   *key;
	int     top_line;
	int     crs_line;
	struct keeploc *next;
};

/*struct fileheader *files = NULL;*/
char    currdirect[STRLEN];
char    keyword[STRLEN];	/* for Ïà¹ØÖ÷Ìâ */
int     screen_len;
int     last_line;
int	old_last_line;
int		AddCombineMode;
struct keeploc* getkeep(char* s, int def_topline, int def_cursline)
{
	static struct keeploc *keeplist = NULL;
	struct keeploc *p;
	for (p = keeplist; p != NULL; p = p->next)
	{
		if (!strcmp(s, p->key))
		{
			if (p->crs_line < 1)
				p->crs_line = 1;	/* DAMMIT! - rrr */
			return p;
		}
	}
	p = (struct keeploc *) malloc(sizeof(*p));
	p->key = (char *) malloc(strlen(s) + 1);
	strcpy(p->key, s);
	p->top_line = def_topline;
	p->crs_line = def_cursline;
	p->next = keeplist;
	keeplist = p;
	return p;
}

void fixkeep(char* s, int first, int last)
{
	struct keeploc *k;
	k = getkeep(s, 1, 1);
	if (k->crs_line >= first)
	{
		k->crs_line = (first == 1 ? 1 : first - 1);
		k->top_line = (first < 11 ? 1 : first - 10);
	}
}

void modify_locmem(struct keeploc* locmem, int total)
{
	if (locmem->top_line > total)
	{
		locmem->crs_line = total;
		locmem->top_line = total - t_lines / 2;
		if (locmem->top_line < 1)
			locmem->top_line = 1;
	}
	else if (locmem->crs_line > total)
	{
		locmem->crs_line = total;
	}
}

int move_cursor_line(struct keeploc* locmem, int mode)
{
	int     top, crs;
	int     reload = 0;
	top = locmem->top_line;
	crs = locmem->crs_line;
	if (mode == READ_PREV)
	{
		if (crs <= top)
		{
			top -= screen_len - 1;
			if (top < 1)
				top = 1;
			reload = 1;
		}
		crs--;
		if (crs < 1)
		{
			crs = 1;
			reload = -1;
		}
	}
	else if (mode == READ_NEXT)
	{
		if (crs + 1 >= top + screen_len)
		{
			top += screen_len - 1;
			reload = 1;
		}
		crs++;
		if (crs > last_line)
		{
			crs = last_line;
			reload = -1;
		}
	}
	locmem->top_line = top;
	locmem->crs_line = crs;
	return reload;
}

void draw_title(int (*dotitle)())
{
	clear();
	(*dotitle) ();
}

void draw_entry(char* (*doentry)(), struct keeploc* locmem, int num, int ssize)
{
	char   *str;
	int     base, i;
	int	n;
	//	char	buf [256];
	setbfile (genbuf, currboard, ".top");
	n = get_num_records (genbuf, sizeof (struct fileheader));
	base = locmem->top_line;
	move(3, 0);
	clrtobot();
	for (i = 0; i < num; i++)
	{
		str = (*doentry) (base + i, &pnt[i * ssize]);
		if (!check_stuffmode())
			prints("%s", str);
		else
			showstuff(str);
		prints("\n");
	}
	/*
		setbfile (genbuf, currboard, ".top");
		n = get_num_records (genbuf, sizeof (struct fileheader));
		if (num < screen_len && !strcmp (currentuser.userid, "efanlee"))
		{
			FILE *fp;
			struct fileheader fh;
			time_t filetime;
			fp = fopen (genbuf, "r");
			if (fp) {
				for (i = num; i < num + n && i < screen_len; ++i)
				{
					fread (&fh, sizeof (struct fileheader), 1, fp);
					filetime = atoi (fh.filename +2);
					if (fh.title[47])
						strcpy (fh.title + 44, "...");
					sprintf (str, " \33[33m[ÌáÊ¾] \33[m%-12.12s %6.6s ¡ñ %-.45s",
					getname(fh.owner), (filetime>740000000)?ctime(&filetime)+4:"", fh.title);
					prints ("%s\n", str);
				}
			}
		}
	*/
	move(t_lines - 1, 0);
	clrtoeol();
	update_endline();
}



void i_read(int cmdmode,char* direct,int (*dotitle)(),char* (*doentry)(),struct one_key *rcmdlist,int ssize)
{
	extern int talkrequest;
	extern int friendflag;
	struct keeploc *locmem;
	char    lbuf[11];
	char    *ptr;
	int     lbc, recbase, mode, ch;
	int     num, entries;
	int	ntop = 0;
	int	size = sizeof (struct fileheader);
	screen_len = t_lines - 4;//ÆÁÄ»ÏÔÊ¾ÄÚÈİÌõÄ¿Êı£¬t_lines=24
	modify_user_mode(cmdmode);
	ptr = pnt = calloc(screen_len, ssize);
	strcpy(currdirect, direct);
	draw_title(dotitle);
	setbfile (genbuf, currboard, ".top");
	ntop = get_num_records (genbuf, sizeof (struct fileheader));//ÖÃµ×ÎÄÕÂÊıÄ¿
	last_line = get_num_records(currdirect, ssize);//ÌõÄ¿Êı
	
	if (cmdmode == READING)
		last_line += ntop;

	old_last_line=last_line;
	if (last_line == 0)
	{
		if (cmdmode == RMAIL)
		{
			move(10, 30);
			prints("Ã»ÓĞÈÎºÎĞÅ¼ş...");
			pressreturn();
			clear();
		}
		else
#ifdef MAILRECYCLE
			if(cmdmode==R_RMAIL)
			{
				prints("ÄãµÄ»ØÊÕÕ¾Îª¿Õ...");
				pressreturn();
				clear();
			}
			else
#endif


				if (cmdmode == GMENU)
				{
					char    desc[5];
					char    buf[40];
					if (friendflag)
						strcpy(desc, "ºÃÓÑ");
					else
						strcpy(desc, "»µÈË");
					sprintf(buf, "Ã»ÓĞÈÎºÎ%s (A)ĞÂÔö%s (Q)Àë¿ª£¿[Q] ", desc, desc);
					getdata(t_lines - 1, 0, buf, genbuf, 4, DOECHO, YEA);
					if (genbuf[0] == 'a' || genbuf[0] == 'A')
						(friendflag) ? friend_add() : reject_add();
				}
				else if(uinfo.mode!=DIGEST)
				{
					getdata(t_lines - 1, 0, "¿´°åĞÂ³ÉÁ¢ (P)·¢±íÎÄÕÂ (Q)Àë¿ª£¿[Q] ",
					        genbuf, 4, DOECHO, YEA);
					if (genbuf[0] == 'p' || genbuf[0] == 'P')
						do_post();
				}
				else
				{
					move(10, 10);
					prints("    ÄúÉĞÎ´Ïò¾«»ªÇøÊÕ²Ø¼ĞÖĞÌí¼ÓÈÎºÎÄÚÈİ, ÇëÔÚÔÄ¶Á¾«»ªÇøÊ±\n");
					move(11, 10);
					prints("°´Ctrl+A½«¹â±êËùÖ¸Î»ÖÃµÄÄ¿Â¼»òÎÄ¼şÊÕ²ØÖÁ´Ë¡£\n");
					pressanykey();
				}
		free(pnt);
		return;
	}

	/* Efan: ÎÄÕÂÖÃ¶¥	*/
	setbfile (genbuf, currboard, ".top");
	ntop = get_num_records (genbuf, size);
	num = last_line - screen_len + 2;
	locmem = getkeep(currdirect, num < 1 ? 1 : num, last_line);
	modify_locmem(locmem, last_line);
	if(temphotinfonumber!=0)
        {
                locmem->crs_line=temphotinfonumber;
                if(temphotinfonumber>=10)
                {
                        locmem->top_line=temphotinfonumber-10;
                }
                else
                {
                        locmem->top_line=1;
                }
                temphotinfonumber=0;
        }
	recbase = locmem->top_line;
	entries = get_records(currdirect, pnt, ssize, recbase, screen_len);
	setbfile (genbuf, currboard, ".top");
	if (entries < screen_len && cmdmode == READING)
	{
		int fd =0;
		int nread = screen_len - entries;
		if (nread > MAX_BRD_ONTOP)
			nread = MAX_BRD_ONTOP;
		fd = open (genbuf, O_RDONLY);
		if (fd != -1)
		{
			int i = read (fd, pnt + entries * ssize, nread * ssize);
			if (i>0)
				entries += i / ssize;
			/* by brew 05.6.22  */
                        /* must close the fd!!!*/
                        close(fd);
		}
	}
	draw_entry(doentry, locmem, entries, ssize);
	PUTCURS;
	lbc = 0;
	mode = DONOTHING;
	while ((ch = egetch()) != EOF)
	{
		setbfile (genbuf, currboard, ".top");
		ntop = get_num_records (genbuf, size);
		if (talkrequest)
		{
			talkreply();
			mode = FULLUPDATE;
		}
		else if (ch >= '0' && ch <= '9'||
		         ((Ctrl('H') == ch || '\177' == ch) && lbc > 0) )
		{
			if(Ctrl('H') == ch || '\177' == ch)
				lbuf[lbc--] = 0;
			else if (lbc < 7)
				lbuf[lbc++] = ch;
			lbuf[lbc] = 0;
			if(!lbc)
				update_endline();
			else if(DEFINE(DEF_ENDLINE))
			{
				extern time_t login_start_time;
				int allstay;
				char pntbuf[256], buf[80];
				allstay = (time(0) - login_start_time)/60;
				sprintf(buf, "[[36m%.12s[33m]", currentuser.userid);
				sprintf( pntbuf,"[1;44;33m[[36mÄ¿Ç°ÄúÒª×ªµ½µÚ [35m%7.7s[36m ÆªÎÄÕÂ[33m][[36m%4d[33mÈË/[1;36m%3d[33mÓÑ][[36m%1s%1s%1s%1s%1s%1s[33m]ÕÊºÅ%-24s[[36m%3d[33m:[36m%2d[33m][m",
				         lbuf,count_users,count_friends,(uinfo.pager&ALL_PAGER)?"P":"p",
				         (uinfo.pager&FRIEND_PAGER)?"O":"o",(uinfo.pager&ALLMSG_PAGER)?"M":"m",
				         (uinfo.pager&FRIENDMSG_PAGER)?"F":"f",(DEFINE(DEF_MSGGETKEY))?"X":"x",
				         (uinfo.invisible==1)?"C":"c",buf,(allstay /60)%1000,allstay%60);
				move(t_lines-1, 0);
				clrtoeol();
				prints(pntbuf);
			}    //add by yiyo ÏÔÊ¾Ìø×ªÎÄÕÂÊı

		}
		else if (lbc > 0 && (ch == '\n' || ch == '\r'))
		{
			update_endline();//add by yiyo
			lbuf[lbc] = '\0';
			lbc = atoi(lbuf);
			if (cursor_pos(locmem, lbc, 10, 0))
				mode = PARTUPDATE;
			lbc = 0;
		}
		else
		{
			if(lbc)
				update_endline();//add by yiyo
			lbc = 0;
			mode = i_read_key(rcmdlist, locmem, ch, ssize);

			while (mode == READ_NEXT || mode == READ_PREV)
			{
				int     reload;
				reload = move_cursor_line(locmem, mode);
				if (reload == -1)
				{
					mode = FULLUPDATE;
					break;
				}
				else if (reload)
				{
					recbase = locmem->top_line;
					entries = get_records(currdirect, pnt, ssize,
					                      recbase, screen_len);
					setbfile (genbuf, currboard, ".top");
					if (entries < screen_len && cmdmode == READING)
					{
						int fd =0;
						int nread = screen_len - entries;
						if (nread > MAX_BRD_ONTOP)
							nread = MAX_BRD_ONTOP;
						fd = open (genbuf, O_RDONLY);
						if (fd != -1)
						{
							int i = read (fd, pnt + entries * ssize, nread * ssize);
							if (i>0)
								entries += i / ssize;
							/* by brew 05.6.22  */
                                                /* must close the fd!!!*/
        	                                        close(fd);

						}
					}

					if (entries <= 0)
					{
						last_line = -1;
						old_last_line=last_line;
						break;
					}
				}
				num = locmem->crs_line - locmem->top_line;
				mode = i_read_key(rcmdlist, locmem, ch, ssize);
			}
			modify_user_mode(cmdmode);
		}
		if (mode == DOQUIT)
		{
			break;
		}
		if (mode == GOTO_NEXT)
		{
			cursor_pos(locmem, locmem->crs_line + 1, 1, 0);
			mode = PARTUPDATE;
		}
		switch (mode)
		{
		case NEWDIRECT:
		case DIRCHANGED:
		case MODECHANGED: // chenhao ½â¾öÎÄÕÂÁĞ±í¿´ĞÅµÄÎÊÌâ
			recbase = -1;
			if (mode == MODECHANGED)
			{ // chenhao
				//setbdir(currdirect, currboard);
				//strcpy(currdirect, direct);//modified by yiyo
				if(uinfo.mode!=DIGEST&&cmdmode!= RMAIL)
					setbdir (currdirect, currboard);
				if(cmdmode == GMENU || cmdmode == R_RMAIL || cmdmode == DIGEST)
					strcpy(currdirect, direct);
				pnt = ptr;
			}
			last_line = get_num_records(currdirect, ssize);
			setbfile (genbuf, currboard, ".top");
			ntop = get_num_records (genbuf, sizeof (struct fileheader));
			if (cmdmode == READING)
				last_line += ntop;
			old_last_line=last_line;
			if (last_line == 0 && digestmode > 0)
			{
				acction_mode();
			}
			if (mode == NEWDIRECT)
			{
				num = last_line - screen_len + 1;
				locmem = getkeep(currdirect, num < 1 ? 1 : num, last_line);
			}
		case FULLUPDATE:
			draw_title(dotitle);
		case PARTUPDATE:
			setbfile (genbuf, currboard, ".top");
			ntop = get_num_records (genbuf, sizeof (struct fileheader));
			if (last_line < locmem->top_line + screen_len)
			{
				num = get_num_records(currdirect, ssize);
				setbfile (genbuf, currboard, ".top");
				ntop = get_num_records (genbuf, sizeof (struct fileheader));
				if (cmdmode == READING)
					num += ntop;
				if (last_line != num)
				{
					last_line = num;
					old_last_line=last_line;
					recbase = -1;
				}
			}
			if (last_line == 0)
			{
				prints("No Messages\n");
				entries = 0;
			}
			else if (recbase != locmem->top_line)
			{
				recbase = locmem->top_line;
				if (recbase > last_line - (cmdmode == READING)?ntop:0)
				{
					recbase = last_line - screen_len / 2;
					if (recbase < 1)
						recbase = 1;
					locmem->top_line = recbase;
				}
				entries = get_records(currdirect, pnt, ssize,
				                      recbase, screen_len);
				setbfile (genbuf, currboard, ".top");
				if (entries < screen_len && cmdmode == READING)
				{
					int fd =0;
					int nread = screen_len - entries;
					if (nread > MAX_BRD_ONTOP)
						nread = MAX_BRD_ONTOP;
					fd = open (genbuf, O_RDONLY);
					if (fd != -1)
					{
						int i = read (fd, pnt + entries * ssize, nread * ssize);
						if (i>0)
							entries += i / ssize;
						/* by brew 05.6.22  */
						/* must close the fd!!!*/
						close(fd);
					}
				
				}

			}
			entries = get_records (currdirect, pnt, ssize, recbase, screen_len);
			setbfile (genbuf, currboard, ".top");
			if (entries < screen_len && cmdmode == READING)
			{
				int fd =0;
				int nread = screen_len - entries;
				if (nread > MAX_BRD_ONTOP)
					nread = MAX_BRD_ONTOP;
				fd = open (genbuf, O_RDONLY);
				if (fd != -1)
				{
					int i = read (fd, pnt + entries * ssize, nread * ssize);
					if (i>0)
						entries += i / ssize;
				         /* by brew 05.6.19 */ 
                                         /* must close the fd!!!*/
                                                close(fd);

				}
			}

			if (locmem->crs_line > last_line)
				locmem->crs_line = last_line;
			draw_entry(doentry, locmem, entries, ssize);
			PUTCURS;
			break;
		default:
			break;
		}	/* Efan: End of switch	*/
		mode = DONOTHING;
		if (entries == 0)
			break;
	}	/* Efan: End of while	*/
	clear();
	free(pnt);
}

int my_get_records (char * filename, char * rptr, int size, int id, int number)
{
	int fd;
	int nret = id;
	if (id >= number)
		return id;
	fd = open (filename, O_RDONLY);
	if (fd != -1)
	{
		int i = read (fd, rptr + id * size, (((number-id)>2)?2:(number-id))*size);
		i /= size;
		if (i >0)
			nret += i;
		close(fd);//mod by brew
	}
//	close (fd);
	return nret;
}
extern char someoneID[31];
int i_read_key(struct one_key* rcmdlist, struct keeploc* locmem, int ch, int ssize)
{
	int	ntop;
	int     i, mode = DONOTHING,savemode;
	setbfile (genbuf, currboard, ".top");
	ntop = get_num_records (genbuf, sizeof (struct fileheader));
	if (ntop < 0 || uinfo.mode != READING)
		ntop = 0;
	last_line = get_num_records (currdirect, ssize);
	last_line += ntop;
	switch (ch)
	{
	case 'P':
	case Ctrl('B'):
				case KEY_PGUP:
						if (locmem->top_line > 1)
				{
					locmem->top_line -= screen_len - 1;
					if (locmem->top_line <= 0)
						locmem->top_line = 1;
					locmem->crs_line = locmem->top_line;
					return PARTUPDATE;
				}
				else
				{
					RMVCURS;
					locmem->crs_line = locmem->top_line;
					PUTCURS;
				}
		break;

	case 'q':
	case 'e':
	case KEY_LEFT:
		//if ( digestmode )
		if( digestmode && uinfo.mode != RMAIL ) //chenhao
		{
			if(digestmode == 5 || digestmode == 6 ) {
                        sprintf(genbuf,"boards/%s/SOMEONE.%s.DIR.%d",currboard,someoneID,digestmode-5);
                        unlink(genbuf);
                } else if (digestmode == 7 ) {
                        sprintf(genbuf,"boards/%s/KEY.%s.DIR",currboard,currentuser.userid);
                        unlink(genbuf);
                }
                digestmode = NA;
                setbdir(currdirect, currboard);
		return NEWDIRECT;
		//	return acction_mode();
		}
		else
			return DOQUIT;
	case Ctrl('L'):
		redoscr();
		break;
	case 'M':
		savemode = uinfo.mode;
		m_new();
		modify_user_mode(savemode);
		return FULLUPDATE;
	case 'u':
		savemode = uinfo.mode;
		modify_user_mode(QUERY);
		t_query();
		modify_user_mode(savemode);
		return FULLUPDATE;
	case 'H':
		if(uinfo.mode != READING)
			return FULLUPDATE;
		read_hot_info();//loveni
		if(temphotinfonumber!=0)
		{
			cursor_pos_topten(locmem,temphotinfonumber,10);
			temphotinfonumber=0;
		}
		//show_help("0Announce/bbslist/day");
		return FULLUPDATE;
	case 'O':
		if (!strcmp("guest", currentuser.userid))
			break;
		move(23, 0);
		if (askyn("ÄãÏëÌí¼ÓÍøÓÑµ½ºÃÓÑÃûµ¥Âğ", NA, NA) == NA)
			return PARTUPDATE;
		friend_add();
		return FULLUPDATE;
	case 'k':
	case KEY_UP:
		if (cursor_pos(locmem, locmem->crs_line - 1, screen_len - 1, 0))
		{
			return PARTUPDATE;
		}
		break;
	case 'j':
	case KEY_DOWN:
		if (cursor_pos(locmem, locmem->crs_line + 1, 0, 1))
		{
			if (locmem->crs_line > last_line + ntop)
				locmem->crs_line = locmem->top_line = last_line - ntop;
			if(last_line!=old_last_line)
			{
				old_last_line=last_line;
			}
			return PARTUPDATE;
		}
		break;
	case 'l':		/* ppfoong */
		show_allmsgs();
		return FULLUPDATE;
	case 'L':		//chenhao ½â¾öÔÚÎÄÕÂÁĞ±íÊ±¿´ĞÅµÄÎÊÌâ
		if(uinfo.mode == RMAIL)
			return DONOTHING;
		savemode = uinfo.mode;
		m_read();
		modify_user_mode(savemode);
		return MODECHANGED;
	case 'N':
	case Ctrl('F'):
	case KEY_PGDN:
	case ' ':
		if (last_line - ntop >= locmem->top_line + screen_len)
		{
			locmem->top_line += screen_len - 1;
			locmem->crs_line = locmem->top_line;
			return PARTUPDATE;
		}
		RMVCURS;
		if (locmem->crs_line >= last_line - ntop)
			locmem->crs_line = last_line;
		else
			locmem->crs_line = last_line - ntop;
		if (locmem->top_line + screen_len <= locmem->crs_line)
		{
			locmem->top_line = locmem->crs_line - screen_len + 1;
			if(last_line!=old_last_line)
			{
				last_line=old_last_line;
			}
			return PARTUPDATE;
		}
		if(last_line!=old_last_line)
		{
			old_last_line=last_line;
			return PARTUPDATE;
		}
		PUTCURS;
		break;
	case KEY_HOME:
		locmem->top_line = 1;
		locmem->crs_line = 1;
		return PARTUPDATE;
	case '$':
	case KEY_END:
		if (last_line - ntop >= locmem->top_line + screen_len)
		{
			locmem->top_line = last_line - screen_len + 1;
			if (locmem->top_line <= 0)
				locmem->top_line = 1;
			locmem->crs_line = last_line - ntop;
			return PARTUPDATE;
		}
		RMVCURS;
		if (locmem->crs_line == last_line)
			locmem->crs_line -= ntop;
		else if (locmem->crs_line >= last_line - ntop)
		{
			locmem->crs_line = last_line;
			if (locmem->top_line + screen_len <= locmem->crs_line)
				locmem->top_line = locmem->crs_line - screen_len + 1;
			return PARTUPDATE;
		}
		else
			locmem->crs_line = last_line - ntop;
		if (locmem->crs_line < locmem->top_line)
		{
			locmem->top_line = locmem->crs_line;
			return PARTUPDATE;
		}
		PUTCURS;
		break;
	case 'S':		/* youzi */
		if (!HAS_PERM(PERM_MESSAGE))
			break;
		s_msg();
		return FULLUPDATE;
		break;
	case 'f':		/* youzi */
		if (!HAS_PERM(PERM_BASIC))
			break;
		t_friends();
		return MODECHANGED;
		break;
	case '!':		/* youzi leave */
		return Goodbye();
		break;
	case '\n':
	case '\r':
	case KEY_RIGHT:
		ch = 'r';
		/* lookup command table */
	default:
		for (i = 0; rcmdlist[i].fptr != NULL; i++)
		{
			if (rcmdlist[i].key == ch)
			{
				mode = (*(rcmdlist[i].fptr)) (locmem->crs_line,
				                              &pnt[(locmem->crs_line - locmem->top_line) * ssize],
				                              currdirect);
				break;
			}
		}
	}
	return mode;
}

int auth_search_down(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	//if (search_author(locmem, 1, fileinfo->owner))
	if (search_author(locmem, 1, strtok(fileinfo->owner," ")))//loveni:ĞÅÏä×÷ÕßËÑË÷
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}

int auth_search_up(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	//if (search_author(locmem, -1, fileinfo->owner))
	if (search_author(locmem, -1, strtok(fileinfo->owner," ")))//loveni:ĞÅÏä×÷ÕßËÑË÷
	
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}


int post_search_down(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_post(locmem, 1))
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}

int post_search_up(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_post(locmem, -1))
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}

int sendmsgtoauthor(int ent, struct fileheader* fileinfo, char* direct)
{
	int good_id;
	struct user_info *uin ;
	if(!HAS_PERM(PERM_MESSAGE))
		return DONOTHING;
	clear();
	uin=(struct user_info*)t_search(fileinfo->owner,NA);
	if (uin == NULL
	        ||uin->invisible&&(!(HAS_PERM(PERM_SYSOP | PERM_SEECLOAK)||canseeme(uin))))
	{
		good_id = NA;
	}
	else
		good_id = YEA;
	if (good_id == YEA && canmsg(uin))
		do_sendmsg(uin,NULL,0,uin->pid);
	else
		do_sendmsg(NULL,NULL,0,0);
	return FULLUPDATE;
} //add by yiyo ¸ø×÷Õß·¢ÏûÏ¢

int show_author(int ent, struct fileheader* fileinfo, char* direct)
{
	t_query(fileinfo->owner);
	return FULLUPDATE;
}
/* Efan: ÓÊÏä°æÖ÷¹¦ÄÜ	*/

int Mail_func2(int ent, struct fileheader *fileinfo, char *direct)
{
	int     i, dotype = 0, result = 0;
	int     punish;
	char    buf[80], ch[4], BMch;
	struct keeploc *locmem;
	static char *SR_BMitems[] =
	    {"É¾³ı", "±£Áô"};
	static char *subBMitems[] =
	    {"ÏàÍ¬Ö÷Ìâ", " ÏàÍ¬×÷Õß", "Ïà¹ØÖ÷Ìâ"};

	if (fileinfo->owner[0] == '-')
		return DONOTHING;
	/*
	if (!chk_currBM(currBM)&&!HAS_PERM(PERM_LOOKADMIN)) return DONOTHING;//bluetent
	*/
	saveline(t_lines - 1, 0);
	move(t_lines - 1, 0);
	clrtoeol();
	ch[0] = '\0';
	getdata(t_lines - 1, 0,
	        "Ö´ĞĞ: 1) ÏàÍ¬Ö÷Ìâ  2) ÏàÍ¬×÷Õß 3) Ïà¹ØÖ÷Ìâ 0) È¡Ïû [0]: ",
	        ch, 3, DOECHO, YEA);
	dotype = atoi(ch);
	if (dotype < 1 || dotype > 3)
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	sprintf(buf, "%s ", subBMitems[dotype - 1]);
	for (i = 0; i < 2; i++)
		sprintf(buf, "%s(%d)%s ", buf, i + 1, SR_BMitems[i]);
	strcat(buf, "? [0]: ");
	getdata(t_lines - 1, 0, buf, ch, 3, DOECHO, YEA);
	BMch = atoi(ch);
	// modified at 2002.7.6
	if(BMch<=0||BMch>2)
	{
		saveline(t_lines-1,1);
		return DONOTHING;
	}
	if((digestmode>1&&BMch==3)||(digestmode==1&&BMch==3)
	        ||(digestmode>2&&BMch<3))
	{
		presskeyfor("´ËÖÖ¿´°åÄ£Ê½ÏÂ²»ÄÜÊ¹ÓÃ¸Ã¹¦ÄÜ",t_lines-1);
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	move(t_lines - 1, 0);
	sprintf(buf,"È·¶¨ÒªÖ´ĞĞ%s[%s]Âğ",subBMitems[dotype-1],SR_BMitems[BMch-1]);
	if (askyn(buf, NA, NA) == 0)
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}

	//add by yiyo
	if (dotype == 3)
	{
		strcpy(keyword, "");
		getdata(t_lines - 1, 0, "ÇëÊäÈëÖ÷Ìâ¹Ø¼ü×Ö: ", keyword, 50, DOECHO, YEA);
		if (keyword[0] == '\0')
		{
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
	}
	else if (dotype == 1)
	{
		strcpy(keyword, fileinfo->title);
	}
	else
	{
		strcpy(keyword, fileinfo->owner);
	}
	move(t_lines - 1, 0);
	sprintf(buf, "ÊÇ·ñ´Ó%sµÚÒ»Æª¿ªÊ¼%s (Y)µÚÒ»Æª (N)Ä¿Ç°ÕâÒ»Æª",
	        (dotype == 2) ? "¸Ã×÷Õß" : "´ËÖ÷Ìâ", SR_BMitems[BMch - 1]);
	if(askyn(buf, YEA, NA) == YEA)
	{
		result = locate_the_post(fileinfo, keyword,5,dotype-1,0);
	}
	else if(dotype == 3)
	{
		result = locate_the_post(fileinfo, keyword,1,2,0);
	}
	else
	{
		memcpy(&SR_fptr, fileinfo, sizeof(SR_fptr));
	}

	if( result == -1 )
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	punish =0;
	while(1)
	{
		locmem = getkeep(currdirect, 1, 1);
		switch(BMch)
		{
		case 1:
			SR_BMDELFLAG = YEA;
			result = mail_del2(locmem->crs_line, &SR_fptr, currdirect,punish);
			SR_BMDELFLAG = NA;
			if(result == -1)
				return DIRCHANGED;
			if (result != DONOTHING)
			{
				last_line--;
				locmem->crs_line--;
			}
			break;
		case 2:
			mail_mark(locmem->crs_line, &SR_fptr, currdirect);
			break;
		}
		if(locmem->crs_line <= 0)
		{
			result = locate_the_post(fileinfo, keyword,5,dotype-1,0);
		}
		else
		{
			result = locate_the_post(fileinfo, keyword,1,dotype-1,0);
		}
		if(result == -1)
			break;
	}
	return DIRCHANGED;
}

// modified at 2002.7.6 for ºÏ¼¯
int SR_BMfunc(int ent, struct fileheader *fileinfo, char *direct)
{
	int     i, dotype = 0, result = 0,full=0;
	int     punish;
	char    buf[80], ch[4], BMch, annpath[512];
	struct keeploc *locmem;
	static char *SR_BMitems[] =
	    {"É¾³ı", "±£Áô", "ÎÄÕª", "¾«»ªÇø", "Ôİ´æµµ","²»¿ÉRE","ºÏ¼¯"};
	static char *subBMitems[] =
	    {"ÏàÍ¬Ö÷Ìâ", " ÏàÍ¬×÷Õß", "Ïà¹ØÖ÷Ìâ", "É¾³ıÍ¼Æ¬"};

	/* Efan: Ó¦¸Ã¸Ä³ÉÖ»¶Áºó½ö½ö²»ÄÜ×öºÏ¼¯±È½ÏÇ¡µ±	*/
	/*
	       if ( !HAS_PERM(PERM_OBOARDS)&&dashf("NOPOST")&&!HAS_PERM(PERM_SYSOP)&&!(HAS_PERM(PERM_CHATCLOAK)&& ( !strcmp(currboard, "PreparatoryWork")|| !strcmp (currboard, "Headquarters") )))
		 {
			prints("\n\n ¶Ô²»Æğ£¬Ä¿Ç°±¾Õ¾´¦ÓÚÖ»¶ÁÎ¬»¤×´Ì¬¡£\n");				                                                   return -1;
	        }//Ö»¶ÁÎ¬»¤ºó²»ÄÜ×÷ºÏ¼¯.
	*/
	if (!strcmp(currboard, "deleted") || !strcmp(currboard, "junk") || !strncmp(currboard, "syssecurity", 11))
		return DONOTHING;
	if (fileinfo->owner[0] == '-')
		return DONOTHING;
	if (!chk_currBM(currBM)&&!HAS_PERM(PERM_LOOKADMIN))
		return DONOTHING;//bluetent
	if(digestmode!=NA)
		return DONOTHING;//added by tdhlshx for a bug
	saveline(t_lines - 1, 0);
	move(t_lines - 1, 0);
	clrtoeol();
	ch[0] = '\0';
	getdata(t_lines - 1, 0,
	        "Ö´ĞĞ: 1) ÏàÍ¬Ö÷Ìâ  2) ÏàÍ¬×÷Õß 3) Ïà¹ØÖ÷Ìâ 4)·¢ÎÄÄ£°å 0) È¡Ïû [0]: ",
	        ch, 3, DOECHO, YEA);
	dotype = atoi(ch);
	if (dotype == 5)
	{
		if (!HAS_PERM(PERM_BLEVELS))
			return DONOTHING;
		kaka_pic ();
		return PARTUPDATE;
	}
    if(dotype==4){
         editpattern();
         return  FULLUPDATE;
	}
	if (dotype < 1 || dotype > 3)
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	sprintf(buf, "%s ", subBMitems[dotype - 1]);
	for (i = 0; i < 7; i++)
		sprintf(buf, "%s(%d)%s ", buf, i + 1, SR_BMitems[i]);
	strcat(buf, "? [0]: ");
	getdata(t_lines - 1, 0, buf, ch, 3, DOECHO, YEA);
	BMch = atoi(ch);
	// modified at 2002.7.6
	if(BMch<=0||BMch>7)
	{
		saveline(t_lines-1,1);
		return DONOTHING;
	}
	if((digestmode>1&&BMch==3)||(digestmode==1&&BMch==3)
	        ||(digestmode>2&&BMch<3))
	{
		presskeyfor("´ËÖÖ¿´°åÄ£Ê½ÏÂ²»ÄÜÊ¹ÓÃ¸Ã¹¦ÄÜ",t_lines-1);
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	if(dotype == 2 && BMch == 7 )
	{
		if(check_user_exist(fileinfo->owner))
			return FULLUPDATE;

	}
	/*if(dotype==2&&BMch==7)
	{
		presskeyfor("²»ÄÜ¶ÔÍ¬×÷ÕßÖ´ĞĞ[ºÏ¼¯]²Ù×÷",t_lines-1);
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}*/
	/* Efan: Ö»¶Áºó½ö½ö²»ÔÊĞí×öºÏ¼¯	*/
        /*
	if ( (BMch == 7) && !HAS_PERM(PERM_OBOARDS)&&dashf("NOPOST")&&!HAS_PERM(PERM_SYSOP)&&!(HAS_PERM(PERM_CHATCLOAK)&& ( !strcmp(currboard, "PreparatoryWork")|| !strcmp (currboard, "Headquarters") || ! strcmp (currboard, "notepad"))))
	{
		move (3, 0);
		clrtobot ();
		move (5, 0);
		prints("          ¶Ô²»Æğ£¬Ä¿Ç°±¾Õ¾´¦ÓÚÖ»¶ÁÎ¬»¤×´Ì¬¡£\n");
		pressreturn ();
		return FULLUPDATE;
	}//Ö»¶ÁÎ¬»¤ºó²»ÄÜ×÷ºÏ¼¯.
       *///¿ÉÒÔ×ö°É£¿ºÇºÇ
	move(t_lines - 1, 0);
	sprintf(buf,"È·¶¨ÒªÖ´ĞĞ%s[%s]Âğ",subBMitems[dotype-1],SR_BMitems[BMch-1]);
	if (askyn(buf, NA, NA) == 0)
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	if(BMch==7)
	{
		move(t_lines - 1, 0);
		sprintf(buf, "ÊÇ·ñÊÕÂ¼ÒıÑÔ (Y)ÊÇ (N)·ñ");
		if(askyn(buf, YEA, NA) == YEA)
			AddCombineMode=1;
		else
			AddCombineMode=0;

	}

	// add 2002.7.6
	if(BMch == 4)
	{
		FILE *fn;
		sethomefile(annpath, currentuser.userid,".announcepath");
		if((fn = fopen(annpath, "r")) == NULL )
		{
			presskeyfor("¶Ô²»Æğ, ÄãÃ»ÓĞÉè¶¨Ë¿Â·. ÇëÏÈÓÃ f Éè¶¨Ë¿Â·.",t_lines-1);
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
		fscanf(fn,"%s",annpath);
		fclose(fn);
		if (!dashd(annpath))
		{
			presskeyfor("ÄãÉè¶¨µÄË¿Â·ÒÑ¶ªÊ§, ÇëÖØĞÂÓÃ f Éè¶¨.",t_lines-1);
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
	}
	else if( BMch == 5 )
	{
		if(askyn("[ÊÕÈëÔİ´æµµÄ£Ê½: Ò»°ã/¾«¼ò] ²ÉÓÃ¾«¼ò·½Ê½Âğ",YEA,YEA)==NA)
			full = 1;
	}
	//add by yiyo
	if (dotype == 3)
	{
		strcpy(keyword, "");
		getdata(t_lines - 1, 0, "ÇëÊäÈëÖ÷Ìâ¹Ø¼ü×Ö: ", keyword, 50, DOECHO, YEA);
		if (keyword[0] == '\0')
		{
			saveline(t_lines - 1, 1);
			return DONOTHING;
		}
	}
	else if (dotype == 1)
	{
		strcpy(keyword, fileinfo->title);
	}
	else
	{
		strcpy(keyword, fileinfo->owner);
	}
	if( (dotype == 1 || dotype == 2 || dotype == 3) && BMch == 7)
	{
		//add to forbidden tabulating articles when the sys is in the situation of ONLY READ.  2003.4.26

		/* Not necessarily	*/
		/*
		              if(!HAS_PERM(PERM_OBOARDS)&&dashf("NOPOST")&&!(HAS_PERM(PERM_CHATCLOAK)&&strcmp(currboard, "PreparatoryWork")&&strcmp (currboard, "Headquarters")))
		              {
		                      move(3,0);
		                      clrtobot();
		                      move(5,0);
		                      prints("         ¶Ô²»Æğ£¬Ä¿Ç°±¾Õ¾´¦ÓÚÖ»¶ÁÎ¬»¤×´Ì¬¡£\n");
		                      pressreturn();
		                     clear();
		                     
		                       move(5,6);
		                      prints("¶Ô²»Æğ£¬Ä¿Ç°±¾Õ¾´¦ÓÚÖ»¶ÁÎ¬»¤×´Ì¬¡£\n");
		                      pressreturn();
		                      return DONOTHING;
		              }               //add over
		 
		*/

		sprintf(buf, "tmp/%s.combine", currentuser.userid);
		if(dashf(buf))
			unlink(buf);
	}
	move(t_lines - 1, 0);
	sprintf(buf, "ÊÇ·ñ´Ó%sµÚÒ»Æª¿ªÊ¼%s (Y)µÚÒ»Æª (N)Ä¿Ç°ÕâÒ»Æª",
	        (dotype == 2) ? "¸Ã×÷Õß" : "´ËÖ÷Ìâ", SR_BMitems[BMch - 1]);
	if(askyn(buf, YEA, NA) == YEA)
	{
		result = locate_the_post(fileinfo, keyword,5,dotype-1,0);
	}
	else if(dotype == 3)
	{
		result = locate_the_post(fileinfo, keyword,1,2,0);
	}
	else
	{
		memcpy(&SR_fptr, fileinfo, sizeof(SR_fptr));
	}

	if( result == -1 )
	{
		saveline(t_lines - 1, 1);
		return DONOTHING;
	}
	/* add by yiyo ´ÓÑÏÉ¾ÎÄ */
	if(BMch == 1)
	{
		/*      move(t_lines - 1, 0);
		      if(askyn("ÑÏ³ÍÃ´[ÎÄÕÂÊı¼õ2]",NA,NA)==NA) punish=0;
			  else punish=1;*/
		ch[0] = '\0';
		getdata(t_lines - 1, 0,
		        "Ñ¡ÔñÉ¾³ıÄ£Ê½:  (0) ÎÄÕÂÊı¼õ1 (1) ÎÄÕÂÊı¼õ2 (2) ²»¼õÎÄÕÂÊı [0]: ",
		        ch, 3, DOECHO, YEA);
		/*   getdata(t_lines - 1, 0,
		      "Ñ¡ÔñÉ¾³ıÄ£Ê½:  (0) ÎÄÕÂÊı¼õ1 (1) ÎÄÕÂÊı¼õ2[0]: ", 
		      ch, 3, DOECHO, YEA);*/
		if(atoi(ch)==0)
			punish=0;
		else if(atoi(ch)==1)
			punish=1;
		else
			punish=2;
		/* Efan: ÅĞ¶ÏÊÇ·ñÉ¾³ıÎÄÕª --2003.4.26	*/
		getdata (t_lines -1, 0, "ÊÇ·ñÉ¾³ıÎÄÕª (Yes/No)£¿[N]",
		         ch, 3, DOECHO, YEA);
		if (ch [0] == 'n' || ch [0] == 'N' || ch [0] == '\0')
			punish |= 4;
		else if ( ch [0] != 'y' && ch [0] != 'Y' )
		{
			saveline (t_lines -1, 1);
			return DONOTHING;
		} //mod by livebird 2004 12 21
		/*if (ch [0] == 'y' || ch [0] == 'Y')
			punish |= 4;
		else if (ch [0] != 'n' && ch [0] != 'N' && ch [0] != 0)
		{
			saveline (t_lines -1, 1);
			return DONOTHING;
		}*/
	}
	/* add end */

	/* Efan: ÔÚsyssecurity°æ×÷¼ÇÂ¼	*/
	if (uinfo.mode == READING && BMch >=1 && BMch <=3)
	{
		char tmpstr [100];
		int tmpint = punish & (~4);
		sprintf (tmpstr, "ËùÔÚ°åÃæ£º%s£¬Ö´ĞĞ£º%s %s", currboard, subBMitems [dotype -1], SR_BMitems [BMch -1]);
		securityreport3 (tmpstr, (tmpint == 0)? 1: ((tmpint == 1)? 2: 0));
	}
	while(1)
	{
		locmem = getkeep(currdirect, 1, 1);
		switch(BMch)
		{
		case 1:
			SR_BMDELFLAG = YEA;
			result = del_post(locmem->crs_line, &SR_fptr, currdirect,punish);
			SR_BMDELFLAG = NA;
			if(result == -1)
				return DIRCHANGED;
			if (result != DONOTHING)
			{
				last_line--;
				locmem->crs_line--;
			}
			break;
		case 2:
			mark_post(locmem->crs_line, &SR_fptr, currdirect);
			break;
		case 3:
			digest_post(locmem->crs_line, &SR_fptr, currdirect);
			break;
		case 4:
			//a_Import("0Announce",currboard,locmem->crs_line,
			//   &SR_fptr,currdirect, YEA);
			a_Import("0Announce",currboard, &SR_fptr,annpath, YEA);//by yiyo
			break;
		case 5:
			//a_Save("0Announce", currboard, &SR_fptr, YEA);
			a_Save("0Announce", currboard, &SR_fptr, YEA, full);
			break;
		case 6:
			underline_post(locmem->crs_line,&SR_fptr,currdirect);
			break;
			/*    add for ºÏ¼¯  */
		case 7:
			Add_Combine(currboard,&SR_fptr,AddCombineMode);
			break;
		}	/* Efan: End of switdh	*/
		if(locmem->crs_line <= 0)
		{
			result = locate_the_post(fileinfo, keyword,5,dotype-1,0);
		}
		else
		{
			result = locate_the_post(fileinfo, keyword,1,dotype-1,0);
		}
		if(result == -1)
			break;
	}	/* Efan: End of while	*/
	// add for ºÏ¼¯
	if( (dotype == 1 || dotype == 2 || dotype == 3) && BMch == 7)
	{
		/*move before while(1)2003.4.27*/
		//add to forbidden tabulating articles when the sys is in the situation of ONLY READ.  2003.4.26
		/*
			      if(!HAS_PERM(PERM_OBOARDS)&&dashf("NOPOST")&&!(HAS_PERM(PERM_CHATCLOAK)&&strcmp(currboard, "PreparatoryWork")&&strcmp (currboard, "Headquarters")))
			      {
				      move(3,0);
				      clrtobot();
				      move(5,0);
				      prints("         ¶Ô²»Æğ£¬Ä¿Ç°±¾Õ¾´¦ÓÚÖ»¶ÁÎ¬»¤×´Ì¬¡£\n");
				      pressreturn();
		                     clear();
				      move(5,6);
				      prints("¶Ô²»Æğ£¬Ä¿Ç°±¾Õ¾´¦ÓÚÖ»¶ÁÎ¬»¤×´Ì¬¡£\n");
				      pressreturn();
				      return DONOTHING;
			      }		      //add over     
		*/

		if( !strncmp(keyword, "Re: ", 4)||!strncmp(keyword,"RE: ",4) )
			sprintf(buf, "¡¾ºÏ¼¯¡¿%s", keyword + 4);
		else
			sprintf(buf, "¡¾ºÏ¼¯¡¿%s", keyword);
		strcpy(keyword, buf);
		sprintf(buf, "tmp/%s.combine", currentuser.userid);
		Postfile( buf,currboard,keyword,2);
		/* add by bluetent */
		if (!junkboard())
		{
			set_safe_record();
			currentuser.numposts++;
			substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
		}
		/* add end */
		unlink(buf);
	}
	return DIRCHANGED;
}

int SR_first_new(int ent, struct fileheader* fileinfo, char* direct)
{
	if(locate_the_post(fileinfo, fileinfo->title,5,0,1)!=-1)
	{
		sread(1, 0, &SR_fptr);
		return FULLUPDATE;
	}
	return PARTUPDATE;
}

int SR_last(int ent, struct fileheader* fileinfo, char* direct)
{
	locate_the_post(fileinfo, fileinfo->title,3,0,0);
	return PARTUPDATE;
}

int SR_first(int ent, struct fileheader* fileinfo, char* direct)
{
	locate_the_post(fileinfo, fileinfo->title,5,0,0);
	return PARTUPDATE;
}

int SR_read(int ent, struct fileheader* fileinfo, char* direct)
{
	sread(1, 0, fileinfo);
	return FULLUPDATE;
}

int SR_author(int ent, struct fileheader* fileinfo, char* direct)
{
	sread(1, 1, fileinfo);
	return FULLUPDATE;
}

int search_author(struct keeploc* locmem, int offset, char* powner)
{
	static char author[IDLEN + 2];
	char    ans[IDLEN + 2], pmt[STRLEN];
	char    currauth[STRLEN];
	strcpy(currauth, powner);
	strtok(currauth,": (");
	sprintf(pmt, "%sµÄÎÄÕÂËÑÑ°×÷Õß [%s]: ", offset > 0 ? "ÍùááÀ´" : "ÍùÏÈÇ°", currauth);
	move(t_lines - 1, 0);
	clrtoeol();
	getdata(t_lines - 1, 0, pmt, ans, IDLEN+1, DOECHO, YEA);
	if (ans[0] != '\0')
		strcpy(author, ans);
	else
		strcpy(author, currauth);
	return search_articles(locmem, author, offset, 1,0);
}

int auth_post_down(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_author(locmem, 1, fileinfo->owner))
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}

int auth_post_up(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_author(locmem, -1, fileinfo->owner))
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}

int t_search_down(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_title(locmem, 1))
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}

int t_search_up(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_title(locmem, -1))
		return PARTUPDATE;
	else
		update_endline();
	return DONOTHING;
}
int thread_up(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_thread(locmem, -1, fileinfo->title))
	{
		update_endline();
		return PARTUPDATE;
	}
	update_endline();
	return DONOTHING;
}

int thread_down(int ent, struct fileheader* fileinfo, char* direct)
{
	struct keeploc *locmem;
	locmem = getkeep(direct, 1, 1);
	if (search_thread(locmem, 1, fileinfo->title))
	{
		update_endline();
		return PARTUPDATE;
	}
	update_endline();
	return DONOTHING;
}

int search_post(struct keeploc* locmem, int offset)
{
	static char query[STRLEN];
	char    ans[STRLEN], pmt[STRLEN];
	strcpy(ans, query);
	sprintf(pmt, "ËÑÑ°%sµÄÎÄÕÂ [%s]: ", offset > 0 ? "ÍùááÀ´" : "ÍùÏÈÇ°", ans);
	move(t_lines - 1, 0);
	clrtoeol();
	getdata(t_lines - 1, 0, pmt, ans, 50, DOECHO, YEA);
	if (ans[0] != '\0')
		strcpy(query, ans);

	return search_articles(locmem, query, offset, -1, 0);
}


int search_title(struct keeploc* locmem, int offset)
{
	static char title[STRLEN];
	char    ans[STRLEN], pmt[STRLEN];
	strcpy(ans, title);
	sprintf(pmt, "%sËÑÑ°±êÌâ [%.16s]: ", offset > 0 ? "Íùáá" : "ÍùÇ°", ans);
	move(t_lines - 1, 0);
	clrtoeol();
	getdata(t_lines - 1, 0, pmt, ans, 46, DOECHO, YEA);
	if (*ans != '\0')
		strcpy(title, ans);
	return search_articles(locmem, title, offset, 2, 0);
}

int search_thread(struct keeploc* locmem, int offset, char* title)
{

	if (title[0] == 'R' && (title[1] == 'e' || title[1] == 'E') && title[2] == ':')
		title += 4;
	setqtitle(title);
	return search_articles(locmem, title, offset, 0, 0);
}

int sread(int readfirst, int auser, struct fileheader *ptitle)
{
	struct keeploc *locmem;
	int     rem_top, rem_crs, ch;
	int     isend = 0, isstart = 0, isnext = 1;
	char    tempbuf[STRLEN], title[STRLEN];
	if(check_user_exist(ptitle->owner))
		return 1;
	if (readfirst)
	{
		isstart = 1;
	}
	else
	{
		isstart = 0;
		move(t_lines - 1, 0);
		clrtoeol();
		prints("[1;44;31m[%8s] [33mÏÂÒ»·â <Space>,<Enter>,¡ı©¦ÉÏÒ»·â ¡ü,U                              [m", auser ? "ÏàÍ¬×÷Õß" : "Ö÷ÌâÔÄ¶Á");
		switch (egetch())
		{
		case ' ':
		case '\n':
		case KEY_DOWN:
		case KEY_RIGHT:
			isnext = 1;
			break;
		case KEY_UP:
		case 'u':
		case 'U':
			isnext = -1;
			break;
		default:
			isend = 1;
			break;
		}
	}
	locmem = getkeep(currdirect, 1, 1);
	rem_top = locmem->top_line;
	rem_crs = locmem->crs_line;
	if (auser == 0)
	{
		strcpy(title, ptitle->title);
		setqtitle(title);
	}
	else
	{
		strcpy(title, ptitle->owner);
		setqtitle(ptitle->title);
	}
	if (!strncmp(title, "Re: ", 4) | !strncmp(title, "RE: ", 4))
	{
		strcpy(title, title + 4);
	}
	memcpy(&SR_fptr, ptitle, sizeof(SR_fptr));
	while (!isend)
	{
		if (!isstart)
		{
			if(search_articles(locmem, title, isnext, auser, 0)==-1)
				break;
		}
		if (uinfo.mode != RMAIL)
			setbfile(tempbuf, currboard, SR_fptr.filename);
		else
			sprintf(tempbuf, "mail/%c/%s/%s", toupper(currentuser.userid[0]),
			        currentuser.userid, SR_fptr.filename);
        if(!dashf(tempbuf)) {
        	snprintf(tempbuf,256,"boards/%s/%s", ((struct fileheader_v *)(&SR_fptr))->board,
				SR_fptr.filename);
		}
		setquotefile(tempbuf);
		ch = ansimore(tempbuf, NA);
		brc_addlist(SR_fptr.filename);
		isstart = 0;
		if( ch == KEY_UP || ch == 'u' || ch == 'U')
		{
			readfirst = (ch == KEY_UP);
			isnext = -1;
			continue;
		}
		move(t_lines - 1, 0);
		clrtoeol();
		prints("\033[1;44;31m[%8s] \033[33m»ØĞÅ R ©¦ ½áÊø Q,¡û ©¦ÏÂÒ»·â ¡ı,Enter©¦ÉÏÒ»·â ¡ü,U ©¦ ^R »Ø¸ø×÷Õß   \033[m", auser ? "ÏàÍ¬×÷Õß" : "Ö÷ÌâÔÄ¶Á");
		switch (ch = egetch())
		{
		case KEY_LEFT:
		case 'N':
		case 'Q':
		case 'n':
		case 'q':
			isend = 1;
			break;
		case 'Y':
		case 'R':
		case 'y':
		case 'r':
			{
				struct shortfile *bp;
				extern struct shortfile *getbcache();

				bp = getbcache(currboard);
				noreply=ptitle->accessed[0]&FILE_NOREPLY||bp->flag&NOREPLY_FLAG;
				if(!noreply || HAS_PERM(PERM_SYSOP)||chk_currBM(currBM))
				{
					local_article=!(ptitle->filename[STRLEN-1]=='S');
					do_reply(ptitle->title,ptitle->owner);
				}
				else
				{
					clear();
					move(5, 6);
					prints("¶Ô²»Æğ, ¸ÃÎÄÕÂÓĞ²»¿É RE ÊôĞÔ, Äã²»ÄÜ»Ø¸´(RE) ÕâÆªÎÄÕÂ.");
					pressreturn();
				}
				break;
			}
		case ' ':
		case '\n':
		case KEY_DOWN:
			readfirst = (ch == KEY_DOWN);
			isnext = 1;
			break;
		case Ctrl('A'):
			clear();
			show_author(0, &SR_fptr, currdirect);
			isnext = 1;
			break;
		case Ctrl('H'):
			deny_from_article(0,&SR_fptr,currdirect);
		case 'Z': //add by yiyo ¸ø×÷Õß·¢ÏûÏ¢
			if (!HAS_PERM(PERM_PAGE))
				break;
			sendmsgtoauthor(0, &SR_fptr, currdirect);
			break;
		case KEY_UP:
		case 'u':
		case 'U':
			readfirst = (ch == KEY_UP);
			isnext = -1;
			break;
		case Ctrl('R'):
						post_reply(0, &SR_fptr, (char *) NULL);
			break;
		case 'g':
			digest_post(0, &SR_fptr, currdirect);
			break;
		default:
			break;
		}
	}
	if (readfirst == 0)
{
		RMVCURS;
		locmem->top_line = rem_top;
		locmem->crs_line = rem_crs;
		PUTCURS;
	}
	/* add by bluetent 2003.4.19 */
	if(curr_ent)
	{
		if(last_line!=old_last_line)
		{
			old_last_line=last_line;
		}
		cursor_pos(locmem, curr_ent, 10, 0);
		curr_ent=0;
	}
	/* add end */
	return 1;
}

char *strstr2(char *s, char *s2)
{
	unsigned char *p;
	int len=strlen(s2);
	for(p=s; p[0]; p++)
	{
		if(!strncasecmp(p, s2, len))
			return p;
		if(p[0]>127 && p[1]>31)
			p++;
	}
	return 0;
}        //2002.9.14 add by yiyo ½â¾ö±êÌâËÑË÷ºº×Ö½Ø¶ÏµÄÎÊÌâ

int searchpattern(char* filename, char* query)
{
	FILE   *fp;
	char    buf[256];
	if ((fp = fopen(filename, "r")) == NULL)
		return 0;
	while (fgets(buf, 256, fp) != NULL)
	{
		if (strstr(buf, query))
		{
			fclose(fp);
			return YEA;
		}
	}
	fclose(fp);
	return NA;
}

int search_articles(struct keeploc* locmem, char* query, int offset, int aflag, int newflag)
{
	char   *ptr;
	int     ent, fd;
	int     complete_search, oldent, lastent = 0;
	int     ssize = sizeof(struct fileheader);

	if (*query == '\0')
		return 0;
	if (aflag >= 2)
	{
		complete_search = 0;
		aflag -= 2;
	}
	else if (aflag==1)
	{
		complete_search=0;
	}
	else
	{
		complete_search = 1;
	}
	if(aflag != -1 && offset > 2)
	{
		ent = 0;
		oldent = 0;
		offset -= 4;
	}
	else
	{
		ent = locmem->crs_line;
		oldent = locmem->crs_line;
	}
	move(t_lines - 1, 0);
	clrtoeol();
	prints("[1;44;33mËÑÑ°ÖĞ£¬ÇëÉÔºò....                                                             [m");
	//   refresh();
	if ((fd = open(currdirect, O_RDONLY, 0)) == -1)
		return -1;
	if(aflag !=-1 && offset < 0)
		ent = 0;
	if(ent)
	{
		if(aflag == -1 && offset < 0)
			ent -= 2;
		if(ent<0 || lseek(fd,(off_t)(ent*ssize),SEEK_SET)==-1)
		{
			close(fd);
			return -1;
		}
	}
	if(aflag != - 1 && offset > 0)
		ent = oldent;
	if(aflag == -1 && offset < 0)
		ent += 2;
	while (read(fd,&SR_fptr,ssize)==ssize)
	{
		if(aflag == -1 && offset < 0)
			ent --;
		else
			ent ++;
		if(aflag != -1 && offset < 0 && oldent > 0  && ent >= oldent)
			break;
		if(newflag && !brc_unread(SR_fptr.filename))
			continue;
		if (aflag == -1)
		{
			char    p_name[256];
			if (uinfo.mode != RMAIL)
				setbfile(p_name, currboard, SR_fptr.filename);
			else
				sprintf(p_name, "mail/%c/%s/%s",toupper(currentuser.userid[0]),
				        currentuser.userid, SR_fptr.filename);
			if (searchpattern(p_name, query))
			{
				lastent = ent;
				break;
			}
			else if(offset > 0)
				continue;
			else
			{
				if(lseek(fd,(off_t)(-2*ssize),SEEK_CUR)==-1)
				{
					close(fd);
					return -1;
				}
				continue;
			}
		}
		//ptr = aflag ?SR_fptr.owner : SR_fptr.title;
		ptr = aflag ? strtok(SR_fptr.owner ," "): SR_fptr.title;
		if (complete_search == 1)
		{
			if((*ptr=='R'||*ptr=='r')&&(*(ptr+1)=='E'||*(ptr+1)=='e')
			        && (*(ptr + 2) == ':') && (*(ptr + 3) == ' '))
			{
				ptr = ptr + 4;
			}
			if (!strncmp(ptr, query,40))
			{
				lastent = ent;
				if(offset > 0)
					break;
			}
		}
		else
		{
			char    upper_ptr[STRLEN], upper_query[STRLEN];
			strtoupper(upper_ptr, ptr);
			strtoupper(upper_query, query);
			if (strstr2(upper_ptr, upper_query) != NULL)
			{
				if(aflag)
				{
					if(strcmp(upper_ptr,upper_query))
						continue;
				}
				lastent = ent;
				if(offset > 0)
					break;
			}
		}
	}
	move(t_lines - 1, 0);
	clrtoeol();
	close(fd);
	if(lastent == 0)
		return -1;
	get_record(currdirect, &SR_fptr, sizeof(SR_fptr), lastent);
	last_line = get_num_records(currdirect, ssize);
	/*
		setbfile (genbuf, currboard, ".top");
		ntop = get_num_records (genbuf, sizeof (struct fileheader));
		if (!strcmp (currentuser.userid, "efanlee"))
			last_line += ntop;
	*/
	return (cursor_pos(locmem, lastent, 10, 0));
}


/*
offset; //-1 µ±Ç°ÏòÉÏ  1 µ±Ç°ÏòÏÂ  3 ×îºóÒ»Æª 5 µÚÒ»Æª
aflag;  // 1 owner  0 Í¬Ö÷Ìâ   2 Ïà¹ØÖ÷Ìâ
newflag; // 1 ±ØĞëÎªĞÂÎÄÕÂ   0 ĞÂ¾É¾ù¿É
 */
int locate_the_post(struct fileheader* fileinfo, char* query, int offset, int aflag, int newflag)
{
	struct keeploc *locmem;
	locmem = getkeep(currdirect, 1, 1);
	if (query[0]=='R'&&(query[1]=='e'||query[1]=='E')&&query[2]==':')
		query += 4;
	setqtitle(query);
	return search_articles(locmem, query, offset, aflag, newflag);
}

/* calc cursor pos and show cursor correctly -cuteyu */
int cursor_pos(struct keeploc *locmem,int val,int from_top,int partornot)
{
	if (val > last_line)
	{
		val = DEFINE(DEF_CIRCLE) ? 1 : last_line;
	}
	if (val <= 0)
	{
		val = DEFINE(DEF_CIRCLE) ? last_line : 1;
	}
	//if(val>=locmem->top_line&&val<locmem->top_line+screen_len-1){
	if(val>=locmem->top_line&&val<locmem->top_line+screen_len&&(partornot==0||old_last_line==last_line))
	{
		RMVCURS;
		locmem->crs_line = val;
		PUTCURS;
		return 0;
	}
	if(partornot!=0&&val>=locmem->top_line&&val<locmem->top_line+screen_len&&val<=old_last_line)
	{
		RMVCURS;
		locmem->crs_line = val;
		PUTCURS;
		return 0;
	}
	if(partornot!=0&&val>=locmem->top_line&&val<locmem->top_line+screen_len&&val>old_last_line)
	{
		old_last_line=last_line;
		locmem->crs_line = val;
		return 1;
	}
	old_last_line=last_line;
	locmem->top_line = val - from_top;
	if (locmem->top_line <= 0)
		locmem->top_line = 1;
	locmem->crs_line = val;
	return 1;
}

int cursor_pos_topten(struct keeploc* locmem, int val, int from_top)
{
	//if(val>=locmem->top_line&&val<locmem->top_line+screen_len-1){
	if(val>=locmem->top_line&&val<locmem->top_line+screen_len)
	{
		RMVCURS;
		locmem->crs_line = val;
		PUTCURS;
		return 0;
	}
	locmem->top_line = val - from_top;
	if (locmem->top_line <= 0)
		locmem->top_line = 1;
	locmem->crs_line = val;
	return 1;
}



int kaka_pic (void)
{
	char filename [256];
	struct dir
	{
		char board[20];
		char userid[14];
		char showname[40];
		char exp[80];
		char type[30];
		int filename;
		int date;
		int level;
		int size;
		int live;
		int click;
		int active;
		int accessed;
	};
	struct dir d;
	FILE *fp;

	getdata (t_lines -1, 0, "ÇëÊäÈëÎÄ¼şÃû£º", filename, 50, DOECHO, YEA);

	/* efan: ·ÀÖ¹¿çÄ¿Â¼É¾³ıÎÄ¼ş	*/
	if (strstr (filename, ".."))
	{
		move (t_lines -1, 0);
		clrtoeol ();
		prints ("¶Ô²»Æğ£¬Ö»ÄÜÉ¾³ı\33[31m±¾°å\33[mµÄÉÏ´«ÎÄ¼ş!");
		egetch ();
		return DONOTHING;
	}
	sprintf (genbuf, "È·¶¨ÒªÉ¾³ıÎÄ¼ş [\33[31m%s\33[m] Âğ£¿", filename);
	move (t_lines -1, 0);
	clrtoeol ();
	if (askyn (genbuf, NA, NA) == 0)
	{
		move (t_lines -1, 0);
		clrtoeol ();
		prints ("È¡ÏûÉ¾³ıÎÄ¼ş");
		egetch ();
		return PARTUPDATE;
	}

	/* efan: É¾³ıuploadÁĞ±íÀïµÄ¼ÇÂ¼	*/
	fp = fopen ("/home/www/upload/.DIR", "r");
	if (fp)
	{
		int nfh;
		int num = 0;
		while ((nfh = fread (&d, sizeof (struct dir), 1, fp)) == 1)
		{
			num++;
			if (!strcmp (d.showname, filename))
				break;
		}
		fclose (fp);
		if (nfh == 1)
			delete_record ("/home/www/upload/.DIR", sizeof (struct dir), num);
		sprintf (genbuf, "/home/www/upload/%d", d.filename);
		remove
			(genbuf);
	}

	/* efan: É¾³ı/home/www/fileÀïÃæµÄ°åÃæÉÏÔØ¸½¼ş	*/
	sprintf (genbuf, "/home/www/file/%s/%s", currboard, filename);
	move (t_lines -1, 0);
	clrtoeol ();
	if (remove
	        (genbuf) == 0)
	{
		sprintf (genbuf, "É¾³ı¸½¼ş [%s/%s]", currboard, filename);
		securityreport (genbuf);
		move (t_lines -1, 0);
		prints ("É¾³ı³É¹¦!");
		egetch ();
	}
	else
	{
		move (t_lines -1, 0);
		prints ("É¾³ıÊ§°Ü! ÓĞÎÊÌâÇëÓë¹ÜÀíÔ±ÁªÏµ");
		egetch ();
	}
	return PARTUPDATE;
}
int deny_from_article(int ent,struct fileheader* fileinfo, char* direct)//add by tdhlshx 2003.12.7°åÃæ°´Ctrl+H·â½ûID.
{
	char msgbuf[512];
	int seek;
	char buf[STRLEN];
	char repbuf[STRLEN];
	char filename[STRLEN];
	if(!chk_currBM(currBM) && (strcmp (currBM, "SYSOPs") || ! HAS_PERM (PERM_ACHATROOM)))
	{
		return DONOTHING;
	}
	if(digestmode!=NA)
	{
		move(3,5);
		clear();
		prints("Çëµ½Ò»°ãÄ£Ê½ÏÂÖ´ĞĞ·â½û¹¦ÄÜ. ^_^ ");
		pressanykey();
		return DONOTHING;
	}
	if(fileinfo->owner[0]!='\0')
	{
		sprintf(filename,"/home/bbs/boards/%s/deny_users",currboard);
		seek = seek_in_file(filename,fileinfo->owner);
	}
	else return DONOTHING;
	if(!seek)
	{
		if (getuser(fileinfo->owner))
		{
                       //  move(2,0);
                        if(askyn("È·¶¨Òª·â½ûÂğ?",NA,YEA)==YEA)
			{
				if (addtodeny(fileinfo->owner,msgbuf,0,1) == 1)
				{
					sprintf(repbuf, "%s±»È¡ÏûÔÚ%s°åµÄ·¢ÎÄÈ¨ÏŞ",
							fileinfo->owner,currboard);
					securityreport4(repbuf);
					if(msgbuf[0]!='\0')
						autoreport(repbuf,msgbuf,YEA,fileinfo->owner);
					pressanykey();
				}
			}
		}
		else
		{
			clear();
			move(3,5);
			printf("²»ÄÜ¶Ô´ËIDÖ´ĞĞ·â½û,ÈçÓĞÎÊÌâ,ÇëÓëÏµÍ³Î¬»¤ÁªÏµ.\n");
			pressanykey();
		}
	}
	else
	{
		if(getuser(fileinfo->owner))
		{
			//clear();
			//move(2,0);
			if(askyn("È·¶¨Òª½â·âÂğ?",YEA,YEA)==YEA)
                        {
				clear();
                                getdata(3,0,"ÊäÈë½â·â¸½ÑÔ: ", buf,40,DOECHO,YEA);
                                if (deldeny(fileinfo->owner,1))
				{
					sprintf(repbuf, "»Ö¸´%sÔÚ%s°åµÄ·¢ÎÄÈ¨ÏŞ",
							fileinfo->owner,currboard);
					securityreport4(repbuf);
					sprintf(msgbuf, "\n  %s ÍøÓÑ£º\n\n"
							"\tÒò·â½ûÊ±¼äÒÑ¹ı£¬ÏÖ»Ö¸´ÄúÔÚ [%s] °åµÄ¡º·¢±íÎÄÕÂ¡»È¨Á¦¡£\n\n",
							fileinfo->owner, currboard);
					if(strcmp(buf, ""))
						sprintf(msgbuf, "%s\t½â·â¸½ÑÔ£º[%s]\n", msgbuf, buf);
					autoreport(repbuf,msgbuf,YEA,fileinfo->owner);
					pressanykey();
				}
			}
		}
        }
        clear();
        return FULLUPDATE;
}

