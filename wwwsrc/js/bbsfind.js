var BBSURL = 'http://nkbbs.org/cgi-bin/bbs/';

function sumFiles()
{
	var intCnt = parseInt(document.getElementById('spnCnt').innerHTML);
	if (isNaN(intCnt))
	{
		intCnt = 0;
	}
	var i, intFiles, oWin, oDoc;
	var strLink, strUser;
	var o;
	oWin = open('');
	if (!oWin)
	{
		alert('���ڵ������ڱ�����,�˹����޷�ʵ��.');
		return false;
	}
	oDoc = oWin.document;
	oDoc.write('<h3 align="center">' + document.getElementById('pTitle').innerHTML + '</h3>');
	for(i = intFiles = 0; i < intCnt; i++)
	{
		o = document.getElementById('div_' + i);
		if (!o)
		{
			continue;
		}
		oDoc.write('<table border="1" align="center">');
		try
		{
			strLink = document.getElementById('aSrc_' + i).getAttribute('href');
			if (strLink.substring(0,4).toLowerCase() != 'http')
			{
				strLink = BBSURL + strLink;
			}
		}
		catch(e)
		{
			strLink = '';
		}
		oDoc.write('<tr><td>#<span color="red">' + (++intFiles) + '</span> ԭ�ģ�<a href="' + strLink + '">' + strLink + '</a></td></tr>');
		oDoc.write('<tr><td>' + o.innerHTML + '</td></tr>');
		oDoc.write('</table>');
	}
	oDoc.write('<p align="center">���ҵ�����' + intCnt + 'ƪ����������������е�' + intFiles + 'ƪ��</p>');
	oDoc.close();
}
