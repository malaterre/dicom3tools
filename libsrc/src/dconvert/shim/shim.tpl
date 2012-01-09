# Offsets are in bytes from 0
# Lengths are in intype units

block="MAINHDR"	offset="0"   intype="String"   inlength="8"  keyword="ZASYSID"	dicomtype="?"	dicomtag="?"	# SYS-ID
block="MAINHDR"	offset="8"   intype="String"   inlength="16" keyword="ZANAME"	dicomtype="PN"	dicomtag="PatientName"	# Name
block="MAINHDR"	offset="24"  intype="String"   inlength="12" keyword="ZAID"	dicomtype="LO"	dicomtag="PatientID"	# ID
block="MAINHDR"	offset="36"  intype="String"   inlength="2"  keyword="ZASEX"	dicomtype="CS"	dicomtag="PatientSex"	enum="'M'=Male,'F'=Female"	# Sex
block="MAINHDR"	offset="38"  intype="String"   inlength="4"  keyword="ZAAGE"	dicomtype="?"	dicomtag="?"	# AGE years
block="MAINHDR"	offset="42"  intype="String"   inlength="2"  keyword="ZACOMM"	dicomtype="LT"	dicomtag="StudyComments"	# Comment
block="MAINHDR"	offset="62"  intype="String"   inlength="18" keyword="ZAHOSP"	dicomtype="LO"	dicomtag="InstitutionName"	# Hospital
block="MAINHDR"	offset="80"  intype="String"   inlength="8"  keyword="ZADATW"	dicomtype="?"	dicomtag="?"	# Date 'YY-MM-DD'
block="MAINHDR"	offset="88"  intype="String"   inlength="8"  keyword="ZATIME"	dicomtype="?"	dicomtag="?"	# Time 'HH:MM:SS'
block="MAINHDR"	offset="96"  intype="String"   inlength="8"  keyword="ZAAREA"	dicomtype="?"	dicomtag="?"	# Zoom Size/NSlices 'n.nX/nn' enum='ES'=100mm,'VS'=150mm,'SS'=200mm,' S'=250mm,' M'=300mm,' L'=350mm,'LL'=400mm	
block="MAINHDR"	offset="104" intype="String"   inlength="2"  keyword="ZASEQT"	dicomtype="?"	dicomtag="?"	enum="'IR'=Inversion Recovery,'SE'=Spin Echo,'FE'=Field Echo"	# TYPE/MODE 'XX/nnn' where nnn is Max(Nx,Ny)
block="MAINHDR"	offset="107" intype="String"   inlength="5"  keyword="ZASEQM"	dicomtype="?"	dicomtag="?"	# TYPE/MODE 'XX/nnn' where nnn is Max(Nx,Ny)
block="MAINHDR"	offset="112" intype="String"   inlength="8"  keyword="ZATR"	dicomtype="?"	dicomtag="?"	# TR mS 'TR=nnnn'
block="MAINHDR"	offset="120" intype="String"   inlength="8"  keyword="ZATE"	dicomtype="?"	dicomtag="?"	# TE mS 'TE=nnn'
block="MAINHDR"	offset="128" intype="String"   inlength="8"  keyword="ZATI"	dicomtype="?"	dicomtag="?"	# TI mS 'TI=nnn'
block="MAINHDR"	offset="136" intype="String"   inlength="8"  keyword="ZALOK"	dicomtype="?"	dicomtag="?"	# Location mm 'OM+nnn','CN+nn'
block="MAINHDR"	offset="144" intype="String"   inlength="8"  keyword="ZAECG"	dicomtype="?"	dicomtag="?"	# ECG 'ECG=nnn'
block="MAINHDR"	offset="152" intype="String"   inlength="6"  keyword="ZADIRL"	dicomtype="?"	dicomtag="?"	enum="'RIGHT '=Right,'LEFT  '=Left,'ANTR  '=Anterior,'POSTR '=Posterior,'HEAD  '=Head,'FEET  '=Feet"	# Left side of image
block="MAINHDR"	offset="158" intype="String"   inlength="6"  keyword="ZADIRB"	dicomtype="?"	dicomtag="?"	enum="'RIGHT '=Right,'LEFT  '=Left,'ANTR  '=Anterior,'POSTR '=Posterior,'HEAD  '=Head,'FEET  '=Feet"	# Bottom side of image
block="MAINHDR"	offset="164" intype="String"   inlength="4"  keyword="ZATCS"	dicomtype="?"	dicomtag="?"	enum="'TRN '=Transverse,'COR '=Coronal,'SAG '=Sagittal"	# 
block="MAINHDR"	offset="168" intype="String"   inlength="8"  keyword="ZAMTE"	dicomtype="?"	dicomtag="?"	# Interslice rest ms 'MTE=nn'
block="MAINHDR"	offset="176" intype="String"   inlength="6"  keyword="ZAPIT"	dicomtype="?"	dicomtag="?"	# Interslice skip mm 'PT=nn'
block="MAINHDR"	offset="182" intype="String"   inlength="8"  keyword="ZAFLIP"	dicomtype="?"	dicomtag="?"	# Flip angle degress 'FLIP=nn'
block="MAINHDR"	offset="190" intype="String"   inlength="8"  keyword="ZAENCD"	dicomtype="?"	dicomtag="?"	# Encode % 'ECD=nn%'
block="MAINHDR"	offset="198" intype="String"   inlength="6"  keyword="ZADIREP"	dicomtype="?"	dicomtag="?"	enum="'(P,F) '=?,'(F,P) '=?"	# 
block="MAINHDR"	offset="204" intype="String"   inlength="8"  keyword="ZANSEQ"	dicomtype="SH"	dicomtag="SequenceName"	# Sequence Name
block="MAINHDR"	offset="212" intype="String"   inlength="8"  keyword="ZAMATER1"	dicomtype="?"	dicomtag="?"	# SYSTEM-ID 1
block="MAINHDR"	offset="220" intype="String"   inlength="8"  keyword="ZAMATER2"	dicomtype="?"	dicomtag="?"	# SYSTEM-ID 2
block="MAINHDR"	offset="228" intype="String"   inlength="8"  keyword="ZAMATER3"	dicomtype="?"	dicomtag="?"	# SYSTEM-ID 3
block="MAINHDR"	offset="236" intype="String"   inlength="2"  keyword="ZASITE"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="238" intype="String"   inlength="8"  keyword="ZAAVG"	dicomtype="?"	dicomtag="?"	# Averages 'NEX=n'
block="MAINHDR"	offset="246" intype="String"   inlength="2"  keyword="ZADIRL2"	dicomtype="?"	dicomtag="?"	enum="'RT'=Right,'LT'=Left,'HD'=Head,'FT'=Feet,'PO'=Posterior,'AN'=Anterior"	# Left direction
block="MAINHDR"	offset="248" intype="String"   inlength="2"  keyword="ZADIRB2"	dicomtype="?"	dicomtag="?"	# Down direction
block="MAINHDR"	offset="250" intype="String"   inlength="6"  name="Unknown250"	dicomtype="?"	dicomtag="?"	# SCAN (?)
block="MAINHDR"	offset="256" intype="Int16_B"  inlength="1"  keyword="ZMAGN"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="258" intype="Int16_B"  inlength="1"  keyword="ZSCSW"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="260" intype="Int16_B"  inlength="1"  keyword="ZIMAGE"	dicomtype="IS"	dicomtag="ImagesInAcquisition"	# Images in series
block="MAINHDR"	offset="262" intype="Int16_B"  inlength="1"  keyword="ZSLICE"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="264" intype="Int16_B"  inlength="1"  keyword="ZECHO"	dicomtype="?"	dicomtag="?"	# 1,2,4
block="MAINHDR"	offset="266" intype="Int16_B"  inlength="1"  keyword="ZXSIZE"	dicomtype="US"	dicomtag="Columns"	#
block="MAINHDR"	offset="268" intype="Int16_B"  inlength="1"  keyword="ZYSIZE"	dicomtype="US"	dicomtag="Rows"	#
block="MAINHDR"	offset="270" intype="Int16_B"  inlength="1"  keyword="ZTBLK"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="272" intype="Int16_B"  inlength="1"  keyword="ZNEGSW"	dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	# Even line DC neg
block="MAINHDR"	offset="274" intype="Int16_B"  inlength="1"  keyword="ZFTYPE"	dicomtype="?"	dicomtag="?"	enum="0=0,1=1,2=2,3=1+2"	#
block="MAINHDR"	offset="276" intype="Int16_B"  inlength="1"  keyword="ZCALB"	dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	# Max
block="MAINHDR"	offset="278" intype="Int16_B"  inlength="1"  keyword="ZLBLK"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="280" intype="Int16_B"  inlength="1"  keyword="ZRBLK"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="282" intype="Int16_B"  inlength="1"  keyword="ZIBLKA"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="284" intype="Int16_B"  inlength="1"  keyword="ZCOIL"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="286" intype="Int16_B"  inlength="1"  keyword="ZDTYPE"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="288" intype="Int16_B"  inlength="1"  keyword="ZETYPE"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="290" intype="Int16_B"  inlength="1"  keyword="ZKRNL"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="292" intype="Int16_B"  inlength="1"  keyword="ZPTOR1"	dicomtype="?"	dicomtag="?"	enum="0=head first,1=feet first"	#
block="MAINHDR"	offset="294" intype="Int16_B"  inlength="1"  keyword="ZPTOR2"	dicomtype="?"	dicomtag="?"	enum="0=supine,1=prone,2=right side down,3=left side down,4=other"	#
block="MAINHDR"	offset="296" intype="Int16_B"  inlength="1"  keyword="ZLRVS"	dicomtype="?"	dicomtag="?"	enum="0=Up,1=Down"	#
block="MAINHDR"	offset="298" intype="Int16_B"  inlength="1"  name="Unknown298"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="300" intype="Int16_B"  inlength="1"  keyword="ZRCDMODE"	dicomtype="?"	dicomtag="?"	# Mode
block="MAINHDR"	offset="302" intype="Int16_B"  inlength="1"  keyword="ZVDBAND"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="304" intype="Int16_B"  inlength="1"  keyword="ZYSTL"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="306" intype="Int16_B"  inlength="1"  keyword="ZPWGHT"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="308" intype="Int16_B"  inlength="1"  keyword="ZPFUSE"	dicomtype="?"	dicomtag="?"	# RF power ratio  %
block="MAINHDR"	offset="310" intype="String"   inlength="2"  keyword="ZSATON"	dicomtype="?"	dicomtag="?"	enum="'P '=Potassium,'H '=Hydrogen,'Na'=Sodium"	#
block="MAINHDR"	offset="312" intype="Int16_B"  inlength="1"  keyword="ZSFREQ"	dicomtype="?"	dicomtag="?"	# 10kHz
block="MAINHDR"	offset="314" intype="Int16_B"  inlength="1"  keyword="ZIMFLT"	dicomtype="?"	dicomtag="?"	enum="0=No"	#
block="MAINHDR"	offset="316" intype="Int16_B"  inlength="1"  keyword="ZSLANT2"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="318" intype="Int16_B"  inlength="1"  keyword="ZROTPHS"	dicomtype="?"	dicomtag="?"	# Recon
block="MAINHDR"	offset="320" intype="Int16_B"  inlength="1"  keyword="ZWIDE"	dicomtype="?"	dicomtag="?"	# Slice width     *10mm
block="MAINHDR"	offset="322" intype="Int16_B"  inlength="1"  keyword="ZPITCH"	dicomtype="?"	dicomtag="?"	# Interslice skip *10mm
block="MAINHDR"	offset="324" intype="Int16_B"  inlength="1"  keyword="ZCLOC"	dicomtype="?"	dicomtag="?"	# Center location *10mm offset from isocenter in ZDIR direction (SAG L+,COR A+,TRN F+) FOR CENTER OF SERIES
block="MAINHDR"	offset="326" intype="Int16_B"  inlength="1"  keyword="ZMAG"	dicomtype="?"	dicomtag="?"	# *100
block="MAINHDR"	offset="328" intype="Int16_B"  inlength="1"  keyword="ZCNTX"	dicomtype="?"	dicomtag="?"	# X mm
block="MAINHDR"	offset="330" intype="Int16_B"  inlength="1"  keyword="ZCNTY"	dicomtype="?"	dicomtag="?"	# Y mm
block="MAINHDR"	offset="332" intype="Int16_B"  inlength="1"  keyword="ZVIEW"	dicomtype="?"	dicomtag="?"	# *10
block="MAINHDR"	offset="334" intype="Int16_B"  inlength="1"  keyword="ZDIR"	dicomtype="?"	dicomtag="?"	enum="1=Trn,2=Cor,3=Sag,6=Non,7=Point"	# Orientation
block="MAINHDR"	offset="336" intype="Int16_B"  inlength="1"  keyword="ZANG1"	dicomtype="?"	dicomtag="?"	# Alpha *10 degrees (tilt angle of vertical, up-to-down direction relative to base plane (C,S,T))
block="MAINHDR"	offset="338" intype="Int16_B"  inlength="1"  keyword="ZANG2"	dicomtype="?"	dicomtag="?"	# Beta *10 degrees (tilt angle of horizontal, left-to-right direction)
block="MAINHDR"	offset="340" intype="String"   inlength="8"  keyword="ZPSID1"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="348" intype="String"   inlength="2"  keyword="ZPSIM1"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="350" intype="String"   inlength="2"  keyword="ZPSCR1"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="352" intype="String"   inlength="8"  keyword="ZPSID2"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="360" intype="String"   inlength="2"  keyword="ZPSIM2"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="362" intype="String"   inlength="2"  keyword="ZPSCR2"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="364" intype="Int16_B"  inlength="1"  keyword="ZANG3S"	dicomtype="?"	dicomtag="?"	# system *10 degrees
block="MAINHDR"	offset="366" intype="Int16_B"  inlength="1"  keyword="ZANG3U"	dicomtype="?"	dicomtag="?"	# user *10 degrees
block="MAINHDR"	offset="368" intype="Int16_B"  inlength="1"  keyword="ZTILTSW"	dicomtype="?"	dicomtag="?"	enum="1=1,2=2,3=Trn,4=Cor"	# Tilt
block="MAINHDR"	offset="370" intype="Int16_B"  inlength="1"  keyword="ZPSOR1"	dicomtype="?"	dicomtag="?"	# location +/-512
block="MAINHDR"	offset="372" intype="Int16_B"  inlength="1"  keyword="ZPSOA1"	dicomtype="?"	dicomtag="?"	# location *10 degrees
block="MAINHDR"	offset="374" intype="Int16_B"  inlength="1"  keyword="ZPSOR2"	dicomtype="?"	dicomtag="?"	# location +/-512
block="MAINHDR"	offset="376" intype="Int16_B"  inlength="1"  keyword="ZPSOA2"	dicomtype="?"	dicomtag="?"	# location *10 degrees
block="MAINHDR"	offset="378" intype="Int16_B"  inlength="1"  keyword="ZPSOD1"	dicomtype="?"	dicomtag="?"	# loc 0-100mm
block="MAINHDR"	offset="380" intype="Int16_B"  inlength="1"  keyword="ZPSOD2"	dicomtype="?"	dicomtag="?"	# loc 0-100mm
block="MAINHDR"	offset="382" intype="Int16_B"  inlength="1"  keyword="ZSLANT"	dicomtype="?"	dicomtag="?"	# orthog=10000
block="MAINHDR"	offset="384" intype="String"   inlength="2"  keyword="ZPMOD"	dicomtype="?"	dicomtag="?"	enum="'IR'=Inversion Recovery,'SE'=Spin Echo,'FE'=Field Echo"	#
block="MAINHDR"	offset="386" intype="Int16_B"  inlength="1"  keyword="ZTR"	dicomtype="?"	dicomtag="?"	# TR ms
block="MAINHDR"	offset="388" intype="Int16_B"  inlength="1"  keyword="ZTE"	dicomtype="?"	dicomtag="?"	# TE ms
block="MAINHDR"	offset="390" intype="Int16_B"  inlength="1"  keyword="ZTI"	dicomtype="?"	dicomtag="?"	# TI ms
block="MAINHDR"	offset="392" intype="Int16_B"  inlength="1"  keyword="ZNX"	dicomtype="?"	dicomtag="?"	# X Encode
block="MAINHDR"	offset="394" intype="Int16_B"  inlength="1"  keyword="ZNY"	dicomtype="?"	dicomtag="?"	# Y Encode
block="MAINHDR"	offset="396" intype="Int16_B"  inlength="1"  keyword="ZXAV"	dicomtype="?"	dicomtag="?"	# X NEX
block="MAINHDR"	offset="398" intype="Int16_B"  inlength="1"  keyword="ZYAV"	dicomtype="?"	dicomtag="?"	# Y NEX
block="MAINHDR"	offset="400" intype="Int16_B"  inlength="1"  keyword="ZEAV"	dicomtype="?"	dicomtag="?"	# Echo
block="MAINHDR"	offset="402" intype="Int16_B"  inlength="1"  keyword="ZSTIM"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="404" intype="String"   inlength="2"  keyword="ZCONST"	dicomtype="?"	dicomtag="?"	enum="'2D'=2D,'3D'=3D"	#
block="MAINHDR"	offset="406" intype="Int16_B"  inlength="1"  keyword="ZFLIP"	dicomtype="?"	dicomtag="?"	# degrees
block="MAINHDR"	offset="408" intype="Int16_B"  inlength="1"  keyword="ZRESP"	dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	#
block="MAINHDR"	offset="410" intype="Int16_B"  inlength="1"  keyword="ZECG"	dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	#
block="MAINHDR"	offset="412" intype="Int16_B"  inlength="1"  keyword="ZDIXON "	dicomtype="?"	dicomtag="?"	# Chemical Shift *10ppm
block="MAINHDR"	offset="414" intype="Int16_B"  inlength="1"  keyword="ZMTE"	dicomtype="?"	dicomtag="?"	# Interslice rest (mS ?)
block="MAINHDR"	offset="416" intype="Int16_B"  inlength="1"  keyword="ZHALF"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="418" intype="Int16_B"  inlength="1"  keyword="ZSGNSW"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="420" intype="Int16_B"  inlength="1"  keyword="ZCINE"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="422" intype="Int16_B"  inlength="1"  keyword="ZPKZ"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="424" intype="Int16_B"  inlength="1"  keyword="ZTEALN"	dicomtype="?"	dicomtag="?"	enum="0=In phase,1=Opposed"	# TE Align
block="MAINHDR"	offset="426" intype="Int16_B"  inlength="1"  keyword="ZCSSW"	dicomtype="?"	dicomtag="?"	# Chemical Shift Switch
block="MAINHDR"	offset="428" intype="Int16_B"  inlength="1"  keyword="ZIMDIR"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="430" intype="Int16_B"  inlength="1"  keyword="ZPSSP1"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="432" intype="Int16_B"  inlength="1"  keyword="ZPSSP2"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="434" intype="Int16_B"  inlength="1"  keyword="ZXWD"	dicomtype="?"	dicomtag="?"	# X Dimension *10mm
block="MAINHDR"	offset="436" intype="Int16_B"  inlength="1"  keyword="ZXLOC1"	dicomtype="?"	dicomtag="?"	# *10mm
block="MAINHDR"	offset="438" intype="Int16_B"  inlength="1"  keyword="ZXLOC2"	dicomtype="?"	dicomtag="?"	# *10mm
block="MAINHDR"	offset="440" intype="Int16_B"  inlength="1"  keyword="ZYWD"	dicomtype="?"	dicomtag="?"	# Y Dimension *10mm
block="MAINHDR"	offset="442" intype="Int16_B"  inlength="1"  keyword="ZYLOC1"	dicomtype="?"	dicomtag="?"	# *10mm
block="MAINHDR"	offset="444" intype="Int16_B"  inlength="1"  keyword="ZYLOC2"	dicomtype="?"	dicomtag="?"	# *10mm
block="MAINHDR"	offset="446" intype="Int16_B"  inlength="1"  keyword="ZHREV"	dicomtype="?"	dicomtag="?"	# Header Revision
block="MAINHDR"	offset="448" intype="Int16_B"  inlength="1"  keyword="ZSYSMAG"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="450" intype="Int16_B"  inlength="1"  keyword="ZSCOPT"	dicomtype="?"	dicomtag="?"	# Optional Control
block="MAINHDR"	offset="452" intype="Int16_B"  inlength="1"  keyword="ZANGIO"	dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	#
block="MAINHDR"	offset="454" intype="Int16_B"  inlength="1"  name="Unknown454"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="456" intype="Int16_B"  inlength="1"  name="Unknown456"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="458" intype="Int16_B"  inlength="1"  name="Unknown458"	dicomtype="?"	dicomtag="?"	#
block="MAINHDR"	offset="460" intype="Int16_B"  inlength="1"  keyword="ZSXADR"	dicomtype="?"	dicomtag="?"	# blk#
block="MAINHDR"	offset="462" intype="Int16_B"  inlength="1"  keyword="ZSPMOD"	dicomtype="?"	dicomtag="?"	# KSPM
block="MAINHDR"	offset="464" intype="String"   inlength="48" keyword="ZIMINF"	dicomtype="?"	dicomtag="?"	#

constant="0" 			dicomtype="US"	dicomtag="PixelPaddingValue"		# 
constant="16" 			dicomtype="US"	dicomtag="BitsAllocated"		# 
constant="16" 			dicomtype="US"	dicomtag="BitsStored"			# 
constant="15" 			dicomtype="US"	dicomtag="HighBit"			# 
constant="0" 			dicomtype="US"	dicomtag="PixelRepresentation"		# Unsigned
constant="1" 			dicomtype="US"	dicomtag="SamplesPerPixel"		# 
constant="MONOCHROME2" 		dicomtype="CS"	dicomtag="PhotometricInterpretation"	# 

constant=""			dicomtype="DA"	dicomtag="PatientBirthDate"		# 
constant=""			dicomtype="SH"	dicomtag="AccessionNumber"		# 
constant="1"			dicomtype="IS"	dicomtag="SeriesInStudy"		# 
constant="1"			dicomtype="IS"	dicomtag="AcquisitionsInStudy"		# 
constant=""			dicomtype="SH"	dicomtag="StudyID"			#
constant="1"			dicomtype="IS"	dicomtag="SeriesNumber"			# 
constant=""			dicomtype="IS"	dicomtag="InstanceNumber"			# 

constant=""			dicomtype="LO"	dicomtag="DeviceSerialNumber"		# 
constant=""			dicomtype="DS"	dicomtag="PatientWeight"		# 
constant="SHIMADZU"		dicomtype="LO"	dicomtag="Manufacturer"			# 
constant=""			dicomtype="LO"	dicomtag="ManufacturerModelName"	# 

constant=""			dicomtype="CS"	dicomtag="BodyPartExamined"		# 
constant="MR"			dicomtype="CS"	dicomtag="Modality"			# 
