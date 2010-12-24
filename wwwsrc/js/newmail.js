function newmail(cnt)
{
	try
	{
		var o=window.parent.frames['f3'].document;
	}
	catch(err)
	{
		if(confirm('您有%d封新邮件,立即查看吗?'))
			window.open('bbsnewmail');
		return;
	}
	var w=800,h=600;
	var d;
	d=o.getElementById('Div_newMail');
	if(d)return;
	d=o.createElement('div');
	var oInputOK=o.createElement('a');
	oInputOK.setAttribute('href','bbsmail');
	oInputOK.innerHTML='查看';
	var oInputCancel=o.createElement('a');
	oInputCancel.setAttribute('href','javascript:var o=document.getElementById(\'Div_newMail\');o.parentNode.removeChild(o);');
	oInputCancel.innerHTML='取消';
	var oTDInput=o.createElement('td');
	var oTDCancel=o.createElement('td');
	oTDInput.setAttribute('align','left');
	oTDInput.setAttribute('width','50%');
	oTDInput.appendChild(oInputOK);
	oTDCancel.setAttribute('align','right');
	oTDCancel.setAttribute('width','50%');
	oTDCancel.appendChild(oInputCancel);
	var oTR=o.createElement('tr');
	oTR.appendChild(oTDInput);
	oTR.appendChild(oTDCancel);
	var oTDTop=o.createElement('td');
	oTDTop.setAttribute('colspan','2');
	oTDTop.innerHTML='您有'+cnt+'封新信';
	var oTRTop=o.createElement('tr');
	oTRTop.appendChild(oTDTop);
	var oTable=o.createElement('table');
	oTable.setAttribute('width','100');
	oTable.setAttribute('height','60');
	oTable.setAttribute('border','0');
	oTable.appendChild(oTRTop);
	oTable.appendChild(oTR);
	d.appendChild(oTable);
	d.style.position='absolute';
	d.style.left=(w/2-50)+'px';
	d.style.top=(h/2-30)+'px';
	d.style.backgroundColor='#cceeff';
	var x=o.body.firstChild;
	if(x)
		o.body.insertBefore(d,x);
	else
		o.body.appendChild(d);
	//o.open();
	o.write(d.outerHTML);
	//o.close();
	return;
}
