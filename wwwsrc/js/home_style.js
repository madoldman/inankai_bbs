function setCookie(sName, sValue, oExpires, sPath, sDomain, bSecure) {
	var sCookie = sName + '=' + encodeURIComponent(sValue);
	if (oExpires) {
		sCookie += '; expires=' + oExpires.toGMTString();
	}
	if (sPath) {
		sCookie += '; path=' + sPath;
	}
	if (sDomain) {
		sCookie += '; domain=' + sDomain;
	}
	if (bSecure) {
		sCookie += '; secure';
	}
	document.cookie = sCookie;
}

function getCookie(sName) {
	var sRE = '(?:; )?' + sName + '=([^;]*);?';
	var oRE = new RegExp(sRE);
	if (oRE.test(document.cookie)) {
		return decodeURIComponent(RegExp['$1']);
	} else {
		return null;
	}
}

function init()
{
	var x,s;
	x=getCookie('home_style');
	if(!x)
		x='violet_spirit';
	if(x.length<2)
		x='violet_spirit';
	s='/css/'+x+'/bbsguide4.css';
	document.getElementById('csssheet').href=s;
}
