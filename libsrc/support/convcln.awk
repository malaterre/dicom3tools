#
# Clean a conversion template by converting offset to dec from 0
# in each block
#

function hextodec(hex) {
		gsub("x","0",hex);
		gsub("X","0",hex);
		n=length(hex);
		dec=0;
		i=1;
		while (i <= n) {
			digit=substr(hex,i,1);
			if (digit == "a") digit=10;
			else if (digit == "b") digit=11;
			else if (digit == "c") digit=12;
			else if (digit == "d") digit=13;
			else if (digit == "e") digit=14;
			else if (digit == "f") digit=15;
			else if (index("0123456789",digit) == 0) break;
			dec=dec*16+digit;
			++i;
		}
		return dec;
}

function octtodec(oct) {
		n=length(oct);
		dec=0;
		i=1;
		while (i <= n) {
			digit=substr(oct,i,1);
			if (index("01234567",digit) == 0) break;
			dec=dec*8+digit;
			++i;
		}
		return dec;
}

function anytodec(any) {
		if (substr(any,1,1) == "0") {
			if (substr(any,2,1) == "x" || substr(any,2,1) == "X")
				return hextodec(any)
			else
				return octtodec(any)
		}
		else
			return any
}

NR==1	{
	blockoffset=0
	lastblock=""
}

!/^block/ { print }

/^block/{
	block=""
	if (match($0,"block=\"[^\"]*\""))
		block=substr($0,RSTART+length("block=\""),
			RLENGTH-length("block=\"")-1);
	offset=""
	if (match($0,"offset=\"[^\"]*\""))
		offset=anytodec(substr($0,RSTART+length("offset=\""),
			RLENGTH-length("offset=\"")-1));

	ok="yes";

	if (block == "" || block == "?" ) {
		print "Line " FNR ": error - no block" >"/dev/tty"
		ok="no";
	}
	if (offset == "" || offset == "?" ) {
		print "Line " FNR ": error - no offset" >"/dev/tty"
		ok="no";
	}

	if (block != lastblock) {
		if (offset != 0) {
			print "Block " block ": add " offset " (dec) to origin" >"/dev/tty"
		}
		blockoffset=offset
	}

	if (blockoffset != 0) offset-=blockoffset

	sub("offset=\"[^\"]*\"","offset=\"" offset "\"")
	print

	lastblock=block
}

