/*$Id: bbsjs.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include <BBSLIB.inc>
#include <stdio.h>

int countwww;
struct max_log_record
{
        int     year;
        int     month;
        int     day;
        int     logins;
        unsigned long   visit;
}
max_log,wwwmax_log;

char * visit_log(void);

int main()
{
        FILE *fp;
        static char s[300];
        NOTADDCSSFILE=1;
        init_all();
        printf("<html><body bgcolor=#99DDBB leftmargin=0 topmargin=0><font style='font-size:12px'>\n");
        printf( "<center>目前在线人数: <font color=green>%d</font> 本站累计访问人次: <font color=green>%s</font>", count_online(), visit_log());
        printf("</font></center></body></html>");
}

int count_www()
{
        int i, total=0;
        for(i=0; i<MAXACTIVE; i++)
                if(shm_utmp->uinfo[i].active)
                        if(shm_utmp->uinfo[i].utmpkey!=-1)
                                total++;
        countwww=total;
        return total;
}

//add by wzk for log www visiters
char * visit_log(void)
{
        int vfp;
        static char genbuf[20];
        time_t now;
        struct tm *tm;
        extern struct UTMPFILE *shm_utmp;

        vfp = open(".visitlog",O_RDWR|O_CREAT,0644);
        if(vfp == -1) {
                http_fatal("Can NOT write visit Log to .visitlog");
                return ;
        }
        flock(vfp, LOCK_EX);
        lseek(vfp,(off_t)0,SEEK_SET);
        read(vfp, &max_log,(size_t)sizeof(max_log));
        read(vfp, &wwwmax_log,(size_t)sizeof(max_log));
        if(max_log.year < 1990 || max_log.year > 2020) {
                now = time(0);
                tm = localtime(&now);
                max_log.year = tm->tm_year+1900;
                max_log.month = tm->tm_mon+1;
                max_log.day = tm->tm_mday;
                max_log.visit = 0;
                max_log.logins = 0;
        }
        wwwmax_log.visit ++;
        //	max_log.visit ++;
        if( max_log.logins > shm_utmp->max_login_num )
                shm_utmp->max_login_num = max_log.logins;
        else
                max_log.logins =  shm_utmp->max_login_num;
        lseek(vfp,(off_t)sizeof(max_log),SEEK_SET);
        write(vfp,&wwwmax_log,(size_t)sizeof(max_log));
        flock(vfp, LOCK_UN);
        close(vfp);
        //   sprintf(genbuf,"%u/%u",
        //      wwwmax_log.visit,max_log.visit);
        sprintf(genbuf,"%u</font>, 其中Web方式访问人次: <font color=green>%u",max_log.visit+wwwmax_log.visit, wwwmax_log.visit);
        return (genbuf);
}

