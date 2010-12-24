#include "BBSLIB.inc"
int main()
{
  char buf[256];                                                                    
  init_all();
  refreshto("checkmsg",10);
  if(!loginok)                                                                                
          return 0;                                                                           
  sprintf(buf, "home/%c/%s/wwwmsg", toupper(currentuser.userid[0]), currentuser.userid);      
  int size=file_size(buf);                                                                    
  if(size<=0)                                                                                 
     return 0;    
  if(u_info->mode!=MSG+20000)
     printf("<script>top.f4.location=\"bbsfoot\";</script>");                                                                  

}
