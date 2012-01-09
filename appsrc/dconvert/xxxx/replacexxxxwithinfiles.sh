#!/bin/sh
#
# Usage: find . -name '*.h'   -exec ./replacexxxxwithinfiles.sh '{}' ';'
# Usage: find . -name '*.cc'  -exec ./replacexxxxwithinfiles.sh '{}' ';'
# Usage: find . -name '*.tpl' -exec ./replacexxxxwithinfiles.sh '{}' ';'
# Usage: find . -name 'Imakefile' -exec ./replacexxxxwithinfiles.sh '{}' ';'
#
# (using -o in same find fails to exec)

filename="${1}"
backupfilename="${filename}.bak"

rm -f "${backupfilename}"
mv "${filename}" "${backupfilename}"

sed -e 's/xxxx/yyyy/g' -e 's/XXXX/YYYY/g' <"${backupfilename}" >"${filename}"

rm "${backupfilename}"
