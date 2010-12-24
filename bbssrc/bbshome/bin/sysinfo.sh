#!/bin/sh
echo "查询时间为:" > /home/bbs/tmp/sysinfo.$1
date >> /home/bbs/tmp/sysinfo.$1
/usr/bin/uptime >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  记忆体使用状况 (以 KB 为单位) *****" >> /home/bbs/tmp/sysinfo.$1
free -t >> /home/bbs/tmp/sysinfo.$1
/usr/bin/ipcs -mu >> /home/bbs/tmp/sysinfo.$1
/usr/bin/ipcs >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  硬碟使用状况 (以 KB 为单位)  *****" >> /home/bbs/tmp/sysinfo.$1
df   >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  信件传输状况  *****" >> /home/bbs/tmp/sysinfo.$1
/usr/sbin/mailstats >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  Kernel Interface Statistics  *****" >> /home/bbs/tmp/sysinfo.$1
netstat -i >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  Process 资讯一览  *****" >> /home/bbs/tmp/sysinfo.$1
ps -aux >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
