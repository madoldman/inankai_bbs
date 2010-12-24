//BBSLIB required

struct wishrec
{
	unsigned int seq;
	char id[16];
	time_t time;
	char cont[1024];
};

int CheckChannel(const char * ch)
{
	FILE * fp;
	char fn[256];
	char buf[256];
	int i;
	char *p;
	sprintf (fn, "%s/etc/wish.cfg", BBSHOME);
	fp = fopen (fn, "rt");
	if (!fp)
	{
		return 0;
	}
	i = 0;
	while (fgets(buf, sizeof(buf), fp))
	{
		p = strchr(buf, '\n');
		if (p)
		{
			*p = 0;
		}
		if (!strcmp(buf, ch))
		{
			i = 1;
			break;
		}
	}
	fclose(fp);
	return i;
}

int LastWishes(const char * ch, int count, struct wishrec * w)
{
	char fn[256];
	char buf[2048];
	FILE *fp;
	int filelen, rr;
	sprintf (fn, "%s/etc/wish/%s", BBSHOME, ch);
	fp = fopen(fn, "rb");
	if (!fp)
	{
		return 0;
	}
	fseek (fp, 0, 2);
	filelen = ftell(fp);
	if (filelen > count * sizeof(struct wishrec))
	{
		fseek (fp, - count * sizeof(struct wishrec), 2);
		rr = count;
	}
	else
	{
		rr = filelen / sizeof(struct wishrec);
		fseek (fp, 0, 0);
	}
	fread (w, sizeof(*w), rr, fp);
	fclose(fp);
	return rr;
}

int SearchWish(const char * ch, const char * id, int count, struct wishrec * w)
{
	return 0;
}

int AppendWish(const char * ch, const char * cont)
{
	char fn[256];
	FILE * fp;
	time_t checkt;
	int checkc, i;
	struct wishrec w;
	if (!loginok)
	{
		return -2;
	}
	checkt = time(0) - 7200;
	checkc = 2;
	sprintf (fn, "%s/etc/wish/%s", BBSHOME, ch);
	fp = fopen(fn, "rb+");
	if (!fp)
	{
		fp = fopen(fn, "w+");
	}
	else
	{
		i = 0;
		while (fread(&w, sizeof(w), 1, fp))
		{
			if (w.time < checkt || strcmp(currentuser.userid, w.id))
			{
				continue;
			}
			i++;
		}
		if (i >= checkc)
		{
			return -1;
		}
	}
	strncpy(w.id, currentuser.userid, sizeof(w.id));
	strncpy(w.cont, cont, sizeof(w.cont));
	w.time = time(0);
	w.seq = ftell(fp) / sizeof(w) + 1;
	fwrite(&w, sizeof(w), 1, fp);
	fclose(fp);
	return 0;
}


