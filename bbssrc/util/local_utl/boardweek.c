#include "bbs.h" 
#include <string.h> 
#include <stdio.h> 
#include <time.h> 
  
main() 
{ 
        FILE    *fd1, *fd2, *fd3, *fd4; 
        char    board[STRLEN], passwd[STRLEN], BM[STRLEN]; 
        char    board1[STRLEN], board2[STRLEN]; 
        char    *id; 
        struct boardheader bh; 
        struct userec user; 
  
        sprintf(board, "%s/%s", BBSHOME, BOARDS); 
        sprintf(passwd, "%s/%s", BBSHOME, PASSFILE); 
        sprintf(board1, "%s/0Announce/board1", BBSHOME); 
        sprintf(board2, "%s/0Announce/board2", BBSHOME); 
        fd1 = fopen(board, "rb"); 
        fd2 = fopen(passwd, "rb"); 
        fd3 = fopen(board1, "w"); 
        fd4 = fopen(board2, "w"); 
        for ( ; ; ) { 
                if (fread(&user, sizeof(user), 1, fd2) <= 0) 
                        break; 
                if (!strcmp(user.userid,"SYSOP")) continue; 
                if (!(user.userlevel & PERM_BOARDS)) continue; 
                fseek(fd1, 0, SEEK_SET); 
                for ( ; ; ) { 
                        if (fread(&bh, sizeof(bh), 1, fd1) <= 0) 
                                break; 
                        if (bh.BM[0] == 0) 
                                continue; 
                        strcpy(BM, bh.BM); 
                        id = strtok(BM, " "); 
                        for ( ; id != NULL; id = strtok(NULL, " ")) { 
                                if (strcmp(user.userid, id) != 0) 
                                        continue; 
                                else { 
                                        if ( time(NULL)-user.lastlogin > 7*86400 && time(NULL)-user.lastlogout > 7*86400) 
                                                fprintf(fd3, "%s(%s)上次上站时间: %s\n", id, bh.filename, ctime(&user.lastlogin)); 
                                        if ( time(NULL)-user.lastlogin >  14*86400 && time(NULL)-user.lastlogout > 14*86400) 
                                                fprintf(fd4, "%s(%s)上次上站时间: %s\n", id, bh.filename, ctime(&user.lastlogin)); 
                                        break; 
                                } 
                        } 
                } 
        } 
        fclose(fd1); 
        fclose(fd2); 
        fclose(fd3); 
        fclose(fd4); 
} 
