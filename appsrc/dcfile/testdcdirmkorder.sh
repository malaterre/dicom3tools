#!/bin/sh

IMGDIR="../../images/dicom/forjpeg2000/discimg"
OUTDIR="/tmp"

US=`pwd`
cd $IMGDIR

for i in pcs psc scp spc csp cps
do
	echo '================================' $i '================================'
	$US/dcdirmk [CN-Z]* > "$OUTDIR/DICOMDIR.$i" -dirrecorder $i
	$US/dciodvfy "$OUTDIR/DICOMDIR.$i"
	$US/dcdirdmp "$OUTDIR/DICOMDIR.$i" >"$OUTDIR/DICOMDIR.$i.dirdump" 2>&1
	$US/dcdirdmp "$OUTDIR/DICOMDIR.$i" -showrecordinfo >"$OUTDIR/DICOMDIR.$i.dirdumpvv" 2>&1
	$US/dcdump "$OUTDIR/DICOMDIR.$i" 2>&1 | grep 'Directory Record Type' >"$OUTDIR/DICOMDIR.$i.dumprectype"
	if [ $i != "pcs" ]
	then
		echo "Should be no differences ..."
		diff "$OUTDIR/DICOMDIR.pcs.dirdump" "$OUTDIR/DICOMDIR.$i.dirdump"
		echo "Should be different record offsets ..."
		diff "$OUTDIR/DICOMDIR.pcs.dirdumpvv" "$OUTDIR/DICOMDIR.$i.dirdumpvv"
		echo "Should be different orders ..."
		cmp "$OUTDIR/DICOMDIR.pcs.dumprectype" "$OUTDIR/DICOMDIR.$i.dumprectype"
	fi
done

