#include "bbs.h"

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PINK 5
#define CYAN 6
#define WHITE 7

#define TOTALNUM 20
#define FRIENDTOP ".FRIENDTOP"
#define BADLIST ".FRIENDBADLIST"

#define PATHLEN 250
#define true YEA
#define false NA

struct usertype
{
	char userid[IDLEN + 2];
	int tried;
	int got;
	time_t create;
}
*users;
int userst = 0;

struct questype
{
	char topic[80];
	char quest[5][40];
	int value[5];
}
*quests;
int questst = 0;

typedef char statinfo[35];
statinfo *stats;
int statst = 0;

//struct userec *lookuser;

void sethome(char *s, char *id)
{
	sprintf(s, "home/%c/%s/.FRIENDTEST", toupper(id[0]), id);
}

void sethometop(char *s, char *id)
{
	sprintf(s, "home/%c/%s/"FRIENDTOP, toupper(id[0]), id);
}

void sethomestat(char *s, char *id)
{
	sprintf(s, "home/%c/%s/.FRIENDSTAT", toupper(id[0]), id);
}

void setfcolor(int i, int j)
{
	prints("\033[%d;%dm", i + 30, j);
}

void setbcolor(int i)
{
	prints("\033[%d;%dm", i + 40);
}

void resetcolor()
{
	prints("\033[27;40;0m");
}

void load_users(char *filename)
{
	int fd;
	struct flock ldata;

	if (userst)
		free(users);
	userst = 0;
	if ((fd = open(filename, O_RDONLY, 0644)) != -1)
	{
		ldata.l_type = F_RDLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			read(fd, &userst, sizeof(userst));
			users = (struct usertype *) malloc(sizeof(struct usertype) * (userst + 1));
			read(fd, users, sizeof(struct usertype) * userst);

			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

void save_users(char *filename)
{
	int fd;
	struct flock ldata;

	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) != -1)
	{
		ldata.l_type = F_WRLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			write(fd, &userst, sizeof(userst));
			write(fd, users, sizeof(struct usertype) * userst);

			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

void load_quests(char *filename)
{
	int fd;
	struct flock ldata;

	if (questst)
		free(quests);
	questst = 0;
	if ((fd = open(filename, O_RDONLY, 0644)) != -1)
	{
		ldata.l_type = F_RDLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			read(fd, &questst, sizeof(questst));
			quests = (struct questype *) malloc(sizeof(struct questype) * TOTALNUM);
			read(fd, quests, sizeof(struct questype) * questst);

			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

void save_quests(char *filename)
{
	int fd;
	struct flock ldata;

	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) != -1)
	{
		ldata.l_type = F_WRLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			write(fd, &questst, sizeof(questst));
			write(fd, quests, sizeof(struct questype) * questst);

			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

void check_stats(char *filename)
{
	int fd, i, saveagain = 0;
	struct flock ldata;
	struct stat st;

	if ((fd = open(filename, O_RDONLY, 0644)) != -1)
	{
		ldata.l_type = F_RDLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			read(fd, &i, sizeof(i));
			if (i != 0x8080)
			{
				if (statst)
					free(stats);
				saveagain = 1;
				lseek(fd, 0, SEEK_SET);
				fstat(fd, &st);
				statst = st.st_size / 20;
				stats = malloc(statst * sizeof(statinfo));
				for (i = 0; i < statst; i++)
				{
					read(fd, stats[i], 20);
					strcpy(stats[i] + 21, "unknown");
				}

				ldata.l_type = F_UNLCK;
				fcntl(fd, F_SETLKW, &ldata);
			}
		}
		close(fd);
	}
	if (saveagain)
	{
		if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) != -1)
		{
			ldata.l_type = F_WRLCK;
			ldata.l_whence = 0;
			ldata.l_len = 0;
			ldata.l_start = 0;
			if (fcntl(fd, F_SETLKW, &ldata) != -1)
			{
				i = 0x8080;
				write(fd, &i, sizeof(i));
				write(fd, stats, statst * sizeof(statinfo));

				ldata.l_type = F_UNLCK;
				fcntl(fd, F_SETLKW, &ldata);
			}
			close(fd);
		}
	}
}

void save_stats(char *filename, statinfo a)
{
	int fd, i;
	struct flock ldata;

	check_stats(filename);
	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) != -1)
	{
		ldata.l_type = F_WRLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			lseek(fd, 0, SEEK_SET);
			i = 0x8080;
			write(fd, &i, sizeof(i));
			lseek(fd, 0, SEEK_END);
			write(fd, a, sizeof(statinfo));

			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

void load_stats(char *filename)
{
	int fd;
	struct flock ldata;
	struct stat st;
	int i;

	check_stats(filename);
	if (statst)
		free(stats);
	statst = 0;
	if ((fd = open(filename, O_RDONLY, 0644)) != -1)
	{
		ldata.l_type = F_RDLCK;
		ldata.l_whence = 0;
		ldata.l_len = 0;
		ldata.l_start = 0;
		if (fcntl(fd, F_SETLKW, &ldata) != -1)
		{
			fstat(fd, &st);
			statst = (st.st_size - 4) / sizeof(statinfo);
			stats = malloc(statst * sizeof(statinfo));
			read(fd, &i, sizeof(i));
			read(fd, stats, statst * sizeof(statinfo));

			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

void sort_users()
{
	int i, j;
	struct usertype tmp;

	for (i = 0; i < userst; i++)
		for (j = i + 1; j < userst; j++)
			if (users[i].tried < users[j].tried)
			{
				memcpy(&tmp, &users[i], sizeof(tmp));
				memcpy(&users[i], &users[j], sizeof(tmp));
				memcpy(&users[j], &tmp, sizeof(tmp));
			}
}

void init()
{}

void done()
{
	/*
	    if (userst)
	        free(users);
	    if (questst)
	        free(quests);
	    if (statst)
	        free(stats);
	*/
}

int usermenu()
{
	int i, tuid;
	char buf[IDLEN + 2], direct[PATHLEN];
	struct stat st;

	clear();
	load_users(FRIENDTOP);
	sort_users();
	move(2, 20);
	setfcolor(WHITE, 0);
	prints("��ӭ���������������(");
	setfcolor(RED, 1);
	prints("FRIENDTEST");
	setfcolor(WHITE, 0);
	prints(")���԰�0.2");
	move(3, 40);
	prints("����: ");
	setfcolor(RED, 0);
	prints("bad@smth.org");
	move(5, 13);
	setfcolor(WHITE, 1);
	prints("===��������ָ������===");
	move(6, 15);
	setfcolor(RED, 1);
	prints("�q���������������r");
	for (i = 0; i < 10; i++)
	{
		move(7 + i, 15);
		setfcolor(RED, 1);
		prints("��");
		if (i >= userst)
		{
			move(7 + i, 20 + 7);
			setfcolor(GREEN, 1);
			prints("--��--");
		}
		else
		{
			move(7 + i, 23 - strlen(users[i].userid) / 2 + 7);
			setfcolor(GREEN, 1);
			prints(users[i].userid);
		}
		move(7 + i, 31 + 14);
		setfcolor(RED, 1);
		prints("��");
	}
	move(17, 15);
	setfcolor(RED, 1);
	prints("�t���������������s");

	load_users(FRIENDTOP);
	move(5, 53);
	setfcolor(WHITE, 1);
	prints("===���¼����û��б�===");
	move(6, 55);
	setfcolor(RED, 1);
	prints("�q���������������r");
	for (i = 0; i < 10; i++)
	{
		move(7 + i, 55 + 7 + 7);
		setfcolor(RED, 1);
		prints("��");
		if (userst - i - 1 < 0)
		{
			move(7 + i, 60 + 7 + 7 + 7);
			setfcolor(GREEN, 1);
			prints("--��--");
		}
		else
		{
			move(7 + i, 63 - strlen(users[userst - i - 1].userid) / 2 + 7 + 7 + 7);
			setfcolor(GREEN, 1);
			prints(users[userst - i - 1].userid);
		}
		move(7 + i, 71 + 14 + 7 + 7);
		setfcolor(RED, 1);
		prints("��");
	}
	move(17, 55);
	setfcolor(RED, 1);
	prints("�t���������������s");
	setfcolor(WHITE, 0);

	move(19, 22);
	prints("����[");
	setfcolor(GREEN, 1);
	prints("a");
	setfcolor(WHITE, 0);
	prints("] ");
	prints("ͳ�ƽ��[");
	setfcolor(GREEN, 1);
	prints("s");
	setfcolor(WHITE, 0);
	prints("] ");
	prints("������[");
	setfcolor(GREEN, 1);
	prints("m");
	setfcolor(WHITE, 0);
	prints("] ");
	prints("ע���б�[");
	setfcolor(GREEN, 1);
	prints("l");
	setfcolor(WHITE, 0);
	prints("] ");
	if (HAS_PERM(PERM_SYSOP|PERM_CHATCLOAK))
	{
		prints("����Ա[");
		setfcolor(GREEN, 1);
		prints("d");
		setfcolor(WHITE, 0);
		prints("] ");
	}
	move(20, 22);
	prints("Ҫ�����˵���,����������id");
	//    setfcolor(WHITE,1);
	while (1)
	{
		move(22, 20);
		clrtoeol();
		getdata(22, 26, "��ѡ��:", buf, IDLEN + 1, true, true);
		if (toupper(buf[0]) == 'A' && !buf[1])
			return 1;
		if (toupper(buf[0]) == 'S' && !buf[1])
			return 3;
		if (toupper(buf[0]) == 'M' && !buf[1])
			return 4;
		if (toupper(buf[0]) == 'L' && !buf[1])
			return 5;
		if (HAS_PERM(PERM_SYSOP|PERM_CHATCLOAK) && toupper(buf[0]) == 'D' && !buf[1])
		{
			return 6;
		}
		else if (buf[0])
		{
			tuid = getuser(buf);

			if (tuid)
			{
				sethome(direct, lookupuser.userid);
				if (stat(direct, &st) != -1)
					return 2;
				else
				{
					move(23, 20);
					clrtoeol();
					prints("ʹ����%s��δ������Ŀ!", lookupuser.userid);
				}
			}
			else
			{
				move(23, 20);
				clrtoeol();
				prints("�����ʹ����id!");
			}
		}
		else
			return 0;
	}
}

int possible_high()
{
	int i, j, k = 0, l;

	for (i = 0; i < questst; i++)
	{
		l = -10;
		for (j = 0; j < 5; j++)
		{
			if (!quests[i].quest[j][0])
				break;
			if (quests[i].value[j] > l)
				l = quests[i].value[j];
		}
		k += l;
	}
	return k;
}

void new_friendtest_reset()
{
	resetcolor();
	clear();
	move(0, 0);
	setfcolor(YELLOW, 1);
	setbcolor(BLUE);
	prints("   FRIENDTEST      �½��û� %s                                                  ", currentuser.userid);
	resetcolor();
	prints("\n");
}

void new_friendtest()
{
	char direct[PATHLEN];
	int i, j, k;
	struct stat st;
	char ans[3], buf[122], buf2[10];
	int empty = 1;

	new_friendtest_reset();

	load_users(BADLIST);
	for (i = 0; i < userst; i++)
		if (!strcmp(users[i].userid, currentuser.userid) && users[i].tried & 1)
		{
			prints("\n���Ѿ�������˳���Ȩ��\n");
			pressreturn();
			done();
			return;
		}

	sethome(direct, currentuser.userid);
	if (stat(direct, &st) != -1)
	{
		empty = 0;
		getdata(1, 0, "�Ѿ���������ʺţ��Ƿ����? [y/N] ", ans, 2, true, true);
		if (toupper(ans[0]) != 'Y')
			return;
	}
	getdata(1, 0, "����������[1--20]", buf, 3, true, true);
	questst = atoi(buf);
	if (questst < 1 || questst > TOTALNUM)
	{
		questst = 0;
		return;
	}
	quests = (struct questype *) malloc(sizeof(struct questype) * questst);
	for (i = 0; i < questst; i++)
	{
again:
		new_friendtest_reset();
		prints("��%d��(��%d��)\n", i + 1, questst);
		getdata(2, 0, "��������Ŀ(40��������): ", quests[i].topic, 80, true, true);
		if (!quests[i].topic[0])
		{
			new_friendtest_reset();
			getdata(1, 0, "�˳������ʺ�? [Y-�˳�/N-����/E-������������]: ", ans, 2, true, true);
			if (toupper(ans[0]) == 'Y')
				return;
			if (toupper(ans[0]) == 'E' && i != 0)
			{
				questst = i;
				break;
			}
			goto again;
		}
		for (j = 0; j < 5; j++)
			quests[i].quest[j][0] = 0;
		do
		{
			move(3, 0);
			clrtoeol();
			getdata(3, 0, "������ѡ�����[1--5]: ", buf, 2, true, true);
			j = atoi(buf);
		}
		while (j < 1 || j > 5);
		for (k = 0; k < j; k++)
		{
			do
			{
				move(4 + k * 2, 0);
				clrtoeol();
				sprintf(buf, "�������%d��ѡ������:", k + 1);
				getdata(4 + k * 2, 0, buf, quests[i].quest[k], 40, true, true);
			}
			while (!quests[i].quest[k][0]);
			do
			{
				move(5 + k * 2, 0);
				clrtoeol();
				sprintf(buf, "�������%d��ѡ���ֵ[-10--10]:", k + 1);
				getdata(5 + k * 2, 0, buf, buf2, 4, true, true);
				quests[i].value[k] = atoi(buf2);
			}
			while (quests[i].value[k] < -10 || quests[i].value[k] > 10);
		}
	}
	new_friendtest_reset();
	move(1, 0);
	prints("��Ĳ��Թ���%d���Ĳ��Կ��ܵõ�����߷���%d", questst, possible_high());
	getdata(2, 0, "���������Ƿ���ȷ��ѡ���ȡ����ѡ���ǽ���[Y/n]", ans, 2, true, true);
	if (toupper(ans[0]) == 'N')
		return;
	save_quests(direct);
	load_users(FRIENDTOP);
	j = 1;
	for (i = 0; i < userst; i++)
		if (!strcmp(users[i].userid, currentuser.userid))
		{
			users[i].tried = 0;
			users[i].got = 0;
			users[i].create = time(0);
			j = 0;
			break;
		}
	if (j)
	{
		if (!userst)
			users = (struct usertype *) malloc(sizeof(struct usertype) * (userst + 1));
		strcpy(users[userst].userid, currentuser.userid);
		users[userst].tried = 0;
		users[userst].got = 0;
		users[userst].create = time(0);
		userst++;
	}
	save_users(FRIENDTOP);
	sethometop(direct, currentuser.userid);
	unlink(direct);
	sethomestat(direct, currentuser.userid);
	unlink(direct);
	move(3, 0);
	prints("��ϲ�㣬���Ѿ��������ʺţ��Ͽ�������ĺ������μӰ�");
	pressreturn();
}

void do_test_reset()
{
	resetcolor();
	clear();
	move(0, 0);
	setfcolor(YELLOW, 1);
	setbcolor(BLUE);
	prints("   FRIENDTEST      ���꾺�� %s <--> %s                                           ", currentuser.userid, lookupuser.userid);
	resetcolor();
	prints("\n");
}

void do_test()
{
	char direct[PATHLEN];
	struct stat st;
	int i, j, k, l;
	char ans[3], buf[122];
	statinfo answer;
	time_t span;

	do_test_reset();

	sethome(direct, lookupuser.userid);
	if (stat(direct, &st) == -1)
	{
		move(1, 0);
		prints("���ʺŲ�����!");
		pressreturn();
		return;
	}
	load_quests(direct);
	sethometop(direct, lookupuser.userid);
	load_users(direct);
	sort_users();
	move(2, 0);
	prints("��ӭ���� %s ���������\n", lookupuser.userid);
	prints("��������߿��ܵ÷�Ϊ%d\n", possible_high());
	if (userst)
	{
		prints("�Ѿ��� %d �˲μӹ�����\n", userst);
		prints("������߷� %d ,  ��ͷ� %d\n", users[0].tried, users[userst - 1].tried);
		prints("\n=======TOP 10========\n");
		for (i = 0; i < 10; i++)
			if (i < userst)
				prints(" %-12s %-3d\n", users[i].userid, users[i].tried);
		getdata(22, 0, "��d��ʾ��ϸ���: ", ans, 2, true, true);
		if (toupper(ans[0]) == 'D')
		{
			do_test_reset();
			move(1, 0);
			prints("��ϸ���Խ��\n");
			move(3, 0);
			prints(" �ʺ�         ����      ���Դ���");
			j = 4;
			for (i = 0; i < userst; i++)
			{
				move(j, 0);
				prints(" %-12s %-3d        %-3d", users[i].userid, users[i].tried, users[i].got);
				if (j >= 20 && i < userst - 1)
				{
					getdata(22, 0, "��\033[32;1mq\033[m�˳�:", ans, 2, true, true);
					if (toupper(ans[0]) == 'Q')
						break;
					do_test_reset();
					move(1, 0);
					prints("��ϸ���Խ��\n");
					move(3, 0);
					prints(" �ʺ�         ����      ���Դ���");
					j = 3;
				}
				j++;
			}
			pressreturn();
			do_test_reset();
		}
	}
	else
	{
		pressreturn();
		do_test_reset();
	}
	for (i = 0; i < userst; i++)
		if (!strcmp(users[i].userid, currentuser.userid))
		{
			span = time(0) - users[i].create;
			if (span < 3600)
			{
				move(2, 0);
				prints("����һСʱ�ڸ����� %s ���������\n", lookupuser.userid);
				prints("����΢��Ϣһ����ٽ�����\n");
				pressreturn();
				return;
			}
		}

	k = 0;
	for (i = 0; i < questst; i++)
	{
doitagain:
		do_test_reset();
		move(1, 0);
		prints("��%d��(��%d��)\n\n", i + 1, questst);
		prints("%s\n", quests[i].topic);
		for (j = 0; j < 5; j++)
		{
			if (!quests[i].quest[j][0])
				break;
			prints("%d) %s\n", j + 1, quests[i].quest[j]);
		}
		sprintf(buf, "ѡ��(��Ϊ�˳�)[1--%d]:", j);
		do
		{
			move(5 + j, 0);
			clrtoeol();
			getdata(5 + j, 0, buf, ans, 2, true, true);
			l = atoi(ans);
			if (!l)
			{
				do_test_reset();
				getdata(1, 0, "�Ƿ��˳�����?[y/N]", ans, 2, true, true);
				if (toupper(ans[0]) == 'Y')
					return;
				goto doitagain;
			}
		}
		while (l < 1 || l > j);
		answer[i] = l;
		k += quests[i].value[l - 1];
	}
	load_users(direct);
	j = 1;
	for (i = 0; i < userst; i++)
		if (!strcmp(users[i].userid, currentuser.userid))
		{
			if (k > users[i].tried)
				users[i].tried = k;
			users[i].got++;
			users[i].create = time(0);
			j = 0;
			break;
		}
	if (j)
	{
		if (!userst)
			users = (struct usertype *) malloc(sizeof(struct usertype) * (userst + 1));
		strcpy(users[userst].userid, currentuser.userid);
		users[userst].tried = k;
		users[userst].got = 1;
		users[userst].create = time(0);
		userst++;
	}
	save_users(direct);

	if (j)
	{
		load_users(FRIENDTOP);
		for (i = 0; i < userst; i++)
			if (!strcmp(users[i].userid, lookupuser.userid))
			{
				users[i].tried++;
				break;
			}
		save_users(FRIENDTOP);
	}
	sethomestat(direct, lookupuser.userid);
	strcpy(answer + 21, currentuser.userid);
	save_stats(direct, answer);

	do_test_reset();
	move(2, 0);
	prints("���Ѿ������� %s ���������\n", lookupuser.userid);
	prints("��������߿��ܵ÷�Ϊ%d\n", possible_high());
	prints("��ĵ÷�Ϊ%d\n", k);
	pressreturn();
}

void show_stat_reset()
{
	resetcolor();
	clear();
	move(0, 0);
	setfcolor(YELLOW, 1);
	setbcolor(BLUE);
	prints("   FRIENDTEST      ͳ�ƽ�� %s                                                 ", currentuser.userid);
	resetcolor();
	prints("\n");
}

void show_stat()
{
	char direct[PATHLEN];
	struct stat st;
	int i, j, k, l, m;
	char ans[3];

	do_test_reset();

	sethome(direct, currentuser.userid);
	if (stat(direct, &st) == -1)
	{
		move(1, 0);
		prints("�㻹û�н����ʺ�!");
		pressreturn();
		return;
	}
	load_quests(direct);
	sethometop(direct, currentuser.userid);
	load_users(direct);
	sort_users();
	sethomestat(direct, currentuser.userid);
	load_stats(direct);

	move(2, 0);
	j = 0;
	for (i = 0; i < userst; i++)
		j += users[i].tried;
	if (userst)
		j /= userst;
	prints("ͳ����Ϣ:\n�� %d ��������Ĳ���\nƽ����: %d\n\n���س�����ʼ��ϸ����", userst, j);
	pressreturn();

	for (i = 0; i < questst; i++)
	{
		do_test_reset();
		move(1, 0);
		prints("��%d��(��%d��)\n\n", i + 1, questst);
		prints("%s\n", quests[i].topic);
		for (j = 0; j < 5; j++)
			if (!quests[i].quest[j][0])
				break;
		for (k = 0; k < j; k++)
		{
			m = 0;
			for (l = 0; l < statst; l++)
				if (stats[l][i] == k + 1)
					m++;
			prints("%d) %s (��ֵ:%d  ѡ���˴�:%d)\n", k + 1, quests[i].quest[k], quests[i].value[k], m);
		}
		getdata(10, 0, "��q�˳�:", ans, 2, true, true);
		if (toupper(ans[0]) == 'Q')
			break;
	}
}

void admin_reset()
{
	resetcolor();
	clear();
	move(0, 0);
	setfcolor(YELLOW, 1);
	setbcolor(BLUE);
	prints("   FRIENDTEST      ����ѡ�� %s                                                 ", currentuser.userid);
	resetcolor();
	prints("\n");
}

void admin_stat()
{
	char direct[PATHLEN];
	struct stat st;
	int i, j, k, l, m;
	char ans[3], buf[122], buf2[10];

	sethome(direct, currentuser.userid);
	if (stat(direct, &st) == -1)
	{
		move(1, 0);
		prints("�㻹û�н����ʺ�!");
		pressreturn();
		return;
	}
	load_quests(direct);
	sethometop(direct, currentuser.userid);
	load_users(direct);
	sort_users();

	while (1)
	{
		admin_reset();

		move(2, 0);
		j = 0;
		for (i = 0; i < userst; i++)
			j += users[i].tried;
		if (userst)
			j /= userst;
		prints("ͳ����Ϣ:\n�� %d ��������Ĳ���\nƽ����: %d", userst, j);

		getdata(7, 0, "ѡ��[q-�˳�,d-ɾ��,e-�޸�,a-���,r-��������,s-�쿴,l-�б�] ", ans, 2, true, true);
		switch (toupper(ans[0]))
		{
		case 'Q':
			return;
		case 'L':
			sethomestat(direct, currentuser.userid);
			load_stats(direct);
			admin_reset();
			move(1, 0);
			prints("�û�            ���б�\n");
			prints("��            ");
			for (j = 0; j < questst; j++)
			{
				l = 0;
				for (k = 0; k < 5; k++)
				{
					if (!quests[j].quest[k][0])
						break;
					if (quests[j].value[k] > quests[j].value[l])
						l = k;
				}
				prints("%d ", l + 1);
			}
			k = 3;
			for (i = 0; i < statst; i++)
			{
				move(k, 0);
				prints("%-16s", stats[i] + 21);
				for (j = 0; j < questst; j++)
					prints("%d ", stats[i][j]);
				k++;
				if (k > 20 && i < statst - 1)
				{
					getdata(22, 0, "��\033[32;1mq\033[m�˳�:", ans, 2, true, true);
					if (toupper(ans[0]) == 'Q')
						break;
					admin_reset();
					move(1, 0);
					prints("�û�            ���б�\n");
					prints("��            ");
					for (j = 0; j < questst; j++)
					{
						l = 0;
						for (k = 0; k < 5; k++)
						{
							if (!quests[j].quest[k][0])
								break;
							if (quests[j].value[k] > quests[j].value[l])
								l = k;
						}
						prints("%d ", l + 1);
					}
					k = 3;
				}
			}
			pressreturn();
			break;
		case 'D':
			sprintf(buf, "����ɾ����(1--%d, allȫɾ):", questst);
			do
			{
				move(8, 0);
				clrtoeol();
				getdata(8, 0, buf, buf2, 4, true, true);
				i = atoi(buf2);
			}
			while ((i < 1 || i > questst) && strcasecmp(buf2, "all"));
			if (!strcasecmp(buf2, "all"))
			{
				free(quests);
				questst = 0;
			}
			else
			{
				i--;
				for (j = i; j < questst - 1; j++)
					memcpy(&quests[j], &quests[j + 1], sizeof(quests[j]));
				questst--;
				if (!questst)
					free(quests);
			}
			sethome(direct, currentuser.userid);
			if (!questst)
			{
				unlink(direct);
				return;
			}
			else
				save_quests(direct);
			break;
		case 'A':
			i = questst;
			if (i >= TOTALNUM)
			{
				move(8, 0);
				prints("��Ŀ���ܳ���20��!\n");
				pressreturn();
				break;
			}
againa:
			new_friendtest_reset();
			prints("��%d��(��%d��)\n", i + 1, questst + 1);
			getdata(2, 0, "��������Ŀ(40��������): ", quests[i].topic, 80, true, true);
			if (!quests[i].topic[0])
			{
				new_friendtest_reset();
				getdata(1, 0, "ȡ��? [Y-�˳�/N-����]: ", ans, 2, true, true);
				if (toupper(ans[0]) == 'Y')
					break;
				goto againa;
			}
			for (j = 0; j < 5; j++)
				quests[i].quest[j][0] = 0;
			do
			{
				move(3, 0);
				clrtoeol();
				getdata(3, 0, "������ѡ�����[1--5]: ", buf, 2, true, true);
				j = atoi(buf);
			}
			while (j < 1 || j > 5);
			for (k = 0; k < j; k++)
			{
				do
				{
					move(4 + k * 2, 0);
					clrtoeol();
					sprintf(buf, "�������%d��ѡ������:", k + 1);
					getdata(4 + k * 2, 0, buf, quests[i].quest[k], 40, true, true);
				}
				while (!quests[i].quest[k][0]);
				do
				{
					move(5 + k * 2, 0);
					clrtoeol();
					sprintf(buf, "�������%d��ѡ���ֵ[-10--10]:", k + 1);
					getdata(5 + k * 2, 0, buf, buf2, 4, true, true);
					quests[i].value[k] = atoi(buf2);
				}
				while (quests[i].value[k] < -10 || quests[i].value[k] > 10);
			}
			questst++;
			sethome(direct, currentuser.userid);
			save_quests(direct);
			break;

		case 'E':
			sprintf(buf, "����༭��(1--%d):", questst);
			do
			{
				move(8, 0);
				clrtoeol();
				getdata(8, 0, buf, buf2, 4, true, true);
				i = atoi(buf2);
			}
			while ((i < 1 || i > questst));
			i--;
againb:
			new_friendtest_reset();
			prints("��%d��(��%d��)\n", i + 1, questst);
			getdata(2, 0, "��������Ŀ(40��������): ", quests[i].topic, 80, true, false);
			if (!quests[i].topic[0])
			{
				new_friendtest_reset();
				getdata(1, 0, "ȡ��? [Y-�˳�/N-����]: ", ans, 2, true, true);
				if (toupper(ans[0]) == 'Y')
					break;
				goto againb;
			}
			for (k = 0; k < 5; k++)
				if (!quests[i].quest[k][0])
					break;
			do
			{
				move(3, 0);
				clrtoeol();
				sprintf(buf, "%d", k);
				getdata(3, 0, "������ѡ�����[1--5]: ", buf, 2, true, false);
				j = atoi(buf);
			}
			while (j < 1 || j > 5);
			if (j < 5)
				quests[i].quest[j][0] = 0;
			for (k = 0; k < j; k++)
			{
				do
				{
					move(4 + k * 2, 0);
					clrtoeol();
					sprintf(buf, "�������%d��ѡ������:", k + 1);
					getdata(4 + k * 2, 0, buf, quests[i].quest[k], 40, true, false);
				}
				while (!quests[i].quest[k][0]);
				do
				{
					move(5 + k * 2, 0);
					clrtoeol();
					sprintf(buf, "�������%d��ѡ���ֵ[-10--10]:", k + 1);
					sprintf(buf2, "%d", quests[i].value[k]);
					getdata(5 + k * 2, 0, buf, buf2, 4, true, false);
					quests[i].value[k] = atoi(buf2);
				}
				while (quests[i].value[k] < -10 || quests[i].value[k] > 10);
			}
			sethome(direct, currentuser.userid);
			save_quests(direct);
			break;
		case 'R':
			getdata(8, 0, "һ������֮�����а񽫱�Ĩȥ��ȷ����(y/N)", ans, 2, true, true);
			if (toupper(ans[0]) != 'Y')
				break;
			load_users(FRIENDTOP);
			j = 1;
			for (i = 0; i < userst; i++)
				if (!strcmp(users[i].userid, currentuser.userid))
				{
					users[i].tried = 0;
					users[i].got = 0;
					users[i].create = time(0);
					j = 0;
					break;
				}
			if (j)
			{
				if (!userst)
					users = (struct usertype *) malloc(sizeof(struct usertype) * (userst + 1));
				strcpy(users[userst].userid, currentuser.userid);
				users[userst].tried = 0;
				users[userst].got = 0;
				users[userst].create = time(0);
				userst++;
			}
			save_users(FRIENDTOP);
			sethometop(direct, currentuser.userid);
			unlink(direct);
			sethomestat(direct, currentuser.userid);
			unlink(direct);
			move(10, 0);
			prints("��Ŀ�Ѿ�������!\n");
			pressreturn();
			break;
		case 'S':
			for (i = 0; i < questst; i++)
			{
				do_test_reset();
				move(1, 0);
				prints("��%d��(��%d��)\n\n", i + 1, questst);
				prints("%s\n", quests[i].topic);
				for (j = 0; j < 5; j++)
					if (!quests[i].quest[j][0])
						break;
				for (k = 0; k < j; k++)
				{
					m = 0;
					for (l = 0; l < statst; l++)
						if (stats[l][i] == k + 1)
							m++;
					prints("%d) %s (��ֵ:%d  ѡ���˴�:%d)\n", k + 1, quests[i].quest[k], quests[i].value[k], m);
				}
				getdata(10, 0, "��q�˳�:", ans, 2, true, true);
				if (toupper(ans[0]) == 'Q')
					break;
			}
			break;
		}
	}
}

void sys_reset()
{
	resetcolor();
	clear();
	move(0, 0);
	setfcolor(YELLOW, 1);
	setbcolor(BLUE);
	prints("   FRIENDTEST      �û��б� %s                                                 ", currentuser.userid);
	resetcolor();
	prints("\n");
}

void sys_list()
{
	char ans[3];
	int i, k;

	sys_reset();
	load_users(FRIENDTOP);
	move(1, 0);
	prints("�û�            ��������\n");
	k = 2;
	for (i = 0; i < userst; i++)
	{
		move(k, 0);
		prints("%-16s %d", users[i].userid, users[i].tried);
		k++;
		if (k > 20 && i < userst - 1)
		{
			getdata(22, 0, "��\033[32;1mq\033[m�˳�:", ans, 2, true, true);
			if (toupper(ans[0]) == 'Q')
				break;
			sys_reset();
			move(1, 0);
			prints("�û�            ��������\n");
			k = 2;
		}
	}
	pressreturn();
}

int admin_menu()
{
	char ans[3];
	int i, j, k, tuid;
	char direct[PATHLEN], buf[IDLEN + 2];

	if (!HAS_PERM(PERM_SYSOP|PERM_CHATCLOAK))
		return;
	resetcolor();
	clear();
	move(0, 0);
	setfcolor(YELLOW, 1);
	setbcolor(BLUE);
	prints("   FRIENDTEST      ����ģʽ %s                                                 ", currentuser.userid);
	resetcolor();
	prints("\n");

	getdata(3, 0, "ѡ��(0-ɾ��,1-��ֹ����,2-������,3-��ֹ����,4-��������,5-�б�,����-�뿪) ", ans, 2, true, true);
	if (ans[0] >= '0' && ans[0] <= '4')
	{
		getdata(5, 0, "������id:", buf, IDLEN + 1, true, true);
		tuid = getuser(buf);
		if (!tuid)
		{
			move(7, 0);
			clrtoeol();
			prints("�����ʹ����id!");
			return;
		}
	}
	{
		switch (ans[0])
		{
		case '0':
			sethome(direct, lookupuser.userid);
			unlink(direct);
			sethometop(direct, lookupuser.userid);
			unlink(direct);
			sethomestat(direct, lookupuser.userid);
			unlink(direct);
			move(7, 0);
			prints("ɾ���ɹ�");
			break;
		case '1':
			load_users(BADLIST);
			j = 1;
			for (i = 0; i < userst; i++)
			{
				if (!strcmp(users[i].userid, lookupuser.userid))
				{
					j = 0;
					if (users[i].tried & 1)
						j = 2;
					users[i].tried |= 1;
					break;
				}
			}
			if (j == 1)
			{
				if (!userst)
					users = (struct usertype *) malloc(sizeof(struct usertype) * (userst + 1));
				strcpy(users[userst].userid, lookupuser.userid);
				users[userst].tried = 1;
				users[userst].got = 0;
				users[userst].create = time(0);
				userst++;
			}
			save_users(BADLIST);
			move(7, 0);
			if (j == 2)
				prints("���û�û�п���Ȩ��");
			else
				prints("����ɹ�");
			break;
		case '2':
			load_users(BADLIST);
			j = 1;
			for (i = 0; i < userst; i++)
			{
				if (!strcmp(users[i].userid, lookupuser.userid))
				{
					j = 0;
					if (!(users[i].tried & 1))
						j = 1;
					users[i].tried &= ~1;
					if (users[i].tried == 0)
					{
						for (k = i; k < userst - 1; k++)
							memcpy(&users[k], &users[k + 1], sizeof(users[k]));
						userst--;
					}
					break;
				}
			}
			save_users(BADLIST);
			move(7, 0);
			if (j == 1)
				prints("���û�δ�����");
			else
				prints("����ɹ�");
			break;
		case '3':
			load_users(BADLIST);
			j = 1;
			for (i = 0; i < userst; i++)
			{
				if (!strcmp(users[i].userid, lookupuser.userid))
				{
					j = 0;
					if (users[i].tried & 2)
						j = 2;
					users[i].tried |= 2;
					break;
				}
			}
			if (j == 1)
			{
				if (!userst)
					users = (struct usertype *) malloc(sizeof(struct usertype) * (userst + 1));
				strcpy(users[userst].userid, lookupuser.userid);
				users[userst].tried = 2;
				users[userst].got = 0;
				users[userst].create = time(0);
				userst++;
			}
			save_users(BADLIST);
			move(7, 0);
			if (j == 2)
				prints("���û�û������Ȩ��");
			else
				prints("����ɹ�");
			break;
		case '4':
			load_users(BADLIST);
			j = 1;
			for (i = 0; i < userst; i++)
			{
				if (!strcmp(users[i].userid, lookupuser.userid))
				{
					j = 0;
					if (!(users[i].tried & 2))
						j = 1;
					users[i].tried &= ~2;
					if (users[i].tried == 0)
					{
						for (k = i; k < userst - 1; k++)
							memcpy(&users[k], &users[k + 1], sizeof(users[k]));
						userst--;
					}
					break;
				}
			}
			save_users(BADLIST);
			move(7, 0);
			if (j == 1)
				prints("���û�δ�����");
			else
				prints("����ɹ�");
			break;
		case '5':
			load_users(BADLIST);
			resetcolor();
			clear();
			move(0, 0);
			setfcolor(YELLOW, 1);
			setbcolor(BLUE);
			prints("   FRIENDTEST      ����ģʽ %s                                                 ", currentuser.userid);
			resetcolor();
			move(1, 0);
			prints("�û�            ����  ����\n");
			k = 2;
			for (i = 0; i < userst; i++)
			{
				move(k, 0);
				prints("%-16s %s     %s", users[i].userid, users[i].tried & 1 ? "Y" : "N", users[i].tried & 2 ? "Y" : "N");
				k++;
				if (k > 20 && i < userst - 1)
				{
					getdata(22, 0, "��\033[32;1mq\033[m�˳�:", ans, 2, true, true);
					if (toupper(ans[0]) == 'Q')
						break;
					resetcolor();
					clear();
					move(0, 0);
					setfcolor(YELLOW, 1);
					setbcolor(BLUE);
					prints("   FRIENDTEST      ����ģʽ %s                                                 ", currentuser.userid);
					resetcolor();
					prints("\n");
					move(1, 0);
					prints("�û�            ����  ����\n");
					k = 2;
				}
			}
			break;
		}
	}
	pressreturn();
}

int friend_main()
{
	int i;

	init();
	load_users(BADLIST);
	for (i = 0; i < userst; i++)
		if (!strcmp(users[i].userid, currentuser.userid) && users[i].tried & 2)
		{
			prints("\n���Ѿ�������˸���ϷȨ��\n");
			pressreturn();
			done();
			return;
		}

	modify_user_mode(FRIENDTEST);
	while (i = usermenu())
	{
		switch (i)
		{
		case 1:
			new_friendtest();
			break;
		case 2:
			do_test();
			break;
		case 3:
			show_stat();
			break;
		case 4:
			admin_stat();
			break;
		case 5:
			sys_list();
			break;
		case 6:
			admin_menu();
			break;
		}
	}

	done();
}
