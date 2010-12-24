/*bbsnet.c*/ // NJU bbsnet, preview version, zhch@dii.nju.edu.cn, 2000.3.23 //
// HIT bbsnet, Changed by Sunner, sun@bbs.hit.edu.cn, 2000.6.11
// PSE bbsnet, Changed by zdh, dh_zheng@hotmail.com,2001.12.04
// Changed by bluetent@ytht.net, bluetent@msn.com, 2003.6.23

#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <time.h>
//#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/telnet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <math.h>
//#include <sys/file.h>
#include "bbs.h"

#define SD_SITENAME	"²¨Èô²¤ÂÜÃÛ"	//×Ô¶¨Òå´©ËóµÄÕ¾µãÃû³Æ
#define T_SITES		75	//Ã¿Ò³ÏÔÊ¾µÄÕ¾µãÊýÄ¿

FILE *fp;
char t[256], *t1, *t2, *t3, *t4;
char host1[T_SITES][40], host2[T_SITES][40], ip[T_SITES][40], ip_zdh[40];
static char buf[100];
int port[100], counts = 0, page = 0, totalpage = 0;
int total = 0;
char str[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-+<>";
char datafile[80] = BBSHOME "/etc/bbsnet.ini";
char userid[80] = "unknown.";

count_sites()
{
	fp = fopen(datafile, "r");
	if (fp == NULL)
		return;
	while (fgets(t, 255, fp))
	{
		t1 = strtok(t, " \t");
		t2 = strtok(NULL, " \t\n");
		t3 = strtok(NULL, " \t\n");
		t4 = strtok(NULL, " \t\n");
		if (t1[0] == '#' || t1 == NULL || t2 == NULL || t3 == NULL)
			continue;
		else
			total++;
	}
	if (total % T_SITES > 0)
		totalpage = total / T_SITES + 1;
	else
		totalpage = total / T_SITES;
	fclose(fp);
}
void lcase(char* pstr)//×ª»»³ÉÐ¡Ð´×Ö·û add by bluetent
{
	while(*pstr)
	{
		if(*pstr>='A'&&*pstr<='Z')
			*pstr+=32;
		pstr++;
	}
}
init_data(int pageno)
{
	int i = 0;
	counts = 0;
	fp = fopen(datafile, "r");
	if (fp == NULL)
		return;
	while (fgets(t, 255, fp) && counts < T_SITES)
	{
		t1 = strtok(t, " \t");
		t2 = strtok(NULL, " \t\n");
		t3 = strtok(NULL, " \t\n");
		t4 = strtok(NULL, " \t\n");
		if (t1[0] == '#' || t1 == NULL || t2 == NULL || t3 == NULL)
			continue;
		if (i >= pageno * T_SITES)
		{
			strncpy(host1[counts], t1, 16);
			strncpy(host2[counts], t2, 36);
			strncpy(ip[counts], t3, 36);
			port[counts] = t4 ? atoi(t4) : 23;
			port[counts] = t4 ? atoi(t4) : 23;
			counts++;
		}
		i++;
	}
	fclose(fp);
}

sh(int n)
{
	static oldn = -1;
	if (n >= counts)
		return;
	if (oldn >= 0 && oldn < counts)
	{
		locate(oldn);
		printf("[1;32m %c.[m%s", str[oldn], host2[oldn]);
	}
	oldn = n;
	if (strcmp(host2[n], SD_SITENAME) == 0)
	{
		printf
		("[22;3H[1;37mËµÃ÷: °´»Ø³µºóÊäÈëip¡£[1;33m[22;32H[1;37m Õ¾Ãû: [1;33m×Ô¶¨ÒåÕ¾µã                  \r\n");
		printf
		("[1;37m[23;3HÁ¬Íù: [1;33m__________                   [21;1H");
	}
	else
	{
		printf
		("[22;3H[1;37mµ¥Î»: [1;33m%s                   [22;32H[1;37mÕ¾Ãû: [1;33m%s              \r\n",
		 host1[n], host2[n]);
		printf
		("[1;37m[23;3HÁ¬Íù: [1;33m%s                   [21;1H",
		 ip[n]);
	}
	locate(n);
	printf("[%c][1;42m%s[m", str[n], host2[n]);
}

show_all()
{
	int n;
	char status[80];
	sprintf(status, " µÚ %d Ò³£¬¹² %d Ò³£¬%d ¸öÕ¾µã ", page + 1, totalpage,
	        total);
	printf("[H[2J[m");
	printf
	("©³©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥[1;35m Íø  Âç  ´©  Ëó [m©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©·\r\n");
	for (n = 1; n < 22; n++)
		printf
		("©§                                                                            ©§\r\n");
	printf
	("©§                                                                  [1;33mCtrl+C[1;36mÍË³ö[m©§\r\n");
	printf("\033[22;69H\033[1;33mCtrl+H\033[1;36m°ïÖú");
	printf
	("\r\n\r\n\033[1;37m©»©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¿");
	printf
	("[21;3H----------------------------------------------------------------------------\033[21;45H%s",
	 status);

	for (n = 0; n < counts; n++)
	{
		locate(n);
		printf("[1;32m %c.[m%s", str[n], host2[n]);
	}
}

locate(int n)
{
	int x, y;
	char buf[20];
	if (n >= counts)
		return;
	y = n % 19 + 2;
	x = n / 19 * 19 + 4;
	sprintf(buf, "[%d;%dH", y, x);
	printf(buf);
}

int getch()
{
	int c, d, e;
	static lastc = 0;
	c = getchar();
	while (c == '\0')
		c = getchar();
	if (c == 127)
		c = '\b';
	if (c == Ctrl('B'))
		return 517;
	if (c == Ctrl('F') || c == ' ')
		return 518;
	if (c == Ctrl('H'))
		return 261;
	if (c == Ctrl('G'))
		return 262;
	if (c == 10 && lastc == 13)
		c = getchar();
	lastc = c;
	if (c != 27)
		return c;
	d = getchar();
	e = getchar();
	if (e == 'A')
		return 257;
	if (e == 'B')
		return 258;
	if (e == 'C')
		return 259;
	if (e == 'D')
		return 260;
	if (e == '5')
		return 517;
	if (e == '6')
		return 518;
	return 0;
}

void QuitTime()
{
	reset_tty();
	exit(0);
}

void SetQuitTime()
{
	signal(SIGALRM, QuitTime);
	alarm(60);
}

static void break_check()
{
	int i, result;
	unsigned char buf[2048];
	struct timeval tv;
	fd_set readfds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	result = select(1, &readfds, NULL, NULL, &tv);
	if (FD_ISSET(0, &readfds))
	{
		result = read(0, buf, 80);
		for (i = 0; i < result; i++)
		{
			if (buf[i] == 3 || buf[i] == 4)
			{
				printf("\r\nBreak\r\n");
				QuitTime(0);
			}
		}
	}
	alarm(1);
}

main_loop()
{
	int p = 0;
	int c, n = 0;
	char ch, string[6];
L:
	show_all();
	sh(p);
	fflush(stdout);
	while (1)
	{
		c = getch();
		if (c == 3 || c == 4 || c == 27 || c < 0)
			break;
		if (c == 517)
		{
			page--;
			if (page < 0)
				page = totalpage - 1;
			init_data(page);
			p = 0;
			show_all();
			fflush(stdout);
		}
		if (c == 518)
		{
			page++;
			if (page >= totalpage)
				page = 0;
			init_data(page);
			p = 0;
			show_all();
			fflush(stdout);
		}
		if (c == 261)
		{
			n = 0;
			printf
			("\033[8;20H\033[33;41m©³©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©·");
			printf
			("\033[9;20H\033[33;41m©§  PageUp   Ctrl+B           Ç°·­Ò»Ò³  ©§");
			printf
			("\033[10;20H\033[33;41m©§  PageDown Ctrl+F SPACE     ºó·­Ò»Ò³  ©§");
			printf
			("\033[11;20H\033[33;41m©§  Ctrl+G                Ìø×ªµ½Ö¸¶¨Ò³  ©§");
			printf
			("\033[12;20H\033[33;41m©§©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©§");
			printf
			("\033[13;20H\033[33;41m©§                      °´ÈÎÒâ¼ü·µ»Ø ...©§");
			printf
			("\033[14;20H\033[33;41m©»©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¿");
			n = getch();
			show_all();
			fflush(stdout);
		}
		if (c == 262)
		{
			for (n = 0; n < 6; n++)
				string[n] = 0;
			n = 0;
			printf
			("\033[10;25H\033[33;41m©³©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©·");
			printf
			("\033[11;25H\033[33;41m©§                          ©§");
			printf
			("\033[12;25H\033[33;41m©»©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¿");
			printf("\033[11;29H\033[33;41mÌø×ªµ½(1-%d):",
			       totalpage);
			do
			{
				ch = getch();
				if (ch >= '0' && ch <= '9')
				{
					printf("%c", ch);
					string[n] = ch;
					n++;
				}
			}
			while (ch != '\n' && n <= 5);
			string[n] = '\0';
			n = atoi(string);
			if (n > 0 && n <= totalpage)
			{
				page = n - 1;
				init_data(page);
				p = 0;
			}
			show_all();
			fflush(stdout);
		}
		if (c == 257 && p > 0)
			p--;
		if (c == 258 && p < counts - 1)
			p++;
		if (c == 259 && p < counts - 19)
			p += 19;
		if (c == 260 && p >= 19)
			p -= 19;
		if (c == 13 || c == 10)
		{
			bbsnet(p);
			goto L;
		}
		for (n = 0; n < counts; n++)
			if (str[n] == c)
				p = n;
		sh(p);
		fflush(stdout);
	}
}

int isLocal (char * theIP)
{
	char fname [256];
	char buf [512];
	char *p, *p2;
	FILE *fp;
	pid_t pid;
	pid = getpid ();
	sprintf (fname, "/tmp/%d%d.bbsnet", pid, rand()%10000);
	sprintf (buf, "ping -c 1 %s > %s", theIP, fname);
	system (buf);
	fp = fopen (fname, "r");
	if (fp == NULL)
		return 0;
	fread (buf, 100, 1, fp);
	buf [100] = 0;
	if (strncmp (buf, "PING", 4)) {
		fclose (fp);
		unlink (fname);
		return 0;
	}
	p = strstr (buf, " (");
	if (p == NULL) {
		fclose (fp);
		unlink (fname);
		return 0;
	}
	p2 = strstr (p, ") ");
	if (p2 == NULL) {
		fclose (fp);
		unlink (fname);
		return 0;
	}
	* p2 = 0;
	if (!strncmp (p + 2, "127.", 4) || strstr (p, "210.51.191.217")) {
		fclose (fp);
		unlink (fname);
		return 1;
	}
	fclose (fp);
	unlink (fname);
	return 0;
}

bbsnet(int n)
{
	char buf1[40], buf2[39], c, buf3[2];
	int i;
	int l;
	int j, m;
	if (n >= counts)
		return;
	if (strcmp(host2[n], SD_SITENAME) == 0)
	{
		for (i = 0; i < 25; i++)
		{
			buf1[i] = '\0';
			buf2[i] = '\0';
		}
		prints("[1;32m[23;3HÁ¬Íù: ");
		refresh();
		j = 0;
		l = 0;
		for (i = 0; i < 15; i++)
		{
			c = getch();
			if (c == ' ' || c == '\015' || c == '\0' || c == '\n')
				break;
			if (c == ':')
			{
				l = 1;
				sprintf(buf3, "%c", c);
				prints("[0;1m");
				prints(buf3);
				prints("[1;33m");
				refresh();
				strcpy(ip_zdh, buf1);
				strcpy(ip[n], ip_zdh);
				for (m = 0; m < 20; m++)
				{
					buf1[m] = '\0';
					buf2[m] = '\0';
				}
				j = 0;
			}
			if ((c >= '0' && c <= '9') || (c == '.')
			        || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
			{
				sprintf(buf3, "%c", c);
				sprintf(buf2, "%s%c", buf1, c);
				sprintf(buf1, "%s", buf2);
				prints(buf3);
				refresh();
				j = j + 1;
			}
			if ((c == 5) && (j >= 1))
			{
				for (m = j - 1; m < 20; m++)
				{
					buf1[m] = '\0';
					buf2[m] = '\0';
				}
				sprintf(buf3, "%c", 0x08);
				prints(buf3);
				prints("\033[33m_\033[32m");
				prints(buf3);
				refresh();
				j--;
			}
			refresh();
			i--;
		}
		if (l == 0)
		{
			strcpy(ip_zdh, buf1);
			strcpy(ip[n], ip_zdh);
		}
		else
		{
			port[n] = atoi(buf1);
		}
	}
	printf("[H[2J[1;32mo Á¬Íù: %s (%s)\r\n", host2[n], ip[n]);
	printf("%s\r\n\r\n\033[m", "o Á¬²»ÉÏÊ±Çë°´ \'Ctrl+C\' ÍË³ö");
	fflush(stdout);
	/*thunder */
	lcase(ip[n]);
/*	if (isLocal (ip [n]))
		return 0;
		*/
	if (port[n]>1023)
		proc(host2[n], ip[n], port[n]);
	if((ip[n][0]!='0')&&strcmp(ip[n],"...")&&strcmp(ip[n],"localhost")&&strncmp(ip[n],BBSIP,14)&&strncmp(ip[n],"127", 3)&&strcmp(ip[n],"localhost.localdomain")&&strcmp(ip[n],"inankai.com")&&strcmp(ip[n],"inankai.net"))
		/**/
		proc(host2[n], ip[n], port[n]);
}

int main(int n, char *cmd[])
{
	srand (time (NULL));
	SetQuitTime();
	get_tty();
	init_tty();
	if (n >= 2)
		strcpy(datafile, cmd[1]);
	if (n >= 3)
		strcpy(userid, cmd[2]);
	count_sites();
	init_data(0);
	main_loop();
	printf("[m");
	reset_tty();
	return 0;//add by tdhlshx
}

syslog(char *s)
{
	char buf[512], timestr[16], *thetime;
	time_t dtime;
	FILE *fp;
	fp = fopen("reclog/bbsnet.log", "a");
	if (!fp)
		return;
	time(&dtime);
	thetime = (char *) ctime(&dtime);
	strncpy(timestr, &(thetime[4]), 15);
	timestr[15] = '\0';
	sprintf(buf, "%s %s %s\n", userid, timestr, s);
	fprintf(fp, buf);
	fclose(fp);
}

#define stty(fd, data) tcsetattr( fd, TCSANOW, data )
#define gtty(fd, data) tcgetattr( fd, data )
struct termios tty_state, tty_new;

get_tty()
{
	if (gtty(1, &tty_state) < 0)
		return 0;
	return 1;
}

init_tty()
{
	long vdisable;
	memcpy(&tty_new, &tty_state, sizeof (tty_new));
	tty_new.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ISIG);
	tty_new.c_cflag &= ~CSIZE;
	tty_new.c_cflag |= CS8;
	tty_new.c_cc[VMIN] = 1;
	tty_new.c_cc[VTIME] = 0;
	if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) >= 0)
	{
		tty_new.c_cc[VSTART] = vdisable;
		tty_new.c_cc[VSTOP] = vdisable;
		tty_new.c_cc[VLNEXT] = vdisable;
	}
	tcsetattr(1, TCSANOW, &tty_new);
}

reset_tty()
{
	stty(1, &tty_state);
}

proc(char *hostname, char *server, int port)
{
	int fd;
	struct sockaddr_in blah;
	struct hostent *he;
	int result;
	unsigned char buf[2048];
	fd_set readfds;
	struct timeval tv;
	signal(SIGALRM, break_check);
	alarm(1);
	bzero((char *) &blah, sizeof (blah));
	blah.sin_family = AF_INET;
	blah.sin_addr.s_addr = inet_addr(server);
	blah.sin_port = htons(port);
	fflush(stdout);
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ((he = gethostbyname(server)) != NULL)
		bcopy(he->h_addr, (char *) &blah.sin_addr, he->h_length);
	else if ((blah.sin_addr.s_addr = inet_addr(server)) < 0)
		return;
	/* efan: ÅÐ¶ÏÊÇ·ñ127.xxx.xxx.xxx	*/	
	if ((blah.sin_addr.s_addr & 0x000000ff) == 127 && port < 1024)
		return;
	if (blah.sin_addr.s_addr == inet_addr(BBSIP))
		return;
	if (connect(fd, (struct sockaddr *) &blah, 16) < 0)
		return;
	signal(SIGALRM, SIG_IGN);
	printf("ÒÑ¾­Á¬½ÓÉÏÖ÷»ú£¬°´'ctrl+]'¿ìËÙÍË³ö¡£\n");
	sprintf(buf, "Login %s (%s)", hostname, server);
	syslog(buf);
	while (1)
	{
		tv.tv_sec = 2400;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		FD_SET(0, &readfds);
		result = select(fd + 1, &readfds, NULL, NULL, &tv);
		if (result <= 0)
			break;
		if (FD_ISSET(0, &readfds))
		{
			result = read(0, buf, 2048);
			if (result <= 0)
				break;
			if (result == 1 && (buf[0] == 10 || buf[0] == 13))
			{
				buf[0] = 13;
				buf[1] = 10;
				result = 2;
			}
			if (buf[0] == 29)
			{
				close(fd);
				break;
			}
			write(fd, buf, result);
		}
		else
		{
			result = read(fd, buf, 2048);
			if (result <= 0)
				break;
			if (strchr(buf, 255))
				telnetopt(fd, buf, result);
			else
				write(0, buf, result);
		}
	}
	sprintf(buf, "Logout %s (%s)", hostname, server);
	syslog(buf);
        SetQuitTime();
}

int telnetopt(int fd, char *buf, int max)
{
	unsigned char c, d, e;
	int pp = 0;
	unsigned char tmp[30];
	while (pp < max)
	{
		c = buf[pp++];
		if (c == 255)
		{
			d = buf[pp++];
			e = buf[pp++];
			fflush(stdout);
			if ((d == 253) && (e == 3 || e == 24))
			{
				tmp[0] = 255;
				tmp[1] = 251;
				tmp[2] = e;
				write(fd, tmp, 3);
				continue;
			}
			if ((d == 251 || d == 252)
			        && (e == 1 || e == 3 || e == 24))
			{
				tmp[0] = 255;
				tmp[1] = 253;
				tmp[2] = e;
				write(fd, tmp, 3);
				continue;
			}
			if (d == 251 || d == 252)
			{
				tmp[0] = 255;
				tmp[1] = 254;
				tmp[2] = e;
				write(fd, tmp, 3);
				continue;
			}
			if (d == 253 || d == 254)
			{
				tmp[0] = 255;
				tmp[1] = 252;
				tmp[2] = e;
				write(fd, tmp, 3);
				continue;
			}
			if (d == 250)
			{
				while (e != 240 && pp < max)
					e = buf[pp++];
				tmp[0] = 255;
				tmp[1] = 250;
				tmp[2] = 24;
				tmp[3] = 0;
				tmp[4] = 65;
				tmp[5] = 78;
				tmp[6] = 83;
				tmp[7] = 73;
				tmp[8] = 255;
				tmp[9] = 240;
				write(fd, tmp, 10);
			}
		}
		else
			write(0, &c, 1);
	}
}

int refresh()
{
	write(0, buf, strlen(buf));
	buf[0] = 0;
}

int prints(char *b)
{
	strcat(buf, b);
}
