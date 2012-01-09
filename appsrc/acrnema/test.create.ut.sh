#!/bin/sh

# Usage: $0 create|compare

TMPROOT="/tmp/`basename $0`.$$"

TESTLOGDIR="../../test/appsrc/acrnema"

ANCREATE="./ancreate"
ANDUMP="./andump"
DCDUMP="../dcfile/dcdump"
DCCP="../dcfile/dccp"

bad="false"

if [ "$#" != 1 ]
then
	bad="true"
else
	mode="$1"	# eg. create|compare

	if [ "$mode" != "create" -a "$mode" != "compare" ]
	then
		echo "$0: mode not create or compare" 1>&2
		bad="true"
	fi
fi

if [ "$bad" = "true" ]
then
	echo "Usage: $0 create|compare" 1>&2
	exit 1
fi

if [ "$mode" = "create" ]
then
	WHERE="$TESTLOGDIR"
else
	WHERE="$TMPROOT"
fi

if [ ! -d "$WHERE" ]; then mkdirhier "$WHERE"; fi

comparebinaries() {
	echo Comparing $1
	cmp $TESTLOGDIR/$1 $WHERE/$1
}

comparedumps() {
	echo Diffs for $1
	diff -b $TESTLOGDIR/$1 $WHERE/$1
}

echo $0 starting

# Specify VR but will be lost in implicit stream ...

$ANCREATE <<EOF > $WHERE/ut.ile.nm
(0x0040,0xa160) UT Text Value 	 VR=<UT>   VL=<0x000e> <Test VR of UT>
EOF

if [ "$mode" = "compare" ]; then comparebinaries ut.ile.nm ; fi

$ANDUMP $WHERE/ut.ile.nm > $WHERE/ut.ile.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.andump ; fi
$DCDUMP  $WHERE/ut.ile.nm > $WHERE/ut.ile.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.dcdump ; fi
od -b $WHERE/ut.ile.nm > $WHERE/ut.ile.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.odx ; fi
od -c $WHERE/ut.ile.nm > $WHERE/ut.ile.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.odc ; fi


$DCCP $WHERE/ut.ile.nm $WHERE/ut.ile.nm.cp.ile.nm -n -nodisclaimer > $WHERE/ut.ile.nm.cp.ile.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries ut.ile.nm.cp.ile.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ile.nm.dccp ; fi
$ANDUMP $WHERE/ut.ile.nm.cp.ile.nm > $WHERE/ut.ile.nm.cp.ile.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ile.nm.andump ; fi
$DCDUMP  $WHERE/ut.ile.nm.cp.ile.nm > $WHERE/ut.ile.nm.cp.ile.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ile.nm.dcdump ; fi
od -b $WHERE/ut.ile.nm.cp.ile.nm > $WHERE/ut.ile.nm.cp.ile.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ile.nm.odx ; fi
od -c $WHERE/ut.ile.nm.cp.ile.nm > $WHERE/ut.ile.nm.cp.ile.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ile.nm.odc ; fi

$DCCP $WHERE/ut.ile.nm $WHERE/ut.ile.nm.cp.ele.nm -n -vr explicit -endian little -nodisclaimer > $WHERE/ut.ile.nm.cp.ele.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries ut.ile.nm.cp.ele.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ele.nm.dccp ; fi
$ANDUMP $WHERE/ut.ile.nm.cp.ele.nm > $WHERE/ut.ile.nm.cp.ele.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ele.nm.andump ; fi
$DCDUMP  $WHERE/ut.ile.nm.cp.ele.nm > $WHERE/ut.ile.nm.cp.ele.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ele.nm.dcdump ; fi
od -b $WHERE/ut.ile.nm.cp.ele.nm > $WHERE/ut.ile.nm.cp.ele.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ele.nm.odx ; fi
od -c $WHERE/ut.ile.nm.cp.ele.nm > $WHERE/ut.ile.nm.cp.ele.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ile.nm.cp.ele.nm.odc ; fi


# Specify VRs directly for ut elements in explicit stream ...

$ANCREATE <<EOF > $WHERE/ut.ele.nm -explicit
(0x0040,0xa160) UT Text Value 	 VR=<UT>   VL=<0x000e> <Test VR of UT>
EOF

if [ "$mode" = "compare" ]; then comparebinaries ut.ele.nm ; fi

$ANDUMP $WHERE/ut.ele.nm > $WHERE/ut.ele.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.andump ; fi
$DCDUMP  $WHERE/ut.ele.nm > $WHERE/ut.ele.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.dcdump ; fi
od -b $WHERE/ut.ele.nm > $WHERE/ut.ele.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.odx ; fi
od -c $WHERE/ut.ele.nm > $WHERE/ut.ele.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.odc ; fi

$DCCP $WHERE/ut.ele.nm $WHERE/ut.ele.nm.cp.ile.nm -n -nodisclaimer > $WHERE/ut.ele.nm.cp.ile.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries ut.ele.nm.cp.ile.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ile.nm.dccp ; fi
$ANDUMP $WHERE/ut.ele.nm.cp.ile.nm > $WHERE/ut.ele.nm.cp.ile.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ile.nm.andump ; fi
$DCDUMP  $WHERE/ut.ele.nm.cp.ile.nm > $WHERE/ut.ele.nm.cp.ile.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ile.nm.dcdump ; fi
od -b $WHERE/ut.ele.nm.cp.ile.nm > $WHERE/ut.ele.nm.cp.ile.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ile.nm.odx ; fi
od -c $WHERE/ut.ele.nm.cp.ile.nm > $WHERE/ut.ele.nm.cp.ile.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ile.nm.odc ; fi

$DCCP $WHERE/ut.ele.nm $WHERE/ut.ele.nm.cp.ele.nm -n -vr explicit -endian little -nodisclaimer > $WHERE/ut.ele.nm.cp.ele.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries ut.ele.nm.cp.ele.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ele.nm.dccp ; fi
$ANDUMP $WHERE/ut.ele.nm.cp.ele.nm > $WHERE/ut.ele.nm.cp.ele.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ele.nm.andump ; fi
$DCDUMP  $WHERE/ut.ele.nm.cp.ele.nm > $WHERE/ut.ele.nm.cp.ele.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ele.nm.dcdump ; fi
od -b $WHERE/ut.ele.nm.cp.ele.nm > $WHERE/ut.ele.nm.cp.ele.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ele.nm.odx ; fi
od -c $WHERE/ut.ele.nm.cp.ele.nm > $WHERE/ut.ele.nm.cp.ele.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    ut.ele.nm.cp.ele.nm.odc ; fi


if [ "$mode" = "compare" ]
then
	rm -rf "$TMPROOT"
fi

echo $0 finished
