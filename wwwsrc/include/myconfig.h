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
        "��վϵͳ", "[��վ]",
        "�Ͽ���ѧ", "[��У][Ժϵ]",
        "���Լ���", "[����][ϵͳ][����]",
        "�Ӿ�����", "[����][��]",
        "��ѧ����", "[��ѧ][����]",
        "�������", "[����][���]",
        "��������", "[У԰][����]",
        "��������", "[����][����]",
        "֪�Ը���", "[̸��][����]",
        "�����˶�", "[����][�˶�]",
        "����Ⱥ��",   "[����][Ⱥ��]",
        "��Ϸ����  ", "[��Ϸ]",
        "�¿�����", "[�¿�]",
	"�ĺ�һ��", "[����][��У]"
};

char seczonename[SECNUM][24]={
     "0��",  // "��վϵͳ", "[��վ]",
     "1��",   // "�Ͽ���ѧ", "[��У][Ժϵ]",
     "2��",   // "���Լ���", "[����][ϵͳ][����]",
     "3��",   // "ѧ����ѧ", "[����][��]",
     "4��",   // "��ѧ����", "[��ѧ][����]",
     "5��",  //  "�������", "[����][���]",
     "6��",  //  "��������", "[У԰][����]",
     "7��",  //  "��������", "[����][����]",
     "8��",  //  "֪�Ը���", "[̸��][����]",
     "9��",  //  "�����˶�", "[����][�˶�]",
     "C��",  //  "Ⱥ����",   "[����][Ⱥ��]",
     "G��", //  "��Ϸ����  ", "[��Ϸ]",
     "N��",  //  "�¿�����", "[�¿�]",
     "F��",  //  "�ĺ�һ��", "[����][��У]"
};

int NOTADDCSSFILE=0;//add by bluetent 2002.11.1 �����Ƿ�Ƕ��bbs.css
