int
friend_search(uid, uentp, tblsize)
unsigned short uid;
struct user_info *uentp;
int     tblsize;
{
	int     hi, low, mid;
	int     cmp;
	if (uid == 0) {
		return NA;
	}
	hi = tblsize - 1;
	low = 0;
	
        while (low <= hi) {
		mid = (low + hi) / 2;
		cmp = uentp->friend[mid] - uid;
		if (cmp == 0) {
                      
			return YEA;
		}
		if (cmp > 0)
			hi = mid - 1;
		else
			low = mid + 1;
	}
	return NA;
}
int
hisfriend(uentp)
struct user_info *uentp;
{
        return friend_search(u_info->uid, uentp, uentp->fnum);
}

