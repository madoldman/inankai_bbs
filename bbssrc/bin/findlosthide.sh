#!/bin/bash
if [ $# != 1 ]
then
	echo "usage:$0 path"
	exit 1;
fi
num=1
for file in `ls $1`
do
	grep $file $1/.Names
	if [ $? != 0 ] && [ $file != counter.person ]
	then
		echo "Name=ур╩ь$num" >> $1/.Names
		echo "Path=~/$file" >> $1/.Names
		echo '#' >> $1/.Names
		num=`expr $num + 1`
	fi
done
