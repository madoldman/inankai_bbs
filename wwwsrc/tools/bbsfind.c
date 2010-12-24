/*$Id: bbsfind.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"
char day[20], user[20], title[80];

int main()
{
        char user[32], title3[80], title[80], title2[80];
        int day;
        init_all();
        printf("<body background=%s bgproperties=fixed>",mytheme.bgpath);
        modify_mode(u_info,READING+20000);	//bluetent
        strsncpy(user, getparm("user"), 13);
        strsncpy(title, getparm("title"), 50);
        strsncpy(title2, getparm("title2"), 50);
        strsncpy(title3, getparm("title3"), 50);



        day=atoi(getparm("day"));
        if(day==0||(!strlen(user)&&!(strlen(title)))) {//huangxu@060406:禁止空查询
                //		printf("%s -- 站内文章查询<hr color=green>\n", BBSNAME);
                printf("<center><img src=/search_b.gif></center><hr color=green>");
                printf("<form name=bbsfind action=bbsfind>\n");
                printf("文章作者: <input maxlength=12 size=12 type=text name=user> (不填查找所有作者)<br>\n");
                printf("标题含有: <input maxlength=60 size=20 type=text name=title>");
                printf(" AND <input maxlength=60 size=20 type=text name=title2><br>\n");
                printf("标题不含: <input maxlength=60 size=20 type=text name=title3><br>\n");
                printf("查找最近: <input maxlength=5 size=5 type=text name=day value=7> 天以内的文章<br><br>\n");
				        if (HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_DENYPOST)&&HAS_PERM(PERM_OBOARDS))//huangxu@060406
				        {
				        	printf("<p><input type='radio' checked name='adminmode' value='0' />一般查找</p>");
        					printf("<p><input type='radio' name='adminmode' value='1' />查找文章以便删除</p>");
        					printf("<p><input type='radio' name='adminmode' value='2' />查找回收站</p>");
        				}
                printf("<input type=submit value=提交查询></form><script language=javascript>bbsfind.user.focus()</script>\n");
        } else {
                search(user, title, title2, title3, day*86400);
        }
        printf("</body>");
        http_quit();
}

//huangxu@060405:批量删除文章方便
int search(char *id, char *pat, char *pat2, char *pat3, int dt)
{
        FILE *fp;
        char flgadmin=0;//huangxu@060406:管理模式，允许批量删除文章
        int j;//长度
        char board[256], dir[256], buf2[150];
        int total, now=time(0), i, sum=0, n, t;
        struct fileheader x;
        printf("<script language=\"javascript\" src=\"/js/ajax.js\"></script>");
        printf("\
<script language=\"javascript\">\
var BBSURL = '/cgi-bin/bbs/';\
\
function sumFiles()\
{\
	var intCnt = parseInt(document.getElementById('spnCnt').innerHTML);\
	if (isNaN(intCnt))\
	{\
		intCnt = 0;\
	}\
	var i, intFiles, oWin, oDoc;\
	var strLink, strUser;\
	var o;\
	oWin = open('');\
	if (!oWin)\
	{\
		alert('由于弹出窗口被拦截,此功能无法实现.');\
		return false;\
	}\
	oDoc = oWin.document;\
	oDoc.write('<h3 align=\"center\">' + document.getElementById('pTitle').innerHTML + '</h3>');\
	for(i = intFiles = 0; i < intCnt; i++)\
	{\
		o = document.getElementById('div_' + i);\
		if (!o)\
		{\
			continue;\
		}\
		oDoc.write('<table border=\"1\" align=\"center\" width=\"610\">');\
		try\
		{\
			strLink = document.getElementById('aSrc_' + i).getAttribute('href');\
			if (strLink.substring(0,4).toLowerCase() != 'http')\
			{\
				strLink = BBSURL + strLink;\
			}\
		}\
		catch(e)\
		{\
			strLink = '';\
		}\
		oDoc.write('<tr><td>#<span color=\"red\">' + (++intFiles) + '</span> 原文：<a href=\"' + strLink + '\">' + strLink + '</a></td></tr>');\
		oDoc.write('<tr><td>' + o.innerHTML + '</td></tr>');\
		oDoc.write('</table>');\
	}\
	oDoc.write('<p align=\"center\">共找到文章' + intCnt + '篇，本报表包含了其中的' + intFiles + '篇。</p>');\
	oDoc.close();\
}\
var arrCont=new Array();\
function insertAfter(newNode,refNode)\
{\
	var n=false;\
	n=refNode.nextSibling;\
	if(n)\
	{\
		refNode.parentNode.insertBefore(newNode,n);\
	}\
	else\
	{\
		refNode.parentNode.appendChild(newNode);\
	}\
	return newNode;\
}\
function modifcont(id,info)\
{\
	var o=false;\
	o=document.getElementById('div_'+id);\
	if(!o)\
		return 0;\
	o.innerHTML=info;\
	return 1;\
}\
function showcont(objreq,url,id)\
{\
	arrCont[id]=objreq.responseText;\
	modifcont(id,objreq.responseText);\
}\
function getcont(id)\
{\
	var o=document.getElementById('trdiv_'+id);\
	if(o)\
	{\
		o.parentNode.removeChild(o);\
		o=document.getElementById('a_'+id);\
		if(o)\
		{\
			o.innerHTML='预览';\
		}\
	}\
	else\
	{\
		o=document.getElementById('a_'+id);\
		if(o)\
		{\
			o.innerHTML='隐藏';\
		}\
		o=false;\
		o=document.getElementById('tr_'+id);\
		if(!o)\
			return 0;\
		var oDiv=document.createElement('div');\
		oDiv.setAttribute('id','div_'+id);\
		oDiv.innerHTML='Loading...';\
		var oTD=document.createElement('td');\
		oTD.setAttribute('id','tddiv_'+id);\
		oTD.colSpan='5';\
		oTD.appendChild(oDiv);\
		var oTR=document.createElement('tr');\
		oTR.setAttribute('id','trdiv_'+id);\
		oTR.appendChild(oTD);\
		insertAfter(oTR,o);\
		if(arrCont[id])\
		{\
			modifcont(id,arrCont[id]);\
		}\
		else\
		{\
			o=document.getElementById('alink_'+id);\
			var link=o.getAttribute('href');\
			loadXML(link,showcont,id);\
		}\
	}\
}\
</script>\
");
        if (HAS_PERM(PERM_SYSOP)||HAS_PERM(PERM_DENYPOST)&&HAS_PERM(PERM_OBOARDS))
        	flgadmin=1;//huangxu@060406:OD权限并存则管理权限有效
        if (flgadmin) flgadmin=atoi(getparm("adminmode"));
        printf("%s -- 站内文章查询结果 <br>\n", BBSNAME);
        if (flgadmin==1)
        	printf("<form action='delfiles' method='post'>\n");	
        printf("<p id=\"pTitle\">");
        if (flgadmin==2)
        	printf("回收站内查询结果 ");
        printf("作者: %s ", id);
        printf("标题含有: '%s' ", nohtml(pat));
        if(pat2[0])
                printf("和 '%s' ", nohtml(pat2));
        if(pat3[0])
                printf("不含 '%s'", nohtml(pat3));
        printf("时间: %d 天</p><hr color=green>\n", dt/86400);
        j=0;
        for(i=0; i<MAXBOARD; i++) {
                total=0;
                strcpy(board, shm_bcache->bcache[i].filename);
                if(!has_read_perm(&currentuser, board))
                        continue;
                if (flgadmin==2)
                	sprintf(dir,"boards/%s/.junk",board);
                else
                	sprintf(dir, "boards/%s/.DIR", board);
                fp=fopen(dir, "r");
                if(fp==0)
                        continue;
                n=0;
                printf("<table width=610 border=1>\n");
                while(1) {
                        n++;
                        if(fread(&x, sizeof(x), 1, fp)<=0)
                                break;
                        t=atoi(x.filename+2);
                        if(id[0]!=0 && strcasecmp(x.owner, id))
                                continue;
                        if(pat[0] && !strcasestr(x.title, pat))
                                continue;
                        if(abs(now-t)>dt)
                                continue;
                        if(pat2[0] && !strcasestr(x.title, pat2))
                                continue;
                        if(pat3[0] && strcasestr(x.title, pat3))
                                continue;
                        printf("<tr id=\"tr_%d\"><td>",j);
                        if (flgadmin==1)
                        	printf("<input type='checkbox' value='%s板%s' name='chk%d' onClick=\"if(event.shiftKey){var min=%d;var max=form.lastsel.value;if(min>max){var tmp=min;min=max;max=tmp;}var stt=form.chk%d.checked;for(var i=min;i<=max;i++)form['chk'+i].checked=stt;}form.lastsel.value=%d;\" />",board,x.filename,j,j,j,j);
                        printf("%d</td><td><a id=\"aUsr_%d\" href=bbsqry?userid=%s>%s</a></td>",
                               n, j, x.owner, x.owner);
                        printf("<td>%6.6s</td>", Ctime(atoi(x.filename+2))+4);
                        printf("<td><a id=\"aSrc_%d\" href=newcon?board=%s&file=%s&num=%d%s>%s</a></td>\n",//huangxu@060406:修改bbscon为newcon，增加回收站模式
                               j, board, x.filename, n-1, (flgadmin==2)?"&junk=yes":"" , nohtml(x.title));
                        printf("<td><a id=\"a_%d\" href=\"javascript:getcont(%d);\">预览</a>"
                        "<a id=\"alink_%d\" href=\"jbbscon?board=%s&file=%s%s&htmlmode=3\"></a></td></tr>",j,j,j,
                        board,x.filename,(flgadmin==2)?"&boardmode=1":"");
                        total++;
                        sum++;
                        j++;//huangxu@060406:按Shift区段选择，爽吧？
                        if(sum>1999) {
                                printf("</table> ....");
                                http_quit();
                        }
                }
                printf("</table>\n");
                if(total==0)
                        continue;
                printf("<br>以上%d篇来自 <a href=bbsdoc?board=%s>%s</a><br><br>\n",
                       total, board, board);
        }
        printf("一共找到<span id=\"spnCnt\">%d</span>篇文章符合查找条件，使用“预览”功能展开您需要的文章，然后点这里<input type=\"button\" value=\"生成报表\" onclick=\"sumFiles();\" /><br>\n", sum);
        if (flgadmin==1)
        {
        	printf("<input type=hidden name=total id=total value=%d />",j);
        	printf("<input type=hidden name=lastsel id=lastsel value=0 />");
        	printf("<p>\n\r<input type=\"checkbox\" name=\"chkall\" onClick=\"for (var i=0;i<form.elements.length;i++){var e = form.elements[i];if (e.name != 'chkall')e.checked = form.chkall.checked;}\" />全选&nbsp;\n\r");
        	printf("<input type=button value='删除选中的文章' onClick=\"{if(confirm('您确定执行的操作吗?')){disabled=true;submit();return true;}return false;}\" /></p>");
        	printf("</form>");
      	}
      	
}
