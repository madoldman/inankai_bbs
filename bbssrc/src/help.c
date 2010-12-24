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
$Id: help.c,v 1.1.1.1 2008-11-23 08:13:23 madoldman Exp $
*/

#include "bbs.h"
void show_help(char* fname)
{
	ansimore(fname, YEA);
	clear();
}

void show_topten(char *fname)
{
	ansimore_topten(fname,YEA);
	clear();
}
/*void
standhelp( mesg )
char    *mesg;
{
    prints("[1;32;44m");
    prints( mesg ) ;
    prints("[m");
}*/

int mainreadhelp()
{
	show_help("help/mainreadhelp");
	return FULLUPDATE;
}

int mailreadhelp()
{
	show_help("help/mailreadhelp");
	return FULLUPDATE;   
}



#ifdef MAILRECYCLE
int recycle_mailreadhelp()
{
	show_help("help/recycle_mailreadhelp");
	return FULLUPDATE;
}
#endif
