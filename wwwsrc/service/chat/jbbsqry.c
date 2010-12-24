//jbbsqry.c
//xml格式的用户信息

#include "BBSLIB.inc"
#include "NEWLIB.inc"

#define QRY_IDNAME 0x1
#define QRY_NICKNAME 0x2
#define QRY_HOROSCOPE 0x4
#define QRY_SEX 0x8
#define QRY_LASTLOGIN 0x10
#define QRY_LASTLOGOUT 0x20
#define QRY_LASTHOST 0x40
#define QRY_NEWMAIL 0x80

#define QRY_POSTNUM 0x100
#define QRY_POSTC 0x200
#define QRY_EXPNUM 0x400
#define QRY_EXPC 0x800
#define QRY_LOGINNUM 0x1000
#define QRY_LIFE 0x2000
#define QRY_PERFNUM 0x4000
#define QRY_PERFC 0x8000

#define QRY_MEDAL 0x10000
#define QRY_PERMSTR 0x20000
#define QRY_SPECIAL 0x40000
#define QRY_BMLIST 0x80000
#define QRY_RECLUSION 0x100000
#define QRY_STATUS 0x200000
#define QRY_SMD 0x400000
#define QRY_CORPUS 0x800000

/*
<idinfo>
bit0	<idname>id</idname>
bit1	<nickname>昵称</nickname>
bit2	<horoscope>星座</horoscope>
bit3	<sex>性别</sex>
bit4	<lastlogin>上次登录</lastlogin>
bit5	<lastlogout>上次注销</lastlogout>
bit6	<lasthost>登录ip</lasthost>
bit7	<newmail>信</newmail>

bit8	<postnum>文章数</postnum>
bit9	<postc>文章等级</postc>
bit10	<expnum>经验值</expnum>
bit11	<expc>经验等级</expc>
bit12	<loginnum>登录次数</loginnum>
bit13	<life>生命力</life>
bit14	<perfnum>表现值</perfnum>
bit15	<perfc>表现等级</perfc>

bit16	<medal>
		<total>奖章数</total>
		<medalitem>
			<type>奖章名字</type>
			<count>奖章数量</type>
		</medalitem>
	</medal>
bit17	<permstr>权限</permstr>
bit18	<special>称号</special>
bit19	<bmlist>
		<bmitem>板面名字</bmitem>
	</bmlist>
bit20 <reclusion>归隐天数,-15表示自杀</reclusion>
bit21	<status>
		<statusitem>状态</statusitem>
	</status>
bit22	<smd>
		...
	</smd>
bit23	<corpus>文集</corpus>
</idinfo>
*/

int show_special2(char *id2)
{
	FILE *fp;
	char  id1[80], name[80];
	fp=fopen("etc/sysops", "r");
	if(fp==0)
	return;
	while(1) {
		id1[0]=0;
		name[0]=0;
		if(fscanf(fp, "%s %s", id1, name)<=0)
			break;
		if(!strcmp(id1, id2))
		{
			printf("<special>%s</special>", nohtml(noansi(name)));
			//似乎一个special就够了
			break;
		}
	}
	fclose(fp);
}

int main()
{
	int mode, tmp1, tmp2, i;
	struct userec *x;
	struct user_info *u;
	char useridbuf[16];
	char * userid;
	char buf[256];
	FILE * fp,*fq;
	xml_init();
	mode = atoi(getparm("mode"));
	//mode |= 1;//必须显示id吧？
	if (!mode)
	{
		mode = -1;
	}
	strsncpy(useridbuf, getparm("userid"), 13);
	//strsncpy(useridbuf, "ziteng", 13);
	userid = trim(useridbuf);
	x = getuser(userid);
	printf("<?xml version=\"1.0\" encoding=\"gb18030\" ?>\n");
	printf("<idinfo>\n");
	if (!x)
	{
		printf ("<idname></idname>\n");
		mode = 0;
		//相当于退出
	}
	if (mode & QRY_IDNAME)
	{
		printf ("<idname>%s</idname>\n", x->userid);
	}
	if (mode & QRY_NICKNAME)
	{
		printf ("<nickname>%s</nickname>\n", nohtml(noansi(x->username)));
	}
	if ((mode & QRY_HOROSCOPE) && (x->userdefine & DEF_S_HOROSCOPE))
	{
		printf ("<horoscope>%s</horoscope>\n", horoscope(x->birthmonth, x->birthday));
	}
	if ((mode & QRY_SEX) && (x->userdefine & DEF_S_HOROSCOPE))
	{
		printf ("<sex>%c</sex>\n", x->gender);
	}
	if (mode & QRY_LASTLOGIN)
	{
		getdatestring(x->lastlogin, NA);
		printf ("<lastlogin>%s</lastlogin>\n", datestring);
	}
	if (mode & QRY_LASTLOGOUT)
	{
		if(x->lastlogout < x->lastlogin) 
		{
			getdatestring(((time(0)-x->lastlogin)/120)%47+1+x->lastlogin,NA);
		}
		else
		{
			getdatestring(x->lastlogout,NA);
		}
		printf ("<lastlogout>%s</lastlogout>\n", datestring);
	}
	if (mode & QRY_LASTHOST)
	{
		printf ("<lasthost>%s</lasthost>\n", x->lasthost);
	}
	if (mode & QRY_NEWMAIL)
	{
		count_mails(userid, &tmp1, &tmp2);
		if (strcmp(currentuser.userid, x->userid))
		{
			if (tmp2 > 0)
			{
			 	tmp2 = 1;
			}
		}
		printf ("<newmail>%d</newmail>\n", tmp2);
	}
	if (mode & QRY_POSTNUM)
	{
		printf ("<postnum>%d</postnum>\n", x->numposts);
	}
	if (mode & QRY_POSTC)
	{
		printf ("<postc>%s</postc>\n", numpostsstr(x));
	}
	if (mode & QRY_EXPNUM)
	{
		printf ("<expnum>%d</expnum>\n", countexp(x));
	}
	if (mode & QRY_EXPC)
	{
		printf ("<expc>%s</expc>\n", expstr(x));
	}
	if (mode & QRY_LOGINNUM)
	{
		printf ("<loginnum>%d</loginnum>\n", x->numlogins);
	}
	if (mode & QRY_LIFE)
	{
		printf ("<life>%d</life>\n", count_life_value(x));
	}
	if (mode & QRY_PERFNUM)
	{
		printf ("<perfnum>%d</perfnum>\n", countperf(x));
	}
	if (mode & QRY_PERFC)
	{
		printf ("<perfc>%s</perfc>\n", perfstr(x));
	}
	if (mode & QRY_MEDAL)
	{
		tmp2 = x->nummedals;
		tmp1 = 0;  // 根据位置判断奖章类型
		for (i = 1; i <= 7; ++i) {
			tmp1 += (tmp2 >> (i << 2)) & 0x0f;
		}
		printf ("<medal><total>%d</total>\n", tmp1);
		for (i = 7; i > 0; i--)
		{
			tmp1 = (tmp2 >> (i << 2)) & 0x0f;
			if (!tmp1)
			{
				continue;
			}
			j_hsprintf(1, NULL, NULL);
			j_hsprintf(0, buf, "%s", cmedal(8-i));
			printf ("<medalitem><type>%s</type><count>%d</count></medalitem>\n", buf, tmp1);
		}
		printf ("</medal>\n");
	}
	if (mode & QRY_PERMSTR)
	{
		uleveltochar(buf, x->userlevel);
		printf ("<permstr>%s</permstr>\n", buf);
	}
	if (mode & QRY_SPECIAL)
	{
		show_special2(userid);
	}
	if ((mode & QRY_BMLIST) && (x->userlevel & PERM_BOARDS))
	{
		sprintf(buf, "%s/home/%c/%s/.bmfile", BBSHOME, toupper(x->userid[0]),x->userid);
		if (fp = fopen(buf, "rt"))
		{
			printf ("<bmlist>");
			for(i = 0; i < 3; i++)
			{
				if (feof(fp))
				{
					break;
				}
				fscanf (fp, "%s\n", buf);
				if (!getbcache(buf))
				{
					continue;
				}
				printf ("<bmitem>%s</bmitem>", buf);
			}
			printf ("</bmlist>\n");
			fclose(fp);
		}
	}
	if (mode & QRY_RECLUSION)
	{
		printf ("<reclusion>");
		if (!(x->userlevel & PERM_SYSOP) && (x->userlevel & PERM_RECLUSION))
		{
			sprintf(buf, "home/%c/%s/Reclusion", toupper(x->userid[0]), x->userid);
			if ((file_exist(buf)) && (fp = fopen(buf, "rt")))
			{
				fscanf(fp, "%d", &tmp1);
				fclose(fp);
			}
			printf ("归隐中，还有%d天", (tmp1-(time(0)-x->lastlogout)/86400)>0?(tmp1-(time(0)-x->lastlogout)/86400):0);
		}
		else if (!(x->userlevel & PERM_SYSOP) && (x->userlevel & PERM_SUICIDE))
		{
			printf ("自杀中");
		}
		printf ("</reclusion>");
	}
	if (mode & QRY_STATUS)
	{
		printf ("<status>");
		for(i = 0; i < MAXACTIVE; i++) 
		{
			u=&(shm_utmp->uinfo[i]);
			if(!strcmp(u->userid, x->userid))
			{
				if(u->active==0 || u->pid==0 || u->invisible && !HAS_PERM(PERM_SEECLOAK) && strcmp(u->userid,currentuser.userid) && !canseeme(u->userid))
				{
					continue;
				}
				printf ("<statusitem>");
				if(u->invisible)
				{
					printf ("［隐］");
				}
				tmp1 = u->mode;
				if (tmp1 > 20000)
				{
					tmp1 -= 20000;
					printf ("@");
				}
				printf ("%s", noansi(ModeType(tmp1)));
				printf ("</statusitem>\n");
			}
		}
		printf ("</status>");
	}
	if (mode & QRY_SMD)
	{
		printf ("<smd>");
		sprintf (buf, "home/%c/%s/plans", toupper(x->userid[0]), x->userid); 
		j_show_file (buf, 0);
		printf ("</smd>\n");
	}
	if (mode & QRY_CORPUS)
	{
		printf ("<corpus>");
		sprintf(buf, "0Announce/PersonalCorpus/%c/%s", toupper(x->userid[0]), x->userid);
		if(file_exist(buf))
		{
			printf ("yes");
		}
		printf ("</corpus>");
	}
	printf("</idinfo>\n");
	return 0;
}

