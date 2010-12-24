/*$Id: adminwebdefault.c,v 1.3 2009-02-10 15:07:15 sillyhan Exp $
 */
#include "BBSLIB.inc"
#define nqoutput(x) noquoteoutput((x),sizeof(x))
#define fnqoutput(y,x) fnoquoteoutput((y),(x),sizeof(x))

char JSTAG[]="<!--ADMINWEBDEFAULT_JS-->";

struct webdef//1024B
{
	char pic[480];
	char link[480];
	char alt[60];
	int right;
};

void noquoteoutput(char * src,int length)
{
	char * p=src;
	printf("\"");
	for(p=src;(*p)&&(p<src+length);p++)
	{
		printf("%c",*p);
		if(*p=='\"')printf("\"");
	}
	printf("\"");
	return;
}

void fnoquoteoutput(FILE * fp,char * src,int length)
{
	char * p=src;
	for(p=src;(*p)&&(p<src+length);p++)
	{
		if(*p==0)break;
		if(*p=='\"'||*p=='\''||*p=='\\')fprintf(fp,"\\");
		fprintf(fp,"%c",*p);
	}
	return;
}

void modifydefault()
{
	FILE * fp;
	if(!(fp=fopen("etc/webdesign/config","r")))
		if(!(fp=fopen("etc/webdesign/config","w+")))
			http_fatal("�޷����ļ�");
	struct webdef w;
	int i;
	printf("<h5 align=center><font color=red>��������60�ֽ�;��ַ����480�ֽ�;��ʹ�����·��,��/��ʼ��</font></h5>");
	printf("<script language=\"javascript\">function add(){	var c=parseInt(document.getElementById('count').value);"
	"	var oPIC=document.createElement('input');	var oALT=document.createElement('input');	var oLINK=document.createElement('input');"
	"	var oRIGHT=document.createElement('input');	var oDEL=document.createElement('a');	var oTDpic=document.createElement('td');"
	"	var oTDalt=document.createElement('td');	var oTDlink=document.createElement('td');	var oTDright=document.createElement('td');"
	"	var oTDdel=document.createElement('td');	var oTR=document.createElement('tr');	oPIC.setAttribute('name','pic_'+c);"
	"	oALT.setAttribute('name','alt_'+c);	oLINK.setAttribute('name','link_'+c);	oRIGHT.setAttribute('name','right_'+c);"
	"	oPIC.setAttribute('type','text');	oALT.setAttribute('type','text');	oLINK.setAttribute('type','text');	oRIGHT.setAttribute('type','text');"
	"	oDEL.setAttribute('href','javascript:del('+c+');');	oDEL.innerHTML='ɾ��';	oTDpic.appendChild(oPIC);	oTDalt.appendChild(oALT);"
	"	oTDlink.appendChild(oLINK);	oTDright.appendChild(oRIGHT);	oTDdel.appendChild(oDEL);	oTR.setAttribute('id','tr_'+c);	oTR.appendChild(oTDpic);"
	"	oTR.appendChild(oTDalt);	oTR.appendChild(oTDlink);	oTR.appendChild(oTDright);	oTR.appendChild(oTDdel);"
	"	document.getElementById('tblmain').appendChild(oTR);	document.getElementById('count').value=c+1;	return;}"
	"function del(x){	var c=parseInt(document.getElementById('count').value);	if(x>=c||x<0)return;	var o=document.getElementById('tr_'+x);"
	"	for(;o.childNodes.length;o.removeChild(o.firstChild));	return;}</script>");
	printf("<form action=adminwebdefault?mode=17 method=post>");
	printf("<table align=center border=0 ><tbody id=tblmain>");
	printf("<tr><td>ͼƬ</td><td>����</td><td>ָ�������</td><td>Ȩ��</td><td><a href='javascript:add();'>����</a></td></tr>");
	for(i=0;;i++)
	{
		if(!fread(&w,sizeof(w),1,fp))break;
		printf("<tr id=tr_%d><td><input type=text name=pic_%d value=",i,i);
		nqoutput(w.pic);
		printf(" /></td><td><input type=text name=alt_%d value=",i);
		nqoutput(w.alt);
		printf(" /></td><td><input type=text name=link_%d value=",i);
		nqoutput(w.link);
		printf(" /></td><td><input type=text name=right_%d value=%d />",i,w.right);
		printf("</td><td><a href='javascript:del(%d);'>ɾ��</a></td></tr>",i);
	}
	printf("</tbody></table><input type=hidden name=count id=count value=%d />",i);
	printf("<p align=center><input type=submit value=�ύ>&nbsp;<input type=reset value=��д></p></form>");
	fclose(fp);
	return;
}

void savemodifiedpage()
{
	struct webdef w;
	FILE * fp,fpw;
	char * pbuf;
	int c=atoi(getparm("count"));
	int v=0;
	int i;
	int r,l;
	int rsum=0;
	char * ptag;
	char input[16];
	struct webdef * pw;
	if(!(pw=(struct webdef *)(malloc(c*sizeof(w)))))
		http_fatal("һ�����");
	for(i=0;i<c;i++)
	{
		sprintf(input,"right_%d",i);
		r=atoi(getparm(input));
		if(r<=0)continue;
		w.right=r;
		sprintf(input,"pic_%d",i);
		strncpy(w.pic,getparm(input),sizeof(w.pic)-1);
		if(!*(w.pic))continue;
		sprintf(input,"alt_%d",i);
		strncpy(w.alt,getparm(input),sizeof(w.alt)-1);
		if(!*(w.alt))continue;
		sprintf(input,"link_%d",i);
		strncpy(w.link,getparm(input),sizeof(w.link)-1);
		if(!*(w.link))continue;
		rsum+=w.right;
		*(pw+(v++))=w;
	}
	if(v<=0)http_fatal("���󡪡�������һ��");
	if(!(fp=fopen("etc/webdesign/config","w")))
		http_fatal("�޷����ļ�");
	fwrite(pw,sizeof(w),v,fp);
	fclose(fp);
	if(!(fp=fopen("etc/webdesign/default.html","r")))
		http_fatal("�޷����ļ�");
	fseek(fp,0,2);
	l=ftell(fp);
	if(!(pbuf=(char*)(malloc(l))))
		http_fatal("һ�����");
	fseek(fp,0,0);
	fread(pbuf,1,l,fp);
	fclose(fp);
	if(!(ptag=strstr(pbuf,JSTAG)))http_fatal("ģ�����");
	if(!(fp=fopen("/home/www/html/default.html","w")))
		http_fatal("�޷����ļ�");
	fwrite(pbuf,1,ptag-pbuf,fp);
	fprintf(fp,"<script language=javascript>var n=Math.floor(Math.random()*%d);",rsum);
	fprintf(fp,"var o=document.getElementById('tdpic');");
	rsum=0;
	for(i=0;i<v;i++)
	{
		if(i)(fprintf(fp,"else "));
		fprintf(fp,"if(n<%d)o.innerHTML=\"<a href=\\\"",rsum+=(pw+i)->right);
		fnqoutput(fp,(pw+i)->link);
		fprintf(fp,"\\\" title=\\\"");
		fnqoutput(fp,(pw+i)->alt);
		fprintf(fp,"\\\"><img border=0 src=\\\"");
		fnqoutput(fp,(pw+i)->pic);
		fprintf(fp,"\\\"></a>\";");
	}
	fprintf(fp,"</script>");
	fwrite(ptag+strlen(JSTAG),1,l-(ptag-pbuf)-strlen(JSTAG),fp);
	fclose(fp);
	free(pbuf);
	free(pw);
	printf("<a href=/ >��������ҳ�ɹ�</a>");
	return;
}
int check(FILE * fp)
{
        if(fp==NULL) {
                printf("can not open file");
                return 0;
        }
        return 1;
}
int readfile(char *path,char *bgbuf)
{

        int fd=0;
        FILE*fp;
        if((fd=open(path,O_RDONLY))==EOF) {
                printf("�ļ������ڣ��½��ļ�");//ʵ���ϴ򲻿�,������ڵĶ��ܴ�
                fopen(path,"w+");
                //http_quit();
        } else {
        				fp=fopen(path,"r");
        				fseek(fp,0,2);
        				fd=ftell(fp);
        				fseek(fp,0,0);
        				if(fd>=102400)fd=102400-1;
                bzero(bgbuf,102400);
                fread(bgbuf,1,fd,fp);
                fclose(fp);
        }

}
int out(char *title,char *inc,char *bgbuf,int mode)
{


        printf("<center>%s -- %s[ʹ����: %s]<hr color=%s></center>", BBSNAME, title,currentuser.userid,mytheme.bar);


        printf(" <br>\
               <form action=adminwebdefault method=post name=form1>\
               <input type=hidden name=mode value=%d>\
               %s\
               <br><br><textarea   onkeypress='if(event.keyCode==10) return document.form1.submit()' name=text rows=10 cols=100 wrap=off>%s</textarea><br>&nbsp\
               <input type=submit value=ok>\
               <input type=button value=cancel onclick='javascript:history.go(-1)'>\
\
\
               ",mode+16,inc,bgbuf);//huangxu@060513:ע��,+16
}

int writefile(char *path)
{
        char bgbuf[102400];
        strcpy(bgbuf,getparm("text"));
        if(bgbuf!=NULL) {
                FILE *fp=fopen(path,"w+");
                if(check(fp)) {
                        fprintf(fp,"%s",bgbuf);
                        fclose(fp);
                }
        }
        return 0;
}

void adminwebdefault(int mode)
{
        char bgbuf[102400];
        char path[STRLEN];
        char buf[512];
        if(mode==2) {//�ֹ��༭��ҳ
                strcpy(path,"/home/www/html/default.html");
                readfile(path,bgbuf);
                strcpy(buf,"HTML����,�����ù��ߴ���ú�ճ��");
                out("�ֹ��༭��ҳ",buf,bgbuf,mode);
                http_quit();
                return;
        }
        if(mode==3) {//�ֹ��༭��ҳģ��

                strcpy(path,"/home/bbs/etc/webdesign/default.html");
                readfile(path,bgbuf);
                strcpy(buf,"HTML����,���ù��ߴ���ú�ճ��<br>��ע���滻���:<font color=red>&lt!--ADMINWEBDEFAULT_JS--&gt;</font>");
                out("�ֹ��༭��ҳģ��",buf,bgbuf,mode);
                http_quit();
                return;
        }
        if(mode==4) {//�ֹ��༭��ҳ����
                strcpy(path,"/home/www/html/index/announce.js");
                readfile(path,bgbuf);
                strcpy(buf,"HTML����,�����ù��ߴ���ú�ճ��");
                out("�ֹ��༭��ҳ����",buf,bgbuf,mode);
                http_quit();
                return;
        }
        if(mode==18) {
                strcpy(path,"/home/www/html/default.html");
                writefile(path);
        }
        if(mode==19) {
                strcpy(path,"/home/bbs/etc/webdesign/default.html");
                writefile(path);
        }
        if(mode==20) {
                strcpy(path,"/home/www/html/index/announce.js");
                writefile(path);
        }
        if(mode&16)
        {
                printf("<a href=adminwebdefault>done</a>");
        }
}

int main()
{
        init_all();
        int mode=0;
        mode =atoi(getparm("mode"));
        if(!loginok) {
                showinfo("��û�е�½��");
                http_quit();
                return 1;
        }
        if(!HAS_PERM(PERM_WELCOME)) {
                showinfo("����Ȩ���д��������");
                http_quit();
                return 1;
        }

        printf("<body background=%s><font color=%s>",mytheme.bgpath,mytheme.mf);
        switch (mode)
        {
        	case 0:
        		printf("<h3 align=center>Web��ҳ����</h3>");
        		printf("<p align=center><a href='adminwebdefault?mode=1'>�޸���ҳ</a>&nbsp;<a href='adminwebdefault?mode=2'>�ֹ��༭</a>&nbsp;<a href='adminwebdefault?mode=3'>�༭ģ��</a>&nbsp;<a href='adminwebdefault?mode=4'>��ҳ����</a></p>");
        		break;
        	case 1:
        		printf("<h3 align=center>�޸���ҳ</h3>");
        		modifydefault();
        		break;
        	case 2:
        	case 3:
        	case 4:
        	case 18:
        	case 19:
        	case 20:
        		adminwebdefault(mode);
        		break;
        	case 17:
        		savemodifiedpage();
        		break;
        	default:
        		break;
        }
        http_quit();
        return;
}


