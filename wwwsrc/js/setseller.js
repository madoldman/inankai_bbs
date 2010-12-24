//javascript
function addseller()
{
	var o=document.getElementById('tblmain');
	if(!o)return;
	var o2=document.getElementById('itemcount');
	if(!o2)return;
	var c=parseInt(o2.value)+1;
	var oAtext=document.createTextNode('É¾³ý');
	var oA=document.createElement('a');
	oA.setAttribute('href','javascript:delseller('+c+');');
	oA.appendChild(oAtext);
	var oINPUTid=document.createElement('input');
	var oINPUTtel=document.createElement('input');
	var oINPUTtel2=document.createElement('input');
	var oINPUTaddr=document.createElement('input');
	var oINPUTname=document.createElement('input');
	var oINPUTipreg=document.createElement('input');
	oINPUTid.setAttribute('type','text');
	oINPUTtel.setAttribute('type','text');
	oINPUTtel2.setAttribute('type','text');
	oINPUTaddr.setAttribute('type','text');
	oINPUTname.setAttribute('type','text');
	oINPUTipreg.setAttribute('type','text');
	oINPUTid.setAttribute('name','id_'+c);
	oINPUTtel.setAttribute('name','tel_'+c);
	oINPUTtel2.setAttribute('name','tel2_'+c);
	oINPUTaddr.setAttribute('name','addr_'+c);
	oINPUTname.setAttribute('name','name_'+c);
	oINPUTipreg.setAttribute('name','ipreg_'+c);
	var oTDid=document.createElement('td');
	var oTDtel=document.createElement('td');
	var oTDtel2=document.createElement('td');
	var oTDaddr=document.createElement('td');
	var oTDname=document.createElement('td');
	var oTDlink=document.createElement('td');
	var oTDipreg=document.createElement('td');
	oTDid.appendChild(oINPUTid);
	oTDtel.appendChild(oINPUTtel);
	oTDtel2.appendChild(oINPUTtel2);
	oTDaddr.appendChild(oINPUTaddr);
	oTDname.appendChild(oINPUTname);
	oTDlink.appendChild(oA);
	oTDipreg.appendChild(oINPUTipreg);
	var oTR=document.createElement('tr');
	oTR.appendChild(oTDid);
	oTR.appendChild(oTDtel);
	oTR.appendChild(oTDtel2);
	oTR.appendChild(oTDaddr);
	oTR.appendChild(oTDname);
	oTR.appendChild(oTDipreg);
	oTR.appendChild(oTDlink);
	o.appendChild(oTR);
	o2.value=c;
	return;
}
function delseller(id2d)
{
	if(id2d<=0)return;
	var o=document.getElementById('tblmain');
	var x=o.childNodes[id2d];
	if(!x)return;
	while(x.firstChild)(x.removeChild(x.childNodes[0]));
	x.style.display='none';
	return;
}
