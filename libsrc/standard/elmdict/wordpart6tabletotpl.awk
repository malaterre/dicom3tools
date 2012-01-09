{FS="|"; keyword=$4; gsub("[ -]*","",keyword); print $1 " VERS=\"3MR\"  VR=\"" $2 "\"   VM=\"" $3 "\"	Keyword=\"" keyword "\"	Name=\"" $5 "\"";}
