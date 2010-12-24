//bbspst.wml.c
//huangxu@nkbbs.org

//²Ù×÷£ºĞÂ½¨£¬±à¼­£¬±£´æ
//¶ÔÏó£ºÎÄÕÂ£¬ĞÅ¼ş

#include "BBSLIB.inc"
#include "WMLLIB.inc"
#include "boardrc.inc"

//bbspst.wml?board=(1)&tite=(2)&userid=(3)&file=(4)&act=(5)&boardmode=(6)
//ÓÊ¼ş£ºboard=*mail&title=%s&userid=%s&file=%s
//·¢ÎÄ£ºboard=%s&title=%s&file=%s

//act = 0: ĞÂ½¨
//act = 1: Ìá½»
//act = 256: ±à¼­
//act = 257: ±£´æ

char board[80];
char title[256];
char userid[256];
char file[256];
struct shortfile * b;
int act;
int boardmode;

int bbspst_act(char * info)
{
	char buf[1024], buf2[1024];
	char * p, * cont;
	int tmp, i;
	int ismail, isbm, qmd;
	int nore, outgo;
	FILE * fp;
	struct fileheader fh;
	memset (&fh, 0, sizeof(fh));
	if (!loginok)
	{
		sprintf (info, "´Ò´Ò¹ı¿ÍÎŞÈ¨Ê¹ÓÃ¸ÃÒ³£¡ÇëÊ¹ÓÃÄúµÄ¼ÆËã»ú·ÃÎÊ%sÒÔ×¢²áID¡£", BBSNAME);
		return -1;
	}
	act = atoi(getparm("act"));// ^ 65536;//ÎÒ¾ö¶¨Ä¬ÈÏwml2.x¡£
	strncpy (board, getparm("board"), 79);
	strncpy (title, getparm("title"), 79);
	strncpy (userid, getparm("userid"), 255);
	strncpy (file, getparm("file"), 255);
	nore = atoi(getparm("nore"));
	outgo = atoi(getparm("outgo"));
	boardmode = atoi(getparm("boardmode"));
	b = getbcache(board);
	ismail = !strcmp(board, WML_VB_MAIL) || *userid;
	wmlmode(u_info, ismail ? SMAIL : POSTING);
	isbm = ismail || has_BM_perm(&currentuser, board);
	if (act == 0 || act == 1)
	{
		if (!HAS_PERM(PERM_POST))
		{
			sprintf (info, "ÄúÉĞÎ´»ñµÃ·¢ÎÄÈ¨ÏŞ¡£");
			return -1;
		}
		if (!ismail && !isAllowedIp(fromhost))
		{
			sprintf (info, "¶Ô²»Æğ£¬ÄúĞèÒª°ó¶¨ÖĞ¹ú´óÂ½µØÇø½ÌÓıÍøÓÊÏä²ÅÄÜ·¢ÎÄ¡£ÇëÊ¹ÓÃÄúµÄ¼ÆËã»ú·ÃÎÊ±¾Õ¾ÒÔÖ´ĞĞ´Ë²Ù×÷¡£");
			return -5;
		}
		if (!ismail && !has_post_perm(&currentuser, board))
		{
			sprintf (info, "ÄúÎŞÈ¨ÔÚ´Ë°åÃæ·¢±íÎÄÕÂ¡£");
			//TODO: Ç¿ÖÆÄ£°å·¢ÎÄ
			return -1;
		}
		if (!ismail && boardmode && !(act & 256))
		{
			sprintf (info, "Õı³£Ä£Ê½ÏÂ²ÅÄÜ·¢±íÎÄÕÂ¡£");
			return -6;
		}
	}
	//boardModeDir(board, boardmode, buf, buf2);
	tmp = fhfromdir(file, board, boardmode, &fh);
	if ((act & 256) && tmp < 0)
	{
		sprintf (info, "¸ÃÎÄÕÂ²»´æÔÚ¡£");
		return -2;
	}
	if (act & 256)
	{
		if (!ismail && !isbm && strcmp(currentuser.userid, fh.owner))
		{
			sprintf (info, "ÄúÎŞÈ¨±à¼­¸ÃÎÄÕÂ¡£");
			return -3;
		}
	}
	if (!*title && (act & 255) && !(act & 256))//¿Õ±êÌâ
	{
		if (ismail)
		{
			strcpy (title, "Ã»Ö÷Ìâ");
		}
		else
		{
			sprintf (info, "ÎÄÕÂ±ØĞëÓĞ±êÌâ¡£");
			return -4;
		}
	}
	//if (!ismail && check_post_limit(board))
	//WARNING: ¹àË®»ú²¿·ÖÊÇÖ±½ÓÊä³ö¶«Î÷µÄ£¬±È½ÏÓôÃÆ-_-b
	//·´ÕıÏÖÔÚÒ²Ã»ÓĞÏŞÖÆ¡£
	//ps:±¾ÈËÇ¿ÁÒ·´¶Ô¹àË®»ú¡£
	//{
	//	return -6;
	//}
	if (!(act & 256))
	{
		if (!isbm && (fh.accessed[0] & FILE_NOREPLY || b->flag & NOREPLY_FLAG))
		{
			sprintf (info, "¸ÃÎÄÕÂº¬ÓĞ²»¿ÉREÊôĞÔ£¬Äú²»ÄÜ»Ø¸´¸ÃÎÄÕÂ¡£");
			return -4;
		}
	}
	if ((act & 255) == 0)//±à¼­Ä£Ê½
	{
		printf ("<card title=\"%s %s -- %s\">", !ismail ? board : "Ğ´ĞÅ", (act & 256) ? "±à¼­" : "ĞÂ½¨", BBSNAME);
		if (!(act & 65536))
		{
			printf ("<form action=\"bbspst.wml?board=%s&amp;act=%d\" method=\"post\">", ismail ? WML_VB_MAIL : board, act | 1);
		}
		if (ismail)
		{
			printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />ĞÅÏä</anchor></p>", WML_VB_MAIL);
		}
		else
		{
			printf ("<p>°åÃæ£º<anchor><go href=\"bbsdoc.wml?board=%s\" />", board);
			w_hprintf ("%s/%s", b->title + 11, b->filename);
			printf ("</anchor></p>");
		}
		strcpy (buf2, title);
		if (!*buf2)
		{
			p = fh.title;
			if (*p)
			{
				if (!strncasecmp(p, "Re: ", 3))
				{
					p += 4;
				}
				sprintf (buf2, "%s%s", (act & 256) ? "" : "Re: ", p);
			}
		}
		if (!(act & 256))
		{
			w_hsprintf (buf, "%s", buf2);
			if (act & 65536)
			{
				printf ("<p>±êÌâ£º<input type=\"text\" name=\"inp_title\" value=\"%s\" /></p>", buf);
			}
			else
			{
				printf ("<p>±êÌâ£º<textarea name=\"title\" row=\"1\" col=\"18\">%s</textarea></p>", buf);
			}
		}
		if (ismail)
		{
			w_hsprintf (buf, "%s", userid);
			if (act & 65536)
			{
				printf ("<p>ËÍ¸ø£º<input type=\"text\" name=\"inp_userid\" value=\"%s\" /></p>", buf);
			}
			else
			{
				printf ("<p>ËÍ¸ø£º<textarea name=\"userid\">%s</textarea></p>", buf);
			}
		}
		if (!(act & 256))
		{
			printf ("<p>Ç©Ãûµµ£º<select title=\"Ç©Ãûµµ\" multiple=\"false\" name=\"%sqmd\" value=\"0\">", (act & 65536) ? "inp_" : "");
			printf ("<option value=\"0\" title=\"ÎŞ\">ÎŞ</option>");
			for (i = 1; i < 6; i++)
			{
				printf ("<option value=\"%d\" title=\"%d\">%d</option>", i, i, i);
			}
			printf ("</select></p>");
		}
		if (!(act & 65536))
		{
			printf ("<p>");
			if (!ismail)
			{
				printf ("<input type=\"checkbox\" name=\"nore\" value=\"1\" />²»¿ÉRE ");
			}
			if (b && (b->flag & OUT_FLAG))
			{
				printf ("<input type=\"checkbox\" name=\"outgo\" />×ªĞÅ ");
			}
			printf ("</p>");
			printf ("<textarea name=\"cont\" col=\"20\" rows=\"5\">");
		}
		else
		{
			if (act & 256)
			{
				printf ("<p>Ç¿ÁÒ½¨Òé²»ÒªÔÚWML1.xÄ£Ê½ÏÂ±à¼­ÎÄÕÂ¡£</p>");
			}
			printf ("<p><input type=\"text\" name=\"inp_cont\" value=\"");
		}
		if (strcmp(board, WML_VB_MAIL))//ÕâÀï²»ÄÜÓÃismail
		{
			sprintf (buf, "boards/%s/%s", board, fh.filename);
		}
		else
		{
			sprintf (buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fh.filename);
		}
		if (!file_isfile(buf))
		{
			fp = NULL;
		}
		else
		{
			fp = fopen(buf, "rt");//WARNING£ºÕâÀïµÄfp¿ÉÄÜÊÇ¿Õ£¡
		}
		skipHeader(fp);
		if (!(act & 256))//ĞÂ½¨/»Ø¸´
		{
			if (fp && (!(act & 65536)))
			{
				printf ("\n\n");
				if (fh.owner)
				{
					w_hprintf ("¡¾ ÔÚ %s µÄ´ó×÷ÖĞÌáµ½: ¡¿", fh.owner);
					printf ("\n");
				}
				for (i = 0; i < 6; i++)
				{
					if (!readFileLine(buf, 256, fp))
					{
						break;
					}
					else
					{
						w_hprintf (": %s", buf);
						printf ("\n");
					}
				}
			}
		}
		else//±à¼­
		{
			while (readFileLine(buf, 256, fp))
			{
				w_hprintf ("%s", buf);
				if (!(act & 65536))
				{
					printf ("\n");
				}
			}
		}
		if (fp)
		{
			fclose(fp);
		}
		if (!(act & 65536))
		{
			printf ("</textarea>");
			printf ("<p><input type=\"submit\" value=\"Ìá½»\" />&#32;&#32;<anchor><prev />·µ»Ø</anchor></p></form>");
		}
		else
		{
			printf ("\" /></p>");
			printf ("<p><anchor>Ìá½»<go href=\"bbspst.wml?board=%s&amp;act=%d\" method=\"post\">\
<postfield name=\"title\" value=\"$(inp_title)\"/>\
<postfield name=\"userid\" value=\"$(inp_userid)\"/>\
<postfield name=\"cont\" value=\"$(inp_cont)\"/>\
<postfield name=\"qmd\" value=\"$(inp_qmd)\"/>\
</go></anchor>&#32;&#32;<anchor><prev />·µ»Ø</anchor></p>", ismail ? WML_VB_MAIL : board, act | 1);
		}
		printf ("<p><anchor><go href=\"bbspst.wml?board=%s&amp;userid=%s&amp;file=%s&amp;act=%d&amp;boardmode=%d\" />ÇĞ»»µ½WML%d.xÄ£Ê½</anchor></p>", board, userid, file, act ^ 65536, boardmode, (act & 65536) ? 2 : 1);
	}
	else//Ìá½»Ä£Ê½
	{
		if (ismail)
		{
			nore = 0;
		}
		if (act & 256)//ĞŞ¸Ä
		{
			printf ("<card title=\"±£´æ -- %s\">", BBSNAME);
			if (!ismail)//ÕâÀï¾ÍÊÇismailÁË
			{
				sprintf (buf, "boards/%s/%s", board, fh.filename);
			}
			else
			{
				sprintf (buf, "mail/%c/%s/%s", toupper(currentuser.userid[0]), currentuser.userid, fh.filename);
			}
			fp = fopen(buf, "rt+");
			if (fp)
			{
				skipHeader(fp);
				ftruncate (fileno(fp), ftell(fp));
				fprintf (fp, "%s", getparm("cont"));
				fprintf (fp, "\n[1;%dm¡ù ĞŞ¸Ä:¡¤ %s %s ¡¤Wap[FROM: %.20s][m\n", 31+rand()%7, BBSNAME, BBSHOST, fromhost);
				fclose (fp);
			}
			else
			{
				printf ("<p>ÎŞ·¨´ò¿ªÎÄ¼ş¡£</p>");
			}
		}
		else//·¢ÎÄ»òĞÅ
		{
			qmd = atoi(getparm("qmd"));
			cont = getparm("cont");//×¢ÒâÔİÊ±²»ÄÜgetparm
			if (!ismail && !(b->flag2&NOFILTER_FLAG) && (bad_words(title) || bad_words(cont) ))
			{
				//printf ("ÄúµÄÎÄÕÂ¿ÉÄÜ°üº¬²»±ãÏÔÊ¾µÄÄÚÈİ£¬ÒÑ¾­Ìá½»ÉóºË¡£Çë·µ»Ø²¢½øĞĞĞŞ¸Ä¡£");
				sprintf (buf2, "TEMP-%s", board);
				strcpy (board, "TEMP");
			}
			else
			{
				strcpy (buf2, board);
			}
			sprintf (buf, "tmp/%d.%s.tmp", getpid(),currentuser.userid);
			f_append (buf, cont);//½ûÖ¹getparm½â³ı
			if (ismail)
			{
				tmp = post_mail_d(userid, title, buf, currentuser.userid, currentuser.username, fromhost, qmd - 1, 1, "Wap");
			}
			else
			{
				if (nore)
				{
					nore = FILE_NOREPLY;
				}
				tmp = post_article2_d (buf2, title, buf, currentuser.userid, currentuser.username, fromhost, qmd - 1, nore, 0, outgo, "Wap");
				if(!junkboard(board)) 
				{
					currentuser.numposts++;
					save_user_data(&currentuser);
				}
			}
			unlink (buf);
			if (tmp <= 0)
			{
				sprintf (info, "Äú·¢ËÍµÄ%sÊ§°Ü¡£", ismail ? "ĞÅ¼ş" : "ÎÄÕÂ");
				return -7;
			}
			if (!ismail)
			{
				sprintf(buf2, "M.%d.A", tmp);
				brc_init(currentuser.userid, board);
				brc_add_read(buf2);
				brc_update(currentuser.userid, board);
				sprintf(buf, "posted Wap '%s' on '%s'", title, board);
			}
			else
			{
				sprintf(buf, "mailed Wap %s", userid);
			}
			report (buf);
			printf ("<card title=\"³É¹¦ -- %s\" ontimer=\"bbsdoc.wml?board=%s\">", BBSNAME, board);
			printf ("<timer value=\"30\" />");
			if (!strcmp (board, "TEMP"))
			{
				printf ("<p><em>Äú·¢ËÍµÄÄÚÈİº¬ÓĞÃô¸Ğ´Ê£¬ĞèÒªÉóºËºó²ÅÄÜ³öÏÖÔÚ°åÃæÉÏ¡£</em></p>");
			}
			printf ("<p>Äú·¢ËÍµÄ%sÒÑ³É¹¦ËÍ³ö¡£</p>", ismail ? "ĞÅ¼ş" : "ÎÄÕÂ");
			printf ("<p>×Ô¶¯Ìø×ªÖĞ¡­¡­</p>");
			printf ("<p><anchor><go href=\"bbsdoc.wml?board=%s\" />Èç¹û²»ÄÜ×Ô¶¯Ìø×ª£¬ÇëÊ¹ÓÃ´ËÁ´½Ó¡£</anchor></p>", board);
		}
	}
	return 0;
}

int main()
{
	char buf[1024];
	wml_init();
	wml_head();
	if (bbspst_act(buf))
	{
		printf ("<card title=\"´íÎó\" >");
		printf ("<p>%s</p>", buf);
		printf ("<p><anchor><prev />·µ»Ø</anchor></p>");
	}
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
