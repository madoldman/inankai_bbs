//JavaScript
		var myInter;
		var bigheight;
		bigheight=document.getElementById("topright").offsetHeight;
		if (document.getElementById("topleft").offsetHeight>bigheight)		bigheight=document.getElementById("topleft").offsetHeight;
		document.getElementById("topright").style.height = bigheight;
		document.getElementById("topleft").style.height = bigheight;
		document.getElementById("spacer").style.height = bigheight;
		
		function StartRV() {
			var rollspeed=60;
			var Rname="RV";
			var wyoffset=0;
			var ooRollV=document.getElementById("RV");
			var ooRollV1=document.getElementById("RV1");
			var ooRollV2=document.getElementById("RV2");
			
			if (!ooRollV1.offsetTop)wyoffset=0;
			else wyoffset=ooRollV1.offsetTop;    //For firefox
			var Marq = function()
			{
			
			//alert(ooRollV1.offsetTop);
			//2:202
			if(ooRollV.scrollTop>=(ooRollV2.offsetTop-wyoffset)) {
				ooRollV.scrollTop-=ooRollV1.offsetHeight;
			}else{
				ooRollV.scrollTop++;
			}
			};
			
			if (ooRollV) {
				if (parseInt(ooRollV.style.height)>=ooRollV2.offsetTop) {
					ooRollV.style.height = ooRollV2.offsetTop;
					return;
				}
				ooRollV2.innerHTML=ooRollV1.innerHTML;
				
				myInter=setInterval(Marq,rollspeed);
				
				ooRollV.onmouseover=function() {clearInterval(myInter);};
				ooRollV.onmouseout=function() {myInter=setInterval(Marq,rollspeed);};
			}
		}

StartRV();