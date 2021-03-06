/******************************************************
杀人游戏2003, 作者: bad@smth.org  Qian Wenjie
在水木清华bbs系统上运行
 
本游戏是自由软件，请随意复制移植
请在修改后的文件头部保留版权信息
******************************************************/
/******************************************************
 * 杀人游戏 2003 for FB 2000 作者: TurboZV@UESTC Zhang Wei
 *
 * 我已经将smth的杀人游戏移植到FB2000系统上，
 * 谢谢bad@smth为此游戏的付出
 * 
 *本游戏是自由软件，请随意复制移植
 *请在修改后的文件头部保留版权信息
 *				    2003-5-12
******************************************************/
/* ChangeLog 030515
 *  1) 正确的处理了颜色的显示
 *  2) 加入表情功能(*)
 *  3) 投票用下划线表示(*)
 *  4) 加了一个排行榜功能(*)
 *  5) 对警察的功能作了一些修正(*)
 *  6) 警察只能用Ctrl+T试探坏人 (*)
 * 打*为bad@smth所做的修改
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
//旁观者
#define PEOPLE_SPECTATOR 01
//杀手
#define PEOPLE_KILLER 02
//
#define PEOPLE_ALIVE 04
//
#define PEOPLE_ROOMOP 010
//警察
#define PEOPLE_POLICE 020
//what's this???
#define PEOPLE_TESTED 040
#define PEOPLE_VOTED  0100
#define PEOPLE_HIDE   0200

struct people_struct
{
	int style; // Efan: style == -1 即被踢
	char id[IDLEN+2];
	char nick[NAMELEN];
	int flag;
	int pid;
	int vote;
	int vnum;
};

#define INROOM_STOP 1
//晚上
#define INROOM_NIGHT 2
//白天
#define INROOM_DAY 3
//指证阶段
#define INROOM_CHECK 4
//辩护阶段
#define INROOM_DEFENCE 5
//投票阶段
#define INROOM_VOTE 6
//死者留遗言
#define INROOM_DARK 7


struct inroom_struct
{
	int w;
	int status;
	int killernum;
	int policenum;
	/* 尚未加入的代码 */
	int turn; //轮到某人发言
	int rotate; //第几轮投票
	int dead; //该轮被杀的人
	int test; //警察验证的人
	int suicide; //杀手崩溃
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
	int w; //0 - 平民胜利 1 - 杀手胜利
	time_t t;
	int peoplet;
	char id[MAX_PEOPLE][IDLEN+2];
	int st[MAX_PEOPLE]; // 0 - 活着平民 1 - 死了平民 2 - 活着杀手 3 - 死了杀手 4 - 其他情况
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
                if(!strncmp(buf, "你被踢了", 8)) kicked=1;
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
	char *verb;                 /* 动词 */
	char *part1_msg;            /* 介词 */
	char *part2_msg;            /* 动作 */
};

struct action party_data[] =
    {
	    {"?", "很疑惑的看着", ""
	    },
	    {"admire", "对", "的景仰之情犹如滔滔江水连绵不绝"},
	    {"agree", "完全同意", "的看法"},
	    {"bearhug", "热情的拥抱", ""},
	    {"bless", "祝福", "心想事成"},
	    {"bow", "毕躬毕敬的向", "鞠躬"},
	    {"bye", "看着", "的背影，凄然泪下。身后的收音机传来邓丽君的歌声:\n\"[31m何日君再来.....[m\""},
	    {"caress", "轻轻的抚摸", ""},
	    {"cat", "像只小猫般地依偎在", "的怀里撒娇。"},
	    {"cringe", "向", "卑躬屈膝，摇尾乞怜"},
	    {"cry", "向", "嚎啕大哭"},
	    {"comfort", "温言安慰", ""},
	    {"clap", "向", "热烈鼓掌"},
	    {"dance", "拉着", "的手翩翩起舞"},
	    {"dogleg", "对", "狗腿"},
	    {"drivel", "对著", "流口水"},
	    {"dunno", "瞪大眼睛，天真地问：", "，你说什麽我不懂耶... :("},
	    {"faint", "晕倒在", "的怀里"},
	    {"fear", "对", "露出怕怕的表情"},
	    {"fool", "请大家注意", "这个大白痴....\n真是天上少有....人间仅存的活宝....\n不看太可惜了！"},
	    {"forgive", "大度的对", "说：算了，原谅你了"},
	    {"giggle", "对著", "傻傻的呆笑"},
	    {"grin", "对", "露出邪恶的笑容"},
	    {"growl", "对", "咆哮不已"},
	    {"hand", "跟", "握手"},
	    {"hammer", "举起好大好大的铁锤！！哇！往",
	     "头上用力一敲！\n***************\n*  5000000 Pt *\n***************\n      | |      ★☆★☆★☆\n      | |         好多的星星哟\n      |_|"},
	    {"heng", "看都不看", "一眼， 哼了一声，高高的把头扬起来了,不屑一顾的样子..."},
	    {"hug", "轻轻地拥抱", ""},
	    {"idiot", "无情地耻笑", "的痴呆。"},
	    {"kick", "把", "踢的死去活来"},
	    {"kiss", "轻吻", "的脸颊"},
	    {"laugh", "大声嘲笑", ""},
	    {"lovelook", "拉着", "的手，温柔地默默对视。目光中孕含着千种柔情，万般蜜意"},
	    {"nod", "向", "点头称是"},
	    {"nudge", "用手肘顶", "的肥肚子"},
	    {"oh", "对", "说：“哦，酱子啊！”"},
	    {"pad", "轻拍", "的肩膀"},
	    {"papaya", "敲了敲", "的木瓜脑袋"},
	    {"pat", "轻轻拍拍", "的头"},
	    {"pinch", "用力的把", "拧的黑青"},
	    {"puke", "对着", "吐啊吐啊，据说吐多几次就习惯了"},
	    {"punch", "狠狠揍了", "一顿"},
	    {"pure", "对", "露出纯真的笑容"},
	    {"qmarry", "向", "勇敢的跪了下来:\n\"你愿意嫁给我吗？\"\n---真是勇气可嘉啊"},
	    {"report", "偷偷地对", "说：“报告我好吗？”"},
	    {"shrug", "无奈地向", "耸了耸肩膀"},
	    {"sigh", "对", "叹了一口气"},
	    {"slap", "啪啪的巴了", "一顿耳光"},
	    {"smooch", "拥吻著", ""},
	    {"snicker", "嘿嘿嘿..的对", "窃笑"},
	    {"sniff", "对", "嗤之以鼻"},
	    {"sorry", "痛哭流涕的请求", "原谅"},
	    {"spank", "用巴掌打", "的臀部"},
	    {"squeeze", "紧紧地拥抱著", ""},
	    {"thank", "向", "道谢"},
	    {"tickle", "咕叽!咕叽!搔", "的痒"},
	    {"waiting", "深情地对", "说：每年每月的每一天，每分每秒我都在这里等着你"},
	    {"wake", "努力的摇摇", "，在其耳边大叫：“快醒醒，会着凉的！”"},
	    {"wave", "对著", "拼命的摇手"},
	    {"welcome", "热烈欢迎", "的到来"},
	    {"wink", "对", "神秘的眨眨眼睛"},
	    {"xixi", "嘻嘻地对", "笑了几声"},
	    {"zap", "对", "疯狂的攻击"},
	    {"inn", "双眼饱含着泪水，无辜的望着", ""},
	    {"mm", "色眯眯的对", "问好：“美眉好～～～～”。大色狼啊！！！"},
	    {"disapp", "这下没搞头啦，为什么", "姑娘对我这个造型完全没反应？没办法！"},
	    {"miss", "真诚的望着", "：我想念你我真的想念你我太--想念你了!你相不相信?"},
	    {"buypig", "指着", "：“这个猪头给我切一半，谢谢！”"},
	    {"rascal", "对", "大叫：“你这个臭流氓！！！！！！！！！！！！！！！！”"},
	    {"qifu", "小嘴一扁，对", "哭道：“你欺负我，你欺负我！！！”"},
	    {"wa", "对", "大叫一声：“哇哇哇哇哇哇酷弊了耶！！！！！！！！！！！！！！”"},
	    {"feibang", "喔－－！熟归熟，", "你这样乱讲话，我一样可以告你毁谤，哈！"},
	    {"badman", "指着", "的脑袋, 手舞足蹈的喊:“ 杀~~手~~在~~这~~里~~~”"},
	    {NULL, NULL, NULL}
    };

struct action speak_data[] =
    {
	    {"ask", "询问", NULL
	    },
	    {"chant", "歌颂", NULL},
	    {"cheer", "喝采", NULL},
	    {"chuckle", "轻笑", NULL},
	    {"curse", "咒骂", NULL},
	    {"demand", "要求", NULL},
	    {"frown", "蹙眉", NULL},
	    {"groan", "呻吟", NULL},
	    {"grumble", "发牢骚", NULL},
	    {"hum", "喃喃自语", NULL},
	    {"moan", "悲叹", NULL},
	    {"notice", "注意", NULL},
	    {"order", "命令", NULL},
	    {"ponder", "沈思", NULL},
	    {"pout", "噘著嘴说", NULL},
	    {"pray", "祈祷", NULL},
	    {"request", "恳求", NULL},
	    {"shout", "大叫", NULL},
	    {"sing", "唱歌", NULL},
	    {"smile", "微笑", NULL},
	    {"smirk", "假笑", NULL},
	    {"swear", "发誓", NULL},
	    {"tease", "嘲笑", NULL},
	    {"whimper", "呜咽的说", NULL},
	    {"yawn", "哈欠连天", NULL},
	    {"yell", "大喊", NULL},
	    {NULL, NULL, NULL}
    };

struct action condition_data[] =
    {
	    {":D", "乐的合不拢嘴", NULL
	    },
	    {":)", "乐的合不拢嘴", NULL},
	    {":P", "乐的合不拢嘴", NULL},
	    {":(", "乐的合不拢嘴", NULL},
	    {"applaud", "啪啪啪啪啪啪啪....", NULL},
	    {"blush", "脸都红了", NULL},
	    {"cough", "咳了几声", NULL},
	    {"faint", "咣当一声，晕倒在地", NULL},
	    {"happy", "的脸上露出了幸福的表情，并学吃饱了的猪哼哼了起来", NULL},
	    {"lonely", "一个人坐在房间里，百无聊赖，希望谁来陪陪。。。。", NULL},
	    {"luck", "哇！福气啦！", NULL},
	    {"puke", "真恶心，我听了都想吐", NULL},
	    {"shake", "摇了摇头", NULL},
	    {"sleep", "Zzzzzzzzzz，真无聊，都快睡著了", NULL},
	    {"so", "就酱子!!", NULL},
	    {"strut", "大摇大摆地走", NULL},
	    {"tongue", "吐了吐舌头", NULL},
	    {"think", "歪著头想了一下", NULL},
	    {"wawl", "惊天动地的哭", NULL},
	    {"goodman", "用及其无辜的表情看着大家: “我真的是好人耶~~”", NULL},
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
					sprintf(buf, "%s掉线了", inrooms[myroom].peoples[i].nick);
					send_msg(-1, buf);
					start_change_inroom();
					inrooms[myroom].peoples[i].style=-1;
					/* Efan: 以后再说~~~ */
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
					/* Efan: 暂时设为一人掉线即结束游戏 */
					/*
					if (inrooms[myroom].status != INROOM_STOP && inrooms[myroom].peoples[i].flag & PEOPLE_ALIVE && !(inrooms[myroom].peoples[i].flag & PEOPLE_SPECTATOR)) {
					inrooms[myroom].status = INROOM_STOP;
					send_msg (-1, "\33[31;1m游戏异常结束\33[m");
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
								sprintf(buf, "%s成为新房主", inrooms[myroom].peoples[k].nick);
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
	("[44;33;1m 房间:[36m%-12s[33m话题:[36m%-40s[33m状态:[36m%6s[m",
	 rooms[myroom].name, rooms[myroom].title,
	 (inrooms[myroom].status ==
	  INROOM_STOP ? "未开始" : (inrooms[myroom].status ==
	                            INROOM_NIGHT ? "黑夜中" : "大白天")));
	clrtoeol();
	k_resetcolor();
	k_setfcolor(YELLOW, 1);
	move(1, 0);
	prints("[1;33m");
	prints
	("╭[1;32m玩家[33m—————╮╭[1;32m讯息[33m———————————————————————————╮");
	move(t_lines - 2, 0);
	prints
	("╰———————╯╰—————————————————————————————╯[m");

	me = mypos;
	msgst = get_msgt();

	for (i = 2; i <= t_lines - 3; i++)
	{
		strcpy(disp, "│");

		// 要显示用户
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
					/* Efan: 警察不应该看到坏人投票
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
		strcat(disp, "[1;33m││[m");

		// 处理显示的消息
		if (msgst - 1 - (t_lines - 3 - i) - jpage >= 0)
		{
			char *ss = get_msgs(msgst - 1 - (t_lines - 3 - i) - jpage);
			if (!strcmp(ss, "你被踢了"))
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
		strcat(disp, "[1;33m│");

		// 显示
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
		send_msg(me, "[31;1m至少3人参加才能开始游戏[m");
		end_change_inroom();
		refreshit();
		return;
	}
	if(totalk==0)
		totalk=((double)total/6+0.5);
	/* Efan: 杀手和警察数目由玩家自由设定，程序不再强迫
	    if(totalk>=total/4) totalk=total/4;
	    if(totalc>=total/6) totalc=total/6;
	    if(totalk>total) {
	        send_msg(me, "[31;1m总人数少于要求的坏人人数,无法开始游戏[m");
	        end_change_inroom();
	        refreshit();
	        return;
	    }
	*/
	/* Efan: 但是应该保证至少有一个是平民~~ */
	if (totalk + totalc > total - 1)
	{
		send_msg (me, "\33[31;1m没有平民能玩么？请重新设定杀手和警察数目\33[m");
		end_change_inroom ();
		refreshit ();
		return;
	}
	/* Efan: 也要保证有一个杀手…… */
	if (totalk == 0)
	{
		send_msg (me, "[31;1m没有杀手怎么杀人哇~~~请重新设定杀手数目[m");
		end_change_inroom ();
		refreshit ();
		return;
	}

	if(totalc==0)
		sprintf(buf, "[31;1m游戏开始啦! 人群中出现了%d个坏人[m", totalk);
	else
		sprintf(buf, "[31;1m游戏开始啦! 人群中出现了%d个坏人, %d个警察[m", totalk, totalc);
	send_msg(-1, buf);
	sprintf (buf, "[31;1m现在玩的是 %d(p)-%d(k)-%d(c) .[m", totalc, totalk, total - totalc - totalk);
	send_msg (-1, buf);
	for(i=0;i<totalk;i++)
	{
		do
		{
			j=rand()%MAX_PEOPLE;
		}
		while(inrooms[myroom].peoples[j].style==-1 || inrooms[myroom].peoples[j].flag&PEOPLE_KILLER || inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR);
		inrooms[myroom].peoples[j].flag |= PEOPLE_KILLER;
		send_msg(j, "你做了一个无耻的坏人");
		send_msg(j,"[31;1m上下移动光标[m选择对象");
		if (totalk != 1)
		{
			send_msg(j,"用你的尖刀([31;1mCtrl+S[m)选择要残害的人吧...");
		}
		else
		{
			send_msg (j, "用你的尖刀([31;1mCtrl+S[m)选择你想残害的人吧...");
			send_msg (j, "[31;1m此局暂不可杀人……[m");
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
		send_msg(j, "你做了一位光荣的人民警察");
		send_msg(j, "请[31;1m上下移动光标[m");
		send_msg(j, "用你的警棒([31;1mCtrl+S[m)查看你怀疑的人...");
		/*
		if (totalk == 1 && totalc == 1)
			send_msg (j, "不过要到[31;1m下一回合[m开始才可以~~~");
			*/
	}
	for(i=0;i<MAX_PEOPLE;i++)
		if(inrooms[myroom].peoples[i].style!=-1)
			if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR))
			{
				inrooms[myroom].peoples[i].flag |= PEOPLE_ALIVE;
				if(!(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER))
					send_msg(i, "现在是晚上...");
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
	    { "0-返回", "1-退出游戏", "2-改名字", "3-玩家列表", "4-改话题",
	      "5-设置房间", "6-踢玩家", "7-开始游戏"
	    };
	int i, j, k, sel = 0, ch, max = 0, me;
	char buf[80], disp[128];
	if (inrooms[myroom].status != INROOM_STOP)
		strcpy(menus[7], "7-结束游戏");
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
					send_msg(me, "你还在游戏,不能退出");
					refreshit();
					return 0;
				}
				return 1;
			case 2:
				if (inrooms[myroom].status != INROOM_STOP)
				{
					send_msg (me, "游戏中，不能改ID");
					refreshit ();
					return 0;
				}
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "请输入名字:", buf, 13, 1, 0, 1);
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
						prints(" 名字不符合规范");
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
						prints(" 已有人用这个名字了");
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
				k_getdata(t_lines-1, 0, "请输入话题:", buf, 31, 1, 0, 1);
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
				k_getdata(t_lines-1, 0, "请输入房间最大人数:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					if(i>0&&i<=100)
					{
						rooms[myroom].maxpeople = i;
						sprintf(buf, "屋主设置房间最大人数为%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "设置为隐藏房间? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_SECRET;
					else
						rooms[myroom].flag&=~ROOM_SECRET;
					sprintf(buf, "屋主设置房间为%s", (buf[0]=='Y')?"隐藏":"不隐藏");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "设置为锁定房间? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_LOCKED;
					else
						rooms[myroom].flag&=~ROOM_LOCKED;
					sprintf(buf, "屋主设置房间为%s", (buf[0]=='Y')?"锁定":"不锁定");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "设置为拒绝旁观者的房间? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_DENYSPEC;
					else
						rooms[myroom].flag&=~ROOM_DENYSPEC;
					sprintf(buf, "屋主设置房间为%s", (buf[0]=='Y')?"拒绝旁观":"不拒绝旁观");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "设置坏人的数目:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					/* Efan: 杀手数限制在十人以内还情有可原 */
					if(i>=0&&i<=10)
					{
						inrooms[myroom].killernum = i;
						sprintf(buf, "屋主设置房间坏人数为%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "设置警察的数目:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					/* Efan: 警察数居然限制在二人以内~~~清华的杀人太怪异了~~~
					                        if(i>=0&&i<=2) {
					                            inrooms[myroom].policenum = i;
					                            sprintf(buf, "屋主设置房间警察数为%d", i);
					                            send_msg(-1, buf);
					                        }
					*/
					if (i >=0 && i <= 10)
					{
						inrooms [myroom].policenum = i;
						sprintf (buf, "屋主设置房间警察数为%d", i);
						send_msg (-1, buf);
					}
				}
				kill_msg(-1);
				return 0;
			case 6:
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "请输入要踢的id:", buf, 30, 1, 0, 1);
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
						send_msg(i, "你被踢了");
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
					send_msg(-1, "游戏被屋主强制结束");
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
	/*    sprintf(buf, "%s进入房间", currentuser.userid);
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
			ch=-k_getdata(t_lines-1, 0, "输入:", buf, 70, 1, NULL, 1);
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
			/* Efan: 南开版杀人，警察在晚上验证杀手 */
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

				/* Efan: 辩护时自杀 */
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
					send_msg (me, "[31;1m你崩溃了……[m");
					kill_msg (-1);
					start_change_inroom ();
					inrooms[myroom].suicide = me;
					end_change_inroom ();
					continue;
				}
				/* Efan: 指证 */
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
						send_msg (me, "[31;1m你崩溃了……[m");
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
				/* Efan: 杀手活动 */
				if(inrooms[myroom].status==INROOM_NIGHT && inrooms[myroom].peoples[me].flag & PEOPLE_KILLER)
				{
					if (inrooms[myroom].peoples[me].style == -1 || inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR || !(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER) || !(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE))
						continue;
					if (inrooms[myroom].peoples[sel].flag & PEOPLE_SPECTATOR)
					{
						send_msg (me, "\33[31;1m这是旁观者\33[m");
						refreshit ();
						continue;
					}
					else if (!(inrooms[myroom].peoples[sel].flag & PEOPLE_ALIVE))
					{
						send_msg (me, "\33[31;1m此人已死\33[m");
						refreshit ();
						continue;
					}
					if (inrooms[myroom].dead != -1)
						if (inrooms[myroom].killernum != -1)
							send_to_fellow (PEOPLE_KILLER, "\33[31;1m-_-!!太不敬业了，哪有一晚上杀两个人的……\33[m");
						else
							send_to_fellow (PEOPLE_KILLER, "[31;1m-_-!!太不敬业了，等明晚吧![m");
					else
					{
						//int i;
						inrooms[myroom].dead = pid;
						if (inrooms[myroom].killernum != -1)
							sprintf (buf, "好，决定了，就杀\33[32;1m %s \33[m!", inrooms[myroom].peoples[sel].nick);
						else
							sprintf (buf, "好，先吓[32;1m %s [m一吓吧!", inrooms[myroom].peoples[sel].nick);
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
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m本轮警察已经侦查过了\33[m");
						refreshit ();
						kill_msg (-1);
						continue;
					}
					if(inrooms[myroom].peoples[sel].flag&PEOPLE_SPECTATOR)
						send_to_fellow(PEOPLE_POLICE, "[31;1m此人是旁观者[m");
					else if(!(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE))
						send_to_fellow(PEOPLE_POLICE, "[31;1m此人已死[m");
					else if (inrooms[myroom].peoples[sel].flag&PEOPLE_POLICE)
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m这是自己人~~~\33[m");
					else if((inrooms[myroom].peoples[sel].flag&PEOPLE_KILLER))
					{
						inrooms[myroom].test = sel;
						sprintf (buf,"那忧郁的眼神、唏嘘的须根、神乎其技的刀法，早已把他出卖...没错，\33[32;1m%s \33[m就是一位出色的杀手!", inrooms[myroom].peoples[sel].nick);
						send_to_fellow (PEOPLE_POLICE, buf);
					}
					else
					{
						/* Efan: 南开版 */
						sprintf (buf, "调查\33[32;1m %s \33[m的身份", inrooms[myroom].peoples[sel].nick);
						send_to_fellow(PEOPLE_POLICE, buf);
						inrooms[myroom].test = sel;
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m笨笨，猜错了啦，这明明是好人嘛~~~\33[m");
					}
					kill_msg (-1);
					if (inrooms[myroom].test != -1 && inrooms[myroom].dead != -1)
					{
						/* Efan: 南开版，是时候转入白天了 */
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
							sprintf(buf, "[32;1m投%d %s一票[m", sel+1, inrooms[myroom].peoples[sel].nick);
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
									sprintf (buf, "[32;1m%d %s投了%d %s一票[m", i + 1, inrooms[myroom].peoples[i].nick, tmp + 1, inrooms[myroom].peoples[tmp].nick);
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
							/* Efan: 以下代码用于找出前两名的票数，不是我写的 */
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

							sprintf(buf, "\33[31;1m第 %d 轮投票结果:\33[m", inrooms[myroom].rotate);
							send_msg(-1, buf);
							for(i=0;i<MAX_PEOPLE;i++)
								if(inrooms[myroom].peoples[i].style!=-1)
									if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) && inrooms[myroom].peoples[i].flag & PEOPLE_VOTED &&
									        inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
									{
										sprintf(buf, "%s被投: %d 票",
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
							/* Efan: 必须过半数，方算有效 */
							if (t1 > t3/2)
								ok = 1;
							else
								ok = 0;
							if(!ok && inrooms[myroom].rotate < 3)
							{
								int t;
								struct people_struct * ppeop;
								send_msg(-1, "\33[31m最高票数未超过半数,请重新投票...\33[m");
								start_change_inroom();
								t = 0;
								ppeop = inrooms[myroom].peoples;
								if (inrooms[myroom].rotate == 1)
								{//逆时针
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
								{//inrooms[myroom].rotate == 2 顺时针
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
									send_msg (-1, "\33[1;31m游戏异常结束\33[m");
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
								send_msg (-1, "\33[31;1m获得最多票数的公民请为自己辩护\33[m");
								sprintf (buf, "首先是\33[32;1m %d %s \33[m发言", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
								send_msg (-1, buf);
								send_msg (inrooms[myroom].voted[0], "辩护完后，请按\33[31;1mCtrl+T\33[m结束发言");
							}
							else
							{
								int a=0,b=0;
								int k;
								struct people_struct * ppeop = inrooms[myroom].peoples;
								/* Efan: 投票三次仍无结果，法官随机抽取一人 */
								if (!ok && inrooms[myroom].rotate >= 3)
								{
									send_msg (-1, "\33[31;1m投票三次仍未有结果，法官随机处决\33[m");
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
										send_msg (-1, "[1;31m游戏异常结束[m");
										inrooms[myroom].status = INROOM_STOP;
										end_change_inroom ();
										kill_msg (-1);
										continue;
									}
									max = ppeop[a].vnum;
									maxi = a;
									maxpid = ppeop[a].pid;
								}
								send_msg(maxi, "\33[31;1m你被法官处决了!\33[m");
								sprintf (buf, "\33[32;1m %d %s \33[31;1m被法官处决了\33[m", maxi + 1, ppeop[maxi].nick);
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
						                      send_msg(me, "[31;1m你不能选择自杀[m");
						                  else if(!(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE))
						                      send_msg(me, "[31;1m此人已死[m");
						                  else
						                      send_msg(me, "[31;1m此人是旁观者[m");
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

				/* Efan: 下面代码完成刚转入白天时，死者发言的功能 */
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
					sprintf (buf, "那么，请\33[35;1m %d %s \33[m朋友以旁观者的身份，继续游戏...", me + 1, ppeop [me].nick);
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
						send_msg (-1, "\33[31;1m游戏异常结束!\33[m");
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
					send_msg (-1, "\33[31;1m现在开始指证\33[m");
					sprintf (buf,"先请\33[32;1m %d %s \33[m发言", inrooms[myroom].voted[turn] + 1, ppeop [inrooms[myroom].voted[turn]].nick);
					send_msg (-1, buf);
					send_msg (inrooms[myroom].voted[turn], "请[31;1m上下移动光标[m，在您怀疑的对象上");
					send_msg (inrooms[myroom].voted[turn], "按\33[31;1mCtrl+S\33[m指证");
					send_msg (inrooms[myroom].voted[turn], "指证完后，用\33[31;1mCtrl+T\33[m结束发言");
					if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
						send_msg (inrooms[myroom].voted[turn], "\33[31;1m把光标移到自己位置上，用Ctrl+S可自杀\33[m");
					kill_msg (-1);
					continue;
				}
				/* Efan: 指证 */
				if (inrooms[myroom].status == INROOM_CHECK)
				{
					struct people_struct * ppeop = inrooms[myroom].peoples;
					int turn = inrooms[myroom].turn;
					if (me != inrooms[myroom].voted[turn])
						continue;
					if (inrooms[myroom].test == -1)
					{
						send_msg (me, "请先指证一个人");
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
						sprintf (buf, "\33[32;1m%d %s 指证 %d %s\33[m", me+1, ppeop[me].nick, inrooms[myroom].test+1, ppeop[inrooms[myroom].test].nick);
					else
						sprintf (buf, "\33[31;1m杀手 %d %s 崩溃了……\33[m", me + 1, ppeop[me].nick);
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
							send_msg (-1, "\33[31;1m游戏异常结束!\33[m");
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
						send_msg (-1, "\33[31;1m指证结束\33[m");
						send_msg (-1, "\33[31;1m下面请被指证的公民发言\33[m");
						sprintf (buf, "首先是\33[32;1m %d %s \33[m为自己辩护", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[0], "辩护完后，请按\33[31;1mCtrl+T\33[m结束发言");
						if (ppeop[inrooms[myroom].voted[0]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[0], "\33[31;1m把光标移到自己位置上，用Ctrl+S可自杀\33[m");
					}
					else
					{
						sprintf (buf, "下面是\33[32;1m %d %s \33[m发言", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[turn], "请[31;1m上下移动光标[m，在您怀疑的对象上");
						send_msg (inrooms[myroom].voted[turn], "按\33[31;1mCtrl+S\33[m指证");
						send_msg (inrooms[myroom].voted[turn], "指证完后，用\33[31;1mCtrl+T\33[m结束发言");
						if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[turn], "\33[31;1m把光标移到自己位置上，用Ctrl+S可自杀\33[m");
						inrooms[myroom].test = -1;
					}
					end_change_inroom ();
					kill_msg (-1);
					continue;
				}
				/* Efan: 辩护 */
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
						sprintf (buf, "[31;1m杀手 %d %s 崩溃了……[m", me + 1, inrooms[myroom].peoples[me].nick);
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
						send_msg (-1, "\33[31;1m辩护结束，现在开始投票\33[m");
						send_msg (-1, "\33[31;1m请上下移动光标选择对象，用Ctrl+S投票...\33[m");
						for (i = 0; i < MAX_PEOPLE; ++i)
							ppeop[i].vote = 0;
						inrooms[myroom].status = INROOM_VOTE;
					}
					else
					{
						sprintf (buf, "下面是\33[32;1m %d %s \33[m为自己辩护", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[turn], "辩护完后，请按\33[31;1mCtrl+T\33[m结束发言");
						if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[turn], "\33[31;1m把光标移到自己位置上，用Ctrl+S可自杀\33[m");
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
					send_msg (-1, "\33[1;31m阿门...\33[m");
					start_change_inroom ();
					gotonight ();
					end_change_inroom ();
					continue;
				}
				/* Efan: 杀手活动 */
				/*
				                if(inrooms[myroom].status==INROOM_NIGHT) {
							if (inrooms[myroom].peoples[me].style == -1 || inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR || !(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER) || !(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE))
							       continue;
							if (inrooms[myroom].peoples[sel].flag & PEOPLE_SPECTATOR) {
								send_msg (me, "\33[31;1m这是旁观者\33[m");
								refreshit ();
								continue;
							}
							else if (!(inrooms[myroom].peoples[sel].flag & PEOPLE_ALIVE)) {
								send_msg (me, "\33[31;1m此人已死\33[m");
								refreshit ();
								continue;
							}
							if (inrooms[myroom].dead != -1)
								send_to_fellow (PEOPLE_KILLER, "\33[31;1m-_-!!太不敬业了，哪有一晚上杀两个人的……\33[m");
							else {
								int i;
								inrooms[myroom].dead = pid;
								sprintf (buf, "好，决定了，就杀\33[32;1m %s \33[m!", inrooms[myroom].peoples[sel].nick);
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
					sprintf(buf, "%s [1m%s[m %s", party_data[i].part1_msg, buf2[0]?buf2:"大家", party_data[i].part2_msg);
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
				/* Efan: 晚上秘密说话 */
				int flag = inrooms[myroom].peoples[me].flag;
				if (inrooms[myroom].peoples[me].style != -1 &&
				        flag & PEOPLE_ALIVE && (flag & PEOPLE_KILLER || flag & PEOPLE_POLICE))
					send_to_fellow (flag, buf);
			}
			else if (inrooms[myroom].status == INROOM_STOP)
			{
				/* Efan: 非游戏进行时，可任意发言 */
				/*
						if (!(inrooms[myroom].peoples[me].flag&PEOPLE_SPECTATOR))
				*/
				send_msg (-1, buf);
			}
			else if (inrooms[myroom].status == INROOM_DARK)
			{
				/* Efan: 投票结束后，死者留遗言 */
				if (me == inrooms[myroom].turn)
					send_msg( -1, buf);
			}
			else
			{
				/* Efan: 在其他时候，只能轮流发言 */
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
					send_msg(i, "你被踢了");
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
	        sprintf(buf, "杀手%s潜逃了", buf2);
	    else
	        sprintf(buf, "%s离开房间", buf2);
	    for(i=0;i<myroom->people;i++) {
	        send_msg(inrooms.peoples+i, buf);
	        kill(inrooms.peoples[i].pid, SIGUSR1);
	    }*/
quitgame2:
	kicked=0;
	k_getdata(t_lines-1, 0, "寄回本次全部信息吗?[y/N]", buf3, 3, 1, 0, 1);
	if(toupper(buf3[0])=='Y')
	{
		sprintf(buf, "tmp/%d.msg", rand());
		save_msgs(buf);
		sprintf(buf2, "\"%s\"的杀人记录", roomname);
		mail_file(buf, currentuser.userid, buf2);

	}
	//    signal(SIGUSR1, talk_request);
}

static int room_list_refresh(struct _select_def *conf)
{
	clear();
	docmdtitle("[游戏室选单]",
	           "  退出[[1;32m←[0;37m,[1;32me[0;37m] 进入[[1;32mEnter[0;37m] 选择[[1;32m↑[0;37m,[1;32m↓[0;37m] 添加[[1;32ma[0;37m] 加入[[1;32mJ[0;37m] [m      作者: [31;1mbad@smth.org[m");
	move(2, 0);
	prints("[0;1;37;44m    %4s %-14s %-12s %4s %4s %6s %-20s[m", "编号", "游戏室名称", "创建者", "人数", "最多", "锁隐旁", "话题");
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
		prints("  %3d  %-14s %-12s %3d  %3d  %2s%2s%2s %-36s", i, r->name, r->creator, r->people, r->maxpeople, (r->flag&ROOM_LOCKED)?"√":"", (r->flag&ROOM_SECRET)?"√":"", (!(r->flag&ROOM_DENYSPEC))?"√":"", r->title);
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
		prints(" 该房间已被锁定!");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	r2 = rooms+j;
	if(r2->people>=r2->maxpeople&&!HAS_PERM(PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" 该房间人数已满");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	k_getdata(0, 0, "是否以旁观者身份进入? [y/N]", ans, 3, 1, NULL, 1);
	if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC&&!HAS_PERM(PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" 该房间拒绝旁观者");
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
		k_getdata(0, 0, "房间名:", name, 13, 1, NULL, 1);
		if(!name[0])
			return SHOW_REFRESH;
		if(name[0]==' '||name[strlen(name)-1]==' ')
		{
			move(0, 0);
			clrtoeol();
			prints(" 房间名开头结尾不能为空格");
			refresh();
			sleep(1);
			return SHOW_CONTINUE;
		}
		strcpy(r.name, name);
		r.style = 1;
		r.flag = 0;
		r.people = 0;
		r.maxpeople = 100;
		strcpy(r.title, "杀人有害健康...");
		if(add_room(&r)==-1)
		{
			move(0, 0);
			clrtoeol();
			prints(" 有一样名字的房间啦!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(r.name), 0);
		return SHOW_DIRCHANGE;
	case 'J':
		k_getdata(0, 0, "房间名:", name, 12, 1, NULL, 1);
		if(!name[0])
			return SHOW_REFRESH;
		if((i=find_room(name))==-1)
		{
			move(0, 0);
			clrtoeol();
			prints(" 没有找到该房间!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		r2 = rooms+i;
		if(r2->people>=r2->maxpeople&&!HAS_PERM(PERM_SYSOP))
		{
			move(0, 0);
			clrtoeol();
			prints(" 该房间人数已满");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		k_getdata(0, 0, "是否以旁观者身份进入? [y/N]", ans, 3, 1, NULL, 1);
		if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC&&!HAS_PERM(PERM_SYSOP))
		{
			move(0, 0);
			clrtoeol();
			prints(" 该房间拒绝旁观者");
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
	grouplist_conf.prompt = "◆";
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
	{//坏人胜了
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m坏人获得了胜利...\33[m");
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
			if (ppeop[i].style != -1 && ppeop[i].flag & PEOPLE_KILLER && ppeop[i].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "原来\33[32;1m %d %s \33[m是杀手!", i + 1, ppeop[i].nick);
				send_msg (-1, buf);
			}
	}
	else if (a == 0)
	{//好人胜了
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m所有杀手都被处决了，好人获得了胜利...\33[m");
		save_result (0);
	}
	else
	{//游戏还在继续
		inrooms[myroom].status = INROOM_DARK;
		inrooms[myroom].turn = maxi;
		if (ppeop[maxi].flag & PEOPLE_KILLER)
		{
			send_msg (-1, "\33[31;1m没有遗言\33[m");
			gotonight ();
		}
		else
		{
			sprintf (buf, "\33[32;1m%d %s \33[31;1m请留下遗言\33[m", maxi + 1, ppeop[maxi].nick);
			send_msg (-1, buf);
			send_msg (maxi, "按\33[31;1mCtrl+T\33[m结束留言");
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
	{//坏人胜了
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m坏人获得了胜利...\33[m");
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
			if (ppeop[i].style != -1 && ppeop[i].flag & PEOPLE_KILLER && ppeop[i].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "原来\33[32;1m %s \33[m是杀手!", ppeop[i].nick);
				send_msg (-1, buf);
			}
	}
	else if (a == 0)
	{//好人胜了
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m所有杀手都被处决了，好人获得了胜利...\33[m");
		save_result (0);
	}
	else
	{//游戏还在继续
		send_msg (-1, "[31;1m取消投票[m");
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
	send_msg (-1, "\33[31;1m恐怖的夜色又降临了...\33[m");
	send_to_fellow (PEOPLE_KILLER, "请抓紧你的宝贵时间用\33[31;1mCtrl+S\33[m杀人");
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
			sprintf (buf,"[1;31m不幸的是，被害者掉线了……[m");
		else
			sprintf (buf, "\33[1;33m阴森森的月光下，横躺着\33[32;1m %d %s \33[33m的尸体...\33[m", i + 1, inrooms[myroom].peoples[i].nick);
		send_msg (-1, buf);
		send_msg (i, "孩子，欢迎你来到天堂，你被凶手残忍地杀害了...");
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
		send_msg (-1, "\33[31;1m坏人获得了胜利...\33[m");
		inrooms[myroom].status = INROOM_STOP;
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
		{
			register int flag = inrooms[myroom].peoples[i].flag;
			if (inrooms[myroom].peoples[i].style != -1 && flag & PEOPLE_KILLER && flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "原来\33[32;1m %d %s \33[m是杀手!", i + 1, inrooms[myroom].peoples[i].nick);
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
		send_msg (-1, "\33[31;1m所有杀手都被处决了，好人获得了胜利...\33[m");
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
		send_msg (-1, "天亮了...");
	}
	for (i = 0; i < MAX_PEOPLE; ++i)
	{
		inrooms[myroom].peoples[i].vote = 0;
		inrooms[myroom].peoples[i].flag &= ~PEOPLE_VOTED;
	}
	if (inrooms[myroom].killernum != -1 && j < MAX_PEOPLE)
	{
		sprintf (buf, "现在，被害者\33[32;1m %d %s \33[m在天堂什么话要说？", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "请按\33[31;1mCtrl+T\33[m结束遗言");
	}
	else if (j < MAX_PEOPLE)
	{
		inrooms[myroom].test = -1;
		send_msg (-1, "\33[31;1m现在开始指证\33[m");
		sprintf (buf,"先请\33[32;1m %d %s \33[m发言", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "请[31;1m上下移动光标[m，在您怀疑的对象上");
		send_msg (j, "按\33[31;1mCtrl+S\33[m指证");
		send_msg (j, "指证完后，用\33[31;1mCtrl+T\33[m结束发言");
		if (inrooms[myroom].peoples[j].flag & PEOPLE_KILLER)
			send_msg (j, "\33[31;1m把光标移到自己位置上，用Ctrl+S可自杀\33[m");
		else
			send_msg (j, "\33[31;1m小心！杀手已经盯上你了！！\33[m");
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
		send_msg (-1, "[1;31m游戏异常结束[m");
		inrooms[myroom].status = INROOM_STOP;
		kill_msg (-1);
		end_change_inroom ();
		return 0;
	}
	if (inrooms[myroom].dead >= MAX_PEOPLE)
	{
		send_msg (-1, "[1;31m现在开始指证[m");
		sprintf (buf, "先请[1;32m %d %s [m发言", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "请[1;31m上下移动光标[m，在您怀疑的对象上");
		send_msg (j, "按[1;31mCtrl+S[m指证");
		send_msg (j, "指证完后，用[1;31mCtrl+T[m结束发言");
		if (inrooms[myroom].peoples[j].flag & PEOPLE_KILLER)
			send_msg (j, "[1;31m把光标移到自己位置上，用Ctrl+S可自杀");
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
			sprintf (buf, "[1;31m唯一的杀手[1;32m %d %s [1;31m掉线了，好人获得了胜利！", i + 1, ppeop[i].nick);
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
			sprintf (buf, "[1;31m唯一的警察[1;32m %d %s [1;31m掉线了，坏人获得了胜利！", i + 1, ppeop[i].nick);
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
			sprintf (buf, "[1;31m唯一的平民[1;32m %d %s [1;31m掉线了，坏人获得了胜利！", i + 1, ppeop[i].nick);
			send_msg (-1, buf);
			inrooms[myroom].status = INROOM_STOP;
		}
		break;
	case INROOM_DAY:
		if (i != inrooms[myroom].voted[turn])
			return;
		sprintf (buf, "[1;31mfaint...[1;35m %d %s [1;31m还没说完就掉线了~~~", i + 1, ppeop[i].nick);
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
			send_msg (-1, "[1;31m游戏异常结束！[m");
			inrooms[myroom].status = INROOM_STOP;
			return;
		}
		inrooms[myroom].turn = turn;
		inrooms[myroom].status = INROOM_CHECK;
		inrooms[myroom].test = -1;
		for (j = 0; j < MAX_PEOPLE; ++j)
			ppeop[j].vote = 0;
		send_msg (-1, "[1;31m现在开始指证[m");
		sprintf (buf, "先请[1;32m %d %s [m发言", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
		send_msg (-1, buf);
		send_msg (inrooms[myroom].voted[turn], "请[1;31m上下移动光标[m，在您怀疑的对象上");
		send_msg (inrooms[myroom].voted[turn], "按[1;31mCtrl+S[m指证");
		send_msg (inrooms[myroom].voted[turn], "指证完后，用[1;31mCtrl+T[m结束发言");
		if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
			send_msg (inrooms[myroom].voted[turn], "[1;31m把光标移到自己位置上，用Ctrl+S可自杀[m");
		break;
	case INROOM_CHECK:
		if (i != inrooms[myroom].voted[turn])
			return;
		++turn;
		sprintf (buf, "[1;32m%d %s[m: [1;31mOver.[m", i + 1, ppeop[i].nick);
		send_msg (-1, buf);
		if (inrooms[myroom].suicide == i)
		{
			sprintf (buf, "[1;31m杀手[1;32m %d %s [1;31m崩溃了……[m", i + 1, ppeop[i].nick);
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
				send_msg (-1, "[1;31m游戏异常结束！[m");
				inrooms[myroom].status = INROOM_STOP;
				return;
			}
			inrooms[myroom].numvoted = t;
			inrooms[myroom].turn = 0;
			inrooms[myroom].status = INROOM_DEFENCE;
			for (k = 0; k < MAX_PEOPLE; ++k)
				ppeop[k].vote = 0;
			send_msg (-1, "[1;31m指证结束[m");
			send_msg (-1, "[1;31m下面请被指证的公民发言[m");
			sprintf (buf, "首先是[1;32m %d %s [m为自己辩护", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[0], "辩护完后，请用[1;31mCtrl+T[m结束发言");
			if (ppeop[inrooms[myroom].voted[0]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[0], "[1;31m把光标移到自己位置上，用Ctrl+S可自杀[m");
		}
		else
		{
			sprintf (buf, "下面是[1;32m %d %s [m发言", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[turn], "请[1;31m上下移动光标[m，在您怀疑的对象上");
			send_msg (inrooms[myroom].voted[turn], "按[1;31mCtrl+S[m指证");
			send_msg (inrooms[myroom].voted[turn], "指证完后，用[1;31mCtrl+T[m结束发言");
			if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[turn], "[1;31m把光标移到自己位置上，用Ctrl+S可自杀[m");
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
			sprintf (buf, "[1;31m杀手[1;32m %d %s [1;31m崩溃了……[m", i + 1, ppeop[i].nick);
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
			send_msg (-1, "[1;31m辩护结束，现在开始投票[m");
			send_msg (-1, "[1;31m请上下移动光标选择对象，用Ctrl+S投票...[m");
			for (j = 0; j < MAX_PEOPLE; ++j)
				ppeop[j].vote = 0;
			inrooms[myroom].status = INROOM_VOTE;
		}
		else
		{
			sprintf (buf, "下面是[1;31m %d %s [m为自己辩护", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[turn], "辩护完后，请按[1;31mCtrl+T[m结束发言");
			if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[turn], "[1;31m把光标移到自己位置上，用Ctrl+S可自杀[m");
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
				sprintf (buf, "[1;32m%d %s投了%d %s一票[m", j + 1, ppeop[j].nick, tmp + 1, ppeop[tmp].nick);
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
		sprintf (buf, "[1;31m第 %d 轮投票结果:[m", inrooms[myroom].rotate);
		send_msg (-1, buf);
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1 && !(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].flag & PEOPLE_VOTED && ppeop[j].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "%s被投: %d 票", ppeop[j].nick, ppeop[j].vnum);
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
				send_msg (-1, "[1;31m游戏异常结束[m");
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
			send_msg (-1, "[1;31m获得最多票数的公民请为自己辩护[m");
			sprintf (buf, "首先是[1;32m %d %s [m发言", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[0], "辩护完后，请按[1;31mCtrl+T[m结束发言");
		}
		else
		{
			int a = 0, b = 0;
			if (!ok && inrooms[myroom].rotate >= 3)
			{
				send_msg (-1, "[1;31m投票三次仍未有结果，法官随机处决[m");
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
					send_msg (-1, "[1;31m游戏异常结束[m");
					inrooms[myroom].status = INROOM_STOP;
					kill_msg (-1);
					return;
				}
				max = ppeop[a].vnum;
				maxi = a;
				maxpid = ppeop[a].pid;
			}
			send_msg (maxi, "[1;31m你被法官处决了![m");
			sprintf (buf, "[1;32m %d %s [1;31m被法官处决了[m", maxi + 1, ppeop[maxi].nick);
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
		send_msg (-1, "[1;31m阿门...[m");
		gotonight ();
		break;
	}
}
