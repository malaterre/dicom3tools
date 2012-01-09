# Usage:	mv ge9800.tpl ge9800.tpl.bak; awk -F\t -f doubleoffset.awk <ge9800.tpl.bak >ge9800.tpl; rm ge9800.tpl.bak

	{
		if (match($0,"offset=") > 0) {
			#print $2
			if (match($2,"[0-9]+") > 0) {
				offsetvalue = substr($2,RSTART,RLENGTH);
				#print $2 " = " offsetvalue;
				newoffsetvalue = offsetvalue * 2;
				replaceoffsetstring = "offset=\"" newoffsetvalue "\"";
				#print $2 " = " offsetvalue " ; " replaceoffsetstring;
				sub($2,replaceoffsetstring,$0);
				print $0;
			}
		}
		else {
			print $0
		}
	}
