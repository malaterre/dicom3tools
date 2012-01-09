# create list of composite iod elements by module 

NR==1	{
	module=""
}

/^[ 	]*Module=/ || /^[ 	]*DefineMacro=/ {

	module=""
	if (match($0,"Module=\"[^\"]*\""))
		module=substr($0,RSTART+length("Module=\""),
			RLENGTH-length("Module=\"")-1);
	else if (match($0,"DefineMacro=\"[^\"]*\""))
		module=substr($0,RSTART+length("DefineMacro=\""),
			RLENGTH-length("DefineMacro=\"")-1);

	sequencenestingdepth=0;
}

/^[ 	]*ModuleEnd/ || /^[ 	]*MacroEnd/{

	module=""
}

/^[ 	]*Name.*[ 	][ 	]*Type=/ {

	name=""
	if (match($0,"Name=\"[^\"]*\""))
		name=substr($0,RSTART+length("Name=\""),
			RLENGTH-length("Name=\"")-1);

	type=""
	if (match($0,"Type=\"[^\"]*\""))
		type=substr($0,RSTART+length("Type=\""),
			RLENGTH-length("Type=\"")-1);

	condition=""
	if (match($0,"Condition=\"[^\"]*\""))
		condition=substr($0,RSTART+length("Condition=\""),
			RLENGTH-length("Condition=\"")-1);

	if (sequencenestingdepth == 0) {
		printf("%35s  %-40s %-3s %-50s\n",module,name,type,condition);
	}

}

/^[ 	]*Sequence=/ {
	++sequencenestingdepth
}

/^[ 	]*SequenceEnd/ {
	--sequencenestingdepth
}

END {
}

