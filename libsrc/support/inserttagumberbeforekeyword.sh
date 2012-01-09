#!/bin/sh
#
# invoke as xargs -L 1 $0 dictionary
#

#DICTIONARY="elmdict/dicom3.tpl"
DICTIONARY="$1"
shift

srcLine="$*"
keyword=`echo "${srcLine}" | awk -F, '{print $1}'`
dictionaryLine=`grep Keyword=\"${keyword}\" ${DICTIONARY} | head -1`
tag=`echo "${dictionaryLine}" | awk '{print $1}'`
#vm=`echo "${srcLine}" | awk -F, '{print $7}'`
#if [ -z "${vm}" ]
#then
#	vm=`echo "${dictionaryLine}" | sed -e 's/^.*VM="\([^"]*\)".*$/\1/'`
#	echo "\"${tag}\",${srcLine}${vm}"
#else
	echo "\"${tag}\",${srcLine}"
#fi
