# Offsets are in 8 bit bytes from 0
# Lengths are in intype words

# Global Header

block="GLOBALHDR"	offset="0"	intype="String"	inlength="6"	name="?"	dicomtype="?"	dicomtag="?"				# Data base membership ID (ASCII '9800') 
block="GLOBALHDR"	offset="6"	intype="String"	inlength="2"	name="?"	dicomtype="?"	dicomtag="?"				# Revision Level 
block="GLOBALHDR"	offset="8"	intype="String"	inlength="2"	name="?"	dicomtype="?"	dicomtag="?"				# Format code 
block="GLOBALHDR"	offset="10"	intype="String"	inlength="2"	name="?"	dicomtype="?"	dicomtag="?"				# User Revision code
block="GLOBALHDR"	offset="12"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Month of Creation 
block="GLOBALHDR"	offset="14"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Day of Creation 
block="GLOBALHDR"	offset="16"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Year of Creation 
block="GLOBALHDR"	offset="18"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Hour of Creation 
block="GLOBALHDR"	offset="20"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Minute of Creation 
block="GLOBALHDR"	offset="22"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Second of Creation 
block="GLOBALHDR"	offset="24"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Originating Station ID 

block="GLOBALHDR"	offset="32"	intype="String"	inlength="14"	name="?"	dicomtype="?"	dicomtag="?"				# External File Name 

block="GLOBALHDR"	offset="64"	intype="String"	inlength="2"	name="?"	dicomtype="?"	dicomtag="?"				# Image Block 0 revision code 

block="GLOBALHDR"	offset="66"	intype="Int16_B"	inlength="1"	name="GLOBALHDRPTR"		dicomtype="?"	dicomtag="?"		# Word ptr to global header
block="GLOBALHDR"	offset="68"	intype="Int16_B"	inlength="1"	keyword="EXAMHDRPTR"		dicomtype="?"	dicomtag="?"		# Word ptr to exam header
block="GLOBALHDR"	offset="70"	intype="Int16_B"	inlength="1"	keyword="IMAGE1HDRPTR"		dicomtype="?"	dicomtag="?"		# Word ptr to image header 1
block="GLOBALHDR"	offset="72"	intype="Int16_B"	inlength="1"	keyword="IMAGE2HDRPTR"		dicomtype="?"	dicomtag="?"		# Word ptr to image header 2
block="GLOBALHDR"	offset="74"	intype="Int16_B"	inlength="1"	keyword="IMAGEMAPPTR"		dicomtype="?"	dicomtag="?"		# Word ptr to image map
block="GLOBALHDR"	offset="76"	intype="Int16_B"	inlength="1"	keyword="IMAGEDATAPTR"		dicomtype="?"	dicomtag="?"		# Word ptr to image data

block="GLOBALHDR"	offset="78"	intype="Int16_B"	inlength="1"	name="GLOBALHDRLNG"		dicomtype="?"	dicomtag="?"		# Blocks in global header
block="GLOBALHDR"	offset="80"	intype="Int16_B"	inlength="1"	name="EXAMHDRLNG"		dicomtype="?"	dicomtag="?"		# Blocks in exam header
block="GLOBALHDR"	offset="82"	intype="Int16_B"	inlength="1"	name="IMAGE1HDRLNG"		dicomtype="?"	dicomtag="?"		# Blocks in image header 1
block="GLOBALHDR"	offset="84"	intype="Int16_B"	inlength="1"	name="IMAGE2HDRLNG"		dicomtype="?"	dicomtag="?"		# Blocks in image header 2
block="GLOBALHDR"	offset="86"	intype="Int16_B"	inlength="1"	name="IMAGEMAPLNG"		dicomtype="?"	dicomtag="?"		# Blocks in image map
block="GLOBALHDR"	offset="88"	intype="Int16_B"	inlength="1"	name="IMAGEDATALNG"		dicomtype="?"	dicomtag="?"		# Blocks in image data

block="GLOBALHDR"	offset="90"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# max # of images in file 
block="GLOBALHDR"	offset="92"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 3*16 bits for images 
block="GLOBALHDR"	offset="94"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recon done flags 
block="GLOBALHDR"	offset="100"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Image write locked flags 
block="GLOBALHDR"	offset="106"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Archive required flags 
block="GLOBALHDR"	offset="112"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Archive done flags 
block="GLOBALHDR"	offset="118"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Image removed flags 
block="GLOBALHDR"	offset="124"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# block ptrs to image hdrs 
block="GLOBALHDR"	offset="192"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# should be constant = 1 
block="GLOBALHDR"	offset="194"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# should be constant = 512

# Exam Header

block="EXAMHDR"	offset="0"	intype="String"	inlength="2"	name="?"	dicomtype="LO"	dicomtag="SoftwareVersions"		# Revision Code 
block="EXAMHDR"	offset="2"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# CT System number 
block="EXAMHDR"	offset="4"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="SH"	dicomtag="StationName"			# CT Station ID 
block="EXAMHDR"	offset="6"	intype="Int16_B"	inlength="1"	keyword="StudyNumber"	dicomtype="SH"	dicomtag="StudyID"		# Exam Number (integer) 
block="EXAMHDR"	offset="8"	intype="String"	inlength="14"	name="?"	dicomtype="?"	dicomtag="?"				# Exam Number (ascii) 
block="EXAMHDR"	offset="22"	intype="String"	inlength="12"	name="?"	dicomtype="LO"	dicomtag="PatientID"			# Patient ID 
block="EXAMHDR"	offset="34"	intype="String"	inlength="30"	name="?"	dicomtype="PN"	dicomtag="PatientName"			# Patient Name 
block="EXAMHDR"	offset="64"	intype="String"	inlength="30"	name="?"	dicomtype="PN"	dicomtag="ReferringPhysicianName"	# Referring Physician 
block="EXAMHDR"	offset="94"	intype="String"	inlength="24"	name="?"	dicomtype="LO"	dicomtag="InstitutionName"		# Hospital Name 
block="EXAMHDR"	offset="118"	intype="String"	inlength="30"	name="?"	dicomtype="PN"	dicomtag="PerformingPhysicianName"	# Diagnostician 
block="EXAMHDR"	offset="148"	intype="String"	inlength="4"	name="?"	dicomtype="PN"	dicomtag="OperatorsName"			# Operator Initials 
block="EXAMHDR"	offset="152"	intype="String"	inlength="80"	name="?"	dicomtype="LO"	dicomtag="StudyDescription"		# Exam Description 

# Exam Details

block="EXAMHDR"	offset="232"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of scout scans 
block="EXAMHDR"	offset="234"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of axial scans 
block="EXAMHDR"	offset="236"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of standard axial scans 
block="EXAMHDR"	offset="238"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of dynamic scans done 
block="EXAMHDR"	offset="240"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of standard target axials 
block="EXAMHDR"	offset="242"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # dynamic target axials 
block="EXAMHDR"	offset="244"	intype="Int16_B"	inlength="1"	name="EXAMSTARTDATEMM"	dicomtype="?"	dicomtag="?"			# Exam start Date (M/D/Y) 
block="EXAMHDR"	offset="246"	intype="Int16_B"	inlength="1"	name="EXAMSTARTDATEDD"	dicomtype="?"	dicomtag="?"			#  
block="EXAMHDR"	offset="248"	intype="Int16_B"	inlength="1"	name="EXAMSTARTDATEYY"	dicomtype="?"	dicomtag="?"			#  
block="EXAMHDR"	offset="250"	intype="Int16_B"	inlength="1"	name="EXAMSTARTTIMEHH"	dicomtype="?"	dicomtag="?"			# Exam start Time (H/M/S) 
block="EXAMHDR"	offset="252"	intype="Int16_B"	inlength="1"	name="EXAMSTARTTIMEMM"	dicomtype="?"	dicomtag="?"			#  
block="EXAMHDR"	offset="254"	intype="Int16_B"	inlength="1"	name="EXAMSTARTTIMESS"	dicomtype="?"	dicomtag="?"			# 
block="EXAMHDR"	offset="256"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"			# Exam end Date (M/D/Y) 
block="EXAMHDR"	offset="258"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"			#  
block="EXAMHDR"	offset="260"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"			#  
block="EXAMHDR"	offset="262"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"			# Exam end Time (H/M/S) 
block="EXAMHDR"	offset="264"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"			#  
block="EXAMHDR"	offset="266"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"			# 
block="EXAMHDR"	offset="268"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Scan Revision Type 
block="EXAMHDR"	offset="270"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Trauma Flag (0=n,1=y) 

# Hardware

block="EXAMHDR"	offset="272"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# CT Detector Type 
block="EXAMHDR"	offset="274"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of Detectors 
block="EXAMHDR"	offset="276"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Detector # at theta=0 
block="EXAMHDR"	offset="280"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Detector spacing in mm (fp) 
block="EXAMHDR"	offset="284"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="DistanceSourceToDetector"	# Source to detector mm (fp) 
block="EXAMHDR"	offset="288"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="DistanceSourceToPatient"	# Source to center mm (fp) 
block="EXAMHDR"	offset="292"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Tube type 
block="EXAMHDR"	offset="294"	intype="String"	inlength="4"	name="?"	dicomtype="?"	dicomtag="?"				# DAS type 
block="EXAMHDR"	offset="298"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Das read direction 
block="EXAMHDR"	offset="300"	intype="String"	inlength="4"	name="?"	dicomtype="?"	dicomtag="?"				# Table firmware revision 
block="EXAMHDR"	offset="304"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Drive count 
block="EXAMHDR"	offset="306"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Axial trigger rate 
block="EXAMHDR"	offset="308"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Scout trigger rate 
block="EXAMHDR"	offset="310"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Disk Drive Type 

# Header Control Values

block="EXAMHDR"	offset="312"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Calcheck file ? 
block="EXAMHDR"	offset="314"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Header info ok? 
block="EXAMHDR"	offset="316"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# All prospective recons done? 
block="EXAMHDR"	offset="318"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Prospect. & scouts archived? 
block="EXAMHDR"	offset="320"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Images archived? 
block="EXAMHDR"	offset="322"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Header modified? 
block="EXAMHDR"	offset="324"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Validation tape number 
block="EXAMHDR"	offset="326"	intype="String"		inlength="2"	name="?"	dicomtype="?"	dicomtag="?"				# Engineering release number 
block="EXAMHDR"	offset="328"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# temp (was checksum ?)
block="EXAMHDR"	offset="330"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 9800 Family ID 
block="EXAMHDR"	offset="332"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon PRS number 
block="EXAMHDR"	offset="334"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon 1st baseline 
block="EXAMHDR"	offset="336"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon last baseline 
block="EXAMHDR"	offset="338"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon 1st enhanced 
block="EXAMHDR"	offset="340"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon last enhanced 
block="EXAMHDR"	offset="342"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Checksum 

# Image Header #1

block="IMAGE1HDR"	offset="0"	intype="String"		inlength="2"	name="?"	dicomtype="?"	dicomtag="?"				# Revision code 
block="IMAGE1HDR"	offset="2"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Station ID 
block="IMAGE1HDR"	offset="4"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Exam Number 
block="IMAGE1HDR"	offset="6"	intype="String"		inlength="14"	name="?"	dicomtype="?"	dicomtag="?"				# Exam Number (ascii) 
block="IMAGE1HDR"	offset="20"	intype="Int16_B"	inlength="1"	keyword="?"	dicomtype="?"	dicomtag="?"		# Position (study) number 
block="IMAGE1HDR"	offset="22"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Intermediate support pos 
block="IMAGE1HDR"	offset="24"	intype="Int16_B"	inlength="1"	keyword="GroupType"	dicomtype="?"	dicomtag="?"				# Group (2=scout,3=std,4=dyn,5=arr) 
block="IMAGE1HDR"	offset="26"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="SeriesNumber"				# Group Number 
block="IMAGE1HDR"	offset="28"	intype="String"		inlength="30"	name="?"	dicomtype="LO"	dicomtag="SeriesDescription"				# Group Description 
block="IMAGE1HDR"	offset="92"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="IS"	dicomtag="AcquisitionNumber"		# Scan Number 
block="IMAGE1HDR"	offset="94"	intype="Int16_B"	inlength="1"	keyword="ImageNumber"	dicomtype="IS"	dicomtag="InstanceNumber"		# Image Number 
block="IMAGE1HDR"	offset="96"	intype="Int16_B"	inlength="1"	name="PatientType"	dicomtype="?"	dicomtag="?"			# Patient Type (2=sm,3=me,4=la) 
block="IMAGE1HDR"	offset="98"	intype="Int16_B"	inlength="1"	keyword="PatientOrientation"	dicomtype="?"	dicomtag="?"		# Patient Orientation (1=Head first,2=Feet first) 
block="IMAGE1HDR"	offset="100"	intype="Int16_B"	inlength="1"	keyword="APPatientOrientation"	dicomtype="?"	dicomtag="?"		# AP Orientation (1=prone,2=supine,3=left lateral decubitus,4=right lateral decubitus) 
block="IMAGE1HDR"	offset="102"	intype="String"		inlength="6"	keyword="PositionReference"	dicomtype="LO"	dicomtag="PositionReferenceIndicator"		# Position Reference 
block="IMAGE1HDR"	offset="108"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Contrast Flag (0=n,1=y) 
block="IMAGE1HDR"	offset="110"	intype="String"		inlength="64"	name="?"	dicomtype="LO"	dicomtag="ContrastBolusAgent"		# Contrast Description 
block="IMAGE1HDR"	offset="174"	intype="Int16_B"	inlength="1"	keyword="BowtieFilter"	dicomtype="?"	dicomtag="?"			# Bowtie Filter (1=body,2=head,3=a) 
block="IMAGE1HDR"	offset="176"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="SliceThickness"		# Aperture (1.5,3,5,10) (fp) 
block="IMAGE1HDR"	offset="180"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="KVP"				# Generator KVP (80,120,140) 
block="IMAGE1HDR"	offset="182"	intype="Int16_B"	inlength="1"	keyword="TubeCurrent"	dicomtype="IS"	dicomtag="XRayTubeCurrent"	# Tube current (10-360) 
block="IMAGE1HDR"	offset="184"	intype="DG_Float_F"	inlength="1"	keyword="GantryAngle"	dicomtype="DS"	dicomtag="GantryDetectorTilt"			# Gantry Angle 
block="IMAGE1HDR"	offset="188"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="DS"	dicomtag="TableHeight"			# Table Height (mm) (fp) 
block="IMAGE1HDR"	offset="192"	intype="DG_Float_F"	inlength="1"	keyword="SlicePosition"	dicomtype="DS"	dicomtag="SliceLocation"	# Axial Table Location (fp) 
block="IMAGE1HDR"	offset="196"	intype="DG_Float_F"	inlength="1"	keyword="ScanTime"	dicomtype="?"	dicomtag="?"			# Scan time (1.4-9.4) (fp) 
block="IMAGE1HDR"	offset="200"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Cal phantom size (1=sm,2=md,3=la) 
block="IMAGE1HDR"	offset="202"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Cal type (1=air,2=water,3=poly) 
block="IMAGE1HDR"	offset="204"	intype="Int16_B"	inlength="1"	name="STARTOFSCANDATEMM"	dicomtype="?"	dicomtag="?"			# Start of Scan (M/D/Y) 
block="IMAGE1HDR"	offset="206"	intype="Int16_B"	inlength="1"	name="STARTOFSCANDATEDD"	dicomtype="?"	dicomtag="?"			#  
block="IMAGE1HDR"	offset="208"	intype="Int16_B"	inlength="1"	name="STARTOFSCANDATEYY"	dicomtype="?"	dicomtag="?"			#  
block="IMAGE1HDR"	offset="210"	intype="Int16_B"	inlength="1"	name="STARTOFSCANTIMEHH"	dicomtype="?"	dicomtag="?"			# Start of Scan (H:M:S) 
block="IMAGE1HDR"	offset="212"	intype="Int16_B"	inlength="1"	name="STARTOFSCANTIMEMM"	dicomtype="?"	dicomtag="?"			#  
block="IMAGE1HDR"	offset="214"	intype="Int16_B"	inlength="1"	name="STARTOFSCANTIMESS"	dicomtype="?"	dicomtag="?"			# 
block="IMAGE1HDR"	offset="216"	intype="Int16_B"	inlength="1"	name="ENDOFSCANTIMEHH"	dicomtype="?"	dicomtag="?"				# End of Scan (H:M:S)
block="IMAGE1HDR"	offset="218"	intype="Int16_B"	inlength="1"	name="ENDOFSCANTIMEMM"	dicomtype="?"	dicomtag="?"			#  
block="IMAGE1HDR"	offset="220"	intype="Int16_B"	inlength="1"	name="ENDOFSCANTIMESS"	dicomtype="?"	dicomtag="?"			# 
block="IMAGE1HDR"	offset="222"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of overranged views 
block="IMAGE1HDR"	offset="224"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # overranges 

# Axial Scans

block="IMAGE1HDR"	offset="226"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Angle of 1st view 
block="IMAGE1HDR"	offset="228"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Scout Scan # use to prescribe 
block="IMAGE1HDR"	offset="230"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Axial type (1=std,2=half,3=over) 
block="IMAGE1HDR"	offset="232"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Calcheck image number 
block="IMAGE1HDR"	offset="234"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Prep Delay in sec (fp) 
block="IMAGE1HDR"	offset="238"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Mid-scan time in sec (fp) 
block="IMAGE1HDR"	offset="242"	intype="Int16_B"	inlength="1"	keyword="RotationDirection"	dicomtype="?"	dicomtag="?"		# Gantry rotation dir (1=cw,2=ccw) 
block="IMAGE1HDR"	offset="244"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# image type (0=no,1=seg,2=tar) 
block="IMAGE1HDR"	offset="246"	intype="Int16_B"	inlength="1"	keyword="ImageSize"	dicomtype="?"	dicomtag="?"			# image size (256/320/512) 

# Scout Scans

block="IMAGE1HDR"	offset="248"	intype="DG_Float_F"	inlength="1"	name="?"			dicomtype="?"	dicomtag="?"		# table speed (37.5,75. mm/sec) (fp) 
block="IMAGE1HDR"	offset="252"	intype="Int16_B"	inlength="1"	keyword="TubeAzimuth"		dicomtype="?"	dicomtag="?"		# x-ray azimuth angle 0-360) 
block="IMAGE1HDR"	offset="254"	intype="DG_Float_F"	inlength="1"	keyword="ScoutEndLocation"	dicomtype="?"	dicomtag="?"		# scout end loc (mm) (fp) 
block="IMAGE1HDR"	offset="258"	intype="DG_Float_F"	inlength="1"	name="ViewSpacing"		dicomtype="?"	dicomtag="?"		# view spacing (mm) (fp) 

# Data Structure Information

block="IMAGE1HDR"	offset="262"	intype="Int16_B"	inlength="1"	keyword="DetectorsPerView"	dicomtype="?"	dicomtag="?"		# # data detectors (wds)/view 
block="IMAGE1HDR"	offset="264"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 1st detector # of a view 
block="IMAGE1HDR"	offset="266"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# padding/view (to block align) 
block="IMAGE1HDR"	offset="268"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# view compression factor 
block="IMAGE1HDR"	offset="270"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# total input views/scan 
block="IMAGE1HDR"	offset="272"	intype="Int16_B"	inlength="1"	keyword="ViewsPerScan"		dicomtype="?"	dicomtag="?"		# views/scan (compressed) 

# Raw Data Structure

block="IMAGE1HDR"	offset="274"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# views/frame 
block="IMAGE1HDR"	offset="276"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# views/superframe 
block="IMAGE1HDR"	offset="278"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# frame size in words 
block="IMAGE1HDR"	offset="280"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# blocks per frame 
block="IMAGE1HDR"	offset="282"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# ref channel rbn pointer 
block="IMAGE1HDR"	offset="284"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# drive # of ref channel frame 

# Recon Image Creation

block="IMAGE1HDR"	offset="286"	intype="DG_Float_F"	inlength="1"	keyword="XDiameter"	dicomtype="?"	dicomtag="?"			# X diam of recon (height) mm (fp) 
block="IMAGE1HDR"	offset="290"	intype="DG_Float_F"	inlength="1"	keyword="YDiameter"	dicomtype="?"	dicomtag="?"			# Y diam of recon (length) mm (fp) 
block="IMAGE1HDR"	offset="294"	intype="DG_Float_F"	inlength="1"	name="Pixel256"	dicomtype="?"	dicomtag="?"			# Std pixel size for image size = 256 
block="IMAGE1HDR"	offset="298"	intype="DG_Float_F"	inlength="1"	name="Pixel320"	dicomtype="?"	dicomtag="?"			# Std pixel size for image size = 320 
block="IMAGE1HDR"	offset="302"	intype="DG_Float_F"	inlength="1"	name="Pixel512"	dicomtype="?"	dicomtag="?"			# Std pixel size for image size = 512 

# Miscellaneous

block="IMAGE1HDR"	offset="306"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Floppy pixel numbers 

# Display Defaults

block="IMAGE1HDR"	offset="308"	intype="DG_Float_F"	inlength="1"	keyword="MagFactor"	dicomtype="?"	dicomtag="?"				# magnification factor (fp) 
block="IMAGE1HDR"	offset="312"	intype="DG_Float_F"	inlength="1"	keyword="XCenter"	dicomtype="?"	dicomtag="?"				# image display x center (fp) 
block="IMAGE1HDR"	offset="316"	intype="DG_Float_F"	inlength="1"	keyword="YCenter"	dicomtype="?"	dicomtag="?"				# image display y center (fp) 
block="IMAGE1HDR"	offset="320"	intype="Int16_B"	inlength="1"	keyword="WindowSetting"	dicomtype="?"	dicomtag="?"			# window (-1=default) 
block="IMAGE1HDR"	offset="322"	intype="Int16_B"	inlength="1"	keyword="LevelSetting"	dicomtype="?"	dicomtag="?"			# level (0=default) 
block="IMAGE1HDR"	offset="324"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Gamma table (-1=default) 
block="IMAGE1HDR"	offset="326"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Hdr modified flag 
block="IMAGE1HDR"	offset="328"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# checksum 
block="IMAGE1HDR"	offset="330"	intype="Int16_B"	inlength="1"	name="RECONDATEMM"	dicomtype="?"	dicomtag="?"			# Recon date (M/D/Y) 
block="IMAGE1HDR"	offset="332"	intype="Int16_B"	inlength="1"	name="RECONDATEDD"	dicomtype="?"	dicomtag="?"			#  
block="IMAGE1HDR"	offset="334"	intype="Int16_B"	inlength="1"	name="RECONDATEYY"	dicomtype="?"	dicomtag="?"			#  
block="IMAGE1HDR"	offset="336"	intype="Int16_B"	inlength="1"	name="RECONTIMEHH"	dicomtype="?"	dicomtag="?"			# Recon time (H:M:S) 
block="IMAGE1HDR"	offset="338"	intype="Int16_B"	inlength="1"	name="RECONTIMEMM"	dicomtype="?"	dicomtag="?"			#  
block="IMAGE1HDR"	offset="340"	intype="Int16_B"	inlength="1"	name="RECONTIMESS"	dicomtype="?"	dicomtag="?"			# 
block="IMAGE1HDR"	offset="342"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recon station ID 
block="IMAGE1HDR"	offset="344"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Recon unique number 
block="IMAGE1HDR"	offset="346"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# blocks in image (incl headers) 
block="IMAGE1HDR"	offset="348"	intype="Int16_B"	inlength="1"	keyword="UseImageMap"		dicomtype="?"	dicomtag="?"		# use image map (1=y,2=n (rect) 
block="IMAGE1HDR"	offset="350"	intype="String"	inlength="80"	name="?"	dicomtype="?"	dicomtag="?"				# line of free text 
block="IMAGE1HDR"	offset="430"	intype="DG_Float_F"	inlength="1"	keyword="ReconPixelSize"	dicomtype="?"	dicomtag="?"		# actual image pixel size (mm,fp) (scout), standard pixel size/target factor (recon) 
block="IMAGE1HDR"	offset="434"	intype="Int16_B"	inlength="1"	keyword="FileType"	dicomtype="?"	dicomtag="?"			# file type (1=prospective,2=scout,3=retrospective,4=segmented,5=screensave,6=plot) 
block="IMAGE1HDR"	offset="436"	intype="Int16_B"	inlength="1"	keyword="DataRange"	dicomtype="?"	dicomtag="?"			# Data range (# bits) 

# Recon information

block="IMAGE1HDR"	offset="438"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 0=std, >0 = segment # 
block="IMAGE1HDR"	offset="440"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# back proj. window (0-2) 
block="IMAGE1HDR"	offset="442"	intype="Int16_B"	inlength="1"	keyword="PreprocessReconType"	dicomtype="?"	dicomtag="?"		# preprocess target recon type 0=undefined,1=bone,2=soft tissue,3=quick soft tissue 
block="IMAGE1HDR"	offset="444"	intype="DG_Float_F"	inlength="1"	keyword="ReconXDiameter"	dicomtype="?"	dicomtag="?"		# actual x_diam recon (hgt) mm-fp 
block="IMAGE1HDR"	offset="448"	intype="DG_Float_F"	inlength="1"	keyword="ReconYDiameter"	dicomtype="?"	dicomtag="?"		# actual y_diam recon (len) mm-fp 
block="IMAGE1HDR"	offset="452"	intype="DG_Float_F"	inlength="1"	keyword="ReconXCenter"	dicomtype="?"	dicomtag="?"			# x_center of target recon mm-fp 
block="IMAGE1HDR"	offset="456"	intype="DG_Float_F"	inlength="1"	keyword="ReconYCenter"	dicomtype="?"	dicomtag="?"			# y-center of target recon mm-fp 
block="IMAGE1HDR"	offset="460"	intype="DG_Float_F"	inlength="1"	keyword="ReconMagFactor"	dicomtype="?"	dicomtag="?"		# target mag factor 

# Pre-Pre-Processing

block="IMAGE1HDR"	offset="464"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 0 = norm, 1 = halfview 
block="IMAGE1HDR"	offset="466"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# sagittal format? (0=n,1=y) 
block="IMAGE1HDR"	offset="468"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# disk image compressed? 
block="IMAGE1HDR"	offset="470"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# scout image orient (0=ap,1=lat) 
block="IMAGE1HDR"	offset="472"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# archive done flag 
block="IMAGE1HDR"	offset="474"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# image use cnt 
block="IMAGE1HDR"	offset="476"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# image removed flag 

block="IMAGE1HDR"	offset="478"	intype="String"	inlength="2"	name="?"	dicomtype="?"	dicomtag="?"				# image hdr revision code 
block="IMAGE1HDR"	offset="480"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# image map type (0=large) 
block="IMAGE1HDR"	offset="482"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# image origin (1=98,2=88,3=emi) 
block="IMAGE1HDR"	offset="484"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# image file source format 
block="IMAGE1HDR"	offset="486"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# flow image flag (0=not) 

# Image Header #2

block="IMAGE2HDR"	offset="0"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# requesting station id 
block="IMAGE2HDR"	offset="2"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# ap1 type 
block="IMAGE2HDR"	offset="4"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # creation failures 
block="IMAGE2HDR"	offset="6"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# recon-construction id 
block="IMAGE2HDR"	offset="8"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # ap's in system 
block="IMAGE2HDR"	offset="10"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# ap2 type 
block="IMAGE2HDR"	offset="12"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# birp type 
block="IMAGE2HDR"	offset="14"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # reference channels/view 
block="IMAGE2HDR"	offset="16"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# birp recon symmetry mode 
block="IMAGE2HDR"	offset="18"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # of imhproc != 0 
block="IMAGE2HDR"	offset="20"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 0=norm, 1=gated 
block="IMAGE2HDR"	offset="22"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 0=normal,1=rubout,2=son of rubout
block="IMAGE2HDR"	offset="24"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 0=normal,1=oversize,2=fatman,3=both 
block="IMAGE2HDR"	offset="26"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 0=norm,1=iterative 
block="IMAGE2HDR"	offset="28"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# fft size (0=nominal) 
block="IMAGE2HDR"	offset="30"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# source image type for targ 
block="IMAGE2HDR"	offset="32"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# source image number for targ 
block="IMAGE2HDR"	offset="34"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# first view number 
block="IMAGE2HDR"	offset="36"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 1=std,2=halfscan,3=overscan 
block="IMAGE2HDR"	offset="38"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 0=std,1=half-detector 
block="IMAGE2HDR"	offset="40"	intype="String"	inlength="20"	name="?"	dicomtype="?"	dicomtag="?"				# filename for rubout information
block="IMAGE2HDR"	offset="60"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # views in filter region 
block="IMAGE2HDR"	offset="62"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # detectors in filter region 
block="IMAGE2HDR"	offset="64"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# filter center wght 
block="IMAGE2HDR"	offset="68"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# filter subtraction wght 
block="IMAGE2HDR"	offset="72"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# min of cal and windowed data 
block="IMAGE2HDR"	offset="76"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# max of cal and windowed data 
block="IMAGE2HDR"	offset="80"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# avg of cal and windowed data 
block="IMAGE2HDR"	offset="84"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# filter function 
block="IMAGE2HDR"	offset="86"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Head Scan (0=n,1=y) 
block="IMAGE2HDR"	offset="88"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Tech factor (0=n,1=y) 
block="IMAGE2HDR"	offset="90"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 SMoothing (0=n,1=y) 
block="IMAGE2HDR"	offset="92"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Top view # 
block="IMAGE2HDR"	offset="94"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Data Flag (0=rev,1=norm,2=BH, 
block="IMAGE2HDR"	offset="96"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Dtyp (0=10,1=11,2=11x,3=12) 
block="IMAGE2HDR"	offset="98"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 old contrast flag 
block="IMAGE2HDR"	offset="100"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 0=60Hz, 1 = 50Hz 
block="IMAGE2HDR"	offset="102"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 file complete (0=y) 
block="IMAGE2HDR"	offset="104"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Bit flags for recond slices 
block="IMAGE2HDR"	offset="112"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 'SS' flag for rev >=EA 
block="IMAGE2HDR"	offset="114"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Display rev (0=old,1=new) 
block="IMAGE2HDR"	offset="116"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 used by EMI files 
block="IMAGE2HDR"	offset="118"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Pulse Width Code 
block="IMAGE2HDR"	offset="120"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Screen save mode 
block="IMAGE2HDR"	offset="122"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 EMI system code 
block="IMAGE2HDR"	offset="124"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 EMI system code 
block="IMAGE2HDR"	offset="126"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 link to raw data run # 
block="IMAGE2HDR"	offset="128"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 3 X-ray counts 
block="IMAGE2HDR"	offset="130"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 link to orig run # 
block="IMAGE2HDR"	offset="132"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 oversize recon flag 
block="IMAGE2HDR"	offset="134"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Scout DAS speed 
block="IMAGE2HDR"	offset="136"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Scout C2 table bias 
block="IMAGE2HDR"	offset="138"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Scout table elevation 
block="IMAGE2HDR"	offset="140"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Scout Pulse Width cnt 
block="IMAGE2HDR"	offset="142"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Scout Interp Compr. 
block="IMAGE2HDR"	offset="146"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# 8800 Scout Pulse Code Freq 
block="IMAGE2HDR"	offset="148"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# # recon checkpoints 
block="IMAGE2HDR"	offset="150"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Percent Xenon used 
block="IMAGE2HDR"	offset="152"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Arterial enhancement val 
block="IMAGE2HDR"	offset="156"	intype="DG_Float_F"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Arterial Rate  constant 
block="IMAGE2HDR"	offset="160"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon Start time (H:M:S) H
block="IMAGE2HDR"	offset="162"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon Start time (H:M:S) M
block="IMAGE2HDR"	offset="164"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Xenon Start time (H:M:S) S
block="IMAGE2HDR"	offset="166"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Hematocrit value 
block="IMAGE2HDR"	offset="168"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PCO2 values 1,2,3 1
block="IMAGE2HDR"	offset="170"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PCO2 values 1,2,3 2
block="IMAGE2HDR"	offset="172"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# PCO2 values 1,2,3 3
block="IMAGE2HDR"	offset="174"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Filter used in flow 
block="IMAGE2HDR"	offset="176"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Mask used in flow 
block="IMAGE2HDR"	offset="178"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Baseline scans 1,2 1
block="IMAGE2HDR"	offset="180"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# Baseline scans 1,2 2
block="IMAGE2HDR"	offset="182"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# enhancement scans 1-6 1
block="IMAGE2HDR"	offset="184"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# enhancement scans 1-6 2
block="IMAGE2HDR"	offset="186"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# enhancement scans 1-6 3
block="IMAGE2HDR"	offset="188"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# enhancement scans 1-6 4
block="IMAGE2HDR"	offset="190"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# enhancement scans 1-6 5
block="IMAGE2HDR"	offset="192"	intype="Int16_B"	inlength="1"	name="?"	dicomtype="?"	dicomtag="?"				# enhancement scans 1-6 6

constant="16"									dicomtype="US"	dicomtag="BitsAllocated"				#
constant="0"									dicomtype="US"	dicomtag="PixelRepresentation"			# Unsigned
constant="1"									dicomtype="US"	dicomtag="SamplesPerPixel"				# 
constant="MONOCHROME2"							dicomtype="CS"	dicomtag="PhotometricInterpretation"	# 
constant="-1024"								dicomtype="DS"	dicomtag="RescaleIntercept"				# 
constant="1"									dicomtype="DS"	dicomtag="RescaleSlope"					# 
constant="HU"									dicomtype="LO"	dicomtag="RescaleType"					# 
constant=""										dicomtype="SH"	dicomtag="AccessionNumber"				# 
constant=""										dicomtype="DA"	dicomtag="PatientBirthDate"				# 
constant=""										dicomtype="CS"	dicomtag="PatientSex"					# 
constant="GE Medical Systems"					dicomtype="LO"	dicomtag="Manufacturer"					# 
constant="1.2.840.10008.5.1.4.1.1.2"			dicomtype="UI"	dicomtag="SOPClassUID"					# 
constant="CT"									dicomtype="CS"	dicomtag="Modality"						# 

