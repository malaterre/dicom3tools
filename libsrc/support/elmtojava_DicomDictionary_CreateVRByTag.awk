# create Java from element dictionary template 

NR==1	{
	print "\tprotected void createValueRepresentationsByTag() {"
	print ""
	print "\t\tvalueRepresentationsByTag = new HashMap(100);"
	print ""
	}

/^[ 	]*[#]/	{}

/^[ 	]*[(]/ {

	match($0,"[(][0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX],");
	group=substr($0,RSTART+1,4);

	match($0,",[0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX][0-9a-fA-FxX]");
	element=substr($0,RSTART+1,4);

	match($0,"Keyword=\"[^\"]*\"")
	keyword=substr($0,RSTART+length("Keyword=\""),
		RLENGTH-length("Keyword=\"")-1);

	match($0,"Name=\"[^\"]*\"")
	name=substr($0,RSTART+length("Name=\""),
		RLENGTH-length("Name=\"")-1);

	gsub("\&","\\\&amp;",name);
	gsub("'","\\\&apos;",name);
	gsub("\"","\\\&quot;",name);
	gsub("<","\\\&lt;",name);
	gsub(">","\\\&gt;",name);

	match($0,"VR=\"[^\"]*\"");
	vr=substr($0,RSTART+length("VR=\""),RLENGTH-length("VR=\"")-1);

	match($0,"VM=\"[^\"]*\"");
	if (vr == "US or SS") vr= "XS";
	if (vr == "US or OW") vr= "XO";
	if (vr == "US or SS or OW") vr= "XO";
	if (vr == "US\\US or SS\\US") vr = "XS";
	if (vr == "OW/OB") vr = "OX";

	vm=substr($0,RSTART+length("VM=\""),RLENGTH-length("VM=\"")-1);
	if (vm == "") vm=0;
	vmmin=vmmax=vm;
	if (vm == "n") {
		vmmin=1;
		vmmax="n";
	}
	if (match(vm,"-")) {
		match(vm,"[0-9]*-");
		vmmin=substr(vm,RSTART,RLENGTH-1);
		match(vm,"-[0-9n]");
		vmmax=substr(vm,RSTART+1,RLENGTH-1);
		if (vmmax == "n") vmmax="n";
	}

        print "\t\tvalueRepresentationsByTag.put(TagFromName." keyword ",ValueRepresentation." vr ");"
	}

END {
	print "\t}"
	print ""
}

