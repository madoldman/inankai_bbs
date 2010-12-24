#define FIRST_PAGE      "/index.html"
#define CSS_FILE        "/bbs.css"
#define CHARSET         "gbk"
#define UCACHE_SHMKEY   get_shmkey("UCACHE_SHMKEY")
#define UTMP_SHMKEY     get_shmkey("UTMP_SHMKEY")
#define BCACHE_SHMKEY   get_shmkey("BCACHE_SHMKEY")
#define UPLOAD_PATH     "/upload/upload2"
#define NETSAVE_PATH    "/home/www/netsave"
#define FILE_PATH       "/home/www/file"
#define SECNUM 14
#define DOCROOT         "/home/www/html"
#define color0          "#009966"
#define color1          "#DDF0E7"
#define tablec          "#FFFFFF"
char seccode[SECNUM][5]={
       "a", "by", "cde", "fg", "hi", "jk", "lm", "no", "pq", "rs", "tuv", "w","x","zA",
};
#define PATHRMD "/home/www/html/news/recommanded"
#define MAXINDEX 10
#define PATHLEN 256
#define MAXTOP 6
#define HOTBOARDS "/home/www/html/news/hotboards"
#define RMDBOARDS "/home/www/html/news/boards"
char secname[SECNUM][2][24]={
        "本站系统", "[本站]",
        "南开大学", "[本校][院系]",
        "电脑技术", "[电脑][系统][网络]",
        "视觉联盟", "[文商][理工]",
        "文学艺术", "[文学][艺术]",
        "人文社会", "[人文][社会]",
        "服务特区", "[校园][生活]",
        "休闲娱乐", "[休闲][娱乐]",
        "知性感性", "[谈天][感性]",
        "体育运动", "[体育][运动]",
        "社团群体",   "[社团][群体]",
        "游戏天堂  ", "[游戏]",
        "新开板面", "[新开]",
	"四海一家", "[地区][友校]"
};

char seczonename[SECNUM][24]={
     "0区",  // "本站系统", "[本站]",
     "1区",   // "南开大学", "[本校][院系]",
     "2区",   // "电脑技术", "[电脑][系统][网络]",
     "3区",   // "学术科学", "[文商][理工]",
     "4区",   // "文学艺术", "[文学][艺术]",
     "5区",  //  "人文社会", "[人文][社会]",
     "6区",  //  "服务特区", "[校园][生活]",
     "7区",  //  "休闲娱乐", "[休闲][娱乐]",
     "8区",  //  "知性感性", "[谈天][感性]",
     "9区",  //  "体育运动", "[体育][运动]",
     "C区",  //  "群体区",   "[社团][群体]",
     "G区", //  "游戏天堂  ", "[游戏]",
     "N区",  //  "新开板面", "[新开]",
     "F区",  //  "四海一家", "[地区][友校]"
};

int NOTADDCSSFILE=0;//add by bluetent 2002.11.1 设置是否嵌入bbs.css
