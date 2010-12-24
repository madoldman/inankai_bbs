#! /bin/sh
# Install.sh a shell script to install whole BBS

BBS_HOME=/home/bbs
BBSUID=bbs
BBSGRP=bbs
INSTALL="/usr/bin/install -c"
TARGET=/home/bbs/bin

if [ -d ${BBS_HOME} ] ; then
        echo -n "Warning: ${BBS_HOME} already exists, overwrite whole bbs [N]?"
        read ans
        ans=${ans:-N}
        case $ans in
            [Yy]) echo "Installing new bbs to ${BBS_HOME}" ;;
            *) echo "Abort ..." ; exit ;;
        esac
	echo " "
	if [ -f "${BBS_HOME}/.BOARDS" ] ; then
        	if [ ".${FORCE_INSTALL}" = ".YES" ] ; then
                	echo "force install ........ that might be many error, be aware!  "
                	echo "Press <Enter> to continue ..."
                	read ans
        	else
                	echo "WARNING: You have \"make install\" before, so you wont be install again."
                	echo "To install the new binary, please try \"make update\"."
                	echo " "
                	echo "If you want to force the install, please try "
                	echo "% make install FORCE_INSTALL=YES"
			echo "If you only to update your bbs, please try "
			echo "% make update"
                	exit
        	fi
	fi
else
        echo "Making dir ${BBS_HOME}"
        mkdir ${BBS_HOME}
        chown -R ${BBSUID} ${BBS_HOME}
        chgrp -R ${BBSGRP} ${BBS_HOME}
fi

( cd ../bbshome ; tar cf - * ) | ( cd ${BBS_HOME} ; tar xf - )

echo "Setup bbs directory tree ....."
mv ${BBS_HOME}/BOARDS ${BBS_HOME}/.BOARDS
mv ${BBS_HOME}/badname ${BBS_HOME}/.badname
mv ${BBS_HOME}/bad_email ${BBS_HOME}/.bad_email
touch ${BBS_HOME}/.hushlogin
touch ${BBS_HOME}/.visitlog

echo "creating necessary empty directory (user mail, user home)"

alphabet="A B C D E F G H I J K L M N O P Q R S T U V W X Y Z"
dirs="mail home"
preboards="boards vote"
boards="bbslists deleted junk newcomers notepad sysop syssecurity vote"

for x in $dirs; \
do \
	echo "in $x .."
	echo -n "   "
	for y in $alphabet; \
	do \
		echo -n " $y"
		mkdir -p ${BBS_HOME}"/$x/$y"; \
	done
	echo " done.  "
done

for x in $preboards; \
do \
	echo "in $x .."
	echo -n "  "
	for y in $boards; \
	do \
		echo -n " $y"
		mkdir -p ${BBS_HOME}"/$x/$y"; \
	done
	echo " done.  "
done

echo " "

mkdir -p ${BBS_HOME}"/bm"
mkdir -p ${BBS_HOME}"/tmp"
mkdir -p ${BBS_HOME}"/reclog"
mkdir -p ${BBS_HOME}"/so"

echo -n "installing movie items (into boards/notepad) .. "
mv ${BBS_HOME}/movie.dir ${BBS_HOME}/boards/notepad/.DIR
mv ${BBS_HOME}/movie.digest ${BBS_HOME}/boards/notepad/.DIGEST
mv ${BBS_HOME}/movie.1 ${BBS_HOME}/boards/notepad/M.940902694.A
mv ${BBS_HOME}/movie.1.g ${BBS_HOME}/boards/notepad/G.940902694.A
mv ${BBS_HOME}/movie.2 ${BBS_HOME}/boards/notepad/M.941325410.A
mv ${BBS_HOME}/movie.2.g ${BBS_HOME}/boards/notepad/G.941325410.A
mv ${BBS_HOME}/movie.3 ${BBS_HOME}/boards/notepad/M.941325248.A
mv ${BBS_HOME}/movie.3.g ${BBS_HOME}/boards/notepad/G.941325248.A
mv ${BBS_HOME}/movie.4 ${BBS_HOME}/boards/notepad/M.941655041.A
mv ${BBS_HOME}/movie.4.g ${BBS_HOME}/boards/notepad/G.941655041.A

chown -R ${BBSUID} ${BBS_HOME}
chgrp -R ${BBSGRP} ${BBS_HOME}

if [ -f bbsd ] ; then
	${INSTALL} -m 550  -s -g ${BBSGRP} -o ${BBSUID}  bbsd       ${TARGET}
else
	${INSTALL} -m 550  -s -g ${BBSGRP} -o ${BBSUID}  bbs        ${TARGET}
	${INSTALL} -m 4555 -s -g bin       -o root       bbsrf      ${TARGET}
fi
${INSTALL} -m 550  -s -g ${BBSGRP} -o ${BBSUID}  chatd	    ${TARGET}
${INSTALL} -m 550  -s -g ${BBSGRP} -o ${BBSUID}  thread     ${TARGET}
${INSTALL} -m 550  -s -g ${BBSGRP} -o ${BBSUID}  expire     ${TARGET}
${INSTALL} -m 550  -s -g ${BBSGRP} -o ${BBSUID} bbsnet ${TARGET}
if [ -f admintool.so ] ; then
${INSTALL} -m 550  -s -g ${BBSGRP} -o ${BBSUID} admintool.so ${TARGET}/../so
fi

cat > ${BBS_HOME}/etc/sysconf.ini << EOF
#---------------------------------------------------------------
# Here is where you adjust the BBS System Configuration
# Delete ../sysconf.img to make the change after modification
#---------------------------------------------------------------

#---------------------------------------------------------------
# Do not modify anything below unless you know what you are doing...
#---------------------------------------------------------------
PERM_BASIC      = 0x00001
PERM_CHAT       = 0x00002
PERM_PAGE       = 0x00004
PERM_POST       = 0x00008
PERM_LOGINOK    = 0x00010
PERM_DENYPOST   = 0x00020
PERM_CLOAK      = 0x00040
PERM_SEECLOAK   = 0x00080
PERM_XEMPT      = 0x00100
PERM_WELCOME    = 0x00200
PERM_BOARDS     = 0x00400
PERM_ACCOUNTS   = 0x00800
PERM_CHATCLOAK  = 0x01000
PERM_OVOTE      = 0x02000
PERM_SYSOP      = 0x04000
PERM_POSTMASK   = 0x08000
PERM_ANNOUNCE   = 0x10000
PERM_OBOARDS    = 0x20000
PERM_ACBOARD    = 0x40000
PERM_NOZAP      = 0x80000
PERM_FORCEPAGE  = 0x100000
PERM_EXT_IDLE   = 0x200000
PERM_MESSAGE    = 0x400000
PERM_ACHATROOM  = 0x800000
PERM_LARGEMAIL  = 0x1000000
PERM_SPECIAL4   = 0x2000000
PERM_SPECIAL5   = 0x4000000
PERM_SPECIAL6   = 0x8000000
PERM_SPECIAL7   = 0x10000000
PERM_SPECIAL8   = 0x20000000

PERM_ESYSFILE  =  PERM_SYSOP,PERM_WELCOME,PERM_ACBOARD,PERM_ACCOUNTS
PERM_ADMINMENU =  PERM_ACCOUNTS,PERM_OVOTE,PERM_SYSOP,PERM_OBOARDS,PERM_WELCOME,PERM_ACBOARD,PERM_DENYPOST
PERM_BLEVELS   =  PERM_SYSOP,PERM_OBOARDS
PERM_UCLEAN    =  PERM_SYSOP,PERM_ACCOUNTS

#include "etc/menu.ini"

EOF

if test -f ../.reldate; then
  echo "cleanning CVS directories in bbshome ...."
  find ${BBS_HOME} -name "CVS" -print | xargs rm -fr
fi

echo "Install is over...."
echo "Check the configuration in ${BBS_HOME}/etc/sysconf.ini"
echo "Then login your BBS and create an account called SYSOP (case-sensitive)"
