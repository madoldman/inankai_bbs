#!/bin/sh
echo "��ѯʱ��Ϊ:" > /home/bbs/tmp/sysinfo.$1
date >> /home/bbs/tmp/sysinfo.$1
/usr/bin/uptime >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  ������ʹ��״�� (�� KB Ϊ��λ) *****" >> /home/bbs/tmp/sysinfo.$1
free -t >> /home/bbs/tmp/sysinfo.$1
/usr/bin/ipcs -mu >> /home/bbs/tmp/sysinfo.$1
/usr/bin/ipcs >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  Ӳ��ʹ��״�� (�� KB Ϊ��λ)  *****" >> /home/bbs/tmp/sysinfo.$1
df   >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  �ż�����״��  *****" >> /home/bbs/tmp/sysinfo.$1
/usr/sbin/mailstats >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  Kernel Interface Statistics  *****" >> /home/bbs/tmp/sysinfo.$1
netstat -i >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
echo "*****  Process ��Ѷһ��  *****" >> /home/bbs/tmp/sysinfo.$1
ps -aux >> /home/bbs/tmp/sysinfo.$1
echo "" >> /home/bbs/tmp/sysinfo.$1
