/******************************************************
ɱ����Ϸ2003, ����: bad@smth.org  Qian Wenjie
��ˮľ�廪bbsϵͳ������
 
����Ϸ����������������⸴����ֲ
�����޸ĺ���ļ�ͷ��������Ȩ��Ϣ
******************************************************/
/******************************************************
 * ɱ����Ϸ 2003 for FB 2000 ����: TurboZV@UESTC Zhang Wei
 *
 * ���Ѿ���smth��ɱ����Ϸ��ֲ��FB2000ϵͳ�ϣ�
 * ллbad@smthΪ����Ϸ�ĸ���
 * 
 *����Ϸ����������������⸴����ֲ
 *�����޸ĺ���ļ�ͷ��������Ȩ��Ϣ
 *				    2003-5-12
******************************************************/
/* ChangeLog 030515
 *  1) ��ȷ�Ĵ�������ɫ����ʾ
 *  2) ������鹦��(*)
 *  3) ͶƱ���»��߱�ʾ(*)
 *  4) ����һ�����а���(*)
 *  5) �Ծ���Ĺ�������һЩ����(*)
 *  6) ����ֻ����Ctrl+T��̽���� (*)
 * ��*Ϊbad@smth�������޸�
 * */

#define BBSMAIN
#include "select.h"
#include "bbs.h"

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PINK 5
#define CYAN 6
#define WHITE 7

#define MAX_ROOM 30
#define MAX_PEOPLE 100
#define MAX_MSG 2000

#define BBS_PAGESIZE    (t_lines - 4)

void player_drop (int);
int k_getdata(line, col, prompt, buf, len, echo, clearlabel)
int line, col, len, echo, clearlabel;
char *prompt, *buf;
{
	buf[0] = '\0';
	getdata(line, col, prompt, buf, len, echo, clearlabel);
}

void k_setfcolor(int i, int j)
{}

void k_resetcolor()
{}

#define ROOM_LOCKED 01
#define ROOM_SECRET 02
#define ROOM_DENYSPEC 04

struct room_struct
{
	int w;
	int style; /* 0 - chat room 1 - killer room */
	char name[14];
	char title[NAMELEN];
	char creator[IDLEN+2];
	unsigned int level;
	int flag;
	int people;
	int maxpeople;
};
//�Թ���
#define PEOPLE_SPECTATOR 01
//ɱ��
#define PEOPLE_KILLER 02
//
#define PEOPLE_ALIVE 04
//
#define PEOPLE_ROOMOP 010
//����
#define PEOPLE_POLICE 020
//what's this???
#define PEOPLE_TESTED 040
#define PEOPLE_VOTED  0100
#define PEOPLE_HIDE   0200

struct people_struct
{
	int style; // Efan: style == -1 ������
	char id[IDLEN+2];
	char nick[NAMELEN];
	int flag;
	int pid;
	int vote;
	int vnum;
};

#define INROOM_STOP 1
//����
#define INROOM_NIGHT 2
//����
#define INROOM_DAY 3
//ָ֤�׶�
#define INROOM_CHECK 4
//�绤�׶�
#define INROOM_DEFENCE 5
//ͶƱ�׶�
#define INROOM_VOTE 6
//����������
#define INROOM_DARK 7


struct inroom_struct
{
	int w;
	int status;
	int killernum;
	int policenum;
	/* ��δ����Ĵ��� */
	int turn; //�ֵ�ĳ�˷���
	int rotate; //�ڼ���ͶƱ
	int dead; //���ֱ�ɱ����
	int test; //������֤����
	int suicide; //ɱ�ֱ���
	char voted [MAX_PEOPLE];
	int numvoted;
	/**/
	struct people_struct peoples[MAX_PEOPLE];
	char msgs[MAX_MSG][60];
	int msgpid[MAX_MSG];
	int msgi;
};

struct room_struct * rooms;
struct inroom_struct * inrooms;

struct killer_record
{
	int w; //0 - ƽ��ʤ�� 1 - ɱ��ʤ��
	time_t t;
	int peoplet;
	char id[MAX_PEOPLE][IDLEN+2];
	int st[MAX_PEOPLE]; // 0 - ����ƽ�� 1 - ����ƽ�� 2 - ����ɱ�� 3 - ����ɱ�� 4 - �������
};

int myroom, mypos;

int kicked;

void save_result(int w)
{
	int fd;
	struct flock ldata;
	struct killer_record r;
	int i,j;
	char filename[80]="service/.KILLERRESULT";
	r.t = time(0);
	r.w = w;
	r.peoplet = 0;
	j = 0;
	for(i=0;i<MAX_PEOPLE;i++)
		if(inrooms[myroom].peoples[i].style!=-1&&!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR))
		{
			strcpy(r.id[j], inrooms[myroom].peoples[i].id);
			r.st[j] = 4;
			if(!(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER))
			{
				if(inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
					r.st[j] = 0;
				else
					r.st[j] = 1;
			}
			else
			{
				if(inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
					r.st[j] = 2;
				else
					r.st[j] = 3;
			}

			j++;
			r.peoplet++;
		}
	if((fd = open(filename, O_WRONLY|O_CREAT, 0644))!=-1)
	{
		ldata.l_type=F_WRLCK;
		ldata.l_whence=0;
		ldata.l_len=0;
		ldata.l_start=0;
		if(fcntl(fd, F_SETLKW, &ldata)!=-1)
		{
			lseek(fd, 0, SEEK_END);
			write(fd, &r, sizeof(struct killer_record));

			ldata.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &ldata);
		}
		close(fd);
	}
}

/*void load_msgs()
{
    FILE* fp;
    int i;
    char filename[80], buf[80];
    msgst=0;
    sprintf(filename, "home/%c/%s/.INROOMMSG%d", toupper(currentuser.userid[0]), currentuser.userid, uinfo.pid);
    fp = fopen(filename, "r");
    if(fp) {
        while(!feof(fp)) {
            if(fgets(buf, 79, fp)==NULL) break;
            if(buf[0]) {
                if(!strncmp(buf, "�㱻����", 8)) kicked=1;
                if(msgst==200) {
                    msgst--;
                    for(i=0;i<msgst;i++)
                        strcpy(msgs[i],msgs[i+1]);
                }
                strcpy(msgs[msgst],buf);
                msgst++;
            }
        }
        fclose(fp);
    }
}*/

void start_change_inroom()
{
	if(inrooms[myroom].w)
		sleep(0);
	inrooms[myroom].w = 1;
}

void end_change_inroom()
{
	inrooms[myroom].w = 0;
}

struct action
{
	char *verb;                 /* ���� */
	char *part1_msg;            /* ��� */
	char *part2_msg;            /* ���� */
};

struct action party_data[] =
    {
	    {"?", "���ɻ�Ŀ���", ""
	    },
	    {"admire", "��", "�ľ���֮���������Ͻ�ˮ���಻��"},
	    {"agree", "��ȫͬ��", "�Ŀ���"},
	    {"bearhug", "�����ӵ��", ""},
	    {"bless", "ף��", "�����³�"},
	    {"bow", "�Ϲ��Ͼ�����", "�Ϲ�"},
	    {"bye", "����", "�ı�Ӱ����Ȼ���¡����������������������ĸ���:\n\"[31m���վ�����.....[m\""},
	    {"caress", "����ĸ���", ""},
	    {"cat", "��ֻСè���������", "�Ļ���������"},
	    {"cringe", "��", "������ϥ��ҡβ����"},
	    {"cry", "��", "�������"},
	    {"comfort", "���԰�ο", ""},
	    {"clap", "��", "���ҹ���"},
	    {"dance", "����", "������������"},
	    {"dogleg", "��", "����"},
	    {"drivel", "����", "����ˮ"},
	    {"dunno", "�ɴ��۾���������ʣ�", "����˵ʲ���Ҳ���Ү... :("},
	    {"faint", "�ε���", "�Ļ���"},
	    {"fear", "��", "¶�����µı���"},
	    {"fool", "����ע��", "�����׳�....\n������������....�˼����Ļ....\n����̫��ϧ�ˣ�"},
	    {"forgive", "��ȵĶ�", "˵�����ˣ�ԭ������"},
	    {"giggle", "����", "ɵɵ�Ĵ�Ц"},
	    {"grin", "��", "¶��а���Ц��"},
	    {"growl", "��", "��������"},
	    {"hand", "��", "����"},
	    {"hammer", "����ô�ô�����������ۣ���",
	     "ͷ������һ�ã�\n***************\n*  5000000 Pt *\n***************\n      | |      �������\n      | |         �ö������Ӵ\n      |_|"},
	    {"heng", "��������", "һ�ۣ� ����һ�����߸ߵİ�ͷ��������,��мһ�˵�����..."},
	    {"hug", "�����ӵ��", ""},
	    {"idiot", "����س�Ц", "�ĳմ���"},
	    {"kick", "��", "�ߵ���ȥ����"},
	    {"kiss", "����", "������"},
	    {"laugh", "������Ц", ""},
	    {"lovelook", "����", "���֣������ĬĬ���ӡ�Ŀ�����к���ǧ�����飬�������"},
	    {"nod", "��", "��ͷ����"},
	    {"nudge", "�����ⶥ", "�ķʶ���"},
	    {"oh", "��", "˵����Ŷ�����Ӱ�����"},
	    {"pad", "����", "�ļ��"},
	    {"papaya", "������", "��ľ���Դ�"},
	    {"pat", "��������", "��ͷ"},
	    {"pinch", "�����İ�", "š�ĺ���"},
	    {"puke", "����", "�°��°�����˵�¶༸�ξ�ϰ����"},
	    {"punch", "�ݺ�����", "һ��"},
	    {"pure", "��", "¶�������Ц��"},
	    {"qmarry", "��", "�¸ҵĹ�������:\n\"��Ը��޸�����\"\n---���������ɼΰ�"},
	    {"report", "͵͵�ض�", "˵���������Һ��𣿡�"},
	    {"shrug", "���ε���", "�����ʼ��"},
	    {"sigh", "��", "̾��һ����"},
	    {"slap", "žž�İ���", "һ�ٶ���"},
	    {"smooch", "ӵ����", ""},
	    {"snicker", "�ٺٺ�..�Ķ�", "��Ц"},
	    {"sniff", "��", "��֮�Ա�"},
	    {"sorry", "ʹ�����������", "ԭ��"},
	    {"spank", "�ð��ƴ�", "���β�"},
	    {"squeeze", "������ӵ����", ""},
	    {"thank", "��", "��л"},
	    {"tickle", "��ߴ!��ߴ!ɦ", "����"},
	    {"waiting", "����ض�", "˵��ÿ��ÿ�µ�ÿһ�죬ÿ��ÿ���Ҷ������������"},
	    {"wake", "Ŭ����ҡҡ", "��������ߴ�У��������ѣ��������ģ���"},
	    {"wave", "����", "ƴ����ҡ��"},
	    {"welcome", "���һ�ӭ", "�ĵ���"},
	    {"wink", "��", "���ص�գգ�۾�"},
	    {"xixi", "�����ض�", "Ц�˼���"},
	    {"zap", "��", "���Ĺ���"},
	    {"inn", "˫�۱�������ˮ���޹�������", ""},
	    {"mm", "ɫ���еĶ�", "�ʺã�����ü�á�������������ɫ�ǰ�������"},
	    {"disapp", "����û��ͷ����Ϊʲô", "����������������ȫû��Ӧ��û�취��"},
	    {"miss", "��ϵ�����", "�����������������������̫--��������!���಻����?"},
	    {"buypig", "ָ��", "���������ͷ������һ�룬лл����"},
	    {"rascal", "��", "��У������������å����������������������������������"},
	    {"qifu", "С��һ�⣬��", "�޵��������۸��ң����۸��ң�������"},
	    {"wa", "��", "���һ�����������������ۿ����Ү������������������������������"},
	    {"feibang", "ร���������죬", "�������ҽ�������һ�����Ը���ٰ�������"},
	    {"badman", "ָ��", "���Դ�, �����㵸�ĺ�:�� ɱ~~��~~��~~��~~��~~~��"},
	    {NULL, NULL, NULL}
    };

struct action speak_data[] =
    {
	    {"ask", "ѯ��", NULL
	    },
	    {"chant", "����", NULL},
	    {"cheer", "�Ȳ�", NULL},
	    {"chuckle", "��Ц", NULL},
	    {"curse", "����", NULL},
	    {"demand", "Ҫ��", NULL},
	    {"frown", "��ü", NULL},
	    {"groan", "����", NULL},
	    {"grumble", "����ɧ", NULL},
	    {"hum", "������", NULL},
	    {"moan", "��̾", NULL},
	    {"notice", "ע��", NULL},
	    {"order", "����", NULL},
	    {"ponder", "��˼", NULL},
	    {"pout", "������˵", NULL},
	    {"pray", "��", NULL},
	    {"request", "����", NULL},
	    {"shout", "���", NULL},
	    {"sing", "����", NULL},
	    {"smile", "΢Ц", NULL},
	    {"smirk", "��Ц", NULL},
	    {"swear", "����", NULL},
	    {"tease", "��Ц", NULL},
	    {"whimper", "���ʵ�˵", NULL},
	    {"yawn", "��Ƿ����", NULL},
	    {"yell", "��", NULL},
	    {NULL, NULL, NULL}
    };

struct action condition_data[] =
    {
	    {":D", "�ֵĺϲ�£��", NULL
	    },
	    {":)", "�ֵĺϲ�£��", NULL},
	    {":P", "�ֵĺϲ�£��", NULL},
	    {":(", "�ֵĺϲ�£��", NULL},
	    {"applaud", "žžžžžžž....", NULL},
	    {"blush", "��������", NULL},
	    {"cough", "���˼���", NULL},
	    {"faint", "�۵�һ�����ε��ڵ�", NULL},
	    {"happy", "������¶�����Ҹ��ı��飬��ѧ�Ա��˵���ߺ�������", NULL},
	    {"lonely", "һ�������ڷ��������������ϣ��˭�����㡣������", NULL},
	    {"luck", "�ۣ���������", NULL},
	    {"puke", "����ģ������˶�����", NULL},
	    {"shake", "ҡ��ҡͷ", NULL},
	    {"sleep", "Zzzzzzzzzz�������ģ�����˯����", NULL},
	    {"so", "�ͽ���!!", NULL},
	    {"strut", "��ҡ��ڵ���", NULL},
	    {"tongue", "��������ͷ", NULL},
	    {"think", "����ͷ����һ��", NULL},
	    {"wawl", "���춯�صĿ�", NULL},
	    {"goodman", "�ü����޹��ı��鿴�Ŵ��: ��������Ǻ���Ү~~��", NULL},
	    {NULL, NULL, NULL}
    };

void send_to_fellow (int u, char * msg)
{
	int i;
	if (u & PEOPLE_KILLER)
		for (i = 0; i < MAX_PEOPLE; ++i)
		{
			register int flag = inrooms[myroom].peoples[i].flag;
			if (inrooms[myroom].peoples[i].style != -1 &&
			        flag & PEOPLE_ALIVE && flag & PEOPLE_KILLER)
				send_msg (i, msg);
		}
	else if (u & PEOPLE_POLICE)
		for (i = 0; i < MAX_PEOPLE; ++i)
		{
			register int flag = inrooms[myroom].peoples[i].flag;
			if (inrooms[myroom].peoples[i].style != -1 &&
			        flag & PEOPLE_ALIVE && flag & PEOPLE_POLICE)
				send_msg (i, msg);
		}
	else if (u == PEOPLE_SPECTATOR)
		for (i = 0; i < MAX_PEOPLE; ++i)
		{
			register int flag = inrooms[myroom].peoples[i].flag;
			if (inrooms[myroom].peoples[i].style != -1 &&
			        (flag & PEOPLE_SPECTATOR || !(flag & PEOPLE_ALIVE)))
				send_msg (i, msg);
		}
}

int  send_msg(int u, char* msg)
{
	int i, j, k, f;
	char buf[200], buf2[200], buf3[80];

	strcpy(buf, msg);

	for(i=0;i<=6;i++)
	{
		buf3[0]='%';
		buf3[1]=i+48;
		buf3[2]=0;
		while(strstr(buf, buf3)!=NULL)
		{
			strcpy(buf2, buf);
			k=strstr(buf, buf3)-buf;
			buf2[k]=0;
			k+=2;
			sprintf(buf, "%s[3%dm%s", buf2, (i>0)?i:7, buf2+k);
		}
	}

	while(strchr(buf, '\n')!=NULL)
	{
		i = strchr(buf, '\n')-buf;
		buf[i]=0;
		send_msg(u, buf);
		strcpy(buf2, buf+i+1);
		strcpy(buf, buf2);
	}
	while(strlen(buf)>56)
	{
		int maxi=0;
		k=0;
		j = 0;
		f = 0;
		for(i=0;i<strlen(buf);i++)
		{
			if(buf[i]=='')
				f = 1;
			if(f)
			{
				if(isalpha(buf[i]))
					f=0;
				continue;
			}
			if(k==0&&i<=56)
			{
				if(i>maxi)
					maxi = i;
			}
			j++;
			if(k)
				k=0;
			else if(buf[i]<0)
				k=1;
		}
		if(maxi<strlen(buf)&&maxi!=0)
		{
			strcpy(buf2, buf);
			buf[maxi]=0;
			send_msg(u, buf);
			strcpy(buf, buf2+maxi);
		}
		else
			break;
	}
	j=MAX_MSG;
	if(inrooms[myroom].msgs[(MAX_MSG-1+inrooms[myroom].msgi)%MAX_MSG][0]==0)
		for(i=0;i<MAX_MSG;i++)
			if(inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG][0]==0)
			{
				j=(i+inrooms[myroom].msgi)%MAX_MSG;
				break;
			}
	if(j==MAX_MSG)
	{
		strcpy(inrooms[myroom].msgs[inrooms[myroom].msgi], buf);
		if(u==-1)
			inrooms[myroom].msgpid[inrooms[myroom].msgi] = -1;
		else
			inrooms[myroom].msgpid[inrooms[myroom].msgi] = inrooms[myroom].peoples[u].pid;
		inrooms[myroom].msgi = (inrooms[myroom].msgi+1)%MAX_MSG;
	}
	else
	{
		strcpy(inrooms[myroom].msgs[j], buf);
		if(u==-1)
			inrooms[myroom].msgpid[j] = u;
		else
			inrooms[myroom].msgpid[j] = inrooms[myroom].peoples[u].pid;
	}
}

void kill_msg(int u)
{
	int i,j,k;
	char buf[80];
	for(i=0;i<MAX_PEOPLE;i++)
		if(inrooms[myroom].peoples[i].style!=-1)
			if(u==-1||i==u)
			{
				j=kill(inrooms[myroom].peoples[i].pid, SIGUSR1);
				if(j==-1)
				{
					sprintf(buf, "%s������", inrooms[myroom].peoples[i].nick);
					send_msg(-1, buf);
					start_change_inroom();
					inrooms[myroom].peoples[i].style=-1;
					/* Efan: �Ժ���˵~~~ */
					/*
					if ((inrooms[myroom].status == INROOM_DAY || inrooms[myroom].status == INROOM_CHECK || inrooms[myroom].status == INROOM_DEFENCE) && inrooms[myroom].voted[inrooms[myroom].turn] == i) {
					 struct people_struct * ppeop;
					 int turn = inrooms[myroom].turn;
					 turn++;
					 ppeop = inrooms[myroom].peoples;
					 while (turn < inrooms[myroom].numvoted) {
					  int tmp = inrooms[myroom].voted[turn];
					  if (ppeop[turn].style != -1 && ppeop[turn].flag & PEOPLE_ALIVE && !(ppeop[turn].flag & PEOPLE_SPECTATOR))
					   break;
					  turn++;
					 }
					 if (turn >= inrooms[myroom].numvoted) {
					 }
					}
					*/
					/* Efan: ��ʱ��Ϊһ�˵��߼�������Ϸ */
					/*
					if (inrooms[myroom].status != INROOM_STOP && inrooms[myroom].peoples[i].flag & PEOPLE_ALIVE && !(inrooms[myroom].peoples[i].flag & PEOPLE_SPECTATOR)) {
					inrooms[myroom].status = INROOM_STOP;
					send_msg (-1, "\33[31;1m��Ϸ�쳣����\33[m");
					kill_msg (-1);
					}
					*/
					player_drop (i);
					rooms[myroom].people--;
					if(inrooms[myroom].peoples[i].flag&PEOPLE_ROOMOP)
					{
						for(k=0;k<MAX_PEOPLE;k++)
							if(inrooms[myroom].peoples[k].style!=-1&&!(inrooms[myroom].peoples[k].flag&PEOPLE_SPECTATOR))
							{
								inrooms[myroom].peoples[k].flag|=PEOPLE_ROOMOP;
								sprintf(buf, "%s��Ϊ�·���", inrooms[myroom].peoples[k].nick);
								send_msg(-1, buf);
								break;
							}
					}
					end_change_inroom();
					kill_msg (-1);
					i=-1;
				}
			}
}

int add_room(struct room_struct * r)
{
	int i,j;
	for(i=0;i<MAX_ROOM;i++)
		if(rooms[i].style==1)
		{
			if(!strcmp(rooms[i].name, r->name))
				return -1;
			if(!strcmp(rooms[i].creator, currentuser.userid))
				return -1;
		}
	for(i=0;i<MAX_ROOM;i++)
		if(rooms[i].style==-1)
		{
			memcpy(rooms+i, r, sizeof(struct room_struct));
			inrooms[i].status = INROOM_STOP;
			inrooms[i].killernum = 0;
			inrooms[i].msgi = 0;
			inrooms[i].policenum = 0;
			inrooms[i].w = 0;
			for(j=0;j<MAX_MSG;j++)
				inrooms[i].msgs[j][0]=0;
			for(j=0;j<MAX_PEOPLE;j++)
				inrooms[i].peoples[j].style = -1;
			return 0;
		}
	return -1;
}

/*
int del_room(struct room_struct * r)
{
    int i, j;
    for(i=0;i<*roomst;i++)
    if(!strcmp(rooms[i].name, r->name)) {
        rooms[i].name[0]=0;
        break;
    }
    return 0;
}
*/

void clear_room()
{
	int i;
	for(i=0;i<MAX_ROOM;i++)
		if((rooms[i].style!=-1) && (rooms[i].people==0))
			rooms[i].style=-1;
}

int can_see(struct room_struct * r)
{
	if(r->style==-1)
		return 0;
	//    if(r->level&currentuser.userlevel!=r->level) return 0;
	if(r->style!=1)
		return 0;
	if(r->flag&ROOM_SECRET&&!HAS_PERM(PERM_SYSOP))
		return 0;
	return 1;
}

int can_enter(struct room_struct * r)
{
	if(r->style==-1)
		return 0;
	//    if(r->level&currentuser.userlevel!=r->level) return 0;
	if(r->style!=1)
		return 0;
	if(r->flag&ROOM_LOCKED&&!HAS_PERM(PERM_SYSOP))
		return 0;
	return 1;
}

int room_count()
{
	int i,j=0;
	for(i=0;i<MAX_ROOM;i++)
		if(can_see(rooms+i))
			j++;
	return j;
}

int room_get(int w)
{
	int i,j=0;
	for(i=0;i<MAX_ROOM;i++)
	{
		if(can_see(rooms+i))
		{
			if(w==j)
				return i;
			j++;
		}
	}
	return -1;
}

int find_room(char * s)
{
	int i;
	struct room_struct * r2;
	for(i=0;i<MAX_ROOM;i++)
	{
		r2 = rooms+i;
		if(!can_enter(r2))
			continue;
		if(!strcmp(r2->name, s))
			return i;
	}
	return -1;
}

int selected = 0, ipage=0, jpage=0;

int getpeople(int i)
{
	int j, k=0;
	for(j=0;j<MAX_PEOPLE;j++)
	{
		if(inrooms[myroom].peoples[j].style==-1)
			continue;
		if(i==k)
			return j;
		k++;
	}
	return -1;
}

int get_msgt()
{
	int i,j=0,k;
	for(i=0;i<MAX_MSG;i++)
	{
		if(inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG][0]==0)
			break;
		k=inrooms[myroom].msgpid[(i+inrooms[myroom].msgi)%MAX_MSG];
		if(k==-1||k==uinfo.pid)
			j++;
	}
	return j;
}

char * get_msgs(int s)
{
	int i,j=0,k;
	char * ss;
	for(i=0;i<MAX_MSG;i++)
	{
		if(inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG][0]==0)
			break;
		k=inrooms[myroom].msgpid[(i+inrooms[myroom].msgi)%MAX_MSG];
		if(k==-1||k==uinfo.pid)
		{
			if(j==s)
			{
				ss = inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG];
				return ss;
			}
			j++;
		}
	}
	return NULL;
}

void save_msgs(char * s)
{
	FILE* fp;
	int i;
	fp=fopen(s, "w");
	if(fp==NULL)
		return;
	for(i=0;i<get_msgt();i++)
		fprintf(fp, "%s\n", get_msgs(i));
	fclose(fp);
}

void refreshit()
{
	int i, j, me, msgst, k, i0, m_col;
	char buf[30], buf2[30], buf3[30], disp[256];
	for (i = 0; i < t_lines - 1; i++)
	{
		move(i, 0);
		clrtoeol();
	}
	move(0, 0);
	prints
	("[44;33;1m ����:[36m%-12s[33m����:[36m%-40s[33m״̬:[36m%6s[m",
	 rooms[myroom].name, rooms[myroom].title,
	 (inrooms[myroom].status ==
	  INROOM_STOP ? "δ��ʼ" : (inrooms[myroom].status ==
	                            INROOM_NIGHT ? "��ҹ��" : "�����")));
	clrtoeol();
	k_resetcolor();
	k_setfcolor(YELLOW, 1);
	move(1, 0);
	prints("[1;33m");
	prints
	("�q[1;32m���[33m�����������r�q[1;32mѶϢ[33m�������������������������������������������������������r");
	move(t_lines - 2, 0);
	prints
	("�t���������������s�t�����������������������������������������������������������s[m");

	me = mypos;
	msgst = get_msgt();

	for (i = 2; i <= t_lines - 3; i++)
	{
		strcpy(disp, "��");

		// Ҫ��ʾ�û�
		if (ipage + i - 2 >= 0 && ipage + i - 2 < rooms[myroom].people)
		{
			j = getpeople(ipage + i - 2);
			if (j == -1)
				continue;
			if (inrooms[myroom].status != INROOM_STOP)
				if (inrooms[myroom].peoples[j].flag&PEOPLE_KILLER && (!(inrooms[myroom].peoples[j].flag & PEOPLE_HIDE) || inrooms[myroom].peoples[me].flag & PEOPLE_KILLER) &&
				        (inrooms[myroom].peoples[me].flag&PEOPLE_KILLER ||
				         !(inrooms[myroom].peoples[j].flag&PEOPLE_ALIVE)))
				{
					strcat(disp, "[31m*");
				}
				else if (inrooms[myroom].peoples[j].flag & PEOPLE_POLICE &&
				         inrooms[myroom].peoples[me].flag & PEOPLE_POLICE)
					/*
							 inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR)
					*/
					strcat (disp, "\33[36m@");
				else
					strcat(disp, " ");

			if (inrooms[myroom].status != INROOM_STOP
			        && !(inrooms[myroom].peoples[j].flag & PEOPLE_ALIVE)
			        && !(inrooms[myroom].peoples[j].flag & PEOPLE_SPECTATOR))
			{
				strcat(disp, "[34mX");
			}
			else
				if ((inrooms[myroom].peoples[j].flag & PEOPLE_SPECTATOR))
				{
					strcat(disp, "[32mO");
				}
				else if (inrooms[myroom].status == INROOM_VOTE)// ||
					/*
					inrooms[myroom].status == INROOM_NIGHT &&
					(inrooms[myroom].peoples[me].flag & PEOPLE_KILLER
					 || inrooms[myroom].
					 peoples[me].flag & PEOPLE_SPECTATOR))
					 */
					/* Efan: ���첻Ӧ�ÿ�������ͶƱ
					|| inrooms[myroom].
					peoples[me].flag & PEOPLE_POLICE))
					*/
					if ((inrooms[myroom].peoples[j].flag & PEOPLE_ALIVE)
					        && (inrooms[myroom].peoples[j].vote != 0))
					{
						//		    strcat(disp, "[0;33mv[1m");
					}
			m_col = 4;
			if (ipage + i - 2 == selected)
			{
				k_setfcolor(RED, 1);
				strcat(disp, "[31m");
			}
			else
				strcat(disp, "[m");

			if (inrooms[myroom].peoples[j].nick[0] == '\0')
				sprintf(buf, "%-2d%-9.9s", j+1, inrooms[myroom].peoples[j].id);
			else
				sprintf(buf, "%-2d%-9.9s", j+1, inrooms[myroom].peoples[j].nick);
			cutHalf(buf);
			//        buf[12]=0;
			if (inrooms[myroom].status == INROOM_DAY ||
			        inrooms[myroom].status == INROOM_NIGHT &&
			        (inrooms[myroom].peoples[me].flag & PEOPLE_KILLER ||
			         inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR ||
			         inrooms[myroom].peoples[me].flag & PEOPLE_POLICE))
			{
				k = 0;
				for (i0 = 0; i0 < MAX_PEOPLE; i0++)
					if (inrooms[myroom].peoples[i0].style != -1
					        && inrooms[myroom].peoples[i0].vote ==
					        inrooms[myroom].peoples[j].pid)
						k++;
				if (k > 0)
				{
					int j0 = 0;
					if (k >= strlen(buf))
						k = strlen(buf);
					for (i0 = 0; i0 < k; i0++)
					{
						if (j0)
							j0 = 0;
						else if (buf[i0] < 0)
							j0 = 1;
					}
					if (j0 && k < strlen(buf))
						k++;
					strcpy(buf2, buf);
					buf2[k] = 0;
					strcpy(buf3, buf + k);
					sprintf(buf, "[4m%s[m%s%s", buf2,
					        (ipage + i - 2 ==
					         selected) ? "[31;1m" : "", buf3);
				}
			}

			m_col = strlen2(disp);
			while (m_col < 4)
			{
				strcat(disp, " ");
				m_col++;
			}
			strcat(disp, buf);
			if (inrooms[myroom].status == INROOM_CHECK || inrooms[myroom].status == INROOM_DEFENCE || inrooms[myroom].status == INROOM_VOTE)
				if (inrooms[myroom].peoples[j].flag & PEOPLE_ALIVE && !(inrooms[myroom].peoples[j].flag & PEOPLE_SPECTATOR) && inrooms[myroom].peoples[j].flag & PEOPLE_VOTED)
					strcat (disp, "\33[36;1m$");
		}
		m_col = strlen2(disp);
		while (m_col < 16)
		{
			strcat(disp, " ");
			m_col++;
		}
		strcat(disp, "[1;33m����[m");

		// ������ʾ����Ϣ
		if (msgst - 1 - (t_lines - 3 - i) - jpage >= 0)
		{
			char *ss = get_msgs(msgst - 1 - (t_lines - 3 - i) - jpage);
			if (!strcmp(ss, "�㱻����"))
				kicked = 1;
			if (ss)
				strcat(disp, ss);
		}

		m_col = strlen2(disp);
		while (m_col < 78)
		{
			strcat(disp, " ");
			m_col++;
		}
		strcat(disp, "[1;33m��");

		// ��ʾ
		move(i, 0);
		prints(disp);
	}

}


extern int RMSG;

void room_refresh(int signo)
{
	int y,x;
	signal(SIGUSR1, room_refresh);

	if(RMSG)
		return;
	if(rooms[myroom].style!=1)
		kicked = 1;

	getyx(&y, &x);
	refreshit();
	move(y, x);
	refresh();
}

void start_killergame()
{
	int i,j,totalk=0,total=0,totalc=0, me;
	char buf[80];
	me=mypos;
	for(i=0;i<MAX_PEOPLE;i++)
		if(inrooms[myroom].peoples[i].style!=-1)
		{
			inrooms[myroom].peoples[i].flag &= ~PEOPLE_KILLER;
			inrooms[myroom].peoples[i].flag &= ~PEOPLE_POLICE;
			inrooms[myroom].peoples[i].flag &= ~PEOPLE_HIDE;
			inrooms[myroom].peoples[i].vote = 0;
		}
	if (inrooms[myroom].killernum == -1)
		inrooms[myroom].killernum = 1;
	totalk=inrooms[myroom].killernum;
	totalc=inrooms[myroom].policenum;
	for(i=0;i<MAX_PEOPLE;i++)
		if(inrooms[myroom].peoples[i].style!=-1)
			if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR))
				total++;
	if(total<3)
	{
		send_msg(me, "[31;1m����3�˲μӲ��ܿ�ʼ��Ϸ[m");
		end_change_inroom();
		refreshit();
		return;
	}
	if(totalk==0)
		totalk=((double)total/6+0.5);
	/* Efan: ɱ�ֺ;�����Ŀ����������趨��������ǿ��
	    if(totalk>=total/4) totalk=total/4;
	    if(totalc>=total/6) totalc=total/6;
	    if(totalk>total) {
	        send_msg(me, "[31;1m����������Ҫ��Ļ�������,�޷���ʼ��Ϸ[m");
	        end_change_inroom();
	        refreshit();
	        return;
	    }
	*/
	/* Efan: ����Ӧ�ñ�֤������һ����ƽ��~~ */
	if (totalk + totalc > total - 1)
	{
		send_msg (me, "\33[31;1mû��ƽ������ô���������趨ɱ�ֺ;�����Ŀ\33[m");
		end_change_inroom ();
		refreshit ();
		return;
	}
	/* Efan: ҲҪ��֤��һ��ɱ�֡��� */
	if (totalk == 0)
	{
		send_msg (me, "[31;1mû��ɱ����ôɱ����~~~�������趨ɱ����Ŀ[m");
		end_change_inroom ();
		refreshit ();
		return;
	}

	if(totalc==0)
		sprintf(buf, "[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������[m", totalk);
	else
		sprintf(buf, "[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������, %d������[m", totalk, totalc);
	send_msg(-1, buf);
	sprintf (buf, "[31;1m��������� %d(p)-%d(k)-%d(c) .[m", totalc, totalk, total - totalc - totalk);
	send_msg (-1, buf);
	for(i=0;i<totalk;i++)
	{
		do
		{
			j=rand()%MAX_PEOPLE;
		}
		while(inrooms[myroom].peoples[j].style==-1 || inrooms[myroom].peoples[j].flag&PEOPLE_KILLER || inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR);
		inrooms[myroom].peoples[j].flag |= PEOPLE_KILLER;
		send_msg(j, "������һ���޳ܵĻ���");
		send_msg(j,"[31;1m�����ƶ����[mѡ�����");
		if (totalk != 1)
		{
			send_msg(j,"����ļ⵶([31;1mCtrl+S[m)ѡ��Ҫ�к����˰�...");
		}
		else
		{
			send_msg (j, "����ļ⵶([31;1mCtrl+S[m)ѡ������к����˰�...");
			send_msg (j, "[31;1m�˾��ݲ���ɱ�ˡ���[m");
			inrooms[myroom].killernum = -1;
		}
	}
	for(i=0;i<totalc;i++)
	{
		do
		{
			j=rand()%MAX_PEOPLE;
		}
		while(inrooms[myroom].peoples[j].style==-1 || inrooms[myroom].peoples[j].flag&PEOPLE_KILLER || inrooms[myroom].peoples[j].flag&PEOPLE_POLICE || inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR);
		inrooms[myroom].peoples[j].flag |= PEOPLE_POLICE;
		send_msg(j, "������һλ���ٵ����񾯲�");
		send_msg(j, "��[31;1m�����ƶ����[m");
		send_msg(j, "����ľ���([31;1mCtrl+S[m)�鿴�㻳�ɵ���...");
		/*
		if (totalk == 1 && totalc == 1)
			send_msg (j, "����Ҫ��[31;1m��һ�غ�[m��ʼ�ſ���~~~");
			*/
	}
	for(i=0;i<MAX_PEOPLE;i++)
		if(inrooms[myroom].peoples[i].style!=-1)
			if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR))
			{
				inrooms[myroom].peoples[i].flag |= PEOPLE_ALIVE;
				if(!(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER))
					send_msg(i, "����������...");
			}
	inrooms[myroom].test = -1;
	inrooms[myroom].dead = -1;
	inrooms[myroom].status = INROOM_NIGHT;
	inrooms[myroom].suicide = -1;
	end_change_inroom();
	kill_msg(-1);
}

#define menust 8
int do_com_menu()
{
	char menus[menust][15] =
	    { "0-����", "1-�˳���Ϸ", "2-������", "3-����б�", "4-�Ļ���",
	      "5-���÷���", "6-�����", "7-��ʼ��Ϸ"
	    };
	int i, j, k, sel = 0, ch, max = 0, me;
	char buf[80], disp[128];
	if (inrooms[myroom].status != INROOM_STOP)
		strcpy(menus[7], "7-������Ϸ");
	do
	{
		k_resetcolor();
		move(t_lines - 1, 0);
		clrtoeol();
		j = mypos;
		disp[0] = '\0';
		for (i = 0; i < menust; i++)
			if (inrooms[myroom].peoples[j].flag & PEOPLE_ROOMOP || i <= 3)
			{
				if (i == sel)
				{
					strcat(disp, "[1;31m");
					strcat(disp, menus[i]);
					strcat(disp, "[m");
				}
				else
					strcat(disp, menus[i]);
				strcat(disp, " ");
				if (i >= max - 1)
					max = i + 1;
			}
		prints(disp);


		ch=igetkey();
		if(kicked)
			return 0;
		switch(ch)
		{
		case KEY_UP:
			jpage += t_lines/2;
			if (jpage > get_msgt () - 1)
				jpage = get_msgt () - 1;
			refreshit ();
			break;
		case KEY_LEFT:
			sel--;
			if(sel<0)
				sel=max-1;
			break;
		case KEY_DOWN:
			jpage -= t_lines/2;
			if (jpage <= 0)
				jpage = 0;
			refreshit ();
			break;
		case KEY_RIGHT:
			sel++;
			if(sel>=max)
				sel=0;
			break;
		case '\n':
		case '\r':
			switch(sel)
			{
			case 0:
				return 0;
			case 1:
				me=mypos;
				if(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE&&!(inrooms[myroom].peoples[me].flag&PEOPLE_ROOMOP)&&inrooms[myroom].status!=INROOM_STOP)
				{
					send_msg(me, "�㻹����Ϸ,�����˳�");
					refreshit();
					return 0;
				}
				return 1;
			case 2:
				if (inrooms[myroom].status != INROOM_STOP)
				{
					send_msg (me, "��Ϸ�У����ܸ�ID");
					refreshit ();
					return 0;
				}
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "����������:", buf, 13, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					k=1;
					for(j=0;j<strlen(buf);j++)
						k=k&&(isprint2(buf[j]));
					k=k&&(buf[0]!=' ');
					k=k&&(buf[strlen(buf)-1]!=' ');
					if(!k)
					{
						move(t_lines-1,0);
						k_resetcolor();
						clrtoeol();
						prints(" ���ֲ����Ϲ淶");
						refresh();
						sleep(1);
						return 0;
					}
					me=mypos;
					j=0;
					for(i=0;i<MAX_PEOPLE;i++)
						if(inrooms[myroom].peoples[i].style!=-1)
							if(i!=me)
								if(!strcmp(buf,inrooms[myroom].peoples[i].id) || !strcmp(buf,inrooms[myroom].peoples[i].nick))
									j=1;
					if(j)
					{
						move(t_lines-1,0);
						k_resetcolor();
						clrtoeol();
						prints(" �����������������");
						refresh();
						sleep(1);
						return 0;
					}
					start_change_inroom();
					me=mypos;
					strcpy(inrooms[myroom].peoples[me].nick, buf);
					end_change_inroom();
					kill_msg(-1);
				}
				return 0;
			case 3:
				me=mypos;
				for(i=0;i<MAX_PEOPLE;i++)
					if(inrooms[myroom].peoples[i].style!=-1)
					{
						sprintf(buf, "%-12s  %s", inrooms[myroom].peoples[i].id, inrooms[myroom].peoples[i].nick);
						send_msg(me, buf);
					}
				refreshit();
				return 0;
			case 4:
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "�����뻰��:", buf, 31, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					start_change_inroom();
					strcpy(rooms[myroom].title, buf);
					end_change_inroom();
					kill_msg(-1);
				}
				return 0;
			case 5:
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "�����뷿���������:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					if(i>0&&i<=100)
					{
						rooms[myroom].maxpeople = i;
						sprintf(buf, "�������÷����������Ϊ%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "����Ϊ���ط���? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_SECRET;
					else
						rooms[myroom].flag&=~ROOM_SECRET;
					sprintf(buf, "�������÷���Ϊ%s", (buf[0]=='Y')?"����":"������");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "����Ϊ��������? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_LOCKED;
					else
						rooms[myroom].flag&=~ROOM_LOCKED;
					sprintf(buf, "�������÷���Ϊ%s", (buf[0]=='Y')?"����":"������");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "����Ϊ�ܾ��Թ��ߵķ���? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_DENYSPEC;
					else
						rooms[myroom].flag&=~ROOM_DENYSPEC;
					sprintf(buf, "�������÷���Ϊ%s", (buf[0]=='Y')?"�ܾ��Թ�":"���ܾ��Թ�");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "���û��˵���Ŀ:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					/* Efan: ɱ����������ʮ�����ڻ����п�ԭ */
					if(i>=0&&i<=10)
					{
						inrooms[myroom].killernum = i;
						sprintf(buf, "�������÷��仵����Ϊ%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "���þ������Ŀ:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					/* Efan: ��������Ȼ�����ڶ�������~~~�廪��ɱ��̫������~~~
					                        if(i>=0&&i<=2) {
					                            inrooms[myroom].policenum = i;
					                            sprintf(buf, "�������÷��侯����Ϊ%d", i);
					                            send_msg(-1, buf);
					                        }
					*/
					if (i >=0 && i <= 10)
					{
						inrooms [myroom].policenum = i;
						sprintf (buf, "�������÷��侯����Ϊ%d", i);
						send_msg (-1, buf);
					}
				}
				kill_msg(-1);
				return 0;
			case 6:
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "������Ҫ�ߵ�id:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					for(i=0;i<MAX_PEOPLE;i++)
						if(inrooms[myroom].peoples[i].style!=-1)
							if(!strcmp(inrooms[myroom].peoples[i].id, buf))
								break;
					if(i < MAX_PEOPLE && !strcmp(inrooms[myroom].peoples[i].id, buf) && inrooms[myroom].peoples[i].pid!=uinfo.pid)
					{
						start_change_inroom ();
						//inrooms[myroom].peoples[i].flag&=~PEOPLE_ALIVE;
						send_msg(i, "�㱻����");
						inrooms[myroom].peoples[i].style = -1;
						player_drop (i);
						rooms[myroom].people--;
						kill_msg(-1);
						end_change_inroom ();
						return 2;
					}
				}
				return 0;
			case 7:
				start_change_inroom();
				if(inrooms[myroom].status == INROOM_STOP)
					start_killergame();
				else
				{
					inrooms[myroom].status = INROOM_STOP;
					send_msg(-1, "��Ϸ������ǿ�ƽ���");
					end_change_inroom();
					kill_msg(-1);
				}
				return 0;
			}
			break;
		default:
			for(i=0;i<max;i++)
				if(ch==menus[i][0])
					sel=i;
			break;
		}
	}
	while(1);
}

void join_room(int w, int spec)
{
	char buf[200],buf2[200],buf3[200],msg[200],roomname[80];
	int i,j,k,me;
	clear();
	myroom = w;
	start_change_inroom();
	if(rooms[myroom].style!=1)
	{
		end_change_inroom();
		return;
	}
	strcpy(roomname, rooms[myroom].name);
	signal(SIGUSR1, room_refresh);
	i=0;
	while(inrooms[myroom].peoples[i].style!=-1)
		i++;
	mypos = i;
	inrooms[myroom].peoples[i].style = 0;
	inrooms[myroom].peoples[i].flag = 0;
	strcpy(inrooms[myroom].peoples[i].id, currentuser.userid);
	strcpy(inrooms[myroom].peoples[i].nick, currentuser.userid);
	inrooms[myroom].peoples[i].pid = uinfo.pid;
	if(rooms[myroom].people==0 && !strcmp(rooms[myroom].creator, currentuser.userid))
		inrooms[myroom].peoples[i].flag = PEOPLE_ROOMOP;
	if(spec)
		inrooms[myroom].peoples[i].flag|=PEOPLE_SPECTATOR;
	rooms[myroom].people++;
	end_change_inroom();

	kill_msg(-1);
	/*    sprintf(buf, "%s���뷿��", currentuser.userid);
	    for(i=0;i<myroom->people;i++) {
	        send_msg(inrooms.peoples+i, buf);
	        kill(inrooms.peoples[i].pid, SIGUSR1);
	    }*/

	room_refresh(0);
	while(1)
	{
		do
		{
			int ch;
			ch=-k_getdata(t_lines-1, 0, "����:", buf, 70, 1, NULL, 1);
			if(rooms[myroom].style!=1)
				kicked = 1;
			if(kicked)
				goto quitgame;
			if(ch==KEY_UP)
			{
				selected--;
				if(selected<0)
					selected = rooms[myroom].people-1;
				if(ipage>selected)
					ipage=selected;
				if(selected>ipage+t_lines-5)
					ipage=selected-(t_lines-5);
				refreshit();
			}
			else if(ch==KEY_DOWN)
			{
				selected++;
				if(selected>=rooms[myroom].people)
					selected=0;
				if(ipage>selected)
					ipage=selected;
				if(selected>ipage+t_lines-5)
					ipage=selected-(t_lines-5);
				refreshit();
			}
			else if(ch==KEY_PGUP)
			{
				jpage+=t_lines/2;
				if (jpage > get_msgt () - 1)
					jpage = get_msgt () - 1;
				refreshit();
			}
			else if(ch==KEY_PGDN)
			{
				jpage-=t_lines/2;
				if(jpage<=0)
					jpage=0;
				refreshit();
			}
			/* Efan: �Ͽ���ɱ�ˣ�������������֤ɱ�� */
			else if(ch==Ctrl('S'))
			{
				int pid;
				int sel;
				sel = getpeople(selected);
				if(sel==-1)
					continue;
				if(inrooms[myroom].peoples[sel].style == -1)
					continue;
				me=mypos;
				pid=inrooms[myroom].peoples[sel].pid;

				/* Efan: �绤ʱ��ɱ */
				if (inrooms[myroom].status == INROOM_DEFENCE)
				{
					struct people_struct * ppeop = inrooms[myroom].peoples;
					int turn = inrooms[myroom].turn;
					if (me != inrooms[myroom].voted[turn])
						continue;
					if (me != sel || inrooms[myroom].suicide != -1)
						continue;
					if (!(ppeop[me].flag & PEOPLE_KILLER))
						continue;
					send_msg (me, "[31;1m������ˡ���[m");
					kill_msg (-1);
					start_change_inroom ();
					inrooms[myroom].suicide = me;
					end_change_inroom ();
					continue;
				}
				/* Efan: ָ֤ */
				if (inrooms[myroom].status == INROOM_CHECK)
				{
					struct people_struct * ppeop = inrooms[myroom].peoples;
					int turn = inrooms[myroom].turn;
					if (me != inrooms[myroom].voted[turn])
						continue;
					if (ppeop[sel].style == -1 || !(ppeop[sel].flag & PEOPLE_ALIVE) || ppeop[sel].flag & PEOPLE_SPECTATOR)
						continue;
					if (inrooms[myroom].test != -1)
						continue;
					if (inrooms[myroom].suicide == -1 && me == sel && inrooms[myroom].peoples[me].flag & PEOPLE_KILLER)
					{
						send_msg (me, "[31;1m������ˡ���[m");
						kill_msg (-1);
						start_change_inroom ();
						inrooms[myroom].suicide = me;
						inrooms[myroom].test = me;
						end_change_inroom ();
						continue;
					}
					else if (me == sel)
						continue;
					start_change_inroom ();
					ppeop[sel].vote = 1;
					ppeop[sel].flag |= PEOPLE_VOTED;
					inrooms[myroom].test = sel;
					end_change_inroom ();
					kill_msg (-1);
					continue;
				}
				/* Efan: ɱ�ֻ */
				if(inrooms[myroom].status==INROOM_NIGHT && inrooms[myroom].peoples[me].flag & PEOPLE_KILLER)
				{
					if (inrooms[myroom].peoples[me].style == -1 || inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR || !(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER) || !(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE))
						continue;
					if (inrooms[myroom].peoples[sel].flag & PEOPLE_SPECTATOR)
					{
						send_msg (me, "\33[31;1m�����Թ���\33[m");
						refreshit ();
						continue;
					}
					else if (!(inrooms[myroom].peoples[sel].flag & PEOPLE_ALIVE))
					{
						send_msg (me, "\33[31;1m��������\33[m");
						refreshit ();
						continue;
					}
					if (inrooms[myroom].dead != -1)
						if (inrooms[myroom].killernum != -1)
							send_to_fellow (PEOPLE_KILLER, "\33[31;1m-_-!!̫����ҵ�ˣ�����һ����ɱ�����˵ġ���\33[m");
						else
							send_to_fellow (PEOPLE_KILLER, "[31;1m-_-!!̫����ҵ�ˣ��������![m");
					else
					{
						//int i;
						inrooms[myroom].dead = pid;
						if (inrooms[myroom].killernum != -1)
							sprintf (buf, "�ã������ˣ���ɱ\33[32;1m %s \33[m!", inrooms[myroom].peoples[sel].nick);
						else
							sprintf (buf, "�ã�����[32;1m %s [mһ�Ű�!", inrooms[myroom].peoples[sel].nick);
						send_to_fellow (PEOPLE_KILLER, buf);
						kill_msg (-1);
						if (inrooms[myroom].test != -1 || inrooms[myroom].policenum == 0)/* || inrooms[myroom].killernum == -1*/
							gotoday ();
					}
					kill_msg (-1);
					continue;
				}
				if (inrooms[myroom].status == INROOM_NIGHT && inrooms[myroom].peoples[me].flag & PEOPLE_POLICE)
				{
					int pid;
					int sel;
					sel = getpeople(selected);
					if(sel==-1)
						continue;
					me = mypos;
					pid = inrooms[myroom].peoples[me].pid;
					if(!(inrooms[myroom].peoples[me].flag&PEOPLE_POLICE))
						continue;
					if(!(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE))
						continue;
					if(inrooms[myroom].test!=-1)
					{
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m���־����Ѿ�������\33[m");
						refreshit ();
						kill_msg (-1);
						continue;
					}
					if(inrooms[myroom].peoples[sel].flag&PEOPLE_SPECTATOR)
						send_to_fellow(PEOPLE_POLICE, "[31;1m�������Թ���[m");
					else if(!(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE))
						send_to_fellow(PEOPLE_POLICE, "[31;1m��������[m");
					else if (inrooms[myroom].peoples[sel].flag&PEOPLE_POLICE)
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m�����Լ���~~~\33[m");
					else if((inrooms[myroom].peoples[sel].flag&PEOPLE_KILLER))
					{
						inrooms[myroom].test = sel;
						sprintf (buf,"��������������������������似�ĵ��������Ѱ�������...û��\33[32;1m%s \33[m����һλ��ɫ��ɱ��!", inrooms[myroom].peoples[sel].nick);
						send_to_fellow (PEOPLE_POLICE, buf);
					}
					else
					{
						/* Efan: �Ͽ��� */
						sprintf (buf, "����\33[32;1m %s \33[m�����", inrooms[myroom].peoples[sel].nick);
						send_to_fellow(PEOPLE_POLICE, buf);
						inrooms[myroom].test = sel;
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m�������´��������������Ǻ�����~~~\33[m");
					}
					kill_msg (-1);
					if (inrooms[myroom].test != -1 && inrooms[myroom].dead != -1)
					{
						/* Efan: �Ͽ��棬��ʱ��ת������� */
						gotoday ();
					}
					refreshit();
				}
				if(inrooms[myroom].peoples[me].vote==0)
					if(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE&&
					        inrooms[myroom].status==INROOM_VOTE)
					{
						if(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE &&
						        !(inrooms[myroom].peoples[sel].flag&PEOPLE_SPECTATOR) &&
						        inrooms[myroom].peoples[sel].flag & PEOPLE_VOTED)
						{
							int i,j,t1,t2,t3,t4;
							int max=0, ok=0, maxi, maxpid;
							sprintf(buf, "[32;1mͶ%d %sһƱ[m", sel+1, inrooms[myroom].peoples[sel].nick);
							send_msg (me, buf);
							kill_msg (-1);
							start_change_inroom();
							inrooms[myroom].peoples[me].vote = sel + 1;
							end_change_inroom();
							for (i = 0; i < MAX_PEOPLE; ++i)
								if (inrooms[myroom].peoples[i].style != -1 && inrooms[myroom].peoples[i].flag & PEOPLE_ALIVE && !(inrooms[myroom].peoples[i].flag & PEOPLE_SPECTATOR) && inrooms[myroom].peoples[i].vote == 0)
									break;
							if (i < MAX_PEOPLE)
							{
								continue;
							}
checkvote:
							for (i = 0; i < MAX_PEOPLE; ++i)
								if (inrooms[myroom].peoples[i].style != -1 && inrooms[myroom].peoples[i].flag & PEOPLE_ALIVE && ! (inrooms[myroom].peoples[i].flag & PEOPLE_SPECTATOR))
								{
									int tmp = inrooms[myroom].peoples[i].vote - 1;
									sprintf (buf, "[32;1m%d %sͶ��%d %sһƱ[m", i + 1, inrooms[myroom].peoples[i].nick, tmp + 1, inrooms[myroom].peoples[tmp].nick);
									inrooms[myroom].peoples[i].vote = inrooms[myroom].peoples[tmp].pid;
									send_msg (-1, buf);
								}
							kill_msg (-1);
							inrooms[myroom].rotate++;
							t1=0;
							t2=0;
							t3=0;
							t4 = 0;
							for(i=0;i<MAX_PEOPLE;i++)
								inrooms[myroom].peoples[i].vnum = 0;
							for(i=0;i<MAX_PEOPLE;i++)
								if(inrooms[myroom].peoples[i].style!=-1)
									if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR)&&
									        inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
									{
										for(j=0;j<MAX_PEOPLE;j++)
											if(inrooms[myroom].peoples[j].style!=-1)
												if(inrooms[myroom].peoples[j].pid == inrooms[myroom].peoples[i].vote)
												{
													t3++;
													inrooms[myroom].peoples[j].vnum++;
												}
									}
							/* Efan: ���´��������ҳ�ǰ������Ʊ����������д�� */
							for(i=0;i<MAX_PEOPLE;i++)
								if(inrooms[myroom].peoples[i].style!=-1)
									if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR)&&
									        inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
									{
										if(inrooms[myroom].peoples[i].vnum>t1)
										{
											t2=t1;
											t1=inrooms[myroom].peoples[i].vnum;
											maxi = i;
											max = t2;
											maxpid = inrooms[myroom].peoples[i].pid;
										}
										else if(inrooms[myroom].peoples[i].vnum>t2 && inrooms[myroom].peoples[i].vnum != t1)
										{
											t2=inrooms[myroom].peoples[i].vnum;
										}
									}
							if (t2 == 0)
								t2 = t1;
							for (i = 0; i < MAX_PEOPLE; ++i)
								if (inrooms[myroom].peoples[i].style != -1 && !(inrooms[myroom].peoples[i].flag & PEOPLE_SPECTATOR) && inrooms[myroom].peoples[i].flag & PEOPLE_ALIVE && inrooms[myroom].peoples[i].vnum == t1)
									t4++;
							if (t4 >= 2)
								t2 = t1;

							sprintf(buf, "\33[31;1m�� %d ��ͶƱ���:\33[m", inrooms[myroom].rotate);
							send_msg(-1, buf);
							for(i=0;i<MAX_PEOPLE;i++)
								if(inrooms[myroom].peoples[i].style!=-1)
									if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) && inrooms[myroom].peoples[i].flag & PEOPLE_VOTED &&
									        inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
									{
										sprintf(buf, "%s��Ͷ: %d Ʊ",
										        inrooms[myroom].peoples[i].nick,
										        inrooms[myroom].peoples[i].vnum);
										/*
										                            if(inrooms[myroom].peoples[i].vnum==max)
										                                ok=0;
										                            if(inrooms[myroom].peoples[i].vnum>max) {
										                                max=inrooms[myroom].peoples[i].vnum;
										                                ok=1;
										                                maxi=i;
										                                maxpid=inrooms[myroom].peoples[i].pid;
										                            }
										*/
										send_msg(-1, buf);
									}
							/* Efan: �����������������Ч */
							if (t1 > t3/2)
								ok = 1;
							else
								ok = 0;
							if(!ok && inrooms[myroom].rotate < 3)
							{
								int t;
								struct people_struct * ppeop;
								send_msg(-1, "\33[31m���Ʊ��δ��������,������ͶƱ...\33[m");
								start_change_inroom();
								t = 0;
								ppeop = inrooms[myroom].peoples;
								if (inrooms[myroom].rotate == 1)
								{//��ʱ��
									j = inrooms[myroom].dead;
									if (j < 0)
										j = MAX_PEOPLE - 1;
									for (i = 0; i < MAX_PEOPLE; ++i)
									{
										if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR) && (ppeop[j].vnum == t1 || ppeop[j].vnum == t2) && ppeop[j].flag & PEOPLE_VOTED)
										{
											inrooms[myroom].voted[t] = j;
											++t;
										}
										else
										{
											ppeop[j].flag &= ~PEOPLE_VOTED;
										}
										--j;
										if (j < 0)
											j = MAX_PEOPLE - 1;
									}
								}
								else
								{//inrooms[myroom].rotate == 2 ˳ʱ��
									j = inrooms[myroom].dead;
									if (j >= MAX_PEOPLE)
										j = 0;
									for (i = 0; i < MAX_PEOPLE; ++i)
									{
										if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR) && (ppeop[j].vnum == t1 || ppeop[j].vnum == t2) && ppeop[j].flag & PEOPLE_VOTED)
										{
											inrooms[myroom].voted[t] = j;
											++t;
										}
										else
										{
											ppeop[j].flag &= ~PEOPLE_VOTED;
										}
										++j;
										if (j >= MAX_PEOPLE)
											j = 0;
									}
								}
								if (t <= 0)
								{
									send_msg (-1, "\33[1;31m��Ϸ�쳣����\33[m");
									inrooms[myroom].status = INROOM_STOP;
									end_change_inroom ();
									kill_msg (-1);
									continue;
								}
								inrooms[myroom].numvoted = t;
								inrooms[myroom].turn = 0;
								inrooms[myroom].test = -1;
								inrooms[myroom].status = INROOM_DEFENCE;
								for(j=0;j<MAX_PEOPLE;j++)
									inrooms[myroom].peoples[j].vote=0;
								end_change_inroom();
								send_msg (-1, "\33[31;1m������Ʊ���Ĺ�����Ϊ�Լ��绤\33[m");
								sprintf (buf, "������\33[32;1m %d %s \33[m����", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
								send_msg (-1, buf);
								send_msg (inrooms[myroom].voted[0], "�绤����밴\33[31;1mCtrl+T\33[m��������");
							}
							else
							{
								int a=0,b=0;
								int k;
								struct people_struct * ppeop = inrooms[myroom].peoples;
								/* Efan: ͶƱ�������޽�������������ȡһ�� */
								if (!ok && inrooms[myroom].rotate >= 3)
								{
									send_msg (-1, "\33[31;1mͶƱ������δ�н���������������\33[m");
									b = rand () % inrooms[myroom].numvoted;
									for (k = 0; k < inrooms[myroom].numvoted; ++k)
									{
										a = inrooms[myroom].voted[b];
										if (ppeop[a].style != -1 && ppeop[a].flag & PEOPLE_ALIVE && !(ppeop[a].flag & PEOPLE_SPECTATOR) && ppeop[a].flag & PEOPLE_VOTED)
											break;
										b++;
										if (b >= inrooms[myroom].numvoted)
											b = 0;
									}
									if (k >= inrooms[myroom].numvoted)
									{
										send_msg (-1, "[1;31m��Ϸ�쳣����[m");
										inrooms[myroom].status = INROOM_STOP;
										end_change_inroom ();
										kill_msg (-1);
										continue;
									}
									max = ppeop[a].vnum;
									maxi = a;
									maxpid = ppeop[a].pid;
								}
								send_msg(maxi, "\33[31;1m�㱻���ٴ�����!\33[m");
								sprintf (buf, "\33[32;1m %d %s \33[31;1m�����ٴ�����\33[m", maxi + 1, ppeop[maxi].nick);
								for(j=0;j<MAX_PEOPLE;j++)
									if(inrooms[myroom].peoples[j].style!=-1)
										if(j!=maxi)
											send_msg(j, buf);
								gotodark (maxpid);
							}
							kill_msg(-1);
						}
						/*
						              else {
						                  if(sel==me)
						                      send_msg(me, "[31;1m�㲻��ѡ����ɱ[m");
						                  else if(!(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE))
						                      send_msg(me, "[31;1m��������[m");
						                  else
						                      send_msg(me, "[31;1m�������Թ���[m");
						                  refreshit();
						              }
						*/
					}
			}
			else if(ch==Ctrl('T'))
			{
				int pid;
				int sel;
				sel = getpeople(selected);
				if(sel==-1)
					continue;
				me=mypos;
				pid=inrooms[myroom].peoples[sel].pid;

				/* Efan: ���������ɸ�ת�����ʱ�����߷��ԵĹ��� */
				if (inrooms[myroom].status == INROOM_DAY)
				{
					struct people_struct * ppeop;
					int turn = inrooms[myroom].turn;
					if (me != inrooms[myroom].voted[turn])
						continue;
					start_change_inroom ();
					ppeop = inrooms[myroom].peoples;
					sprintf (buf, "[35;1m%d %s[m: \33[31;1mOver.\33[m", me + 1, inrooms[myroom].peoples[me].nick);
					send_msg (-1, buf);
					sprintf (buf, "��ô����\33[35;1m %d %s \33[m�������Թ��ߵ���ݣ�������Ϸ...", me + 1, ppeop [me].nick);
					send_msg (-1, buf);
					++turn;
					while (turn < inrooms[myroom].numvoted)
					{
						int tmp = inrooms[myroom].voted[turn];
						if (ppeop[tmp].style != -1 && ppeop[tmp].flag & PEOPLE_ALIVE && !(ppeop[tmp].flag & PEOPLE_SPECTATOR))
							break;
						++turn;
					}
					if (turn >= inrooms[myroom].numvoted)
					{
						send_msg (-1, "\33[31;1m��Ϸ�쳣����!\33[m");
						kill_msg (-1);
						inrooms[myroom].status = INROOM_STOP;
						end_change_inroom ();
						continue;
					}
					inrooms[myroom].turn = turn;
					inrooms[myroom].status = INROOM_CHECK;
					inrooms[myroom].test = -1;
					for (i = 0; i < MAX_PEOPLE; ++i)
						ppeop[i].vote = 0;
					end_change_inroom ();
					send_msg (-1, "\33[31;1m���ڿ�ʼָ֤\33[m");
					sprintf (buf,"����\33[32;1m %d %s \33[m����", inrooms[myroom].voted[turn] + 1, ppeop [inrooms[myroom].voted[turn]].nick);
					send_msg (-1, buf);
					send_msg (inrooms[myroom].voted[turn], "��[31;1m�����ƶ����[m���������ɵĶ�����");
					send_msg (inrooms[myroom].voted[turn], "��\33[31;1mCtrl+S\33[mָ֤");
					send_msg (inrooms[myroom].voted[turn], "ָ֤�����\33[31;1mCtrl+T\33[m��������");
					if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
						send_msg (inrooms[myroom].voted[turn], "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
					kill_msg (-1);
					continue;
				}
				/* Efan: ָ֤ */
				if (inrooms[myroom].status == INROOM_CHECK)
				{
					struct people_struct * ppeop = inrooms[myroom].peoples;
					int turn = inrooms[myroom].turn;
					if (me != inrooms[myroom].voted[turn])
						continue;
					if (inrooms[myroom].test == -1)
					{
						send_msg (me, "����ָ֤һ����");
						refreshit ();
						continue;
					}
					start_change_inroom ();
					/*
								ppeop[sel].vote = 1;
								ppeop[sel].flag |= PEOPLE_VOTED;
					*/
					sprintf (buf, "[32;1m%d %s[m: \33[31;1mOver.\33[m", me + 1, inrooms[myroom].peoples[me].nick);
					send_msg (-1, buf);
					if (inrooms[myroom].suicide != me)
						sprintf (buf, "\33[32;1m%d %s ָ֤ %d %s\33[m", me+1, ppeop[me].nick, inrooms[myroom].test+1, ppeop[inrooms[myroom].test].nick);
					else
						sprintf (buf, "\33[31;1mɱ�� %d %s �����ˡ���\33[m", me + 1, ppeop[me].nick);
					send_msg (-1, buf);
					++turn;
					while (turn < inrooms[myroom].numvoted)
					{
						int tmp = inrooms[myroom].voted[turn];
						if (ppeop[tmp].style != -1 && ppeop[tmp].flag & PEOPLE_ALIVE && !(ppeop[tmp].flag & PEOPLE_SPECTATOR))
							break;
						++turn;
					}
					inrooms[myroom].turn = turn;
					if (turn >= inrooms[myroom].numvoted)
					{
						int t = 0;
						if (inrooms[myroom].suicide != -1)
						{
							end_change_inroom ();
							killer_suicided ();
							continue;
						}
						j = inrooms[myroom].dead;
						if (j >= MAX_PEOPLE)
							j = 0;
						for (i = 0; i < MAX_PEOPLE; ++i)
						{
							if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].vote == 1)
							{
								inrooms[myroom].voted[t] = j;
								++t;
							}
							++j;
							if (j >= MAX_PEOPLE)
								j = 0;
						}
						if (t <= 0)
						{
							send_msg (-1, "\33[31;1m��Ϸ�쳣����!\33[m");
							kill_msg (-1);
							inrooms[myroom].status = INROOM_STOP;
							end_change_inroom ();
							continue;
						}
						inrooms[myroom].numvoted = t;
						inrooms[myroom].turn = 0;
						inrooms[myroom].status = INROOM_DEFENCE;
						for (i = 0; i < MAX_PEOPLE; ++i)
							ppeop[i].vote = 0;
						send_msg (-1, "\33[31;1mָ֤����\33[m");
						send_msg (-1, "\33[31;1m�����뱻ָ֤�Ĺ�����\33[m");
						sprintf (buf, "������\33[32;1m %d %s \33[mΪ�Լ��绤", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[0], "�绤����밴\33[31;1mCtrl+T\33[m��������");
						if (ppeop[inrooms[myroom].voted[0]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[0], "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
					}
					else
					{
						sprintf (buf, "������\33[32;1m %d %s \33[m����", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[turn], "��[31;1m�����ƶ����[m���������ɵĶ�����");
						send_msg (inrooms[myroom].voted[turn], "��\33[31;1mCtrl+S\33[mָ֤");
						send_msg (inrooms[myroom].voted[turn], "ָ֤�����\33[31;1mCtrl+T\33[m��������");
						if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[turn], "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
						inrooms[myroom].test = -1;
					}
					end_change_inroom ();
					kill_msg (-1);
					continue;
				}
				/* Efan: �绤 */
				if (inrooms[myroom].status == INROOM_DEFENCE)
				{
					int turn = inrooms[myroom].turn;
					struct people_struct * ppeop = inrooms[myroom].peoples;
					if (me != inrooms[myroom].voted[turn])
						continue;
					++turn;
					sprintf (buf, "[32;1m%d %s[m: \33[31;1mOver.\33[m", me + 1, inrooms[myroom].peoples[me].nick);
					send_msg (-1, buf);
					if (inrooms[myroom].suicide == me)
					{
						sprintf (buf, "[31;1mɱ�� %d %s �����ˡ���[m", me + 1, inrooms[myroom].peoples[me].nick);
						send_msg (-1, buf);
					}
					start_change_inroom ();
					while (turn < inrooms[myroom].numvoted)
					{
						int tmp = inrooms[myroom].voted[turn];
						if (ppeop[tmp].style != -1 && ppeop[tmp].flag & PEOPLE_ALIVE && !(ppeop[tmp].flag & PEOPLE_SPECTATOR))
							break;
						++turn;
					}
					inrooms[myroom].turn = turn;
					if (turn >= inrooms[myroom].numvoted)
					{
						int i;
						if (inrooms[myroom].suicide != -1)
						{
							end_change_inroom ();
							killer_suicided ();
							continue;
						}
						send_msg (-1, "\33[31;1m�绤���������ڿ�ʼͶƱ\33[m");
						send_msg (-1, "\33[31;1m�������ƶ����ѡ�������Ctrl+SͶƱ...\33[m");
						for (i = 0; i < MAX_PEOPLE; ++i)
							ppeop[i].vote = 0;
						inrooms[myroom].status = INROOM_VOTE;
					}
					else
					{
						sprintf (buf, "������\33[32;1m %d %s \33[mΪ�Լ��绤", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[turn], "�绤����밴\33[31;1mCtrl+T\33[m��������");
						if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[turn], "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
					}
					kill_msg (-1);
					end_change_inroom ();
					continue;
				}
				if (inrooms[myroom].status == INROOM_DARK)
				{
					if (me != inrooms[myroom].turn)
						continue;
					sprintf (buf, "[32;1m%d %s[m: \33[31;1mOver.\33[m", me + 1, inrooms[myroom].peoples[me].nick);
					send_msg (-1, buf);
					send_msg (-1, "\33[1;31m����...\33[m");
					start_change_inroom ();
					gotonight ();
					end_change_inroom ();
					continue;
				}
				/* Efan: ɱ�ֻ */
				/*
				                if(inrooms[myroom].status==INROOM_NIGHT) {
							if (inrooms[myroom].peoples[me].style == -1 || inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR || !(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER) || !(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE))
							       continue;
							if (inrooms[myroom].peoples[sel].flag & PEOPLE_SPECTATOR) {
								send_msg (me, "\33[31;1m�����Թ���\33[m");
								refreshit ();
								continue;
							}
							else if (!(inrooms[myroom].peoples[sel].flag & PEOPLE_ALIVE)) {
								send_msg (me, "\33[31;1m��������\33[m");
								refreshit ();
								continue;
							}
							if (inrooms[myroom].dead != -1)
								send_to_fellow (PEOPLE_KILLER, "\33[31;1m-_-!!̫����ҵ�ˣ�����һ����ɱ�����˵ġ���\33[m");
							else {
								int i;
								inrooms[myroom].dead = pid;
								sprintf (buf, "�ã������ˣ���ɱ\33[32;1m %s \33[m!", inrooms[myroom].peoples[sel].nick);
								send_to_fellow (PEOPLE_KILLER, buf);
								kill_msg (-1);
								if (inrooms[myroom].test != -1)
									gotoday ();
							}
							continue;
						}
				*/

			}
			else if(ch<=0&&!buf[0])
			{
				kill_msg (-1);
				j=do_com_menu();
				if(kicked)
					goto quitgame;
				if(j==1)
					goto quitgame;
				if(j==2)
					if(inrooms[myroom].status!=INROOM_STOP)
						goto checkvote;
			}
			else if(ch<=0)
			{
				break;
			}
		}
		while(1);
		start_change_inroom();
		me=mypos;
		strcpy(msg, buf);
		if(msg[0]=='/'&&msg[1]=='/')
		{
			i=2;
			while(msg[i]!=' '&&i<strlen(msg))
				i++;
			strcpy(buf, msg+2);
			buf[i-2]=0;
			while(msg[i]==' '&&i<strlen(msg))
				i++;
			buf2[0]=0;
			buf3[0]=0;
			if(msg[i-1]==' '&&i<strlen(msg))
			{
				k=i;
				while(msg[k]!=' '&&k<strlen(msg))
					k++;
				strcpy(buf2, msg+i);
				buf2[k-i]=0;
				i=k;
				while(msg[i]==' '&&i<strlen(msg))
					i++;
				if(msg[i-1]==' '&&i<strlen(msg))
				{
					k=i;
					while(msg[k]!=' '&&k<strlen(msg))
						k++;
					strcpy(buf3, msg+i);
					buf3[k-i]=0;
				}
			}
			k=1;
			for(i=0;;i++)
			{
				if(!party_data[i].verb)
					break;
				if(!strcasecmp(party_data[i].verb, buf))
				{
					k=0;
					sprintf(buf, "%s [1m%s[m %s", party_data[i].part1_msg, buf2[0]?buf2:"���", party_data[i].part2_msg);
					break;
				}
			}
			if(k)
				for(i=0;;i++)
				{
					if(!speak_data[i].verb)
						break;
					if(!strcasecmp(speak_data[i].verb, buf))
					{
						k=0;
						sprintf(buf, "%s: %s", speak_data[i].part1_msg, buf2);
						break;
					}
				}
			if(k)
				for(i=0;;i++)
				{
					if(!condition_data[i].verb)
						break;
					if(!strcasecmp(condition_data[i].verb, buf))
					{
						k=0;
						sprintf(buf, "%s", condition_data[i].part1_msg);
						break;
					}
				}

			if(k)
				continue;
			strcpy(buf2, buf);
			sprintf(buf, "[1m%d %s[m %s", me+1, inrooms[myroom].peoples[me].nick, buf2);
		}
		else
		{
			strcpy(buf2, buf);
			if (inrooms[myroom].peoples[me].style != -1)
				if (inrooms[myroom].status == INROOM_STOP || (inrooms[myroom].peoples[me].flag & PEOPLE_ALIVE && !(inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR)))
					sprintf(buf, "[32;1m%d %s[m: %s", me+1, inrooms[myroom].peoples[me].nick, buf2);
				else
					sprintf(buf, "[35;1m%d %s[m: %s", me+1, inrooms[myroom].peoples[me].nick, buf2);
		}
		if (inrooms[myroom].peoples[me].style != -1 && inrooms[myroom].status != INROOM_DAY && inrooms[myroom].status != INROOM_DARK && inrooms[myroom].status != INROOM_STOP && ((inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR) || !(inrooms[myroom].peoples[me].flag & PEOPLE_ALIVE)))
		{
			send_to_fellow (PEOPLE_SPECTATOR, buf);
		}
		else if (!(inrooms[myroom].status == INROOM_VOTE))
			if(inrooms[myroom].status==INROOM_NIGHT)
			{
				/*
				            if(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER)
				            for(i=0;i<MAX_PEOPLE;i++) 
				            if(inrooms[myroom].peoples[i].style!=-1)
				            {
				                if(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER||
				                    inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) {
				                    send_msg(i, buf);
				                }
				            }
				*/
				/* Efan: ��������˵�� */
				int flag = inrooms[myroom].peoples[me].flag;
				if (inrooms[myroom].peoples[me].style != -1 &&
				        flag & PEOPLE_ALIVE && (flag & PEOPLE_KILLER || flag & PEOPLE_POLICE))
					send_to_fellow (flag, buf);
			}
			else if (inrooms[myroom].status == INROOM_STOP)
			{
				/* Efan: ����Ϸ����ʱ�������ⷢ�� */
				/*
						if (!(inrooms[myroom].peoples[me].flag&PEOPLE_SPECTATOR))
				*/
				send_msg (-1, buf);
			}
			else if (inrooms[myroom].status == INROOM_DARK)
			{
				/* Efan: ͶƱ���������������� */
				if (me == inrooms[myroom].turn)
					send_msg( -1, buf);
			}
			else
			{
				/* Efan: ������ʱ��ֻ���������� */
				int turn = inrooms[myroom].turn;
				if(inrooms[myroom].voted[turn] == me && !(inrooms[myroom].peoples[me].flag&PEOPLE_SPECTATOR))
					send_msg(-1, buf);
			}
		end_change_inroom();
		kill_msg(-1);
	}

quitgame:
	start_change_inroom();
	me=mypos;
	if(inrooms[myroom].peoples[me].flag&PEOPLE_ROOMOP)
	{
		for(i=0;i<MAX_PEOPLE;i++)
			if(inrooms[myroom].peoples[i].style!=-1)
				if(i!=me)
				{
					send_msg(i, "�㱻����");
				}
		rooms[myroom].style = -1;
		end_change_inroom();
		for(i=0;i<MAX_PEOPLE;i++)
			if(inrooms[myroom].peoples[i].style!=-1)
				if(i!=me)
					kill_msg(i);
		goto quitgame2;
	}
	inrooms[myroom].peoples[me].style=-1;
	rooms[myroom].people--;
	end_change_inroom();

	/*    if(killer)
	        sprintf(buf, "ɱ��%sǱ����", buf2);
	    else
	        sprintf(buf, "%s�뿪����", buf2);
	    for(i=0;i<myroom->people;i++) {
	        send_msg(inrooms.peoples+i, buf);
	        kill(inrooms.peoples[i].pid, SIGUSR1);
	    }*/
quitgame2:
	kicked=0;
	k_getdata(t_lines-1, 0, "�Ļر���ȫ����Ϣ��?[y/N]", buf3, 3, 1, 0, 1);
	if(toupper(buf3[0])=='Y')
	{
		sprintf(buf, "tmp/%d.msg", rand());
		save_msgs(buf);
		sprintf(buf2, "\"%s\"��ɱ�˼�¼", roomname);
		mail_file(buf, currentuser.userid, buf2);

	}
	//    signal(SIGUSR1, talk_request);
}

static int room_list_refresh(struct _select_def *conf)
{
	clear();
	docmdtitle("[��Ϸ��ѡ��]",
	           "  �˳�[[1;32m��[0;37m,[1;32me[0;37m] ����[[1;32mEnter[0;37m] ѡ��[[1;32m��[0;37m,[1;32m��[0;37m] ���[[1;32ma[0;37m] ����[[1;32mJ[0;37m] [m      ����: [31;1mbad@smth.org[m");
	move(2, 0);
	prints("[0;1;37;44m    %4s %-14s %-12s %4s %4s %6s %-20s[m", "���", "��Ϸ������", "������", "����", "���", "������", "����");
	clrtoeol();
	k_resetcolor();
	update_endline();
	return SHOW_CONTINUE;
}

static int room_list_show(struct _select_def *conf, int i)
{
	struct room_struct * r;
	int j = room_get(i-1);
	if(j!=-1)
	{
		r=rooms+j;
		prints("  %3d  %-14s %-12s %3d  %3d  %2s%2s%2s %-36s", i, r->name, r->creator, r->people, r->maxpeople, (r->flag&ROOM_LOCKED)?"��":"", (r->flag&ROOM_SECRET)?"��":"", (!(r->flag&ROOM_DENYSPEC))?"��":"", r->title);
	}
	return SHOW_CONTINUE;
}

static int room_list_select(struct _select_def *conf)
{
	struct room_struct * r, * r2;
	int i=room_get(conf->pos-1), j;
	char ans[4];
	if(i==-1)
		return SHOW_CONTINUE;
	r = rooms+i;
	j=find_room(r->name);
	if(j==-1)
	{
		move(0, 0);
		clrtoeol();
		prints(" �÷����ѱ�����!");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	r2 = rooms+j;
	if(r2->people>=r2->maxpeople&&!HAS_PERM(PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" �÷�����������");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	k_getdata(0, 0, "�Ƿ����Թ�����ݽ���? [y/N]", ans, 3, 1, NULL, 1);
	if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC&&!HAS_PERM(PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" �÷���ܾ��Թ���");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	join_room(find_room(r2->name), toupper(ans[0])=='Y');
	return SHOW_DIRCHANGE;
}

static int room_list_k_getdata(struct _select_def *conf, int pos, int len)
{
	clear_room();
	conf->item_count = room_count();
	return SHOW_CONTINUE;
}

static int room_list_prekey(struct _select_def *conf, int *key)
{
	switch (*key)
	{
	case 'e':
	case 'q':
		*key = KEY_LEFT;
		break;
	case 'p':
	case 'k':
		*key = KEY_UP;
		break;
	case ' ':
	case 'N':
		*key = KEY_PGDN;
		break;
	case 'n':
	case 'j':
		*key = KEY_DOWN;
		break;
	}
	return SHOW_CONTINUE;
}

static int room_list_key(struct _select_def *conf, int key)
{
	struct room_struct r, *r2;
	int i,j;
	char name[40], ans[4];
	switch(key)
	{
	case 'a':
		strcpy(r.creator, currentuser.userid);
		k_getdata(0, 0, "������:", name, 13, 1, NULL, 1);
		if(!name[0])
			return SHOW_REFRESH;
		if(name[0]==' '||name[strlen(name)-1]==' ')
		{
			move(0, 0);
			clrtoeol();
			prints(" ��������ͷ��β����Ϊ�ո�");
			refresh();
			sleep(1);
			return SHOW_CONTINUE;
		}
		strcpy(r.name, name);
		r.style = 1;
		r.flag = 0;
		r.people = 0;
		r.maxpeople = 100;
		strcpy(r.title, "ɱ���к�����...");
		if(add_room(&r)==-1)
		{
			move(0, 0);
			clrtoeol();
			prints(" ��һ�����ֵķ�����!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(r.name), 0);
		return SHOW_DIRCHANGE;
	case 'J':
		k_getdata(0, 0, "������:", name, 12, 1, NULL, 1);
		if(!name[0])
			return SHOW_REFRESH;
		if((i=find_room(name))==-1)
		{
			move(0, 0);
			clrtoeol();
			prints(" û���ҵ��÷���!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		r2 = rooms+i;
		if(r2->people>=r2->maxpeople&&!HAS_PERM(PERM_SYSOP))
		{
			move(0, 0);
			clrtoeol();
			prints(" �÷�����������");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		k_getdata(0, 0, "�Ƿ����Թ�����ݽ���? [y/N]", ans, 3, 1, NULL, 1);
		if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC&&!HAS_PERM(PERM_SYSOP))
		{
			move(0, 0);
			clrtoeol();
			prints(" �÷���ܾ��Թ���");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(name), toupper(ans[0])=='Y');
		return SHOW_DIRCHANGE;
	case 'K':
		if(!HAS_PERM(PERM_SYSOP))
			return SHOW_CONTINUE;
		i = room_get(conf->pos-1);
		if(i!=-1)
		{
			r2 = rooms+i;
			r2->style = -1;
			for(j=0;j<MAX_PEOPLE;j++)
				if(inrooms[i].peoples[j].style!=-1)
					kill(inrooms[i].peoples[j].pid, SIGUSR1);
		}
		return SHOW_DIRCHANGE;
	}
	return SHOW_CONTINUE;
}

void show_top_board()
{
	FILE* fp;
	char buf[80];
	int i,j,x,y;
	clear();
	for(i=1;i<=6;i++)
	{
		sprintf(buf, "service/killer.%d", i);
		fp=fopen(buf, "r");
		if(!fp)
			return;
		for(j=0;j<7;j++)
		{
			if(feof(fp))
				break;
			y=(i-1)%3*8+j;
			x=(i-1)/3*40;
			if(fgets(buf, 80, fp)==NULL)
				break;
			move(y, x);
			k_resetcolor();
			if(j==2)
				k_setfcolor(RED, 1);
			prints("%s", buf);
		}
		fclose(fp);
	}
	pressanykey();
}

int choose_room()
{
	struct _select_def grouplist_conf;
	int i;
	POINT *pts;

	clear_room();
	bzero(&grouplist_conf, sizeof(struct _select_def));
	grouplist_conf.item_count = room_count();
	if (grouplist_conf.item_count == 0)
	{
		grouplist_conf.item_count = 1;
	}
	pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
	for (i = 0; i < BBS_PAGESIZE; i++)
	{
		pts[i].x = 2;
		pts[i].y = i + 3;
	}
	grouplist_conf.item_per_page = BBS_PAGESIZE;
	grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
	grouplist_conf.prompt = "��";
	grouplist_conf.item_pos = pts;
	grouplist_conf.arg = NULL;
	grouplist_conf.title_pos.x = 0;
	grouplist_conf.title_pos.y = 0;
	grouplist_conf.pos = 1;     /* initialize cursor on the first mailgroup */
	grouplist_conf.page_pos = 1;        /* initialize page to the first one */

	grouplist_conf.on_select = room_list_select;
	grouplist_conf.show_data = room_list_show;
	grouplist_conf.pre_key_command = room_list_prekey;
	grouplist_conf.key_command = room_list_key;
	grouplist_conf.show_title = room_list_refresh;
	grouplist_conf.get_data = room_list_k_getdata;
	show_top_board();
	list_select_loop(&grouplist_conf);
	free(pts);
}

int killer_main()
{
	int i,oldmode;
	void * shm, * shm2;
	shm=attach_shm2("GAMEROOM_SHMKEY", 3451, sizeof(struct room_struct)*MAX_ROOM, &i);
	rooms = shm;
	if(i)
	{
		for(i=0;i<MAX_ROOM;i++)
		{
			rooms[i].style=-1;
			rooms[i].w = 0;
			rooms[i].level = 0;
		}
	}
	shm2=attach_shm2("KILLER_SHMKEY", 9578, sizeof(struct inroom_struct)*MAX_ROOM, &i);
	inrooms = shm2;
	if(i)
	{
		for(i=0;i<MAX_ROOM;i++)
			inrooms[i].w = 0;
	}
	oldmode = uinfo.mode;
	modify_user_mode(KILLER);
	srand (time (NULL));
	choose_room();
	modify_user_mode(oldmode);
	shmdt(shm);
	shmdt(shm2);
}

int gotodark (int maxpid)
{
	int a, b, c, i, maxi;
	char buf [256];
	struct people_struct * ppeop;
	ppeop = inrooms[myroom].peoples;
	start_change_inroom ();
	for (i = 0; i < MAX_PEOPLE; ++i)
		if (ppeop[i].style != -1 && ppeop[i].pid == maxpid)
		{
			maxi = i;
			ppeop[i].flag &= ~PEOPLE_ALIVE;
		}
	a = b = c = 0;
	for (i = 0; i < MAX_PEOPLE; ++i)
	{
		register int flag = ppeop[i].flag;
		if (ppeop[i].style != -1 && flag & PEOPLE_ALIVE && !(flag & PEOPLE_SPECTATOR))
			if (flag & PEOPLE_KILLER)
				a++;
			else if (flag & PEOPLE_POLICE)
				b++;
			else
				c++;
	}
	if (a > 0 && (b == 0 || c == 0))
	{//����ʤ��
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m���˻����ʤ��...\33[m");
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
			if (ppeop[i].style != -1 && ppeop[i].flag & PEOPLE_KILLER && ppeop[i].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "ԭ��\33[32;1m %d %s \33[m��ɱ��!", i + 1, ppeop[i].nick);
				send_msg (-1, buf);
			}
	}
	else if (a == 0)
	{//����ʤ��
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m����ɱ�ֶ��������ˣ����˻����ʤ��...\33[m");
		save_result (0);
	}
	else
	{//��Ϸ���ڼ���
		inrooms[myroom].status = INROOM_DARK;
		inrooms[myroom].turn = maxi;
		if (ppeop[maxi].flag & PEOPLE_KILLER)
		{
			send_msg (-1, "\33[31;1mû������\33[m");
			gotonight ();
		}
		else
		{
			sprintf (buf, "\33[32;1m%d %s \33[31;1m����������\33[m", maxi + 1, ppeop[maxi].nick);
			send_msg (-1, buf);
			send_msg (maxi, "��\33[31;1mCtrl+T\33[m��������");
		}
	}
	kill_msg (-1);
	end_change_inroom ();
	return 0;
}

int killer_suicided (void)
{
	int a, b, c, i, maxi;
	int maxpid;
	char buf [256];
	struct people_struct * ppeop;
	ppeop = inrooms[myroom].peoples;
	maxpid = ppeop[inrooms[myroom].suicide].pid;
	start_change_inroom ();
	for (i = 0; i < MAX_PEOPLE; ++i)
		if (ppeop[i].style != -1 && ppeop[i].pid == maxpid)
		{
			maxi = i;
			ppeop[i].flag &= ~PEOPLE_ALIVE;
		}
	a = b = c = 0;
	for (i = 0; i < MAX_PEOPLE; ++i)
	{
		register int flag = ppeop[i].flag;
		if (ppeop[i].style != -1 && flag & PEOPLE_ALIVE && !(flag & PEOPLE_SPECTATOR))
			if (flag & PEOPLE_KILLER)
				a++;
			else if (flag & PEOPLE_POLICE)
				b++;
			else
				c++;
	}
	if (a > 0 && (b == 0 || c == 0))
	{//����ʤ��
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m���˻����ʤ��...\33[m");
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
			if (ppeop[i].style != -1 && ppeop[i].flag & PEOPLE_KILLER && ppeop[i].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "ԭ��\33[32;1m %s \33[m��ɱ��!", ppeop[i].nick);
				send_msg (-1, buf);
			}
	}
	else if (a == 0)
	{//����ʤ��
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m����ɱ�ֶ��������ˣ����˻����ʤ��...\33[m");
		save_result (0);
	}
	else
	{//��Ϸ���ڼ���
		send_msg (-1, "[31;1mȡ��ͶƱ[m");
		gotonight ();
	}
	kill_msg (-1);
	end_change_inroom ();
	return 0;
}

int gotonight ()
{
	int i;
	inrooms[myroom].status = INROOM_NIGHT;
	send_msg (-1, "\33[31;1m�ֲ���ҹɫ�ֽ�����...\33[m");
	send_to_fellow (PEOPLE_KILLER, "��ץ����ı���ʱ����\33[31;1mCtrl+S\33[mɱ��");
	for (i = 0; i < MAX_PEOPLE; ++i)
	{
		inrooms[myroom].peoples[i].vote = 0;
		inrooms[myroom].peoples[i].flag &= ~PEOPLE_VOTED;
	}
	inrooms[myroom].dead = -1;
	inrooms[myroom].test = -1;
	kill_msg (-1);
	return 0;
}

int gotoday ()
{
	int a, b, c, i, j, t;
	char buf [256];
	start_change_inroom ();
	for (i = 0; i < MAX_PEOPLE; ++i)
		if (inrooms[myroom].peoples[i].style != -1 && inrooms[myroom].peoples[i].pid == inrooms[myroom].dead)
		{
			if (inrooms[myroom].killernum != -1)
			{
				inrooms[myroom].peoples[i].flag &= ~PEOPLE_ALIVE;
				if (inrooms[myroom].peoples[i].flag & PEOPLE_KILLER)
					inrooms[myroom].peoples[i].flag |= PEOPLE_HIDE;
			}
			break;
		}
	if (inrooms[myroom].killernum != -1)
	{
		if (i >= MAX_PEOPLE)
			sprintf (buf,"[1;31m���ҵ��ǣ������ߵ����ˡ���[m");
		else
			sprintf (buf, "\33[1;33m��ɭɭ���¹��£�������\33[32;1m %d %s \33[33m��ʬ��...\33[m", i + 1, inrooms[myroom].peoples[i].nick);
		send_msg (-1, buf);
		send_msg (i, "���ӣ���ӭ���������ã��㱻���ֲ��̵�ɱ����...");
	}
	j = i;
	a = b = c = 0;
	for (i = 0; i < MAX_PEOPLE; ++i)
	{
		register int flag = inrooms[myroom].peoples[i].flag;
		if (inrooms[myroom].peoples[i].style != -1 && flag & PEOPLE_ALIVE && !(flag & PEOPLE_SPECTATOR))
			if (flag & PEOPLE_KILLER)
				a++;
			else if (flag & PEOPLE_POLICE)
				b++;
			else
				c++;
	}
	if (a > 0 && (b == 0 || c == 0))
	{
		send_msg (-1, "\33[31;1m���˻����ʤ��...\33[m");
		inrooms[myroom].status = INROOM_STOP;
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
		{
			register int flag = inrooms[myroom].peoples[i].flag;
			if (inrooms[myroom].peoples[i].style != -1 && flag & PEOPLE_KILLER && flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "ԭ��\33[32;1m %d %s \33[m��ɱ��!", i + 1, inrooms[myroom].peoples[i].nick);
				send_msg (-1, buf);
			}
		}
		kill_msg (-1);
		end_change_inroom ();
		return 0;
	}
	else if (a == 0)
	{
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m����ɱ�ֶ��������ˣ����˻����ʤ��...\33[m");
		save_result (0);
		kill_msg (-1);
		end_change_inroom ();
		return 0;
	}
	else
	{
		if (inrooms[myroom].killernum != -1)
			inrooms[myroom].status = INROOM_DAY;
		else
			inrooms[myroom].status = INROOM_CHECK;
		send_msg (-1, "������...");
	}
	for (i = 0; i < MAX_PEOPLE; ++i)
	{
		inrooms[myroom].peoples[i].vote = 0;
		inrooms[myroom].peoples[i].flag &= ~PEOPLE_VOTED;
	}
	if (inrooms[myroom].killernum != -1 && j < MAX_PEOPLE)
	{
		sprintf (buf, "���ڣ�������\33[32;1m %d %s \33[m������ʲô��Ҫ˵��", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "�밴\33[31;1mCtrl+T\33[m��������");
	}
	else if (j < MAX_PEOPLE)
	{
		inrooms[myroom].test = -1;
		send_msg (-1, "\33[31;1m���ڿ�ʼָ֤\33[m");
		sprintf (buf,"����\33[32;1m %d %s \33[m����", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "��[31;1m�����ƶ����[m���������ɵĶ�����");
		send_msg (j, "��\33[31;1mCtrl+S\33[mָ֤");
		send_msg (j, "ָ֤�����\33[31;1mCtrl+T\33[m��������");
		if (inrooms[myroom].peoples[j].flag & PEOPLE_KILLER)
			send_msg (j, "\33[31;1m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ\33[m");
		else
			send_msg (j, "\33[31;1mС�ģ�ɱ���Ѿ��������ˣ���\33[m");
	}
	inrooms[myroom].dead = j;
	if (j >= MAX_PEOPLE)
		j = 0;
	inrooms[myroom].voted[0] = j++;
	t = 1;
	for (i = 1; i < MAX_PEOPLE; ++i)
	{
		if (inrooms[myroom].peoples[j].style != -1 && inrooms[myroom].peoples[j].flag & PEOPLE_ALIVE && !(inrooms[myroom].peoples[j].flag & PEOPLE_SPECTATOR))
		{
			inrooms[myroom].voted [t] = j;
			++t;
		}
		++j;
		if (j >= MAX_PEOPLE)
			j = 0;
	}
	for (j = 0; j < t; ++j)
		if (inrooms[myroom].peoples[inrooms[myroom].voted[j]].style != -1)
			break;
	if (j >= t)
	{
		send_msg (-1, "[1;31m��Ϸ�쳣����[m");
		inrooms[myroom].status = INROOM_STOP;
		kill_msg (-1);
		end_change_inroom ();
		return 0;
	}
	if (inrooms[myroom].dead >= MAX_PEOPLE)
	{
		send_msg (-1, "[1;31m���ڿ�ʼָ֤[m");
		sprintf (buf, "����[1;32m %d %s [m����", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "��[1;31m�����ƶ����[m���������ɵĶ�����");
		send_msg (j, "��[1;31mCtrl+S[mָ֤");
		send_msg (j, "ָ֤�����[1;31mCtrl+T[m��������");
		if (inrooms[myroom].peoples[j].flag & PEOPLE_KILLER)
			send_msg (j, "[1;31m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ");
		inrooms[myroom].status = INROOM_CHECK;
		inrooms[myroom].dead = 0;
		inrooms[myroom].test = -1;
	}
	inrooms[myroom].turn = j;
	inrooms[myroom].numvoted = t;
	if (inrooms[myroom].killernum == -1)
		inrooms[myroom].killernum = 1;
	inrooms[myroom].rotate = 0;
	inrooms[myroom].suicide = -1;
	end_change_inroom ();
	kill_msg (-1);
	return 0;
}

void player_drop (int i)
{
	struct people_struct * ppeop = inrooms[myroom].peoples;
	int turn = inrooms[myroom].turn;
	char buf [256];
	int j, k, t;
	int t1, t2, t3, t4;
	int maxi, max, maxpid;
	int ok;
	if (inrooms[myroom].status == INROOM_STOP)
		return;
	if (!(ppeop[i].flag & PEOPLE_ALIVE))
		return;
	if (ppeop[i].flag & PEOPLE_SPECTATOR)
		return;
	switch (inrooms[myroom].status)
	{
	case INROOM_NIGHT:
		if (ppeop[i].flag & PEOPLE_KILLER)
		{
			int j, t;
			for (t = j = 0; j < MAX_PEOPLE; ++j)
				if (ppeop[j].style != -1 && (ppeop[j].flag & PEOPLE_ALIVE) && !(ppeop[j].flag & PEOPLE_SPECTATOR) && (ppeop[j].flag & PEOPLE_KILLER))
					++t;
			if (t > 0)
				return;
			sprintf (buf, "[1;31mΨһ��ɱ��[1;32m %d %s [1;31m�����ˣ����˻����ʤ����", i + 1, ppeop[i].nick);
			send_msg (-1, buf);
			inrooms[myroom].status = INROOM_STOP;
		}
		else if (ppeop[i].flag & PEOPLE_POLICE)
		{
			int j, t;
			for (t = j = 0; j < MAX_PEOPLE; ++j)
				if (ppeop[j].style != -1 && (ppeop[j].flag & PEOPLE_ALIVE) && !(ppeop[j].flag & PEOPLE_SPECTATOR) && (ppeop[j].flag & PEOPLE_POLICE))
					++t;
			if (t > 0)
				return;
			sprintf (buf, "[1;31mΨһ�ľ���[1;32m %d %s [1;31m�����ˣ����˻����ʤ����", i + 1, ppeop[i].nick);
			send_msg (-1, buf);
			inrooms[myroom].status = INROOM_STOP;
		}
		else
		{
			int j, t;
			for (t = j = 0; j < MAX_PEOPLE; ++j)
				if (ppeop[j].style != -1 && (ppeop[j].flag & PEOPLE_ALIVE) && !(ppeop[j].flag & PEOPLE_SPECTATOR) && !(ppeop[j].flag & PEOPLE_POLICE) && !(ppeop[j].flag & PEOPLE_KILLER))
					++t;
			if (t > 0)
				return;
			sprintf (buf, "[1;31mΨһ��ƽ��[1;32m %d %s [1;31m�����ˣ����˻����ʤ����", i + 1, ppeop[i].nick);
			send_msg (-1, buf);
			inrooms[myroom].status = INROOM_STOP;
		}
		break;
	case INROOM_DAY:
		if (i != inrooms[myroom].voted[turn])
			return;
		sprintf (buf, "[1;31mfaint...[1;35m %d %s [1;31m��û˵��͵�����~~~", i + 1, ppeop[i].nick);
		send_msg (-1, buf);
		++turn;
		while (turn < inrooms[myroom].numvoted)
		{
			int tmp = inrooms[myroom].voted[turn];
			if (ppeop[tmp].style != -1 && ppeop[tmp].flag & PEOPLE_ALIVE && !(ppeop[tmp].flag & PEOPLE_SPECTATOR))
				break;
			++turn;
		}
		if (turn >= inrooms[myroom].numvoted)
		{
			send_msg (-1, "[1;31m��Ϸ�쳣������[m");
			inrooms[myroom].status = INROOM_STOP;
			return;
		}
		inrooms[myroom].turn = turn;
		inrooms[myroom].status = INROOM_CHECK;
		inrooms[myroom].test = -1;
		for (j = 0; j < MAX_PEOPLE; ++j)
			ppeop[j].vote = 0;
		send_msg (-1, "[1;31m���ڿ�ʼָ֤[m");
		sprintf (buf, "����[1;32m %d %s [m����", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
		send_msg (-1, buf);
		send_msg (inrooms[myroom].voted[turn], "��[1;31m�����ƶ����[m���������ɵĶ�����");
		send_msg (inrooms[myroom].voted[turn], "��[1;31mCtrl+S[mָ֤");
		send_msg (inrooms[myroom].voted[turn], "ָ֤�����[1;31mCtrl+T[m��������");
		if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
			send_msg (inrooms[myroom].voted[turn], "[1;31m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ[m");
		break;
	case INROOM_CHECK:
		if (i != inrooms[myroom].voted[turn])
			return;
		++turn;
		sprintf (buf, "[1;32m%d %s[m: [1;31mOver.[m", i + 1, ppeop[i].nick);
		send_msg (-1, buf);
		if (inrooms[myroom].suicide == i)
		{
			sprintf (buf, "[1;31mɱ��[1;32m %d %s [1;31m�����ˡ���[m", i + 1, ppeop[i].nick);
			send_msg (-1, buf);
		}
		while (turn < inrooms[myroom].numvoted)
		{
			int tmp = inrooms[myroom].voted[turn];
			if (ppeop[tmp].style != -1 && ppeop[tmp].flag & PEOPLE_ALIVE && !(ppeop[tmp].flag & PEOPLE_SPECTATOR))
				break;
			++turn;
		}
		inrooms[myroom].turn = turn;
		if (turn >= inrooms[myroom].numvoted)
		{
			int t = 0;
			int j, k;
			if (inrooms[myroom].suicide != -1)
			{
				end_change_inroom ();
				killer_suicided ();
				start_change_inroom ();
				return;
			}
			j = inrooms[myroom].dead;
			if (j >= MAX_PEOPLE)
				j = 0;
			for (k = 0; k < MAX_PEOPLE; ++k)
			{
				if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].vote == 1)
				{
					inrooms[myroom].voted[t] = j;
					++t;
				}
				++j;
				if (j >= MAX_PEOPLE)
					j = 0;
			}
			if (t <= 0)
			{
				send_msg (-1, "[1;31m��Ϸ�쳣������[m");
				inrooms[myroom].status = INROOM_STOP;
				return;
			}
			inrooms[myroom].numvoted = t;
			inrooms[myroom].turn = 0;
			inrooms[myroom].status = INROOM_DEFENCE;
			for (k = 0; k < MAX_PEOPLE; ++k)
				ppeop[k].vote = 0;
			send_msg (-1, "[1;31mָ֤����[m");
			send_msg (-1, "[1;31m�����뱻ָ֤�Ĺ�����[m");
			sprintf (buf, "������[1;32m %d %s [mΪ�Լ��绤", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[0], "�绤�������[1;31mCtrl+T[m��������");
			if (ppeop[inrooms[myroom].voted[0]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[0], "[1;31m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ[m");
		}
		else
		{
			sprintf (buf, "������[1;32m %d %s [m����", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[turn], "��[1;31m�����ƶ����[m���������ɵĶ�����");
			send_msg (inrooms[myroom].voted[turn], "��[1;31mCtrl+S[mָ֤");
			send_msg (inrooms[myroom].voted[turn], "ָ֤�����[1;31mCtrl+T[m��������");
			if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[turn], "[1;31m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ[m");
			inrooms[myroom].test = -1;
		}
		break;
	case INROOM_DEFENCE:
		if (i != inrooms[myroom].voted[turn])
			return;
		++turn;
		sprintf (buf, "[1;32m%d %s[m: [1;31mOver.[m", i + 1, ppeop[i].nick);
		send_msg (-1, buf);
		if (inrooms[myroom].suicide == i)
		{
			sprintf (buf, "[1;31mɱ��[1;32m %d %s [1;31m�����ˡ���[m", i + 1, ppeop[i].nick);
			send_msg (-1, buf);
		}
		while (turn < inrooms[myroom].numvoted)
		{
			int tmp = inrooms[myroom].voted[turn];
			if (ppeop[tmp].style != -1 && ppeop[tmp].flag & PEOPLE_ALIVE && !(ppeop[tmp].flag & PEOPLE_SPECTATOR))
				break;
			++turn;
		}
		inrooms[myroom].turn = turn;
		if (turn >= inrooms[myroom].numvoted)
		{
			int j;
			if (inrooms[myroom].suicide != -1)
			{
				end_change_inroom ();
				killer_suicided ();
				start_change_inroom ();
				return;
			}
			send_msg (-1, "[1;31m�绤���������ڿ�ʼͶƱ[m");
			send_msg (-1, "[1;31m�������ƶ����ѡ�������Ctrl+SͶƱ...[m");
			for (j = 0; j < MAX_PEOPLE; ++j)
				ppeop[j].vote = 0;
			inrooms[myroom].status = INROOM_VOTE;
		}
		else
		{
			sprintf (buf, "������[1;31m %d %s [mΪ�Լ��绤", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[turn], "�绤����밴[1;31mCtrl+T[m��������");
			if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[turn], "[1;31m�ѹ���Ƶ��Լ�λ���ϣ���Ctrl+S����ɱ[m");
		}
		break;
	case INROOM_VOTE:
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].vote == 0)
				break;
		if (j < MAX_PEOPLE)
			return;
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR))
			{
				int tmp = ppeop[j].vote - 1;
				sprintf (buf, "[1;32m%d %sͶ��%d %sһƱ[m", j + 1, ppeop[j].nick, tmp + 1, ppeop[tmp].nick);
				ppeop[j].vote = ppeop[tmp].pid;
				send_msg (-1, buf);
			}
		kill_msg (-1);
		inrooms[myroom].rotate++;
		t1 = t2 = t3 = t4 = 0;
		for (j = 0; j < MAX_PEOPLE; ++j)
			ppeop[j].vnum = 0;
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1)
				if (!(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].flag & PEOPLE_ALIVE)
				{
					for (k = 0; k < MAX_PEOPLE; ++k)
						if (ppeop[k].style != -1 && ppeop[k].pid == ppeop[j].vote)
						{
							t3++;
							ppeop[k].vnum++;
						}
				}
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1)
				if (!(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].flag & PEOPLE_ALIVE)
				{
					if (ppeop[j].vnum > t1)
					{
						t2 = t1;
						t1 = ppeop[j].vnum;
						maxi = j;
						max = t2;
						maxpid = ppeop[j].pid;
					}
					else if (ppeop[j].vnum > t2 && ppeop[j].vnum != t1)
					{
						t2 = ppeop[j].vnum;
					}
				}
		if (t2 == 0)
			t2 = t1;
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1 && !(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].flag & PEOPLE_ALIVE && ppeop[j].vnum == t1)
				t4++;
		if (t4 >= 2)
			t2 = t1;
		sprintf (buf, "[1;31m�� %d ��ͶƱ���:[m", inrooms[myroom].rotate);
		send_msg (-1, buf);
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1 && !(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].flag & PEOPLE_VOTED && ppeop[j].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "%s��Ͷ: %d Ʊ", ppeop[j].nick, ppeop[j].vnum);
				send_msg (-1, buf);
			}
		if (t1 > t3/2)
			ok = 1;
		else
			ok = 0;
		if (!ok && inrooms[myroom].rotate < 3)
		{
			int t;
			t = 0;
			if (inrooms[myroom].rotate == 1)
			{
				j = inrooms[myroom].dead;
				if (j < 0)
					j = MAX_PEOPLE - 1;
				for (k = 0; k < MAX_PEOPLE; ++k)
				{
					if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR) && (ppeop[j].vnum == t1 || ppeop[j].vnum == t2) && ppeop[j].flag & PEOPLE_VOTED)
					{
						inrooms[myroom].voted[t] = j;
						++t;
					}
					else
					{
						ppeop[j].flag &= ~PEOPLE_VOTED;
					}
					--j;
					if (j < 0)
						j = MAX_PEOPLE - 1;
				}
			}
			else
			{
				j = inrooms[myroom].dead;
				if (j >= MAX_PEOPLE)
					j = 0;
				for (k = 0; k < MAX_PEOPLE; ++k)
				{
					if (ppeop[j].style != -1 && ppeop[j].flag & PEOPLE_ALIVE && !(ppeop[j].flag & PEOPLE_SPECTATOR) && (ppeop[j].vnum == t1 || ppeop[j].vnum == t2) && ppeop[j].flag & PEOPLE_VOTED)
					{
						inrooms[myroom].voted[t] = j;
						++t;
					}
					else
					{
						ppeop[j].flag &= ~PEOPLE_VOTED;
					}
					++j;
					if (j >= MAX_PEOPLE)
						j = 0;
				}
			}
			if (t <= 0)
			{
				send_msg (-1, "[1;31m��Ϸ�쳣����[m");
				inrooms[myroom].status = INROOM_STOP;
				kill_msg (-1);
				return;
			}
			inrooms[myroom].numvoted = t;
			inrooms[myroom].turn = 0;
			inrooms[myroom].test = -1;
			inrooms[myroom].status = INROOM_DEFENCE;
			for (j = 0; j < MAX_PEOPLE; ++j)
				ppeop[j].vote = 0;
			send_msg (-1, "[1;31m������Ʊ���Ĺ�����Ϊ�Լ��绤[m");
			sprintf (buf, "������[1;32m %d %s [m����", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[0], "�绤����밴[1;31mCtrl+T[m��������");
		}
		else
		{
			int a = 0, b = 0;
			if (!ok && inrooms[myroom].rotate >= 3)
			{
				send_msg (-1, "[1;31mͶƱ������δ�н���������������[m");
				b = rand () % inrooms[myroom].numvoted;
				for (k = 0; k < inrooms[myroom].numvoted;++k)
				{
					a = inrooms[myroom].voted[b];
					if (ppeop[a].style != -1 && ppeop[a].flag & PEOPLE_ALIVE && !(ppeop[a].flag & PEOPLE_SPECTATOR) && ppeop[a].flag & PEOPLE_VOTED)
						break;
					b++;
					if (b >= inrooms[myroom].numvoted)
						b = 0;
				}
				if (k >= inrooms[myroom].numvoted)
				{
					send_msg (-1, "[1;31m��Ϸ�쳣����[m");
					inrooms[myroom].status = INROOM_STOP;
					kill_msg (-1);
					return;
				}
				max = ppeop[a].vnum;
				maxi = a;
				maxpid = ppeop[a].pid;
			}
			send_msg (maxi, "[1;31m�㱻���ٴ�����![m");
			sprintf (buf, "[1;32m %d %s [1;31m�����ٴ�����[m", maxi + 1, ppeop[maxi].nick);
			for (j = 0; j < MAX_PEOPLE; ++j)
				if (ppeop[j].style != -1 && j != maxi)
					send_msg (j, buf);
			gotodark (maxpid);
		}
		break;
	case INROOM_DARK:
		if (i != inrooms[myroom].turn)
			return;
		sprintf (buf, "[1;32m%d %s[m: [1;31mOver.[m", i + 1, ppeop[i].nick);
		send_msg (-1, buf);
		send_msg (-1, "[1;31m����...[m");
		gotonight ();
		break;
	}
}
