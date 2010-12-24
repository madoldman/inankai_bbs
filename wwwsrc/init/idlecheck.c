/*$Id
*/

#include "BBSLIB.inc"
int main(int argc,char *argv[])
{

  shm_init();
  struct user_info *u; 
  int n,m,i=0; 
  

  FILE *fp;
  fp=fopen("/home/bbs/.UTMP.lock", "a");
  flock(fileno(fp), LOCK_EX);
/* 
   int ii=atoi(argv[1]);
   u=&(shm_utmp->uinfo[ii]);
   printf("%d:%s %d  %d",ii,u->userid,u->active,u->pid);
   return;
*/
  for(n=0; n<MAXACTIVE; n++){
     u=&(shm_utmp->uinfo[n]);
     if (u->active==0)
         continue;
     if(u->mode<20000) continue;
     m=abs(time(0) - u->idle_time);
     if(m<IDLE_TIMEOUT) continue;

     i++;
     printf("%d.%s   %d \n",n,u->userid,m/60);
     u->active=0;
  }
       
  printf("Total %d\n" ,i); 


  
  flock(fileno(fp), LOCK_UN);
  fclose(fp);


}
