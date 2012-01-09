# Offsets are in bytes from 0
# Lengths are in intype units

block="STDHDR"	offset="0"	intype="String"	inlength="4"	keyword="?"	dicomtype="?"	dicomtag="?"			# comment
block="STDHDR"	offset="66"	intype="String"	inlength="2"	keyword="?"	dicomtype="?"	dicomtag="?"	enum="'SP'=Supine,'PR'=Prone"	# comment

block="STDHDR"	offset="116"	intype="String_Date_YMD"	inlength="10"	keyword="?"	dicomtype="?"	dicomtag="?"	# yyyy/mm/dd
block="STDHDR"	offset="126"	intype="String_Time"		inlength="12"	keyword="?"	dicomtype="?"	dicomtag="?"	# hh/mm/ss.xxx
block="STDHDR"	offset="210"	intype="Time_Milliseconds_B"	inlength="1"	keyword="?"	dicomtype="?"	dicomtag="?"	# 

block="STDHDR"	offset="306"	intype="Uint8"		inlength="1"	keyword="?"	dicomtype="?"	dicomtag="?"	enum="1=X,2=Y"	# 
block="STDHDR"	offset="380"	intype="Uint16_B"	inlength="1"	keyword="?"	dicomtype="?"	dicomtag="?"	bitmap="0:N,X;1:N,Y"	# 
block="STDHDR"	offset="382"	intype="Uint32_B"	inlength="1"	keyword="?"	dicomtype="?"	dicomtag="?"		# 

block="IMGHDR"	offset="84"	intype="IEEE_Float32_B"	inlength="1"	keyword="?"	dicomtype="?"	dicomtag="?"		# 

block="IMGHDR"	offset="112"	intype="String"			inlength="4"	keyword="?"	dicomtype="DS"	dicomtag="SliceThickness"	# 
block="IMGHDR2"	offset="4"	intype="Time_Milliseconds_B"	inlength="1"	keyword="?"	dicomtype="TM"	dicomtag="TimeOfLastCalibration"	# 
block="IMGHDR2"	offset="96"	intype="Uint16_B"		inlength="1"	keyword="?"	dicomtype="US"	dicomtag="Columns"	# 

constant="0" 		dicomtype="US"	dicomtag="PixelPaddingValue"		# 
constant="MONOCHROME2" 	dicomtype="CS"	dicomtag="PhotometricInterpretation"	# 
constant="0" 		dicomtype="DS"	dicomtag="RescaleIntercept"		# 
constant="" 		dicomtype="PN"	dicomtag="ReferringPhysicianName"	# 

