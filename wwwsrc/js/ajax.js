//javascript
var formok;
var sUserAgent=navigator.userAgent;
var isIE = sUserAgent.indexOf('compatible') > -1 && sUserAgent.indexOf('MSIE') > -1;
var ieVer = isIE?parseFloat(navigator.appVersion.substring(navigator.appVersion.indexOf("MSIE ")+5, navigator.appVersion.length)):0;
var isWin = (navigator.platform == 'Win32') || (navigator.platform == 'Windows');
var EventUtil = new Object;
EventUtil.addEventHandler = function (oTarget, sEventType, fnHandler) {
if (oTarget.addEventListener) { //for DOM-compliant browsers
oTarget.addEventListener(sEventType, fnHandler, false);
} else if (oTarget.attachEvent) { //for IE
oTarget.attachEvent('on' + sEventType, fnHandler);
} else { //for all others
oTarget['on' + sEventType] = fnHandler;
}
};
EventUtil.removeEventHandler = function (oTarget, sEventType, fnHandler) {
if (oTarget.removeEventListener) { //for DOM-compliant browsers
oTarget.removeEventListener(sEventType, fnHandler, false);
} else if (oTarget.detachEvent) { //for IE
oTarget.detachEvent('on' + sEventType, fnHandler);
} else { //for all others
oTarget['on' + sEventType] = null;
}
};
EventUtil.formatEvent = function (oEvent) {
return oEvent;
};
EventUtil.formatEvent = function (oEvent) {
if (isIE && isWin) {
oEvent.charCode = (oEvent.type == 'keypress') ? oEvent.keyCode : 0;
oEvent.eventPhase = 2;
oEvent.isChar = (oEvent.charCode > 0);
oEvent.pageX = oEvent.clientX + document.body.scrollLeft;
oEvent.pageY = oEvent.clientY + document.body.scrollTop;
oEvent.preventDefault = function () {
this.returnValue = false;
};
if (oEvent.type == 'mouseout') {
oEvent.relatedTarget = oEvent.toElement;
} else if (oEvent.type == 'mouseover') {
oEvent.relatedTarget = oEvent.fromElement;
}
oEvent.stopPropagation = function () {
this.cancelBubble = true;
};
oEvent.target = oEvent.srcElement;
oEvent.time = (new Date).getTime();
}
return oEvent;
};
EventUtil.getEvent = function() {
if (window.event) {
return this.formatEvent(window.event);
}
};
function getHttpObj(url,handler,varobj) {
	var http_request = false;
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
		http_request = new XMLHttpRequest();
		if (http_request.overrideMimeType) {
		http_request.overrideMimeType('text/xml');
		}
	} else if (window.ActiveXObject) { // IE
		try {
			http_request = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
				http_request = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {}
		}
	}
	if (!http_request) {
		alert('Giving up :( Cannot create an XMLHTTP instance');
		return false;
	}
	try
	{
		http_request.onreadystatechange = function() {
		if(http_request.readyState == 4) handler(http_request,url,varobj);
		}
	}
	catch (e)
	{
		http_request.onload = function() { handler(http_request,url,varobj); };
	}
	return http_request;
}
function loadXML(url,handler,varobj) {
	var http_request=getHttpObj(url,handler,varobj);
	if(!http_request)return false;
	http_request.open('GET', url, true);
	http_request.send(null);
	return true;
}
