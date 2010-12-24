/* $Id: bbspattern.c,v 1.2 2008-12-19 09:55:04 madoldman Exp $
 
*/

#include"BBSLIB.inc"
#include"boardrc.inc"
int readbin(char *s,int *q,FILE *fp)
{
        int tmp;
        if(fread(&tmp,sizeof(int),1,fp)<=0)
                return 0;
        if(tmp==9999)
                return 0;
        *q=tmp;
        fread(&tmp,sizeof(int),1,fp);
        fread(s,tmp,1,fp);
        return 1;

}

int main()
{


        init_all();
        char path[STRLEN],board[STRLEN],title[STRLEN],buf[STRLEN];
        int total=0,num;
        struct pattern p;

        strsncpy(board, getparm("board"), 20);
        num=atoi(getparm("num"));
        if(!has_post_perm(&currentuser, board))
                http_fatal("�������������������Ȩ�ڴ���������������");
        //huangxu@060331

        int  mode=atoi(getparm("mode"));
        setbfile(path,board,"pattern");
        total=get_num_records(path,sizeof(p));
        if(total==0) {
                showinfo("�ð����޷���ģ��.");
                return 1;
        }

        if(num!=0)
                if(num>total||num<0) {
                        showinfo("����ȷ��ģ��.");
                        return 1;
                } else
                        if(mode)
                                goto DOPOST;
                        else
                                goto POST;

        printf("<center>%s ����ģ��<br>",board);
        printf("----------------------------------------------------------");
        printf("<table width=80% boarder=0 align=center>");
        printf("<tr><td>���</td><td>ģ������</td><td>�������</td><td></td></tr>");
        FILE *fp=fopen(path,"r");
        if(fp==0) {
                printf("�ļ���ȡ����,������.");
                return 1;
        }
        int  i=1;
        while(1) {
                if(fread(&p, sizeof(p), 1, fp)<=0)
                        break;
                printf("<tr><td>%d</td><td>%s</td><td>%d</td><td><a href=bbspattern?board=%s&&num=%d>ʹ�ø�ģ�巢��</a>\
                       </td></tr>",i,p.name,p.question,board,i);
                i++;
        }

        printf("</table>");
        fclose(fp);
        return 0;



POST:
        get_record(&p,sizeof(p),num-1,path);
        printf("<center>%s ģ�巢��</center><br>",board);
        printf("ʹ��ģ�� %s",p.name);
        sprintf(buf,"%s.question",p.file);
        setbfile(path,board,buf);
        FILE *fq=fopen(path,"r");
        if(fq==0) {
                printf("��ģ��Ŀǰ������.");
                return -1;
        }
        struct question q;
        if(fread(&q,sizeof(struct question),1,fq)<=0) {
                printf("ģ�������ļ���ȡ����.");
                return -1;
        }

        printf("<form name=form1 action=bbspattern?board=%s&&num=%d&&mode=1 method=post>",board,num);
        printf("ʹ�ñ���: <input type=text name=title size=40 maxlength=48>");
        printf("<br>  ʹ��ǩ���� ");
        printf("<input type=radio name=signature value=1 checked>1");
        printf("<input type=radio name=signature value=2>2");
        printf("<input type=radio name=signature value=3>3");
        printf("<input type=radio name=signature value=4>4");
        printf("<input type=radio name=signature value=5>5");
        printf("<input type=radio name=signature value=0>0");
        printf(" [<a target=_blank href=bbssig>�鿴ǩ����</a>] ");
        printf("<input type=checkbox name=noreply>���ɻظ�");

        struct shortfile *bp = getbcache(board);
        if(bp->flag & OUT_FLAG)                                         
             printf("<input type=checkbox checked name=outgo>ת��");      



        int size=0;
        for(i=1;i<=p.question ;i++ ) {
                size=q.q[i-1].size;
                if(size<=80)
                        printf("<br><br>%d.%s (��%d��)<br><input type=text name=p%d size=60 maxlength=size>",i,q.q[i-1].con,size,i,size);
                else
                        printf("<br><br>%d.%s (%d ��)<br><textarea  rows=5 cols=80 wrap=physicle name=p%d></textarea>",i,q.q[i-1].con,size,i);

        }
        printf("<br><br><input type=submit class=btn value=�������� onclick=\"this.value='���Ժ�...';this.disabled=true;form1.submit();\">");
        printf("<input type=button class=btn value=�����༭ onclick=javascript:history.go(-1)>");
        printf("</form>");
        return 0;

DOPOST:
 
	if(!isAllowedIp(fromhost) && !HAS_PERM(PERM_BOARDS)) {
		http_fatal("�Բ��𣬱�վ�������û�ʵ��ֻ�����������Ĵ�������");
	}
        if(!has_post_perm(&currentuser, board))
                http_fatal("����������Ψ����, ����������Ȩ���ڴ˷�������");
        sprintf(buf, "boards/%s/deny_users", board);
        if(file_has_word(buf, currentuser.userid))
                http_fatal("�ܱ�Ǹ, �㱻������Աֹͣ�˱����postȨ��");
        if(abs(time(0) - *(int*)(u_info->from+36))<6) {
                *(int*)(u_info->from+36)=time(0);
                http_fatal("���η��ļ������, ����Ϣ���������");
        }

        get_record(&p,sizeof(p),num-1,path);

        sprintf(buf,"%s.question",p.file);
        setbfile(path,board,buf);
        fq=fopen(path,"r");
        sprintf(buf,"%s.bin",p.file);
        setbfile(path,board,buf);
        FILE *fbin=fopen(path,"r");
        if(fq==0||fbin==0) {
                printf("��ģ��Ŀǰ������.");
                return -1;
        }

        if(fread(&q,sizeof(struct question),1,fq)<=0) {
                printf("ģ�������ļ���ȡ����.");
                return -1;
        }

        sprintf(path, "tmp/%d.%s.tmp", getpid(),currentuser.userid);
        FILE *out=fopen(path,"w+");
        if(out==0) {
                printf("�½��ļ�����.");
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

        int  r=post_article2(board, title, path, currentuser.userid, currentuser.username, fromhost, sig-1, accessed,0,outgo);
        unlink(path);
        sprintf(buf,"M.%d.A",r);
        brc_init(currentuser.userid, board);
        brc_add_read(buf);
        brc_update(currentuser.userid, board);
        sprintf(buf, "posted WWW '%s' on '%s'", title, board);
        report(buf);
        sprintf(buf, "bbsdoc?board=%s#bottom", board);
        if(!junkboard(board)) {
                currentuser.numposts++;
                save_user_data(&currentuser);
                //	write_posts(currentuser.userid, board, title);
        }
        redirect(buf);
        return 0;
}

