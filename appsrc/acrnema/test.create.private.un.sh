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

$ANCREATE <<EOF > $WHERE/private.ile.nm
(0x0008,0x0008) VR=<CS>   VL=<0x14>   <ORIGINAL\PRIMARY\MPR>
(0x0008,0x0016) VR=<UI>   VL=<0x001a> <1.2.840.10008.5.1.4.1.1.7>
(0x0009,0x0010) VR=<LO>   VL=<0x001a> <CREATOR_THAT_DOESNT_EXIST > 
(0x0009,0x1010) VR=<LO>   VL=<0x000e> <Test VL of LO>
(0x0009,0x1011) VR=<US>   VL=<0x0002> [0x00]
EOF

if [ "$mode" = "compare" ]; then comparebinaries private.ile.nm ; fi

$ANDUMP $WHERE/private.ile.nm > $WHERE/private.ile.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.andump ; fi
$DCDUMP  $WHERE/private.ile.nm > $WHERE/private.ile.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.dcdump ; fi
od -b $WHERE/private.ile.nm > $WHERE/private.ile.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.odx ; fi
od -c $WHERE/private.ile.nm > $WHERE/private.ile.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.odc ; fi


$DCCP $WHERE/private.ile.nm $WHERE/private.ile.nm.cp.ile.nm -n -nodisclaimer > $WHERE/private.ile.nm.cp.ile.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries private.ile.nm.cp.ile.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ile.nm.dccp ; fi
$ANDUMP $WHERE/private.ile.nm.cp.ile.nm > $WHERE/private.ile.nm.cp.ile.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ile.nm.andump ; fi
$DCDUMP  $WHERE/private.ile.nm.cp.ile.nm > $WHERE/private.ile.nm.cp.ile.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ile.nm.dcdump ; fi
od -b $WHERE/private.ile.nm.cp.ile.nm > $WHERE/private.ile.nm.cp.ile.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ile.nm.odx ; fi
od -c $WHERE/private.ile.nm.cp.ile.nm > $WHERE/private.ile.nm.cp.ile.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ile.nm.odc ; fi

$DCCP $WHERE/private.ile.nm $WHERE/private.ile.nm.cp.ele.nm -n -vr explicit -endian little -nodisclaimer > $WHERE/private.ile.nm.cp.ele.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries private.ile.nm.cp.ele.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ele.nm.dccp ; fi
$ANDUMP $WHERE/private.ile.nm.cp.ele.nm > $WHERE/private.ile.nm.cp.ele.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ele.nm.andump ; fi
$DCDUMP  $WHERE/private.ile.nm.cp.ele.nm > $WHERE/private.ile.nm.cp.ele.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ele.nm.dcdump ; fi
od -b $WHERE/private.ile.nm.cp.ele.nm > $WHERE/private.ile.nm.cp.ele.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ele.nm.odx ; fi
od -c $WHERE/private.ile.nm.cp.ele.nm > $WHERE/private.ile.nm.cp.ele.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ile.nm.cp.ele.nm.odc ; fi


# Specify VRs directly for private elements in explicit stream ...

$ANCREATE <<EOF > $WHERE/private.ele.nm -explicit
(0x0008,0x0008) VR=<CS>   VL=<0x14>   <ORIGINAL\PRIMARY\MPR>
(0x0008,0x0016) VR=<UI>   VL=<0x001a> <1.2.840.10008.5.1.4.1.1.7>
(0x0009,0x0010) VR=<LO>   VL=<0x001a> <CREATOR_THAT_DOESNT_EXIST > 
(0x0009,0x1010) VR=<LO>   VL=<0x000e> <Test VL of LO>
(0x0009,0x1011) VR=<US>   VL=<0x0002> [0x00]
EOF

if [ "$mode" = "compare" ]; then comparebinaries private.ele.nm ; fi

$ANDUMP $WHERE/private.ele.nm > $WHERE/private.ele.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.andump ; fi
$DCDUMP  $WHERE/private.ele.nm > $WHERE/private.ele.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.dcdump ; fi
od -b $WHERE/private.ele.nm > $WHERE/private.ele.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.odx ; fi
od -c $WHERE/private.ele.nm > $WHERE/private.ele.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.odc ; fi

$DCCP $WHERE/private.ele.nm $WHERE/private.ele.nm.cp.ile.nm -n -nodisclaimer > $WHERE/private.ele.nm.cp.ile.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries private.ele.nm.cp.ile.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ile.nm.dccp ; fi
$ANDUMP $WHERE/private.ele.nm.cp.ile.nm > $WHERE/private.ele.nm.cp.ile.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ile.nm.andump ; fi
$DCDUMP  $WHERE/private.ele.nm.cp.ile.nm > $WHERE/private.ele.nm.cp.ile.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ile.nm.dcdump ; fi
od -b $WHERE/private.ele.nm.cp.ile.nm > $WHERE/private.ele.nm.cp.ile.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ile.nm.odx ; fi
od -c $WHERE/private.ele.nm.cp.ile.nm > $WHERE/private.ele.nm.cp.ile.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ile.nm.odc ; fi

$DCCP $WHERE/private.ele.nm $WHERE/private.ele.nm.cp.ele.nm -n -vr explicit -endian little -nodisclaimer > $WHERE/private.ele.nm.cp.ele.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries private.ele.nm.cp.ele.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ele.nm.dccp ; fi
$ANDUMP $WHERE/private.ele.nm.cp.ele.nm > $WHERE/private.ele.nm.cp.ele.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ele.nm.andump ; fi
$DCDUMP  $WHERE/private.ele.nm.cp.ele.nm > $WHERE/private.ele.nm.cp.ele.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ele.nm.dcdump ; fi
od -b $WHERE/private.ele.nm.cp.ele.nm > $WHERE/private.ele.nm.cp.ele.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ele.nm.odx ; fi
od -c $WHERE/private.ele.nm.cp.ele.nm > $WHERE/private.ele.nm.cp.ele.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.cp.ele.nm.odc ; fi


# Insert UN VR directly into explicit stream ...

$ANCREATE <<EOF > $WHERE/private.ele.nm.un -explicit
(0x0008,0x0008) VR=<CS>   VL=<0x14>   <ORIGINAL\PRIMARY\MPR>
(0x0008,0x0016) VR=<UI>   VL=<0x001a> <1.2.840.10008.5.1.4.1.1.7>
(0x0009,0x0010) VR=<LO>   VL=<0x001a> <CREATOR_THAT_DOESNT_EXIST > 
(0x0009,0x1010) VR=<UN>   VL=<0x000e> <Test VL of LO>
(0x0009,0x1011) VR=<UN>   VL=<0x0002> [0x00]
EOF

if [ "$mode" = "compare" ]; then comparebinaries private.ele.nm.un ; fi

$ANDUMP $WHERE/private.ele.nm.un > $WHERE/private.ele.nm.un.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.andump ; fi
$DCDUMP  $WHERE/private.ele.nm.un > $WHERE/private.ele.nm.un.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.dcdump ; fi
od -b $WHERE/private.ele.nm.un > $WHERE/private.ele.nm.un.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.odx ; fi
od -c $WHERE/private.ele.nm.un > $WHERE/private.ele.nm.un.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.odc ; fi

$DCCP $WHERE/private.ele.nm.un $WHERE/private.ele.nm.un.cp.ile.nm -n -nodisclaimer > $WHERE/private.ele.nm.un.cp.ile.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries private.ele.nm.un.cp.ile.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ile.nm.dccp ; fi
$ANDUMP $WHERE/private.ele.nm.un.cp.ile.nm > $WHERE/private.ele.nm.un.cp.ile.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ile.nm.andump ; fi
$DCDUMP  $WHERE/private.ele.nm.un.cp.ile.nm > $WHERE/private.ele.nm.un.cp.ile.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ile.nm.dcdump ; fi
od -b $WHERE/private.ele.nm.un.cp.ile.nm > $WHERE/private.ele.nm.un.cp.ile.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ile.nm.odx ; fi
od -c $WHERE/private.ele.nm.un.cp.ile.nm > $WHERE/private.ele.nm.un.cp.ile.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ile.nm.odc ; fi

$DCCP $WHERE/private.ele.nm.un $WHERE/private.ele.nm.un.cp.ele.nm -n -vr explicit -endian little -nodisclaimer > $WHERE/private.ele.nm.un.cp.ele.nm.dccp 2>&1
if [ "$mode" = "compare" ]; then comparebinaries private.ele.nm.un.cp.ele.nm ; fi
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ele.nm.dccp ; fi
$ANDUMP $WHERE/private.ele.nm.un.cp.ele.nm > $WHERE/private.ele.nm.un.cp.ele.nm.andump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ele.nm.andump ; fi
$DCDUMP  $WHERE/private.ele.nm.un.cp.ele.nm > $WHERE/private.ele.nm.un.cp.ele.nm.dcdump 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ele.nm.dcdump ; fi
od -b $WHERE/private.ele.nm.un.cp.ele.nm > $WHERE/private.ele.nm.un.cp.ele.nm.odx 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ele.nm.odx ; fi
od -c $WHERE/private.ele.nm.un.cp.ele.nm > $WHERE/private.ele.nm.un.cp.ele.nm.odc 2>&1
if [ "$mode" = "compare" ]; then comparedumps    private.ele.nm.un.cp.ele.nm.odc ; fi

if [ "$mode" = "compare" ]
then
	rm -rf "$TMPROOT"
fi

echo $0 finished
