# create Java from element dictionary template 

NR==1	{
	print "\tprotected void createFullNameByTag() {"
	print ""
	print "\t\tfullNameByTag = new HashMap(100);"
	print ""
	}

/^[ 	]*[#]/	{}

/^[ 	]*[(]/ {

	match($0,"Keyword=\"[^\"]*\"")
	keyword=substr($0,RSTART+length("Keyword=\""),
		RLENGTH-length("Keyword=\"")-1);

	match($0,"Name=\"[^\"]*\"")
	name=substr($0,RSTART+length("Name=\""),
		RLENGTH-length("Name=\"")-1);

	print "\t\tfullNameByTag.put(TagFromName." keyword ",\"" name "\");"
}

END {
	print "\t}"
}

