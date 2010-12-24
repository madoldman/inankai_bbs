//bbspattern.wml.c
//huangxu@nkbbs.org
//���ļ���ȫ��bbspattern.c��д
//��Ϊ�������ף�ֻ��������

//ģ�巢���á�
//����û������ǿ��ģ�巢�ġ�

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
		strcpy(info, "�������������������Ȩ�ڴ���������������");
		return -1;
	}
	int  mode=atoi(getparm("mode"));
	setbfile(path,board,"pattern");
	total=get_num_records(path,sizeof(p));
	if(total==0) {
		strcpy(info, "�ð����޷���ģ��.");
		return -1;
	}

	if(num!=0)
		if(num>total||num<0) {
			strcpy(info, "����ȷ��ģ��.");
			return -1;
		} else
			if(mode)
				goto DOPOST;
			else
				goto POST;
//�󺹣�goto
	FILE *fp=fopen(path,"r");
	if(fp==0) {
		strcpy(info, "�ļ���ȡ����,������.");
		return 1;
	}
	int  i=1;
	printf ("<card title=\"����ģ�� -- %s\">", BBSNAME);
	while(1) {
		if(fread(&p, sizeof(p), 1, fp)<=0)
			break;
		w_hsprintf (buf, "%s", p.name);
		printf("<p><anchor><go href=\"bbspattern.wml?board=%s&amp;num=%d\" />#%d %s %d������</anchor></p>",board, i, i, buf, p.question);
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
		strcpy(info, "��ģ��Ŀǰ������.");
		return -1;
	}
	struct question q;
	if(fread(&q,sizeof(struct question),1,fq)<=0) {
		strcpy(info, "ģ�������ļ���ȡ����.");
		return -1;
	}

	printf ("<card title=\"����ģ�� -- %s\">", BBSNAME);
	w_hsprintf (buf, "%s", p.name);
	printf("<p>ʹ��ģ�� %s</p>",buf);
	//�ð���ֻ֧��wml2��
	printf("<form action=\"bbspattern.wml?board=%s&amp;num=%d&amp;mode=1\" method=\"post\">",board,num);
	printf("<p>ʹ�ñ���: <input type=\"text\" name=\"title\" maxlength=\"48\" />");
	printf("<br /> ");
	printf ("ǩ������<select title=\"ǩ����\" multiple=\"false\" name=\"sig\" value=\"0\">");
	printf ("<option value=\"0\" title=\"��\">��</option>");
	for (i = 1; i < 6; i++)
	{
		printf ("<option value=\"%d\" title=\"%d\">%d</option>", i, i, i);
	}
	printf ("</select>");
	printf("<input type=\"checkbox\" name=\"noreply\" /> ���ɻظ�");

	struct shortfile *bp = getbcache(board);
	if(bp->flag & OUT_FLAG)					 
	     printf("<input type=\"checkbox\" checked=\"checked\" name=\"outgo\" />ת��");      
	int size=0;
	for(i=1;i<=p.question ;i++ ) {
		size=q.q[i-1].size;
		w_hsprintf(buf, "%s", q.q[i - 1].con);
		if(size<=80)
			printf("<br /><br />%d.%s (��%d��)<br /><input type=\"text\" name=\"p%d\" size=\"60\" maxlength=\"size\" />",i,buf,size,i,size);
		else
			printf("<br /><br />%d.%s (%d ��)<br /><textarea  rows=\"5\" cols=\"80\" name=\"p%d\"></textarea>",i,buf,size,i);
	}
	printf("<br /><br /><input type=\"submit\" value=\"��������\" /></p>");
	printf("</form>");
	return 0;

DOPOST:
 
	if(!isAllowedIp(fromhost) && !HAS_PERM(PERM_BOARDS)) {
			sprintf (info, "�Բ�������Ҫ���й���½����������������ܷ��ġ���ʹ�����ļ�������ʱ�վ��ִ�д˲�����");
			return -5;
	}
	if(abs(time(0) - *(int*)(u_info->from+36))<6) {
		*(int*)(u_info->from+36)=time(0);
		sprintf (info, "���η��ļ������, ����Ϣ���������");
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
		sprintf(info, "��ģ��Ŀǰ������.");
		return -1;
	}

	if(fread(&q,sizeof(struct question),1,fq)<=0) {
		sprintf(info, "ģ�������ļ���ȡ����.");
		return -1;
	}

	sprintf(path, "tmp/%d.%s.tmp", getpid(),currentuser.userid);
	FILE *out=fopen(path,"w+");
	if(out==0) {
		sprintf(info,"�½��ļ�����.");
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

	int  r=post_article2_d(board, title, path, currentuser.userid, currentuser.username, fromhost, sig-1, accessed,0,outgo, "Wapģ��");
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
	printf ("<card title=\"���ĳɹ� -- %s\" ontimer=\"%s\">", BBSNAME, buf);
	printf ("<timer value=\"30\" />");
	printf ("<p>ģ�巢�ĳɹ����Զ���ת�С���</p>");
	printf ("<p><anchor><go href=\"%s\" />��������Զ���ת����ʹ�ô����ӡ�</anchor></p>", buf);
	return 0;
}

int main()
{
	char buf[256];
	wml_init();
	wml_head();
	if (bbspattern_act(buf))
	{
		printf ("<card title=\"����\" >");
		printf ("<p>%s</p>", buf);
	}
	printf ("<p><anchor><prev />����</anchor></p>");
	wml_bar(0);
	printf ("</card>");
	wml_tail();
	return 0;
}
