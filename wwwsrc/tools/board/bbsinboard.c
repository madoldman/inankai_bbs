/*$Id: bbsinboard.c,v 1.2 2008-12-19 09:55:09 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "boardrc.inc"

int main()
{
        struct shortfile data[MAXBOARD], *x;
        int i, total=0;
        int all=0;//����Ʒζ���µ��û���
        init_all();
        getdatestring(time(0), NA);
        printf("<body background=/bg_1.gif bgproperties=fixed>");
        printf("<center>%s -- ��ǰ���������������� (%s)\n<hr>\n", BBSNAME, datestring);
        for(i=0; i<MAXBOARD; i++) {
                x=&(shm_bcache->bcache[i]);
                if(x->filename[0]<=32 || x->filename[0]>'z')
                        continue;
                if(!has_read_perm(&currentuser, x->filename))
                        continue;
                memcpy(&data[total], x, sizeof(struct shortfile));
                total++;
                all+=*((int*)(x->filename+72));
        }
        qsort(data, total, sizeof(struct shortfile), cmpboard);
        printf("<table><tr><td>����<td>��������<td>��������<td>��������<td>�ٷֱ�");
        for(i=0; i<total; i++) {
                printf("<tr><td>%3d", i+1);
                printf("<td>%s", data[i].filename);
                printf("<td>%s", data[i].title+8);
                printf("<td align=right>%d", *((int*)(data[i].filename+72)));
                printf("<td align=right>%5.2f%%", (*((int*)(data[i].filename+72)))*100.0/all);
        }

        return 1;
}
