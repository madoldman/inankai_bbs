#include <dirent.h>

int fmg_life[]={30,3,7,14,60,120,180,-1};//�����ڣ�-1��ʾ�����
char * fmg_lifestr[]={"30��","3��","7��","14��","60��","120��","180��","����"};
int fmg_defmaxsize=100;
int fmg_defmaxtotal=0;

#define INT_SYSMAXUPFILESIZE 20 * 1024 * 1024

struct fmg_FileHead//512B
{
	char name[24];      // 20B ����
	double totalsize;   //  8B �ܴ�С
	double itemsize[8]; // 64B �����С
	int reccount;       //  4B ��¼����
	int nextid;         //  4B ��һ����¼��id
	int defaultlife;    //  4B Ĭ��������
	int maxsize[8];     // 32B �ߴ�����
	int maxtotal;       //  4B
	int maxfsize;
	char access[108];   //108B
	char access2[256];  //256B
};

struct fmg_FileRec//512B
{
	//�ļ���¼
	char filename[224];//�ļ���
	char uploader[15];//�ϴ���
	char flag;//bit0~3:��������
	time_t timeupload;
	int filesize;
	int longflag;
	int id;
	char info[256];//˵��
};

struct fmg_attdir
{
	FILE * fp;
	struct fmg_FileHead fh;
	int prec;//��¼��
	int todelmin;//׼��ɾ���ļ�¼
	int mode;//1=readonly
};

struct fmg_attdir fmg_openattdir(char * board,int openmode)//��Ŀ¼
{
	//openmode:0 r+ 1 r
	char path[128];
	FILE * fp;
	struct shortfile *x;
	struct fmg_attdir ad;
	ad.fp=0;
	if(!(x=getbcache(board)))return ad;
	snprintf(path,127,"boards/%s/.attdir",x->filename);
	fp=fopen(path,openmode?"r":"r+");
	if(!fp)
	{
		fp=fopen(path,"w");
		memset(&(ad.fh),0,sizeof(ad.fh));
		strcpy(ad.fh.name,x->filename);
		ad.fh.maxsize[7]=fmg_defmaxsize;
		ad.fh.maxtotal=fmg_defmaxtotal;
		fwrite(&(ad.fh),sizeof(ad.fh),1,fp);
		fclose(fp);
		fp=fopen(path,openmode?"r":"r+");
		fseek(fp,0,2);
	}
	else
		fread(&(ad.fh),sizeof(ad.fh),1,fp);
	strcpy(ad.fh.name,x->filename);
	ad.fp=fp;
	ad.prec=0;
	ad.todelmin=-1;
	ad.mode=openmode;
	return ad;
}

int fmg_saveattdir(struct fmg_attdir * ad)//����
{
	int i;
	int dlt=0;
	struct fmg_FileRec frec;
	FILE * fp=ad->fp;
	if(!(fp)||ad->mode)return 0;
	flock(fileno(fp),LOCK_EX);
	fseek(fp,0,0);
	fwrite(&(ad->fh),sizeof(ad->fh),1,fp);
	fseek(fp,ad->prec*sizeof(struct fmg_FileRec),1);
	if(ad->todelmin>=0)
	{
		dlt=0;
		for(i=ad->todelmin;;i++)
		{
			fseek(fp,i*sizeof(struct fmg_FileRec)+sizeof(struct fmg_FileHead),0);
			if(!fread(&frec,sizeof(frec),1,fp))break;
			if(!*(frec.filename))
			{
				dlt++;
				continue;
			}
			fseek(fp,-(dlt+1)*sizeof(struct fmg_FileHead),1);
			fwrite(&frec,sizeof(frec),1,fp);
		}
		ftruncate(fileno(fp), ad->fh.reccount*sizeof(struct fmg_FileRec)+sizeof(struct fmg_FileHead));
	}
	flock(fileno(ad->fp),LOCK_UN);
	ad->todelmin=-1;
	return 1;
}

int fmg_closeattdir(struct fmg_attdir * ad)//�ر�
{
	fmg_saveattdir(ad);
	fclose(ad->fp);
	ad->fp=NULL;
	ad->prec=-1;
	return 1;
}

int fmg_seekrec(char * filename,struct fmg_attdir * ad)//������¼���ļ�����
{
	if(!(ad->fp))return -2;
	fseek(ad->fp,sizeof(ad->fh),0);
	int i=0;
	FILE * fp=ad->fp;
	struct fmg_FileRec frec;
	for(;;i++)
	{
		if(!fread(&frec,sizeof(frec),1,fp))break;
		if(!strcmp(filename,frec.filename))
		{
			ad->prec=i+1;
			return i;
		}
	}
	return -1;
}

int fmg_seekrec_i(int id,struct fmg_attdir * ad)//������¼��id��
{
	int i,min,max;
	struct fmg_FileRec frec;
	FILE * fp=ad->fp;
	min=0;
	if(id<0||id>=ad->fh.nextid)return -3;
	fseek(fp,0,2);
	max=(ftell(fp)-sizeof(struct fmg_FileHead))/sizeof(struct fmg_FileRec);
	for(;max>min+1;)
	{
		i=(max+min)/2;
		fseek(fp,i*sizeof(struct fmg_FileRec)+sizeof(struct fmg_FileHead),0);
		for(*(frec.filename)=0;!*(frec.filename);i++)
		{
			if(i==max)break;
			fread(&frec,sizeof(frec),1,fp);
		}
		if(frec.id==id)
		{
			ad->prec=i+1;
			return i;
		}
		else if(frec.id>id)
			max=i;
		else
			min=i;
	}
	fseek(fp,min*sizeof(struct fmg_FileRec)+sizeof(struct fmg_FileHead),0);
	for(i=min;i<=max;i++)
	{
		fread(&frec,sizeof(frec),1,fp);
		if(frec.id==id)
		{
			ad->prec=i+1;
			return i;
		}
	}
	ad->prec=max+1;
	return -1;
}

int fmg_addrec_d(struct fmg_FileRec * frec,struct fmg_attdir * ad)//������ֱ������
{
	if(!(ad->fp)||ad->mode)return 0;
	#ifdef FMG_SHOWDETAIL
	printf("Add file: %s %s",frec->filename,conmode?"\n":"<br>");
	#endif
	fseek(ad->fp,0,2);
	frec->id=ad->fh.nextid++;
	flock(fileno(ad->fp),LOCK_EX);
	fwrite(frec,sizeof(struct fmg_FileRec),1,ad->fp);
	flock(fileno(ad->fp),LOCK_UN);
	ad->prec=++(ad->fh.reccount);
	ad->fh.totalsize+=frec->filesize;
	ad->fh.itemsize[(frec->flag)&7]+=frec->filesize;
	return (ad->fh.reccount-1);
}

int fmg_addrec(struct fmg_FileRec * frec,struct fmg_attdir * ad)//������¼
{
	if(!(ad->fp))return 0;
	int i;
	i=fmg_seekrec(frec->filename,ad);
	if(i<0)
		return fmg_addrec_d(frec,ad);
	else
		return i;
}

int fmg_delrec_p(int position,struct fmg_attdir * ad)//ɾ����¼��λ�ã�
{
	struct fmg_FileRec frec;
	if(ad->mode)return -3;
	if(position<0)return position;
	fseek(ad->fp,position*sizeof(struct fmg_FileRec)+sizeof(struct fmg_FileHead),0);
	fread(&frec,sizeof(frec),1,ad->fp);
	if(*(frec.filename))
	{
		#ifdef FMG_SHOWDETAIL
		printf("Delete file: %s %s",frec.filename,conmode?"\n":"<br>");
		#endif
		ad->fh.totalsize-=frec.filesize;
		ad->fh.itemsize[(frec.flag&7)]-=frec.filesize;
		unlink(frec.filename);
		memset(&(frec.filename),0,sizeof(frec.filename));
		fseek(ad->fp,-sizeof(frec),1);
		fwrite(&frec,sizeof(frec),1,ad->fp);
	}
	if(ad->todelmin<0||ad->todelmin>position)ad->todelmin=position;
	ad->fh.reccount--;
	return position;
}

int fmg_delrec_n(char * filename,struct fmg_attdir * ad)//ɾ����¼���ļ�����
{
	return fmg_delrec_p(fmg_seekrec(filename,ad),ad);
}

int fmg_delrec_i(int id,struct fmg_attdir * ad)//ɾ����¼��id��
{
	return fmg_delrec_p(fmg_seekrec_i(id,ad),ad);
}

struct fmg_FileRec fmg_buildrec(char * filename,char * userid,char lifeflag)//������¼
{
	struct fmg_FileRec frec;
	memset(&frec,0,sizeof(frec));
	strcpy(frec.filename,filename);
	strcpy(frec.uploader,userid);
	frec.flag=(lifeflag&7);
	frec.timeupload=file_time(filename);
	frec.filesize=file_size(filename);
	return frec;
}

int fmg_addrec_n(char * filename,char * userid,char lifeflag,struct fmg_attdir * ad)//������¼���ļ�����
{
	int i;
	struct fmg_FileRec frec;
	i=fmg_seekrec(filename,ad);
	if(i>=0)return i;
	frec=fmg_buildrec(filename,userid,lifeflag);
	return fmg_addrec_d(&frec,ad);
}

int fmg_addrec_ns(char * filename,int filesize,char * userid,char lifeflag,struct fmg_attdir * ad)//������¼���ļ�����
{
	int i;
	struct fmg_FileRec frec;
	i=fmg_seekrec(filename,ad);
	if(i>=0)return i;
	frec=fmg_buildrec(filename,userid,lifeflag);
	frec.filesize=filesize;
	return fmg_addrec_d(&frec,ad);
}

int fmg_checkexist(struct fmg_attdir * ad)//�����ļ�
{
	struct fmg_FileRec frec;
	int i;
	int s;
	ad->fh.totalsize=0;
	for(i=0;i<8;i++)
		ad->fh.itemsize[i]=0;
	fseek(ad->fp,sizeof(struct fmg_FileHead),0);
	for(i=0;;i++)
	{
		if(!fread(&frec,sizeof(frec),1,ad->fp))break;
		if(!file_exist(frec.filename))fmg_delrec_p(i,ad);
		else
		{
			s=file_size(frec.filename);
			if(s!=frec.filesize)
				frec.filesize=s;
			ad->fh.totalsize+=s;
			ad->fh.itemsize[frec.flag&7]+=s;
		}
	}
	fmg_saveattdir(ad);
	return i;
}

int fmg_changelife(char * filename,int lifemode,struct fmg_attdir * ad)
{
	if(!(ad->fp)||(ad->mode))return -2;
	int i=fmg_seekrec(filename,ad);
	struct fmg_FileRec frec;
	if(lifemode<-1||lifemode>7)return -4;
	if(i<0)return -1;
	if(lifemode==-1)
	{
		fmg_delrec_p(i,ad);
		return 1;
	}
	fseek(ad->fp,i*sizeof(struct fmg_FileRec)+sizeof(struct fmg_FileHead),0);
	fread(&frec,sizeof(frec),1,ad->fp);
	if(ad->fh.maxsize[lifemode]>0&&(ad->fh.itemsize[lifemode]+frec.filesize>ad->fh.maxsize[lifemode]*1048576.0))
		return -3;
	ad->fh.itemsize[lifemode]+=frec.filesize;
	ad->fh.itemsize[frec.flag&7]-=frec.filesize;
	frec.flag&=~7;
	frec.flag|=lifemode;
	fseek(ad->fp,-sizeof(struct fmg_FileRec),1);
	fwrite(&frec,sizeof(frec),1,ad->fp);
	return 1;
}

int fmg_addsinglefile_s(char * board,char * filename,int filesize,char * userid)
{
	struct fmg_attdir ad;
	ad=fmg_openattdir(board,0);
	if(!(ad.fp))return 0;
	if((ad.fh.maxsize[ad.fh.defaultlife]>0&&(ad.fh.itemsize[ad.fh.defaultlife]+filesize>ad.fh.maxsize[ad.fh.defaultlife]*1048576.0))||
		(ad.fh.maxtotal>0&&ad.fh.totalsize+filesize>ad.fh.maxtotal*1048576.0))
	{
		unlink(filename);
		fmg_closeattdir(&ad);
		return 0;
	}
	fmg_addrec_ns(filename,filesize,userid,ad.fh.defaultlife,&ad);
	fmg_closeattdir(&ad);
	return 1;
}

int fmg_addsinglefile(char * board,char * filename,char * userid)
{
	struct fmg_attdir ad;
	ad=fmg_openattdir(board,0);
	if(!(ad.fp))return 0;
	if((ad.fh.maxsize[ad.fh.defaultlife]>0&&(ad.fh.itemsize[ad.fh.defaultlife]+file_size(filename)>ad.fh.maxsize[ad.fh.defaultlife]*1048576.0))||
		(ad.fh.maxtotal>0&&ad.fh.totalsize+file_size(filename)>ad.fh.maxtotal*1048576.0)
		||
		(ad.fh.maxfsize && ad.fh.maxfsize < file_size(filename)))
	{
		unlink(filename);
		fmg_closeattdir(&ad);
		return 0;
	}
	fmg_addrec_n(filename,userid,ad.fh.defaultlife,&ad);
	fmg_closeattdir(&ad);
	return 1;
}

int fmg_sizestr(char * buf,int size)
{
	if(size>=1048576000)
		sprintf(buf,"%.3fGB",size/(1024*1024*1024.0));
	else if(size>1024000)
		sprintf(buf,"%.3fMB",size/(1048576.0));
	else if(size>1000)
		sprintf(buf,"%.3fKB",size/(1024.0));
	else
		sprintf(buf,"%d     B",size);
	return strlen(buf);
}

int fmg_sizestr_d(char * buf,double size)
{
	if(size>=1048576000)
		sprintf(buf,"%.3fGB",size/(1024*1024*1024.0));
	else if(size>1024000)
		sprintf(buf,"%.3fMB",size/(1048576.0));
	else if(size>1000)
		sprintf(buf,"%.3fKB",size/(1024.0));
	else
		sprintf(buf,"%d     B",size);
	return strlen(buf);
}

int fmg_expfiles(char * board)
{
	struct fmg_attdir ad;
	ad=fmg_openattdir(board,0);
	if(!(ad.fp))return 0;
	int cnt=0;
	int i,v;
	time_t now;
	struct fmg_FileRec frec;
	time(&now);
	fseek(ad.fp,sizeof(struct fmg_FileHead),0);
	for(i=0;;i++)
	{
		if(!(fread(&frec,sizeof(frec),1,ad.fp)))break;
		v=fmg_life[frec.flag&7];
		if(v<=0)continue;
		if(frec.timeupload+v*86400<now&&(*(frec.filename)))
		{
			#ifdef FMG_SHOWDETAIL
			printf("Delete file: %s %s",frec.filename,conmode?"\n":"<br>");
			#endif
			ad.fh.totalsize-=frec.filesize;
			ad.fh.itemsize[(frec.flag&7)]-=frec.filesize;
			unlink(frec.filename);
			memset(&(frec.filename),0,sizeof(frec.filename));
			fseek(ad.fp,-sizeof(frec),1);
			fwrite(&frec,sizeof(frec),1,ad.fp);
			if(ad.todelmin<0||ad.todelmin>i)ad.todelmin=i;
			ad.fh.reccount--;
		}
	}
	fmg_closeattdir(&ad);
	return cnt;
}

int fmg_sortrec__cmp(const void * p1,const void * p2)
{
	return (int)(((struct fmg_FileRec*)(p1))->timeupload-((struct fmg_FileRec*)(p2))->timeupload);
}

int fmg_sortrec(struct fmg_attdir * ad)
{
	char * p;
	int i;
	if(!(ad->fp)||ad->mode)return -2;
	fmg_saveattdir(ad);
	fseek(ad->fp,sizeof(struct fmg_FileHead),0);
	if(!(p=(char*)(malloc(ad->fh.reccount*sizeof(struct fmg_FileRec)))))return -1;
	fread(p,sizeof(struct fmg_FileRec),ad->fh.reccount,ad->fp);
	qsort(p,ad->fh.reccount,sizeof(struct fmg_FileRec),fmg_sortrec__cmp);
	fseek(ad->fp,sizeof(struct fmg_FileHead),0);
	for(i=0;i<ad->fh.reccount;i++)(((struct fmg_FileRec*)p)+i)->id=i;
	ad->fh.nextid=i;
	fwrite(p,sizeof(struct fmg_FileRec),ad->fh.reccount,ad->fp);
	free(p);
	return 1;
}