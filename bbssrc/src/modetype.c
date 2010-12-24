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
		return "��������";
	case QMD:
		return "����ǩ����";
	case NEW:
		return "��վ��ע��";
	case LOGIN:
		return "���뱾վ";
	case DIGEST:
		return "���������";
	case MMENU:
		return "ת�������";
	case ADMIN:
		return "������ѡ��";
	case SELECT:
		return "ѡ��������";
	case READBRD:
		return "һ����ɽС";
	case READNEW:
		return "����������";
	case READING:
		return "Ʒζ����";
	case POSTING:
		return "�ĺ��ӱ�";
	case MAIL:
		return "�����ż�";
	case SMAIL:
		return "�����Ÿ�";
	case RMAIL:
		return "�����ż�";
	case TMENU:
		return "����ѡ��";
	case LUSERS:
		return "�����ķ�";
	case FRIEND:
		return "Ѱ�Һ���";
	case MONITOR:
		return "̽������";
	case QUERY:
		return "��ѯ����";
	case QMEDAL:
		return "�鿴����";
	case TALK:
		return "ȵ��ϸ��";
	case PAGE:
		return "������...";
	case CHAT1:
		return "���ʻ�����";
	case CHAT2:
		return "����������";
	case IRCCHAT:
		return "��̸IRC";
	case LAUSERS:
		return "̽������";
	case XMENU:
		return "ϵͳ��Ѷ";
	case VOTING:
		return "ͶƱ��...";
	case BBSNET:
		return "BBSNET";
	case EDITWELC:
		return "�༭Welc";
	case EDITUFILE:
		return "�༭���˵�";
	case EDITSFILE:
		return "����ϵͳ��";
	case ZAP:
		return "����������";
	case GAME:
		return "��������";
	case SYSINFO:
		return "���ϵͳ";
	case FTPSEARCH:
		return "FTP����";
	case DICT:
		return "�����ֵ�";
	case LOCKSCREEN:
		return "өĻ����";
	case NOTEPAD:
		return "���԰�";
	case GMENU:
		return "������";
	case MSG:
		return "��ѶϢ";
	case USERDEF:
		return "�Զ�����";
	case EDIT:
		return "�޸�����";
	case OFFLINE:
		return "��ɱ��..";
	case EDITANN:
		return "���޾���";
	case HYTELNET:
		return "Hytelnet";
	case CCUGOPHER:
		return "��վ����";
	case LOOKMSGS:
		return "�쿴ѶϢ";
	case WFRIEND:
		return "Ѱ������";
	case WNOTEPAD:
		return "���߻���";
	case BBSPAGER:
		return "��·����";
	case 10001:
		return "Web ���";
	case ADDRESSBOOK:
		return "�쿴ͨѶ¼";
	case M_BLACKJACK:
		return "��ڼ׿ˡ�";
	case M_XAXB:
		return "������֡�";
	case M_DICE:
		return "����������";
	case M_GP:
		return "���˿����";
	case M_NINE:
		return "��ؾž�";
	case WINMINE:
		return "����ɨ��";
	case WINMINE2:
		return "��Ӧɨ��";
	case M_BINGO:
		return "��������";
	case FIVE:
		return "��ս������";
	case MARKET:
		return "�����г�";
	case PAGE_FIVE:
		return "��������";
	case CHICK:
		return "����С��";
	case MARY:
		return "��������";
	case CHICKEN:
		return "�ǿ�ս����";
	case GOODWISH:
		return "������ף��";
	case SAID:
		return "���û�����";
	case SAAR:
		return "��������";
	case QUERYIP:
		return "��ѯ����IP";
	case FREEIP:
		return "IP��ַ��ѯ";
	case TT:
		return "������ϰ";
	case TETRIS:
		return "����˹����";
	case LUNAR:
		return "����������";
	case FRIENDTEST:
		return "�������";
	case M_2NDHAND:
		return "�����г�";
	case YBQH:
		return "���ʱ�����";
	case RECLUSION:
		return "������...";
	case WORKER:
		return "������";
	case XFIVE:
		return "�˻�������";
	case R_RMAIL:
		return "�ʼ�����վ";
	case KILLER:
		return "ɱ����Ϸ";
        case COMPACTLIST:
                return "�������б�";
        case COMPACTREADING:
                return "�Ķ�������"; 
        case KISSCHAT:
                return "KISS������";
	default:
		return "ȥ������!?";
	}
}
