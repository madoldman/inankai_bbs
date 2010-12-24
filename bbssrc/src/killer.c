/******************************************************
É±ÈËÓÎÏ·2003, ×÷Õß: bad@smth.org  Qian Wenjie
ÔÚË®Ä¾Çå»ªbbsÏµÍ³ÉÏÔËĞĞ
 
±¾ÓÎÏ·ÊÇ×ÔÓÉÈí¼ş£¬ÇëËæÒâ¸´ÖÆÒÆÖ²
ÇëÔÚĞŞ¸ÄºóµÄÎÄ¼şÍ·²¿±£Áô°æÈ¨ĞÅÏ¢
******************************************************/
/******************************************************
 * É±ÈËÓÎÏ· 2003 for FB 2000 ×÷Õß: TurboZV@UESTC Zhang Wei
 *
 * ÎÒÒÑ¾­½«smthµÄÉ±ÈËÓÎÏ·ÒÆÖ²µ½FB2000ÏµÍ³ÉÏ£¬
 * Ğ»Ğ»bad@smthÎª´ËÓÎÏ·µÄ¸¶³ö
 * 
 *±¾ÓÎÏ·ÊÇ×ÔÓÉÈí¼ş£¬ÇëËæÒâ¸´ÖÆÒÆÖ²
 *ÇëÔÚĞŞ¸ÄºóµÄÎÄ¼şÍ·²¿±£Áô°æÈ¨ĞÅÏ¢
 *				    2003-5-12
******************************************************/
/* ChangeLog 030515
 *  1) ÕıÈ·µÄ´¦ÀíÁËÑÕÉ«µÄÏÔÊ¾
 *  2) ¼ÓÈë±íÇé¹¦ÄÜ(*)
 *  3) Í¶Æ±ÓÃÏÂ»®Ïß±íÊ¾(*)
 *  4) ¼ÓÁËÒ»¸öÅÅĞĞ°ñ¹¦ÄÜ(*)
 *  5) ¶Ô¾¯²ìµÄ¹¦ÄÜ×÷ÁËÒ»Ğ©ĞŞÕı(*)
 *  6) ¾¯²ìÖ»ÄÜÓÃCtrl+TÊÔÌ½»µÈË (*)
 * ´ò*Îªbad@smthËù×öµÄĞŞ¸Ä
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
//ÅÔ¹ÛÕß
#define PEOPLE_SPECTATOR 01
//É±ÊÖ
#define PEOPLE_KILLER 02
//
#define PEOPLE_ALIVE 04
//
#define PEOPLE_ROOMOP 010
//¾¯²ì
#define PEOPLE_POLICE 020
//what's this???
#define PEOPLE_TESTED 040
#define PEOPLE_VOTED  0100
#define PEOPLE_HIDE   0200

struct people_struct
{
	int style; // Efan: style == -1 ¼´±»Ìß
	char id[IDLEN+2];
	char nick[NAMELEN];
	int flag;
	int pid;
	int vote;
	int vnum;
};

#define INROOM_STOP 1
//ÍíÉÏ
#define INROOM_NIGHT 2
//°×Ìì
#define INROOM_DAY 3
//Ö¸Ö¤½×¶Î
#define INROOM_CHECK 4
//±ç»¤½×¶Î
#define INROOM_DEFENCE 5
//Í¶Æ±½×¶Î
#define INROOM_VOTE 6
//ËÀÕßÁôÒÅÑÔ
#define INROOM_DARK 7


struct inroom_struct
{
	int w;
	int status;
	int killernum;
	int policenum;
	/* ÉĞÎ´¼ÓÈëµÄ´úÂë */
	int turn; //ÂÖµ½Ä³ÈË·¢ÑÔ
	int rotate; //µÚ¼¸ÂÖÍ¶Æ±
	int dead; //¸ÃÂÖ±»É±µÄÈË
	int test; //¾¯²ìÑéÖ¤µÄÈË
	int suicide; //É±ÊÖ±ÀÀ£
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
	int w; //0 - Æ½ÃñÊ¤Àû 1 - É±ÊÖÊ¤Àû
	time_t t;
	int peoplet;
	char id[MAX_PEOPLE][IDLEN+2];
	int st[MAX_PEOPLE]; // 0 - »î×ÅÆ½Ãñ 1 - ËÀÁËÆ½Ãñ 2 - »î×ÅÉ±ÊÖ 3 - ËÀÁËÉ±ÊÖ 4 - ÆäËûÇé¿ö
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
                if(!strncmp(buf, "Äã±»ÌßÁË", 8)) kicked=1;
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
	char *verb;                 /* ¶¯´Ê */
	char *part1_msg;            /* ½é´Ê */
	char *part2_msg;            /* ¶¯×÷ */
};

struct action party_data[] =
    {
	    {"?", "ºÜÒÉ»óµÄ¿´×Å", ""
	    },
	    {"admire", "¶Ô", "µÄ¾°ÑöÖ®ÇéÓÌÈçÌÏÌÏ½­Ë®Á¬Ãà²»¾ø"},
	    {"agree", "ÍêÈ«Í¬Òâ", "µÄ¿´·¨"},
	    {"bearhug", "ÈÈÇéµÄÓµ±§", ""},
	    {"bless", "×£¸£", "ĞÄÏëÊÂ³É"},
	    {"bow", "±Ï¹ª±Ï¾´µÄÏò", "¾Ï¹ª"},
	    {"bye", "¿´×Å", "µÄ±³Ó°£¬ÆàÈ»ÀáÏÂ¡£ÉíºóµÄÊÕÒô»ú´«À´µËÀö¾ıµÄ¸èÉù:\n\"[31mºÎÈÕ¾ıÔÙÀ´.....[m\""},
	    {"caress", "ÇáÇáµÄ¸§Ãş", ""},
	    {"cat", "ÏñÖ»Ğ¡Ã¨°ãµØÒÀÙËÔÚ", "µÄ»³ÀïÈö½¿¡£"},
	    {"cringe", "Ïò", "±°¹ªÇüÏ¥£¬Ò¡Î²ÆòÁ¯"},
	    {"cry", "Ïò", "º¿ßû´ó¿Ş"},
	    {"comfort", "ÎÂÑÔ°²Î¿", ""},
	    {"clap", "Ïò", "ÈÈÁÒ¹ÄÕÆ"},
	    {"dance", "À­×Å", "µÄÊÖôæôæÆğÎè"},
	    {"dogleg", "¶Ô", "¹·ÍÈ"},
	    {"drivel", "¶ÔÖø", "Á÷¿ÚË®"},
	    {"dunno", "µÉ´óÑÛ¾¦£¬ÌìÕæµØÎÊ£º", "£¬ÄãËµÊ²÷áÎÒ²»¶®Ò®... :("},
	    {"faint", "ÔÎµ¹ÔÚ", "µÄ»³Àï"},
	    {"fear", "¶Ô", "Â¶³öÅÂÅÂµÄ±íÇé"},
	    {"fool", "Çë´ó¼Ò×¢Òâ", "Õâ¸ö´ó°×³Õ....\nÕæÊÇÌìÉÏÉÙÓĞ....ÈË¼ä½ö´æµÄ»î±¦....\n²»¿´Ì«¿ÉÏ§ÁË£¡"},
	    {"forgive", "´ó¶ÈµÄ¶Ô", "Ëµ£ºËãÁË£¬Ô­ÁÂÄãÁË"},
	    {"giggle", "¶ÔÖø", "ÉµÉµµÄ´ôĞ¦"},
	    {"grin", "¶Ô", "Â¶³öĞ°¶ñµÄĞ¦Èİ"},
	    {"growl", "¶Ô", "ÅØÏø²»ÒÑ"},
	    {"hand", "¸ú", "ÎÕÊÖ"},
	    {"hammer", "¾ÙÆğºÃ´óºÃ´óµÄÌú´¸£¡£¡ÍÛ£¡Íù",
	     "Í·ÉÏÓÃÁ¦Ò»ÇÃ£¡\n***************\n*  5000000 Pt *\n***************\n      | |      ¡ï¡î¡ï¡î¡ï¡î\n      | |         ºÃ¶àµÄĞÇĞÇÓ´\n      |_|"},
	    {"heng", "¿´¶¼²»¿´", "Ò»ÑÛ£¬ ºßÁËÒ»Éù£¬¸ß¸ßµÄ°ÑÍ·ÑïÆğÀ´ÁË,²»Ğ¼Ò»¹ËµÄÑù×Ó..."},
	    {"hug", "ÇáÇáµØÓµ±§", ""},
	    {"idiot", "ÎŞÇéµØ³ÜĞ¦", "µÄ³Õ´ô¡£"},
	    {"kick", "°Ñ", "ÌßµÄËÀÈ¥»îÀ´"},
	    {"kiss", "ÇáÎÇ", "µÄÁ³¼Õ"},
	    {"laugh", "´óÉù³°Ğ¦", ""},
	    {"lovelook", "À­×Å", "µÄÊÖ£¬ÎÂÈáµØÄ¬Ä¬¶ÔÊÓ¡£Ä¿¹âÖĞÔĞº¬×ÅÇ§ÖÖÈáÇé£¬Íò°ãÃÛÒâ"},
	    {"nod", "Ïò", "µãÍ·³ÆÊÇ"},
	    {"nudge", "ÓÃÊÖÖâ¶¥", "µÄ·Ê¶Ç×Ó"},
	    {"oh", "¶Ô", "Ëµ£º¡°Å¶£¬½´×Ó°¡£¡¡±"},
	    {"pad", "ÇáÅÄ", "µÄ¼ç°ò"},
	    {"papaya", "ÇÃÁËÇÃ", "µÄÄ¾¹ÏÄÔ´ü"},
	    {"pat", "ÇáÇáÅÄÅÄ", "µÄÍ·"},
	    {"pinch", "ÓÃÁ¦µÄ°Ñ", "Å¡µÄºÚÇà"},
	    {"puke", "¶Ô×Å", "ÍÂ°¡ÍÂ°¡£¬¾İËµÍÂ¶à¼¸´Î¾ÍÏ°¹ßÁË"},
	    {"punch", "ºİºİ×áÁË", "Ò»¶Ù"},
	    {"pure", "¶Ô", "Â¶³ö´¿ÕæµÄĞ¦Èİ"},
	    {"qmarry", "Ïò", "ÓÂ¸ÒµÄ¹òÁËÏÂÀ´:\n\"ÄãÔ¸Òâ¼Ş¸øÎÒÂğ£¿\"\n---ÕæÊÇÓÂÆø¿É¼Î°¡"},
	    {"report", "ÍµÍµµØ¶Ô", "Ëµ£º¡°±¨¸æÎÒºÃÂğ£¿¡±"},
	    {"shrug", "ÎŞÄÎµØÏò", "ËÊÁËËÊ¼ç°ò"},
	    {"sigh", "¶Ô", "Ì¾ÁËÒ»¿ÚÆø"},
	    {"slap", "Å¾Å¾µÄ°ÍÁË", "Ò»¶Ù¶ú¹â"},
	    {"smooch", "ÓµÎÇÖø", ""},
	    {"snicker", "ºÙºÙºÙ..µÄ¶Ô", "ÇÔĞ¦"},
	    {"sniff", "¶Ô", "àÍÖ®ÒÔ±Ç"},
	    {"sorry", "Í´¿ŞÁ÷ÌéµÄÇëÇó", "Ô­ÁÂ"},
	    {"spank", "ÓÃ°ÍÕÆ´ò", "µÄÍÎ²¿"},
	    {"squeeze", "½ô½ôµØÓµ±§Öø", ""},
	    {"thank", "Ïò", "µÀĞ»"},
	    {"tickle", "¹¾ß´!¹¾ß´!É¦", "µÄÑ÷"},
	    {"waiting", "ÉîÇéµØ¶Ô", "Ëµ£ºÃ¿ÄêÃ¿ÔÂµÄÃ¿Ò»Ìì£¬Ã¿·ÖÃ¿ÃëÎÒ¶¼ÔÚÕâÀïµÈ×ÅÄã"},
	    {"wake", "Å¬Á¦µÄÒ¡Ò¡", "£¬ÔÚÆä¶ú±ß´ó½Ğ£º¡°¿ìĞÑĞÑ£¬»á×ÅÁ¹µÄ£¡¡±"},
	    {"wave", "¶ÔÖø", "Æ´ÃüµÄÒ¡ÊÖ"},
	    {"welcome", "ÈÈÁÒ»¶Ó­", "µÄµ½À´"},
	    {"wink", "¶Ô", "ÉñÃØµÄÕ£Õ£ÑÛ¾¦"},
	    {"xixi", "ÎûÎûµØ¶Ô", "Ğ¦ÁË¼¸Éù"},
	    {"zap", "¶Ô", "·è¿ñµÄ¹¥»÷"},
	    {"inn", "Ë«ÑÛ±¥º¬×ÅÀáË®£¬ÎŞ¹¼µÄÍû×Å", ""},
	    {"mm", "É«ÃĞÃĞµÄ¶Ô", "ÎÊºÃ£º¡°ÃÀÃ¼ºÃ¡«¡«¡«¡«¡±¡£´óÉ«ÀÇ°¡£¡£¡£¡"},
	    {"disapp", "ÕâÏÂÃ»¸ãÍ·À²£¬ÎªÊ²Ã´", "¹ÃÄï¶ÔÎÒÕâ¸öÔìĞÍÍêÈ«Ã»·´Ó¦£¿Ã»°ì·¨£¡"},
	    {"miss", "Õæ³ÏµÄÍû×Å", "£ºÎÒÏëÄîÄãÎÒÕæµÄÏëÄîÄãÎÒÌ«--ÏëÄîÄãÁË!ÄãÏà²»ÏàĞÅ?"},
	    {"buypig", "Ö¸×Å", "£º¡°Õâ¸öÖíÍ·¸øÎÒÇĞÒ»°ë£¬Ğ»Ğ»£¡¡±"},
	    {"rascal", "¶Ô", "´ó½Ğ£º¡°ÄãÕâ¸ö³ôÁ÷Ã¥£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡¡±"},
	    {"qifu", "Ğ¡×ìÒ»±â£¬¶Ô", "¿ŞµÀ£º¡°ÄãÆÛ¸ºÎÒ£¬ÄãÆÛ¸ºÎÒ£¡£¡£¡¡±"},
	    {"wa", "¶Ô", "´ó½ĞÒ»Éù£º¡°ÍÛÍÛÍÛÍÛÍÛÍÛ¿á±×ÁËÒ®£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡¡±"},
	    {"feibang", "à¸£­£­£¡Êì¹éÊì£¬", "ÄãÕâÑùÂÒ½²»°£¬ÎÒÒ»Ñù¿ÉÒÔ¸æÄã»Ù°ù£¬¹ş£¡"},
	    {"badman", "Ö¸×Å", "µÄÄÔ´ü, ÊÖÎè×ãµ¸µÄº°:¡° É±~~ÊÖ~~ÔÚ~~Õâ~~Àï~~~¡±"},
	    {NULL, NULL, NULL}
    };

struct action speak_data[] =
    {
	    {"ask", "Ñ¯ÎÊ", NULL
	    },
	    {"chant", "¸èËÌ", NULL},
	    {"cheer", "ºÈ²É", NULL},
	    {"chuckle", "ÇáĞ¦", NULL},
	    {"curse", "ÖäÂî", NULL},
	    {"demand", "ÒªÇó", NULL},
	    {"frown", "õ¾Ã¼", NULL},
	    {"groan", "ÉëÒ÷", NULL},
	    {"grumble", "·¢ÀÎÉ§", NULL},
	    {"hum", "à«à«×ÔÓï", NULL},
	    {"moan", "±¯Ì¾", NULL},
	    {"notice", "×¢Òâ", NULL},
	    {"order", "ÃüÁî", NULL},
	    {"ponder", "ÉòË¼", NULL},
	    {"pout", "àÙÖø×ìËµ", NULL},
	    {"pray", "Æíµ»", NULL},
	    {"request", "¿ÒÇó", NULL},
	    {"shout", "´ó½Ğ", NULL},
	    {"sing", "³ª¸è", NULL},
	    {"smile", "Î¢Ğ¦", NULL},
	    {"smirk", "¼ÙĞ¦", NULL},
	    {"swear", "·¢ÊÄ", NULL},
	    {"tease", "³°Ğ¦", NULL},
	    {"whimper", "ÎØÑÊµÄËµ", NULL},
	    {"yawn", "¹şÇ·Á¬Ìì", NULL},
	    {"yell", "´óº°", NULL},
	    {NULL, NULL, NULL}
    };

struct action condition_data[] =
    {
	    {":D", "ÀÖµÄºÏ²»Â£×ì", NULL
	    },
	    {":)", "ÀÖµÄºÏ²»Â£×ì", NULL},
	    {":P", "ÀÖµÄºÏ²»Â£×ì", NULL},
	    {":(", "ÀÖµÄºÏ²»Â£×ì", NULL},
	    {"applaud", "Å¾Å¾Å¾Å¾Å¾Å¾Å¾....", NULL},
	    {"blush", "Á³¶¼ºìÁË", NULL},
	    {"cough", "¿ÈÁË¼¸Éù", NULL},
	    {"faint", "ßÛµ±Ò»Éù£¬ÔÎµ¹ÔÚµØ", NULL},
	    {"happy", "µÄÁ³ÉÏÂ¶³öÁËĞÒ¸£µÄ±íÇé£¬²¢Ñ§³Ô±¥ÁËµÄÖíºßºßÁËÆğÀ´", NULL},
	    {"lonely", "Ò»¸öÈË×øÔÚ·¿¼äÀï£¬°ÙÎŞÁÄÀµ£¬Ï£ÍûË­À´ÅãÅã¡£¡£¡£¡£", NULL},
	    {"luck", "ÍÛ£¡¸£ÆøÀ²£¡", NULL},
	    {"puke", "Õæ¶ñĞÄ£¬ÎÒÌıÁË¶¼ÏëÍÂ", NULL},
	    {"shake", "Ò¡ÁËÒ¡Í·", NULL},
	    {"sleep", "Zzzzzzzzzz£¬ÕæÎŞÁÄ£¬¶¼¿ìË¯ÖøÁË", NULL},
	    {"so", "¾Í½´×Ó!!", NULL},
	    {"strut", "´óÒ¡´ó°ÚµØ×ß", NULL},
	    {"tongue", "ÍÂÁËÍÂÉàÍ·", NULL},
	    {"think", "ÍáÖøÍ·ÏëÁËÒ»ÏÂ", NULL},
	    {"wawl", "¾ªÌì¶¯µØµÄ¿Ş", NULL},
	    {"goodman", "ÓÃ¼°ÆäÎŞ¹¼µÄ±íÇé¿´×Å´ó¼Ò: ¡°ÎÒÕæµÄÊÇºÃÈËÒ®~~¡±", NULL},
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
					sprintf(buf, "%sµôÏßÁË", inrooms[myroom].peoples[i].nick);
					send_msg(-1, buf);
					start_change_inroom();
					inrooms[myroom].peoples[i].style=-1;
					/* Efan: ÒÔºóÔÙËµ~~~ */
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
					/* Efan: ÔİÊ±ÉèÎªÒ»ÈËµôÏß¼´½áÊøÓÎÏ· */
					/*
					if (inrooms[myroom].status != INROOM_STOP && inrooms[myroom].peoples[i].flag & PEOPLE_ALIVE && !(inrooms[myroom].peoples[i].flag & PEOPLE_SPECTATOR)) {
					inrooms[myroom].status = INROOM_STOP;
					send_msg (-1, "\33[31;1mÓÎÏ·Òì³£½áÊø\33[m");
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
								sprintf(buf, "%s³ÉÎªĞÂ·¿Ö÷", inrooms[myroom].peoples[k].nick);
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
	("[44;33;1m ·¿¼ä:[36m%-12s[33m»°Ìâ:[36m%-40s[33m×´Ì¬:[36m%6s[m",
	 rooms[myroom].name, rooms[myroom].title,
	 (inrooms[myroom].status ==
	  INROOM_STOP ? "Î´¿ªÊ¼" : (inrooms[myroom].status ==
	                            INROOM_NIGHT ? "ºÚÒ¹ÖĞ" : "´ó°×Ìì")));
	clrtoeol();
	k_resetcolor();
	k_setfcolor(YELLOW, 1);
	move(1, 0);
	prints("[1;33m");
	prints
	("¨q[1;32mÍæ¼Ò[33m¡ª¡ª¡ª¡ª¡ª¨r¨q[1;32mÑ¶Ï¢[33m¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¨r");
	move(t_lines - 2, 0);
	prints
	("¨t¡ª¡ª¡ª¡ª¡ª¡ª¡ª¨s¨t¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¨s[m");

	me = mypos;
	msgst = get_msgt();

	for (i = 2; i <= t_lines - 3; i++)
	{
		strcpy(disp, "©¦");

		// ÒªÏÔÊ¾ÓÃ»§
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
					/* Efan: ¾¯²ì²»Ó¦¸Ã¿´µ½»µÈËÍ¶Æ±
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
		strcat(disp, "[1;33m©¦©¦[m");

		// ´¦ÀíÏÔÊ¾µÄÏûÏ¢
		if (msgst - 1 - (t_lines - 3 - i) - jpage >= 0)
		{
			char *ss = get_msgs(msgst - 1 - (t_lines - 3 - i) - jpage);
			if (!strcmp(ss, "Äã±»ÌßÁË"))
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
		strcat(disp, "[1;33m©¦");

		// ÏÔÊ¾
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
		send_msg(me, "[31;1mÖÁÉÙ3ÈË²Î¼Ó²ÅÄÜ¿ªÊ¼ÓÎÏ·[m");
		end_change_inroom();
		refreshit();
		return;
	}
	if(totalk==0)
		totalk=((double)total/6+0.5);
	/* Efan: É±ÊÖºÍ¾¯²ìÊıÄ¿ÓÉÍæ¼Ò×ÔÓÉÉè¶¨£¬³ÌĞò²»ÔÙÇ¿ÆÈ
	    if(totalk>=total/4) totalk=total/4;
	    if(totalc>=total/6) totalc=total/6;
	    if(totalk>total) {
	        send_msg(me, "[31;1m×ÜÈËÊıÉÙÓÚÒªÇóµÄ»µÈËÈËÊı,ÎŞ·¨¿ªÊ¼ÓÎÏ·[m");
	        end_change_inroom();
	        refreshit();
	        return;
	    }
	*/
	/* Efan: µ«ÊÇÓ¦¸Ã±£Ö¤ÖÁÉÙÓĞÒ»¸öÊÇÆ½Ãñ~~ */
	if (totalk + totalc > total - 1)
	{
		send_msg (me, "\33[31;1mÃ»ÓĞÆ½ÃñÄÜÍæÃ´£¿ÇëÖØĞÂÉè¶¨É±ÊÖºÍ¾¯²ìÊıÄ¿\33[m");
		end_change_inroom ();
		refreshit ();
		return;
	}
	/* Efan: Ò²Òª±£Ö¤ÓĞÒ»¸öÉ±ÊÖ¡­¡­ */
	if (totalk == 0)
	{
		send_msg (me, "[31;1mÃ»ÓĞÉ±ÊÖÔõÃ´É±ÈËÍÛ~~~ÇëÖØĞÂÉè¶¨É±ÊÖÊıÄ¿[m");
		end_change_inroom ();
		refreshit ();
		return;
	}

	if(totalc==0)
		sprintf(buf, "[31;1mÓÎÏ·¿ªÊ¼À²! ÈËÈºÖĞ³öÏÖÁË%d¸ö»µÈË[m", totalk);
	else
		sprintf(buf, "[31;1mÓÎÏ·¿ªÊ¼À²! ÈËÈºÖĞ³öÏÖÁË%d¸ö»µÈË, %d¸ö¾¯²ì[m", totalk, totalc);
	send_msg(-1, buf);
	sprintf (buf, "[31;1mÏÖÔÚÍæµÄÊÇ %d(p)-%d(k)-%d(c) .[m", totalc, totalk, total - totalc - totalk);
	send_msg (-1, buf);
	for(i=0;i<totalk;i++)
	{
		do
		{
			j=rand()%MAX_PEOPLE;
		}
		while(inrooms[myroom].peoples[j].style==-1 || inrooms[myroom].peoples[j].flag&PEOPLE_KILLER || inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR);
		inrooms[myroom].peoples[j].flag |= PEOPLE_KILLER;
		send_msg(j, "Äã×öÁËÒ»¸öÎŞ³ÜµÄ»µÈË");
		send_msg(j,"[31;1mÉÏÏÂÒÆ¶¯¹â±ê[mÑ¡Ôñ¶ÔÏó");
		if (totalk != 1)
		{
			send_msg(j,"ÓÃÄãµÄ¼âµ¶([31;1mCtrl+S[m)Ñ¡ÔñÒª²Ğº¦µÄÈË°É...");
		}
		else
		{
			send_msg (j, "ÓÃÄãµÄ¼âµ¶([31;1mCtrl+S[m)Ñ¡ÔñÄãÏë²Ğº¦µÄÈË°É...");
			send_msg (j, "[31;1m´Ë¾ÖÔİ²»¿ÉÉ±ÈË¡­¡­[m");
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
		send_msg(j, "Äã×öÁËÒ»Î»¹âÈÙµÄÈËÃñ¾¯²ì");
		send_msg(j, "Çë[31;1mÉÏÏÂÒÆ¶¯¹â±ê[m");
		send_msg(j, "ÓÃÄãµÄ¾¯°ô([31;1mCtrl+S[m)²é¿´Äã»³ÒÉµÄÈË...");
		/*
		if (totalk == 1 && totalc == 1)
			send_msg (j, "²»¹ıÒªµ½[31;1mÏÂÒ»»ØºÏ[m¿ªÊ¼²Å¿ÉÒÔ~~~");
			*/
	}
	for(i=0;i<MAX_PEOPLE;i++)
		if(inrooms[myroom].peoples[i].style!=-1)
			if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR))
			{
				inrooms[myroom].peoples[i].flag |= PEOPLE_ALIVE;
				if(!(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER))
					send_msg(i, "ÏÖÔÚÊÇÍíÉÏ...");
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
	    { "0-·µ»Ø", "1-ÍË³öÓÎÏ·", "2-¸ÄÃû×Ö", "3-Íæ¼ÒÁĞ±í", "4-¸Ä»°Ìâ",
	      "5-ÉèÖÃ·¿¼ä", "6-ÌßÍæ¼Ò", "7-¿ªÊ¼ÓÎÏ·"
	    };
	int i, j, k, sel = 0, ch, max = 0, me;
	char buf[80], disp[128];
	if (inrooms[myroom].status != INROOM_STOP)
		strcpy(menus[7], "7-½áÊøÓÎÏ·");
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
					send_msg(me, "Äã»¹ÔÚÓÎÏ·,²»ÄÜÍË³ö");
					refreshit();
					return 0;
				}
				return 1;
			case 2:
				if (inrooms[myroom].status != INROOM_STOP)
				{
					send_msg (me, "ÓÎÏ·ÖĞ£¬²»ÄÜ¸ÄID");
					refreshit ();
					return 0;
				}
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "ÇëÊäÈëÃû×Ö:", buf, 13, 1, 0, 1);
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
						prints(" Ãû×Ö²»·ûºÏ¹æ·¶");
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
						prints(" ÒÑÓĞÈËÓÃÕâ¸öÃû×ÖÁË");
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
				k_getdata(t_lines-1, 0, "ÇëÊäÈë»°Ìâ:", buf, 31, 1, 0, 1);
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
				k_getdata(t_lines-1, 0, "ÇëÊäÈë·¿¼ä×î´óÈËÊı:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					if(i>0&&i<=100)
					{
						rooms[myroom].maxpeople = i;
						sprintf(buf, "ÎİÖ÷ÉèÖÃ·¿¼ä×î´óÈËÊıÎª%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "ÉèÖÃÎªÒş²Ø·¿¼ä? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_SECRET;
					else
						rooms[myroom].flag&=~ROOM_SECRET;
					sprintf(buf, "ÎİÖ÷ÉèÖÃ·¿¼äÎª%s", (buf[0]=='Y')?"Òş²Ø":"²»Òş²Ø");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "ÉèÖÃÎªËø¶¨·¿¼ä? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_LOCKED;
					else
						rooms[myroom].flag&=~ROOM_LOCKED;
					sprintf(buf, "ÎİÖ÷ÉèÖÃ·¿¼äÎª%s", (buf[0]=='Y')?"Ëø¶¨":"²»Ëø¶¨");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "ÉèÖÃÎª¾Ü¾øÅÔ¹ÛÕßµÄ·¿¼ä? [Y/N]", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				buf[0]=toupper(buf[0]);
				if(buf[0]=='Y'||buf[0]=='N')
				{
					if(buf[0]=='Y')
						rooms[myroom].flag|=ROOM_DENYSPEC;
					else
						rooms[myroom].flag&=~ROOM_DENYSPEC;
					sprintf(buf, "ÎİÖ÷ÉèÖÃ·¿¼äÎª%s", (buf[0]=='Y')?"¾Ü¾øÅÔ¹Û":"²»¾Ü¾øÅÔ¹Û");
					send_msg(-1, buf);
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "ÉèÖÃ»µÈËµÄÊıÄ¿:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					/* Efan: É±ÊÖÊıÏŞÖÆÔÚÊ®ÈËÒÔÄÚ»¹ÇéÓĞ¿ÉÔ­ */
					if(i>=0&&i<=10)
					{
						inrooms[myroom].killernum = i;
						sprintf(buf, "ÎİÖ÷ÉèÖÃ·¿¼ä»µÈËÊıÎª%d", i);
						send_msg(-1, buf);
					}
				}
				move(t_lines-1, 0);
				clrtoeol();
				k_getdata(t_lines-1, 0, "ÉèÖÃ¾¯²ìµÄÊıÄ¿:", buf, 30, 1, 0, 1);
				if(kicked)
					return 0;
				if(buf[0])
				{
					i=atoi(buf);
					/* Efan: ¾¯²ìÊı¾ÓÈ»ÏŞÖÆÔÚ¶şÈËÒÔÄÚ~~~Çå»ªµÄÉ±ÈËÌ«¹ÖÒìÁË~~~
					                        if(i>=0&&i<=2) {
					                            inrooms[myroom].policenum = i;
					                            sprintf(buf, "ÎİÖ÷ÉèÖÃ·¿¼ä¾¯²ìÊıÎª%d", i);
					                            send_msg(-1, buf);
					                        }
					*/
					if (i >=0 && i <= 10)
					{
						inrooms [myroom].policenum = i;
						sprintf (buf, "ÎİÖ÷ÉèÖÃ·¿¼ä¾¯²ìÊıÎª%d", i);
						send_msg (-1, buf);
					}
				}
				kill_msg(-1);
				return 0;
			case 6:
				move(t_lines-1, 0);
				k_resetcolor();
				clrtoeol();
				k_getdata(t_lines-1, 0, "ÇëÊäÈëÒªÌßµÄid:", buf, 30, 1, 0, 1);
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
						send_msg(i, "Äã±»ÌßÁË");
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
					send_msg(-1, "ÓÎÏ·±»ÎİÖ÷Ç¿ÖÆ½áÊø");
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
	/*    sprintf(buf, "%s½øÈë·¿¼ä", currentuser.userid);
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
			ch=-k_getdata(t_lines-1, 0, "ÊäÈë:", buf, 70, 1, NULL, 1);
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
			/* Efan: ÄÏ¿ª°æÉ±ÈË£¬¾¯²ìÔÚÍíÉÏÑéÖ¤É±ÊÖ */
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

				/* Efan: ±ç»¤Ê±×ÔÉ± */
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
					send_msg (me, "[31;1mÄã±ÀÀ£ÁË¡­¡­[m");
					kill_msg (-1);
					start_change_inroom ();
					inrooms[myroom].suicide = me;
					end_change_inroom ();
					continue;
				}
				/* Efan: Ö¸Ö¤ */
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
						send_msg (me, "[31;1mÄã±ÀÀ£ÁË¡­¡­[m");
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
				/* Efan: É±ÊÖ»î¶¯ */
				if(inrooms[myroom].status==INROOM_NIGHT && inrooms[myroom].peoples[me].flag & PEOPLE_KILLER)
				{
					if (inrooms[myroom].peoples[me].style == -1 || inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR || !(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER) || !(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE))
						continue;
					if (inrooms[myroom].peoples[sel].flag & PEOPLE_SPECTATOR)
					{
						send_msg (me, "\33[31;1mÕâÊÇÅÔ¹ÛÕß\33[m");
						refreshit ();
						continue;
					}
					else if (!(inrooms[myroom].peoples[sel].flag & PEOPLE_ALIVE))
					{
						send_msg (me, "\33[31;1m´ËÈËÒÑËÀ\33[m");
						refreshit ();
						continue;
					}
					if (inrooms[myroom].dead != -1)
						if (inrooms[myroom].killernum != -1)
							send_to_fellow (PEOPLE_KILLER, "\33[31;1m-_-!!Ì«²»¾´ÒµÁË£¬ÄÄÓĞÒ»ÍíÉÏÉ±Á½¸öÈËµÄ¡­¡­\33[m");
						else
							send_to_fellow (PEOPLE_KILLER, "[31;1m-_-!!Ì«²»¾´ÒµÁË£¬µÈÃ÷Íí°É![m");
					else
					{
						//int i;
						inrooms[myroom].dead = pid;
						if (inrooms[myroom].killernum != -1)
							sprintf (buf, "ºÃ£¬¾ö¶¨ÁË£¬¾ÍÉ±\33[32;1m %s \33[m!", inrooms[myroom].peoples[sel].nick);
						else
							sprintf (buf, "ºÃ£¬ÏÈÏÅ[32;1m %s [mÒ»ÏÅ°É!", inrooms[myroom].peoples[sel].nick);
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
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m±¾ÂÖ¾¯²ìÒÑ¾­Õì²é¹ıÁË\33[m");
						refreshit ();
						kill_msg (-1);
						continue;
					}
					if(inrooms[myroom].peoples[sel].flag&PEOPLE_SPECTATOR)
						send_to_fellow(PEOPLE_POLICE, "[31;1m´ËÈËÊÇÅÔ¹ÛÕß[m");
					else if(!(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE))
						send_to_fellow(PEOPLE_POLICE, "[31;1m´ËÈËÒÑËÀ[m");
					else if (inrooms[myroom].peoples[sel].flag&PEOPLE_POLICE)
						send_to_fellow(PEOPLE_POLICE, "\33[31;1mÕâÊÇ×Ô¼ºÈË~~~\33[m");
					else if((inrooms[myroom].peoples[sel].flag&PEOPLE_KILLER))
					{
						inrooms[myroom].test = sel;
						sprintf (buf,"ÄÇÓÇÓôµÄÑÛÉñ¡¢ßñĞêµÄĞë¸ù¡¢ÉñºõÆä¼¼µÄµ¶·¨£¬ÔçÒÑ°ÑËû³öÂô...Ã»´í£¬\33[32;1m%s \33[m¾ÍÊÇÒ»Î»³öÉ«µÄÉ±ÊÖ!", inrooms[myroom].peoples[sel].nick);
						send_to_fellow (PEOPLE_POLICE, buf);
					}
					else
					{
						/* Efan: ÄÏ¿ª°æ */
						sprintf (buf, "µ÷²é\33[32;1m %s \33[mµÄÉí·İ", inrooms[myroom].peoples[sel].nick);
						send_to_fellow(PEOPLE_POLICE, buf);
						inrooms[myroom].test = sel;
						send_to_fellow(PEOPLE_POLICE, "\33[31;1m±¿±¿£¬²Â´íÁËÀ²£¬ÕâÃ÷Ã÷ÊÇºÃÈËÂï~~~\33[m");
					}
					kill_msg (-1);
					if (inrooms[myroom].test != -1 && inrooms[myroom].dead != -1)
					{
						/* Efan: ÄÏ¿ª°æ£¬ÊÇÊ±ºò×ªÈë°×ÌìÁË */
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
							sprintf(buf, "[32;1mÍ¶%d %sÒ»Æ±[m", sel+1, inrooms[myroom].peoples[sel].nick);
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
									sprintf (buf, "[32;1m%d %sÍ¶ÁË%d %sÒ»Æ±[m", i + 1, inrooms[myroom].peoples[i].nick, tmp + 1, inrooms[myroom].peoples[tmp].nick);
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
							/* Efan: ÒÔÏÂ´úÂëÓÃÓÚÕÒ³öÇ°Á½ÃûµÄÆ±Êı£¬²»ÊÇÎÒĞ´µÄ */
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

							sprintf(buf, "\33[31;1mµÚ %d ÂÖÍ¶Æ±½á¹û:\33[m", inrooms[myroom].rotate);
							send_msg(-1, buf);
							for(i=0;i<MAX_PEOPLE;i++)
								if(inrooms[myroom].peoples[i].style!=-1)
									if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) && inrooms[myroom].peoples[i].flag & PEOPLE_VOTED &&
									        inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
									{
										sprintf(buf, "%s±»Í¶: %d Æ±",
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
							/* Efan: ±ØĞë¹ı°ëÊı£¬·½ËãÓĞĞ§ */
							if (t1 > t3/2)
								ok = 1;
							else
								ok = 0;
							if(!ok && inrooms[myroom].rotate < 3)
							{
								int t;
								struct people_struct * ppeop;
								send_msg(-1, "\33[31m×î¸ßÆ±ÊıÎ´³¬¹ı°ëÊı,ÇëÖØĞÂÍ¶Æ±...\33[m");
								start_change_inroom();
								t = 0;
								ppeop = inrooms[myroom].peoples;
								if (inrooms[myroom].rotate == 1)
								{//ÄæÊ±Õë
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
								{//inrooms[myroom].rotate == 2 Ë³Ê±Õë
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
									send_msg (-1, "\33[1;31mÓÎÏ·Òì³£½áÊø\33[m");
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
								send_msg (-1, "\33[31;1m»ñµÃ×î¶àÆ±ÊıµÄ¹«ÃñÇëÎª×Ô¼º±ç»¤\33[m");
								sprintf (buf, "Ê×ÏÈÊÇ\33[32;1m %d %s \33[m·¢ÑÔ", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
								send_msg (-1, buf);
								send_msg (inrooms[myroom].voted[0], "±ç»¤Íêºó£¬Çë°´\33[31;1mCtrl+T\33[m½áÊø·¢ÑÔ");
							}
							else
							{
								int a=0,b=0;
								int k;
								struct people_struct * ppeop = inrooms[myroom].peoples;
								/* Efan: Í¶Æ±Èı´ÎÈÔÎŞ½á¹û£¬·¨¹ÙËæ»ú³éÈ¡Ò»ÈË */
								if (!ok && inrooms[myroom].rotate >= 3)
								{
									send_msg (-1, "\33[31;1mÍ¶Æ±Èı´ÎÈÔÎ´ÓĞ½á¹û£¬·¨¹ÙËæ»ú´¦¾ö\33[m");
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
										send_msg (-1, "[1;31mÓÎÏ·Òì³£½áÊø[m");
										inrooms[myroom].status = INROOM_STOP;
										end_change_inroom ();
										kill_msg (-1);
										continue;
									}
									max = ppeop[a].vnum;
									maxi = a;
									maxpid = ppeop[a].pid;
								}
								send_msg(maxi, "\33[31;1mÄã±»·¨¹Ù´¦¾öÁË!\33[m");
								sprintf (buf, "\33[32;1m %d %s \33[31;1m±»·¨¹Ù´¦¾öÁË\33[m", maxi + 1, ppeop[maxi].nick);
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
						                      send_msg(me, "[31;1mÄã²»ÄÜÑ¡Ôñ×ÔÉ±[m");
						                  else if(!(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE))
						                      send_msg(me, "[31;1m´ËÈËÒÑËÀ[m");
						                  else
						                      send_msg(me, "[31;1m´ËÈËÊÇÅÔ¹ÛÕß[m");
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

				/* Efan: ÏÂÃæ´úÂëÍê³É¸Õ×ªÈë°×ÌìÊ±£¬ËÀÕß·¢ÑÔµÄ¹¦ÄÜ */
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
					sprintf (buf, "ÄÇÃ´£¬Çë\33[35;1m %d %s \33[mÅóÓÑÒÔÅÔ¹ÛÕßµÄÉí·İ£¬¼ÌĞøÓÎÏ·...", me + 1, ppeop [me].nick);
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
						send_msg (-1, "\33[31;1mÓÎÏ·Òì³£½áÊø!\33[m");
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
					send_msg (-1, "\33[31;1mÏÖÔÚ¿ªÊ¼Ö¸Ö¤\33[m");
					sprintf (buf,"ÏÈÇë\33[32;1m %d %s \33[m·¢ÑÔ", inrooms[myroom].voted[turn] + 1, ppeop [inrooms[myroom].voted[turn]].nick);
					send_msg (-1, buf);
					send_msg (inrooms[myroom].voted[turn], "Çë[31;1mÉÏÏÂÒÆ¶¯¹â±ê[m£¬ÔÚÄú»³ÒÉµÄ¶ÔÏóÉÏ");
					send_msg (inrooms[myroom].voted[turn], "°´\33[31;1mCtrl+S\33[mÖ¸Ö¤");
					send_msg (inrooms[myroom].voted[turn], "Ö¸Ö¤Íêºó£¬ÓÃ\33[31;1mCtrl+T\33[m½áÊø·¢ÑÔ");
					if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
						send_msg (inrooms[myroom].voted[turn], "\33[31;1m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±\33[m");
					kill_msg (-1);
					continue;
				}
				/* Efan: Ö¸Ö¤ */
				if (inrooms[myroom].status == INROOM_CHECK)
				{
					struct people_struct * ppeop = inrooms[myroom].peoples;
					int turn = inrooms[myroom].turn;
					if (me != inrooms[myroom].voted[turn])
						continue;
					if (inrooms[myroom].test == -1)
					{
						send_msg (me, "ÇëÏÈÖ¸Ö¤Ò»¸öÈË");
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
						sprintf (buf, "\33[32;1m%d %s Ö¸Ö¤ %d %s\33[m", me+1, ppeop[me].nick, inrooms[myroom].test+1, ppeop[inrooms[myroom].test].nick);
					else
						sprintf (buf, "\33[31;1mÉ±ÊÖ %d %s ±ÀÀ£ÁË¡­¡­\33[m", me + 1, ppeop[me].nick);
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
							send_msg (-1, "\33[31;1mÓÎÏ·Òì³£½áÊø!\33[m");
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
						send_msg (-1, "\33[31;1mÖ¸Ö¤½áÊø\33[m");
						send_msg (-1, "\33[31;1mÏÂÃæÇë±»Ö¸Ö¤µÄ¹«Ãñ·¢ÑÔ\33[m");
						sprintf (buf, "Ê×ÏÈÊÇ\33[32;1m %d %s \33[mÎª×Ô¼º±ç»¤", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[0], "±ç»¤Íêºó£¬Çë°´\33[31;1mCtrl+T\33[m½áÊø·¢ÑÔ");
						if (ppeop[inrooms[myroom].voted[0]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[0], "\33[31;1m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±\33[m");
					}
					else
					{
						sprintf (buf, "ÏÂÃæÊÇ\33[32;1m %d %s \33[m·¢ÑÔ", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[turn], "Çë[31;1mÉÏÏÂÒÆ¶¯¹â±ê[m£¬ÔÚÄú»³ÒÉµÄ¶ÔÏóÉÏ");
						send_msg (inrooms[myroom].voted[turn], "°´\33[31;1mCtrl+S\33[mÖ¸Ö¤");
						send_msg (inrooms[myroom].voted[turn], "Ö¸Ö¤Íêºó£¬ÓÃ\33[31;1mCtrl+T\33[m½áÊø·¢ÑÔ");
						if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[turn], "\33[31;1m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±\33[m");
						inrooms[myroom].test = -1;
					}
					end_change_inroom ();
					kill_msg (-1);
					continue;
				}
				/* Efan: ±ç»¤ */
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
						sprintf (buf, "[31;1mÉ±ÊÖ %d %s ±ÀÀ£ÁË¡­¡­[m", me + 1, inrooms[myroom].peoples[me].nick);
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
						send_msg (-1, "\33[31;1m±ç»¤½áÊø£¬ÏÖÔÚ¿ªÊ¼Í¶Æ±\33[m");
						send_msg (-1, "\33[31;1mÇëÉÏÏÂÒÆ¶¯¹â±êÑ¡Ôñ¶ÔÏó£¬ÓÃCtrl+SÍ¶Æ±...\33[m");
						for (i = 0; i < MAX_PEOPLE; ++i)
							ppeop[i].vote = 0;
						inrooms[myroom].status = INROOM_VOTE;
					}
					else
					{
						sprintf (buf, "ÏÂÃæÊÇ\33[32;1m %d %s \33[mÎª×Ô¼º±ç»¤", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
						send_msg (-1, buf);
						send_msg (inrooms[myroom].voted[turn], "±ç»¤Íêºó£¬Çë°´\33[31;1mCtrl+T\33[m½áÊø·¢ÑÔ");
						if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
							send_msg (inrooms[myroom].voted[turn], "\33[31;1m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±\33[m");
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
					send_msg (-1, "\33[1;31m°¢ÃÅ...\33[m");
					start_change_inroom ();
					gotonight ();
					end_change_inroom ();
					continue;
				}
				/* Efan: É±ÊÖ»î¶¯ */
				/*
				                if(inrooms[myroom].status==INROOM_NIGHT) {
							if (inrooms[myroom].peoples[me].style == -1 || inrooms[myroom].peoples[me].flag & PEOPLE_SPECTATOR || !(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER) || !(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE))
							       continue;
							if (inrooms[myroom].peoples[sel].flag & PEOPLE_SPECTATOR) {
								send_msg (me, "\33[31;1mÕâÊÇÅÔ¹ÛÕß\33[m");
								refreshit ();
								continue;
							}
							else if (!(inrooms[myroom].peoples[sel].flag & PEOPLE_ALIVE)) {
								send_msg (me, "\33[31;1m´ËÈËÒÑËÀ\33[m");
								refreshit ();
								continue;
							}
							if (inrooms[myroom].dead != -1)
								send_to_fellow (PEOPLE_KILLER, "\33[31;1m-_-!!Ì«²»¾´ÒµÁË£¬ÄÄÓĞÒ»ÍíÉÏÉ±Á½¸öÈËµÄ¡­¡­\33[m");
							else {
								int i;
								inrooms[myroom].dead = pid;
								sprintf (buf, "ºÃ£¬¾ö¶¨ÁË£¬¾ÍÉ±\33[32;1m %s \33[m!", inrooms[myroom].peoples[sel].nick);
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
					sprintf(buf, "%s [1m%s[m %s", party_data[i].part1_msg, buf2[0]?buf2:"´ó¼Ò", party_data[i].part2_msg);
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
				/* Efan: ÍíÉÏÃØÃÜËµ»° */
				int flag = inrooms[myroom].peoples[me].flag;
				if (inrooms[myroom].peoples[me].style != -1 &&
				        flag & PEOPLE_ALIVE && (flag & PEOPLE_KILLER || flag & PEOPLE_POLICE))
					send_to_fellow (flag, buf);
			}
			else if (inrooms[myroom].status == INROOM_STOP)
			{
				/* Efan: ·ÇÓÎÏ·½øĞĞÊ±£¬¿ÉÈÎÒâ·¢ÑÔ */
				/*
						if (!(inrooms[myroom].peoples[me].flag&PEOPLE_SPECTATOR))
				*/
				send_msg (-1, buf);
			}
			else if (inrooms[myroom].status == INROOM_DARK)
			{
				/* Efan: Í¶Æ±½áÊøºó£¬ËÀÕßÁôÒÅÑÔ */
				if (me == inrooms[myroom].turn)
					send_msg( -1, buf);
			}
			else
			{
				/* Efan: ÔÚÆäËûÊ±ºò£¬Ö»ÄÜÂÖÁ÷·¢ÑÔ */
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
					send_msg(i, "Äã±»ÌßÁË");
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
	        sprintf(buf, "É±ÊÖ%sÇ±ÌÓÁË", buf2);
	    else
	        sprintf(buf, "%sÀë¿ª·¿¼ä", buf2);
	    for(i=0;i<myroom->people;i++) {
	        send_msg(inrooms.peoples+i, buf);
	        kill(inrooms.peoples[i].pid, SIGUSR1);
	    }*/
quitgame2:
	kicked=0;
	k_getdata(t_lines-1, 0, "¼Ä»Ø±¾´ÎÈ«²¿ĞÅÏ¢Âğ?[y/N]", buf3, 3, 1, 0, 1);
	if(toupper(buf3[0])=='Y')
	{
		sprintf(buf, "tmp/%d.msg", rand());
		save_msgs(buf);
		sprintf(buf2, "\"%s\"µÄÉ±ÈË¼ÇÂ¼", roomname);
		mail_file(buf, currentuser.userid, buf2);

	}
	//    signal(SIGUSR1, talk_request);
}

static int room_list_refresh(struct _select_def *conf)
{
	clear();
	docmdtitle("[ÓÎÏ·ÊÒÑ¡µ¥]",
	           "  ÍË³ö[[1;32m¡û[0;37m,[1;32me[0;37m] ½øÈë[[1;32mEnter[0;37m] Ñ¡Ôñ[[1;32m¡ü[0;37m,[1;32m¡ı[0;37m] Ìí¼Ó[[1;32ma[0;37m] ¼ÓÈë[[1;32mJ[0;37m] [m      ×÷Õß: [31;1mbad@smth.org[m");
	move(2, 0);
	prints("[0;1;37;44m    %4s %-14s %-12s %4s %4s %6s %-20s[m", "±àºÅ", "ÓÎÏ·ÊÒÃû³Æ", "´´½¨Õß", "ÈËÊı", "×î¶à", "ËøÒşÅÔ", "»°Ìâ");
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
		prints("  %3d  %-14s %-12s %3d  %3d  %2s%2s%2s %-36s", i, r->name, r->creator, r->people, r->maxpeople, (r->flag&ROOM_LOCKED)?"¡Ì":"", (r->flag&ROOM_SECRET)?"¡Ì":"", (!(r->flag&ROOM_DENYSPEC))?"¡Ì":"", r->title);
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
		prints(" ¸Ã·¿¼äÒÑ±»Ëø¶¨!");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	r2 = rooms+j;
	if(r2->people>=r2->maxpeople&&!HAS_PERM(PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" ¸Ã·¿¼äÈËÊıÒÑÂú");
		refresh();
		sleep(1);
		return SHOW_REFRESH;
	}
	k_getdata(0, 0, "ÊÇ·ñÒÔÅÔ¹ÛÕßÉí·İ½øÈë? [y/N]", ans, 3, 1, NULL, 1);
	if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC&&!HAS_PERM(PERM_SYSOP))
	{
		move(0, 0);
		clrtoeol();
		prints(" ¸Ã·¿¼ä¾Ü¾øÅÔ¹ÛÕß");
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
		k_getdata(0, 0, "·¿¼äÃû:", name, 13, 1, NULL, 1);
		if(!name[0])
			return SHOW_REFRESH;
		if(name[0]==' '||name[strlen(name)-1]==' ')
		{
			move(0, 0);
			clrtoeol();
			prints(" ·¿¼äÃû¿ªÍ·½áÎ²²»ÄÜÎª¿Õ¸ñ");
			refresh();
			sleep(1);
			return SHOW_CONTINUE;
		}
		strcpy(r.name, name);
		r.style = 1;
		r.flag = 0;
		r.people = 0;
		r.maxpeople = 100;
		strcpy(r.title, "É±ÈËÓĞº¦½¡¿µ...");
		if(add_room(&r)==-1)
		{
			move(0, 0);
			clrtoeol();
			prints(" ÓĞÒ»ÑùÃû×ÖµÄ·¿¼äÀ²!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		join_room(find_room(r.name), 0);
		return SHOW_DIRCHANGE;
	case 'J':
		k_getdata(0, 0, "·¿¼äÃû:", name, 12, 1, NULL, 1);
		if(!name[0])
			return SHOW_REFRESH;
		if((i=find_room(name))==-1)
		{
			move(0, 0);
			clrtoeol();
			prints(" Ã»ÓĞÕÒµ½¸Ã·¿¼ä!");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		r2 = rooms+i;
		if(r2->people>=r2->maxpeople&&!HAS_PERM(PERM_SYSOP))
		{
			move(0, 0);
			clrtoeol();
			prints(" ¸Ã·¿¼äÈËÊıÒÑÂú");
			refresh();
			sleep(1);
			return SHOW_REFRESH;
		}
		k_getdata(0, 0, "ÊÇ·ñÒÔÅÔ¹ÛÕßÉí·İ½øÈë? [y/N]", ans, 3, 1, NULL, 1);
		if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC&&!HAS_PERM(PERM_SYSOP))
		{
			move(0, 0);
			clrtoeol();
			prints(" ¸Ã·¿¼ä¾Ü¾øÅÔ¹ÛÕß");
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
	grouplist_conf.prompt = "¡ô";
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
	{//»µÈËÊ¤ÁË
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m»µÈË»ñµÃÁËÊ¤Àû...\33[m");
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
			if (ppeop[i].style != -1 && ppeop[i].flag & PEOPLE_KILLER && ppeop[i].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "Ô­À´\33[32;1m %d %s \33[mÊÇÉ±ÊÖ!", i + 1, ppeop[i].nick);
				send_msg (-1, buf);
			}
	}
	else if (a == 0)
	{//ºÃÈËÊ¤ÁË
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1mËùÓĞÉ±ÊÖ¶¼±»´¦¾öÁË£¬ºÃÈË»ñµÃÁËÊ¤Àû...\33[m");
		save_result (0);
	}
	else
	{//ÓÎÏ·»¹ÔÚ¼ÌĞø
		inrooms[myroom].status = INROOM_DARK;
		inrooms[myroom].turn = maxi;
		if (ppeop[maxi].flag & PEOPLE_KILLER)
		{
			send_msg (-1, "\33[31;1mÃ»ÓĞÒÅÑÔ\33[m");
			gotonight ();
		}
		else
		{
			sprintf (buf, "\33[32;1m%d %s \33[31;1mÇëÁôÏÂÒÅÑÔ\33[m", maxi + 1, ppeop[maxi].nick);
			send_msg (-1, buf);
			send_msg (maxi, "°´\33[31;1mCtrl+T\33[m½áÊøÁôÑÔ");
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
	{//»µÈËÊ¤ÁË
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1m»µÈË»ñµÃÁËÊ¤Àû...\33[m");
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
			if (ppeop[i].style != -1 && ppeop[i].flag & PEOPLE_KILLER && ppeop[i].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "Ô­À´\33[32;1m %s \33[mÊÇÉ±ÊÖ!", ppeop[i].nick);
				send_msg (-1, buf);
			}
	}
	else if (a == 0)
	{//ºÃÈËÊ¤ÁË
		inrooms[myroom].status = INROOM_STOP;
		send_msg (-1, "\33[31;1mËùÓĞÉ±ÊÖ¶¼±»´¦¾öÁË£¬ºÃÈË»ñµÃÁËÊ¤Àû...\33[m");
		save_result (0);
	}
	else
	{//ÓÎÏ·»¹ÔÚ¼ÌĞø
		send_msg (-1, "[31;1mÈ¡ÏûÍ¶Æ±[m");
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
	send_msg (-1, "\33[31;1m¿Ö²ÀµÄÒ¹É«ÓÖ½µÁÙÁË...\33[m");
	send_to_fellow (PEOPLE_KILLER, "Çë×¥½ôÄãµÄ±¦¹óÊ±¼äÓÃ\33[31;1mCtrl+S\33[mÉ±ÈË");
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
			sprintf (buf,"[1;31m²»ĞÒµÄÊÇ£¬±»º¦ÕßµôÏßÁË¡­¡­[m");
		else
			sprintf (buf, "\33[1;33mÒõÉ­É­µÄÔÂ¹âÏÂ£¬ºáÌÉ×Å\33[32;1m %d %s \33[33mµÄÊ¬Ìå...\33[m", i + 1, inrooms[myroom].peoples[i].nick);
		send_msg (-1, buf);
		send_msg (i, "º¢×Ó£¬»¶Ó­ÄãÀ´µ½ÌìÌÃ£¬Äã±»Ğ×ÊÖ²ĞÈÌµØÉ±º¦ÁË...");
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
		send_msg (-1, "\33[31;1m»µÈË»ñµÃÁËÊ¤Àû...\33[m");
		inrooms[myroom].status = INROOM_STOP;
		save_result (1);
		for (i = 0; i < MAX_PEOPLE; ++i)
		{
			register int flag = inrooms[myroom].peoples[i].flag;
			if (inrooms[myroom].peoples[i].style != -1 && flag & PEOPLE_KILLER && flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "Ô­À´\33[32;1m %d %s \33[mÊÇÉ±ÊÖ!", i + 1, inrooms[myroom].peoples[i].nick);
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
		send_msg (-1, "\33[31;1mËùÓĞÉ±ÊÖ¶¼±»´¦¾öÁË£¬ºÃÈË»ñµÃÁËÊ¤Àû...\33[m");
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
		send_msg (-1, "ÌìÁÁÁË...");
	}
	for (i = 0; i < MAX_PEOPLE; ++i)
	{
		inrooms[myroom].peoples[i].vote = 0;
		inrooms[myroom].peoples[i].flag &= ~PEOPLE_VOTED;
	}
	if (inrooms[myroom].killernum != -1 && j < MAX_PEOPLE)
	{
		sprintf (buf, "ÏÖÔÚ£¬±»º¦Õß\33[32;1m %d %s \33[mÔÚÌìÌÃÊ²Ã´»°ÒªËµ£¿", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "Çë°´\33[31;1mCtrl+T\33[m½áÊøÒÅÑÔ");
	}
	else if (j < MAX_PEOPLE)
	{
		inrooms[myroom].test = -1;
		send_msg (-1, "\33[31;1mÏÖÔÚ¿ªÊ¼Ö¸Ö¤\33[m");
		sprintf (buf,"ÏÈÇë\33[32;1m %d %s \33[m·¢ÑÔ", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "Çë[31;1mÉÏÏÂÒÆ¶¯¹â±ê[m£¬ÔÚÄú»³ÒÉµÄ¶ÔÏóÉÏ");
		send_msg (j, "°´\33[31;1mCtrl+S\33[mÖ¸Ö¤");
		send_msg (j, "Ö¸Ö¤Íêºó£¬ÓÃ\33[31;1mCtrl+T\33[m½áÊø·¢ÑÔ");
		if (inrooms[myroom].peoples[j].flag & PEOPLE_KILLER)
			send_msg (j, "\33[31;1m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±\33[m");
		else
			send_msg (j, "\33[31;1mĞ¡ĞÄ£¡É±ÊÖÒÑ¾­¶¢ÉÏÄãÁË£¡£¡\33[m");
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
		send_msg (-1, "[1;31mÓÎÏ·Òì³£½áÊø[m");
		inrooms[myroom].status = INROOM_STOP;
		kill_msg (-1);
		end_change_inroom ();
		return 0;
	}
	if (inrooms[myroom].dead >= MAX_PEOPLE)
	{
		send_msg (-1, "[1;31mÏÖÔÚ¿ªÊ¼Ö¸Ö¤[m");
		sprintf (buf, "ÏÈÇë[1;32m %d %s [m·¢ÑÔ", j + 1, inrooms[myroom].peoples[j].nick);
		send_msg (-1, buf);
		send_msg (j, "Çë[1;31mÉÏÏÂÒÆ¶¯¹â±ê[m£¬ÔÚÄú»³ÒÉµÄ¶ÔÏóÉÏ");
		send_msg (j, "°´[1;31mCtrl+S[mÖ¸Ö¤");
		send_msg (j, "Ö¸Ö¤Íêºó£¬ÓÃ[1;31mCtrl+T[m½áÊø·¢ÑÔ");
		if (inrooms[myroom].peoples[j].flag & PEOPLE_KILLER)
			send_msg (j, "[1;31m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±");
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
			sprintf (buf, "[1;31mÎ¨Ò»µÄÉ±ÊÖ[1;32m %d %s [1;31mµôÏßÁË£¬ºÃÈË»ñµÃÁËÊ¤Àû£¡", i + 1, ppeop[i].nick);
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
			sprintf (buf, "[1;31mÎ¨Ò»µÄ¾¯²ì[1;32m %d %s [1;31mµôÏßÁË£¬»µÈË»ñµÃÁËÊ¤Àû£¡", i + 1, ppeop[i].nick);
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
			sprintf (buf, "[1;31mÎ¨Ò»µÄÆ½Ãñ[1;32m %d %s [1;31mµôÏßÁË£¬»µÈË»ñµÃÁËÊ¤Àû£¡", i + 1, ppeop[i].nick);
			send_msg (-1, buf);
			inrooms[myroom].status = INROOM_STOP;
		}
		break;
	case INROOM_DAY:
		if (i != inrooms[myroom].voted[turn])
			return;
		sprintf (buf, "[1;31mfaint...[1;35m %d %s [1;31m»¹Ã»ËµÍê¾ÍµôÏßÁË~~~", i + 1, ppeop[i].nick);
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
			send_msg (-1, "[1;31mÓÎÏ·Òì³£½áÊø£¡[m");
			inrooms[myroom].status = INROOM_STOP;
			return;
		}
		inrooms[myroom].turn = turn;
		inrooms[myroom].status = INROOM_CHECK;
		inrooms[myroom].test = -1;
		for (j = 0; j < MAX_PEOPLE; ++j)
			ppeop[j].vote = 0;
		send_msg (-1, "[1;31mÏÖÔÚ¿ªÊ¼Ö¸Ö¤[m");
		sprintf (buf, "ÏÈÇë[1;32m %d %s [m·¢ÑÔ", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
		send_msg (-1, buf);
		send_msg (inrooms[myroom].voted[turn], "Çë[1;31mÉÏÏÂÒÆ¶¯¹â±ê[m£¬ÔÚÄú»³ÒÉµÄ¶ÔÏóÉÏ");
		send_msg (inrooms[myroom].voted[turn], "°´[1;31mCtrl+S[mÖ¸Ö¤");
		send_msg (inrooms[myroom].voted[turn], "Ö¸Ö¤Íêºó£¬ÓÃ[1;31mCtrl+T[m½áÊø·¢ÑÔ");
		if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
			send_msg (inrooms[myroom].voted[turn], "[1;31m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±[m");
		break;
	case INROOM_CHECK:
		if (i != inrooms[myroom].voted[turn])
			return;
		++turn;
		sprintf (buf, "[1;32m%d %s[m: [1;31mOver.[m", i + 1, ppeop[i].nick);
		send_msg (-1, buf);
		if (inrooms[myroom].suicide == i)
		{
			sprintf (buf, "[1;31mÉ±ÊÖ[1;32m %d %s [1;31m±ÀÀ£ÁË¡­¡­[m", i + 1, ppeop[i].nick);
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
				send_msg (-1, "[1;31mÓÎÏ·Òì³£½áÊø£¡[m");
				inrooms[myroom].status = INROOM_STOP;
				return;
			}
			inrooms[myroom].numvoted = t;
			inrooms[myroom].turn = 0;
			inrooms[myroom].status = INROOM_DEFENCE;
			for (k = 0; k < MAX_PEOPLE; ++k)
				ppeop[k].vote = 0;
			send_msg (-1, "[1;31mÖ¸Ö¤½áÊø[m");
			send_msg (-1, "[1;31mÏÂÃæÇë±»Ö¸Ö¤µÄ¹«Ãñ·¢ÑÔ[m");
			sprintf (buf, "Ê×ÏÈÊÇ[1;32m %d %s [mÎª×Ô¼º±ç»¤", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[0], "±ç»¤Íêºó£¬ÇëÓÃ[1;31mCtrl+T[m½áÊø·¢ÑÔ");
			if (ppeop[inrooms[myroom].voted[0]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[0], "[1;31m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±[m");
		}
		else
		{
			sprintf (buf, "ÏÂÃæÊÇ[1;32m %d %s [m·¢ÑÔ", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[turn], "Çë[1;31mÉÏÏÂÒÆ¶¯¹â±ê[m£¬ÔÚÄú»³ÒÉµÄ¶ÔÏóÉÏ");
			send_msg (inrooms[myroom].voted[turn], "°´[1;31mCtrl+S[mÖ¸Ö¤");
			send_msg (inrooms[myroom].voted[turn], "Ö¸Ö¤Íêºó£¬ÓÃ[1;31mCtrl+T[m½áÊø·¢ÑÔ");
			if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[turn], "[1;31m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±[m");
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
			sprintf (buf, "[1;31mÉ±ÊÖ[1;32m %d %s [1;31m±ÀÀ£ÁË¡­¡­[m", i + 1, ppeop[i].nick);
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
			send_msg (-1, "[1;31m±ç»¤½áÊø£¬ÏÖÔÚ¿ªÊ¼Í¶Æ±[m");
			send_msg (-1, "[1;31mÇëÉÏÏÂÒÆ¶¯¹â±êÑ¡Ôñ¶ÔÏó£¬ÓÃCtrl+SÍ¶Æ±...[m");
			for (j = 0; j < MAX_PEOPLE; ++j)
				ppeop[j].vote = 0;
			inrooms[myroom].status = INROOM_VOTE;
		}
		else
		{
			sprintf (buf, "ÏÂÃæÊÇ[1;31m %d %s [mÎª×Ô¼º±ç»¤", inrooms[myroom].voted[turn] + 1, ppeop[inrooms[myroom].voted[turn]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[turn], "±ç»¤Íêºó£¬Çë°´[1;31mCtrl+T[m½áÊø·¢ÑÔ");
			if (ppeop[inrooms[myroom].voted[turn]].flag & PEOPLE_KILLER)
				send_msg (inrooms[myroom].voted[turn], "[1;31m°Ñ¹â±êÒÆµ½×Ô¼ºÎ»ÖÃÉÏ£¬ÓÃCtrl+S¿É×ÔÉ±[m");
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
				sprintf (buf, "[1;32m%d %sÍ¶ÁË%d %sÒ»Æ±[m", j + 1, ppeop[j].nick, tmp + 1, ppeop[tmp].nick);
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
		sprintf (buf, "[1;31mµÚ %d ÂÖÍ¶Æ±½á¹û:[m", inrooms[myroom].rotate);
		send_msg (-1, buf);
		for (j = 0; j < MAX_PEOPLE; ++j)
			if (ppeop[j].style != -1 && !(ppeop[j].flag & PEOPLE_SPECTATOR) && ppeop[j].flag & PEOPLE_VOTED && ppeop[j].flag & PEOPLE_ALIVE)
			{
				sprintf (buf, "%s±»Í¶: %d Æ±", ppeop[j].nick, ppeop[j].vnum);
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
				send_msg (-1, "[1;31mÓÎÏ·Òì³£½áÊø[m");
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
			send_msg (-1, "[1;31m»ñµÃ×î¶àÆ±ÊıµÄ¹«ÃñÇëÎª×Ô¼º±ç»¤[m");
			sprintf (buf, "Ê×ÏÈÊÇ[1;32m %d %s [m·¢ÑÔ", inrooms[myroom].voted[0] + 1, ppeop[inrooms[myroom].voted[0]].nick);
			send_msg (-1, buf);
			send_msg (inrooms[myroom].voted[0], "±ç»¤Íêºó£¬Çë°´[1;31mCtrl+T[m½áÊø·¢ÑÔ");
		}
		else
		{
			int a = 0, b = 0;
			if (!ok && inrooms[myroom].rotate >= 3)
			{
				send_msg (-1, "[1;31mÍ¶Æ±Èı´ÎÈÔÎ´ÓĞ½á¹û£¬·¨¹ÙËæ»ú´¦¾ö[m");
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
					send_msg (-1, "[1;31mÓÎÏ·Òì³£½áÊø[m");
					inrooms[myroom].status = INROOM_STOP;
					kill_msg (-1);
					return;
				}
				max = ppeop[a].vnum;
				maxi = a;
				maxpid = ppeop[a].pid;
			}
			send_msg (maxi, "[1;31mÄã±»·¨¹Ù´¦¾öÁË![m");
			sprintf (buf, "[1;32m %d %s [1;31m±»·¨¹Ù´¦¾öÁË[m", maxi + 1, ppeop[maxi].nick);
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
		send_msg (-1, "[1;31m°¢ÃÅ...[m");
		gotonight ();
		break;
	}
}
