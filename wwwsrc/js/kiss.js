function flush() {
      window.status="nkbbs.org";
      parent.hide.location='kiss?action=flush';
      window.status="nkbbs.org";
      setTimeout('flush()', 2000);
}

function check(){
   if(document.form1.id.value==""){
      alert("你想对谁说呢?");
       return false;
   }
   if(document.form1.word.value==""){
      alert("聊天内容不能为空!");
	  return false;
   }
   document.form1.word2.value=document.form1.word.value;
   document.form1.word.value=""
   document.form1.word.focus();
   return true;
}

function changeuser(id,nick){
        top.input.document.form1.id.value=id;
               top.input.document.form1.nick.value=nick;
               top.input.document.form1.talker.value=id+"("+nick+")";

 top.input.document.form1.word.focus();

}

function changecolor(c){
      top.input.document.form1.color.value=c; 
      top.input.document.form1.word.focus(); 
}

function say(s)
{
   top.input.document.form1.word.value=s;
   top.input.document.form1.word2.value=s;
   top.input.document.form1.word.focus();

}
