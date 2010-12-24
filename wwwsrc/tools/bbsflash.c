/*$Id: bbsflash.c,v 1.2 2008-12-19 09:55:08 madoldman Exp $
*/

#include "BBSLIB.inc"
int main()
{
        char buf[256];
        init_all();
        strsncpy(buf, getparm("id"), 256);
        printf("<title> %s Flash–¿…Õ</title>\
               <body><center>\
               <object classid=clsid:D27CDB6E-AE6D-11cf-96B8-444553540000  width=600 height=400 codebase=http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=5,0,0,0 >\
               <param name=movie value=\"%s\">\
               <param name=quality value=high>\
               <embed src=\"%s\"  quality=high\
               pluginspage=http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash\
               \
               type=application/x-shockwave-flash>\
               </embed>\
               </object>\
               \
               </center>   <br>",BBSNAME,nohtml(buf),nohtml(buf));
        http_quit();
}


