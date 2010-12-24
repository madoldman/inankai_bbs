// JavaScript Document
var dropTargets = [];
var mouseOffset = null;
var iMouseDown = false;
var lMouseState = false;
var dragObject = null;
var DragDrops = [];
var curTarget = null;
var lastTarget = null;
var dragHelper = null;
var tempDiv = null;
var rootParent = null;
var rootSibling = null;

Number.prototype.NaN0=function(){return isNaN(this)?0:this;}

function addDropTarget(dropTarget){
	dropTargets.push(dropTarget);
}
function CreateDragContainer(){
	var cDrag = DragDrops.length;
	DragDrops[cDrag] = [];
	for(var i=0; i<arguments.length; i++){
		var cObj = arguments[i];
		DragDrops[cDrag].push(cObj);
		cObj.setAttribute('DropObj', cDrag);
		for(var j=0; j<cObj.childNodes.length; j++){
			if(cObj.childNodes[j].nodeName=='#text') continue;
			cObj.childNodes[j].setAttribute('DragObj', cDrag);
		}
	}
}
function getMouseOffset(target, ev){
	ev = ev || window.event;

	var docPos = getPosition(target);
	var mousePos = mouseCoords(ev);
	return {x:mousePos.x - docPos.x, y:mousePos.y - docPos.y};
}
function getPosition(e){
	var left = 0;
	var top = 0;

	while (e.offsetParent){
		left += e.offsetLeft;
		top += e.offsetTop;
		e = e.offsetParent;
	}

	left += e.offsetLeft;
	top += e.offsetTop;

	return {x:left, y:top};
}
function makeDraggable(item){
	if(!item) return;
	item.onmousedown = function(ev){
		dragObject = this;
		mouseOffset = getMouseOffset(this, ev);
		return false;
	}
}
function mouseCoords(ev){
	if(ev.pageX || ev.pageY){
		return {x:ev.pageX, y:ev.pageY};
	}
	return {
		x:ev.clientX + document.body.scrollLeft - document.body.clientLeft,
		y:ev.clientY + document.body.scrollTop - document.body.clientTop
	};
}
function mouseDown(){
	iMouseDown = true;
	if(lastTarget){
		return false;
	}
}
function mouseMove(ev){
	ev = ev || window.event;
	var target = ev.target || ev.srcElement;
	var mousePos = mouseCoords(ev);
	if(lastTarget && (target!==lastTarget)){
		var origClass = lastTarget.getAttribute('origClass');
		if(origClass) lastTarget.className = origClass;
	}
	var dragObj = target.getAttribute('DragObj');
	if(dragObj!=null){
		if(target!=lastTarget){
			var oClass = target.getAttribute('overClass');
			if(oClass){
				target.setAttribute('origClass', target.className);
				target.className = oClass;
			}
		}
		if(iMouseDown && !lMouseState){
			curTarget = target;
			rootParent = curTarget.parentNode;
			rootSibling = curTarget.nextSibling;
			mouseOffset = getMouseOffset(target, ev);
			for(var i=0; i<dragHelper.childNodes.length; i++) dragHelper.removeChild(dragHelper.childNodes[i]);
			dragHelper.appendChild(curTarget.cloneNode(true));
			dragHelper.style.display = 'block';
			var dragClass = curTarget.getAttribute('dragClass');
			if(dragClass){
				dragHelper.firstChild.className = dragClass;
			}
			dragHelper.firstChild.removeAttribute('DragObj');
			var dragConts = DragDrops[dragObj];
			curTarget.setAttribute('startWidth', parseInt(curTarget.offsetWidth));
			curTarget.setAttribute('startHeight', parseInt(curTarget.offsetHeight));
			curTarget.style.display = 'none';
			for(var i=0; i<dragConts.length; i++){
				with(dragConts[i]){
					var pos = getPosition(dragConts[i]);
					setAttribute('startWidth', parseInt(offsetWidth));
					setAttribute('startHeight', parseInt(offsetHeight));
					setAttribute('startLeft', parseInt(pos.x));
					setAttribute('startTop', parseInt(pos.y));
				}
				for(var j=0; j<dragConts[i].childNodes.length; j++){
					with(dragConts[i].childNodes[j]){
						if((nodeName=='#text') || (dragConts[i].childNodes[j]==curTarget)) continue;
						var pos = getPosition(dragConts[i].childNodes[j]);
						setAttribute('startWidth', parseInt(offsetWidth));
						setAttribute('startHeight', parseInt(offsetHeight));
						setAttribute('startLeft', parseInt(pos.x));
						setAttribute('startTop', parseInt(pos.y));
					}
				}
			}
		}
	}
	if(curTarget){
		dragHelper.style.top =mousePos.y - mouseOffset.y + 'px';
		dragHelper.style.left = mousePos.x - mouseOffset.x + 'px';
		var dragConts = DragDrops[curTarget.getAttribute('DragObj')];
		var activeCont = null;
		var xPos = mousePos.x - mouseOffset.x + (parseInt(curTarget.getAttribute('startWidth')) /2);
		var yPos = mousePos.y - mouseOffset.y + (parseInt(curTarget.getAttribute('startHeight'))/2);
		for(var i=0; i<dragConts.length; i++){
			with(dragConts[i]){
				if(((getAttribute('startLeft')) < xPos) &&
					((getAttribute('startTop')) < yPos) &&
					((parseInt(getAttribute('startLeft')) + parseInt(getAttribute('startWidth'))) > xPos) &&
					((parseInt(getAttribute('startTop')) + parseInt(getAttribute('startHeight')) + 5) > yPos)){
						activeCont = dragConts[i];
						break;
				}
			}
		}
		if(activeCont){
			var beforeNode = null;
			for(var i=activeCont.childNodes.length-1; i>=0; i--){
				with(activeCont.childNodes[i]){
					if(nodeName=='#text') continue;
					if(
						curTarget != activeCont.childNodes[i] &&
						((parseInt(getAttribute('startLeft')) + parseInt(getAttribute('startWidth'))) > xPos) &&
						((parseInt(getAttribute('startTop')) + parseInt(getAttribute('startHeight'))) > yPos)){
							beforeNode = activeCont.childNodes[i];
					}
				}
			}
			if(beforeNode){
				if(beforeNode!=curTarget.nextSibling){
					activeCont.insertBefore(curTarget, beforeNode);
				}
			} else {
				if((curTarget.nextSibling) || (curTarget.parentNode!=activeCont)){
					activeCont.appendChild(curTarget);
				}
			}
			if(curTarget.style.display!=''){
				curTarget.style.display = '';
			}
		} else {
			if(curTarget.style.display!='none'){
				curTarget.style.display = 'none';
			}
		}
	}
	lMouseState = iMouseDown;
	lastTarget = target;
	lMouseState = iMouseDown;
	return false;
}
function mouseUp(ev){
	if(curTarget){
		dragHelper.style.display = 'none';
		if(curTarget.style.display == 'none'){
			if(rootSibling){
				rootParent.insertBefore(curTarget, rootSibling);
			} else {
				rootParent.appendChild(curTarget);
			}
		}
		curTarget.style.display = '';
	}
	curTarget = null;
	iMouseDown = false;
}
