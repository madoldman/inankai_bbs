#include "BBSLIB.inc"
#include "filemgr.inc"

int boardact(char * board)
{
	struct fmg_attdir ad;
	char buf[256];
	int i,v;
	int min=0;
	char path[80];
	FILE * fp;
	if(!*(getparm("act")))return 0;
	ad=fmg_openattdir(board,0);
	if(!(ad.fp))return 0;
	for(i=0;i<8;i++)
	{
		sprintf(buf,"maxsize%d",i);
		if(*(getparm(buf)))
		{
			v=atoi(getparm(buf));
			if(v<0)v=0;
			ad.fh.maxsize[i]=v;
		}
		if(ad.fh.maxsize[i]>0&&(min>ad.fh.maxsize[i]||min<=0))min=ad.fh.maxsize[i];
	}
	if(*(getparm("maxtotal")))
	{
		v=atoi(getparm("maxtotal"));
		if(v<0)v=0;
		if(v&&v<min)v=min;
		ad.fh.maxtotal=v;
	}
	if(*(getparm("maxfsize")))
	{
		v=atoi(getparm("maxfsize"));
		if(v<0)v=0;
		if(v>INT_SYSMAXUPFILESIZE)v=INT_SYSMAXUPFILESIZE;
		ad.fh.maxfsize = v;
	}
	if(*(getparm("defaultlife")))
	{
		v=atoi(getparm("defaultlife"));
		if(v>=0&&v<8)ad.fh.defaultlife=v;
	}
	fmg_closeattdir(&ad);
	printf("<h5 align=\"center\">�����޸��Ѿ���Ч��</h5>");
	sprintf(path, "tmp/%d.tmp", getpid());
	fp = fopen(path, "w");
	fprintf(fp, "�µ��޶����£�\n");
	fprintf(fp, "�ܸ���: %d MB\n", ad.fh.maxtotal);
	for(i = 0; i < 8; i++)
	{
		fprintf(fp, "%s����: %d MB %s\n",fmg_lifestr[i], ad.fh.maxsize[i], ad.fh.defaultlife==i?"Ĭ��":"");
	}
	fprintf(fp, "�����ļ���С: %d Bytes\n", ad.fh.maxfsize);
	getuinfo(fp);
	fclose(fp);
	sprintf(buf, "�޸�%s�帽���޶�", board);
	post_article("syssecurity1", buf, path, currentuser.userid, "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1);
	unlink(path);
	return 1;
}

int boardshow(char * board)
{
	struct fmg_attdir ad;
	char buf[256];
	int i;
	ad=fmg_openattdir(board,1);
	if(!(ad.fp))return 0;
	printf("<h3 align=\"center\">%s ��������</h3>",board);
	printf("<form action=\"bbsattadmin?board=%s&act=update\" method=\"post\">",board);
	printf("<table align=\"center\" boarder=\"1\"><tr><td>��������</td><td><a href=\"bbsatt?board=%s\">%s</a></td><td><a href=\"bbsatt?board=%s\">�鿴����</a></td></tr>",board,board,board);
	fmg_sizestr_d(buf,ad.fh.totalsize);
	printf("<tr><td>���帽��</td><td>����%s</td><td>/�޶�<input type=\"text\" name=\"maxtotal\" value=\"%d\" size=\"4\">MB</td></tr>",buf,ad.fh.maxtotal);
	for(i = 0; i < 8; i++)
	{
		fmg_sizestr_d(buf,ad.fh.itemsize[i]);
		printf("<tr><td>%s����</td><td>����%s</td><td>/�޶�<input type=\"text\" name=\"maxsize%d\" value=\"%d\" size=\"4\">MB</td></tr>",fmg_lifestr[i],buf,i,ad.fh.maxsize[i]);
	}
	//fmg_sizestr_d(buf,ad.fh.itemsize[0]);
	//printf("<tr><td>30�츽��</td><td>����%s/�޶�<input type=\"text\" name=\"maxsize0\" value=\"%d\" size=\"4\">MB</td></tr>",buf,ad.fh.maxsize[0]);
	*buf=0;
	//sprintf(buf,"<option value=\"0\" %s>%s</option><option value=\"7\" %s>%s</option>",(ad.fh.defaultlife==0)?"selected":"",fmg_lifestr[0],
	//	(ad.fh.defaultlife==7)?"selected":"",fmg_lifestr[7]);
	printf("<tr><td>Ĭ��������</td><td><select name=\"defaultlife\" >");
	for(i = 0; i < 8; i++)
	{
		printf("<option value=\"%d\" %s>%s</option>", i, (ad.fh.defaultlife==i)?"selected":"", fmg_lifestr[i]);
	}
	printf("</select></td></tr>");
	printf("<tr><td>�����ļ�����</td><td><input type='text' name='maxfsize' value='%d' size='4' /></td><td>Bytes</td></tr>", ad.fh.maxfsize);
	printf("<tr><td><input type=\"submit\" value=\"�ύ\"></td><td><input type=\"reset\" value=\"��д\"></td></tr>");
	printf("</table>");
	printf("<p align=\"center\">����������<a href=\"fmbuild?board=%s\">�ؽ�����</a>&nbsp;<a href=\"bbsattadmin\">�������渽������</a></p>",board);
	fmg_closeattdir(&ad);
	return 1;
}

/*int zoneact(int zone)
{
	if(!*(getparm("act")))return 0;
	struct fmg_attdir ad;
	int i,j,v;
	char board[40];
	struct shortfile *x;
	char buf[128];
	int min=0;
	for(i=0;;i++)
	{
		sprintf(buf,"brd%d",i);
		strsncpy(board,getparm(buf),32);
		if(!(x=getbcache(board)))
			break;
		strcpy(board,x->filename);
		//if(!has_read_perm(&currentuser,board))continue;
		ad=fmg_openattdir(board,0);
		if(!(ad.fp))break;
		for(j=0;j<8;j++)
		{
			sprintf(buf,"maxsize%d_%d",i,j);
			if(*(getparm(buf)))
			{
				v=atoi(getparm(buf));
				if(v<0)v=0;
				ad.fh.maxsize[j]=v;
			}
			if(ad.fh.maxsize[j]>0&&(min>ad.fh.maxsize[j]||min<=0))min=ad.fh.maxsize[j];
		}
		sprintf(buf,"maxtotal%d",i);
		if(*(getparm(buf)))
		{
			v=atoi(getparm(buf));
			if(v<0)v=0;
			if(v&&v<min)v=min;
			ad.fh.maxtotal=v;
		}
		sprintf(buf,"defaultlife%d",i);
		if(*(getparm(buf)))
		{
			v=atoi(getparm(buf));
			if(v>=0&&v<8)ad.fh.defaultlife=v;
		}
		fmg_closeattdir(&ad);
	}
	return 1;
}
*/

int zoneshow(int zone)
{
	//huangxu:��̫�࣬û��Ū�޸��ˡ������ι۰ɡ�
	struct fmg_attdir ad;
	char buf[256];
	struct shortfile *x;
	int i,j,cnt=0;
	printf("<h3 align=\"center\">%s ��������</h3>",secname[zone]);
	printf("<hr>");//<form action=\"bbsattadmin?zone=%d&act=update\" method=\"post\">",zone);
	printf("<table align=\"center\" border=\"0\">");
	//printf("<tr bgcolor=\"%6s\"><td align=\"center\" rowspan=\"2\"><font color=\"%6s\">����</font></td>"
	//"<td align=\"center\" colspan=\"3\"><font color=\"%6s\">���帽��</font></td><td align=\"center\" colspan=\"3\"><font color=\"%6s\">���ø���</font></td>"
	//"<td align=\"center\" colspan=\"3\"><font color=\"%6s\">30�츽��</font></td><td align=\"center\" rowspan=\"2\"><font color=\"%6s\">Ĭ��������</font></td>"
	//"<tr bgcolor=\"%6s\"><td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td>"
	//"<td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td>"
	//"<td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td></tr>",
	//mytheme.bar,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.bar,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf);
	printf("<tr bgcolor=\"%6s\"><td align=\"center\" rowspan=\"2\"><font color=\"%6s\">����</font></td>"
	"<td align=\"center\" colspan=\"3\"><font color=\"%6s\">����ϼ�</font></td>",mytheme.bar,mytheme.lf,mytheme.lf);
	for(j = 0; j < 8; j++)
	{
		printf("<td align=\"center\" colspan=\"3\"><font color=\"%6s\">%s����</font></td>",mytheme.lf,fmg_lifestr[j]);
	}
	printf("<td align=\"center\" rowspan=\"2\"><font color=\"%6s\">Ĭ��������</font></td></tr>",mytheme.lf);
	printf("<tr bgcolor=\"%6s\">",mytheme.bar);
	for(j = -1; j < 8; j++)//��Ϊ���кϼ�
	{
		printf("<td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td>",
			mytheme.lf,mytheme.lf,mytheme.lf);
	}
	printf("</tr>");
	cnt=0;
	for(i=0;i<MAXBOARD;i++)
	{
		x=&(shm_bcache->bcache[i]);
		if(x->filename[0]<=32 || x->filename[0]>'z')continue;
		//if(!has_read_perm(&currentuser, x->filename))continue;
		if(!strchr(seccode[zone], x->title[0]))continue;
		if(x->flag&ZONE_FLAG)continue;
		ad=fmg_openattdir(x->filename,1);
		if(!(ad.fp))continue;
		printf("<tr><td align=\"center\"><a href=\"bbsattadmin?board=%s\">%s</a></td>",
			x->filename,x->filename,cnt,x->filename);
		fmg_sizestr_d(buf,ad.fh.totalsize);
		//printf("<td align=\"right\">%s</td><td>/</td><td><input type=\"text\" name=\"maxtotal%d\" value=\"%d\" size=\"4\">MB</td>",buf,cnt,ad.fh.maxtotal);
		printf("<td align=\"right\">%s</td><td>/</td><td>%d MB</td>",buf,ad.fh.maxtotal);
		//fmg_sizestr_d(buf,ad.fh.itemsize[7]);
		//printf("<td align=\"right\">%s</td><td>/</td><td><input type=\"text\" name=\"maxsize%d_7\" value=\"%d\" size=\"4\">MB</td>",buf,cnt,ad.fh.maxsize[7]);
		//fmg_sizestr_d(buf,ad.fh.itemsize[0]);
		//printf("<td align=\"right\">%s</td><td>/</td><td><input type=\"text\" name=\"maxsize%d_0\" value=\"%d\" size=\"4\">MB</td>",buf,cnt,ad.fh.maxsize[0]);
		for(j = 0; j < 8; j++)
		{
			fmg_sizestr_d(buf,ad.fh.itemsize[j]);
			printf("<td align=\"right\">%s</td><td>/</td><td>%d MB</td>",buf,ad.fh.maxsize[j]);
		}
		//sprintf(buf,"<option value=\"0\" %s>%s</option><option value=\"7\" %s>%s</option>",(ad.fh.defaultlife==0)?"selected":"",fmg_lifestr[0],
		//	(ad.fh.defaultlife==7)?"selected":"",fmg_lifestr[7]);
		//printf("<td><select name=\"defaultlife%d\" >%s</select></td></tr>",cnt,buf);
		printf("<td align=\"right\"> %s </td></tr>",fmg_lifestr[ad.fh.defaultlife]);
		fmg_closeattdir(&ad);
		cnt++;
	}
	printf("</table>");
	//<p align=\"center\"><input type=\"submit\" value=\"�ύ\">&nbsp;<input type=\"reset\" value=\"��д\"></p></form>");
	printf("<p align=\"center\">ѡ�������е��帽���޶�������ߵ�<a href=\"bbsattadmin\">����</a>����ȫ�ֹ���</p>");
	return 1;
}

int fmtotalsort(void * c1,void * c2)
{
	double x1=((struct fmg_FileHead*)(c1))->totalsize;
	double x2=((struct fmg_FileHead*)(c2))->totalsize;
	if(x1>x2)
		return -1;
	else if(x1<x2)
		return 1;
	else
		return 0;
}

int globalreport()
{
	struct fmg_attdir ad;
	char buf[256];
	struct shortfile *x;
	int i,j,cnt;
	struct fmg_FileHead fh[MAXBOARD];
	memset (fh, 0, sizeof(fh));
	printf("<hr>");//<form action=\"bbsattadmin?zone=%d&act=update\" method=\"post\">",zone);
	printf("<table align=\"center\" border=\"0\">");
	//printf("<tr bgcolor=\"%6s\"><td align=\"center\" rowspan=\"2\"><font color=\"%6s\">����</font></td>"
	//"<td align=\"center\" colspan=\"3\"><font color=\"%6s\">���帽��</font></td><td align=\"center\" colspan=\"3\"><font color=\"%6s\">���ø���</font></td>"
	//"<td align=\"center\" colspan=\"3\"><font color=\"%6s\">30�츽��</font></td><td align=\"center\" rowspan=\"2\"><font color=\"%6s\">Ĭ��������</font></td>"
	//"<tr bgcolor=\"%6s\"><td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td>"
	//"<td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td>"
	//"<td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td></tr>",
	//mytheme.bar,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.bar,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf,mytheme.lf);
	printf("<tr bgcolor=\"%6s\"><td align=\"center\" rowspan=\"2\"><font color=\"%6s\">����</font></td>"
	"<td align=\"center\" colspan=\"3\"><font color=\"%6s\">����ϼ�</font></td>",mytheme.bar,mytheme.lf,mytheme.lf);
	for(j = 0; j < 8; j++)
	{
		printf("<td align=\"center\" colspan=\"3\"><font color=\"%6s\">%s����</font></td>",mytheme.lf,fmg_lifestr[j]);
	}
	printf("<td align=\"center\" rowspan=\"2\"><font color=\"%6s\">Ĭ��������</font></td></tr>",mytheme.lf);
	printf("<tr bgcolor=\"%6s\">",mytheme.bar);
	for(j = -1; j < 8; j++)//��Ϊ���кϼ�
	{
		printf("<td><font color=\"%6s\">����</font></td><td><font color=\"%6s\">/</font></td><td><font color=\"%6s\">�޶�</font></td>",
			mytheme.lf,mytheme.lf,mytheme.lf);
	}
	printf("</tr>");
	cnt=0;
	for(i=0;i<MAXBOARD;i++)
	{
		x=&(shm_bcache->bcache[i]);
		if(x->filename[0]<=32 || x->filename[0]>'z')continue;
		//if(!has_read_perm(&currentuser, x->filename))continue;
		if(x->flag&ZONE_FLAG)continue;
		//printf("--%s--",x->filename);
		ad=fmg_openattdir(x->filename,1);
		if(!(ad.fp))continue;
		memcpy(fh+cnt,&(ad.fh),sizeof(struct fmg_FileHead));
		fmg_closeattdir(&ad);
		cnt++;
	}
	qsort(fh,cnt,sizeof(struct fmg_FileHead),fmtotalsort);
	for(i=0;i<cnt;i++)
	{
		printf("<tr><td align=\"center\"><a href=\"bbsattadmin?board=%s\">%s</a></td>",
			(fh+i)->name,(fh+i)->name,(fh+i)->name);
		fmg_sizestr_d(buf,(fh+i)->totalsize);
		//printf("<td align=\"right\">%s</td><td>/</td><td><input type=\"text\" name=\"maxtotal%d\" value=\"%d\" size=\"4\">MB</td>",buf,cnt,ad.fh.maxtotal);
		printf("<td align=\"right\">%s</td><td>/</td><td>%d MB</td>",buf,(fh+i)->maxtotal);
		//fmg_sizestr_d(buf,ad.fh.itemsize[7]);
		//printf("<td align=\"right\">%s</td><td>/</td><td><input type=\"text\" name=\"maxsize%d_7\" value=\"%d\" size=\"4\">MB</td>",buf,cnt,ad.fh.maxsize[7]);
		//fmg_sizestr_d(buf,ad.fh.itemsize[0]);
		//printf("<td align=\"right\">%s</td><td>/</td><td><input type=\"text\" name=\"maxsize%d_0\" value=\"%d\" size=\"4\">MB</td>",buf,cnt,ad.fh.maxsize[0]);
		for(j = 0; j < 8; j++)
		{
			fmg_sizestr_d(buf,(fh+i)->itemsize[j]);
			printf("<td align=\"right\">%s</td><td>/</td><td>%d MB</td>",buf,(fh+i)->maxsize[j]);
		}
		//sprintf(buf,"<option value=\"0\" %s>%s</option><option value=\"7\" %s>%s</option>",(ad.fh.defaultlife==0)?"selected":"",fmg_lifestr[0],
		//	(ad.fh.defaultlife==7)?"selected":"",fmg_lifestr[7]);
		//printf("<td><select name=\"defaultlife%d\" >%s</select></td></tr>",cnt,buf);
		printf("<td align=\"right\"> %s </td></tr>",fmg_lifestr[(fh+i)->defaultlife]);
	}
	printf("</table>");
	printf("<p align=\"center\">ѡ��һ���������й���</p>");
	return 1;
}

int globalshow()
{
	printf("<h3 align=\"center\">��������</h3>");
	printf("<p align=\"center\">��ѡ��һ�������鿴</h3>");
	int i;
	printf("<table align=\"center\" boarder=\"0\"><tr>");
	for(i=0;i<SECNUM;i++)
	{
		printf("<td><a href=\"bbsattadmin?zone=%d\">%s</a></td>",i,secname[i]);
		if(i%3==2)printf("</tr><tr>");
	}
	printf("</tr></table>");
	globalreport();
	return 1;
}

int main()
{
	init_all();
	char board[40];
	int zone=-1;
	struct shortfile *x;
	if(!HAS_PERM(PERM_ANNOUNCE)&&(!HAS_PERM(PERM_SYSOP))&&(!HAS_PERM(PERM_OBOARDS)))http_fatal("����Ȩʹ�ñ�ҳ");
	strsncpy(board, getparm("board"), 32);
	if(!(x=getbcache(board)))
		*board=0;
	else
	{
		strcpy(board,x->filename);
		//if(!has_read_perm(&currentuser,board))http_fatal("��û��Ȩ�޷��ʴ�������");
		boardact(board);
		boardshow(board);
		http_quit();
	}
	zone=atoi(getparm("zone"));
	if(!*(getparm("zone")))zone=-1;
	if(zone>=0&&zone<SECNUM)
	{
		//zoneact(zone);
		zoneshow(zone);
		http_quit();
	}
	globalshow();
	http_quit();
	return 0;
}
