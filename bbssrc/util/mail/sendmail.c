#include "../../include/bbs.h"
#include "netinet/in.h"
#include "netdb.h"
#include "sys/types.h"
#define MAILSERVER "210.51.191.217"

int main(int n, char *cmd[]) {
        if(n<4) {
                printf("usage: sendmail -f mail-from mail-reciever.\r\n");
                exit(0);
        }
        sendmail(cmd[2], cmd[3]);
}

int sendmail(char *from, char *to) {
        struct sockaddr_in blah;
        int fd;
        char buf[1024];
        blah.sin_family=AF_INET;
        blah.sin_addr.s_addr=inet_addr(MAILSERVER);
        blah.sin_port=htons(25);
        fd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(connect(fd, (struct sockaddr *)&blah, 16)<0) {    
                printf("Can't connect to the mail server.\r\n");
                return;
        }
        readserver(fd);
        sprintf(buf, "helo %s\n",BBSHOST);
        write(fd, buf, strlen(buf));
        readserver(fd);
        sprintf(buf, "mail from: %s\n", from);
        write(fd, buf, strlen(buf));
        readserver(fd);
        sprintf(buf, "rcpt to: %s\n", to);
        write(fd, buf, strlen(buf));
        readserver(fd);
        write(fd, "data\n", 5);
        readserver(fd);
        while(1) {
                if(fgets(buf, 256, stdin)<=0) break;
                buf[255]=0;
                if(buf[0]=='.' && (buf[1]==10 || buf[1]==13)) continue;
                write(fd, buf, strlen(buf));
        }
        write(fd, "\n.\n", 3);         
        readserver(fd);
        write(fd, "quit\n", 5);
        readserver(fd);
}

int readserver(int fd) {
        char buf[256];
        if(read(fd, buf, 256)<=0) {
                printf("Mail server error 1001.\r\n");
                exit(0);
        }
        buf[255]=0;
        if(atoi(buf)>=500) {
                printf("Mail server error: %s.\r\n", buf);
                exit(0);
        }
}                   

