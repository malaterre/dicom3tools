# Offsets are in 8 bit bytes from 0
# Lengths are in intype words

block="STUDYHDR"	offset="0"		intype="String"		inlength="14"	name="?"	dicomtype="?"	dicomtag="?"				# Study Header Identifier
block="STUDYHDR"	offset="14"		intype="String"		inlength="8"	name="?"	dicomtype="?"	dicomtag="?"				# Study Header Revision Number xx.xx.xx
block="STUDYHDR"	offset="22"		intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Number of Study Header Blocks
block="STUDYHDR"	offset="24"		intype="String"		inlength="32"	name="?"	dicomtype="?"	dicomtag="?"				# Study Header Creator (Process Name:PID)
block="STUDYHDR"	offset="56"		intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Study Header Creator (Task ID)
block="STUDYHDR"	offset="58"		intype="String"		inlength="5"	name="?"	dicomtype="?"	dicomtag="?"				# Original Raw Data Study Number (null unless different from study number)
block="STUDYHDR"	offset="64"		intype="String"		inlength="5"	keyword="StudyNumber"	dicomtype="SH"	dicomtag="StudyID"		# Study Number
block="STUDYHDR"	offset="70"		intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Raw Data System ID
block="STUDYHDR"	offset="74"		intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# System Generation ID
block="STUDYHDR"	offset="78"		intype="String"		inlength="9"	name="?"	dicomtype="?"	dicomtag="?"				# Date of Study (ascii dd-mmm-yy)
block="STUDYHDR"	offset="88"		intype="Int16_B"	inlength="1"	keyword="StudyDateDay"	dicomtype="?"	dicomtag="?"			# Date of Study (integer day)
block="STUDYHDR"	offset="90"		intype="Int16_B"	inlength="1"	keyword="StudyDateMonth"	dicomtype="?"	dicomtag="?"		# Date of Study (integer month)
block="STUDYHDR"	offset="92"		intype="Int16_B"	inlength="1"	keyword="StudyDateYear"	dicomtype="?"	dicomtag="?"			# Date of Study (integer year-1900)
block="STUDYHDR"	offset="94"		intype="String"		inlength="8"	name="?"	dicomtype="?"	dicomtag="?"				# Time of Study (ascii hh:mm:ss)
block="STUDYHDR"	offset="102"	intype="Int16_B"	inlength="1"	keyword="StudyTimeHour"	dicomtype="?"	dicomtag="?"			# Time of Study (integer hr)
block="STUDYHDR"	offset="104"	intype="Int16_B"	inlength="1"	keyword="StudyTimeMin"	dicomtype="?"	dicomtag="?"			# Time of Study (integer min)
block="STUDYHDR"	offset="106"	intype="Int16_B"	inlength="1"	keyword="StudyTimeSec"	dicomtype="?"	dicomtag="?"			# Time of Study (integer sec)
block="STUDYHDR"	offset="108"	intype="String"		inlength="32"	name="?"	dicomtype="PN"	dicomtag="PatientName"			# Patient Name
block="STUDYHDR"	offset="140"	intype="String"		inlength="12"	name="?"	dicomtype="LO"	dicomtag="PatientID"			# Patient ID
block="STUDYHDR"	offset="152"	intype="String"		inlength="4"	name="?"	dicomtype="?"	dicomtag="?"				# Patient ID padding for future exp.
block="STUDYHDR"	offset="156"	intype="String"		inlength="4"	keyword="PatientAge"	dicomtype="?"	dicomtag="?"			# Age of patient - xxx years or xxx[dDwWmMyY]
block="STUDYHDR"	offset="160"	intype="String"		inlength="1"	name="?"	dicomtype="CS"	dicomtag="PatientSex"			# Sex of patient - M or F
block="STUDYHDR"	offset="162"	intype="Int32_B"	inlength="1"	keyword="PatientWeightGrams"	dicomtype="?"	dicomtag="?"		# Weight of the patient in grams
block="STUDYHDR"	offset="166"	intype="String"		inlength="32"	name="?"	dicomtype="PN"	dicomtag="ReferringPhysicianName"	# Referred by
block="STUDYHDR"	offset="198"	intype="String"		inlength="32"	name="?"	dicomtype="PN"	dicomtag="PerformingPhysicianName"	# Diagnostician
block="STUDYHDR"	offset="230"	intype="String"		inlength="32"	name="?"	dicomtype="PN"	dicomtag="OperatorsName"			# Operator
block="STUDYHDR"	offset="262"	intype="String"		inlength="60"	name="?"	dicomtype="LO"	dicomtag="StudyDescription"		# Description
block="STUDYHDR"	offset="322"	intype="String"		inlength="120"	name="?"	dicomtype="LT"	dicomtag="AdditionalPatientHistory"	# History
block="STUDYHDR"	offset="442"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Creation time in seconds.
block="STUDYHDR"	offset="446"	intype="String"		inlength="32"	name="?"	dicomtype="LO"	dicomtag="InstitutionName"		# Hospital name
block="STUDYHDR"	offset="478"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=inpatient,1=outpatient,2=emergency,3=referral,4=blank"	# Patient status
block="STUDYHDR"	offset="480"	intype="String"		inlength="12"	name="?"	dicomtype="?"	dicomtag="?"				# Req. Number from Scan Rx first page
block="STUDYHDR"	offset="1022"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# checksum


block="SERIESHDR"	offset="0"		intype="String"		inlength="14"	name="?"	dicomtype="?"	dicomtag="?"				# Series Header Identifier
block="SERIESHDR"	offset="14"		intype="String"		inlength="8"	name="?"	dicomtype="?"	dicomtag="?"				# Series Header Revision Number xx.xx.xx
block="SERIESHDR"	offset="22"		intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Number of Series Header Blocks
block="SERIESHDR"	offset="24"		intype="String"		inlength="32"	name="?"	dicomtype="?"	dicomtag="?"				# Series Header Creator (Process Name:PID)
block="SERIESHDR"	offset="56"		intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Series Header Creator (Task ID)
block="SERIESHDR"	offset="58"		intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Original Series Number
block="SERIESHDR"	offset="62"		intype="String"		inlength="3"	keyword="SeriesNumber"	dicomtype="IS"	dicomtag="SeriesNumber"		# Series Number
block="SERIESHDR"	offset="66"		intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Raw Data System ID
block="SERIESHDR"	offset="70"		intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# System Generation ID
block="SERIESHDR"	offset="74"		intype="String"		inlength="9"	name="?"	dicomtype="?"	dicomtag="?"				# Date of Series (ascii dd-mmm-yy)
block="SERIESHDR"	offset="84"		intype="Int16_B"	inlength="1"	keyword="SeriesDateDay"	dicomtype="?"	dicomtag="?"			# Date of Series (integer day)
block="SERIESHDR"	offset="86"		intype="Int16_B"	inlength="1"	keyword="SeriesDateMonth"	dicomtype="?"	dicomtag="?"		# Date of Series (integer month)
block="SERIESHDR"	offset="88"		intype="Int16_B"	inlength="1"	keyword="SeriesDateYear"	dicomtype="?"	dicomtag="?"		# Date of Series (integer year-1900)
block="SERIESHDR"	offset="90"		intype="String"		inlength="8"	name="?"	dicomtype="?"	dicomtag="?"				# Time of Series (ascii hh:mm:ss)
block="SERIESHDR"	offset="98"		intype="Int16_B"	inlength="1"	keyword="SeriesTimeHour"	dicomtype="?"	dicomtag="?"		# Time of Series (integer hr)
block="SERIESHDR"	offset="100"	intype="Int16_B"	inlength="1"	keyword="SeriesTimeMin"	dicomtype="?"	dicomtag="?"			# Time of Series (integer min)
block="SERIESHDR"	offset="102"	intype="Int16_B"	inlength="1"	keyword="SeriesTimeSec"	dicomtype="?"	dicomtag="?"			# Time of Series (integer sec)
block="SERIESHDR"	offset="104"	intype="String"		inlength="120"	name="?"	dicomtype="LO"	dicomtag="SeriesDescription"		# Series Description
block="SERIESHDR"	offset="224"	intype="Int16_B"	inlength="1"	keyword="SeriesType"	dicomtype="?"	dicomtag="?"	enum="0=normal,1=screensave,2=composite"	# Series Type
block="SERIESHDR"	offset="226"	intype="Int16_B"	inlength="1"	name="CoilType"	dicomtype="?"	dicomtag="?"	enum="0=head,1=body,2=surface"			# Coil Type
block="SERIESHDR"	offset="228"	intype="String"		inlength="16"	name="CoilName"	dicomtype="?"	dicomtag="?"			# Coil Name (HEAD,BODY,coil name) (Not always present)
block="SERIESHDR"	offset="244"	intype="String"		inlength="32"	name="?"	dicomtype="LO"	dicomtag="ContrastBolusAgent"		# Contrast Description
block="SERIESHDR"	offset="276"	intype="Int16_B"	inlength="1"	name="PlaneType"	dicomtype="?"	dicomtag="?"	enum="0=axial,1=sagittal,2=coronal,3=oblique,4=screensave"	# Plane Type
block="SERIESHDR"	offset="278"	intype="String"		inlength="16"	name="PlaneName"	dicomtype="?"	dicomtag="?"			# Plane Name (AXIAL,SAGITTAL,CORONAL,OBLIQUE,SCREEN SAVE)
block="SERIESHDR"	offset="294"	intype="Int16_B"	inlength="1"	keyword="ImageMode"	dicomtype="?"	dicomtag="?"	enum="0=2D Single,1=2D Multiple,2=3D Volume,3=Cine,4=Spectro"	# Image Mode
block="SERIESHDR"	offset="296"	intype="Int16_B"	inlength="1"	keyword="MagneticFieldStrengthGauss"	dicomtype="?"	dicomtag="?"	# Magnetic Field Strength (Gauss)
block="SERIESHDR"	offset="298"	intype="Int16_B"	inlength="1"	keyword="PulseSequence"	dicomtype="?"	dicomtag="?"	enum="0=MEMP,1=IR,2=PS,3=RM,4=RMGE,5=GRE,6=VEMP,7=MPGR,8=MPGRV,9=MPIRS,10=MPIRI,11=3D/GRE,12=CINE/GRE,13=SPGR,14=SSFP,15=CINE/SPGR,16=3D/SPGR,17=FSE,18=FVE,19=FSPGR,20=FGR,21=FMPSPGR,22=FMPGR,23=FMPIR,24=PROBE.S,25=PROBE.P"	# Pulse Sequence
block="SERIESHDR"	offset="300"	intype="Int16_B"	inlength="1"	name="?"		dicomtype="?"	dicomtag="?"	enum="0=chopper"	# Pulse sequence subtype
block="SERIESHDR"	offset="302"	intype="DG_Float_F"	inlength="1"	name="FieldOfView"	dicomtype="DS"	dicomtag="ReconstructionDiameter"	# Field of view mm
block="SERIESHDR"	offset="306"	intype="DG_Float_F"	inlength="1"	name="CenterRL"	dicomtype="?"	dicomtag="?"			# Center RL R+ (relative to landmark)
block="SERIESHDR"	offset="310"	intype="DG_Float_F"	inlength="1"	name="CenterAP"	dicomtype="?"	dicomtag="?"			# Center AP A+ (relative to landmark)
block="SERIESHDR"	offset="314"	intype="DG_Float_F"	inlength="1"	name="CenterSI"	dicomtype="?"	dicomtag="?"			# Center SI S+ (relative to landmark)
block="SERIESHDR"	offset="318"	intype="Int16_B"	inlength="1"	keyword="PatientOrientation"	dicomtype="?"	dicomtag="?"		# Orientation (0=supine,1=prone,2=lt decubitus,3=rt decubitus)
block="SERIESHDR"	offset="320"	intype="Int16_B"	inlength="1"	keyword="PatientPosition"	dicomtype="?"	dicomtag="?"	enum="0=Head First,1=Feet First"	# Position
block="SERIESHDR"	offset="322"	intype="String"		inlength="32"	name="LongitudinalReference"	dicomtype="LO"	dicomtag="PositionReferenceIndicator"	# Longitudinal Anatomical Reference
block="SERIESHDR"	offset="354"	intype="String"		inlength="32"	name="VerticalReference"	dicomtype="?"	dicomtag="?"		# Vertical Anatomical Reference
block="SERIESHDR"	offset="386"	intype="DG_Float_F"	inlength="1"	name="VerticalLandmark"	dicomtype="?"	dicomtag="?"		# Vertical Landmark (relative to tabletop) mm
block="SERIESHDR"	offset="390"	intype="DG_Float_F"	inlength="1"	name="HorizontalLandmark"	dicomtype="?"	dicomtag="?"		# Horizontal Landmark (relative to table center) mm
block="SERIESHDR"	offset="394"	intype="DG_Float_F"	inlength="1"	name="PhysicalTableLocation"	dicomtype="?"	dicomtag="?"		# Physical Table Location relative to home
block="SERIESHDR"	offset="398"	intype="Int16_B"	inlength="1"	name="ScanMatrixX"	dicomtype="?"	dicomtag="?"			# Scan Matrix - X
block="SERIESHDR"	offset="400"	intype="Int16_B"	inlength="1"	name="ScanMatrixY"	dicomtype="?"	dicomtag="?"			# Scan Matrix - Y
block="SERIESHDR"	offset="402"	intype="Int16_B"	inlength="1"	keyword="ImageMatrix"	dicomtype="?"	dicomtag="?"			# Image Matrix
block="SERIESHDR"	offset="404"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# No. of Images Allocated
block="SERIESHDR"	offset="406"	intype="Int16_B"	inlength="1"	keyword="GatingType"	dicomtype="?"	dicomtag="?"	bitmap="0:none,EG;1:none,RESP;2:none,RC;3:none,FC;4:none,CL;5:none,ST;6:none,PG;7:none,NP;8:none,RF;9:none,RT;10:none,VB;11:none,ED;12:none,PM;15:none,MP"	# Gating Type
block="SERIESHDR"	offset="408"	intype="Int16_B"	inlength="1"	name="PSMode"	dicomtype="?"	dicomtag="?"	enum="0=Product,1=Research,2=Research GE"	# Pulse sequence mode
block="SERIESHDR"	offset="410"	intype="String"		inlength="12"	name="?"	dicomtype="?"	dicomtag="?"				# PSD name from NAME = aPPL psd name
block="SERIESHDR"	offset="422"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Landmark counter
block="SERIESHDR"	offset="426"	intype="String"		inlength="20"	name="?"	dicomtype="LO"	dicomtag="ProtocolName"			# Protocol name for Scan
block="SERIESHDR"	offset="446"	intype="Int16_B"	inlength="1"	name="SCoilType"	dicomtype="?"	dicomtag="?"	enum="0=receive,1=transmit/receive"	# Surface coil type
block="SERIESHDR"	offset="448"	intype="Int16_B"	inlength="1"	keyword="SuppTech"		dicomtype="?"	dicomtag="?"	enum="0=none,1=fat:F,2=water:W"	# Suppression technique
block="SERIESHDR"	offset="450"	intype="Int16_B"	inlength="1"	name="Sat"		dicomtype="?"	dicomtag="?"	bitmap="0:none,superior(S);1:none,inferior(I);2:none,right(R);3:none,left(R);4:none,anterior(A);5:none,posterior(P);6:none,superior(s);7:none,inferior(i);8:none,right(r);9:none,left(l);10:none,anterior(a);11:none,posterior(p)"	#  Bitmap of SAT selections
block="SERIESHDR"	offset="452"	intype="Int16_B"	inlength="1"	keyword="SCIC"	dicomtype="?"	dicomtag="?"	enum="0=off,1=on"	# Surface Coil Intensity Correction Flag
block="SERIESHDR"	offset="454"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# R saturation pulse location  - mm from landmark
block="SERIESHDR"	offset="456"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# L saturation pulse location  - mm from landmark
block="SERIESHDR"	offset="458"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# A saturation pulse location  - mm from landmark
block="SERIESHDR"	offset="460"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# P saturation pulse location  - mm from landmark
block="SERIESHDR"	offset="462"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# S saturation pulse location  - mm from landmark
block="SERIESHDR"	offset="464"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# I saturation pulse location  - mm from landmark
block="SERIESHDR"	offset="466"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# X saturation pulse thickness - mm
block="SERIESHDR"	offset="468"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Y saturation pulse thickness - mm
block="SERIESHDR"	offset="470"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Z saturation pulse thickness - mm
block="SERIESHDR"	offset="472"	intype="Int16_B"	inlength="1"	keyword="VasMode"	dicomtype="?"	dicomtag="?"	enum="0=none,1=TOF,2=PC"	# Vascular mode
block="SERIESHDR"	offset="474"	intype="Int16_B"	inlength="1"	name="FlowAxis"	dicomtype="?"	dicomtag="?"	bitmap="0:none,S/I;1:none,R/L;2:none,A/P;3:none,SLICE"	# Phase contrast flow axis
block="SERIESHDR"	offset="476"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# unused venc ... see image header
block="SERIESHDR"	offset="478"	intype="Int16_B"	inlength="1"	keyword="GatingType2"	dicomtype="?"	dicomtag="?"	bitmap="0:none,IR Prep;1:none,DE Prep"	# more pulse sequence types
block="SERIESHDR"	offset="1022"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# checksum


block="IMAGEHDR"	offset="0"		intype="String"		inlength="14"	name="?"	dicomtype="?"	dicomtag="?"				# Image Header Identifier
block="IMAGEHDR"	offset="14"		intype="String"		inlength="8"	name="?"	dicomtype="LO"	dicomtag="SoftwareVersions"		# Image Header Revision Number xx.xx.xx
block="IMAGEHDR"	offset="22"		intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Number of Image Header Blocks
block="IMAGEHDR"	offset="24"		intype="String"		inlength="32"	name="?"	dicomtype="?"	dicomtag="?"				# Image Header Creator (Process Name:PID)
block="IMAGEHDR"	offset="56"		intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Image Header Creator (Task ID)
block="IMAGEHDR"	offset="58"		intype="String"		inlength="9"	name="?"	dicomtype="?"	dicomtag="?"				# Date of Image (ascii dd-mmm-yy)
block="IMAGEHDR"	offset="68"		intype="Int16_B"	inlength="1"	keyword="ImageDateDay"	dicomtype="?"	dicomtag="?"			# Date of Image (integer day)
block="IMAGEHDR"	offset="70"		intype="Int16_B"	inlength="1"	keyword="ImageDateMonth"	dicomtype="?"	dicomtag="?"		# Date of Image (integer month)
block="IMAGEHDR"	offset="72"		intype="Int16_B"	inlength="1"	keyword="ImageDateYear"	dicomtype="?"	dicomtag="?"			# Date of Image (integer year-1900)
block="IMAGEHDR"	offset="74"		intype="String"		inlength="8"	name="?"	dicomtype="?"	dicomtag="?"				# Time of Image (ascii hh:mm:ss)
block="IMAGEHDR"	offset="82"		intype="Int16_B"	inlength="1"	keyword="ImageTimeHour"	dicomtype="?"	dicomtag="?"			# Time of Image (integer hr)
block="IMAGEHDR"	offset="84"		intype="Int16_B"	inlength="1"	keyword="ImageTimeMin"	dicomtype="?"	dicomtag="?"			# Time of Image (integer min)
block="IMAGEHDR"	offset="86"		intype="Int16_B"	inlength="1"	keyword="ImageTimeSec"	dicomtype="?"	dicomtag="?"			# Time of Image (integer sec)
block="IMAGEHDR"	offset="88"		intype="String"		inlength="3"	keyword="ImageNumber"	dicomtype="IS"	dicomtag="InstanceNumber"		# Image Number
block="IMAGEHDR"	offset="92"		intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Series Number of Image
block="IMAGEHDR"	offset="96"		intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Raw Data System ID
block="IMAGEHDR"	offset="100"	intype="String"		inlength="3"	name="?"	dicomtype="SH"	dicomtag="StationName"			# System Generation ID
block="IMAGEHDR"	offset="104"	intype="DG_Float_F"	inlength="1"	name="LocationRMin"	dicomtype="?"	dicomtag="?"			# Start Location X, Right min
block="IMAGEHDR"	offset="108"	intype="DG_Float_F"	inlength="1"	name="LocationRMax"	dicomtype="?"	dicomtag="?"			# End Location X, Right max
block="IMAGEHDR"	offset="112"	intype="DG_Float_F"	inlength="1"	name="LocationAMin"	dicomtype="?"	dicomtag="?"			# Start Location Y, Anterior min
block="IMAGEHDR"	offset="116"	intype="DG_Float_F"	inlength="1"	name="LocationAMax"	dicomtype="?"	dicomtag="?"			# End Location Y, Anterior max
block="IMAGEHDR"	offset="120"	intype="DG_Float_F"	inlength="1"	name="LocationSMin"	dicomtype="?"	dicomtag="?"			# Start Location Z, Superior min
block="IMAGEHDR"	offset="124"	intype="DG_Float_F"	inlength="1"	name="LocationSMax"	dicomtype="?"	dicomtag="?"			# End Location Z, Superior max
block="IMAGEHDR"	offset="146"	intype="DG_Float_F"	inlength="1"	name="SliceLocation"	dicomtype="DS"	dicomtag="SliceLocation"	# Image Location relative to landmark
block="IMAGEHDR"	offset="150"	intype="DG_Float_F"	inlength="1"	name="TablePosition"	dicomtype="?"	dicomtag="?"			# Table Position
block="IMAGEHDR"	offset="154"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="SliceThickness"		# Thickness in mm
block="IMAGEHDR"	offset="158"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="SpacingBetweenSlices"		# Spacing in mm
block="IMAGEHDR"	offset="162"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="-1=round to nearest slice"	# Round
block="IMAGEHDR"	offset="164"	intype="DG_Float_F"	inlength="1"	keyword="RepetitionTimeUS"	dicomtype="?"	dicomtag="?"		# Repetition/Recovery Time uS
block="IMAGEHDR"	offset="168"	intype="DG_Float_F"	inlength="1"	name="ScanTimeUS"	dicomtype="?"	dicomtag="?"			# Scan Time uS
block="IMAGEHDR"	offset="172"	intype="DG_Float_F"	inlength="1"	keyword="EchoTimeUS"	dicomtype="?"	dicomtag="?"			# Echo Delay uS
block="IMAGEHDR"	offset="176"	intype="DG_Float_F"	inlength="1"	keyword="InversionTimeUS"	dicomtype="?"	dicomtag="?"		# Inversion Time uS
block="IMAGEHDR"	offset="180"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Other time 0 - Reserved for future use.
block="IMAGEHDR"	offset="184"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Other time 1 - Reserved for future use.
block="IMAGEHDR"	offset="188"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Other time 2 - Reserved for future use.
block="IMAGEHDR"	offset="192"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Other time 3 - Reserved for future use.
block="IMAGEHDR"	offset="196"	intype="Int16_B"	inlength="1"	keyword="NumberOfEchoes"	dicomtype="?"	dicomtag="?"		# Number of echos.
block="IMAGEHDR"	offset="198"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="EchoNumbers"			# Echo number.
block="IMAGEHDR"	offset="200"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="ImagesInAcquisition"		# Number of slices in scan group.
block="IMAGEHDR"	offset="202"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Number of averages (before fractional nex)
block="IMAGEHDR"	offset="204"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,-1=yes"	# Research mode used
block="IMAGEHDR"	offset="206"	intype="String"		inlength="32"	name="?"	dicomtype="?"	dicomtag="?"				# Name of PSD file.
block="IMAGEHDR"	offset="238"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Creation Date of PSD file dd
block="IMAGEHDR"	offset="240"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Creation Date of PSD file mm
block="IMAGEHDR"	offset="242"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Creation Date of PSD file yy-1900
block="IMAGEHDR"	offset="244"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Creation Date of PSD file hh
block="IMAGEHDR"	offset="246"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Creation Date of PSD file mm
block="IMAGEHDR"	offset="248"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Creation Date of PSD file ss
block="IMAGEHDR"	offset="250"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,-1=yes"	# Graphically Prescribed (? any non-zero is yes)
block="IMAGEHDR"	offset="252"	intype="String"		inlength="9"	name="?"	dicomtype="?"	dicomtag="?"				# Series Numbers [3*3] from which prescribed
block="IMAGEHDR"	offset="262"	intype="String"		inlength="9"	name="?"	dicomtype="?"	dicomtag="?"				# Image Numbers [3*3] from which prescribed
block="IMAGEHDR"	offset="272"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=box,1=ellipse"	# Image Shape
block="IMAGEHDR"	offset="274"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="US"	dicomtag="Rows"				# X pixel dimension
block="IMAGEHDR"	offset="276"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="US"	dicomtag="Columns"			# Y pixel dimension
block="IMAGEHDR"	offset="278"	intype="DG_Float_F"	inlength="1"	keyword="PixelSize"	dicomtype="?"	dicomtag="?"			# Pixel Size - mm
block="IMAGEHDR"	offset="282"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no"		# Image Compressed (non-zero=technique)
block="IMAGEHDR"	offset="284"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Bits per Pixel - not always filled in
block="IMAGEHDR"	offset="286"	intype="Int16_B"	inlength="1"	keyword="DefaultWindow"	dicomtype="?"	dicomtag="?"	# Default Window
block="IMAGEHDR"	offset="288"	intype="Int16_B"	inlength="1"	keyword="DefaultLevel"	dicomtype="?"	dicomtag="?"	# Default Level
block="IMAGEHDR"	offset="290"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Number of Blocks in File
block="IMAGEHDR"	offset="292"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="NumberOfAverages"		# Number of excitations (presumably post-fractional NEX)
block="IMAGEHDR"	offset="296"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Value of peak SAR watts/kg
block="IMAGEHDR"	offset="300"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="SAR"				# Value of average SAR watts/kg
block="IMAGEHDR"	offset="304"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# SAR monitored
block="IMAGEHDR"	offset="306"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# Contiguous slices (? any non-zero is yes)
block="IMAGEHDR"	offset="308"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="HeartRate"			# Cardiac Heart Rate
block="IMAGEHDR"	offset="310"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Total Delay Time After Trigger - ms between QRS peak and 1st excitation pulse
block="IMAGEHDR"	offset="314"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="TriggerWindow"		# Arrhythmia Rejection Ratio - % of avg RR during which trigger is recognized
block="IMAGEHDR"	offset="316"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Cardiac Rep Time - 1 pulse every beat, 2 pulse very 2nd beat, etc
block="IMAGEHDR"	offset="318"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="CardiacNumberOfImages"	# Images per Cardiac Cycle (single scan only)
block="IMAGEHDR"	offset="320"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="IntervalsAcquired"		# Number of RR's during the Scan
block="IMAGEHDR"	offset="324"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Transmit attenuator setting .1 dB
block="IMAGEHDR"	offset="326"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recieve attenuator setting .1dB
block="IMAGEHDR"	offset="328"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Magnetic Field Strength 10 microgauss
block="IMAGEHDR"	offset="332"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Image offset - frequency/phase offset [-256...256]
block="IMAGEHDR"	offset="334"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Inter image/inter location delay - ms between excitation pulses within RR interval
block="IMAGEHDR"	offset="338"	intype="String"		inlength="12"	name="?"	dicomtype="?"	dicomtag="?"				# PSD name from NAME = aPPL psd name
block="IMAGEHDR"	offset="350"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="FlipAngle"			# Flip angle for GRASS
block="IMAGEHDR"	offset="352"	intype="String"		inlength="4"	name="?"	dicomtype="?"	dicomtag="?"				# Type of correction for surface coils ?????
block="IMAGEHDR"	offset="356"	intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Series no. of corrected/uncor images ?????
block="IMAGEHDR"	offset="360"	intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Image no. of corrected/uncor images ?????
block="IMAGEHDR"	offset="364"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=false,1=true"	# Extremity coil
block="IMAGEHDR"	offset="386"	intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Series no. of second localizer
block="IMAGEHDR"	offset="390"	intype="String"		inlength="3"	name="?"	dicomtype="?"	dicomtag="?"				# Image no. of second localizer
block="IMAGEHDR"	offset="394"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# R center coordinate on plane image mm
block="IMAGEHDR"	offset="398"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# A center coordinate on plane image mm
block="IMAGEHDR"	offset="402"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# S center coordinate on plane image mm
block="IMAGEHDR"	offset="406"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# R normal coordinate mm
block="IMAGEHDR"	offset="410"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# A normal coordinate mm
block="IMAGEHDR"	offset="414"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# S normal coordinate mm
block="IMAGEHDR"	offset="418"	intype="DG_Float_F"	inlength="1"	keyword="TLHC_R"	dicomtype="?"	dicomtag="?"			# TLHC R coordinate mm
block="IMAGEHDR"	offset="422"	intype="DG_Float_F"	inlength="1"	keyword="TLHC_A"	dicomtype="?"	dicomtag="?"			# TLHC A coordinate mm
block="IMAGEHDR"	offset="426"	intype="DG_Float_F"	inlength="1"	keyword="TLHC_S"	dicomtype="?"	dicomtag="?"			# TLHC S coordinate mm
block="IMAGEHDR"	offset="430"	intype="DG_Float_F"	inlength="1"	keyword="TRHC_R"	dicomtype="?"	dicomtag="?"			# TRHC R coordinate mm
block="IMAGEHDR"	offset="434"	intype="DG_Float_F"	inlength="1"	keyword="TRHC_A"	dicomtype="?"	dicomtag="?"			# TRHC A coordinate mm
block="IMAGEHDR"	offset="438"	intype="DG_Float_F"	inlength="1"	keyword="TRHC_S"	dicomtype="?"	dicomtag="?"			# TRHC S coordinate mm
block="IMAGEHDR"	offset="442"	intype="DG_Float_F"	inlength="1"	keyword="BLHC_R"	dicomtype="?"	dicomtag="?"			# BLHC R coordinate mm
block="IMAGEHDR"	offset="446"	intype="DG_Float_F"	inlength="1"	keyword="BLHC_A"	dicomtype="?"	dicomtag="?"			# BLHC A coordinate mm
block="IMAGEHDR"	offset="450"	intype="DG_Float_F"	inlength="1"	keyword="BLHC_S"	dicomtype="?"	dicomtag="?"			# BLHC S coordinate mm
block="IMAGEHDR"	offset="454"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Image header disclaimer
block="IMAGEHDR"	offset="456"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Minimum delay after trigger ms
block="IMAGEHDR"	offset="458"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Number of cardiac phases to reconstruct [1...32]
block="IMAGEHDR"	offset="460"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# TE2 (VEMP) us
block="IMAGEHDR"	offset="464"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# Swap phase/frequency axis - operators choice
block="IMAGEHDR"	offset="466"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Pause interval ms
block="IMAGEHDR"	offset="468"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Pause time ms
block="IMAGEHDR"	offset="472"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Bitmap defining users CVs
block="IMAGEHDR"	offset="474"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 0
block="IMAGEHDR"	offset="478"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 1
block="IMAGEHDR"	offset="482"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 2
block="IMAGEHDR"	offset="486"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 3
block="IMAGEHDR"	offset="490"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 4
block="IMAGEHDR"	offset="494"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 5
block="IMAGEHDR"	offset="498"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 6
block="IMAGEHDR"	offset="502"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 7
block="IMAGEHDR"	offset="506"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 8
block="IMAGEHDR"	offset="510"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PSD user variable 9
block="IMAGEHDR"	offset="514"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	bitmap="0;none,Axial;1:none,Sagittal;2:none,Coronal;4:none,Oblique"	# Oblique plane (most like plane)
block="IMAGEHDR"	offset="516"	intype="Int16_B"	inlength="1"	name="ContrastUsed"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# Contrast used
block="IMAGEHDR"	offset="518"	intype="String"		inlength="10"	name="?"	dicomtype="?"	dicomtag="?"				# Contrast agent
block="IMAGEHDR"	offset="528"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Contrast amount
block="IMAGEHDR"	offset="532"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=pre 3.0,1=post 3.0"	# File format
block="IMAGEHDR"	offset="534"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=current,1=midpoint,2=water,3=fat,4=peak,5=centroid"	# Auto center frequency
block="IMAGEHDR"	offset="536"	intype="Int32_B"	inlength="1"	name="TransmitFreq"	dicomtype="?"	dicomtag="?"				# Actual transmit freq used on scan Hz
block="IMAGEHDR"	offset="540"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Actual receive freq used on scan Hz
block="IMAGEHDR"	offset="544"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recommended automated transmit freq Hz
block="IMAGEHDR"	offset="548"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recommended automated receive freq Hz
block="IMAGEHDR"	offset="552"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recommended automated transmit attenuation .1 dB
block="IMAGEHDR"	offset="556"	intype="Int32_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recommended automated receive attenuation .1 dB
block="IMAGEHDR"	offset="560"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# Histogram present in raw header?
block="IMAGEHDR"	offset="562"	intype="Int16_B"	inlength="1"	keyword="PFSwapped"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# Swapped phase/frequency - reality (rules or choice)
block="IMAGEHDR"	offset="564"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# R1 for prescan
block="IMAGEHDR"	offset="566"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# R2 for prescan
block="IMAGEHDR"	offset="568"	intype="Int16_B"	inlength="1"	keyword="VariableBandwidth"	dicomtype="?"	dicomtag="?"				# Variable bandwidth - docs say enum=0=no,1=yes but seems to be kHz
block="IMAGEHDR"	offset="570"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# R1 manual
block="IMAGEHDR"	offset="572"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# R2 manual
block="IMAGEHDR"	offset="574"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=none,1=auto failed,2=auto succeeded,3=manual,4=auto failed/manual,5=auto succeeded/manual"	# auto/manual prescan flag
block="IMAGEHDR"	offset="576"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	bitmap="0:none,CF;1:none,TA;2:none,R1;3:none,R2"	# Changed prescan values
block="IMAGEHDR"	offset="578"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=Magnitude,1=Phase,2=Real,3=Imaginary"		# Image type
block="IMAGEHDR"	offset="580"	intype="Int16_B"	inlength="1"	keyword="VascCollapse"	dicomtype="?"	dicomtag="?"	enum="0=Off,1=Col,2=Mag,3=R/L,4=A/P,5=S/I,6=Pjn,7=All,8=OMag,9=OR/L,10=OA/P,11=OS/I,12=OAll,13=OCol"	# Vascular collapse
block="IMAGEHDR"	offset="582"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# Thickness disclaimer
block="IMAGEHDR"	offset="584"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PC venc mm/sec
block="IMAGEHDR"	offset="586"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# tardis projection angle - degrees
block="IMAGEHDR"	offset="590"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	# concatenated sat selected
block="IMAGEHDR"	offset="592"	intype="Int16_B"	inlength="1"	keyword="FracEcho"	dicomtype="?"	dicomtag="?"	bitmap="0:none,Fractional(Fr);1:none,Effective(Ef)"	# TE Flag
block="IMAGEHDR"	offset="594"	intype="Int32_B"	inlength="1"	keyword="EchoTrainLength"	dicomtype="?"	dicomtag="?"				# Echo train length
block="IMAGEHDR"	offset="598"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Slice multiplier to obtain phases for FAST - MP option (multiphase) - number of phases per location
block="IMAGEHDR"	offset="600"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Cardiac phase number of current image
block="IMAGEHDR"	offset="602"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Number of Acquisitions in scan
block="IMAGEHDR"	offset="604"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"	bitmap=""0:none,noflags;1:none,magweight"	# Vascular imaging flags
block="IMAGEHDR"	offset="606"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# VENC scaling factor
block="IMAGEHDR"	offset="1022"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# checksum

constant="16" 							dicomtype="US"	dicomtag="BitsAllocated"
constant="12" 							dicomtype="US"	dicomtag="BitsStored"
constant="11" 							dicomtype="US"	dicomtag="HighBit"
constant="0" 							dicomtype="US"	dicomtag="PixelRepresentation"	# Unsigned
constant="1" 							dicomtype="US"	dicomtag="SamplesPerPixel"
constant="MONOCHROME2" 					dicomtype="CS"	dicomtag="PhotometricInterpretation"
constant=""								dicomtype="SH"	dicomtag="AccessionNumber"
constant=""								dicomtype="DA"	dicomtag="PatientBirthDate"
constant="GE Medical Systems"			dicomtype="LO"	dicomtag="Manufacturer"
constant="Signa"						dicomtype="LO"	dicomtag="ManufacturerModelName"
constant="1.2.840.10008.5.1.4.1.1.4"	dicomtype="UI"	dicomtag="SOPClassUID"
constant="MR"							dicomtype="CS"	dicomtag="Modality"

