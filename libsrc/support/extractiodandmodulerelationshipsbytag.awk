NR==1	{
	gIod=""
	gIe=""
	gModule=""
	gMacro=""
}

/^[ 	]*CompositeIOD=/ {
	gIod=""
	if (match($0,"CompositeIOD=\"[^\"]*\""))
		gIod=substr($0,RSTART+length("CompositeIOD=\""),
			RLENGTH-length("CompositeIOD=\"")-1);
	if (gIod != "") {
		gIodList[gIod]=1
	}
}

/^[ 	]*CompositeIODEnd/ {
	gIod=""
	gModule=""			# we need this, because as we move from IOD template file to Module template file, the overloaded use of Module= changes from invocation to definition
}

/^[ 	]*InformationEntity=/ {
	gIe=""
	if (match($0,"InformationEntity=\"[^\"]*\""))
		gIe=substr($0,RSTART+length("InformationEntity=\""),
			RLENGTH-length("InformationEntity=\"")-1);
	if (gIe != "") {
		gIeList[gIe]=1
	}
}

/^[ 	]*InformationEntityEnd/ {
	gIe=""
}

/^[ 	]*DefineMacro=/ {
	gMacro=""
	if (match($0,"Macro=\"[^\"]*\""))
		gMacro=substr($0,RSTART+length("Macro=\""),
			RLENGTH-length("Macro=\"")-1);
	if (gMacro != "") {
		gMacroList[gMacro]=1
		#print "Defining macro " gMacro >"/dev/stderr"
	}
}

/^[ 	]*MacroEnd/ {
	gMacro=""
}

/^[ 	]*InvokeMacro=/ {
	useMacro=""
	if (match($0,"InvokeMacro=\"[^\"]*\""))
		useMacro=substr($0,RSTART+length("InvokeMacro=\""),
			RLENGTH-length("InvokeMacro=\"")-1);
	if (useMacro != "") {
		#print "Invoking macro " useMacro >"/dev/stderr"
		if (gModule != "") {
			for (tryIOD in gIodList) {
				if ((tryIOD,gModule) in gModulesByIOD_usage) {
					if (useMacro in gMacroList) {
						for (tryMacroAndName in gNamesByMacro_type) {
							if (split(tryMacroAndName,subscripts,SUBSEP) == 2) {
								if (useMacro == subscripts[1]) {
									print subscripts[2] "," tryIOD "," gModule "," gModulesByIOD_usage[tryIOD,gModule] "," gNamesByMacro_path[useMacro,subscripts[2]] "," gNamesByMacro_type[useMacro,subscripts[2]] "," gNamesByMacro_vm[useMacro,subscripts[2]]
									#print "Defining " subscripts[2] " in module " gModule " because of inclusion of " useMacro " (path = " gNamesByMacro_path[useMacro,subscripts[2]] ")" >"/dev/stderr"
								}
							}
						}
					}
					else {
						print "ERROR: undefined macro " useMacro " required in " gModule " in " tryIOD >"/dev/stderr"
					}
				}
			}
		}
		else if (gMacro != "") {
			# we are invoking a macro within another macro definition, so "expand" it 
			if (useMacro in gMacroList) {
				for (tryMacroAndName in gNamesByMacro_type) {
					if (split(tryMacroAndName,subscripts,SUBSEP) == 2) {
						if (useMacro == subscripts[1]) {
							gNamesByMacro_type[gMacro,subscripts[2]]=gNamesByMacro_type[useMacro,subscripts[2]]
							gNamesByMacro_vm[gMacro,subscripts[2]]=gNamesByMacro_vm[useMacro,subscripts[2]]
							gNamesByMacro_path[gMacro,subscripts[2]]= gMacro "/" gNamesByMacro_path[useMacro,subscripts[2]]
							#print "Defining " subscripts[2] " in macro " gMacro " because of inclusion of " useMacro " (path = " gNamesByMacro_path[gMacro,subscripts[2]] ")" >"/dev/stderr"
						}
					}
				}
			}
			else {
				print "ERROR: undefined macro " useMacro " required in " gMacro >"/dev/stderr"
			}
		}
	}
}


/^[ 	]*Module/ {
	gModule=""
	if (match($0,"Module=\"[^\"]*\""))
		gModule=substr($0,RSTART+length("Module=\""),
			RLENGTH-length("Module=\"")-1);

	usage=""
	if (match($0,"Usage=\"[^\"]*\""))
		usage=substr($0,RSTART+length("Usage=\""),
			RLENGTH-length("Usage=\"")-1);

	if (gModule != "") {
		gModuleList[gModule]=1
		if (gIod != "") {
			gModulesByIOD_usage[gIod,gModule]=usage
		}
		# else we are doing the module.tpl file not the iodcomp.tpl file
		# and this is the definition of the module rather than its use
	}
}

/^[ 	]*ModuleEnd/ {
	gModule=""
}

/^[ 	]*(Name|Sequence)=/ {
	nameOrSequence=""
	if (match($0,"Name=\"[^\"]*\""))
		nameOrSequence=substr($0,RSTART+length("Name=\""),
			RLENGTH-length("Name=\"")-1);
	if (match($0,"Sequence=\"[^\"]*\""))
		nameOrSequence=substr($0,RSTART+length("Sequence=\""),
			RLENGTH-length("Sequence=\"")-1);

	type=""
	if (match($0,"Type=\"[^\"]*\""))
		type=substr($0,RSTART+length("Type=\""),
			RLENGTH-length("Type=\"")-1);

	vm=""
	match($0,"VM=\"[^\"]*\"");
	vm=substr($0,RSTART+length("VM=\""),RLENGTH-length("VM=\"")-1);

	if (!match($0,"Condition=\"Never")) {
		if (nameOrSequence != "") {
			if (gModule != "") {
				for (tryIOD in gIodList) {
					if ((tryIOD,gModule) in gModulesByIOD_usage) {
						print nameOrSequence "," tryIOD "," gModule "," gModulesByIOD_usage[tryIOD,gModule] ",," type "," vm
						#print "Adding " nameOrSequence " to module " gModule " in " tryIOD >"/dev/stderr"
					}
				}
			}
			if (gMacro != "") {
				gNamesByMacro_type[gMacro,nameOrSequence]=type
				gNamesByMacro_vm[gMacro,nameOrSequence]=vm
				gNamesByMacro_path[gMacro,nameOrSequence]= gMacro
				#print "Defining " nameOrSequence " in macro " gMacro " (path = " gNamesByMacro_path[gMacro,nameOrSequence] ")" >"/dev/stderr"
			}
		}
	}
}

END {
}

