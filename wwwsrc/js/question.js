function tosubmit()
{
	var i,j,s,l,o,t;
	s=new Array();
	t=parseInt(document.getElementById('spncnt').firstChild.data);
	o=document.getElementsByTagName('input');
	l=o.length;
	for(i=0;i<l;i++)
	{
		if(o[i].getAttribute('type')!='radio')continue;
		j=o[i].getAttribute('id');
		if(j.substring(0,4)!='ans_')continue;
		j=parseInt(j.substring(4));
		if(o[i].checked)
		{
			s[j]=o[i].getAttribute('value');
		}
	}
	j='';
	l=0;
	for(i=0;i<t;i++)
	{
		if(s[i]&&s[i].length>0)
		{
			j+=s[i].substring(0,1);
		}
		else
		{
			j+=' ';
			l++;
		}
	}
	if(!confirm('��������'+t+'�������е�'+(t-l)+'����ȷ���ύô��'))return;
	document.getElementById('answer').value=j;
	document.forms['frmans'].submit();
}
