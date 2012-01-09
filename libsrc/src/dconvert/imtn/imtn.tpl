# Offsets are in bytes from 0
# Lengths are in intype units

block="MAINHDR"	offset="0"	intype="Uint16_L"	inlength="1"	keyword="FHENTRIES"	dicomtype="?"	dicomtag="?"	# Pointer to first block in the file header
block="MAINHDR"	offset="2"	intype="Uint16_L"	inlength="1"	keyword="FHCOUNT"	dicomtype="?"	dicomtag="?"	# Number of entries in the file header
block="MAINHDR"	offset="4"	intype="Uint16_L"	inlength="1"	keyword="FHDATA"	dicomtype="?"	dicomtag="?"	# Pointer to first block of the file header data

block="MAINHDR"	offset="6"	intype="Uint16_L"	inlength="1"	keyword="SLENTRIES"	dicomtype="?"	dicomtag="?"	# Pointer to first block in the slice header
block="MAINHDR"	offset="8"	intype="Uint16_L"	inlength="1"	keyword="SLCOUNT"	dicomtype="?"	dicomtag="?"	# Number of entries in the slice header
block="MAINHDR"	offset="10"	intype="Uint16_L"	inlength="1"	keyword="SLTBPOSN"	dicomtype="?"	dicomtag="?"	# Pointer to first block of the slice header position table
block="MAINHDR"	offset="12"	intype="Uint16_L"	inlength="1"	keyword="SLTBSIZE"	dicomtype="?"	dicomtag="?"	# Number of words in a header table entry

block="MAINHDR"	offset="14"	intype="Uint16_L"	inlength="1"	keyword="VERSION"	dicomtype="LO"	dicomtag="SoftwareVersions"	# File type version number

block="MAINHDR"	offset="16"	intype="Uint16_L"	inlength="1"	keyword="BLKDETOFF"	dicomtype="?"	dicomtag="?"	# Number of 512 byte blocks of detector offset data
block="MAINHDR"	offset="18"	intype="Uint16_L"	inlength="1"	keyword="BLKFHENTRIES"	dicomtype="?"	dicomtag="?"	# Number of 512 byte blocks in file header table
block="MAINHDR"	offset="20"	intype="Uint16_L"	inlength="1"	keyword="BLKFHDATA"	dicomtype="?"	dicomtag="?"	# Number of 512 byte blocks of file header data
block="MAINHDR"	offset="22"	intype="Uint16_L"	inlength="1"	keyword="BLKSLENTRIES"	dicomtype="?"	dicomtag="?"	# Number of 512 byte blocks in slice header
block="MAINHDR"	offset="24"	intype="Uint16_L"	inlength="1"	keyword="BLKSLTBPOSN"	dicomtype="?"	dicomtag="?"	# Number of 512 byte blocks in slice header position table
block="MAINHDR"	offset="26"	intype="Uint16_L"	inlength="1"	keyword="BLKSLTBDATA"	dicomtype="?"	dicomtag="?"	# Number of 512 byte blocks for each section of slice header data
block="MAINHDR"	offset="28"	intype="Uint16_L"	inlength="1"	keyword="DETOFF"	dicomtype="?"	dicomtag="?"	# Pointer to start of detector offset blocks

block="FILEHDR" method="ExtractTaggedInteger"	keyword="IFHLEN"	dicomtype="?"	dicomtag="?"	# number of 512 byte blocks in file header
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ISHLEN"	dicomtype="?"	dicomtag="?"	# number of 512 byte blocks in slice header
block="FILEHDR" method="ExtractTaggedString"	keyword="IAFN"		dicomtype="?"	dicomtag="?"	# file descriptor nnnnnn.eee
block="FILEHDR" method="ExtractTaggedString"	keyword="IADATE"	dicomtype="?"	dicomtag="?"	# date
block="FILEHDR" method="ExtractTaggedString"	keyword="IATIME"	dicomtype="?"	dicomtag="?"	# time
block="FILEHDR" method="ExtractTaggedString"	keyword="IPATID"	dicomtype="LO"	dicomtag="PatientID"		# patient ID
block="FILEHDR" method="ExtractTaggedString"	keyword="IPATNA"	dicomtype="PN"	dicomtag="PatientName"		# patient name
block="FILEHDR" method="ExtractTaggedString"	keyword="ICMNTS"	dicomtype="LO"	dicomtag="StudyDescription"	# comments
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NDETS"		dicomtype="US"	dicomtag="NumberOfDetectors"	# number of detectors (432 or 864)
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IDEMAP"	dicomtype="?"	dicomtag="?"	# detector status map (series of words, channel k IW=1+(k-1)/16,IBIT = k-(IW-1)*16-1))
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ISTOB"		dicomtype="?"	dicomtag="?"	# starting block for detector offset measurements (0 for none)
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NSLICE"	dicomtype="IS"	dicomtag="ImagesInAcquisition"	# number of slices in file
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IORGAN"	dicomtype="?"	dicomtag="?"	enum="-2=unsorted raw MM (AIR/PIN/OFFSET) ,-1=unsorted raw MM (Non-calibration),0=source-fan data1=detector-fan,2=image,3=tuning point,4=deflection buffer,5=processed calibration,6=processed AIR data,7=processed OFFSET"	# file organization code
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ITTICK"	dicomtype="?"	dicomtag="?"	# DAS clock period microseconds
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NPHVEW"	dicomtype="?"	dicomtag="?"	# number of phantoms
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IDATYP"	dicomtype="?"	dicomtag="?"	enum="0=DAS output words (all RAW data),1=Integer,2=Floating point (Sinogram/tuning/offsets),3=Scaled 11-bit integer data (image and screen save),4=AP400 block floating point mantissas,5=MM address data (calibration data),6=Octal data (deflection buffer files),7=Packed Fast Raw Averaged Data,8=Scaled 12-bit integer data (image and screen save)"	# data type
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NDETOM"	dicomtype="?"	dicomtag="?"	# Number of detector offset measurements
block="FILEHDR" method="ExtractTaggedFloat"	keyword="XMMTMU"	dicomtype="?"	dicomtag="?"	# Scale factor to change mm to MIP machine units (units are m.u./mm)
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IREP"		dicomtype="?"	dicomtag="?"	enum="3=Multi-slice (50ms) scan,6=Single-slice (100ms) scan"	# number of DAS samples per detector per source fan
block="FILEHDR" method="ExtractTaggedFloat"	keyword="PIXLEN"	dicomtype="?"	dicomtag="?"	# Pixel length mm from reconstruction
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NLEVEL"	dicomtype="?"	dicomtag="?"	# Number of levels in file
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NPLEVL"	dicomtype="?"	dicomtag="?"	# Number of images per level
block="FILEHDR" method="ExtractTaggedString"	keyword="IREF"		dicomtype="LO"	dicomtag="PositionReferenceIndicator"	# description of the reference point
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ISTUDY"	dicomtype="?"	dicomtag="?"	enum="-1=Screen save,0=Special,1=Localization,2=Flow,3=Movie,4=Average volume,5=Volume,6=Average flow,51=Image averaging,52=Reformat,53=FIP Maximum Difference,54=FIP Time to Peak,55=FIP Area Under the Curve, 56=FIP Center of Mass,102=Image subtraction flow,103=Image subtraction movie,105=Image subtraction volume,106=Image subtraction average flow"	# study type
block="FILEHDR" method="ExtractTaggedString"	keyword="ICONTR"	dicomtype="LO"	dicomtag="ContrastBolusAgent"	# Type of contrast
block="FILEHDR" method="ExtractTaggedFloat"	keyword="DOSECN"	dicomtype="DS"	dicomtag="ContrastBolusVolume"	# Contrast dose in cc
block="FILEHDR" method="ExtractTaggedString"	keyword="INJSIT"	dicomtype="LO"	dicomtag="ContrastBolusRoute"	# Injection Site
block="FILEHDR" method="ExtractTaggedString"	keyword="ISTRES"	dicomtype="?"	dicomtag="?"	# Type of stress
block="FILEHDR" method="ExtractTaggedString"	keyword="IRPHYS"	dicomtype="PN"	dicomtag="ReferringPhysicianName"	# Referring physician last name
block="FILEHDR" method="ExtractTaggedString"	keyword="IRADIO"	dicomtype="PN"	dicomtag="PerformingPhysicianName"	# Radiologist last name
block="FILEHDR" method="ExtractTaggedString"	keyword="ITECH"		dicomtype="PN"	dicomtag="OperatorsName"	# Radiation technologist initials
block="FILEHDR" method="ExtractTaggedString"	keyword="IBDATE"	dicomtype="?"	dicomtag="?"	# patient DOB dd-mmm-yy
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ISTHCK"	dicomtype="DS"	dicomtag="SliceThickness"	# Slice thickness in mm
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ICALIB"	dicomtype="?"	dicomtag="?"	# Calibration number
block="FILEHDR" method="ExtractTaggedInteger"	keyword="KERNEL"	dicomtype="?"	dicomtag="?"	# Desired kernel flag
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ITRTYP"	dicomtype="?"	dicomtag="?"	enum="1=manual,2=timed,3=ecg without trace data,4=ecg with trace data,5=undefined"	# trigger type
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IPATSZ"	dicomtype="?"	dicomtag="?"	enum="1=small,2=medium,3=large,4=shoulder or pelvis"	# patient size
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IPRLVL"	dicomtype="?"	dicomtag="?"	enum="0=none"	# Regular reconstructions first level to reconstruct
block="FILEHDR" method="ExtractTaggedString"	keyword="IDIAG"		dicomtype="LT"	dicomtag="PatientComments"	# Diagnosis comment
block="FILEHDR" method="ExtractTaggedString"	keyword="IHOSP"		dicomtype="SH"	dicomtag="StationName"	# Hospital (actually scanner)
block="FILEHDR" method="ExtractTaggedFloat"	keyword="BOLTIM"	dicomtype="?"	dicomtag="?"	# Bolus times
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NSPLIT"	dicomtype="?"	dicomtag="?"	# Number of images to be created from each raw slice
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IDLINP"	dicomtype="?"	dicomtag="?"	enum="0=do not delete after recon,1=delete after complete recon"	# Delete raw data flag
block="FILEHDR" method="ExtractTaggedFloat"	keyword="CDENS"		dicomtype="?"	dicomtag="?"	# Density of contrast
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IOFMIN"	dicomtype="?"	dicomtag="?"	# Time since midnight in minutes of last offsets
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IOFDAT"	dicomtype="?"	dicomtag="?"	# Day since dec 31 of last offsets
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NRINGS"	dicomtype="?"	dicomtag="?"	# Number of detector rings used
block="FILEHDR" method="ExtractTaggedInteger"	keyword="NTARGT"	dicomtype="?"	dicomtag="?"	# Number of targets used
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ICNREC"	dicomtype="?"	dicomtag="?"	enum="0=not suitable,1=suitable,2=suitable and used"	# Cone beam algorithm
block="FILEHDR" method="ExtractTaggedString"	keyword="KERNAM"	dicomtype="SH"	dicomtag="ConvolutionKernel"	# Kernel name used
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ISNTYP"	dicomtype="?"	dicomtag="?"	# Sinogram type
block="FILEHDR" method="ExtractTaggedInteger"	keyword="IANTYP"	dicomtype="?"	dicomtag="?"	enum="1=Cone analysis,2=Air analysis,3=Pin analysis"	# Analysis type for ASA
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ISTHCF"	dicomtype="?"	dicomtag="?"	# Slice thickness in .01 mm
block="FILEHDR" method="ExtractTaggedInteger"	keyword="ICOLL"		dicomtype="?"	dicomtag="?"	enum="1=1.5mm,3=3mm,6=6mm"	# Collimator setting

block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ISDATP"	dicomtype="?"	dicomtag="?"	# 512 byte block pointer to data for this slice
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="R1MU"		dicomtype="?"	dicomtag="?"	# Linear attenuation co-efficient for water at this energy and current, ring 1
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="IROTA"		dicomtype="?"	dicomtag="?"	enum="1=clockwise,-1=counter-clockwise"	# rotation
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="HVDES"		dicomtype="?"	dicomtag="?"	# Desired high voltage for this scan, kV
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="HVACT"		dicomtype="DS"	dicomtag="KVP"	# Actual high voltage for this scan, kV
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ICURNT"	dicomtype="IS"	dicomtag="XRayTubeCurrent"	# Actual electron beam current, mA
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="FVDES"		dicomtype="?"	dicomtag="?"	# Desired filament voltage, volts
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="FVACT"		dicomtype="?"	dicomtag="?"	# Actual filament voltage, volts
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="FCACT"		dicomtype="?"	dicomtag="?"	# Actual filament current, mA
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="IRING"		dicomtype="?"	dicomtag="?"	enum="0=raw slice with both rings interleaved,1=ring 1 (closest to gun),2=ring 2 (farther from gun)"	# detector ring
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ITARGT"	dicomtype="?"	dicomtag="?"	enum="65=A,66=B,67=C,68=D,69=E"	# target ring used
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="NSLAVG"	dicomtype="?"	dicomtag="?"	# number of scans averaged to produce this slice
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="PICRAD"	dicomtype="DS"	dicomtag="ReconstructionDiameter"	# Picture radius in mm
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="XORG"		dicomtype="?"	dicomtag="?"	# X coordinate of reconstruction center (0.0 is isocenter) in mm
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="YORG"		dicomtype="?"	dicomtag="?"	# Y coordinate of reconstruction center (0.0 is isocenter) in mm
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="ZOOM"		dicomtype="?"	dicomtag="?"	# Zoom factor (1.0 = no zoom) for reconstruction (also seems 0 = no zoom)
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="NROWS"		dicomtype="US"	dicomtag="Rows"		# rows
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="NCOLS"		dicomtype="US"	dicomtag="Columns"	# columns
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="VALMAX"	dicomtype="XS"	dicomtag="LargestImagePixelValue"	# Maximum value in the slice
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="VALMIN"	dicomtype="XS"	dicomtag="SmallestImagePixelValue"	# Minimum value in the slice
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="RSCALE"	dicomtype="?"	dicomtag="?"	# Data has been scaled and biased such that actual data = data/RSCALE + RMIN
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="RMIN"		dicomtype="?"	dicomtag="?"	# Data has been scaled and biased such that actual data = data/RSCALE + RMIN
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="IPATH"		dicomtype="?"	dicomtag="?"	enum="0=path was holding path,1=path was the first for that pulse,2=slice was not the first of that pulse (slices 2-N for a movie or volume)"	# Holding path flag
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="ELAPSE"	dicomtype="?"	dicomtag="?"	# Time since the first scan, in seconds
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="LEVELN"	dicomtype="?"	dicomtag="?"	# Level number for a given slice
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ISTAGE"	dicomtype="?"	dicomtag="?"	# Old:2 word array,2nd word unused,1st word is >=0 if data is present and useful
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="INOUT"		dicomtype="?"	dicomtag="?"	# In-out table position relative to reference in .1 mm
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="IHITE"		dicomtype="DS"	dicomtag="TableHeight"	# Up-down table position relative to referencein  mm
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ITILT"		dicomtype="DS"	dicomtag="GantryDetectorTilt"	# Table tilt relative to horizontal in degrees
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ISLEW"		dicomtype="DS"	dicomtag="GantryDetectorSlew"	# Table slew relative to straight in degrees
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ICPHAS"	dicomtype="DS"	dicomtag="TriggerWindow"	# cardiac phase (% of R-R interval)
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="IBEAT"		dicomtype="?"	dicomtag="?"	# Heart beat number for this image
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="HRATE"		dicomtype="DS"	dicomtag="HeartRate"	# Heart rate in beats per minute
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="IPATOR"	dicomtype="?"	dicomtag="?"	enum="-8=decubitus left feet first flipped,-7=decubitus right feet first flipped,-6=prone feet first flipped,-5=supine feet first flipped,0=unknown,1=supine head first,2=prone head first,3=decubitus right head first,4=decubitus left head first,5=prone head first flipped"	# Patient orientation
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="SLSIZE"	dicomtype="?"	dicomtag="?"	# Size of slice in words
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="ITN"		dicomtype="?"	dicomtag="?"	# Order of Chebychev polynomial applied to data (only if valid during calibration, 0 for normal recon)
block="SLICEHDR" select="image"	method="ExtractTaggedFloat"	keyword="R2MU"		dicomtype="?"	dicomtag="?"	# Linear attenuation coefficient for water at this energy and current, ring 2
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="IVMFLG"	dicomtype="?"	dicomtag="?"	# Bit-map of flags used by recon
block="SLICEHDR" select="image"	method="ExtractTaggedInteger"	keyword="NTARGS"	dicomtype="?"	dicomtag="?"	# Number of target sections of this target ring

constant="0" 			dicomtype="US"	dicomtag="PixelPaddingValue"		# 
constant="16" 			dicomtype="US"	dicomtag="BitsAllocated"		# 
constant="16" 			dicomtype="US"	dicomtag="BitsStored"			# 
constant="15" 			dicomtype="US"	dicomtag="HighBit"			# 
constant="0" 			dicomtype="US"	dicomtag="PixelRepresentation"		# Unsigned
constant="1" 			dicomtype="US"	dicomtag="SamplesPerPixel"		# 
constant="MONOCHROME2" 		dicomtype="CS"	dicomtag="PhotometricInterpretation"	# 
constant="-1000" 		dicomtype="DS"	dicomtag="RescaleIntercept"		# 
constant="1" 			dicomtype="DS"	dicomtag="RescaleSlope"			# 
constant="Hounsfield Units" 	dicomtype="LO"	dicomtag="RescaleType"			# 
constant="30"			dicomtype="DS"	dicomtag="WindowCenter"			# 
constant="250"			dicomtype="DS"	dicomtag="WindowWidth"			# 
constant="Soft tissue"		dicomtype="LO"	dicomtag="WindowCenterWidthExplanation"	# 

constant=""			dicomtype="SH"	dicomtag="AccessionNumber"		# 
constant="1"			dicomtype="IS"	dicomtag="SeriesInStudy"		# 
constant="1"			dicomtype="IS"	dicomtag="AcquisitionsInStudy"		# 
constant=""			dicomtype="SH"	dicomtag="StudyID"			#
constant="1"			dicomtype="IS"	dicomtag="SeriesNumber"			# 
constant=""			dicomtype="IS"	dicomtag="InstanceNumber"			# 

constant=""			dicomtype="LO"	dicomtag="DeviceSerialNumber"		# 
constant=""			dicomtype="DS"	dicomtag="PatientWeight"		# 
constant=""			dicomtype="CS"	dicomtag="PatientSex"			# 
constant="IMATRON"		dicomtype="LO"	dicomtag="Manufacturer"			# 
constant="C-100/150"		dicomtype="LO"	dicomtag="ManufacturerModelName"	# 

constant=""			dicomtype="CS"	dicomtag="BodyPartExamined"		# 
constant=""			dicomtype="SH"	dicomtag="FilterType"			# 
constant="CT"			dicomtype="CS"	dicomtag="Modality"			# 
