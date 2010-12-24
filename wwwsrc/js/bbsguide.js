// hide all div
var timerIdDivHide;

// the height of the div to show
var tableHeight = "0";

// show the div
var timerIdTabShow;
var objShowDiv = null;
var numShowDivHeight = 0;
var objShowTable = null;
// hide the div
var timerIdTabHide;
var objHideDiv = null;
var numHideDivHeight = 0;

function init(){
	var aryTemp = new Array();
	var numI = 0;
	clearTimeout(timerIdTabHide);
	clearTimeout(timerIdTabShow);
	clearTimeout(timerIdTabHide);

	aryTemp = document.getElementsByName("tabitem");
	for(numI=0;numI<aryTemp.length;numI++){
		aryTemp[numI].onmouseover = funcMouseover;
	}
	document.getElementById("tabs1previewdiv").onmouseover = function(){clearTimeout(timerIdDivHide);};
	document.getElementById("tabs2previewdiv").onmouseover = function(){clearTimeout(timerIdDivHide);};
	document.getElementById("tabs1previewdiv").onmouseout = function(){timerIdDivHide = setTimeout('funcHideAll()',3000);};
	document.getElementById("tabs2previewdiv").onmouseout = function(){timerIdDivHide = setTimeout('funcHideAll()',3000);};
	aryTemp = document.getElementsByTagName("ul");
	for(numI=0;numI<aryTemp.length;numI++){
		if(aryTemp[numI].className.indexOf("patabs")>-1){
			aryTemp[numI].onmouseover = function(){clearTimeout(timerIdDivHide);};
			aryTemp[numI].onmouseout = function(){timerIdDivHide = setTimeout('funcHideAll()',3000);};
		}
	}
	aryTemp = document.getElementsByTagName("span");
	for(numI=0;numI<aryTemp.length;numI++){
		if(aryTemp[numI].className.indexOf("icon")>-1){
			aryTemp[numI].onmouseover = function(){clearTimeout(timerIdDivHide);};
		}
	}
}

function funcHideAll(){
	var aryDiv = new Array();
	aryDiv[0] = document.getElementById("tabs1previewdiv");
	aryDiv[1] = document.getElementById("tabs2previewdiv");
	var blnEnd = true;
	for(var numI=0;numI<2;numI++){
		if(aryDiv[numI].style.display != "none"){
			var numHideDivHeight = aryDiv[numI].offsetHeight;
			if(numHideDivHeight<=20){
				aryDiv[numI].style.height = "1px";
				aryDiv[numI].style.display = "none";
				if(document.getElementById("tabs1previewdiv").style.display=="none"
						&&document.getElementById("tabs2previewdiv").style.display=="none"){
					var aryTemp = document.getElementsByName("tabitem");
					for(var numI=0;numI<aryTemp.length;numI++){
						aryTemp[numI].style.backgroundColor = "white";
					}
				}
			}else{
				aryDiv[numI].style.height = (numHideDivHeight - 20)+"px";
				blnEnd = false;
			}
		}
	}
	blnEnd ? 1 : setTimeout("funcHideAll()",50);
}
function funcMouseover(){
	clearTimeout(timerIdDivHide);
	var aryTemp;
	var objTarget = this;
	clearTimeout(timerIdTabHide);
	clearTimeout(timerIdTabShow);
	var divIdShow = "";
	var divIdHide = "";
	var divToShow = null;
	var divToHide = null;
	if(objTarget.parentNode.parentNode.parentNode.parentNode.getAttribute("id")=="tabs1"){
		divToShow = document.getElementById("tabs1previewdiv");
		divToHide = document.getElementById("tabs2previewdiv");
		divIdShow = "tabs1previewdiv";
		divIdHide = "tabs2previewdiv";
	}else{
		divToShow = document.getElementById("tabs2previewdiv");
		divToHide = document.getElementById("tabs1previewdiv");
		divIdShow = "tabs2previewdiv";
		divIdHide = "tabs1previewdiv";
	}
	var tableId = objTarget.getAttribute("id").replace("pa","table_");
	var objTable = document.getElementById(tableId);
	if(objTable.style.display!="inline"&&objShowTable){
		objShowTable.style.display = "none";
		objShowTable.style.marginTop = "-9999px";
	}
	objShowTable = objTable;
	aryTemp = document.getElementsByName("tabitem");
	for(var numI=0;numI<aryTemp.length;numI++){
		aryTemp[numI].style.backgroundColor = "white";
	}
	objTarget.style.backgroundColor = "dodgerblue";
	if(divToShow.style.display!="block"){
		divToShow.style.display = "block";
		divToShow.style.height = "1px";
	}
	tableHeight = getHeight(objShowTable);
	tableHeight = (tableHeight < 70) ? 70 : tableHeight;
	objShowTable.style.marginTop="0px";
	objShowTable.style.display="inline";
	objShowDiv = document.getElementById(divIdShow);
	objHideDiv = document.getElementById(divIdHide);
	funcShowTab();
	funcHideTab();
}

function funcShowTab(){
	var numShowDivHeight = objShowDiv.offsetHeight;
	var numTemp = (tableHeight-numShowDivHeight);
	if(numTemp>-20&&numTemp<20){
		objShowDiv.style.height = tableHeight;
	}else{
		objShowDiv.style.height = (numShowDivHeight+((tableHeight>numShowDivHeight)?20:-20))+"px";
		timerIdTabShow = setTimeout("funcShowTab()",50);
	}
}
function funcHideTab(){
	var numHideDivHeight = objHideDiv.offsetHeight;
	if(numHideDivHeight<=20){
		objHideDiv.style.height = "1px";
		objHideDiv.style.display = "none";
		if(document.getElementById("tabs1previewdiv").style.display=="none"
				&&document.getElementById("tabs2previewdiv").style.display=="none"){
			var aryTemp = document.getElementsByName("tabitem");
			for(var numI=0;numI<aryTemp.length;numI++){
				aryTemp[numI].style.backgroundColor = "white";
			}
		}
	}else{
		objHideDiv.style.height = (numHideDivHeight - 20)+"px";
		timerIdTabHide = setTimeout("funcHideTab()",50);
	}
}
function getHeight(elem) {
	// save the old css style
	var oldcss = new Object();
	oldcss.position = elem.style.position;
	oldcss.visibility = elem.style.visibility;
	oldcss.display = elem.style.display;
	oldcss.top = elem.style.top;
	oldcss.left =elem.style.left;
	oldcss.marginTop=elem.style.marginTop;
	// show the elem	
	elem.style.position = "absolute";
	elem.style.display = "block";
	elem.style.visibility = "visible";
	elem.style.top = "0px";
	elem.style.left ="0px";
	elem.style.marginTop="0px";
	// get the height
	var val = elem.offsetHeight;
	//var padding = 0, border = 0;
	//padding += objElem.style.padding || 0;
	//border += objElem.style.borderWidth || 0;
	//val -= Math.round(padding + border);
	val = (0<val)?val:0;
	// reset the css style
	elem.style.position = oldcss.position;
	elem.style.visibility = oldcss.visibility;
	elem.style.display = oldcss.display;
	elem.style.top = oldcss.top;
	elem.style.left =oldcss.left;
	elem.style.marginTop=oldcss.marginTop;
	return val;
}
function scrollDiv(strDivName,mt,height){
	if(mt+height>0){
		mt = mt-1;
		document.getElementById(strDivName).style.marginTop = mt+"px";
	}else{
		mt = 0;
		document.getElementById(strDivName).style.marginTop = "0px";
	}
	setTimeout("scrollDiv('"+strDivName+"',"+mt+","+height+")",120);
}
