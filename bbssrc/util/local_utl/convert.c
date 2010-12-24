#include "bbs.h"

report()
{
}
void conv(unsigned int * new,unsigned int * old,int len)
{
        char *old_p,*new_p;
        int i=0;
        old_p=(char *)old;
        new_p=(char *)new;
        len--;
        for(i=0;i<=len;i++)
        {
                (*(new_p+i))=(*(old_p+(len-i)));
        }
};

main()
{
    struct userec fh,nfh;        
    char fname[80];
    char dname[80];
    char genbuf[120];
    FILE *fp, *fp2;

    memset(&fh,0,sizeof(struct userec));
    memset(&nfh,0,sizeof(struct userec));

    sprintf( fname, "PASSWDS");
    sprintf( dname, "PASSWDS.new");

    if ((fp = fopen(fname, "rb")) == NULL) {
        printf("Error: Cannot open PASSWDS.\n");
        return 0;
    }
    if ((fp2 = fopen(dname, "wb")) == NULL) {
        printf("Error: Cannot write to PASSWDS.new.\n");
        return 0;
    }
    while(1)
    {
      if(fread(&fh,sizeof(fh),1,fp)<=0) break;      
      strcpy(nfh.userid,fh.userid);
      conv((int *)&nfh.firstlogin,(int *)&fh.firstlogin,sizeof(time_t));
      strcpy(nfh.lasthost,fh.lasthost);
      conv(&nfh.numlogins,&fh.numlogins,sizeof(int));
      conv(&nfh.numposts,&fh.numposts,sizeof(int));
      strcpy(nfh.flags,fh.flags);
      strcpy(nfh.passwd,fh.passwd);
      strcpy(nfh.username,fh.username);
      strcpy(nfh.ident,fh.ident);
      strcpy(nfh.termtype,fh.termtype);
      strcpy(nfh.reginfo,fh.reginfo);
      conv(&nfh.userlevel,&fh.userlevel,sizeof(int));
      conv((int *)&nfh.lastlogin,(int *)&fh.lastlogin,sizeof(time_t));
      conv((int *)&nfh.stay,(int *)&fh.stay,sizeof(time_t));
      strcpy(nfh.realname,fh.realname);
      strcpy(nfh.address,fh.address);
      strcpy(nfh.email,fh.email);
      conv(&nfh.signature,&fh.signature,sizeof(int));
      conv(&nfh.userdefine,&fh.userdefine,sizeof(int));
      conv((int *)&nfh.notedate,(int *)&fh.notedate,sizeof(time_t));
      conv(&nfh.noteline,&fh.noteline,sizeof(int));
      /* ---下面转换用户的出生日期和性别------------------------------------ */
      memcpy(&nfh.gender,&fh.gender, sizeof(fh.gender));
      memcpy(&nfh.birthday,&fh.birthday,sizeof(char));
      memcpy(&nfh.birthmonth,&fh.birthmonth,sizeof(char));
      memcpy(&nfh.birthyear,&fh.birthyear,sizeof(char));
      conv((int *)&nfh.lastjustify,(int *)&fh.lastjustify,sizeof(time_t));
      /* ------------------------------------------------------------------ */
      append_record(dname,&nfh,sizeof(nfh));
    }
    fclose(fp);
    fclose(fp2);
    return 1;
}                  

