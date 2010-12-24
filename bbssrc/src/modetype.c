/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@@rock.b11.ingr.com
                        Guy Vega, gtvega@@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@@csie.ncu.edu.tw
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "modes.h"

char* ModeType(int mode)
{
	switch (mode)
	{
	case IDLE:
		return "";
	case TYPEN:
		return "发公告中";
	case QMD:
		return "设置签名档";
	case NEW:
		return "新站友注册";
	case LOGIN:
		return "进入本站";
	case DIGEST:
		return "浏览精华区";
	case MMENU:
		return "转换阵地中";
	case ADMIN:
		return "管理者选单";
	case SELECT:
		return "选择讨论区";
	case READBRD:
		return "一览众山小";
	case READNEW:
		return "阅览新文章";
	case READING:
		return "品味文章";
	case POSTING:
		return "文豪挥笔";
	case MAIL:
		return "处理信笺";
	case SMAIL:
		return "寄语信鸽";
	case RMAIL:
		return "阅览信笺";
	case TMENU:
		return "聊天选单";
	case LUSERS:
		return "环顾四方";
	case FRIEND:
		return "寻找好友";
	case MONITOR:
		return "探视民情";
	case QUERY:
		return "查询网友";
	case QMEDAL:
		return "查看奖章";
	case TALK:
		return "鹊桥细语";
	case PAGE:
		return "呼叫中...";
	case CHAT1:
		return "国际会议厅";
	case CHAT2:
		return "公共聊天室";
	case IRCCHAT:
		return "会谈IRC";
	case LAUSERS:
		return "探视网友";
	case XMENU:
		return "系统资讯";
	case VOTING:
		return "投票中...";
	case BBSNET:
		return "BBSNET";
	case EDITWELC:
		return "编辑Welc";
	case EDITUFILE:
		return "编辑个人档";
	case EDITSFILE:
		return "编修系统档";
	case ZAP:
		return "订阅讨论区";
	case GAME:
		return "脑力激汤";
	case SYSINFO:
		return "检查系统";
	case FTPSEARCH:
		return "FTP搜索";
	case DICT:
		return "翻查字典";
	case LOCKSCREEN:
		return "萤幕锁定";
	case NOTEPAD:
		return "留言板";
	case GMENU:
		return "工具箱";
	case MSG:
		return "送讯息";
	case USERDEF:
		return "自订参数";
	case EDIT:
		return "修改文章";
	case OFFLINE:
		return "自杀中..";
	case EDITANN:
		return "编修精华";
	case HYTELNET:
		return "Hytelnet";
	case CCUGOPHER:
		return "他站精华";
	case LOOKMSGS:
		return "察看讯息";
	case WFRIEND:
		return "寻人名册";
	case WNOTEPAD:
		return "欲走还留";
	case BBSPAGER:
		return "网路传呼";
	case 10001:
		return "Web 浏览";
	case ADDRESSBOOK:
		return "察看通讯录";
	case M_BLACKJACK:
		return "★黑甲克★";
	case M_XAXB:
		return "★猜数字★";
	case M_DICE:
		return "★西八拉★";
	case M_GP:
		return "金扑克梭哈";
	case M_NINE:
		return "天地九九";
	case WINMINE:
		return "键盘扫雷";
	case WINMINE2:
		return "感应扫雷";
	case M_BINGO:
		return "宾果宾果";
	case FIVE:
		return "决战五子棋";
	case MARKET:
		return "交易市场";
	case PAGE_FIVE:
		return "邀请下棋";
	case CHICK:
		return "电子小鸡";
	case MARY:
		return "超级玛丽";
	case CHICKEN:
		return "星空战斗鸡";
	case GOODWISH:
		return "给朋友祝福";
	case SAID:
		return "找用户文章";
	case SAAR:
		return "标题搜索";
	case QUERYIP:
		return "查询上线IP";
	case FREEIP:
		return "IP地址查询";
	case TT:
		return "打字练习";
	case TETRIS:
		return "俄罗斯方块";
	case LUNAR:
		return "电子万年历";
	case FRIENDTEST:
		return "友谊测试";
	case M_2NDHAND:
		return "跳蚤市场";
	case YBQH:
		return "查邮编区号";
	case RECLUSION:
		return "归隐中...";
	case WORKER:
		return "推箱子";
	case XFIVE:
		return "人机五子棋";
	case R_RMAIL:
		return "邮件回收站";
	case KILLER:
		return "杀人游戏";
        case COMPACTLIST:
                return "过刊区列表";
        case COMPACTREADING:
                return "阅读过刊区"; 
        case KISSCHAT:
                return "KISS聊天室";
	default:
		return "去了哪里!?";
	}
}
