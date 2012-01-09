#!/bin/sh

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

baseappname=binpatch


echo "hello world" > "$TMPROOT.src"

for v in \
	1.16 \
	2.16 \
	3.16 \
	4.16 \
	1.-16 \
	2.-16 \
	3.-16 \
	4.-16 \
	1.0x7f \
	1.0xff \
	1.0x7fff \
	2.0x7fff \
	3.0x7fff \
	4.0x7fff \
	1.0xffff \
	2.0xffff \
	3.0xffff \
	4.0xffff \
	4.0x7fffff \
	4.0xffffff \
	4.0x7fffffff \
	4.0xffffffff
do
	for endian in l b
	do

		comparename="$testlogdir/$baseappname.$v.$endian.dst"

		if [ "$mode" = "compare" ]
		then
			dst="$TMPROOT.$v.$endian.dst"
		else
			dst="$comparename"
		fi

		length=`echo "$v" | sed 's/^\([0-9]*\)[.].*$/\1/'`
		value=`echo "$v" | sed 's/^[0-9]*[.]\(.*\)$/ \1/'`

		echo binpatch "$length" "$value" "$endian"
		./binpatch "$TMPROOT.src" "$dst" -i 1 "$length" "$value" "$endian"
		od -x "$dst" >"$dst.od"

		if [ "$mode" = "compare" ]
		then
			if [ -f "$comparename" ]
			then
				echo "$baseappname:   comparing binary"
				cmp "$dst" "$comparename"
			else
				echo "$baseappname:     nothing stored  to compare binary"
			fi
			rm "$dst"

			if [ -f "$comparename.od" ]
			then
				echo "$baseappname:   comparing od"
				diff "$dst.od" "$comparename.od"
			else
				echo "$baseappname:     nothing stored  to compare od"
				cat "$dst.od"
			fi
			rm "$dst.od"
		fi
	done
done

rm -f "$TMPROOT.src"
