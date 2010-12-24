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
$Id: delete.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif
int seclusionday=0;
FILE *fp;
char tmp[80];
void mail_info(int mode,char *lastword);


//int offline()
void d_offline(int mode)
{
	int i;
	char    buf[STRLEN], lastword[640];

	modify_user_mode(mode?RECLUSION:OFFLINE);
	clear();
	if (count_user( )>1)
	{/*Added by Tim 1999/10/14. avoid suiciding or fading when multi-login
		            this is a serious bug..vectra is a victim due to the bug.
		            wuwuw....now, vectra is a newcomer...:(*/
		move(1,0);
		prints("ƒ„ƒø«∞µƒlogin≤ª÷π“ª∏ˆ,«ÎÕÀ≥ˆ∆‰À˚login‘Ÿ÷ÿ ‘“ª¥Œ\n");
		pressreturn();
		clear();
		return;
	}

	if (HAS_PERM(PERM_BOARDS) || HAS_PERM(PERM_ADMINMENU) || HAS_PERM(PERM_CHATCLOAK))
	{
		move(1, 0);
		prints("\n\nƒ˙”–÷ÿ»Œ‘⁄…Ì, ≤ªƒ‹ÀÊ±„%s¿≤!!\n",mode?"πÈ“˛":"◊‘…±");
		pressreturn();
		clear();
		return;
	}
	if (!HAS_PERM(PERM_BASIC)||!HAS_PERM(PERM_CHAT)||!HAS_PERM(PERM_PAGE)||!HAS_PERM(PERM_POST)||!HAS_PERM(PERM_LOGINOK)||!HAS_PERM(PERM_MESSAGE))
	{
		move(1, 0);
		prints("\n\n∂‘≤ª∆, ƒ˙ªπŒ¥πª◊ ∏Ò÷¥––¥À√¸¡Ó!!\n");
		prints("«Î mail ∏¯ SYSOP Àµ√˜‘≠“Ú, –ª–ª°£\n");
		pressreturn();
		clear();
		return;
	}  /*loveni:“˛æ”◊‘…±≤ª‘Ÿœﬁ÷∆*/
	getdata(1, 0, "«Î ‰»Îƒ„µƒ√‹¬Î: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
	{
		prints("\n\n∫‹±ß«∏, ƒ˙ ‰»Îµƒ√‹¬Î≤ª’˝»∑°£\n");
		pressreturn();
		clear();
		return;
	}
	getdata(3, 0, "«ÎŒ ƒ„Ω– ≤√¥√˚◊÷? ", buf, NAMELEN, DOECHO, YEA);
	if (*buf == '\0' || strcmp(buf, currentuser.realname))
	{
		prints("\n\n∫‹±ß«∏, Œ“≤¢≤ª»œ ∂ƒ„°£\n");
		pressreturn();
		clear();
		return;
	}
	clear();
	move(1, 0);
	if(mode)
	{
		//add by bluetent 2002.12.05
		int usrexp=0;
		prints("ƒø«∞πÊ∂®£∫\n–¬ ÷…œ¬∑ø…“‘“˛æ”3-5ÃÏ£¨“ª∞„’Ω”—ø…“‘“˛æ”3-15ÃÏ£¨\n÷–º∂’æ”—ø…“‘“˛æ”3-30ÃÏ£¨∏ﬂº∂’æ”—ø…“‘“˛æ”3-60ÃÏ£¨\n¿œ’æ”—ø…“‘“˛æ”3-90ÃÏ£¨≥§¿œº∂ø…“‘“˛æ”3-120ÃÏ£¨\n±æ’æ‘™¿œº∞∆‰“‘…œµ»º∂ø…“‘“˛æ”3-180ÃÏ°£\n”–πÿµ»º∂√Ë ˆ«Î»•BBSHelp∞Â≤Èø¥æ´ª™«¯°£\n¡ÌÕ‚£¨‘⁄“˛æ”∆⁄º‰£¨ƒ˙ø…“‘ π”√pop3∑˛ŒÒ ’»°’æƒ⁄–≈º˛.\n");
		//	move(8,0);
		sprintf( genbuf, "«Î—°‘Ò“˛æ” ±º‰(ÃÏ) [%d]: ", seclusionday);
		while(1)
		{
			getdata( 8, 0, genbuf, buf, 5, DOECHO,YEA);
			seclusionday = atoi( buf );
			if(seclusionday==0)
				return;
			else if(seclusionday>2&&seclusionday<181)
				break;
		}
		usrexp = currentuser.numposts + currentuser.numlogins / 5 + (time(0) - currentuser.firstlogin) / 86400 + currentuser.stay / 3600;
		if (usrexp<=100)
			seclusionday=(seclusionday>5?5:seclusionday);
		if (usrexp<=450)
			seclusionday=(seclusionday>15?15:seclusionday);
		if (usrexp<=850)
			seclusionday=(seclusionday>30?30:seclusionday);
		if (usrexp<=1500)
			seclusionday=(seclusionday>60?60:seclusionday);
		if (usrexp<=2500)
			seclusionday=(seclusionday>90?90:seclusionday);
		if (usrexp<=3000)
			seclusionday=(seclusionday>120?120:seclusionday);
		if (usrexp>5000)
			seclusionday=(seclusionday>180?180:seclusionday);

		//add end
		move(10,0);
		prints("[1;5;31mæØ∏Ê[0;1;31m£∫ πÈ“˛∫Û, ƒ˙Ω´%dÃÏŒﬁ∑®‘Ÿ”√¥À’ ∫≈Ω¯»Î±æ’æ£°£°\n", seclusionday);
		prints("[1;32mµ´’ ∫≈‘⁄ %d ÃÏ··ƒ„ø…“‘Ω‚≥˝πÈ“˛◊¥Ã¨ :( .....[m", seclusionday);
		pressanykey();
	}
	else
	{
		prints("æØ∏Ê£∫◊‘…±∫Û, ƒ˙µƒ…˙√¸¡¶Ω´ºı…ŸµΩ14ÃÏ£¨14ÃÏ∫Ûƒ˙µƒ’ ∫≈◊‘∂Øœ˚ ß");
		move(2,0);
		prints("‘⁄’‚14ÃÏƒ⁄»Ù∏ƒ±‰÷˜“‚µƒª∞£¨‘Úø…“‘Õ®π˝µ«¬º±æ’æ“ª¥Œª÷∏¥‘≠…˙√¸¡¶");
		move(3, 0);
		prints("[1;31m◊‘…±”√ªßΩ´∂™ ßÀ˘”–[33mÃÿ ‚»®œﬁ[31m£°£°£°[m");
	}
	clear();
	move(5,0);
	i = 0;
	if(askyn("’Ê «…·≤ªµ√ƒ„£¨ƒ„◊ﬂ÷Æ«∞”– ≤√¥ª∞œÎÀµ√¥",NA,NA)==YEA)
	{
		strcpy(lastword,">\n>");
		buf[0] = '\0';
		for(i = 0; i< 8; i++)
		{
			getdata(i+6, 0, ": ", buf, 77, DOECHO, YEA);
			if(buf[0] == '\0')
				break;
			strcat(lastword,buf);
			strcat(lastword,"\n>");
		}
		if(i == 0)
			lastword[0] = '\0';
		else
			strcat(lastword,"\n\n");
		move( i + 8, 0);
		if( i == 0 )
			prints("∞•£¨ƒ„ªπ « ≤√¥∂º≤ª‘∏“‚Àµ£¨ «≤ª «ªπ”––ƒÀºŒ¥¡À£ø");
		else if( i <= 4 )
			prints("ø¥◊≈ƒ„„æ„≤µƒ¡≥£¨Œ“–ƒ∂ºÀÈ¡À ... ");
		else
			prints("Œ“ª·º«µ√ƒ„µƒ£¨≈Û”—£¨Œ““≤÷™µ¿ƒ„µƒ¿Îø™“≤ «√ª”–∞Ï∑®µƒ ¬£¨∫√◊ﬂ¡À");
	}
	else
	{
		lastword[0] = '\0';
	}
	move( i + 10, 0);
	if (askyn("ƒ„»∑∂®“™¿Îø™’‚∏ˆ¥Ûº“Õ•", NA, NA) == 1)
	{
		clear();
		mail_info(mode,lastword);
		if(mode)
		{
			char buf[80];
			sprintf(buf, "home/%c/%s/Reclusion", toupper(currentuser.userid[0]), currentuser.userid);
			fp=fopen(buf,"w+");
			if(fp)
			{
				//	      flock(fileno(fp),LOCK_EX);
				fprintf(fp,"%d", seclusionday);
				//	      flock(fileno(fp),LOCK_UN);
				fclose(fp);
			}
			currentuser.userlevel|=PERM_RECLUSION;
		}
		else
		{
			if((currentuser.userlevel & PERM_PERSONAL) == 0)
				currentuser.userlevel = PERM_DEFAULT|PERM_SUICIDE;
			else
				currentuser.userlevel = PERM_DEFAULT|PERM_SUICIDE|PERM_PERSONAL;
		}   //  by livebird:  ◊‘…±÷–π‹¿ÌŒƒºØ»®œﬁ±£¡Ù
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec), usernum);
		//      mail_info(mode,lastword);
		modify_user_mode(OFFLINE);
		kick_user(&uinfo);
		exit(0);
	}
}

getuinfo(FILE* fn)
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n%sµƒ¥˙∫≈     : %s\n", (currentuser.gender=='F'||currentuser.gender=='f')?"À˝":"À˚",currentuser.userid);
	fprintf(fn, "%sµƒÍ«≥∆     : %s\n", (currentuser.gender=='F'||currentuser.gender=='f')?"À˝":"À˚",currentuser.username);
	fprintf(fn, "’Ê µ–’√˚     : %s\n", currentuser.realname);
	fprintf(fn, "æ”◊°◊°÷∑     : %s\n", currentuser.address);
	fprintf(fn, "µÁ◊”” º˛–≈œ‰ : %s\n", currentuser.email);
	fprintf(fn, "’Ê µ E-mail  : %s\n", currentuser.reginfo);
	fprintf(fn, "’ ∫≈◊¢≤·µÿ÷∑ : %s\n", currentuser.ident);
	getdatestring(currentuser.firstlogin,NA);
	fprintf(fn, "’ ∫≈Ω®¡¢»’∆⁄ : %s\n", datestring);
	getdatestring(currentuser.lastlogin,NA);
	fprintf(fn, "◊ÓΩ¸π‚¡Ÿ»’∆⁄ : %s\n", datestring);
	fprintf(fn, "◊ÓΩ¸π‚¡Ÿª˙∆˜ : %s\n", currentuser.lasthost);
	fprintf(fn, "…œ’æ¥Œ ˝     : %d ¥Œ\n", currentuser.numlogins);
	fprintf(fn, "Œƒ’¬ ˝ƒø     : %d\n", currentuser.numposts);
	fprintf(fn, "…œ’æ◊‹ ± ˝   : %d –° ± %d ∑÷÷”\n",
	        currentuser.stay / 3600, (currentuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(currentuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, " π”√’ﬂ»®œﬁ   : %s\n\n", buf);
}

/* add by yiyo º«¬º±ª≤È—ØID∏ˆ»À–≈œ¢ */
gethisinfo(FILE* fn)
{
	int     num;
	char    buf[40];
	fprintf(fn, "\n\n%sµƒ¥˙∫≈     : %s\n", (lookupuser.gender=='F'||lookupuser.gender=='f')?"À˝":"À˚",lookupuser.userid);
	fprintf(fn, "%sµƒÍ«≥∆     : %s\n", (lookupuser.gender=='F'||lookupuser.gender=='f')?"À˝":"À˚",lookupuser.username);
	fprintf(fn, "’Ê µ–’√˚     : %s\n", lookupuser.realname);
	fprintf(fn, "æ”◊°◊°÷∑     : %s\n", lookupuser.address);
	fprintf(fn, "µÁ◊”” º˛–≈œ‰ : %s\n", lookupuser.email);
	fprintf(fn, "’Ê µ E-mail  : %s\n", lookupuser.reginfo);
	fprintf(fn, "’ ∫≈◊¢≤·µÿ÷∑ : %s\n", lookupuser.ident);
	getdatestring(lookupuser.firstlogin,NA);
	fprintf(fn, "’ ∫≈Ω®¡¢»’∆⁄ : %s\n", datestring);
	getdatestring(lookupuser.lastlogin,NA);
	fprintf(fn, "◊ÓΩ¸π‚¡Ÿ»’∆⁄ : %s\n", datestring);
	fprintf(fn, "◊ÓΩ¸π‚¡Ÿª˙∆˜ : %s\n", lookupuser.lasthost);
	fprintf(fn, "…œ’æ¥Œ ˝     : %d ¥Œ\n", lookupuser.numlogins);
	fprintf(fn, "Œƒ’¬ ˝ƒø     : %d\n", lookupuser.numposts);
	fprintf(fn, "…œ’æ◊‹ ± ˝   : %d –° ± %d ∑÷÷”\n",
	        lookupuser.stay / 3600, (lookupuser.stay / 60) % 60);
	strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F012345678");
	for (num = 0; num < 30; num++)
		if (!(lookupuser.userlevel & (1 << num)))
			buf[num] = '-';
	buf[num] = '\0';
	fprintf(fn, " π”√’ﬂ»®œﬁ   : %s\n\n", buf);
}
/* add end */
void mail_info(int mode,char *lastword)
{
	FILE   *fn;
#ifndef TIMECOUNTER

	time_t  now;
#endif

	char    filename[STRLEN];

#ifndef TIMECOUNTER

	now = time(0);
	getdatestring(now,NA);
#else

	getnowdatestring();
#endif

	sprintf(filename, "tmp/suicide.%s", currentuser.userid);
	if ((fn = fopen(filename, "w")) != NULL)
	{
		if(mode)
		{
			fprintf(fn, "\033[1m%s\033[m “—æ≠‘⁄ \033[1m%s\033[m µ«º«πÈ“˛¡À£¨∆‰◊ ¡œ£¨æ¥«Î±£¡Ù...", currentuser.userid ,datestring);
		}
		else
		{
			fprintf(fn, "[1m%s[m “—æ≠‘⁄ [1m%s[m µ«º«◊‘…±¡À£¨∆‰◊ ¡œ£¨æ¥«Î±£¡Ù...", currentuser.userid ,datestring);
		}
		getuinfo(fn);
		fclose(fn);
		if(mode)
			Postfile(filename,"syssecurity3","πÈ“˛Õ®÷™(¡¢º¥…˙–ß)...",2);
		else
			Postfile(filename, "syssecurity3", "µ«º«◊‘…±Õ®÷™(15ÃÏ··…˙–ß)...", 2);
		unlink(filename);
	}
	if ((fn = fopen(filename, "w")) != NULL)
	{
		fprintf(fn, "¥Ûº“∫√,\n\n");
		fprintf(fn,"Œ“ « %s (%s)°£Œ“º∫æ≠æˆ∂®%s\n\n",
		        currentuser.userid, currentuser.username,mode?"‘› ±¿Îø™“ª∂Œ ±º‰¡À°£":"‘⁄ 15 ÃÏ∫Û¿Îø™’‚¿Ô¡À°£");
		//getdatestring(currentuser.firstlogin,NA);
		/*fprintf(fn, "◊‘◊¢≤·÷¡ΩÒ£¨Œ““—æ≠¿¥¥À %d ¥Œ¡À£¨‘⁄’‚◊‹º∆ %d ∑÷÷”µƒÕ¯¬Á…˙√¸÷–£¨\n",
		        currentuser.numlogins,currentuser.stay/60);
		fprintf(fn, "Œ“”÷»Á∫Œª·«·“◊…·∆˙ƒÿ£øµ´ «Œ“µ√◊ﬂ¡À...  µ„µ„µŒµŒ£≠£≠æ°‘⁄Œ“–ƒ÷–£°\n\n");;*/
		fprintf(fn,"%s",lastword);
		if (!mode)
			fprintf(fn,"≈Û”—√«£¨«Î∞— %s ¥”ƒ„√«µƒ∫√”—√˚µ•÷–ƒ√µÙ∞…°£“ÚŒ™Œ“º∫æ≠æˆ∂®¿Îø™’‚¿Ô¡À!\n\n", currentuser.userid);
		else
			fprintf(fn,"≈Û”—√«£¨«Î≤ª“™Œ™Œ“…À–ƒ£¨\n\n");
		sprintf(tmp, "%dÃÏ÷Æ∫Û", seclusionday);
		fprintf(fn, "%sŒ“ª·ªÿ¿¥µƒ°£ ’‰÷ÿ!! ‘Ÿº˚!!\n\n\n", mode?tmp:"ªÚ–Ì”–≥Ø“ª»’");
#ifndef TIMECOUNTER

		getdatestring(now,NA);
#else

		getnowdatestring();
#endif

		fprintf(fn, "%s ”⁄ %s ¡Ù.\n\n", currentuser.userid, datestring);
		fclose(fn);
		Postfile(filename, "notepad", (mode?"πÈ“˛¡Ù—‘...":"µ«º«◊‘…±¡Ù—‘..."), 2);
		unlink(filename);
	}
}
offline()
{
	d_offline(0);
}

fade_out()
{
	d_offline(1);
}
