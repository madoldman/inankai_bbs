#define UTFCONVTOURL

#include "BBSLIB.inc"
#include "NEWLIB.inc"

int urlconv(char * dest, char * src)
//utf8->gb2312
{
	char * ps, * pd;
	char buf[1024];
	int buflen = 0;
	char c, c1;
	memset(buf, 0, sizeof(buf));
	pd = dest;
	for (ps = src; ps < src + 1024; ps++)
	{
		c = *ps;
		if ((c < ' ') && (c >= 0))
		{
			pd += utfconv(pd, buf, &buflen);
			break;
		}
		else if (c == '%')
		{
			c1 = *(ps + 2);
			c = *(ps + 1);
			if (c >= 'a')
			{
				c -= 32;
			}
			if (c1 >= 'a')
			{
				c1 -= 32;
			}
			if (c >= 'A')
			{
				c -= 55;
			}
			else
			{
				c -= 48;
			}
			if (c1 >= 'A')
			{
				c1 -= 55;
			}
			else
			{
				c1 -= 48;
			}
			c = ((c << 4) | c1);
			*(buf + buflen) = c;
			buflen++;
			ps += 2;
		}
		else if (c == '/')
		{
			pd += utfconv(pd, buf, &buflen);
			*(pd++) = '/';
		}
		else if (c == '?')
		{
			strcpy(pd, ps);
			pd = dest + strlen(dest);
		}
		else
		{
			*(buf + buflen) = c;
			buflen++;
		}
	}
	*(pd++) = 0;
	return pd - dest;
}

int main()
{
	init_all();
	char url[1024], url_show[256];
	char urlc[2048], urlc_show[256];
	strncpy(url, getsenv("REQUEST_URI"), 1023);
	snprintf(url_show, 63, "http://%s%s", getsenv("HTTP_HOST"), url);
	if (strlen(url_show) > 60)
	{
		strcpy(url_show + 55, "...");
	}
	urlconv(urlc, url);
	snprintf(urlc_show, 63, "http://%s%s", getsenv("HTTP_HOST"), urlc);
	if (strlen(urlc_show) > 60)
	{
		strcpy(urlc_show + 55, "...");
	}
	printf("<table align=\"center\" width=\"600\" border=\"0\"><tr><td>"
"<h2 align=\"center\">HTTP Error 404 页面未找到</h2>"
"<p><span style=\"padding: 8px; float: right;\"><a href=\"/\"><img src=\"/NKLogo_Purple.png\" border=\"0\"></a></span>"
"&nbsp;&nbsp;&nbsp;&nbsp;您正在查找的文件<a href=\"%s\">%s</a>不存在。</p>"
"<p>&nbsp;&nbsp;&nbsp;&nbsp;可能的原因有：<ul><li>您输入的地址不正确；</li>"
"<li>您查找的文件已经过期；</li><li>您的浏览器URL编码与服务器的设定不同；</li>"
"<li>其他原因。</li></ul></p>"
"<h3 align=\"center\"></h3>"
"<ul><li>如果您的浏览器URL编码设置与服务器设定的不同，下面的链接<font color=\"red\"><b>可能</b></font>"
"是您真正需要的链接: <br /><a href=\"%s\">%s</a></li>"
"<li>如果您的板面需要更大的附件存储空间，请到对应区务管理板面申请；</li>"
"<li>如果您认为这是一个Bug，请到<a href=\"/cgi-bin/bbs/bbstdoc?board=BugReport\">BugReport板</a>报告。</li>"
"</ul>",
url, url_show, urlc, urlc_show);
	printf("<p></p></td></tr></table>");
	return 0;
}

