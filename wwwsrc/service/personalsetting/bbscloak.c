/*$Id: bbscloak.c,v 1.2 2008-12-19 09:55:07 madoldman Exp $
*/

#include "BBSLIB.inc"

int main()
{
        init_all();
        if(!loginok)
                http_fatal("匆匆过客不能进行此操作, 请先登录");
        if(!(currentuser.userlevel & PERM_CLOAK))
                http_fatal("错误的参数");
        if(u_info->invisible) {
                u_info->invisible=0;
                printf("隐身状态已经停止了.");
        } else {
                u_info->invisible=1;
                printf("隐身状态已经开始了.");
        }
}
