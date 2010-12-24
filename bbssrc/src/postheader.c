/*
$Id: postheader.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"

extern int numofsig;
extern int noreply;
extern int pattern;
extern int mailtoauther;

struct shortfile *getbcache();

void check_title(char *title)
{
	if(killwordsp(title)==0)
		return;
	if(title[0]!='R'||(title[1]!='e'&&title[1]!='E')||title[2]!=':')
		return ;
	title[0] = 'r';
}



int post_header(struct postheader* header)
{
	int     anonyboard = 0;
	char    r_prompt[20], mybuf[256], ans[5],pt[16];
	char    titlebuf[STRLEN];
	struct shortfile *bp;
#ifdef RNDSIGN

	int     oldset, rnd_sign = 0;
#endif

	if (currentuser.signature > numofsig || currentuser.signature < 0)
		currentuser.signature = 1;

#ifdef RNDSIGN

	if (numofsig > 0)
	{
		if (DEFINE(DEF_RANDSIGN))
		{
			oldset = currentuser.signature;
			srand((unsigned) time(0));
			currentuser.signature = (rand() % numofsig) + 1;
			rnd_sign = 1;
		}
		else
		{
			rnd_sign = 0;
		}
	}
#endif

	if (header->reply_mode) {
		strcpy(titlebuf, header->title);
		header->include_mode = 'S';
	} else
		titlebuf[0] = '\0';
	bp = getbcache(currboard);
	if (header->postboard)
		anonyboard = bp->flag & ANONY_FLAG;
	header->chk_anony = (anonyboard) ? 0 : 0;


        int forcepattern=0;
        //if(seek_in_file("etc/patternpost",currboard))
	if(getbcache(currboard)->flag2&TEMPLATE_FLAG)//loveni
		forcepattern=1;
       
	while (1) {
		if (header->reply_mode)
			sprintf(r_prompt, "ÒýÑÔÄ£Ê½ [[1m%c[m]", header->include_mode);
		else{
                         if(forcepattern) 
				pattern=1;
			 if(header->postboard&&pattern)
			   strcpy(pt,"[[1;33mÄ£°å·¢ÎÄ[m]");
                         else
                	   strcpy(pt,"");
               }

		move(t_lines - 4, 0);
		clrtobot();
		prints("[m%s [1m%s[m      %s    %s%s\n",
			(header->postboard) ? "·¢±íÎÄÕÂÓÚ" : "ÊÕÐÅÈË£º", header->ds,
			(anonyboard) ? (header->chk_anony == 1 ? "[1mÒª[mÊ¹ÓÃÄäÃû" : "[1m²»[mÊ¹ÓÃÄäÃû") : "",
	(header->postboard)?((noreply)?"[±¾ÎÄ[1;33m²»¿ÉÒÔ[m»Ø¸´":"[±¾ÎÄ[1;33m¿ÉÒÔ[m»Ø¸´"):"",
	(header->postboard&&header->reply_mode)?((mailtoauther)?",ÇÒ[1;33m·¢ËÍ[m±¾ÎÄÖÁ×÷ÕßÐÅÏä]":",ÇÒ[1;33m²»·¢ËÍ[m±¾ÎÄÖÁ×÷ÕßÐÅÏä]"):(header->postboard)?"]":"");
/*	
	// efan: for secondhand	
	tmpbuf [0] = 0;
	if (!strcmp (header->ds, "Secondhand")) {
		char tmptmpbuf [5];
		tmptmpbuf [0] = 0;
		move (t_lines - 2, 0);
		prints("ÇëÊäÈëÄúµÄÐèÇó£¬ÀýÈç£º¡¾ Âò ¡¿¡¾ Âô ¡¿¡¾Ñ¯ÎÊ¡¿¡¾¹À¼Û¡¿¡¾×ªÈÃ¡¿¡­¡­\n");
		prints ("(À¨ºÅ¿ÉÊ¡ÂÔ)");
		getdata(t_lines - 3, 0, "ÄúÏëÒª: ", tmptmpbuf, 5, DOECHO, NA);
		if (tmptmpbuf [0] == 0)
			return NA;
		sprintf (tmpbuf, "¡¾%s¡¿", tmptmpbuf);
	}
*/

		prints("Ê¹ÓÃ±êÌâ: [1m%-50s[m\n", (header->title[0] == '\0') ? "[ÕýÔÚÉè¶¨Ö÷Ìâ]" : header->title);


#ifdef RNDSIGN

		prints("Ê¹ÓÃµÚ [1m%d[m ¸öÇ©Ãûµµ     %s %s", currentuser.signature
			,(header->reply_mode) ? r_prompt : pt, (rnd_sign == 1) ? "[Ëæ»úÇ©Ãûµµ]" : "");
#else

		prints("Ê¹ÓÃµÚ [1m%d[m ¸öÇ©Ãûµµ     %s", currentuser.signature
			,(header->reply_mode) ? r_prompt : pt);

#endif

		if (titlebuf[0] == '\0') {
			move(t_lines - 1, 0);
			if (header->postboard == YEA || strcmp(header->title, "Ã»Ö÷Ìâ"))
				strcpy(titlebuf, header->title);
			getdata(t_lines - 1, 0, "±êÌâ: ", titlebuf, 50, DOECHO, NA);
			check_title(titlebuf);
			if (titlebuf[0] == '\0') {
				if (header->title[0] != '\0') {
					titlebuf[0] = ' ';
					continue;
				} else
					return NA;
			}
			strcpy(header->title, titlebuf);
			continue;
		}
		move(t_lines - 1, 0);
		sprintf(mybuf,
		     "[1;32m0[m~[1;32m%d V[m¿´Ç©Ãûµµ%s [1;32mX[mËæ»úÇ©Ãûµµ,[1;32mT[m±êÌâ%s%s%s%s,[1;32mQ[m·ÅÆú:",
			numofsig, (header->reply_mode) ? ",[1;32mS[m/[1;32mY[m/[1;32mN[m/[1;32mR[m/[1;32mA[m ÒýÑÔÄ£Ê½" : "", (anonyboard) ? "£¬[1;32mL[mÄäÃû" : "",(header->postboard)?",[1;32mU[mÊôÐÔ":"",(header->postboard&&header->reply_mode)?",[1;32mM[m¼ÄÐÅ":"",(header->postboard&&!header->reply_mode)?",[1;32mP[mÊ¹ÓÃÄ£°å":"");
		getdata(t_lines - 1, 0, mybuf, ans, 3, DOECHO, YEA);
		ans[0] = toupper(ans[0]);
		if ((ans[0] - '0') >= 0 && ans[0] - '0' <= 9) {
			if (atoi(ans) <= numofsig)
				currentuser.signature = atoi(ans);
                } else if ( ans[0] == 'Q' || ans[0] == 'q' ) {
                          pattern=0;
			  return -1;
		} else if (header->reply_mode &&
			(ans[0] == 'Y' || ans[0] == 'N' || ans[0] == 'A' || ans[0] == 'R'||ans[0]=='S')) {
			header->include_mode = ans[0];
		} else if (ans[0] == 'T') {
			titlebuf[0] = '\0';
		} else if (ans[0] == 'L' && anonyboard) {
			header->chk_anony = (header->chk_anony == 1) ? 0 : 1;
		} else if (ans[0] == 'U' && header->postboard){
			noreply = ~noreply;
		} else if (ans[0] == 'P' && header->postboard){
			pattern = ~pattern;
		} else if (ans[0] == 'M' && header->postboard 
			&& header->reply_mode) {
			mailtoauther = ~mailtoauther;
		} else if (ans[0] == 'V') {
			setuserfile(mybuf, "signatures");
			if (askyn("Ô¤ÉèÏÔÊ¾Ç°Èý¸öÇ©Ãûµµ, ÒªÏÔÊ¾È«²¿Âð", NA, YEA) == YEA)
				ansimore(mybuf);
			else {
				clear();
				ansimore2(mybuf, NA, 0, 18);
			}
#ifdef RNDSIGN

		}
		else if (ans[0] == 'X')
		{
			if (rnd_sign == 0 && numofsig != 0)
			{
				oldset = currentuser.signature;
				srand((unsigned) time(0));
				currentuser.signature = (rand() % numofsig) + 1;
				rnd_sign = 1;
			}
			else if (rnd_sign == 1 && numofsig != 0)
			{
				rnd_sign = 0;
				currentuser.signature = oldset;
			}
			ans[0] = ' ';
#endif

		}
		else
		{
			if (header->title[0] == '\0')
				return NA;
			else
				return YEA;
		}
	}
}

int check_anonyboard(char* currboard)
{
	struct shortfile *bp;
	int     anonyboard = 0;
	bp = getbcache(currboard);
	anonyboard = bp->flag & ANONY_FLAG;
	if (anonyboard == 8)
	{
		return YEA;
		//this is because anony_flag = 8 = 100 in binary
	}
	else
	{
		return NA;
	}

}
