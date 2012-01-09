# Offsets are in bytes from 0

# Lengths are in intype words

# Block sizes are 1024 bytes (for image header)

# data types are shared with Sytec

block="STDHDR"	offset="0"	intype="String"	inlength="4"	keyword="STUDY"		dicomtype="?"	dicomtag="?"	# Eyecatcher !STD
block="STDHDR"	offset="6"	intype="Uint8"	inlength="1"	keyword="SMODELNO_Mod"	dicomtype="?"	dicomtag="?"	enum="1=CT,2=MR,3=Reserved"	# Model Number of Device - Modality
block="STDHDR"	offset="7"	intype="Uint8"	inlength="1"	keyword="SMODELNO_Mac"	dicomtype="?"	dicomtag="?"	enum="1=CT8600,2=CT8800,3=CT9000,4=EMI,5=ImageMax,6=Resona(MR MAX),7=Quantex(Pace),8=Reserved"	# Model Number of Device - Machine
block="STDHDR"	offset="8"	intype="Uint16_B"	inlength="1"	keyword="SMACHINE"	dicomtype="LO"	dicomtag="DeviceSerialNumber"	# Manufacturers Serial Number
block="STDHDR"	offset="10"	intype="String"	inlength="5"	keyword="SSTUDYNO"	dicomtype="SH"	dicomtag="StudyID"	# Study Number
block="STDHDR"	offset="16"	intype="String_Date_YMD"	inlength="10"	keyword="SDATE"		dicomtype="?"	dicomtag="?"	# Study Date yyyy/mm/dd
block="STDHDR"	offset="26"	intype="String_Time"	inlength="12"	keyword="STIME"		dicomtype="?"	dicomtag="?"	# Study Time hh/mm/ss.xxx
block="STDHDR"	offset="38"	intype="String"	inlength="12"	keyword="SPATID"	dicomtype="LO"	dicomtag="PatientID"	# Patient ID
block="STDHDR"	offset="54"	intype="String"	inlength="12"	keyword="SPATNAME"	dicomtype="PN"	dicomtag="PatientName"	# Patient Name
block="STDHDR"	offset="80"	intype="String"	inlength="6"	keyword="SAGE"		dicomtype="?"	dicomtag="?"	# Patient Age yyy;mm
block="STDHDR"	offset="86"	intype="String"	inlength="5"	keyword="SWEIGHT"	dicomtype="DS"	dicomtag="PatientWeight"	# Patient weight kg
block="STDHDR"	offset="92"	intype="String"	inlength="2"	keyword="SSEX"		dicomtype="CS"	dicomtag="PatientSex"	enum="'M '=Male,'F '=Female"	# Patient sex
block="STDHDR"	offset="96"	intype="String"	inlength="24"	keyword="SMAKER"	dicomtype="LO"	dicomtag="Manufacturer"			# Manufacturers name
block="STDHDR"	offset="120"	intype="String"	inlength="14"	keyword="SMODELNM"	dicomtype="LO"	dicomtag="ManufacturerModelName"	# Model name
block="STDHDR"	offset="144"	intype="String"	inlength="32"	keyword="SHOSP"		dicomtype="LO"	dicomtag="InstitutionName"		# Hospital name
block="STDHDR"	offset="176"	intype="Uint8"	inlength="1"	keyword="SSYSVNO"	dicomtype="?"	dicomtag="?"	# System version
block="STDHDR"	offset="177"	intype="Uint8"	inlength="1"	keyword="SSYSRNO"	dicomtype="?"	dicomtag="?"	# System revision
block="STDHDR"	offset="178"	intype="Uint8"	inlength="1"	keyword="SDATVNO"	dicomtype="?"	dicomtag="?"	# Database version
block="STDHDR"	offset="179"	intype="Uint8"	inlength="1"	keyword="SDATRNO"	dicomtype="?"	dicomtag="?"	# Database revision
block="STDHDR"	offset="180"	intype="Uint16_B"	inlength="1"	keyword="SATFRB"	dicomtype="?"	dicomtag="?"	bitmap="0:none,SATNOWT;1:none,SATDUMP;2:none,SATOLD;3:none,SATOFF;4:none,SATNODL"	# Study attributes
block="STDHDR"	offset="188"	intype="String"	inlength="4"	keyword="SENHANCE"	dicomtype="?"	dicomtag="?"	enum="'NO C'=No Contrast,'+C  '=With Contrast"	# Contrast media
block="STDHDR"	offset="206"	intype="Pace_Date"	inlength="1"	keyword="SDATEOD"	dicomtype="?"	dicomtag="?"	# Optical Disk: Date for identification check of image number
block="STDHDR"	offset="210"	intype="Time_Milliseconds_B"	inlength="1"	keyword="STIMEOD"	dicomtype="?"	dicomtag="?"	# Optical Disk: Time for identification check of image number
block="STDHDR"	offset="214"	intype="Uint32_B"	inlength="1"	keyword="SSELFOD"	dicomtype="?"	dicomtag="?"	# Optical Disk: Number of start sectors in study information file
block="STDHDR"	offset="218"	intype="Uint16_B"	inlength="1"	keyword="SNOBLK"	dicomtype="?"	dicomtag="?"	# Number of image information blocks
block="STDHDR"	offset="220"	intype="String"	inlength="4"	keyword="SIMINFFL_Vol"	dicomtype="?"	dicomtag="?"	# File name of image information - volume name
block="STDHDR"	offset="224"	intype="Uint16_B"	inlength="1"	keyword="SIMINFFL_User"	dicomtype="?"	dicomtag="?"	# File name of image information - user number
block="STDHDR"	offset="226"	intype="String"	inlength="8"	keyword="SIMINFFL_Cat"	dicomtype="?"	dicomtag="?"	# File name of image information - catalog name
block="STDHDR"	offset="234"	intype="String"	inlength="8"	keyword="SIMINFFL_File"	dicomtype="?"	dicomtag="?"	# File name of image information - file name
block="STDHDR"	offset="242"	intype="String"	inlength="2"	keyword="SIMINFFL_Ext"	dicomtype="?"	dicomtag="?"	# File name of image information - extension
block="STDHDR"	offset="244"	intype="Uint32_B"	inlength="1"	keyword="SPOINTIM"	dicomtype="?"	dicomtag="?"	# Relative sector number of image information
block="STDHDR"	offset="248"	intype="Uint16_B"	inlength="1"	keyword="SNOSER"	dicomtype="IS"	dicomtag="SeriesInStudy"	# Number of series in study
block="STDHDR"	offset="250"	intype="Uint16_B"	inlength="1"	keyword="SNOACQ"	dicomtype="IS"	dicomtag="AcquisitionsInStudy"	# Number of acquisitions in study
block="STDHDR"	offset="252"	intype="Uint16_B"	inlength="1"	keyword="SNOIMG"	dicomtype="IS"	dicomtag="ImagesInAcquisition"	# Number of images in study
block="STDHDR"	offset="254"	intype="Uint16_B"	inlength="1"	keyword="SNEXTOD"	dicomtype="?"	dicomtag="?"	# Optical Disk: Start sector to write next image
block="STDHDR"	offset="256"	intype="String"	inlength="50"	keyword="SDESC1"	dicomtype="?"	dicomtag="?"	# Study description - line 1
block="STDHDR"	offset="306"	intype="String"	inlength="50"	keyword="SDESC2"	dicomtype="?"	dicomtag="?"	# Study description - line 2
block="STDHDR"	offset="356"	intype="String"	inlength="50"	keyword="SDESC3"	dicomtype="?"	dicomtag="?"	# Study description - line 3
block="STDHDR"	offset="406"	intype="String"	inlength="50"	keyword="SDESC4"	dicomtype="?"	dicomtag="?"	# Study description - line 4
block="STDHDR"	offset="456"	intype="String"	inlength="50"	keyword="SDESC5"	dicomtype="?"	dicomtag="?"	# Study description - line 5

block="IMGHDR"	offset="0"	intype="String"	inlength="4"	keyword="IMAGE"		dicomtype="?"	dicomtag="?"	# Eyecatcher !IMG
block="IMGHDR"	offset="4"	intype="Uint16_B"	inlength="1"	keyword="IBLOCK"	dicomtype="?"	dicomtag="?"	# Number of blocks of image information
block="IMGHDR"	offset="6"	intype="Uint8"	inlength="1"	keyword="IMODELNO_Mod"	dicomtype="?"	dicomtag="?"	enum="1=CT,2=MR,3=Reserved"	# Model Number of Device - Modality
block="IMGHDR"	offset="7"	intype="Uint8"	inlength="1"	keyword="IMODELNO_Mac"	dicomtype="?"	dicomtag="?"	enum="1=CT8600,2=CT8800,3=CT9000,4=EMI,5=ImageMax,6=Resona(MR MAX),7=Quantex(Pace),8=Reserved"	# Model Number of Device - Machine
block="IMGHDR"	offset="8"	intype="Uint16_B"	inlength="1"	keyword="IMACHINE"	dicomtype="?"	dicomtag="?"	# Manufacturers Serial Number
block="IMGHDR"	offset="10"	intype="String"	inlength="5"	keyword="ISTUDYNO"	dicomtype="?"	dicomtag="?"	# Study Number
block="IMGHDR"	offset="16"	intype="String"	inlength="2"	keyword="ISERNO"	dicomtype="IS"	dicomtag="SeriesNumber"	# Series Number
block="IMGHDR"	offset="18"	intype="String"	inlength="2"	keyword="IACQNO"	dicomtype="IS"	dicomtag="AcquisitionNumber"	# Acquisition Number
block="IMGHDR"	offset="20"	intype="String"	inlength="2"	keyword="IIMGNO"	dicomtype="IS"	dicomtag="InstanceNumber"	# Image Number
block="IMGHDR"	offset="28"	intype="Uint8"	inlength="1"	keyword="ISYSVNO"	dicomtype="?"	dicomtag="?"	# System version
block="IMGHDR"	offset="29"	intype="Uint8"	inlength="1"	keyword="ISYSRNO"	dicomtype="?"	dicomtag="?"	# System revision
block="IMGHDR"	offset="30"	intype="Uint8"	inlength="1"	keyword="IDATVNO"	dicomtype="?"	dicomtag="?"	# Database version
block="IMGHDR"	offset="31"	intype="Uint8"	inlength="1"	keyword="IDATRNO"	dicomtype="?"	dicomtag="?"	# Database revision
block="IMGHDR"	offset="32"	intype="String_Date_YMD"	inlength="10"	keyword="IDATE"		dicomtype="?"	dicomtag="?"	# Image Date yyyy/mm/dd
block="IMGHDR"	offset="42"	intype="String_Time"	inlength="12"	keyword="ITIME"		dicomtype="?"	dicomtag="?"	# Image Time hh/mm/ss.xxx
block="IMGHDR"	offset="64"	intype="String"	inlength="2"	keyword="IHFFIRST"	dicomtype="?"	dicomtag="?"	enum="'H '=Head First,'F '=Feet First"	#  Patient position
block="IMGHDR"	offset="66"	intype="String"	inlength="2"	keyword="IPATORI"	dicomtype="?"	dicomtag="?"	enum="'SP'=Supine,'PR'=Prone,'LL'=Left Lateral Decubitus,'RL'=Right Lateral Decubitus,'OT'=Other"	# Patient orientation
block="IMGHDR"	offset="68"	intype="String"	inlength="6"	keyword="IANATOMY"	dicomtype="CS"	dicomtag="BodyPartExamined"	# Anatomic location
block="IMGHDR"	offset="74"	intype="String"	inlength="2"	keyword="IAREFHF"	dicomtype="LO"	dicomtag="PositionReferenceIndicator"	# Anatomic reference HF
block="IMGHDR"	offset="76"	intype="String"	inlength="2"	keyword="IAREFAP"	dicomtype="?"	dicomtag="?"	# Anatomic reference AP
block="IMGHDR"	offset="78"	intype="String"	inlength="2"	keyword="IAREFLR"	dicomtype="?"	dicomtag="?"	# Anatomic reference LR
block="IMGHDR"	offset="80"	intype="String"	inlength="4"	keyword="IPLANE"	dicomtype="?"	dicomtag="?"	enum="'AX  '=Axial,'SAG '=Sagittal,'COR '=Coronal"	# Datum plane
block="IMGHDR"	offset="84"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISLPOSHF"	dicomtype="?"	dicomtag="?"	# Slice position by body coords HF mm
block="IMGHDR"	offset="88"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISLPOSAP"	dicomtype="?"	dicomtag="?"	# Slice position by body coords AP mm
block="IMGHDR"	offset="92"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISLPOSLR"	dicomtype="?"	dicomtag="?"	# Slice position by body coords LR mm
block="IMGHDR"	offset="96"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISCENTX"	dicomtype="?"	dicomtag="?"	# Scan center by machine coords X mm
block="IMGHDR"	offset="100"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISCENTY"	dicomtype="?"	dicomtag="?"	# Scan center by machine coords Y mm
block="IMGHDR"	offset="104"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISCENTZ"	dicomtype="?"	dicomtag="?"	# Scan center by machine coords Z mm
block="IMGHDR"	offset="108"	intype="String"	inlength="4"	keyword="ISFOV"		dicomtype="?"	dicomtag="?"	# Scan fov cm
block="IMGHDR"	offset="112"	intype="String"	inlength="4"	keyword="ISTHICK"	dicomtype="DS"	dicomtag="SliceThickness"	# Scan thickness mm
block="IMGHDR"	offset="124"	intype="IEEE_Float32_B"	inlength="1"	keyword="ITBLPOS"	dicomtype="?"	dicomtag="?"	# Table position measured from gantry (machine coords)
block="IMGHDR"	offset="128"	intype="String"	inlength="6"	keyword="ISCANSEQ"	dicomtype="?"	dicomtag="?"	# Scan sequence
block="IMGHDR"	offset="144"	intype="String"	inlength="5"	keyword="ISCANTM"	dicomtype="?"	dicomtag="?"	# Scan time CT xxx.x seconds MR mm:ss
block="IMGHDR"	offset="150"	intype="String"	inlength="4"	keyword="IMTYPE"	dicomtype="?"	dicomtag="?"	# Image type
block="IMGHDR"	offset="160"	intype="String"	inlength="4"	keyword="IENHANCE"	dicomtype="?"	dicomtag="?"	enum="'NO C'=No Contrast,'+C  '=With Contrast"	# Contrast media
block="IMGHDR"	offset="164"	intype="String_Time"	inlength="12"	keyword="ICETIME"	dicomtype="?"	dicomtag="?"	# Start Injection Time hh/mm/ss.xxx
block="IMGHDR"	offset="176"	intype="String"	inlength="4"	keyword="IGENPOW"	dicomtype="?"	dicomtag="?"	# Generator power CT xx.x kW MR xxxx W
block="IMGHDR"	offset="180"	intype="String"	inlength="4"	keyword="ITRIGGER"	dicomtype="?"	dicomtag="?"	# Trigger time: start of R wave to first data mS
block="IMGHDR"	offset="188"	intype="Uint16_B"	inlength="1"	keyword="IOVERCNT"	dicomtype="?"	dicomtag="?"	# Over range count
block="IMGHDR"	offset="190"	intype="Uint16_B"	inlength="1"	keyword="IZEROCNT"	dicomtype="?"	dicomtag="?"	# Zero detect count

block="CTHDR"  condition="isct"	offset="0"	intype="IEEE_Float32_B"	inlength="1"	keyword="ITBLHGT"	dicomtype="?"	dicomtag="?"	# Table height machine coords mm
block="CTHDR"  condition="isct"	offset="4"	intype="String"	inlength="3"	keyword="IAZIMUTH"	dicomtype="?"	dicomtag="?"	# Azimuth of scout degrees
block="CTHDR"  condition="isct"	offset="8"	intype="String"	inlength="5"	keyword="IGTILT"	dicomtype="DS"	dicomtag="GantryDetectorTilt"	# Gantry tilt machine coords degrees
block="CTHDR"  condition="isct"	offset="16"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISCSTAPS"	dicomtype="?"	dicomtag="?"	# Initial location at scout body coords mm
block="CTHDR"  condition="isct"	offset="20"	intype="IEEE_Float32_B"	inlength="1"	keyword="ISCENDPS"	dicomtype="?"	dicomtag="?"	# Final location at scout body coords mm
block="CTHDR"  condition="isct"	offset="24"	intype="String"	inlength="5"	keyword="IGTILTB"	dicomtype="?"	dicomtag="?"	# Gantry tilt body coords degrees
block="CTHDR"  condition="isct"	offset="32"	intype="String"	inlength="5"	keyword="IEXPTIME"	dicomtype="IS"	dicomtag="ExposureTime"	# Exposure time ms
block="CTHDR"  condition="isct"	offset="38"	intype="String"	inlength="3"	keyword="IEXPRATE"	dicomtype="IS"	dicomtag="XRayTubeCurrent"	# Tube current mA
block="CTHDR"  condition="isct"	offset="42"	intype="String"	inlength="5"	keyword="IEXPOSE"	dicomtype="IS"	dicomtag="Exposure"	# Exposure mAS
block="CTHDR"  condition="isct"	offset="48"	intype="String"	inlength="3"	keyword="IKVP"		dicomtype="DS"	dicomtag="KVP"	# Generator output
block="CTHDR"  condition="isct"	offset="52"	intype="String"	inlength="2"	keyword="IROTDIR"	dicomtype="CS"	dicomtag="RotationDirection"	enum="'CW'=Clockwise,'CC'=CounterClockwise"	# Direction of rotation
block="CTHDR"  condition="isct"	offset="54"	intype="String"	inlength="1"	keyword="IFILTER"	dicomtype="SH"	dicomtag="FilterType"	# XRay filter
block="CTHDR"  condition="isct"	offset="56"	intype="Pace_Date"	inlength="1"	keyword="IDATEC"	dicomtype="?"	dicomtag="?"	# Middle date of scan
block="CTHDR"  condition="isct"	offset="60"	intype="Time_Milliseconds_B"	inlength="1"	keyword="ITIMEC"	dicomtype="?"	dicomtag="?"	# Middle time of scan

block="MRHDR"  condition="ismr"	offset="0"	intype="String"	inlength="5"	keyword="IMRTILT"	dicomtype="?"	dicomtag="?"	# Tilt ordered by user Axis+/-Angle [xx+/-xx]
block="MRHDR"  condition="ismr"	offset="64"	intype="String"	inlength="2"	keyword="IECHO"		dicomtype="IS"	dicomtag="EchoNumbers"	# Echo number
block="MRHDR"  condition="ismr"	offset="66"	intype="String"	inlength="2"	keyword="INOOFECH"	dicomtype="IS"	dicomtag="EchoTrainLength"	# Number of echoes
block="MRHDR"  condition="ismr"	offset="68"	intype="String"	inlength="2"	keyword="ISLICE"	dicomtype="?"	dicomtag="?"	# Slice number
block="MRHDR"  condition="ismr"	offset="70"	intype="String"	inlength="2"	keyword="INOOFSL"	dicomtype="?"	dicomtag="?"	# Number of slices
block="MRHDR"  condition="ismr"	offset="72"	intype="String"	inlength="2"	keyword="INEX"		dicomtype="DS"	dicomtag="NumberOfAverages"	# Number of excitations
block="MRHDR"  condition="ismr"	offset="74"	intype="String"	inlength="5"	keyword="ITR"		dicomtype="DS"	dicomtag="RepetitionTime"	# Repetition time ms
block="MRHDR"  condition="ismr"	offset="80"	intype="String"	inlength="5"	keyword="ITI"		dicomtype="DS"	dicomtag="InversionTime"	# Inversion time ms
block="MRHDR"  condition="ismr"	offset="85"	intype="String"	inlength="5"	keyword="ITE"		dicomtype="DS"	dicomtag="EchoTime"	# Echo time ms
block="MRHDR"  condition="ismr"	offset="92"	intype="String"	inlength="4"	keyword="IMATRIXH"	dicomtype="?"	dicomtag="?"	# Matrix horizontal [xxxH]
block="MRHDR"  condition="ismr"	offset="96"	intype="String"	inlength="4"	keyword="IMATRIXV"	dicomtype="?"	dicomtag="?"	# Matrix vertical [xxxV]
block="MRHDR"  condition="ismr"	offset="100"	intype="String"	inlength="4"	keyword="IMATRIXD"	dicomtype="?"	dicomtag="?"	# Matrix depth [xxxD]
block="MRHDR"  condition="ismr"	offset="104"	intype="String"	inlength="1"	keyword="IRCVCOIL_Type"	dicomtype="?"	dicomtag="?"	enum="'H'=Head,'B'=Body,'S'=Surface"	# Receiving coil type
block="MRHDR"  condition="ismr"	offset="105"	intype="String"	inlength="2"	keyword="IRCVCOIL_Num"	dicomtype="?"	dicomtag="?"	# Receiving coil number
block="MRHDR"  condition="ismr"	offset="108"	intype="String"	inlength="1"	keyword="ITRSCOIL_Type"	dicomtype="?"	dicomtag="?"	enum="'H'=Head,'B'=Body,'S'=Surface"	# Transmitting coil type
block="MRHDR"  condition="ismr"	offset="109"	intype="String"	inlength="2"	keyword="ITRSCOIL_Num"	dicomtype="?"	dicomtag="?"	# Transmitting coil number
block="MRHDR"  condition="ismr"	offset="112"	intype="String"	inlength="4"	keyword="IMAGNET"	dicomtype="DS"	dicomtag="MagneticFieldStrength"	# Magnetic flux density (T)

block="IMGHDR2"	offset="0"	intype="Pace_Date"	inlength="1"	keyword="ICALDATE"	dicomtype="DA"	dicomtag="DateOfLastCalibration"	# Calibration date
block="IMGHDR2"	offset="4"	intype="Time_Milliseconds_B"	inlength="1"	keyword="ICALTIME"	dicomtype="TM"	dicomtag="TimeOfLastCalibration"	# Calibration time
block="IMGHDR2"	offset="8"	intype="String"	inlength="6"	keyword="ICALFOV"	dicomtype="?"	dicomtag="?"	# Calibration FOV name
block="IMGHDR2"	offset="16"	intype="String"	inlength="3"	keyword="IIMDIR_NX"	dicomtype="?"	dicomtag="?"	# Direction of patient on -X axis [HFAPLR]
block="IMGHDR2"	offset="19"	intype="String"	inlength="3"	keyword="IIMDIR_PY"	dicomtype="?"	dicomtag="?"	# Direction of patient on +Y axis [HFAPLR]
block="IMGHDR2"	offset="22"	intype="String"	inlength="3"	keyword="IIMDIR_PX"	dicomtype="?"	dicomtag="?"	# Direction of patient on +X axis [HFAPLR]
block="IMGHDR2"	offset="25"	intype="String"	inlength="3"	keyword="IIMDIR_NY"	dicomtype="?"	dicomtag="?"	# Direction of patient on -Y axis [HFAPLR]
block="IMGHDR2"	offset="28"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMPOSX"	dicomtype="?"	dicomtag="?"	# Image center X machine coords mm
block="IMGHDR2"	offset="32"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMPOSY"	dicomtype="?"	dicomtag="?"	# Image center Y machine coords mm
block="IMGHDR2"	offset="36"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMPOSZ"	dicomtype="?"	dicomtag="?"	# Image center Z machine coords mm
block="IMGHDR2"	offset="40"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMORIHX"	dicomtype="?"	dicomtag="?"	# Direction cosine of display horizontal vs. X machine coords
block="IMGHDR2"	offset="44"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMORIHY"	dicomtype="?"	dicomtag="?"	# Direction cosine of display horizontal vs. Y machine coords
block="IMGHDR2"	offset="48"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMORIHZ"	dicomtype="?"	dicomtag="?"	# Direction cosine of display horizontal vs. Z machine coords
block="IMGHDR2"	offset="52"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMORIVX"	dicomtype="?"	dicomtag="?"	# Direction cosine of display vertical vs. X machine coords
block="IMGHDR2"	offset="56"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMORIVY"	dicomtype="?"	dicomtag="?"	# Direction cosine of display vertical vs. Y machine coords
block="IMGHDR2"	offset="60"	intype="IEEE_Float32_B"	inlength="1"	keyword="IIMORIVZ"	dicomtype="?"	dicomtag="?"	# Direction cosine of display vertical vs. Z machine coords
block="IMGHDR2"	offset="64"	intype="String"	inlength="1"	keyword="ITCTYPE"	dicomtype="?"	dicomtag="?"	enum="'P'=Prospective,'R'=Retrospective"	# Target type
block="IMGHDR2"	offset="66"	intype="String"	inlength="6"	keyword="IRMODE"	dicomtype="SH"	dicomtag="ConvolutionKernel"	enum="'STND  '=Standard,'BONY-X'=Bony,'SOFT  '=Soft Tissue"	# Reconstruction mode
block="IMGHDR2"	offset="72"	intype="IEEE_Float32_B"	inlength="1"	keyword="IRCENTX"	dicomtype="?"	dicomtag="?"	# Recon center X mm (CT: gantry coords, MR: display coords)
block="IMGHDR2"	offset="76"	intype="IEEE_Float32_B"	inlength="1"	keyword="IRCENTY"	dicomtype="?"	dicomtag="?"	# Recon center Y mm (CT: gantry coords, MR: display coords)
block="IMGHDR2"	offset="80"	intype="String"	inlength="4"	keyword="IRFOV"		dicomtype="?"	dicomtag="?"	# Recon FOV cm [xx.x]
block="IMGHDR2"	offset="96"	intype="Uint16_B"	inlength="1"	keyword="IXAXIS"	dicomtype="US"	dicomtag="Columns"	# Pixels in X-axis
block="IMGHDR2"	offset="98"	intype="Uint16_B"	inlength="1"	keyword="IYAXIS"	dicomtype="US"	dicomtag="Rows"		# Pixels in Y-axis
block="IMGHDR2"	offset="100"	intype="IEEE_Float32_B"	inlength="1"	keyword="IPIXSIZE"	dicomtype="?"	dicomtag="?"	# Pixel size mm
block="IMGHDR2"	offset="104"	intype="Int16_B"	inlength="1"	keyword="IWINDOWC"	dicomtype="DS"	dicomtag="WindowCenter"	# Window center
block="IMGHDR2"	offset="106"	intype="Int16_B"	inlength="1"	keyword="IWINDOWL"	dicomtype="DS"	dicomtag="WindowWidth"	# Window level
block="IMGHDR2"	offset="108"	intype="Uint16_B"	inlength="1"	keyword="IATTRB"	dicomtype="?"	dicomtag="?"	bitmap="0:none,IATDUMP - Dumping is over;1:none,IATOLD - Old image reloading;2:none,IATOFFR - Off center recon;3:IATWLED - Window level enable,IATWLED - Window level disable;4:none,IATNOWT - Image exists on OD;5:ATPTYP - DPCM,ATPTYP - ORIGINAL;15:IATHZ - 50Hz,IATHZ - 60Hz"	# Image attributes
block="IMGHDR2"	offset="112"	intype="IEEE_Float32_B"	inlength="1"	keyword="IMAGCX"	dicomtype="?"	dicomtag="?"	# Mag center X mm display coords
block="IMGHDR2"	offset="116"	intype="IEEE_Float32_B"	inlength="1"	keyword="IMAGCY"	dicomtype="?"	dicomtag="?"	# Mag center Y mm display coords
block="IMGHDR2"	offset="120"	intype="IEEE_Float32_B"	inlength="1"	keyword="IMAGFACT"	dicomtype="?"	dicomtag="?"	# Mag factor
block="IMGHDR2"	offset="124"	intype="Uint16_B"	inlength="1"	keyword="ITRANS"	dicomtype="?"	dicomtag="?"	bitmap="0:none,ITRRVSRL - R/L reverse;1:none,ITRRVSTB - T/B reverse;2:none,ITRROTCW - Clockwise 90 rotate;3:none,ITRROTCCW - CounterClockwise 90 rotate"	# Transient information - orientation of display image
block="IMGHDR2"	offset="128"	intype="String"	inlength="2"	keyword="IREFIMG1_Ser"	dicomtype="?"	dicomtag="?"	# ID of reference image 1 - series
block="IMGHDR2"	offset="131"	intype="String"	inlength="2"	keyword="IREFIMG1_Acq"	dicomtype="?"	dicomtag="?"	# ID of reference image 1 - acquisition
block="IMGHDR2"	offset="134"	intype="String"	inlength="2"	keyword="IREFIMG1_Img"	dicomtype="?"	dicomtag="?"	# ID of reference image 1 - image
block="IMGHDR2"	offset="136"	intype="String"	inlength="2"	keyword="IREFIMG2_Ser"	dicomtype="?"	dicomtag="?"	# ID of reference image 2 - series
block="IMGHDR2"	offset="139"	intype="String"	inlength="2"	keyword="IREFIMG2_Acq"	dicomtype="?"	dicomtag="?"	# ID of reference image 2 - acquisition
block="IMGHDR2"	offset="142"	intype="String"	inlength="2"	keyword="IREFIMG2_Img"	dicomtype="?"	dicomtag="?"	# ID of reference image 2 - image
block="IMGHDR2"	offset="144"	intype="String"	inlength="2"	keyword="IREFIMG3_Ser"	dicomtype="?"	dicomtag="?"	# ID of reference image 3 - series
block="IMGHDR2"	offset="147"	intype="String"	inlength="2"	keyword="IREFIMG3_Acq"	dicomtype="?"	dicomtag="?"	# ID of reference image 3 - acquisition
block="IMGHDR2"	offset="150"	intype="String"	inlength="2"	keyword="IREFIMG3_Img"	dicomtype="?"	dicomtag="?"	# ID of reference image 3 - image
block="IMGHDR2"	offset="152"	intype="Uint8"	inlength="1"	keyword="IMASKIMG_Modality"	dicomtype="?"	dicomtag="?"	enum="1=CT,2=MR,3=Reserved"	# ID of mask image - modality
block="IMGHDR2"	offset="153"	intype="Uint8"	inlength="1"	keyword="IMASKIMG_Model"	dicomtype="?"	dicomtag="?"	enum="1=CT8600,2=CT8800,3=CT9000,4=EMI,5=ImageMax,6=Resona(MR MAX),7=Quantex(Pace),8=Reserved"	# ID of mask image - model number
block="IMGHDR2"	offset="154"	intype="Uint16_B"	inlength="1"	keyword="IMASKIMG_Serial"	dicomtype="?"	dicomtag="?"	# ID of mask image - serial number
block="IMGHDR2"	offset="156"	intype="String"	inlength="3"	keyword="IMASKIMG_Std"		dicomtype="?"	dicomtag="?"	# ID of mask image - study
block="IMGHDR2"	offset="159"	intype="Uint8"	inlength="1"	keyword="IMASKIMG_Ser"		dicomtype="?"	dicomtag="?"	# ID of mask image - series
block="IMGHDR2"	offset="160"	intype="Uint8"	inlength="1"	keyword="IMASKIMG_Acq"		dicomtype="?"	dicomtag="?"	# ID of mask image - acquisition
block="IMGHDR2"	offset="161"	intype="Uint8"	inlength="1"	keyword="IMASKIMG_Img"		dicomtype="?"	dicomtag="?"	# ID of mask image - image
block="IMGHDR2"	offset="210"	intype="Uint32_B"	inlength="1"	keyword="ISELFOD"	dicomtype="?"	dicomtag="?"	# OD: Start sector of image information
block="IMGHDR2"	offset="214"	intype="Uint16_B"	inlength="1"	keyword="IBITPLNH"	dicomtype="?"	dicomtag="?"	# Screen save bitplane pixels horizontal
block="IMGHDR2"	offset="216"	intype="Uint16_B"	inlength="1"	keyword="IBITPLNV"	dicomtype="?"	dicomtag="?"	# Screen save bitplane pixels vertical
block="IMGHDR2"	offset="218"	intype="Uint32_B"	inlength="1"	keyword="IPACKIM"	dicomtype="?"	dicomtag="?"	# Bytes of packed image data
block="IMGHDR2"	offset="224"	intype="Uint32_B"	inlength="1"	keyword="IPACKSZ"	dicomtype="?"	dicomtag="?"	# Bytes of packed image file
block="IMGHDR2"	offset="228"	intype="String"	inlength="4"	keyword="IIMFILE_Vol"	dicomtype="?"	dicomtag="?"	# File name of image information - volume name
block="IMGHDR2"	offset="232"	intype="Uint16_B"	inlength="1"	keyword="IIMFILE_User"	dicomtype="?"	dicomtag="?"	# File name of image information - user number
block="IMGHDR2"	offset="234"	intype="String"	inlength="8"	keyword="IIMFILE_Cat"	dicomtype="?"	dicomtag="?"	# File name of image information - catalog name
block="IMGHDR2"	offset="242"	intype="String"	inlength="8"	keyword="IIMFILE_File"	dicomtype="?"	dicomtag="?"	# File name of image information - file name
block="IMGHDR2"	offset="250"	intype="String"	inlength="2"	keyword="IIMFILE_Ext"	dicomtype="?"	dicomtag="?"	# File name of image information - extension
block="IMGHDR2"	offset="252"	intype="Uint32_B"	inlength="1"	keyword="IPOINTIM"	dicomtype="?"	dicomtag="?"	# Disk: image pointer number; OD: relative sector number of image data
block="IMGHDR2"	offset="256"	intype="String"	inlength="16"	keyword="ISRDESC1"	dicomtype="?"	dicomtag="?"	# Series description - part 1
block="IMGHDR2"	offset="272"	intype="String"	inlength="14"	keyword="ISRDESC2"	dicomtype="?"	dicomtag="?"	# Series description - part 2
block="IMGHDR2"	offset="286"	intype="String"	inlength="12"	keyword="ISRDESC3"	dicomtype="?"	dicomtag="?"	# Series description - part 3
block="IMGHDR2"	offset="298"	intype="String"	inlength="10"	keyword="IIMDESC1"	dicomtype="?"	dicomtag="?"	# Image description - part 1
block="IMGHDR2"	offset="308"	intype="String"	inlength="8"	keyword="IIMDESC2"	dicomtype="?"	dicomtag="?"	# Image description - part 2
block="IMGHDR2"	offset="316"	intype="String"	inlength="7"	keyword="IIMDESC3"	dicomtype="?"	dicomtag="?"	# Image description - part 3
block="IMGHDR2"	offset="324"	intype="String"	inlength="17"	keyword="IEXDESC1"	dicomtype="?"	dicomtag="?"	# Extension description - line 1
block="IMGHDR2"	offset="341"	intype="String"	inlength="17"	keyword="IEXDESC2"	dicomtype="?"	dicomtag="?"	# Extension description - line 2
block="IMGHDR2"	offset="358"	intype="String"	inlength="17"	keyword="IEXDESC3"	dicomtype="?"	dicomtag="?"	# Extension description - line 3
block="IMGHDR2"	offset="375"	intype="String"	inlength="17"	keyword="IEXDESC4"	dicomtype="?"	dicomtag="?"	# Extension description - line 4
block="IMGHDR2"	offset="392"	intype="String"	inlength="17"	keyword="IEXDESC5"	dicomtype="?"	dicomtag="?"	# Extension description - line 5
block="IMGHDR2"	offset="409"	intype="String"	inlength="17"	keyword="IEXDESC6"	dicomtype="?"	dicomtag="?"	# Extension description - line 6
block="IMGHDR2"	offset="426"	intype="String"	inlength="17"	keyword="IEXDESC7"	dicomtype="?"	dicomtag="?"	# Extension description - line 7
block="IMGHDR2"	offset="443"	intype="String"	inlength="17"	keyword="IEXDESC8"	dicomtype="?"	dicomtag="?"	# Extension description - line 8
block="IMGHDR2"	offset="460"	intype="String"	inlength="17"	keyword="IEXDESC9"	dicomtype="?"	dicomtag="?"	# Extension description - line 9
block="IMGHDR2"	offset="477"	intype="String"	inlength="17"	keyword="IEXDESC10"	dicomtype="?"	dicomtag="?"	# Extension description - line 10
block="IMGHDR2"	offset="494"	intype="String"	inlength="17"	keyword="IEXDESC11"	dicomtype="?"	dicomtag="?"	# Extension description - line 11
block="IMGHDR2"	offset="511"	intype="String"	inlength="17"	keyword="IEXDESC12"	dicomtype="?"	dicomtag="?"	# Extension description - line 12
block="IMGHDR2"	offset="528"	intype="String"	inlength="17"	keyword="IEXDESC13"	dicomtype="?"	dicomtag="?"	# Extension description - line 13
block="IMGHDR2"	offset="545"	intype="String"	inlength="17"	keyword="IEXDESC14"	dicomtype="?"	dicomtag="?"	# Extension description - line 14
block="IMGHDR2"	offset="562"	intype="String"	inlength="17"	keyword="IEXDESC15"	dicomtype="?"	dicomtag="?"	# Extension description - line 15
block="IMGHDR2"	offset="579"	intype="String"	inlength="17"	keyword="IEXDESC16"	dicomtype="?"	dicomtag="?"	# Extension description - line 16
block="IMGHDR2"	offset="596"	intype="String"	inlength="17"	keyword="IEXDESC17"	dicomtype="?"	dicomtag="?"	# Extension description - line 17
block="IMGHDR2"	offset="613"	intype="String"	inlength="17"	keyword="IEXDESC18"	dicomtype="?"	dicomtag="?"	# Extension description - line 18
block="IMGHDR2"	offset="630"	intype="String"	inlength="17"	keyword="IEXDESC19"	dicomtype="?"	dicomtag="?"	# Extension description - line 19
block="IMGHDR2"	offset="647"	intype="String"	inlength="17"	keyword="IEXDESC20"	dicomtype="?"	dicomtag="?"	# Extension description - line 20

constant="0" 		dicomtype="US"	dicomtag="PixelPaddingValue"		# 
constant="16" 		dicomtype="US"	dicomtag="BitsAllocated"		# 
constant="16" 		dicomtype="US"	dicomtag="BitsStored"			# 
constant="15" 		dicomtype="US"	dicomtag="HighBit"			# 
constant="1" 		dicomtype="US"	dicomtag="PixelRepresentation"		# Signed
constant="1" 		dicomtype="US"	dicomtag="SamplesPerPixel"		# 
constant="MONOCHROME2" 	dicomtype="CS"	dicomtag="PhotometricInterpretation"	# 
constant="0" 		dicomtype="DS"	dicomtag="RescaleIntercept"		# 
constant="1" 		dicomtype="DS"	dicomtag="RescaleSlope"			# 
constant="" 		dicomtype="PN"	dicomtag="ReferringPhysicianName"	# 
constant=""		dicomtype="PN"	dicomtag="PerformingPhysicianName"	# 
constant=""		dicomtype="PN"	dicomtag="OperatorsName"			# 
constant=""		dicomtype="SH"	dicomtag="AccessionNumber"		# 
constant=""		dicomtype="DA"	dicomtag="PatientBirthDate"		# 
