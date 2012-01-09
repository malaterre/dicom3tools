# Offsets are in bytes from 0
# Lengths are in intype units

block="IMGHDR"	offset="0"	intype="String"		inlength="4"	keyword="CONT"		dicomtype="?"	dicomtag="?"		# Controller label
block="IMGHDR"	offset="4"	intype="String"		inlength="16"	keyword="KKEYNAME"	dicomtype="?"	dicomtag="?"		# Retrieval key name
block="IMGHDR"	offset="20"	intype="String"		inlength="8"	keyword="KKEYEXT"	dicomtype="?"	dicomtag="?"		# Retrieval key extension
block="IMGHDR"	offset="28"	intype="Uint32_B"	inlength="1"	keyword="DATE"		dicomtype="?"	dicomtag="?"		# Date of preparation
block="IMGHDR"	offset="32"	intype="Uint32_B"	inlength="1"	keyword="TIME"		dicomtype="?"	dicomtag="?"		# Time of preparation
block="IMGHDR"	offset="36"	intype="Uint32_B"	inlength="1"	keyword="SLBN"		dicomtype="?"	dicomtag="?"		# Top logical block num in MD
block="IMGHDR"	offset="40"	intype="Uint32_B"	inlength="1"	keyword="ELBN"		dicomtype="?"	dicomtag="?"		# End logical block num in MD
block="IMGHDR"	offset="44"	intype="Uint32_B"	inlength="1"	keyword="BKSZ"		dicomtype="?"	dicomtag="?"		# Number of data-full blocks (256 byte)
block="IMGHDR"	offset="48"	intype="Uint16_B"	inlength="1"	keyword="IDCT"		dicomtype="?"	dicomtag="?"		# Number of extensions IDs
block="IMGHDR"	offset="50"	intype="Uint16_B"	inlength="1"	keyword="UTNO"		dicomtype="SH"	dicomtag="StationName"		# Unit number
block="IMGHDR"	offset="52"	intype="Uint16_B"	inlength="1"	keyword="OPMD"		dicomtype="?"	dicomtag="?"		# Open mode
block="IMGHDR"	offset="54"	intype="Uint32_B"	inlength="1"	keyword="USEB"		dicomtype="?"	dicomtag="?"		# End peak logical block number
block="IMGHDR"	offset="58"	intype="String"		inlength="4"	keyword="OPSN"		dicomtype="?"	dicomtag="?"		# Open sign (== OPEN)
block="IMGHDR"	offset="62"	intype="Uint16_B"	inlength="1"	keyword="BKSU"		dicomtype="?"	dicomtag="?"		# Number of blocks of one-data unit
block="IMGHDR"	offset="64"	intype="Uint16_B"	inlength="1"	keyword="DTSU"		dicomtype="?"	dicomtag="?"		# Number of data units
block="IMGHDR"	offset="66"	intype="Uint16_B"	inlength="1"	keyword="MATX"		dicomtype="US"	dicomtag="Columns"	# X-directional matrix size
block="IMGHDR"	offset="68"	intype="Uint16_B"	inlength="1"	keyword="MATY"		dicomtype="US"	dicomtag="Rows"		# Y-directional matrix size
block="IMGHDR"	offset="70"	intype="Uint8"		inlength="1"	keyword="DTTP"		dicomtype="?"	dicomtag="?"		# Attribute data type
block="IMGHDR"	offset="71"	intype="Uint8"		inlength="1"	keyword="ASGF"		dicomtype="?"	dicomtag="?"		# Attribute compression method/pixel depth
block="IMGHDR"	offset="72"	intype="Uint16_B"	inlength="1"	keyword="GASY"		dicomtype="?"	dicomtag="?"		# Attribute image kind
block="IMGHDR"	offset="74"	intype="Uint16_B"	inlength="1"	keyword="GASZ"		dicomtype="?"	dicomtag="?"		# Attribute pixel size (um)
block="IMGHDR"	offset="76"	intype="Uint8"		inlength="1"	keyword="RWMD"		dicomtype="?"	dicomtag="?"		# Image compression
block="IMGHDR"	offset="77"	intype="Uint8"		inlength="1"	keyword="YOBI"		dicomtype="?"	dicomtag="?"		# spare
block="IMGHDR"	offset="78"	intype="Uint16_B"	inlength="1"	keyword="PRCD"		dicomtype="?"	dicomtag="?"		# Process code
block="IMGHDR"	offset="80"	intype="Uint16_B"	inlength="1"	keyword="MODE"		dicomtype="?"	dicomtag="?"		# Mode (open)
block="IMGHDR"	offset="82"	intype="Uint16_B"	inlength="1"	keyword="FRAM"		dicomtype="?"	dicomtag="?"		# Frame number
block="IMGHDR"	offset="84"	intype="Uint16_B"	inlength="1"	keyword="BUNK"		dicomtype="?"	dicomtag="?"		# Number of divisions
block="IMGHDR"	offset="86"	intype="Uint16_B"	inlength="1"	keyword="SECT"		dicomtype="?"	dicomtag="?"		# Section number
block="IMGHDR"	offset="88"	intype="Uint16_B"	inlength="1"	keyword="DMW6"		dicomtype="?"	dicomtag="?"		# -
block="IMGHDR"	offset="90"	intype="Uint16_B"	inlength="1"	keyword="KMMR"		dicomtype="?"	dicomtag="?"		# Provisionally assigned record
block="IMGHDR"	offset="92"	intype="Uint16_B"	inlength="1"	keyword="EFRA"		dicomtype="?"	dicomtag="?"		# Trimmed end frame
block="IMGHDR"	offset="94"	intype="Uint16_B"	inlength="1"	keyword="DMW9"		dicomtype="?"	dicomtag="?"		# -
block="IMGHDR"	offset="96"	intype="Uint16_B"	inlength="1"	keyword="DMWA"		dicomtype="?"	dicomtag="?"		# -
block="IMGHDR"	offset="98"	intype="Uint16_B"	inlength="1"	keyword="RETS"		dicomtype="?"	dicomtag="?"		# Return status
block="IMGHDR"	offset="100"	intype="String"		inlength="4"	keyword="XISYS"		dicomtype="?"	dicomtag="?"		# System ID
block="IMGHDR"	offset="104"	intype="String"		inlength="4"	keyword="XIVN"		dicomtype="LO"	dicomtag="SoftwareVersions"		# Version number
block="IMGHDR"	offset="108"	intype="String"		inlength="1"	keyword="XIKND"		dicomtype="?"	dicomtag="?"	enum="'I'=Image,'R'=Raw data"	# Data kind
block="IMGHDR"	offset="109"	intype="String"		inlength="1"	keyword="XISCAN"	dicomtype="?"	dicomtag="?"	enum="'S'=Scanogram,'C'=Normal scan image"	# Scanogram code
block="IMGHDR"	offset="110"	intype="String"		inlength="2"	keyword="MISQ"		dicomtype="?"	dicomtag="?"	enum="'SR'=SR sequence (same as SE),'SE'=Spin echo,'IR'=Inversion recovery,'GE'=Gradient echo,'GR'=Gradient echo with rephasing"	# Pulse sequence
block="IMGHDR"	offset="112"	intype="String"		inlength="1"	keyword="MIITYP"	dicomtype="?"	dicomtag="?"	enum="'A'=Absolute value image,'R'=Real part image,'I'=Imaginary part image,'P'=Phase image"	# Image data type
block="IMGHDR"	offset="113"	intype="String"		inlength="1"	keyword="MICAL"		dicomtype="?"	dicomtag="?"	enum="'0'=Proton density image,'1'=T1 value image,'2'=T2 value image,' '=Normal"	# Calculation image
block="IMGHDR"	offset="114"	intype="String"		inlength="1"	keyword="XISUB"		dicomtype="?"	dicomtag="?"	enum="'S'=Subtraction image,' '=Normal"	# Subtraction image
block="IMGHDR"	offset="115"	intype="String"		inlength="1"	keyword="MISYN"		dicomtype="?"	dicomtag="?"	enum="'S'=Synthesized image,' '=Normal"	# Synthesized image
block="IMGHDR"	offset="116"	intype="String"		inlength="1"	keyword="MIANA"		dicomtype="?"	dicomtag="?"	enum="'A'=Analysis image,' '=Normal"	# Analysis image (3D display etc.)
block="IMGHDR"	offset="117"	intype="String"		inlength="1"	keyword="XIY3"		dicomtype="?"	dicomtag="?"		# spare
block="IMGHDR"	offset="118"	intype="String"		inlength="2"	keyword="MIICLS"	dicomtype="?"	dicomtag="?"	enum="'R '=Image with rephasing,'TF'=Image of TOF angiography"	# Image kind code
block="IMGHDR"	offset="120"	intype="String"		inlength="1"	keyword="XIPRS"		dicomtype="?"	dicomtag="?"	enum="' '=normal"	# Image compression code (1 to 6)
block="IMGHDR"	offset="121"	intype="String"		inlength="1"	keyword="XIY61"		dicomtype="?"	dicomtag="?"		# Orientation indication for MPR (R,L,A,P)
block="IMGHDR"	offset="122"	intype="String"		inlength="4"	keyword="XISYTP"	dicomtype="LO"	dicomtag="ManufacturerModelName"	enum="'MR70'=MRP7000,'MR20'=MRP20"	# System type
block="IMGHDR"	offset="126"	intype="String"		inlength="2"	keyword="XIY64"		dicomtype="?"	dicomtag="?"		# spare
block="IMGHDR"	offset="128"	intype="String"		inlength="20"	keyword="XIHP"	dicomtype="LO"	dicomtag="InstitutionName"	# Hospital name
block="IMGHDR"	offset="148"	intype="String"		inlength="26"	keyword="XINM"	dicomtype="PN"	dicomtag="PatientName"		# Patients name
block="IMGHDR"	offset="174"	intype="String"		inlength="9"	keyword="XIPN"	dicomtype="?"	dicomtag="?"			# Patients ID number
block="IMGHDR"	offset="183"	intype="String"		inlength="1"	keyword="XIY7"	dicomtype="?"	dicomtag="?"			# spare
block="IMGHDR"	offset="184"	intype="String"		inlength="4"	keyword="XISN"	dicomtype="IS"	dicomtag="InstanceNumber"	# Slice number
block="IMGHDR"	offset="188"	intype="String"		inlength="1"	keyword="XIAB"	dicomtype="?"	dicomtag="?"	enum="'H'=Cardiac gate,'R'=Respiratory gate,'T'=3D display,'W'=Contour image for 3D display,'A'=Angiogram,'M'=MPR image"	# Slice kind (for display)
block="IMGHDR"	offset="189"	intype="String"		inlength="1"	keyword="XISX"	dicomtype="CS"	dicomtag="PatientSex"		# Patients sex
block="IMGHDR"	offset="190"	intype="String"		inlength="2"	keyword="XIBYY"	dicomtype="?"	dicomtag="?"			# Birthday year
block="IMGHDR"	offset="192"	intype="String"		inlength="2"	keyword="XIBMM"	dicomtype="?"	dicomtag="?"			# Birthday month
block="IMGHDR"	offset="194"	intype="String"		inlength="2"	keyword="XIBDD"	dicomtype="?"	dicomtag="?"			# Birthday day
block="IMGHDR"	offset="196"	intype="String"		inlength="2"	keyword="XIYY"	dicomtype="?"	dicomtag="?"			# Exam year
block="IMGHDR"	offset="198"	intype="String"		inlength="2"	keyword="XIMM"	dicomtype="?"	dicomtag="?"			# Exam month
block="IMGHDR"	offset="200"	intype="String"		inlength="2"	keyword="XIDD"	dicomtype="?"	dicomtag="?"			# Exam day
block="IMGHDR"	offset="202"	intype="String"		inlength="2"	keyword="XITH"	dicomtype="?"	dicomtag="?"			# Hours (hours)
block="IMGHDR"	offset="204"	intype="String"		inlength="2"	keyword="XITM"	dicomtype="?"	dicomtag="?"			# Hours (minutes)
block="IMGHDR"	offset="206"	intype="String"		inlength="2"	keyword="XITS"	dicomtype="?"	dicomtag="?"			# Hours (seconds)
block="IMGHDR"	offset="208"	intype="String"		inlength="2"	keyword="XISTH"	dicomtype="?"	dicomtag="?"			# Stopwatch (hours)
block="IMGHDR"	offset="210"	intype="String"		inlength="2"	keyword="XISTM"	dicomtype="?"	dicomtag="?"			# Stopwatch (minutes)
block="IMGHDR"	offset="212"	intype="String"		inlength="2"	keyword="XISTS"	dicomtype="?"	dicomtag="?"			# Stopwatch (seconds)
block="IMGHDR"	offset="214"	intype="String"		inlength="4"	keyword="XISLS"	dicomtype="DS"	dicomtag="SliceThickness"	# Slice thickness (mm)
block="IMGHDR"	offset="218"	intype="String"		inlength="4"	keyword="XITR"	dicomtype="DS"	dicomtag="RepetitionTime"	# TR (ms)
block="IMGHDR"	offset="222"	intype="String"		inlength="1"	keyword="XIST"	dicomtype="?"	dicomtag="?"	enum="'S'=yes,' '=no"	# Stopwatch designation
block="IMGHDR"	offset="223"	intype="String"		inlength="1"	keyword="XICM"	dicomtype="?"	dicomtag="?"	enum="'C'=yes,' '=no"	# Use of contrast media
block="IMGHDR"	offset="224"	intype="String"		inlength="3"	keyword="MITE"	dicomtype="DS"	dicomtag="EchoTime"	# TE (ms)
block="IMGHDR"	offset="227"	intype="String"		inlength="1"	keyword="MISNDRCT"	dicomtype="?"	dicomtag="?"		# Scan direction code
block="IMGHDR"	offset="228"	intype="String"		inlength="3"	keyword="MITI"	dicomtype="DS"	dicomtag="InversionTime"	# TI (ms)
block="IMGHDR"	offset="231"	intype="String"		inlength="1"	keyword="XIIROT"	dicomtype="?"	dicomtag="?"	enum="' '=initial,'0'=during processing,'1'=during processing,'2'=during processing,'3'=during processing"	# Image rotation state code
block="IMGHDR"	offset="232"	intype="String"		inlength="6"	keyword="XIPOS"	dicomtype="?"	dicomtag="?"			# Slice position (0.1mm)


block="IMGHDR"	offset="256"	intype="String"		inlength="3"	keyword="MIMSMAX"	dicomtype="IS"	dicomtag="ImagesInAcquisition"	# Number of multislices
block="IMGHDR"	offset="259"	intype="String"		inlength="1"	keyword="MIMEMAX"	dicomtype="IS"	dicomtag="EchoTrainLength"	# Number of multiechoes
block="IMGHDR"	offset="260"	intype="String"		inlength="3"	keyword="MIMS"		dicomtype="IS"	dicomtag="AcquisitionNumber"	# Multislices number
block="IMGHDR"	offset="263"	intype="String"		inlength="1"	keyword="MIME"		dicomtype="IS"	dicomtag="EchoNumbers"		# Multiecho number
block="IMGHDR"	offset="264"	intype="String"		inlength="4"	keyword="XIMTX"	dicomtype="?"	dicomtag="?"			# Reconstruction matrix
block="IMGHDR"	offset="268"	intype="String"		inlength="1"	keyword="MISLPL"	dicomtype="?"	dicomtag="?"	enum="'S'=Sagittal,'C'=Coronal,'T'=Transverse"	# Slice plane
block="IMGHDR"	offset="269"	intype="String"		inlength="1"	keyword="MIOBL"	dicomtype="?"	dicomtag="?"	enum="' '=Non oblique,'1'=Single oblique,'2'=Double oblique"	# Oblique flag
block="IMGHDR"	offset="270"	intype="String"		inlength="1"	keyword="XIRL"	dicomtype="?"	dicomtag="?"	enum="' '=Not transverse,'L'=Right of image is left of patient,'R'=Right of image is right of patient"	# Oblique flag
block="IMGHDR"	offset="271"	intype="String"		inlength="1"	keyword="XIHVIW"	dicomtype="?"	dicomtag="?"	enum="'H'=Toward head (top view),'B'=Toward foot (bottom view)"	# Display direction of head image
block="IMGHDR"	offset="272"	intype="String"		inlength="2"	keyword="XIPSD"	dicomtype="?"	dicomtag="?"	enum="'HF'=Head first,'FF'=Feet first"	# Patient inserting direction
block="IMGHDR"	offset="274"	intype="String"		inlength="2"	keyword="XIPSP"	dicomtype="?"	dicomtag="?"	enum="'S '=Supine,'P '=Prone,'LD'=Left decubitus,'RD'=Right decubitus"	# Patient position
block="IMGHDR"	offset="276"	intype="String"		inlength="26"	keyword="XICMT1"	dicomtype="?"	dicomtag="?"		# Comment in 1st line
block="IMGHDR"	offset="302"	intype="String"		inlength="26"	keyword="XICMT2"	dicomtype="?"	dicomtag="?"		# Comment in 2nd line
block="IMGHDR"	offset="328"	intype="String"		inlength="26"	keyword="XICMT3"	dicomtype="?"	dicomtag="?"		# Comment in 3rd line
block="IMGHDR"	offset="354"	intype="String"		inlength="26"	keyword="XICMT4"	dicomtype="?"	dicomtag="?"		# Comment in 4th line
block="IMGHDR"	offset="380"	intype="String"		inlength="26"	keyword="XICMT5"	dicomtype="?"	dicomtag="?"		# Comment in 5th line
block="IMGHDR"	offset="406"	intype="String"		inlength="1"	keyword="XIPROTS"	dicomtype="?"	dicomtag="?"		# Protocol Number 1st digit (Sequence)
block="IMGHDR"	offset="407"	intype="String"		inlength="1"	keyword="XIPROTR"	dicomtype="?"	dicomtag="?"		# Protocol Number 2nd digit (Region)
block="IMGHDR"	offset="408"	intype="Uint16_B"	inlength="1"	keyword="XIPUL"		dicomtype="?"	dicomtag="?"		# Protocol upper window level
block="IMGHDR"	offset="410"	intype="Uint16_B"	inlength="1"	keyword="XIPUW"		dicomtype="?"	dicomtag="?"		# Protocol upper window width
block="IMGHDR"	offset="412"	intype="Uint16_B"	inlength="1"	keyword="XIPLL"		dicomtype="?"	dicomtag="?"		# Protocol lower window level
block="IMGHDR"	offset="414"	intype="Uint16_B"	inlength="1"	keyword="XIPLW"		dicomtype="?"	dicomtag="?"		# Protocol lower window width
block="IMGHDR"	offset="416"	intype="Uint16_B"	inlength="1"	keyword="XIPSX"		dicomtype="?"	dicomtag="?"		# Original image pixel size X in um
block="IMGHDR"	offset="418"	intype="Uint16_B"	inlength="1"	keyword="XIPSY"		dicomtype="?"	dicomtag="?"		# Original image pixel size Y in um
block="IMGHDR"	offset="420"	intype="Uint16_B"	inlength="1"	keyword="XIMAG"		dicomtype="?"	dicomtag="?"		# Magnification factor x1000
block="IMGHDR"	offset="422"	intype="Uint16_B"	inlength="1"	keyword="XIZCNX"		dicomtype="?"	dicomtag="?"		# X direction mag center deviation (in 512 matrix, x16)
block="IMGHDR"	offset="424"	intype="Uint16_B"	inlength="1"	keyword="XIZCNY"		dicomtype="?"	dicomtag="?"		# Y direction mag center deviation (in 512 matrix, x16)
block="IMGHDR"	offset="426"	intype="String"		inlength="2"	keyword="XISHAP"	dicomtype="?"	dicomtag="?"	enum="'+ '=Enhancement,'- '=Smoothing"	# Edge enhancement
block="IMGHDR"	offset="428"	intype="String"		inlength="4"	keyword="XIDFOV"	dicomtype="DS"	dicomtag="ReconstructionDiameter"	# Display field of view
block="IMGHDR"	offset="432"	intype="String"		inlength="1"	keyword="XILNNO"	dicomtype="?"	dicomtag="?"		# spare
block="IMGHDR"	offset="433"	intype="String"		inlength="1"	keyword="XILNDP"	dicomtype="?"	dicomtag="?"	enum="'T'=slice position,'N'=slice number"	# scan line display class
block="IMGHDR"	offset="434"	intype="String"		inlength="6"	keyword="XISPO"		dicomtype="?"	dicomtag="?"		# Start slice position

block="IMGHDR"	offset="736"	intype="Uint32_B"	inlength="1"	keyword="MIFREQOE"	dicomtype="?"	dicomtag="?"		# Center frequency (END)
block="IMGHDR"	offset="740"	intype="Uint16_B"	inlength="1"	keyword="MISMPLTM"	dicomtype="?"	dicomtag="?"		# Sample time (ms)
block="IMGHDR"	offset="742"	intype="String"		inlength="4"	keyword="XISCSN1"	dicomtype="?"	dicomtag="?"		# 1st select slice number
block="IMGHDR"	offset="746"	intype="String"		inlength="4"	keyword="XISCSN2"	dicomtype="?"	dicomtag="?"		# 2nd select slice number
block="IMGHDR"	offset="750"	intype="String"		inlength="1"	keyword="XISCANO1"	dicomtype="?"	dicomtag="?"	enum="'A'=yes,'N'=no"	# 1st select slice flag
block="IMGHDR"	offset="751"	intype="String"		inlength="1"	keyword="XISCANO2"	dicomtype="?"	dicomtag="?"	enum="'A'=yes,'N'=no"	# 2nd select slice flag


block="IMGHDR"	offset="824"	intype="String"		inlength="2"	keyword="XISCSN"	dicomtype="?"	dicomtag="?"		# Slice number of positioning image
block="IMGHDR"	offset="826"	intype="String"		inlength="1"	keyword="XISCANO"	dicomtype="?"	dicomtag="?"	enum="'A'=yes,'N'=no"	# Execution of positioning
block="IMGHDR"	offset="827"	intype="Uint8"		inlength="1"	keyword="MIAVG"		dicomtype="DS"	dicomtag="NumberOfAverages"	# Number of signals averaged (NSA)
block="IMGHDR"	offset="828"	intype="Uint32_B"	inlength="1"	keyword="MIFREQO"	dicomtype="?"	dicomtag="?"		# Center frequency (Hz)
block="IMGHDR"	offset="832"	intype="Uint16_B"	inlength="1"	keyword="MIVRCAP"	dicomtype="?"	dicomtag="?"		# Value of variable capacitance
block="IMGHDR"	offset="834"	intype="Uint16_B"	inlength="1"	keyword="MIGX"		dicomtype="?"	dicomtag="?"		# X gradient field strength	
block="IMGHDR"	offset="836"	intype="Uint16_B"	inlength="1"	keyword="MIGY"		dicomtype="?"	dicomtag="?"		# Y gradient field strength	
block="IMGHDR"	offset="838"	intype="Uint16_B"	inlength="1"	keyword="MIGZ"		dicomtype="?"	dicomtag="?"		# Z gradient field strength	
block="IMGHDR"	offset="840"	intype="Uint16_B"	inlength="1"	keyword="MISHIFT"	dicomtype="?"	dicomtag="?"		# Number of bit shifts for 2DFT	
block="IMGHDR"	offset="842"	intype="String"		inlength="2"	keyword="MIENCD"	dicomtype="?"	dicomtag="?"	enum="'AP'=anterior-posterior,'HF'=head-feet,'RL'=right-left"	# Frequency encoding direction
block="IMGHDR"	offset="844"	intype="Uint16_B"	inlength="1"	keyword="MISMPL"	dicomtype="?"	dicomtag="?"		# Number of samples (frequency)	
block="IMGHDR"	offset="846"	intype="Uint16_B"	inlength="1"	keyword="MIPROJ"	dicomtype="?"	dicomtag="?"		# Number of projections (phase)	
block="IMGHDR"	offset="848"	intype="Uint16_B"	inlength="1"	keyword="MIGAIN"	dicomtype="?"	dicomtag="?"		# Signal attenuation (dB)	
block="IMGHDR"	offset="850"	intype="Uint16_B"	inlength="1"	keyword="MIBO"		dicomtype="?"	dicomtag="?"		# Static field strength (0.01T)	
block="IMGHDR"	offset="852"	intype="String"		inlength="1"	keyword="XICCT"	dicomtype="?"	dicomtag="?"	enum="'1'=cardiac gated,' '=normal"	# Cardiac gated image
block="IMGHDR"	offset="853"	intype="Uint8"		inlength="1"	keyword="MIBPM"		dicomtype="?"	dicomtag="?"		# Heart rate beats per minute
block="IMGHDR"	offset="854"	intype="Uint16_B"	inlength="1"	keyword="MIECGDL"	dicomtype="?"	dicomtag="?"		# Delay time from R-wave
block="IMGHDR"	offset="856"	intype="String"		inlength="95"	keyword="XICOMNT"	dicomtype="?"	dicomtag="?"		# spare
block="IMGHDR"	offset="951"	intype="String"		inlength="1"	keyword="XIY11"		dicomtype="?"	dicomtag="?"		# R-L RVSL, CLKW
block="IMGHDR"	offset="952"	intype="String"		inlength="2"	keyword="MICOIL"	dicomtype="?"	dicomtag="?"		# Used receiver coil
block="IMGHDR"	offset="954"	intype="Uint16_B"	inlength="1"	keyword="MIFLANG"	dicomtype="DS"	dicomtag="FlipAngle"	# Flip angle
block="IMGHDR"	offset="956"	intype="Uint16_B"	inlength="1"	keyword="MINECNT"	dicomtype="?"	dicomtag="?"		# Number of noise elimination data
block="IMGHDR"	offset="958"	intype="Uint16_B"	inlength="1"	keyword="MIGS"		dicomtype="?"	dicomtag="?"		# Slice encoding GC
block="IMGHDR"	offset="960"	intype="Uint16_B"	inlength="1"	keyword="MIVFSIZE"	dicomtype="?"	dicomtag="?"		# Rectangular FOV size (phase encoding direction) 256 equals to XIFOV
block="IMGHDR"	offset="962"	intype="Uint8"		inlength="1"	keyword="MIVFFG"	dicomtype="?"	dicomtag="?"	enum="0=Normal"	# Rectangular FOV execution code (1-6 is rectangular scan)
block="IMGHDR"	offset="963"	intype="String"		inlength="1"	keyword="MIAWSET"	dicomtype="?"	dicomtag="?"	enum="' '=no,'1'=yes"	# Auto window process scan
block="IMGHDR"	offset="964"	intype="Uint16_B"	inlength="1"	keyword="MIAWGAIN"	dicomtype="?"	dicomtag="?"		# Auto window scan
block="IMGHDR"	offset="966"	intype="Uint16_B"	inlength="1"	keyword="MIVFOPRJ"	dicomtype="?"	dicomtag="?"		# Original number of projection effective in rectangular FOV
block="IMGHDR"	offset="968"	intype="String"		inlength="1"	keyword="MI3D"		dicomtype="?"	dicomtag="?"	enum="'0'=no,'1'=yes"	# 3D acquisition
block="IMGHDR"	offset="969"	intype="Uint8"		inlength="1"	keyword="MI3DSFT"	dicomtype="?"	dicomtag="?"		# Number of bit shifts for FFT of slice encoding
block="IMGHDR"	offset="970"	intype="Uint8"		inlength="1"	keyword="MISLAB"	dicomtype="?"	dicomtag="?"		# Multislab number
block="IMGHDR"	offset="971"	intype="Uint8"		inlength="1"	keyword="MISLABMX"	dicomtype="?"	dicomtag="?"		# Number of multislabs
block="IMGHDR"	offset="972"	intype="Uint8"		inlength="1"	keyword="MISQNO"	dicomtype="?"	dicomtag="?"		# Sequence number
block="IMGHDR"	offset="973"	intype="Uint8"		inlength="1"	keyword="MISQCODE"	dicomtype="?"	dicomtag="?"	enum="1=with rephasing pulse"	# Sequence code
block="IMGHDR"	offset="974"	intype="Uint8"		inlength="1"	keyword="MIRJCTM"	dicomtype="?"	dicomtag="?"		# Rejection sequnec time (ms)
block="IMGHDR"	offset="975"	intype="Uint8"		inlength="1"	keyword="MIECGGT"	dicomtype="?"	dicomtag="?"		# Number of gate counts effective in cardiac gate image
block="IMGHDR"	offset="976"	intype="String"		inlength="1"	keyword="MIRESP"	dicomtype="?"	dicomtag="?"	enum="'I'=inspiration,'E'=expiration"	# Respiratory gating
block="IMGHDR"	offset="977"	intype="Uint8"		inlength="1"	keyword="MIPSATNO"	dicomtype="?"	dicomtag="?"		# Number of pre-saturation pulses
block="IMGHDR"	offset="978"	intype="Uint32_B"	inlength="1"	keyword="MISCANTM"	dicomtype="?"	dicomtag="?"		# Scan time (ms)
block="IMGHDR"	offset="982"	intype="String"		inlength="16"	keyword="MICILNAM"	dicomtype="SH"	dicomtag="ReceiveCoilName"	# Name of receiver coil
block="IMGHDR"	offset="998"	intype="String"		inlength="1"	keyword="MIRFATT"	dicomtype="?"	dicomtag="?"		# Signal attenuation for double RF receiver line (dB)
block="IMGHDR"	offset="999"	intype="String"		inlength="1"	keyword="MIRFLINE"	dicomtype="?"	dicomtag="?"		# Selection of RF receiver line
block="IMGHDR"	offset="1000"	intype="String"		inlength="1"	keyword="MIAGHV"	dicomtype="?"	dicomtag="?"	enum="'H'=horizontal,'V'=vertical"	# Rotation direction of angiogram
block="IMGHDR"	offset="1001"	intype="Uint8"		inlength="1"	keyword="MIHFFG"	dicomtype="?"	dicomtag="?"		# Half scan flag
block="IMGHDR"	offset="1002"	intype="Uint8"		inlength="1"	keyword="MIVFOAVG"	dicomtype="?"	dicomtag="?"		# NSA (before rectangular)
block="IMGHDR"	offset="1003"	intype="Uint8"		inlength="1"	keyword="MIYOBI"	dicomtype="?"	dicomtag="?"		# spare
block="IMGHDR"	offset="1004"	intype="String"		inlength="16"	keyword="XIFD16"	dicomtype="LO"	dicomtag="PatientID"	# Patient ID (16 digits)
block="IMGHDR"	offset="1020"	intype="Uint16_B"	inlength="1"	keyword="XIWL1"		dicomtype="?"	dicomtag="?"		# Window level
block="IMGHDR"	offset="1022"	intype="Uint16_B"	inlength="1"	keyword="XIWW1"		dicomtype="?"	dicomtag="?"		# Window width




constant="16" 		dicomtype="US"	dicomtag="BitsStored"			# 
constant="16" 		dicomtype="US"	dicomtag="BitsAllocated"		# 
constant="15" 		dicomtype="US"	dicomtag="HighBit"			# 
constant="0" 		dicomtype="US"	dicomtag="PixelRepresentation"		# 
constant="0" 		dicomtype="US"	dicomtag="PixelPaddingValue"		# 
constant="1" 		dicomtype="US"	dicomtag="SamplesPerPixel"		# 
constant="MONOCHROME2" 	dicomtype="CS"	dicomtag="PhotometricInterpretation"	# 
constant="0" 		dicomtype="DS"	dicomtag="RescaleIntercept"		# 
constant="" 		dicomtype="PN"	dicomtag="ReferringPhysicianName"	# 
constant="0" 		dicomtype="SH"	dicomtag="StudyID"			# 
constant="Hitachi Medical Systems" 	dicomtype="LO"	dicomtag="Manufacturer"			# 

