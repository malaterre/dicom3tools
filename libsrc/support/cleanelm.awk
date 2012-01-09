/^\([0-9A-Fa-fxX]/ {

	element=$1

	owner=""
	if (match($0,"Owner=\"[^\"]*\""))
		owner=substr($0,RSTART+length("Owner=\""),
			RLENGTH-length("Owner=\"")-1);

	keyword=""
	if (match($0,"Keyword=\"[^\"]*\""))
		keyword=substr($0,RSTART+length("Keyword=\""),
			RLENGTH-length("Keyword=\"")-1);

	name=""
	if (match($0,"Name=\"[^\"]*\""))
		name=substr($0,RSTART+length("Name=\""),
			RLENGTH-length("Name=\"")-1);

	vers=""
	if (match($0,"VERS=\"[^\"]*\""))
		vers=substr($0,RSTART+length("VERS=\""),
			RLENGTH-length("VERS=\"")-1);

	vr=""
	if (match($0,"VR=\"[^\"]*\""))
		vr=substr($0,RSTART+length("VR=\""),
			RLENGTH-length("VR=\"")-1);

	vm=""
	if (match($0,"VM=\"[^\"]*\""))
		vm=substr($0,RSTART+length("VM=\""),
			RLENGTH-length("VM=\"")-1);

	units=""
	if (match($0,"Units=\"[^\"]*\""))
		units=substr($0,RSTART+length("Units=\""),
			RLENGTH-length("Units=\"")-1);

	definedterms=""
	if (match($0,"DefinedTerms=\"[^\"]*\""))
		definedterms=substr($0,RSTART+length("DefinedTerms=\""),
			RLENGTH-length("DefinedTerms=\"")-1);

	enumeratedvalues=""
	if (match($0,"EnumeratedValues=\"[^\"]*\""))
		enumeratedvalues=substr($0,RSTART+length("EnumeratedValues=\""),
			RLENGTH-length("EnumeratedValues=\"")-1);

	privateblock=""
	if (match($0,"PrivateBlock=\"[^\"]*\""))
		privateblock=substr($0,RSTART+length("PrivateBlock=\""),
			RLENGTH-length("PrivateBlock=\"")-1);

	extras=""
	if (units != "") extras=extras "\tUnits=\"" units "\""
	if (privateblock != "") extras=extras "\tPrivateBlock=\"" privateblock "\""
	if (definedterms != "") extras=extras "\tDefinedTerms=\"" definedterms "\""
	if (enumeratedvalues != "") extras=extras "\tEnumeratedValues=\"" enumeratedvalues "\""

	ownertoprint=""
	if (owner != "") ownertoprint="\tOwner=\"" owner "\""

	print element " VERS=\"" vers "\"\tVR=\"" vr "\"   VM=\"" vm "\"" ownertoprint "\tKeyword=\"" keyword "\"\t\t\tName=\"" name "\"" extras
}

