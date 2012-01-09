# create XML from element dictionary template 

NR==1	{
	print "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
	print "<!DOCTYPE DataDictionary ["
	print "<!ELEMENT DataDictionary (DataElement*)>"
	print "<!ELEMENT DataElement (Group,Element,VR,VMMin,VMMax,Keyword,Name)>"
	print "<!ELEMENT Group (#PCDATA)>"
	print "<!ELEMENT Element (#PCDATA)>"
	print "<!ELEMENT VR (#PCDATA)>"
	print "<!ELEMENT VMMin (#PCDATA)>"
	print "<!ELEMENT VMMax (#PCDATA)>"
	print "<!ELEMENT Keyword (#PCDATA)>"
	print "<!ELEMENT Name (#PCDATA)>"
	print "]>"
	print ""
	print "<DataDictionary xmlns:Dicom=\"http://www.nema.org/medical/dicom\">"
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

	print "\t<DataElement><Group>0x" group "</Group><Element>0x" element "</Element><VR>" vr "</VR><VMMin>" vmmin "</VMMin><VMMax>" vmmax "</VMMax><Keyword>" keyword "</Keyword><Name>" name "</Name></DataElement>"

	}

END {
	print "</DataDictionary>"
}

