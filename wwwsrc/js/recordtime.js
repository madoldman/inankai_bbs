function record()
{
	getHttpObj('/cgi-bin/bbs/bbssaveusertime',function(){},{});
	/*
	var http_request = null;
	if (window.XMLHttpRequest) { 
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
	http_request.open('GET', '/cgi-bin/bbs/bbssaveusertime',true);
	http_request.send(null);
	*/
}
EventUtil.addEventHandler( window, "beforeunload", record);
//window.addEventListener("beforeunload",record,false);
