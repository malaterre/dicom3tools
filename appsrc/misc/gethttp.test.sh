#!/bin/sh

HTTPSERVER=flash
HTTPPORT=80
HTTPPATH=pub
LOCALPATH=/usr1/ftp

for i in `(cd $LOCALPATH; find $HTTPPATH -type f -print)`
do
	if gethttp $* $HTTPSERVER $HTTPPORT /$i >/tmp/crap
	then
		cmp $LOCALPATH/$i /tmp/crap
		if [ ! $? ]
		then
			echo "$0: compare failed on $i"
		fi
	else
		echo "$0: gethttp failed on $i"
	fi
done
