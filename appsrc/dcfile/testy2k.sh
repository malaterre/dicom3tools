#!/bin/sh

DCCREATEY2K="./dcsmpte -rows 256 -columns 256 -bits 8 -minval 0 -maxval 255"
DCDUMP=./dcdump
DCCP=./dccp
DCIODVFY=./dciodvfy

createwithnewstyledate() {	# usage: makegenctwithnewdate dstfile yyyy mm dd hh min ss

	$DCCREATEY2K "$1" -r StudyDate "$2$3$4" -r StudyTime "$5$6$7"
}

createwitholdstyledate() {	# usage: makegenctwithnewdate dstfile yyyy mm dd hh min ss

	$DCCREATEY2K "$1" -r StudyDate "$2.$3.$4" -r StudyTime "$5:$6:$7"
}


TMPROOT=/tmp/`basename $0`.$$

if [ "$#" != 2 ]
then
	bad="true"
else
	testlogdir="$1"	# eg. $(TOPDIR)/test/$(CURRENT_DIR)
	mode="$2"	# eg. create|compare

	if [ "$mode" != "create" -a "$mode" != "compare" ]
	then
		echo "$0: mode not create or compare" 1>&2
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

baseappname="dcfile.y2k"
comparename="$testlogdir/$baseappname.log"

if [ "$mode" = "compare" ]
then
	dst="$TMPROOT.$baseappname.log"
else
	dst="$comparename"
fi

rm -f "$dst"

echo "test of valid dates: ============================================" >>"$dst"

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

	echo "New style" >>"$dst"

	createwithnewstyledate "$TMPROOT.test.newstyle.y2k.$datea.dcm" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	echo "dcdump of created file" >>"$dst"
	$DCDUMP "$TMPROOT.test.newstyle.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of created file" >>"$dst"
	$DCIODVFY "$TMPROOT.test.newstyle.y2k.$datea.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	$DCCP "$TMPROOT.test.newstyle.y2k.$datea.dcm" "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm"
	echo "dcdump of copy" >>"$dst"
	$DCDUMP "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of copy" >>"$dst"
	$DCIODVFY "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	rm "$TMPROOT.test.newstyle.y2k.$datea.dcm"
	rm "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm"

	echo "Old style" >>"$dst"

	createwitholdstyledate "$TMPROOT.test.oldstyle.y2k.$datea.dcm" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	echo "dcdump of created file" >>"$dst"
	$DCDUMP "$TMPROOT.test.oldstyle.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of created file" >>"$dst"
	$DCIODVFY "$TMPROOT.test.oldstyle.y2k.$datea.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	$DCCP "$TMPROOT.test.oldstyle.y2k.$datea.dcm" "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm"
	echo "dcdump of copy" >>"$dst"
	$DCDUMP "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of copy" >>"$dst"
	$DCIODVFY "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	rm "$TMPROOT.test.oldstyle.y2k.$datea.dcm"
	rm "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm"
done

echo "test of invalid dates: ============================================" >>"$dst"

for datea in \
	19700229.235959 \
	19980431.235959 \
	19990229.000000 \
	20000230.235959 \
	20010229.235959 \
	20040230.235959
do
	echo "$datea"
	echo "$datea" >>"$dst"
	yyyy=`echo "$datea" | sed 's/^\([0-9][0-9][0-9][0-9]\).*$/\1/'`
	mm=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	dd=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	hh=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][.]\([0-9][0-9]\).*$/\1/'`
	min=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][.][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	ss=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9][.][0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`

	echo "New style" >>"$dst"

	createwithnewstyledate "$TMPROOT.test.newstyle.y2k.$datea.dcm" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	echo "dcdump of created file" >>"$dst"
	$DCDUMP "$TMPROOT.test.newstyle.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of created file" >>"$dst"
	$DCIODVFY "$TMPROOT.test.newstyle.y2k.$datea.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	$DCCP "$TMPROOT.test.newstyle.y2k.$datea.dcm" "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm"
	echo "dcdump of copy" >>"$dst"
	$DCDUMP "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of copy" >>"$dst"
	$DCIODVFY "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	rm "$TMPROOT.test.newstyle.y2k.$datea.dcm"
	rm "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm"

	echo "Old style" >>"$dst"

	createwitholdstyledate "$TMPROOT.test.oldstyle.y2k.$datea.dcm" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	echo "dcdump of created file" >>"$dst"
	$DCDUMP "$TMPROOT.test.oldstyle.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of created file" >>"$dst"
	$DCIODVFY "$TMPROOT.test.oldstyle.y2k.$datea.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	$DCCP "$TMPROOT.test.oldstyle.y2k.$datea.dcm" "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm"
	echo "dcdump of copy" >>"$dst"
	$DCDUMP "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of copy" >>"$dst"
	$DCIODVFY "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	rm "$TMPROOT.test.oldstyle.y2k.$datea.dcm"
	rm "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm"
done


echo "test of ambiguous dates: ============================================" >>"$dst"

for datea in \
	000101.101010 \
	990101.101010
do
	echo "$datea"
	echo "$datea" >>"$dst"
	yyyy=`echo "$datea" | sed 's/^\([0-9][0-9]\).*$/\1/'`
	mm=`echo "$datea" | sed 's/^[0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	dd=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	hh=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][.]\([0-9][0-9]\).*$/\1/'`
	min=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][.][0-9][0-9]\([0-9][0-9]\).*$/\1/'`
	ss=`echo "$datea" | sed 's/^[0-9][0-9][0-9][0-9][0-9][0-9][.][0-9][0-9][0-9][0-9]\([0-9][0-9]\).*$/\1/'`

	echo "New style" >>"$dst"

	createwithnewstyledate "$TMPROOT.test.newstyle.y2k.$datea.dcm" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	echo "dcdump of created file" >>"$dst"
	$DCDUMP "$TMPROOT.test.newstyle.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of created file" >>"$dst"
	$DCIODVFY "$TMPROOT.test.newstyle.y2k.$datea.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	$DCCP "$TMPROOT.test.newstyle.y2k.$datea.dcm" "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm"
	echo "dcdump of copy" >>"$dst"
	$DCDUMP "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of copy" >>"$dst"
	$DCIODVFY "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	rm "$TMPROOT.test.newstyle.y2k.$datea.dcm"
	rm "$TMPROOT.test.newstyle.y2k.$datea.copy.dcm"

	echo "Old style" >>"$dst"

	createwitholdstyledate "$TMPROOT.test.oldstyle.y2k.$datea.dcm" "$yyyy" "$mm" "$dd" "$hh" "$min" "$ss"
	echo "dcdump of created file" >>"$dst"
	$DCDUMP "$TMPROOT.test.oldstyle.y2k.$datea.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of created file" >>"$dst"
	$DCIODVFY "$TMPROOT.test.oldstyle.y2k.$datea.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	$DCCP "$TMPROOT.test.oldstyle.y2k.$datea.dcm" "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm"
	echo "dcdump of copy" >>"$dst"
	$DCDUMP "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm" 2>&1 \
		| egrep -i 'date|time' \
		| egrep -i 'study|series|image|acquisition' \
			>>"$dst"
	echo "dciodvfy of copy" >>"$dst"
	$DCIODVFY "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm" 2>&1 \
		| grep -v SCImage \
			>>"$dst"
	rm "$TMPROOT.test.oldstyle.y2k.$datea.dcm"
	rm "$TMPROOT.test.oldstyle.y2k.$datea.copy.dcm"
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
