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
$Id: comm_lists.c,v 1.5 2009-12-24 03:38:36 madoldman Exp $
*/
#include <dlfcn.h>
#include "bbs.h"
#define SC_BUFSIZE              20480
#define SC_KEYSIZE              256
#define SC_CMDSIZE              256
#define sysconf_ptr( offset )   (&sysconf_buf[ offset ]);

#ifndef DLM
#undef  ALLOWGAME
#endif

struct smenuitem
{
	int     line, col, level;
	char   *name, *desc, *arg;
	int     (*fptr) ();
}
*menuitem;

struct sdefine
{
	char   *key, *str;
	int     val;
}
*sysvar;

char   *sysconf_buf;
int     sysconf_menu, sysconf_key, sysconf_len;
int     domenu();
int     Announce(), Personal(),Boards(), EGroup(), Info(), Goodbye();
int     New(), Post(), GoodBrds(),Read(), Select(), Quick_Select(), Users(), Welcome();
int     setcalltime();
int     show_allmsgs(),show_clearmsgs(),x_lockscreen(), x_showuser();
int     Conditions(), x_cloak(), t_users(), x_info(), x_fillform(), x_vote();
int     x_results(), ent_bnet(), ent_bnet1(),ent_bnet2(), a_edits(), x_edits();
int     shownotepad(), x_userdefine(), x_csh(), help_edits();
int	showannounce();
int     m_new(), m_read(), m_send(), g_send();
int     ov_send(), s_msg(), mailall(), offline();
int     clear_all_new_flag(), AbstractMsg(), SearchMsg();
int	forceresolve;
int	mail_unread();//bluetent
int 	game_killer();
int     get_mail_back();
int     t_lookipusers();//by brew
int     renewtopten();//by loveni
int	find_auth_mail();//by loveni
int 	find_mail_user();//by loveni
int 	edit_mylover(); //by loveni
int 	life666();//by loveni
int	renewweb();
int 	ReleaseEmail();//解除邮箱绑定
int     QueryUserstat();//查询用户全站排名
int		DelPCorpus();//删除用户文集
int		ChangePCName();//用户文集改名
int	syncuserdata();//同步用户数据
//int generate_uuid();//生成邀请码
int user_switchcase(); //改变用户名大小写

#ifdef MAILRECYCLE
int  Rmail();
#endif


#ifdef ALLOWGAME
int	ent_winmine();
#endif

#ifdef INTERNET_EMAIL
int	m_internet();
#endif

int     t_users(), t_friends(), t_list(), t_monitor();
#ifdef CHK_FRIEND_BOOK
int     wait_friend();
#endif
int     t_query(), t_talk(), t_pager(), t_friend(), t_reject(), x_cloak();
int     ent_chat();
int	AddPCorpus(); // deardragon 个人文集
int	sendgoodwish();
#ifdef DLM
int 	exec_mbem();
#endif
int	add_medal ();
int     un_Reclusion();
#ifndef WITHOUT_ADMIN_TOOLS
int	kick_user(), m_vote();
#ifndef DLM
int	x_level(), x_denylevel(), m_info(), look_m_info();
int	d_user(), m_register();
int     d_board(), m_editbrd(), m_mclean(), m_newbrd(), m_newzone(), d_zone();
int 	m_ordainBM(), m_retireBM();
int     m_ordainZM(), m_retireZM();
int	setsystempasswd();
int     refs_mail();
#endif
#endif

#ifdef ALLOWSWITCHCODE
int	switch_code();
#endif

int     wall();
int	friend_wall();
/*Add By Excellent */

int show_user_record();  // add by nicky
int search_all_id();
int search_all_tit();

int x_dict();  //add by yiyo 中英文字典
int free_ip();//add by yiyo 免费IP地址查询
int x_tt();//add by yiyo 打字练习
int tetris();//add by yiyo 俄罗斯方块
int x_worker();
int winmine();
int winmine2();
int x_five();
//int x_calc();
int printtoday();//add by yiyo 电子万年历
int friend_main();//add by yiyo 友谊测试
int sec_hand();//add by yiyo 跳蚤市场
int ybqh();//add by yiyo 邮编区号查询
int fade_out();//add by yiyo 归隐田园
int ip_user();//add by yiyo IP上线用户查询
int edit_deny_user();//add by yiyo 拒收用户邮件
int addressbook();//add by bluetent 留言簿
int x_excearchie();//add by bluetent FTP搜索
int server_info();//add by bluetent 服务器运行信息
int no_count_post();//add by bluetent 显示不算文章数的板面
extern int setreadonly();//add by bluetent 系统只读切换
extern int keywords();//add by bluetent 编辑敏感关键字
int FavDigest();//add by bluetent 精华区收藏
int refs_mail();//add by thunder   系统自动发送拒信
extern int inetmail();//add by macana 开通用户互联网邮件功能

int Hotboards(),Cmdboards(),Newboards();/*thunder*/
int boardrule();/*loveni*/

typedef struct
{
	char   *name;
	/*	int     (*fptr) ();*/
	void    (*fptr);
	int     type;
}
MENU;

MENU    currcmd;

MENU    sysconf_cmdlist[] = {
                                {"domenu", domenu, 0},
                                {"EGroups", EGroup, 0},
                                {"BoardsAll", Boards, 0},
                                {"BoardsGood", GoodBrds,0},
                                {"BoardsNew", New, 0},
                                {"LeaveBBS", Goodbye, 0},
                                {"Announce", Announce, 0},
                                {"Personal", Personal, 0},
                                {"SelectBoard", Select, 0},
                                {"SelBoard", Quick_Select, 0},
                                {"ReadBoard", Read, 0},
                                {"PostArticle", Post, 0},
                                {"SetAlarm", setcalltime, 0},
                                {"MailAll", mailall, 0},
                                {"LockScreen", x_lockscreen, 0},
                                {"ShowUser", x_showuser, 0},
                                {"OffLine", offline, 0},
                                {"ReadNewMail", m_new, 0},
                                {"Killer", game_killer, 0},
								{"ReleaseEmail",ReleaseEmail,0},//解除邮箱绑定
								{"QueryUserstat", QueryUserstat, 0},//查询全站排名
								{"DelPCorpus", DelPCorpus, 0},//删除个人文集
								{"ChangePCName", ChangePCName, 0}, //个人文集改名
								{"SyncUserData", syncuserdata, 0}, //同步用户数据
//								{"GenUUID", 	generate_uuid,0},//generate uuid
								{"SwitchCase",user_switchcase, 0},

                                /* thunder*/
                                {"HotBoards", Hotboards, 0},//热门讨论区
                                {"NewBoards",Newboards, 0},//新开讨论区
                                {"CommBoards", Cmdboards, 0},//推荐讨论区




#ifdef MAILRECYCLE
                                {"MailRecycle",Rmail,0},
#endif

                                {"GETMAILBACK",get_mail_back,0},
                                {"ReadMail", m_read, 0},
                                {"SendMail", m_send, 0},
                                {"GroupSend", g_send, 0},
                                {"OverrideSend", ov_send, 0},
#ifdef INTERNET_EMAIL
                                {"SendNetMail", m_internet, 0},
#endif
								{"InetMail", inetmail, 0},
                                {"UserDefine", x_userdefine, 0},
                                {"ShowFriends", t_friends, 0},
                                {"ShowLogins", t_users, 0},
                                {"QueryUser", t_query, 0},
#ifdef CHK_FRIEND_BOOK
                                {"WaitFriend", wait_friend, 0},
#endif
                                {"Talk", t_talk, 0},
                                {"SetPager", t_pager, 0},
                                {"SetCloak", x_cloak, 0},
                                {"SendMsg", s_msg, 0},
                                {"ShowMsg", show_allmsgs, 0},
								{"ClearMsg", show_clearmsgs, 0},
                                {"AbstractMsg", AbstractMsg, 0},
                                {"SearchMsg", SearchMsg, 0},
                                {"SetFriends", t_friend, 0},
                                {"SetRejects", t_reject, 0},
								{"SetMyVip",edit_mylover,0},
                                {"FriendWall", friend_wall, 0},
                                {"EnterChat", ent_chat, 0},
                                {"ListLogins", t_list, 0},
                                {"Monitor", t_monitor, 0},
                                {"FillForm", x_fillform, 0},
                                {"Information", x_info, 0},
                                {"EditUFiles", x_edits, 0},
                                {"ShowLicense", Conditions, 0},
                                {"ShowVersion", Info, 0},
                                {"Notepad", shownotepad, 0},
				      			{"ShowAnnounce",showannounce,0},
                                {"DoVote", x_vote, 0},
                                {"VoteResult", x_results, 0},
                                {"ExecBBSNet", ent_bnet, 0},
                                {"ExecBBSNet1", ent_bnet1, 0},
                                {"ExecBBSNet2", ent_bnet2, 0},
                                {"ShowWelcome", Welcome, 0},
                                {"QueryIP", show_user_record,0},
                                {"AuthorSearch",search_all_id,0},
                                {"TitleSearch",search_all_tit,0},
                                {"ExecDict",x_dict,0},
                                {"FreeIP",free_ip,0},
                                {"TTGame",x_tt,0},
                                {"Tetris",tetris,0},
                                {"Worker",x_worker,0},
                                {"Winmine",winmine,0},
                                {"Winmine2",winmine2,0},
                                {"Wuzi",x_five,0},
							 	{"SearchIPUsers",t_lookipusers,0},
                                //	{"Calc",x_calc,0},
                                {"Archie",x_excearchie,0},
                                {"Lunar",printtoday,0},
                                {"FriendTest", friend_main, 0},
                                {"Addressbook",addressbook, 0},//bluetent
                                {"SecondHand", sec_hand,0},
                                {"Ybqh",ybqh,0},
                                {"FadeOut", fade_out,0},
                                {"IpUser", ip_user,0},
                                {"E_deny", edit_deny_user,0},
                                {"AllUsers", Users, 0},
                                {"AddPCorpus",AddPCorpus,0},
                                {"GoodWish", sendgoodwish,0},
                                {"ClrAllNFlg", clear_all_new_flag,0},
                                {"ServerInfo", server_info, 0},
                                {"Junkboard", no_count_post, 0},
                                {"ReadMode", setreadonly, 0},
                                {"Keyword", keywords, 0},
								{"EBoardRule",boardrule,0},
                                {"FavDigest", FavDigest, 0},
#ifdef ALLOWSWITCHCODE
                                {"SwitchCode",switch_code,0},
#endif
#ifdef ALLOWGAME
                                {"WinMine",	ent_winmine,0},

                                {"Gagb",	"@mod:so/game.so#gagb",			1},
                                {"BlackJack",	"@mod:so/game.so#BlackJack",		1},
                                {"X_dice",	"@mod:so/game.so#x_dice",		1},
                                {"P_gp",	"@mod:so/game.so#p_gp",			1},
                                {"IP_nine",	"@mod:so/game.so#p_nine",		1},
                                {"OBingo",	"@mod:so/game.so#bingo",		1},
                                {"Chicken",	"@mod:so/game.so#chicken_main",		1},
                                {"Mary",	"@mod:so/game.so#mary_m",		1},
                                {"Borrow",	"@mod:so/game.so#borrow",		1},
                                {"Payoff",	"@mod:so/game.so#payoff",		1},
                                {"Impawn",	"@mod:so/game.so#popshop",		1},
                                {"Doshopping",	"@mod:so/game.so#doshopping",		1},
                                {"Lending",	"@mod:so/game.so#lending",		1},
                                {"StarChicken", "@mod:so/pip.so#mod_default",		1},
#endif
#ifdef DLM
                                {"RunMBEM",exec_mbem,0},
#endif
                                {"Kick", kick_user, 0},
                                {"OpenVote", m_vote, 0},
#ifndef DLM
                                {"Refsmail",    refs_mail,                              0},
                                {"Setsyspass",	setsystempasswd,			0},
                                {"Register", 	m_register, 				0},
                                {"Info", 	m_info, 				0},
                                {"LookInfo",	look_m_info,				0},//add by yiyo
                                {"Level", 	x_level, 				0},
                                /* Efan: 增加奖章	*/
                                {"Promote",	add_medal,				0},
                                /* livebird : 解除归隐 */
                                {"NReclusion",  un_Reclusion,                           0},
                                {"OrdainBM",	m_ordainBM,				0},
								{"OrdainZM",    m_ordainZM,                             0},
                                {"RetireBM",	m_retireBM,				0},
								{"RetireZM",    m_retireZM,                             0},
                                {"ChangeLevel", x_denylevel, 				0},
                                {"DelUser", 	d_user, 				0},
                                {"NewBoard", 	m_newbrd, 				0},
								{"NewZone",	m_newzone,				0},
                                {"ChangeBrd", 	m_editbrd, 				0},
                                {"BoardDel", 	d_board, 				0},
		        				{"ZoneDel",	d_zone,	    				0},
								{"RenewTop",    renewtopten,                            0},
                                {"MailClean", 	m_mclean, 				0},
                                {"SysFiles", 	a_edits, 				0},
                                {"HelpFiles",	help_edits,				0},
                                {"Wall", 	wall, 					0},
								{"AuthMail",	find_auth_mail,				0},
								{"MailUser",	find_mail_user,				0},//loveni
								{"Life666",	life666,				0},//loveni
								{"RenewWeb",	renewweb,				0},


#else
                                {"Refsmail",    "@mod:so/admintool.so#refs_mail", 	1},
                                {"Setsyspass",	"@mod:so/admintool.so#setsystempasswd",1},
                                {"Register", 	"@mod:so/admintool.so#m_register", 	1},
                                {"Info",		"@mod:so/admintool.so#m_info", 		1},
                                {"LookInfo",	"@mod:so/admintool.so#look_m_info",	1},//add by yiyo
                                {"Level", 		"@mod:so/admintool.so#x_level", 	1},
                                {"Promote",		"@mod:so/admintool.so#add_medal",	1},
                                {"NReclusion",  "@mod:so/admintool.so#un_Reclusion",1},
                                {"OrdainBM",	"@mod:so/admintool.so#m_ordainBM",	1},
								{"OrdainZM",    "@mod:so/admintool.so@m_ordainZM",  1},
                                {"RetireBM",	"@mod:so/admintool.so#m_retireBM",	1},
								{"RetireZM",    "@mod:so/admintool.so#m_retireZM",	1},
                                {"ChangeLevel", "@mod:so/admintool.so#x_denylevel", 1},
                                {"DelUser", 	"@mod:so/admintool.so#d_user", 		1},
                                {"NewBoard", 	"@mod:so/admintool.so#m_newbrd", 	1},
								{"NewZone",		"@mod:so/admintool.so#m_newzone",	1},
                                {"ChangeBrd", 	"@mod:so/admintool.so#m_editbrd", 	1},
                                {"BoardDel", 	"@mod:so/admintool.so#d_board", 	1},
		        				{"ZoneDel",		"@mod:so/admintool.so#d_zone",		1},
								{"RenewTop",    "@mod:so/admintool.so#renewtopten", 1},
                                {"MailClean", 	"@mod:so/admintool.so#m_mclean", 	1},
                                {"SysFiles", 	"@mod:so/admintool.so#a_edits", 	1},
                                {"HelpFiles",	"@mod:so/admintool.so#help_edits",	1},
                                {"Wall", 		"@mod:so/admintool.so#wall", 		1},
								{"AuthMail",	"@mod:so/admintool.so#find_auth_mail",1},
								{"MailUser",	"@mod:so/admintool.so#find_mail_user",1},
								{"Life666",		"@mod:so/admintool.so#life666",		1},
								{"RenewWeb",	"@mod:so/admintool.so#renewweb",	1},
#endif

                                {0, 0, 0}
                            };

#ifdef DLM
int exec_mbem(char *s)
{
	void *hdll;
	int (*func)();
	char *c;
	char buf[80];

	strcpy(buf,s);
	s=strstr(buf,"@mod:");
	if(s)
	{
		c=strstr(s+5,"#");
		if(c)
		{
			*c=0;
			c++;
		}
		hdll=dlopen(s+5,RTLD_LAZY);

		if(hdll)
		{
			if(func=dlsym(hdll,c ? c : "mod_main"))
				func();
			else
				report(dlerror());
			dlclose(hdll);
		}
		else
		{
			report(dlerror());
		}
	}
}
#endif

void encodestr(register char* str)
{
	register char ch, *buf;
	int     n;
	buf = str;
	while ((ch = *str++) != '\0')
	{
		if (*str == ch && str[1] == ch && str[2] == ch)
		{
			n = 4;
			str += 3;
			while (*str == ch && n < 100)
			{
				str++;
				n++;
			}
			*buf++ = '\01';
			*buf++ = ch;
			*buf++ = n;
		}
		else
			*buf++ = ch;
	}
	*buf = '\0';
}

void decodestr(register char* str)
{
	register char ch;
	int     n;
	while ((ch = *str++) != '\0')
		if (ch != '\01')
			outc(ch);
		else if (*str != '\0' && str[1] != '\0')
		{
			ch = *str++;
			n = *str++;
			while (--n >= 0)
				outc(ch);
		}
}

void* sysconf_funcptr(char* func_name, int* type)
{
	int     n = 0;
	char   *str;

	while ((str = sysconf_cmdlist[n].name) != NULL)
	{
		if (strcmp(func_name, str) == 0)
		{
			*type = sysconf_cmdlist[n].type;
			return (sysconf_cmdlist[n].fptr);
		}
		n++;
	}

	*type = -1;
	return NULL;
}

void* sysconf_addstr(char* str)
{
	int     len = sysconf_len;
	char   *buf;
	buf = sysconf_buf + len;
	strcpy(buf, str);
	sysconf_len = len + strlen(str) + 1;
	return buf;
}

char* sysconf_str(char* key)
{
	int     n;
	for (n = 0; n < sysconf_key; n++)
		if (strcmp(key, sysvar[n].key) == 0)
			return (sysvar[n].str);
	return NULL;
}

int sysconf_eval(char* key)
{
	int     n;
	for (n = 0; n < sysconf_key; n++)
		if (strcmp(key, sysvar[n].key) == 0)
			return (sysvar[n].val);
	if (*key < '0' || *key > '9')
	{
		sprintf(genbuf, "sysconf: unknown key: %s.", key);
		report(genbuf);
	}
	return (strtol(key, NULL, 0));
}

void sysconf_addkey(char *key, char* str, int val)
{
	int     num;
	if (sysconf_key < SC_KEYSIZE)
	{
		if (str == NULL)
			str = sysconf_buf;
		else
			str = sysconf_addstr(str);
		num = sysconf_key++;
		sysvar[num].key = sysconf_addstr(key);
		sysvar[num].str = str;
		sysvar[num].val = val;
	}
}

void sysconf_addmenu(FILE* fp, char* key)
{
	struct smenuitem *pm;
	char    buf[256];
	char   *cmd, *arg[5], *ptr;
	int     n;
	sysconf_addkey(key, "menu", sysconf_menu);
	while (fgets(buf, sizeof(buf), fp) != NULL && buf[0] != '%')
	{
		cmd = strtok(buf, " \t\n");
		if (cmd == NULL || *cmd == '#')
		{
			continue;
		}
		arg[0] = arg[1] = arg[2] = arg[3] = arg[4] = "";
		n = 0;
		for (n = 0; n < 5; n++)
		{
			if ((ptr = strtok(NULL, ",\n")) == NULL)
				break;
			while (*ptr == ' ' || *ptr == '\t')
				ptr++;
			if (*ptr == '"')
			{
				arg[n] = ++ptr;
				while (*ptr != '"' && *ptr != '\0')
					ptr++;
				*ptr = '\0';
			}
			else
			{
				arg[n] = ptr;
				while (*ptr != ' ' && *ptr != '\t' && *ptr != '\0')
					ptr++;
				*ptr = '\0';
			}
		}
		pm = &menuitem[sysconf_menu++];
		pm->line = sysconf_eval(arg[0]);
		pm->col = sysconf_eval(arg[1]);
		if (*cmd == '@')
		{
			pm->level = sysconf_eval(arg[2]);
			pm->name = sysconf_addstr(arg[3]);
			pm->desc = sysconf_addstr(arg[4]);
			pm->fptr = sysconf_addstr(cmd + 1);
			pm->arg = pm->name;
		}
		else if (*cmd == '!')
		{
			pm->level = sysconf_eval(arg[2]);
			pm->name = sysconf_addstr(arg[3]);
			pm->desc = sysconf_addstr(arg[4]);
			pm->fptr = sysconf_addstr("domenu");
			pm->arg = sysconf_addstr(cmd + 1);
		}
		else
		{
			pm->level = -2;
			pm->name = sysconf_addstr(cmd);
			pm->desc = sysconf_addstr(arg[2]);
			pm->fptr = (void *) sysconf_buf;
			pm->arg = sysconf_buf;
		}
	}
	pm = &menuitem[sysconf_menu++];
	pm->name = pm->desc = pm->arg = sysconf_buf;
	pm->fptr = (void *) sysconf_buf;
	pm->level = -1;
}

void sysconf_addblock(FILE* fp, char* key)
{
	char    buf[256];
	int     num;
	if (sysconf_key < SC_KEYSIZE)
	{
		num = sysconf_key++;
		sysvar[num].key = sysconf_addstr(key);
		sysvar[num].str = sysconf_buf + sysconf_len;
		sysvar[num].val = -1;
		while (fgets(buf, sizeof(buf), fp) != NULL && buf[0] != '%')
		{
			encodestr(buf);
			strcpy(sysconf_buf + sysconf_len, buf);
			sysconf_len += strlen(buf);
		}
		sysconf_len++;
	}
	else
	{
		while (fgets(buf, sizeof(buf), fp) != NULL && buf[0] != '%')
		{}
	}
}

void parse_sysconf(char* fname)
{
	FILE   *fp;
	char    buf[256];
	char    tmp[256], *ptr;
	char   *key, *str;
	int     val;
	if ((fp = fopen(fname, "r")) == NULL)
	{
		return;
	}
	sysconf_addstr("(null ptr)");
	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		ptr = buf;
		while (*ptr == ' ' || *ptr == '\t')
			ptr++;

		if (*ptr == '%')
		{
			strtok(ptr, " \t\n");
			if (strcmp(ptr, "%menu") == 0)
			{
				str = strtok(NULL, " \t\n");
				if (str != NULL)
					sysconf_addmenu(fp, str);
			}
			else
			{
				sysconf_addblock(fp, ptr + 1);
			}
		}
		else if (*ptr == '#')
		{
			key = strtok(ptr, " \t\"\n");
			str = strtok(NULL, " \t\"\n");
			if (key != NULL && str != NULL &&
			        strcmp(key, "#include") == 0)
			{
				parse_sysconf(str);
			}
		}
		else if (*ptr != '\n')
		{
			key = strtok(ptr, "=#\n");
			str = strtok(NULL, "#\n");
			if (key != NULL & str != NULL)
			{
				strtok(key, " \t");
				while (*str == ' ' || *str == '\t')
					str++;
				if (*str == '"')
				{
					str++;
					strtok(str, "\"");
					val = atoi(str);
					sysconf_addkey(key, str, val);
				}
				else
				{
					val = 0;
					strcpy(tmp, str);
					ptr = strtok(tmp, ", \t");
					while (ptr != NULL)
					{
						val |= sysconf_eval(ptr);
						ptr = strtok(NULL, ", \t");
					}
					sysconf_addkey(key, NULL, val);
				}
			}
			else
			{
				report(ptr);
			}
		}
	}
	fclose(fp);
}

void build_sysconf(char* configfile, char* imgfile)
{
	struct smenuitem *old_menuitem;
	struct sdefine *old_sysvar;
	char   *old_buf;
	int     old_menu, old_key, old_len;
	struct sysheader
	{
		char   *buf;
		int     menu, key, len;
	}
	shead;
	int     fh;
	old_menuitem = menuitem;
	old_menu = sysconf_menu;
	old_sysvar = sysvar;
	old_key = sysconf_key;
	old_buf = sysconf_buf;
	old_len = sysconf_len;
	menuitem = (void *) malloc(SC_CMDSIZE * sizeof(struct smenuitem));
	sysvar = (void *) malloc(SC_KEYSIZE * sizeof(struct sdefine));
	sysconf_buf = (void *) malloc(SC_BUFSIZE);
	sysconf_menu = 0;
	sysconf_key = 0;
	sysconf_len = 0;
	parse_sysconf(configfile);
	if ((fh = open(imgfile, O_WRONLY | O_CREAT, 0644)) > 0)
	{
		ftruncate(fh, 0);
		shead.buf = sysconf_buf;
		shead.menu = sysconf_menu;
		shead.key = sysconf_key;
		shead.len = sysconf_len;
		write(fh, &shead, sizeof(shead));
		write(fh, menuitem, sysconf_menu * sizeof(struct smenuitem));
		write(fh, sysvar, sysconf_key * sizeof(struct sdefine));
		write(fh, sysconf_buf, sysconf_len);
		close(fh);
	}
	free(menuitem);
	free(sysvar);
	free(sysconf_buf);
	menuitem = old_menuitem;
	sysconf_menu = old_menu;
	sysvar = old_sysvar;
	sysconf_key = old_key;
	sysconf_buf = old_buf;
	sysconf_len = old_len;
}

void load_sysconf_image(char* imgfile)
{
	struct sysheader
	{
		char   *buf;
		int     menu, key, len;
	}
	shead;
	struct stat st;
	char   *ptr, *func;
	int     fh, n, diff, x;
	if ((fh = open(imgfile, O_RDONLY)) > 0)
	{
		fstat(fh, &st);
		ptr = malloc(st.st_size);
		read(fh, &shead, sizeof(shead));
		read(fh, ptr, st.st_size);
		close(fh);

		menuitem = (void *) ptr;
		ptr += shead.menu * sizeof(struct smenuitem);
		sysvar = (void *) ptr;
		ptr += shead.key * sizeof(struct sdefine);
		sysconf_buf = (void *) ptr;
		ptr += shead.len;
		sysconf_menu = shead.menu;
		sysconf_key = shead.key;
		sysconf_len = shead.len;
		/*
		        sprintf( genbuf, "buf = %d, %d, %d", menuitem, sysvar, sysconf_buf );
		        report( genbuf );
		        sprintf( genbuf, "%d, %d, %d, %d, %s", shead.buf, shead.len,
		                shead.menu, shead.key, sysconf_buf );
		        report( genbuf );
		*/
		diff = sysconf_buf - shead.buf;
		for (n = 0; n < sysconf_menu; n++)
		{
			menuitem[n].name += diff;
			menuitem[n].desc += diff;
			menuitem[n].arg += diff;
			func = (char *) menuitem[n].fptr;
			menuitem[n].fptr = sysconf_funcptr(func + diff, &x);
		}
		for (n = 0; n < sysconf_key; n++)
		{
			sysvar[n].key += diff;
			sysvar[n].str += diff;
		}
	}
}

void load_sysconf()
{
	if (!dashf("sysconf.img"))
	{
		report("build sysconf.img");
		build_sysconf("etc/sysconf.ini", "sysconf.img");
	}
	load_sysconf_image("sysconf.img");
}

int domenu_screen(struct smenuitem* pm)
{
	char   *str;
	int     line, col, num;
	/*    if(!DEFINE(DEF_NORMALSCR))  */
	clear();
	line = 3;
	col = 0;
	num = 0;
	while (1)
	{
		switch (pm->level)
		{
		case -1:
			return (num);
		case -2:
			if (strcmp(pm->name, "title") == 0)
			{
				firsttitle(pm->desc);
			}
			else if (strcmp(pm->name, "screen") == 0)
			{
				if ((str = sysconf_str(pm->desc)) != NULL)
				{
					move(pm->line, pm->col);
					decodestr(str);
				}
			}
			break;
		default:
			if (pm->line >= 0 && HAS_PERM(pm->level))
			{
				if (pm->line == 0)
				{
					pm->line = line;
					pm->col = col;
				}
				else
				{
					line = pm->line;
					col = pm->col;
				}
				move(line, col);
				prints("  %s", pm->desc);
				line++;
			}
			else
			{
				if (pm->line > 0)
				{
					line = pm->line;
					col = pm->col;
				}
				pm->line = -1;
			}
		}
		num++;
		pm++;
	}
}

int domenu(char* menu_name)
{
	extern int refscreen;
	struct smenuitem *pm;
	int     size, now;
	int     cmd, i;
	if (sysconf_menu <= 0)
	{
		return -1;
	}
	pm = &menuitem[sysconf_eval(menu_name)];
	size = domenu_screen(pm);
	now = 0;
	if (strcmp(menu_name, "TOPMENU") == 0 && chkmail())
	{
		for (i = 0; i < size; i++)
			if (pm[i].line > 0 && pm[i].name[0] == 'M')
				now = i;

	}
	modify_user_mode(MMENU);
	R_monitor();
	while (1)
	{
		//printacbar();
		while (pm[now].level < 0 || !HAS_PERM(pm[now].level))
		{
			now++;
			if (now >= size)
				now = 0;
		}
		move(pm[now].line, pm[now].col);
		prints("◆");
		//		move(pm[now].line, pm[now].col+1);
		move(0,80);
		//cmd = egetch();
		cmd=igetkey();
		move(pm[now].line, pm[now].col);
		prints("  ");
		switch (cmd)
		{
		case EOF:
			if (!refscreen)
			{
				abort_bbs();
			}
			domenu_screen(pm);
			modify_user_mode(MMENU);
			R_monitor();
			break;

		case Ctrl('K'):
						x_lockscreen();
			domenu_screen(pm);
			modify_user_mode(MMENU);
			R_monitor();
			break;//add by yiyo 快速屏幕锁定

		case KEY_RIGHT:
			for (i = 0; i < size; i++)
	{
				if (pm[i].line == pm[now].line && pm[i].level >= 0 &&
				        pm[i].col > pm[now].col && HAS_PERM(pm[i].level))
					break;
			}
			if (i < size)
			{
				now = i;
				break;
			}
		case '\n':
		case '\r':
			if (strcmp(pm[now].arg, "..") == 0)
			{
				return 0;
			}
			if (pm[now].fptr != NULL)
			{
				int     type;

				(void *) sysconf_funcptr(pm[now].name, &type);
#ifdef DLM

				if (type == 1)
				{
					exec_mbem((char *)pm[now].fptr);
				}
				else
#endif

					(*pm[now].fptr) (pm[now].arg);
				if (pm[now].fptr == Select)
				{
					now++;
				}
				domenu_screen(pm);
				modify_user_mode(MMENU);
				R_monitor();
			}
			break;
		case KEY_LEFT:
			for (i = 0; i < size; i++)
			{
				if (pm[i].line == pm[now].line && pm[i].level >= 0 &&
				        pm[i].col < pm[now].col && HAS_PERM(pm[i].level))
					break;
				if (pm[i].fptr == Goodbye)
					break;
			}
			if (i < size)
			{
				now = i;
				break;
			}
			return 0;
		case ' ':
		case KEY_DOWN:
			now++;
			break;
		case KEY_UP:
			now--;
			while (pm[now].level < 0 || !HAS_PERM(pm[now].level))
			{
				if (now > 0)
					now--;
				else
					now = size - 1;
			}
			break;
			// Modified by Flier - 2000.5.12 - Begin
		case KEY_PGUP:
			now = 0;
			break;
		case KEY_PGDN:
			now = size - 1;
			while (pm[now].level < 0 || !HAS_PERM(pm[now].level))
				now--;
			break;
			// Modified by Flier - 2000.5.12 - End

			/* add by bluetent 2003.4.19 快速查看信件 */
		case 'V':
			m_new();
			//			printacbar();
			domenu_screen(pm);
			modify_user_mode(MMENU);
			R_monitor();
			break;
			/* add end */
		case '~':
			if (!HAS_PERM(PERM_SYSOP) && !HAS_PERM (PERM_WELCOME) )
			{
				break;
			}
			free(menuitem);
			report("rebuild sysconf.img");
			build_sysconf("etc/sysconf.ini", "sysconf.img");
			report("reload sysconf.img");
			load_sysconf_image("sysconf.img");
			pm = &menuitem[sysconf_eval(menu_name)];
			ActiveBoard_Init();
			size = domenu_screen(pm);
			modify_user_mode(MMENU);
			R_monitor();
			now = 0;
			fill_shmfile(5, "etc/sysmsg", "ENDLINE1_SHMKEY");//bluetent
			break;
		case '!':	/* youzi leave */
			if (strcmp("TOPMENU", menu_name) == 0)
				break;
			else
				return Goodbye();
		default:
			if (cmd >= 'a' && cmd <= 'z')
				cmd = cmd - 'a' + 'A';
			for (i = 0; i < size; i++)
			{
				if (pm[i].line > 0 && cmd == pm[i].name[0] &&
				        HAS_PERM(pm[i].level))
				{
					now = i;
					break;
				}
			}
		}
	}
}
