


function MM_findObj(n, d) { //v4.0
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && document.getElementById) x=document.getElementById(n); return x;
}
 
 function MM_changeProp(objName,x,theProp,p1,p2) { //v3.0
  var obj = MM_findObj(objName);
   var theValue=null;
  theValue=eval("obj."+theProp);
   
//   alert("obj."+theProp);
  // alert(theValue);

  theValue==p1?theValue=p2:theValue=p1;
  if (obj && (theProp.indexOf("style.")==-1 || obj.style)) eval("obj."+theProp+"='"+theValue+"'");
//alert("obj."+theProp);
//alert(theValue);
}

function  menu(o1,o2)
{
MM_changeProp(o1,'','style.display','block','none','div') ;
MM_changeProp(o2,'','src' ,'http://202.113.16.117/image/folderopen.gif','http://202.113.16.117/image/folderclose.gif','img') ;
}

function  menui(o1,o2)
{
MM_changeProp(o1,'','style.display','block','none','div') ;
}


function openreg()
{
        open('/cgi-bin/bbs/bbsreg', '', 'width=640,height=520,resizable=yes,scrollbars=yes');
}

function openchat()
{
	url='bbschat';
	chatWidth=screen.availWidth;
	chatHeight=screen.availHeight;
	winPara='toolbar=no,location=no,status=no,menubar=no,scrollbars=auto,resizable=yes,left=0,top=0,'+'width='+(chatWidth-10)+',height='+(chatHeight-27);
	window.open(url,'_blank',winPara);
}



function chgstat(){

top.lf.cols=="130,*"?top.lf.cols="0,*":top.lf.cols="130,*";
}

function refreshall(){
window.parent.f2.location="bbsleft";
window.parent.f4.location="/js/bbsfoot.htm";
//window.parent.f2tty.location="bbstty0";
}
