//bbsop.wml.c
//huangxu@nkbbs.org

//¼òµ¥ÎÄÕÂ²Ù×÷

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"
#include "announce.inc"

//ÎÄÕÂ²Ù×÷
//bbsop.wml?board=(1)&mode=(2)&file=(3)&act=(4)
//act:
//0 -- ÏÔÊ¾Ò³Ãæ
//1 -- F
//2 -- ^C
//16 -- _x
//32 -- d
//256 -- G//ÐÅ¼þÎÞÐ§
//512 -- M
//1024 -- Y

int bbsop_act(char * info)
{
	char board[80], file[256];
	int boardmode, act;
	char buf[256], buf2[256], buf3[256];
	int i, j, tmp, pl, status, ismail;
	char * ptr;
	struct fileheader fh, fh2;
	struct shortfile * b;
	struct userec * u;
	struct AnnItem ai;
	FILE * fp;
	strncpy (board, getparm("board"), 79);
	strncpy (file, getparm("file"), 255);
	boardmode = atoi(getparm("mode"));
	act = atoi(getparm("act"));
	ismail = !strcmp(board, WML_VB_MAIL);
	//boardModeDir(board, boardmode, buf, buf2);
	if (!loginok || (!ismail && (!has_read_perm(&currentuser, board) || !HAS_PERM(PERM_POST))))
	{
		strcpy (info, "È¨ÏÞ²»¹»¡£");
		return -2;
	}
	tmp = fhfromdir(file, board, boardmode, &fh);
	b = getbcache(board);
	pl = (ismail || has_BM_perm(&currentuser, board)) << 8 | !strcmp(currentuser.userid, fh.owner);
	if (ismail)
	{
		boardmode = 0;
	}
	//256:°ßÖñ£¬1:ËùÓÐÕß
	if (tmp < 0)
	{
		sprintf (info, "²»ÕýÈ·µÄÎÄÕÂÐÅÏ¢¡£", tmp);
		return -1;
	}
	if (*board == '*' && (act & (16 | 256)))
	{
		strcpy (info, "ÐÅÏäÄÚÎÞ·¨Ê¹ÓÃÎÄÕª»ò²»¿É»Ø¸´±ê¼Ç¡£");
		return -1;
	}
	if ((boardmode & (1 | 8 | 16)) && (act & (16 | 256 | 512)))
	{
		strcpy (info, "´ËÄ£Ê½ÏÂ²»ÄÜ½øÐÐ±ê¼Ç¡£");
		return -1;
	}
	if (act != ACT_ANN_EMBODY && ((act & ~255) && !(pl & 256)) || ((act & 240) && !(pl & 257)))
	{
		strcpy (info, "È¨ÏÞ²»¹»¡£");
		return -2;
	}
	status = -1;//´íÎó
	strcpy (info, "²ÎÊý´íÎó");
	if (!ismail)
	{
		sprintf (buf, "boards/%s/%s", board, fh.filename);
	}
	else
	{
		sprintf (buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fh.filename);
		if (!file_exist(buf))
		{
			sprintf (buf, "mail/announce/%s", fh.filename);
		}
	}
	if (!(act != 32) && !file_exist(buf))
	{
		strcpy (info, "ÎÄ¼þÄÚÈÝÒÑ¶ªÊ§¡£");
		return -5;
	}
	switch (act)
	{
		case 1://F
			strncpy (buf3, getparm("to"), IDLEN + 1);
			snprintf(buf2, 60, "[×ª¼Ä] %s", fh.title);
			if (post_mail_d(buf3, buf2, buf, currentuser.userid, currentuser.username, fromhost, -1,1, "Wap×ª¼Ä") <= 0)
			{
				strcpy (info, "×ª¼ÄÊ§°Ü¡£");
				return -6;
			}
			break;
		case 2://^C
			strncpy (buf3, getparm("to"), 80);
			if (!has_post_perm(&currentuser, buf3))
			{
				sprintf (info, "Äú²»ÄÜÔÚ%s°åÕÅÌùÎÄÕÂ¡£", buf3);
				return -7;
			}
			snprintf (buf2, 60, "%s(×ªÔØ)", fh.title);
			if (post_article_d(buf3, buf2, buf, currentuser.userid, currentuser.username, fromhost, -1, "Wap×ªÔØ") <= 0)
			{
				sprintf (info, "×ªÔØÊ§°Ü¡£");
				return -8;
			}
			if (!ismail)
			{
				sprintf (buf, "Wap cross-post \"%s\" from %s to %s ", fh.title, board, buf3);
				report (buf);
			}
			break;
		case 16://_x
		case 256://G
		case 512://M
			if (act == 16)
			{
				tmp = FILE_NOREPLY;
			}
			else if (act == 256)
			{
				tmp = FILE_DIGEST;
				fh2 = fh;
				*fh2.filename = 'G';
				if (!(fh.accessed[0] & FILE_DIGEST))//ÕâÊÇÔ­À´Ã»ÓÐG±ê¼ÇµÄÇé¿ö
				{
					sprintf (buf2, "cp boards/%s/%s boards/%s/%s -f", board, fh.filename, board, fh2.filename);
					//´´½¨Ò»¸öG¸±±¾
					system (buf2);
					boardModeDir(board, 1, buf2, buf3);
					file_append (buf2, &fh2, sizeof(struct fileheader), 1);
					//Ð´ÈëGË÷Òý
				}
				else
					//ÕâÀïÒÑ¾­ÓÐG±ê¼ÇÁË£¬ÔÚ´ËÈ¥µô
				{
					sprintf (buf2, "boards/%s/%s -f", board, fh2.filename);
					unlink(buf2);
					//É¾³ýG¸±±¾
					boardModeDir(board, 1, buf2, buf3);
					seekDel(buf2, &fh2);
					//Í¬Ñù£¬ÇåÀí
				}
			}
			else if (act == 512)
			{
				tmp = FILE_MARKED;
			}
			fh.accessed[0] ^= tmp;
			if (boardModeDir(board, boardmode, buf, buf2) || writeBackDir(buf, &fh))
			{
				printf (info, "±ê¼ÇÊ§°Ü¡£");
				return -9;
			}
			break;
		case 32://d
		case 1024://Y
			//¿ÉÒÔÈ¡ÏûÖÃ¶¥¡¢É¾³ýÆÕÍ¨ÎÄÕÂºÍGÎÄÕÂ£¬Ö»ÓÐÆÕÍ¨ÎÄÕÂ½ø»ØÊÕÕ¾
			if (act == 32)
			{
				if (!(boardmode == 0 || boardmode == 1 || boardmode == 8))
				{
					sprintf (info, "ÔÚ´ËÄ£Ê½ÏÂ²»ÄÜÉ¾ÎÄ¡£");
					return -10;
				}
				if (!HAS_PERM(PERM_SYSOP))
				{
					if (strstr(board, "syssecurity") || (!strcmp(board, "Punishment") && !HAS_PERM(PERM_BLEVELS)))
					{
						sprintf (info, "ÄúÎÞÈ¨ÔÚ%s°åÉ¾ÎÄ¡£", board);
						return -11;
					}
				}
				if (!(pl & 256) && (fh.accessed[0] & FILE_MARKED))
				{
					sprintf (info, "Äú²»ÄÜÉ¾³ý±»°åÖ÷M±ê¼ÇµÄÎÄÕÂ¡£");
					return -14;
				}
				sprintf (buf3, "\n\n[1;%dm¡ù É¾³ý:¡¤%s %s ¡¤Wap[FROM: %.20s][m\n", 31 + rand()%7, BBSNAME, BBSHOST, fromhost);
				f_append (buf, buf3);
				if (!ismail && boardmode == 0)
				{
					u	=	getuser(fh.owner);
					if(!junkboard(board) && u) 
					{
						if(u->numposts>0)
						u->numposts--;
						save_user_data(u);
					}
				}
				if (boardModeDir(board, boardmode, buf, buf2) || boardModeDir(board, 4, buf2, buf3))
				{
					sprintf (info, "°åÃæ²ÎÊý´íÎó¡£");
					return -12;
				}
			}
			else
			{
				if (boardmode != 4)
				{
					sprintf (info, "Ö»ÄÜ»Ö¸´»ØÊÕÕ¾ÖÐµÄÎÄÕÂµ½°åÃæÉÏ¡£");
					return -13;
				}
				//ÕâÀï¿Ï¶¨ÊÇ°åÃæ£¬buf±äÁ¿ÉÐÎ´¸Ä±ä
				fp = fopen (buf, "rt");
				if (fp)
				{
					while (fgets(buf, 255, fp))
					{
						if (*buf >= 0 && *buf < ' ')
						{
							break;
						}
						else if (!strncmp(buf, "±ê  Ìâ: ", 8))
						{
							strcpy (fh.title, buf + 8);
							break;
						}
					}
				}
				if (boardModeDir(board, 4, buf, buf2) || boardModeDir(board, 0, buf2, buf3))
				{
					sprintf (info, "°åÃæ²ÎÊý´íÎó¡£");
					return -12;
				}
			}
			seekDel (buf, &fh);//´ÓÔ­À´µÄDIRÖÐÉ¾µô¼ÇÂ¼
			if ((boardmode == 0 || boardmode == 4) && !ismail)
				//ÕâÑùµÄ»°£¬Ò²¾ÍÊÇËµÒªÐ´ÈëÁíÒ»¸öDIR£¬¼´ÎÄ¼þÈÔÈ»ÓÐÐ§
			{
				file_append (buf2, &fh, sizeof(struct fileheader), 1);
				sprintf (buf, "%s %s: %s", act == 32 ? "deleted" : "restored", fh.filename, fh.title);
				report (buf);
			}
			else
			{
				if (boardmode != 8)//Èç¹ûÊÇ8µÄ»°£¬ÖÃ¶¥ÎÄÕÂÖ»ÊÇÖ¸Õë¶øÒÑ
				{
					//·ñÔòµÄ»°£¬ÒªÉ¾µôÎÄ¼þ
					if (ismail)
						//¶øÇÒ£¬¹«¸æ²»»á±»É¾µôµÄ
					{
						sprintf (buf, "mail/%c/%s/%s", toupper(*currentuser.userid), currentuser.userid, fh.filename);
					}
					else
					{
						sprintf (buf, "boards/%s/%s", board, fh.filename);
					}
					unlink (buf);
					//ÎÄÕªÇø²»½ø»ØÊÕÕ¾£¬É¾µôÎÄ¼þ¡£
					//g±ê¼ÇÔõÃ´°ì£¿È¥µôG±ê¼Ç°É£¿
					if (boardmode == 1)//G±ê¼ÇµÄ´¦Àí
					{
						*fh.filename = 'M';
						strcpy (buf, fh.filename);
						if (fhfromdir(buf, board, 0, &fh) >= 0)//ÕÒµ½µÄ»°
						{
							fh.accessed[0] &= ~FILE_DIGEST;
							boardModeDir(board, 0, buf2, buf3);
							writeBackDir(buf2, &fh);
							//ÕâÀïºÃÂÒµÄËµ...ÖØ¸´±éÀúÁËÈô¸É´Î¡£¹ÃÇÒÕâÑù¡£
						}
					}
				}
			}
			//WARNING ÕâÀï¸Ä±äÁËboardmode
			if (boardmode == 4)
			{
				boardmode = 0;
			}
			else
			{
				boardmode = 4;
			}
			break;
		case ACT_ANN_EMBODY:
			sethomefile(buf2, currentuser.userid, STR_ANN_SINKROAD);
			fp = fopen(buf2, "rt");
			if (!fp)
			{
				sprintf (info, "Ë¿Â·ÊÇ¿ÕµÄ¡£");
				return -7;
			}
			fgets(buf2, LIM_ANN_PATHLEN, fp);
			fclose (fp);
			if (!*buf2)
			{
				sprintf (info, "Ë¿Â·ÊÇ¿ÕµÄ¡£");
				return -7;
			}
			memset (&ai, 0, sizeof (ai));
			strcpy (ai.path, fh.filename);
			strcpy (ai.owner, currentuser.userid);
			strncpy (ai.title, fh.title, 38);
			ai.flag = FLAG_ANN_FILE;
			sprintf (buf3, "0Announce/%s/%s", buf2, ai.path);
			if (file_exist(buf3))
			{
				sprintf (info, "¸ÃÄ¿Â¼ÒÑ¾­ÓÐÍ¬ÃûÎÄ¼þ´æÔÚ¡£");
				return -4;
			}
			sprintf (buf3, "cp -f %s 0Announce/%s/", buf, buf2);
			system(buf3);
			appendAnnIndex(buf2, &ai);
			break;
		case 0://Ä¬ÈÏ
			break;
		default:
			strcpy (info, "Î´Ö¸¶¨µÄ´íÎó");
			return -3;
			break;
	}
	if (act)//²Ù×÷³É¹¦
	{
		if (act == 32)
		{
			sprintf (buf, "bbsdoc.wml?board=%s", board);
		}
		else
		{
			sprintf (buf, "bbscon.wml?board=%s&amp;file=%s&amp;mode=%d", board, file, boardmode);
		}
		printf ("<card title=\"²Ù×÷³É¹¦ -- %s\" ontime=\"%s\">", BBSNAME, buf);
		printf ("<timer value=\"30\" />");
		printf ("<p>×Ô¶¯Ìø×ªÖÐ¡­¡­</p>");
		printf ("<p><anchor><go href=\"%s\" />Èç¹û²»ÄÜ×Ô¶¯Ìø×ª£¬ÇëÊ¹ÓÃ´ËÁ´½Ó¡£</anchor></p>", buf);
		return 0;
	}
	//ÕâÀïÊÇÏÔÊ¾ÁÐ±íµÄÊ±ºòÁË
	printf ("<card title=\"Ñ¡Ôñ²Ù×÷ -- %s\">", BBSNAME);
	if (b)
	{
		printf ("<p>°åÃæ£º<anchor><go href=\"bbsdoc.wml?board=%s&amp;mode=%d\" />%s/%s</anchor><br />", b->filename, boardmode, b->title + 11, b->filename);
	}
	else
	{
		printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />ÐÅÏä</anchor><br />", WML_VB_MAIL);
	}
	w_hsprintf (buf, "%s", fh.title);
	printf ("±êÌâ£º<anchor><go href=\"bbscon.wml?board=%s&amp;file=%s&amp;mode=%d\" />%s</anchor><br />", board, file, boardmode, buf);
	strncpy (buf, fh.owner, IDLEN + 1);
	for (ptr = buf; *ptr; ptr++)
	{
		if (!(isalnum(*ptr)))
		{
			*ptr = 0;
			break;
		}
	}
	printf ("%s£º<anchor><go href=\"bbsqry.wml?userid=%s\" />%s</anchor><br />", b ? "×÷Õß" : "À´×Ô", buf, buf);
	printf ("±ê¼Ç£º%s</p>", flag_str2 (fh.accessed[0], *board == '*' ? (fh.accessed[0] & FILE_READ) : brc_has_read(fh.filename)));
	sprintf (buf, "bbsop.wml?board=%s&amp;mode=%d&amp;file=%s&amp;act=", board, boardmode, file);
	printf ("<p><input name=\"inp_to\" /><anchor>×ª¼Ä<go href=\"%s%d\" method=\"post\"><postfield name=\"to\" value=\"$(inp_to)\"/></go></anchor>&#32;<anchor>×ªÔØ<go href=\"%s%d\" method=\"post\"><postfield name=\"to\" value=\"$(inp_to)\"/></go></anchor></p>", buf, 1, buf, 2);
	printf ("<p><anchor><go href=\"bbsqry.wml?userid=%s\" />×÷Õß</anchor>&#32;<anchor><go href=\"bbsop.wml?board=%s&amp;mode=%d&amp;file=%s&amp;act=%d\" />¾«»ª</anchor>&#32;", fh.owner, board, boardmode, fh.filename, ACT_ANN_EMBODY);
	if (boardmode == 0 && (!(fh.accessed[0] & FILE_NOREPLY) || has_BM_perm(&currentuser, board)) && has_post_perm(&currentuser, board) && isAllowedIp(fromhost))
	{
		printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;act=0\" />»Ø¸´</anchor>&#32;", board, fh.filename);
	}
	printf ("<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;userid=%s&amp;act=0\" />»ØÐÅ</anchor>&#32;", board, fh.filename, fh.owner);
	printf ("</p>");
	if ((pl & 257) && boardmode == 0)
	{
		printf ("<p><anchor><go href=\"%s%d\" />É¾³ý</anchor>&#32;<anchor><go href=\"bbspst.wml?board=%s&amp;file=%s&amp;act=256\" />±à¼­</anchor></p>", buf, 32, board, file);
	}
	if (pl & 256)
	{
		printf ("<p>");
		if (boardmode == 0)
		{
			if (!ismail)
			{
				printf ("<anchor><go href=\"%s%d\" />²»¿ÉRE</anchor>&#32;<anchor><go href=\"%s%d\" />G±ê¼Ç</anchor>&#32;", buf, 16, buf, 256);
			}
			printf ("<anchor><go href=\"%s%d\" />M±ê¼Ç</anchor>&#32;", buf, 512);
		}
		else if (boardmode == 4)
		{
			printf ("<anchor><go href=\"%s%d\" />»Ö¸´</anchor>", buf, 1024);
		}
		printf ("</p>");
	}
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbsop_act(buf))
	{
		printf ("<card title=\"´íÎó -- %s\" >", BBSNAME);
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />·µ»Ø</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
