#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

// ������һЩ���ܵĿ��ƣ�������θÿ��ƣ����ʾ��ʹ���������
// �������ǰ���� "//" ���ʾ�����Σ�Ҫ���θù��ܣ������ "//"

//#define AddWaterONLY		/* �����ǿ����ˮվ�Լ�ʹ�õĴ��룬������ */

//#define SHOW_THANKYOU		/* ��ʾԴ�����ṩ����Ϣ */

/*  ע����ز��ִ���� define */
/*  һ��������ע���ⲿ������һЩ����� �������ܿ�������Ƚ϶����ѵ���Ҫ */
#define NEWCOMERREPORT		/* ������·�� newcomers ���Զ����� */
//#define MAILCHECK		/* �ṩ�ʼ�ע�Ṧ�� */
//#define CODE_VALID 		/* ������֤ */
//#define SAVELIVE		/* �����ʺ� ��ֹ�û���ʱ�䲻���߶�����*/
//#define AUTOGETPERM		/* ����ע�ἴ��ȡ����Ȩ�� */
//#define PASSAFTERTHREEDAYS	/* ������·�������� */
#define PASSAFTERONEDAYS	/* �����ǲ���̫���ˣ�һ��Ҳ��� */
//#define MAILCHANGED		/* �޸� e-mail ��Ҫ������ע��ȷ�� */

/* ��Ϸ��ش���� define */
//#define ALLOWGAME		/* ֧����Ϸ, �ṩ��Ǯ��ʾ */
#define FIVEGAME 		/* ������ */

/* ϵͳ��ȫ��ش���� define */
//#define MUDCHECK_BEFORELOGIN 	/* ��½ǰ����ȷ�� */
//#define CHECK_SYSTEM_PASS	/* վ���� ID ��վѯ��ϵͳ���� */
#define SYSOPLOGINPROTECT 	/* SYSOP �ʺŵ�½��ȫ */

/* ĳЩ�����Դ������� define */
#define BOARD_CONTROL		/* �ṩ��Ա�������ù��� Ctrl+Y */
#define BOARD_READDENY		/* �Ķ����޿��ƣ�������� ID ���뿴���Ȩ��*/
//#define KEEP_DELETED_HEADER 	/* ����ɾ�����¼�¼ */
//#define BELL_DELAY_FILTER	/* ���������е��������ʱ���� */
//#define CHECK_LESS60SEC	/* 60 �����ظ� login ʱҪ�󰴼�ȷ�����*/
//#define MARK_X_FLAG		/* ����ˮ���¼��� 'X' ��� */

/* һ�㲻��Ҫ����� define */
#define SHOWMETOFRIEND		/* �����ķ����Ƿ�Ϊ�Է����ѵ���ʾ */
//#define QCLEARNEWFLAG		/* ���ÿ������δ����ǵ��㷨 */
#define BBSD 			/* ʹ�� BBS daemon, ��ʹ�� bbsrf */
#define ALLOWAUTOWRAP		/* �����Զ��Ű湦�� */
//#define ALLOWSWITCHCODE		/* ���� GB �� <==> Big5 �� �л� */
#define USE_NOTEPAD		/* ʹ�����԰� */
#define INTERNET_EMAIL		/* ���� InterNet Mail */
//#define CHKPASSWDFORK           /* ϵͳ�������ʱ���� fork ���� */
#define COLOR_POST_DATE 	/* ����������ɫ */
#define TALK_LOG 		/* �����¼���� */
#define RNDSIGN			/* ����ǩ���� */
#define DOMAIN_NAME		/* ��½ʱ��������IPΪ���� */
//#define NEW_CREATE_BRD	// �µĴ���/�޸��������Ĺ���
// �ʺ��ڶ�ϵͳ������û�
/* ��վ����ϲ������ ����ξ���ΰɣ� ��Ȼ��Ҫ����������Ǻ� */
//#define MSG_CANCLE_BY_CTRL_C 	/* �� ctrl-c ������ѶϢ */
#define LOG_MY_MESG		/* ѶϢ��¼�м�¼�Լ���������ѶϢ */
#define BIGGER_MOVIE 		/* �Ӵ�����ռ� (����) */
//#define ALWAYS_SHOW_BRDNOTE 	/* ÿ�ν��嶼�� show �����廭�� */

/* �������׼��ȡ���Ĺ��ܣ� �㿴�Ű�� */
/*
    ϵͳѰ�˹����ǵ���û�к�����վ֪ͨ����ʱ����һ��Ѱ�˹��ܡ�
    ������������ʾ��Ϣ�����ױ����ԣ����Һ��Ժ����Ҳ�Ҳ����ˡ�
    ����������������ܣ�Ϊ�˱�����Ϣ���뵽��ͬʱ��һ���Ÿ��Է���
    ����һ�����������Ҳ���Ƕ�����ˣ���Ϊд�ű������һ���ܺ�
    ��Ѱ�˹��ܡ� :) �����Ұ���ȱʡΪȡ���ˡ�
    �������������Ҫ������ܣ����Լ��򿪰ɣ��Ǻǡ�
*/
#define CHK_FRIEND_BOOK		/* �趨ϵͳѰ������ */

#ifdef AddWaterONLY
#define AUTOGETPERM		/* ����ע�ἴ��ȡ����Ȩ�� */
#define SYSOPLOGINPROTECT 	/* SYSOP �ʺŵ�½��ȫ */
//#undef DOMAIN_NAME		/* ��½ʱ������������ */
#endif


#define MAILRECYCLE  /*   �ʼ����չ���*/
/* Efan: ʹ�ý���	*/
#define ALLOW_MEDALS

#endif
/* _FUNCTIONS_H_ */
