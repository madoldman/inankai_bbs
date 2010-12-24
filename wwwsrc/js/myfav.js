//Javascript
function getresult()
{
	var os=document.getElementById('DragChecked').getElementsByTagName('div');
	var i,s='';
	var t;
	for(i=0;i<os.length;i++)
	{
		t=os[i].innerHTML;
		t=t.substring(0,t.indexOf('/'));
		s+=t+'/';
	}
	return s;
}
function divshow(obj)
{
	var o=document.getElementById(obj.id+'_c');
	if(o.style.display=='none')
		o.style.display='';
	else
		o.style.display='none';
}
function rollBack()
{
	if(rootSibling){
		rootParent.insertBefore(curTarget, rootSibling);
	} else {
		rootParent.appendChild(curTarget);
	}
	curTarget.style.display = '';
}
function mouseUp()
{
	if(curTarget){
		dragHelper.style.display = 'none';
		if(curTarget.style.display != 'none'){
			if(rootParent.id=='DragChecked'&&curTarget.parentNode.id!='DragChecked')
			{
				document.getElementById('brd.'+curTarget.id).style.display='';
				curTarget.parentNode.removeChild(curTarget);
			}
			else if(rootParent.id!='DragChecked'&&curTarget.parentNode.id=='DragChecked')
			{
				var o=curTarget.cloneNode(true);
				o.id=o.id.substring(4);
				o.style.backgroundColor='#ffeecc';
				curTarget.parentNode.insertBefore(o,curTarget);
				rollBack();
				curTarget.style.display='none';
			}
			else if(rootParent.id=='DragChecked'&&curTarget.parentNode.id=='DragChecked')
			{
				
			}
			else
				rollBack();
		}
		else
		{
			if(rootParent.id=='DragChecked')
			{
				document.getElementById('brd.'+curTarget.id).style.display='';
				curTarget.parentNode.removeChild(curTarget);
			}
			else
				rollBack();
		}
	}
	curTarget = null;
	iMouseDown = false;
}

function autoOrder()
{
	if(!confirm('确定按英文名排序？'))return;
	var o=document.getElementById('DragChecked');
	var i,j,t;
	var d=document.createElement('div');
	for(i=o.firstChild;i;d.appendChild(i),i=o.firstChild);
	for(i=d.firstChild;i;i=d.firstChild)
	{
		for(j=o.firstChild;j;j=j.nextSibling)
		{
			if(i.innerHTML.toLowerCase()<j.innerHTML.toLowerCase())
			{
				o.insertBefore(i,j);
				break;
			}
		}
		if(!j)
			o.appendChild(i);
	}
	d.parentNode.removeChild(d);
}
