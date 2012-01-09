#!/bin/sh

MKTIME=../../../support/mktime
GENDUMP=./gendump
GENTODC=./gentodc
DCDUMP=../../dcfile/dcdump
BINPATCH=../../misc/binpatch

TMPROOT=/tmp/`basename $0`.$$

makegenctwithnewdate() {	# usage: makegenctwithnewdate srcfile dstfile yyyy mm dd hh min ss

	unixdate=`$MKTIME "$3" "$4" "$5" "$6" "$7" "$8" | awk '{print $4;}'`

	"$BINPATCH" "$1" "$2" \
		-i 1442 4 "$unixdate" b \
		-i 1562 4 "$unixdate" b \
		-i 2270 4 "$unixdate" b \
		-i 2274 4 "$unixdate" b \
		-i 2408 4 "$unixdate" b \
		-i 3292 4 "$unixdate" b \
		-i 3296 4 "$unixdate" b \
		-i 3832 4 "$unixdate" b
}

makegenmrwithnewdate() {	# usage: makegenctwithnewdate srcfile dstfile yyyy mm dd hh min ss

	unixdate=`$MKTIME "$3" "$4" "$5" "$6" "$7" "$8" | awk '{print $4;}'`

	"$BINPATCH" "$1" "$2" \
		-i 2754 4 "$unixdate" b \
		-i 2874 4 "$unixdate" b \
		-i 3582 4 "$unixdate" b \
		-i 3586 4 "$unixdate" b \
		-i 3720 4 "$unixdate" b \
		-i 4604 4 "$unixdate" b \
		-i 4608 4 "$unixdate" b \
		-i 4932 4 "$unixdate" b \
		-i 5110 4 "$unixdate" b
}

if [ "$#" != 3 ]
then
	bad="true"
else
	imagedir="$1"	# eg. $(TOPDIR)/images/genesis
	testlogdir="$2"	# eg. $(TOPDIR)/test/$(CURRENT_DIR)
	mode="$3"	# eg. create|compare

	if [ "$mode" != "create" -a "$mode" != "compare" ]
	then
		echo "$0: mode not create or compare" 1>&2
		bad="true"
	elif [ ! -d "$imagedir" -o ! -f "$imagedir/test.ct" -o ! -f "$imagedir/test.mr" ]
	then
		echo "$0: no such image directory as \"$imagedir\" or test.ct or test.mr missing" 1>&2
		bad="true"
	elif [ ! -d "$testlogdir" -a "$mode" = "create" ]
	then
		echo "$0: making test log directory \"$testlogdir\"" 1>&2
		mkdirhier "$testlogdir"
		#echo "$0: no such test log directory as \"$testlogdir\"" 1>&2
		#bad="true"
	fi
fi

if [ "$bad" = "true" ]
then
	echo "Usage: $0 testlogdir create|compare" 1>&2
	exit 1
fi

if [ "$mode" = "create" ]
then
	if [ ! -d "$testlogdir" ]
	then
		mkdirhier "$testlogdir"
	fi
fi

baseappname="genesis.y2k"
comparename="$testlogdir/$baseappname.log"

if [ "$mode" = "compare" ]
then
	dst="$TMPROOT.$baseappname.log"
else
	dst="$comparename"
fi

rm -f "$dst"

# test of valid dates:

for datea in \
	19700101.000000 \
	19700101.000059 \
	19700101.000100 \
	19700101.005959 \
	19700101.015959 \
	19700101.020000 \
	19700101.235959 \
	19700102.000000 \
	19700227.235959 \
	19700228.235959 \
	19700301.000000 \
	19710227.235959 \
	19710228.235959 \
	19710301.000000 \
	19720227.235959 \
	19720228.235959 \
	19720229.235959 \
	19720301.000000 \
	19981231.235959 \
	19990101.000000 \
	19990227.235959 \
	19990228.235959 \
	19990301.000000 \
	19990909.101010 \
	19991231.235959 \
	20000101.000000 \
	20000227.235959 \
	20000228.235959 \
	20000229.235959 \
	20000301.000000 \
	20001231.235959 \
	20010101.000000 \
	20010228.235959 \
	20010301.000000 \
	20040228.235959 \
	20040229.235959 \
	20040301.000000
do
	echo "$datea"
	echo "$datea" >>"$dst"
	yyyy=`echo "$datea" | sed 's/^\([0-9][0-9][0-9][0-9]\).*$/\1/'`
	mm=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	dd=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	hh=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][.]\([0-9][0-9]\).*$/\1/'`
	min=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][.][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	ss=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][.][0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`

	echo "CT" >>"$dst"

	makegenctwithnewdate "$imagedir/test.ct" "$TMPROOT.test.ct.y2k.$datea" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	$GENDUMP "$TMPROOT.test.ct.y2k.$datea" 2>&1 \
		| egrep -i 'date/time' \
			>>"$dst"
	$GENTODC "$TMPROOT.test.ct.y2k.$datea" "$TMPROOT.test.ct.y2k.$datea.dcm"
	$DCDUMP "$TMPROOT.test.ct.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	rm "$TMPROOT.test.ct.y2k.$datea.dcm"
	rm "$TMPROOT.test.ct.y2k.$datea"

	echo "MR" >>"$dst"

	makegenmrwithnewdate "$imagedir/test.mr" "$TMPROOT.test.mr.y2k.$datea" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	$GENDUMP "$TMPROOT.test.mr.y2k.$datea" 2>&1 \
		| egrep -i 'date/time' \
			>>"$dst"
	$GENTODC "$TMPROOT.test.mr.y2k.$datea" "$TMPROOT.test.mr.y2k.$datea.dcm"
	$DCDUMP "$TMPROOT.test.mr.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	rm "$TMPROOT.test.mr.y2k.$datea.dcm"
	rm "$TMPROOT.test.mr.y2k.$datea"
done

if [ "$mode" = "compare" ]
then
	if [ -f "$comparename" ]
	then
		echo "$baseappname:   comparing"
		cmp "$dst" "$comparename"
	else
		echo "$baseappname:     nothing stored  to compare"
		cat "$dst"
	fi
	rm "$dst"
fi
