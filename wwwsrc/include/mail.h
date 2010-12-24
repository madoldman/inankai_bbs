int  isvalidEmail(char *src)
{
  int i;
  int hasat_ch=0;
  int just_dot=0;
  for(i=0;i<strlen(src);i++)
  {
                if(!(toupper(src[i])>='A'&&toupper(src[i])<='Z'||
                        src[i]>='0'&&src[i]<='9'||
                        src[i]=='_'||src[i]=='@'||src[i]=='.'
			||src[i]=='-'))
                {
                       return 0;
                }
                if(src[i]=='@')
                {
                        if(hasat_ch>0)
                           return 0;
                        hasat_ch=1;
		}
		else
		if(src[i]=='.')
		{
		    if(just_dot>0)
			return 0;
		    just_dot=1;
		}
		else
		{
		    just_dot=0;
		}
   }
   return 1; 
}
void getsenderex(char *src,char *outstr)
{
  char *p;
	char *p2;
   char *strtmp;
  int i;
  strcpy(outstr,src);
  while(!isvalidEmail(outstr))
  {//check whether is the bbs_userid...
	p=strstr(outstr, " (");		
	  if(p!=NULL)
	  {
		*p=0;
		return;
	  }

	//chech whether has the addr like: "brew" <brew@kkk.com> 
	p2=strstr(outstr," <");
	if(p2!=NULL)
	{
	     i=strlen(p2);
	     if(p2[i-1]=='>')
         	{
		  p2[i-1]=0;
		  strcpy(outstr,&p2[2]);
		  if(isvalidEmail(outstr))
		  {
			break;				
		  } 
		}			
	}
	//out put error string
	strcpy(outstr,"");
	strcat(outstr,src);
	return;
	  
  }
  if(strstr(outstr,"@nkbbs.org") || strstr(outstr,"@localhost")
	  || strstr(outstr,"@bbs.nankai.edu.cn")) {
      p=strstr(outstr,"@");
      *p=0;
      return;
  }
  return;
}

int getsender(char *s)
{
 char *p=strstr(s,"@");
  if(!p){
       p=strtok(s, " (");
       if(p==0)
       p="";
       strcpy(s,p);
       return 0;
  }
  p=strstr(s, "<");
  if(p==0)
     p=s;
  else{
     p++;
     char *p2=strstr(p,">");
     if(p2!=0) *p2=0;
  }
  if(strstr(p,"@nkbbs.org") || strstr(p,"@localhost")) {
      char *p3=strstr(p,"@");
      *p3=0;
      strcpy(s,p);
      return 0;
  }
strcpy(s,p);
return 1;

}

