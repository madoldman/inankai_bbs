/*$Id: bbschat.c,v 1.2 2008-12-19 09:55:05 madoldman Exp $
*/

#include "BBSLIB.inc"
#include "netinet/in.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
int chat_fd;
int test=0;
char genbuf[1024];

int main()
{
        char t[80];
        int pid;
        init_all();
        strsncpy(t, getparm("t"), 80);
        pid=atoi(getparm("pid"));
        if(pid==0)
                reg();
        if(!loginok) {
                http_fatal("错误，请先登录");
                exit(0);
        }
        if(pid>0)
                if(kill(pid, 0)!=0) {
                        printf("<script>top.location='about:您已经断线或已有1个窗口进入聊天室了.'</script>");
                        exit(0);
                }
        if(!strcmp(t, "frm"))
                show_frm(pid);
        if(!strcmp(t, "chatsnd"))
                chatsnd(pid);
        if(!strcmp(t, "frame_input"))
                frame_input(pid);
        if(!strcmp(t, "chatrefresh")) {
                test=1;
                chatrefresh(pid);
        }
        printf("err cmd");
}

int reg()
{
        int n, pid;
        if(pid=fork()) {
                sprintf(genbuf, "bbschat?pid=%d&t=frm", pid);
                redirect(genbuf);
                http_quit();
        }
        for(n=0; n<1024; n++)
                close(n);
        pid=getpid();
        agent(pid);
        exit(0);
}

int show_frm(int pid)
{
        printf("\
               <frameset rows=0,0,*,48,16 frameborder=0>\
               <frame name=hide src=''>\
               <frame name=hide2 src=''>\
               <frame name=main src=''>\
               <frame scrolling=auto marginheight=1 framespacing=1 name=input src=bbschat?t=frame_input&pid=%d>\
               <frame scrolling=no marginwidth=4 marginheight=1 framespacing=1 name=foot src=bbsfoot>\
               </frameset>\
               </html>\
               ", pid);
        http_quit();
}

int frame_input(int pid)
{
        printf("\
               <script>\
               function r1() {\
               top.hide2.location='bbschat?t=chatrefresh&pid=%d';\
               setTimeout('r1()', 10000);\
       }\
               setTimeout('r1()', 500);\
               </script>\
               <body onload='document.form1.in1.focus()' bgcolor=eeeee0>\
               <nobr>\
               <form onsubmit='add_cc()' name=form1 action=bbschat?pid=%d&t=chatsnd method=post target=hide>\
               Input: <input name=in1 maxlength=60 size=56>\
               <input type=submit value=发送>\
               <script>\
               var cc, cc2;\
               cc='';\
               function add_cc0(x1, x) {\
               cc2=x1;\
               cc=x;\
       }\
               function do_quit() {\
               if(confirm('您真的要退出了吗？')) {\
               form1.in1.value='/b';\
               form1.submit();\
       }\
       }\
               function do_help() {\
               open('/chathelp.html', '_blank',\
               'toolbar=yes,location=no,status=no,menubar=no,scrollbar=auto,resizable=yes,width=620,height=400');\
       }\
               function do_alias(x) {\
               form1.in1.value=x;\
               form1.submit();\
       }\
               function do_room() {\
               xx=prompt('请输入包厢名称','');\
               if(xx=='' || xx==null) return;\
               form1.in1.value='/j '+ xx;\
               form1.submit();\
       }\
               function do_user() {\
               form1.in1.value='/l';\
               form1.submit();\
       }\
               function do_r() {\
               form1.in1.value='/r';\
               form1.submit();\
       }\
               function do_w() {\
               form1.in1.value='/w';\
               form1.submit();\
       }\
               function do_msg() {\
               xx=prompt('给谁丢小纸条','');\
               if(xx=='' || xx==null) return;\
               yy=prompt('什么内容','');\
               if(yy=='' || xx==null) return;\
               form1.in1.value='/m '+xx+' '+yy;\
               form1.submit();\
       }\
               function do_n() {\
               xx=prompt('你想改成什么名字?','');\
               if(xx=='' || xx==null) return;\
               form1.in1.value='/n '+xx;\
               form1.submit();\
       }\
               function do_pic() {\
               xx=prompt('请输入图片的URL地址:','http://');\
               if(xx=='http://' || xx=='' || xx==null) return;\
               form1.in1.value='<img src='+xx+'>';\
               form1.submit();\
       }\
               </script>\
               <select onChange='do_alias(this.options[this.selectedIndex].value);this.selectedIndex=0;'>\
               <option value=' ' selected>聊天动作</option>\
               <option value='//hehe'>呵呵的傻笑</option>\
               <option value='//faint'>要晕倒了</option>\
               <option value='//sleep'>要睡着了</option>\
               <option value='//:D'>乐滋滋的</option>\
               <option value='//so'>就这样</option>\
               <option value='//shake'>摇摇头</option>\
               <option value='//luck'>真幸运啊</option>\
               <option value='//tongue'>吐吐舌头</option>\
               <option value='//blush'>脸红了</option>\
               <option value='//applaud'>热烈鼓掌</option>\
               <option value='//cough'>咳嗽一下</option>\
               <option value='//happy'>好高兴啊</option>\
               <option value='//hungry'>肚子饿了</option>\
               <option value='//strut>大摇大摆</option>\
               <option value='//think'>想一想</option>\
               <option value='//?'>疑惑不已</option>\
               <option value='//bearbug'>热情拥抱</option>\
               <option value='//bless'>祝福</option>\
               <option value='//bow'>鞠躬</option>\
               <option value='//caress'>抚摸</option>\
               <option value='//cringe'>企求宽恕</option>\
               <option value='//cry'>放声大哭</option>\
               <option value='//comfort'>安慰一下</option>\
               <option value='//clap'>热烈鼓掌</option>\
               <option value='//dance'>翩翩起舞</option>\
               <option value='//drivel'>流口水</option>\
               <option value='//farewell'>再见</option>\
               <option value='//giggle'>呆笑</option>\
               <option value='//grin'>咧嘴笑</option>\
               <option value='//growl'>大声咆哮</option>\
               /*\
               hand      heng      hug       haha      heihei    joycup    kick\
               kiss      koko      laugh     mm        nod       nudge     oh        pad\
               pat       papaya    pinch     punch     pure      puke      report    shrug\
               sigh      slap      smooch    snicker   sniff     spank     squeeze   thank\
               tickle    wave      welcome   wink      xixi      zap\
\
               【 Verb + Message：动词 + 要说的话 】   例：sing 天天天蓝\
               ask       chant     cheer     chuckle   curse     demand    frown     groan\
               grumble   hum       moan      notice    order     ponder    pout      pray\
               request   shout     sing      smile     smirk     swear     tease     whimper\
               yawn      yell\
               */\
               </select>\
               <select name=ccc onChange='add_cc0(this, this.options[this.selectedIndex].value)'>\
               <option value='' selected>白色</option>\
               <option value='%s'><font color=green>红色</font></option>\
               <option value='%s'><font color=red>绿色</font></option>\
               <option value='%s'><font color=blue>蓝色</font></option>\
               <option value='%s'><font color=blue>天蓝</font></option>\
               <option value='%s'><font color=yellow>黄色</font></option>\
               <option value='%s'><font color=red>品红</font></option>\
               <option value='%s'>大字</option>\
               </select>\
               <select onChange='do_func(this.selectedIndex);this.selectedIndex=0;'>\
               <option selected>聊天室功能</option>\
               <option>进入包厢</option>\
               <option>查看包厢名</option>\
               <option>本包厢有谁</option>\
               <option>看有谁在线</option>\
               <option>丢小纸条</option>\
               <option>改聊天代号</option>\
               <option>贴图片</option>\
               <option>清除屏幕</option>\
               <option>背景反色</option>\
               <option>离开聊天室</option>\
               </select>\
               <br>\
               <a href='javascript:do_quit()'>[离开bbs茶馆] </a>\
               <a href='/chathelp.html' target=_blank>[聊天室帮助] </a>\
               <script>\
               function do_func(n) {\
               if(n==0) return;\
               if(n==1) return do_room();\
               if(n==2) return do_r();\
               if(n==3) return do_w();\
               if(n==4) return do_user();\
               if(n==5) return do_msg();\
               if(n==6) return do_n();\
               if(n==7) return do_pic();\
               if(n==8) return do_c();\
               if(n==9) return do_css2();\
               if(n==10) return do_quit();\
       }\
               var css1;\
               css1='/bbschat.css';\
               function do_c() {\
               top.main.document.close();\
               top.main.document.writeln('<link rel=stylesheet type=text/css href='+css1+'><body><pre><font class=c37>');\
       }\
               function do_css2() {\
               if(css1=='/bbschat.css')\
               css1='/bbschat2.css';\
               else\
               css1='/bbs.css';\
               top.main.document.writeln('<link rel=stylesheet type=text/css href='+css1+'><body><pre><font class=c37>');\
       }\
               function add_cc() {\
               xxx=form1.in1.value;\
               if(xxx=='/h') {\
               do_help();\
               form1.in1.value='';\
               return;\
       }\
               if(xxx=='/c') {\
               do_c();\
               form1.in1.value='';\
               return;\
       }\
               if(xxx=='') return;\
               if(xxx.indexOf('/')<0) {\
               form1.in1.value=cc+xxx;\
       }\
               if(cc==\"%I\") {\
               cc='';\
               cc2.selectedIndex=0;\
       }\
       }\
               </script>\
               </form></body>\
               \
               ", pid, pid, "%R", "%G", "%B", "%C", "%Y", "%M", "%I");
        http_quit();
}

int chatsnd(int pid)
{
        char in1[255], filename[256];
        FILE *fp;
        strsncpy(in1, getparm("in1"), 60);
        sprintf(filename, "tmp/%d.in", pid);
        fp=fopen(filename, "a");
        fprintf(fp, "%s\n\n", in1);
        fclose(fp);
        chatrefresh(pid);
}

char *cco(char *s)
{
        static char buf[512];
        char *p=buf, co[20];
        bzero(buf, 512);
        while(s[0]) {
                if(s[0]!='%') {
                        p[0]=s[0];
                        p++;
                        s++;
                        continue;
                }
                bzero(co, 20);
                if(!strncmp(s, "%R", 2))
                        strcpy(co, "[31m");
                if(!strncmp(s, "%G", 2))
                        strcpy(co, "[32m");
                if(!strncmp(s, "%B", 2))
                        strcpy(co, "[34m");
                if(!strncmp(s, "%C", 2))
                        strcpy(co, "[36m");
                if(!strncmp(s, "%Y", 2))
                        strcpy(co, "[33m");
                if(!strncmp(s, "%M", 2))
                        strcpy(co, "[35m");
                if(!strncmp(s, "%N", 2))
                        strcpy(co, "[0m");
                if(!strncmp(s, "%W", 2))
                        strcpy(co, "[37m");
                if(!strncmp(s, "%I", 2))
                        strcpy(co, "[99m");
                if(co[0]) {
                        strncpy(p, co, strlen(co));
                        p+=strlen(co);
                        s+=2;
                        continue;
                }
                p[0]=s[0];
                p++;
                s++;
        }
        return buf;
}

int chatrefresh(int pid)
{
        char filename[256], tmp[256];
        int t1;
        FILE *fp;
        kill(pid, SIGINT);
        usleep(150000);
        if(kill(pid, 0)!=0) {
                printf("<script>top.location='javascript:close()';</script>");
                exit(0);
        }
        sprintf(filename, "tmp/%d.out", pid);
        t1=time(0);
        while(abs(t1-time(0))<8 && !file_exist(filename)) {
                sleep(1);
                continue;
        }
        fp=fopen(filename, "r");
        if(fp)
                while(1) {
                        int i;
                        char buf2[512];
                        if(fgets(buf2, 255, fp)<=0)
                                break;
                        sprintf(genbuf, "%s", cco(buf2));
                        for(i=0; genbuf[i]; i++)
                                if(genbuf[i]==10 || genbuf[i]==13)
                                        genbuf[i]=0;
                        if(!strncmp(genbuf, "/init", 5)) {
                                printf("<script>\n");
                                printf("top.main.document.write(\"");
                                printf("<link rel=stylesheet type=text/css href='/bbschat.css'><body id=body1 bgColor=black><pre>");
                                printf("\");");
                                printf("\n</script>\n");
                                continue;
                        }
                        if(!strncmp(genbuf, "/t", 2)) {
                                int i;
                                printf("<script>top.document.title='bbs茶馆--话题: ");
                                hprintf(genbuf+2);
                                printf("'</script>");
                                sprintf(buf2, "本包厢的话题是: [[1;33m%s[37m]", genbuf+2);
                                strcpy(genbuf, buf2);
                        }
                        if(!strncmp(genbuf, "/r", 2)) {
                                sprintf(buf2, "本包厢的名称是: [[1;33m%s[37m]", genbuf+2);
                                strcpy(genbuf, buf2);
                        }
                        if(!strncmp(genbuf, "/", 1)) {
                                genbuf[0]='>';
                                genbuf[1]='>';
                        }
                        for(i=0; i<strlen(genbuf); i++) {
                                if(genbuf[i]==10 || genbuf[i]==13)
                                        genbuf[i]=0;
                                if(genbuf[i]==34)
                                        genbuf[i]=39;
                        }
                        printf("<script>\n");
                        printf("top.main.document.writeln(\"");
                        hhprintf(genbuf);
                        printf(" <font class=c37>");
                        printf("\");");
                        printf("top.main.scrollBy(0, 99999);\n");
                        if(test==0)
                                printf("top.input.form1.in1.value='';\n");
                        printf("</script>\n");
                }
        unlink(filename);
        printf("<br>");
        http_quit();
}

void foo()
{
        FILE *fp;
        char filename[80], buf[256];
        sprintf(filename, "tmp/%d.in", getpid());
        fp=fopen(filename, "r");
        if(fp) {
                while(1) {
                        if(fgets(buf, 250, fp)<=0)
                                break;
                        write(chat_fd, buf, strlen(buf));
                }
                fclose(fp);
        }
        unlink(filename);
        alarm(60);
}

void abort_chat()
{
        int pid=getpid();
        char filename[200];
        sprintf(filename, "tmp/%d.out", pid);
        unlink(filename);
        sprintf(filename, "tmp/%d.in", pid);
        unlink(filename);
        exit(0);
}

int agent(int pid)
{
        int i, num;
        FILE *fp;
        char filename[80];
        struct sockaddr_in blah;
        sprintf(filename, "tmp/%d.out", pid);
        bzero((char *)&blah, sizeof(blah));
        blah.sin_family=AF_INET;
        blah.sin_addr.s_addr=inet_addr("127.0.0.1");
        blah.sin_port=htons(7202);
        chat_fd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(connect(chat_fd, (struct sockaddr *)&blah, 16)<0)
                return;
        signal(SIGINT, foo);
        signal(SIGALRM, abort_chat);
        alarm(60);
        sprintf(genbuf, "/! %d %d %s %s %d\n",
                u_info->uid, currentuser.userlevel, currentuser.userid, currentuser.userid, 0);
        write(chat_fd, genbuf, strlen(genbuf));
        read(chat_fd, genbuf, 2);
        if(!strcasecmp(genbuf, "OK"))
                exit(0);
        fp=fopen(filename, "w");
        fprintf(fp, "/init\n");
        fclose(fp);
        while(1) {
                num=read(chat_fd, genbuf, 2048);
                if (num<=0)
                        break;
                for(i=0; i<num; i++)
                        if(genbuf[i]==0)
                                genbuf[i]=10;
                genbuf[num]=0;
                fp=fopen(filename, "a");
                fprintf(fp, "%s", genbuf);
                fclose(fp);
        }
        abort_chat();
}

