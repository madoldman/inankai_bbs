#include "bbs.h"
typedef struct tag_lunar
{
	int leap;
	int sy,ly;
	int sm,lm;
	int sd,ld;
	int jq;
	int w;
}
lunar_t;
int smonth_max(int y,int m);
int countday(int sy,int sm,int sd);
int get24jq(int sy,int i);
int isjq(lunar_t date);
void s_to_l(int y,int m,int d,lunar_t *date);
void cday(char *s,int d);
void cmonth(char *s,int lm);
void cyear(char *s,int ly);
void cjq(char *s,int jq);
void printmonth(int y,int m);
void deallunarkey(int sy,int sm);
//µç×ÓÍòÄêÀú
//LUNAR
int printtoday()
{
	char stime[STRLEN],cy[8],cd[8],cm[8];
	char strw[][3]={"ÈÕ","Ò»","¶ş","Èı","ËÄ","Îå","Áù"};
	int y,m,d,t;
	lunar_t today;
	modify_user_mode( LUNAR );
	clear();
	t=time(NULL);
	getdatestring(t,NA);
	strcpy(stime,datestring);
	//  ptime(stime,&t,1);
	sscanf(stime,"%dÄê%dÔÂ%dÈÕ",&y,&m,&d);
	s_to_l(y,m,d,&today);
	today.jq=isjq(today);
	cyear(cy,today.ly);
	cmonth(cm,today.lm);
	cday(cd,today.ld);
	prints("[1;32;44m            ¡¾%s¡¿    [33mµç×ÓÍòÄêÀú    [36m1900¡ª2050              \n",BBSNAME);
	prints("[1;31;44m©°¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©´\n");
	prints("[1;31;44m©¦     [33m¹«Ôª [31m%4dÄê %2dÔÂ %2dÈÕ  [32mĞÇÆÚ%s [33mÅ©Àú [31m%s%s%s      ©¦\n",today.sy,today.sm,today.sd,strw[today.w],cy,cm,cd);
	prints("[1;31;44m©¸¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª©¼");
	printmonth(y,m);
	return 0;
}

void printmonth(int y,int m)
{
	//char strw[][3]={"ÈÕ","Ò»","¶ş","Èı","ËÄ","Îå","Áù"};
	int d=1,t,smax;
	lunar_t day;
	move(4,0);
	clrtobot();
	prints("\033[1;34;44m©³©¥©¥©¥©¥©×©¥©¥©¥©×©¥©¥©¥©×©¥©¥©¥©×©¥©¥©¥©×©¥©¥©¥©×©¥©¥©¥©×©¥©¥©¥©·\033[m\n");
	prints("\033[1;34;44m©§\033[33;45m %-4dÄê \033[34;44m©§\033[31m  ÈÕ  \033[34m©§  \033[37mÒ»  \033[34m©§ ", y);
	prints("\033[37m¶ş   \033[34m©§  \033[37mÈı  \033[34m©§  \033[37mËÄ  \033[34m©§  \033[37mÎå  ");
	prints("\033[34m©§\033[31m  Áù  \033[34m©§\033[m\n");
	prints("\033[1;34;44m©Ç©¥©¥©¥©¥©ï©¥©¥©¥©ë©¥©¥©¥©ë©¥©¥©¥©ë©¥©¥©¥©ë©¥©¥©¥©ë©¥©¥©¥©ë©¥©¥©¥©Í\033[m");
	s_to_l(y,m,d,&day);
	day.jq=isjq(day);
	smax=smonth_max(y,m);
	for(t=1;t<13;t++)
	{
		move(t+6,0);
		prints("[1;34;44m©§%s  %02dÔÂ  [34;44m©§",(t==m)?"[33;45m":"[37;44m",t);
	}
	t=1;
	for(d=1;d<smax+1;d++)
	{
		char cd[STRLEN];
		s_to_l(y,m,d,&day);
		day.jq=isjq(day);
		if(d==1)
		{
			int i;
			move(8,38);
			for(i=0;i<day.w;i++)
				prints("[34m      [34m©¦");
			move(7,38);
			for(i=0;i<day.w;i++)
				prints("[34m     [34m ©¦");
		}
		move(6+t*2-1,38+18*day.w);
		/* change by wzk for print 32 */
		if (d<32)
prints("  %s%2d[34m  ©¦%s",(day.ld==1)?day.leap?"[36m":"[32m":day.jq?"[33m":((day.w==0)||(day.w==6))?"[31m":"[37m",d,(day.w==6)?"[m":"");
		else
			prints("[34m     [34m ©¦%s",(day.w==6)?"[m":"");
		/* end of change */
		move(6+t*2,38+18*day.w);
		if(day.ld==1)
		{
			if(day.lm==1)
				cyear(cd,day.ly);
			else
				cmonth(cd,day.lm);
		}
		else if(day.jq)
			cjq(cd,day.jq);
		else
			cday(cd,day.ld);
		/* change by wzk for print 32 */
		if (d<32)
prints("%s%s[34m©¦%s",(day.ld==1)?day.leap?"[36m":"[32m":day.jq?"[33m":((day.w==0)||(day.w==6))?"[31m":"[37m",cd,(day.w==6)?"[m":"");
		else
			prints("[34m     [34m ©¦%s",(day.w==6)?"[m":"");
		/* end of change*/
		if(day.w==6)
			t++;
	}
	if(day.w!=6)
	{
		int i;
		move(6+t*2-1,38+(day.w+1)*18);
		for(i=day.w+1;i<7;i++)
			prints("[34m      [34m©¦%s",(i==6)?"[m":"");
		move(6+t*2,38+(day.w+1)*18);
		for(i=day.w+1;i<7;i++)
			prints("[34m      [34m©¦%s",(i==6)?"[m":"");
		t++;
	}
	for(;t<7;t++)
	{
		move(6+t*2-1,38);
		prints("      ©¦      ©¦      ©¦      ©¦      ©¦      ©¦      ©¦[m");
		move(6+t*2,38);
		prints("      ©¦      ©¦      ©¦      ©¦      ©¦      ©¦      ©¦[m");
	}
	prints("\n[1;34;44m©»©¥©¥©¥©¥©İ¡ª¡ª¡ª©Ø¡ª¡ª¡ª©Ø¡ª¡ª¡ª©Ø¡ª¡ª¡ª©Ø¡ª¡ª¡ª©Ø¡ª¡ª¡ª©Ø¡ª¡ª¡ª©¼[m");
	move(t_lines-4,0);
	prints("[1;32;44m Ctrl-C  [31mÀë¿ª[32m  ¡ü  [31mÉÏÔÂ  [32m¡ı  [31mÏÂÔÂ [32m¡û  [31mÉÏÄê[32m  ¡ú[31m  ÏÂÄê[32m  Y  [33m¸ü¸ÄÄê·İ   [0;1;37m");
	deallunarkey(y,m);
}
int isjq(lunar_t date)
{
	int i;
	int smd[][2]={{1,1},{2,14},{3,8},{5,1},{5,4},{6,1},
	              {7,1},{8,1},{9,10},{10,1},{12,25}};
	int lmd[][2]={{1,15},{5,5},{8,15},{9,9}};
	int sow[][3]={{5,2,0},{6,3,0},{11,4,4}};
	for(i=0;i<11;i++)
		if((date.sm==smd[i][0])&&(date.sd==smd[i][1]))
			return (i+25);
	for(i=0;i<4;i++)
		if((date.lm==lmd[i][0])&&(date.ld==lmd[i][1]))
			return (i+36);
	for(i=0;i<3;i++)
		if((date.w==sow[i][2])&&(date.sm==sow[i][0]))
		{
			int d,order;
			for(d=1,order=0;d<=date.sd;d++)
			{
				if((countday(date.sy,date.sm,d)%7)==sow[i][2])
					order++;
			}
			if(order==sow[i][1])
				return (i+40);
		}
	if((date.sm==3)||(date.sm==4))
	{
		lunar_t tmpd;
		int tm,td,tw;
		s_to_l(date.sy,3,get24jq(date.sy,5),&tmpd);
		while(1)
		{
			if(tmpd.sd==smonth_max(tmpd.sy,tmpd.sm))
			{
				tm=tmpd.sm+1;
				td=1;
			}
			else
			{
				tm=tmpd.sm;
				td=tmpd.sd+1;
			}
			s_to_l(date.sy,tm,td,&tmpd);
			if(tmpd.ld==15)
				break;
		}
		for(tw=tmpd.w;tw<7;tw++)
		{
			if(td==smonth_max(tmpd.sy,tmpd.sm))
			{
				tm++;
				td=1;
			}
			else
				td++;
		}
		if((tm==date.sm)&&(td==date.sd))
			return 43;
	}
	for(i=(2*date.sm-2);i<(2*date.sm);i++)
		if(date.sd==get24jq(date.sy,i))
			return (i+1);
	return 0;
}
int get24jq(int sy,int i)
{
	int jq_info[]={0,21208,42467,63836,85337,107014,128867,150921,173149,
	               195551,218072,240693,263343,285989,308563,331033,353350,
	               375494,397447,419210,440795,462224,483532,504758};
	double offd=(525948.766245*(sy-1900)+(jq_info[i]+2*60+5))/1440+7;
	return (int)offd-countday(sy,i/2+1,1);
}
void deallunarkey(int sy,int sm)
{
	int cmd,y,m;
	char buf[STRLEN],input[8];
	y=sy,m=sm;
	cmd=egetch();
	switch(cmd)
	{
	case Ctrl('C'): return;
	case KEY_UP:
		if(m==1)
		{
			if(y>1900)
				y--;
			else
				y=2050;
			m=12;
		}
		else
			m--;
		break;
	case KEY_DOWN:
		if(m==12)
		{
			if(y<2050)
				y++;
			else
				y=1900;
			m=1;
		}
		else
			m++;
		break;
	case KEY_LEFT:
		if(y>1900)
			y--;
		else
			y=2050;
		break;
	case KEY_RIGHT:
	case ' ':
		if(y<2050)
			y++;
		else
			y=1900;
		break;
	case 'y':
	case 'Y':
		sprintf(buf,"ÊäÈëÒª²éÑ¯µÄÄê·İ[%d]:",y);
		getdata(t_lines-3,0,buf,input,5,DOECHO,YEA);
		if(input[0]=='\0')
			break;
		else if((atoi(input)>=1900)&&(atoi(input)<2051))
		{
			y=atoi(input);
			break;
		}
		else
		{
			move(t_lines-2,0);
			prints("Äê·İ±ØĞëÔÚ1900¡«2050Ö®¼ä.");
			pressanykey();
			break;
		}
	default:
		break;
	}
	printmonth(y,m);
}
int smonth_max(int y,int m)
{
	int  smonths[]={31,28,31,30,31,30,31,31,30,31,30,31};
	if(m==2)
		return ((y%4 == 0) && (y%100 != 0) || (y%400 == 0))? 29: 28;
	else
		return smonths[m-1];
}
int countday(int sy,int sm,int sd)
{
	int y,m,i=0;
	for(y=1900;y<sy;y++)
	{
		if((y%4 == 0) && (y%100 != 0) || (y%400 == 0))
			i+=366;
		else
			i+=365;
	}
	for(m=1;m<sm;m++)
		i+=smonth_max(sy,m);
	return i+sd;
}
void cjq(char *s,int jq)
{
	char str[][7] = {" Ğ¡º® "," ´óº® "," Á¢´º "," ÓêË® "," ¾ªÕİ "," ´º·Ö ",
	                 " ÇåÃ÷ "," ¹ÈÓê "," Á¢ÏÄ "," Ğ¡Âú "," Ã¢ÖÖ "," ÏÄÖÁ "," Ğ¡Êî "," ´óÊî ",
	                 " Á¢Çï "," ´¦Êî "," °×Â¶ "," Çï·Ö "," º®Â¶ "," Ëª½µ "," Á¢¶¬ "," Ğ¡Ñ© ",
	                 " ´óÑ© "," ¶¬ÖÁ ",
	                 " Ôªµ© ","ÇéÈË½Ú","¸¾Å®½Ú","ÀÍ¶¯½Ú","ÇàÄê½Ú","¶ùÍ¯½Ú","½¨µ³½Ú","½¨¾ü½Ú",
	                 "½ÌÊ¦½Ú","¹úÇì½Ú","Ê¥µ®½Ú",
	                 "ÔªÏü½Ú","¶ËÎç½Ú","ÖĞÇï½Ú","ÖØÑô½Ú",
	                 "Ä¸Ç×½Ú","¸¸Ç×½Ú","¸Ğ¶÷½Ú","¸´»î½Ú"};
	if(jq)
		strcpy(s,str[jq-1]);
}
void cyear(char *s,int ly)
{
	char str[][3]={"Êó","Å£","»¢","ÍÃ","Áú","Éß","Âí","Ñò","ºï","¼¦","¹·","Öí"
	              };
	sprintf(s," %sÄê ",str[(ly-4)%12]);
}
void cmonth(char *s,int lm)
{
	char str[][7]={" ÈòÔÂ "," Ò»ÔÂ "," ¶şÔÂ "," ÈıÔÂ "," ËÄÔÂ "," ÎåÔÂ "," ÁùÔÂ ",
	               " ÆßÔÂ "," °ËÔÂ "," ¾ÅÔÂ "," Ê®ÔÂ ","Ê®Ò»ÔÂ","Ê®¶şÔÂ"};
	strcpy(s,str[lm]);
}
void cday(char *s,int d)
{
	char str[][3]= {"Áã","Ò»","¶ş","Èı","ËÄ","Îå","Áù","Æß","°Ë","¾Å"};
	if(d==10)
		sprintf(s," ³õÊ® ");
	else if(d/10==0)
		sprintf(s," ³õ%s ",str[d%10]);
	else if(d%10)
		sprintf(s," %s%s ",(d/10==1)?"Ê®":"Ø¥",str[d%10]);
	else
		sprintf(s," %sÊ® ",str[d/10]);
}
void s_to_l(int y,int m,int d,lunar_t *date)
{
	int i, leapmonth=0, temp=0,offset;
	int l_info[]={
	                 0x04bd8,0x04ae0,0x0a570,0x054d5,0x0d260,0x0d950,0x16554,0x056a0,0x09ad0,0x055d2,
	                 0x04ae0,0x0a5b6,0x0a4d0,0x0d250,0x1d255,0x0b540,0x0d6a0,0x0ada2,0x095b0,0x14977,
	                 0x04970,0x0a4b0,0x0b4b5,0x06a50,0x06d40,0x1ab54,0x02b60,0x09570,0x052f2,0x04970,
	                 0x06566,0x0d4a0,0x0ea50,0x06e95,0x05ad0,0x02b60,0x186e3,0x092e0,0x1c8d7,0x0c950,
	                 0x0d4a0,0x1d8a6,0x0b550,0x056a0,0x1a5b4,0x025d0,0x092d0,0x0d2b2,0x0a950,0x0b557,
	                 0x06ca0,0x0b550,0x15355,0x04da0,0x0a5d0,0x14573,0x052b0,0x0a9a8,0x0e950,0x06aa0,
	                 0x0aea6,0x0ab50,0x04b60,0x0aae4,0x0a570,0x05260,0x0f263,0x0d950,0x05b57,0x056a0,
	                 0x096d0,0x04dd5,0x04ad0,0x0a4d0,0x0d4d4,0x0d250,0x0d558,0x0b540,0x0b5a0,0x195a6,
	                 0x095b0,0x049b0,0x0a974,0x0a4b0,0x0b27a,0x06a50,0x06d40,0x0af46,0x0ab60,0x09570,
	                 0x04af5,0x04970,0x064b0,0x074a3,0x0ea50,0x06b58,0x055c0,0x0ab60,0x096d5,0x092e0,
	                 0x0c960,0x0d954,0x0d4a0,0x0da50,0x07552,0x056a0,0x0abb7,0x025d0,0x092d0,0x0cab5,
	                 0x0a950,0x0b4a0,0x0baa4,0x0ad50,0x055d9,0x04ba0,0x0a5b0,0x15176,0x052b0,0x0a930,
	                 0x07954,0x06aa0,0x0ad50,0x05b52,0x04b60,0x0a6e6,0x0a4e0,0x0d260,0x0ea65,0x0d530,
	                 0x05aa0,0x076a3,0x096d0,0x04bd7,0x04ad0,0x0a4d0,0x1d0b6,0x0d250,0x0d520,0x0dd45,
	                 0x0b5a0,0x056d0,0x055b2,0x049b0,0x0a577,0x0a4b0,0x0aa50,0x1b255,0x06d20,0x0ada0,
	                 0x14b63};
	date->sy=y;
	date->sm=m;
	date->sd=d;
	offset = countday(y,m,d)-30;
	if(offset<1)
	{
		date->ly=1899;
		date->lm=12;
		date->ld=offset+30;
		date->w=(offset+30)%7;
		if(date->sd==6)
			date->jq=1;
		else if(date->sd==20)
			date->jq=2;
		else
			date->jq=0;
		return;
	}
	date->w=(offset+2)%7;
	for(i=1900; i<2050 && offset>0; i++)
	{
		int j, sum = 348;
		for(j=0x8000; j>0x8; j>>=1)
			sum += ((l_info[i-1900] & j)? 1: 0);
		if(l_info[i-1900] & 0xf)
			sum+=((l_info[i-1900] & 0x10000)? 30: 29);
		if(offset>sum)
			offset-=sum;
		else
			break;
	}
	date->ly = i;
	leapmonth = l_info[i-1900] & 0xf;
	date->leap = 0;
	for(i=1; i<13 && offset>0; i++)
	{
		if(leapmonth>0 && i==(leapmonth+1) && !date->leap)
		{
			--i;
			date->leap = 1;
			temp = ((l_info[date->ly-1900] & 0x10000)? 30: 29);
		}
		else
		{
			temp = ( (l_info[date->ly-1900] & (0x10000>>i))? 30: 29 );
		}
		if(date->leap && i==(leapmonth+1))
			date->leap = 0;
		offset -= temp;
	}
	if(offset==0 && leapmonth>0 && i==leapmonth+1)
		if(date->leap)
		{
			date->leap = 0;
		}

		else
		{
			date->leap = 1;
			--i;
		}
	if(offset<0)
	{
		offset += temp;
		--i;
	}
	if(offset==0)
		offset=temp;
	if(i==13)
		i--;
	date->lm = i;
	date->ld = offset;
	date->jq=0;
}
