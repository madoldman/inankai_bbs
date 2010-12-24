//bbspattern.wml.c
//huangxu@nkbbs.org
//此文件完全照bbspattern.c改写
//因为不大明白，只好这样咯

//模板发文用。
//这里没有做“强制模板发文”

#include "BBSLIB.inc"
#include "boardrc.inc"
#include "WMLLIB.inc"

int readbin(char *s,int *q,FILE *fp)
{
	int tmp;
	if(fread(&tmp,sizeof(int),1,fp)<=0)
	{
		return 0;
	}
	if(tmp==9999)
	return 0;
	*q=tmp;
	fread(&tmp,sizeof(int),1,fp);
	fread(s,tmp,1,fp);
	return 1;
}

int bbspattern_act(char * info)
{
	char path[STRLEN],board[STRLEN],title[STRLEN],buf[STRLEN];
	int total=0,num;
	struct pattern p;
	strsncpy(board, getparm("board"), 20);
	num=atoi(getparm("num"));
	if (loginok)
	{
		wmlmode (u_info, POSTING);
	}
	if(!has_post_perm(&currentuser, board))
	{
		strcpy(info, "错误的讨论区或者您无权在此讨论区发表文章");
		return -1;
	}
	int  mode=atoi(getparm("mode"));
	setbfile(path,board,"pattern");
	total=get_num_records(path,sizeof(p));
	if(total==0) {
		strcpy(info, "该板尚无发文模板.");
		return -1;
	}

	if(num!=0)
		if(num>total||num<0) {
			strcpy(info, "不正确的模板.");
			return -1;
		} else
			if(mode)
				goto DOPOST;
			else
				goto POST;
//大汗，goto
	FILE *fp=fopen(path,"r");
	if(fp==0) {
		strcpy(info, "文件读取错误,请重试.");
		return 1;
	}
	int  i=1;
	printf ("<card title=\"发文模板 -- %s\">", BBSNAME);
	while(1) {
		if(fread(&p, sizeof(p), 1, fp)<=0)
			break;
		w_hsprintf (buf, "%s", p.name);
		printf("<p><anchor><go href=\"bbspattern.wml?board=%s&amp;num=%d\" />#%d %s %d个问题</anchor></p>",board, i, i, buf, p.question);
		i++;
	}
	fclose(fp);
	return 0;



POST:
	get_record(&p,sizeof(p),num-1,path);
	sprintf(buf,"%s.question",p.file);
	setbfile(path,board,buf);
	FILE *fq=fopen(path,"r");
	if(fq==0) {
		strcpy(info, "该模板目前不可用.");
		return -1;
	}
	struct question q;
	if(fread(&q,sizeof(struct question),1,fq)<=0) {
		strcpy(info, "模板问题文件读取错误.");
		return -1;
	}

	printf ("<card title=\"发文模板 -- %s\">", BBSNAME);
	w_hsprintf (buf, "%s", p.name);
	printf("<p>使用模板 %s</p>",buf);
	//好吧我只支持wml2了
	printf("<form action=\"bbspattern.wml?board=%s&amp;num=%d&amp;mode=1\" method=\"post\">",board,num);
	printf("<p>使用标题: <input type=\"text\" name=\"title\" maxlength=\"48\" />");
	printf("<br /> ");
	printf ("签名档：<select title=\"签名档\" multiple=\"false\" name=\"sig\" value=\"0\">");
	printf ("<option value=\"0\" title=\"无\">无</option>");
	for (i = 1; i < 6; i++)
	{
		printf ("<option value=\"%d\" title=\"%d\">%d</option>", i, i, i);
	}
	printf ("</select>");
	printf("<input type=\"checkbox\" name=\"noreply\" /> 不可回复");

	struct shortfile *bp = getbcache(board);
	if(bp->flag & OUT_FLAG)					 
	     printf("<input type=\"checkbox\" checked=\"checked\" name=\"outgo\" />转信");      
	int size=0;
	for(i=1;i<=p.question ;i++ ) {
		size=q.q[i-1].size;
		w_hsprintf(buf, "%s", q.q[i - 1].con);
		if(size<=80)
			printf("<br /><br />%d.%s (限%d字)<br /><input type=\"text\" name=\"p%d\" size=\"60\" maxlength=\"size\" />",i,buf,size,i,size);
		else
			printf("<br /><br />%d.%s (%d 字)<br /><textarea  rows=\"5\" cols=\"80\" name=\"p%d\"></textarea>",i,buf,size,i);
	}
	printf("<br /><br /><input type=\"submit\" value=\"立即发表\" /></p>");
	printf("</form>");
	return 0;

DOPOST:
 
	if(!isAllowedIp(fromhost) && !HAS_PERM(PERM_BOARDS)) {
			sprintf (info, "对不起，您需要绑定中国大陆地区教育网邮箱才能发文。请使用您的计算机访问本站以执行此操作。");
			return -5;
	}
	if(abs(time(0) - *(int*)(u_info->from+36))<6) {
		*(int*)(u_info->from+36)=time(0);
		sprintf (info, "两次发文间隔过密, 请休息几秒后再试");
		return -5;
	}

	get_record(&p,sizeof(p),num-1,path);

	sprintf(buf,"%s.question",p.file);
	setbfile(path,board,buf);
	fq=fopen(path,"r");
	sprintf(buf,"%s.bin",p.file);
	setbfile(path,board,buf);
	FILE *fbin=fopen(path,"r");
	if(fq==0||fbin==0) {
		sprintf(info, "该模板目前不可用.");
		return -1;
	}

	if(fread(&q,sizeof(struct question),1,fq)<=0) {
		sprintf(info, "模板问题文件读取错误.");
		return -1;
	}

	sprintf(path, "tmp/%d.%s.tmp", getpid(),currentuser.userid);
	FILE *out=fopen(path,"w+");
	if(out==0) {
		sprintf(info,"新建文件错误.");
		return -1;
	}
	sprintf(title,"%s%s%s",q.title1,getparm("title"),q.title2);
	title[50]=0;


	int accessed=0;
	if(strlen(getparm("noreply"))>0)
	    accessed=FILE_NOREPLY;
	int outgo=0;
	if(strlen(getparm("outgo"))>0)       
	    outgo=1;	

	int  sig=atoi(getparm("signature"));

	char *ptr[MQ+1];
	ptr[0]=title;
	for(i=1;i<=p.question ;i++ ) {
		size=q.q[i-1].size;
		ptr[i]=(char *)malloc(size);
		bzero(ptr[i],size);
		sprintf(buf,"p%d",i);
		strsncpy(ptr[i],getparm(buf),size);
	}
	//   printf("ok");return 0;
	i=0;
	char tmpb[AS];
	while(1) {
		bzero(tmpb,AS);
		if(!readbin(tmpb,&i,fbin))
			break;
		if(i==8888)
			fprintf(out,"%s",tmpb);

		else
			fprintf(out,"%s%s",tmpb,ptr[i]);
	}
	for(i=1;i<=p.question;i++)
		free(ptr[i]);




	fcloseall();

	int  r=post_article2_d(board, title, path, currentuser.userid, currentuser.username, fromhost, sig-1, accessed,0,outgo, "Wap模板");
	unlink(path);
	sprintf(buf,"M.%d.A",r);
	brc_init(currentuser.userid, board);
	brc_add_read(buf);
	brc_update(currentuser.userid, board);
	sprintf(buf, "posted Wap '%s' on '%s'", title, board);
	report(buf);
	sprintf(buf, "bbsdoc.wml?board=%s", board);
	if(!junkboard(board)) {
		currentuser.numposts++;
		save_user_data(&currentuser);
		//	write_posts(currentuser.userid, board, title);
	}
	printf ("<card title=\"发文成功 -- %s\" ontimer=\"%s\">", BBSNAME, buf);
	printf ("<timer value=\"30\" />");
	printf ("<p>模板发文成功，自动跳转中……</p>");
	printf ("<p><anchor><go href=\"%s\" />如果不能自动跳转，请使用此链接。</anchor></p>", buf);
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbspattern_act(buf))
	{
		printf ("<card title=\"错误\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />返回</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
