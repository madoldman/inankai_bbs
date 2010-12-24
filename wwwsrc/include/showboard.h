/* 返回板面在讨论区排行榜中的位次 */
int chartpos(char *board)
{
	FILE *fp;
	int i, r;
	char buf[256], tmp[80], name[80];
	fp=fopen("0Announce/bbslist/todayboard2", "r");
	if(fp==0) return 0;
	for(i=0; i<MAXBOARD; i++)
	{
		if(fgets(buf, 150, fp)==0) break;
		if(i==0) continue;
		r=sscanf(buf, "%s %s %s %s %s %s", tmp, tmp, name, tmp, tmp, tmp);
		if(r==6&&!strcmp(name, board))
		{
			fclose(fp);
			return i;
		}
	}
	fclose(fp);
	return 0;
}

/* 返回文章字数 */
char *eff_size(char *file) {
	FILE *fp;
	static char buf[256];
	int i, size, size2=0;
	size=file_size(file);
	if(size>3000|| size==0) goto E;
	size=0;
	fp=fopen(file, "r");
	if(fp==0) return "-";
	for(i=0; i<3; i++)
		if(fgets(buf, 255, fp)==0) break;
	while(1) {
		if(fgets(buf, 255, fp)==0) break;
		if(!strcmp(buf, "--\n")) break;
		if(!strncmp(buf, ": ", 2)) continue;
		if(!strncmp(buf, "【 在 ", 4)) continue;
		if(strstr(buf, "※ 来源:．")) continue;
		for(i=0; buf[i]; i++) if(buf[i]<0) size2++;
		size+=strlen(trim(buf));
	}	
	fclose(fp);
E:	
	sprintf (buf, "%d", size-size2/2);
	return buf;
}

/* 记录访问板面日志 */
int board_usage(mode)
char   *mode;
{
	time_t  now;
	char    buf[256];
	now = time(0);
	getdatestring(now,NA);
	sprintf(buf, "%.22s USE %-20.20s Stay: %5ld (%s)WWW\n", datestring, mode, 10, currentuser.userid);
	f_append("use_board", buf);
	return 1;
}

/* 板面IP限制 */

int board_ban_ip(char *board)
{
//	if((!strcmp(board, "SARS")||!strcmp(board, "Nankai_News"))&&!isAllowedIp(fromhost))
         if((!strcmp(board, "SARS"))&&!isAllowedIp(fromhost)) 
                                 http_fatal("本板面暂时关闭访问,欢迎您以后再来");

	return 1;
}
/*
int isAllowedIp(char *ip)
{
if(strstr(ip,"211.102.114.")==ip || strstr(ip,"211.102.112.")==ip || strstr(ip,"61.181.245.238")==ip || strstr(ip,"211.102.110.")==ip || strstr(ip,"10.10.")==ip || strstr(ip,"202.113.16.")==ip || strstr(ip,"202.113.17.")==ip || strstr(ip,"202.113.18.")==ip || strstr(ip,"202.113.19.")==ip || strstr(ip,"202.113.20.")==ip || strstr(ip,"202.113.21.")==ip || strstr(ip,"202.113.22.")==ip || strstr(ip,"202.113.23.")==ip || strstr(ip,"202.113.24.")==ip || strstr(ip,"202.113.25.")==ip || strstr(ip,"202.113.26.")==ip || strstr(ip,"202.113.27.")==ip || strstr(ip,"202.113.28.")==ip || strstr(ip,"202.113.29.")==ip || strstr(ip,"202.113.30.")==ip || strstr(ip,"202.113.31.")==ip || strstr(ip,"202.113.224.")==ip || strstr(ip,"202.113.225.")==ip || strstr(ip,"202.113.226.")==ip || strstr(ip,"202.113.227.")==ip || strstr(ip,"202.113.228.")==ip || strstr(ip,"202.113.229.")==ip || strstr(ip,"202.113.230.")==ip || strstr(ip,"202.113.231.")==ip || strstr(ip,"202.113.232.")==ip || strstr(ip,"202.113.233.")==ip || strstr(ip,"202.113.234.")==ip || strstr(ip,"202.113.235.")==ip || strstr(ip,"202.113.236.")==ip || strstr(ip,"202.113.237.")==ip || strstr(ip,"202.113.238.")==ip || strstr(ip,"202.113.239.")==ip || strstr(ip,"61.136.13.22")==ip)
return 1;
else return 0;
}*/




