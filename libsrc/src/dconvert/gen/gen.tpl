# Offsets are in bytes from 0
# Lengths are in intype units

block="FILEHDR"	offset="0"	intype="Int32_B"	inlength="1"	name="IH_img_magic"		dicomtype="?"	dicomtag="?"		#  magic number
block="FILEHDR"	offset="4"	intype="Int32_B"	inlength="1"	keyword="IH_img_hdr_length"	dicomtype="?"	dicomtag="?"		#  a byte displacement to the <pixel data area>
block="FILEHDR"	offset="8"	intype="Int32_B"	inlength="1"	keyword="IH_img_width"		dicomtype="?"	dicomtag="?"		#  width (pixels) of image
block="FILEHDR"	offset="12"	intype="Int32_B"	inlength="1"	keyword="IH_img_height"		dicomtype="?"	dicomtag="?"		#  height (pixels) of image
block="FILEHDR"	offset="16"	intype="Int32_B"	inlength="1"	keyword="IH_img_depth"		dicomtype="?"	dicomtag="?"		#  depth (1, 8, 16, or 24 bits) of
block="FILEHDR"	offset="20"	intype="Int32_B"	inlength="1"	keyword="IH_img_compress"	dicomtype="?"	dicomtag="?"		#  type of compression; see IC_* below
block="FILEHDR"	offset="24"	intype="Int32_B"	inlength="1"	name="IH_img_dwindow"		dicomtype="DS"	dicomtag="WindowWidth"	#  default window setting
block="FILEHDR"	offset="28"	intype="Int32_B"	inlength="1"	name="IH_img_dlevel"		dicomtype="DS"	dicomtag="WindowCenter"	#  default level setting
block="FILEHDR"	offset="32"	intype="Int32_B"	inlength="1"	name="IH_img_bgshade"		dicomtype="?"	dicomtag="?"		#  background shade to use for non-image
block="FILEHDR"	offset="36"	intype="Int32_B"	inlength="1"	name="IH_img_ovrflow"		dicomtype="?"	dicomtag="?"		#  overflow value
block="FILEHDR"	offset="40"	intype="Int32_B"	inlength="1"	name="IH_img_undflow"		dicomtype="?"	dicomtag="?"		#  underflow value
block="FILEHDR"	offset="44"	intype="Int32_B"	inlength="1"	name="IH_img_top_offset"	dicomtype="?"	dicomtag="?"		#  number of blank lines at image top
block="FILEHDR"	offset="48"	intype="Int32_B"	inlength="1"	name="IH_img_bot_offset"	dicomtype="?"	dicomtag="?"		#  number of blank lines at image bottom
block="FILEHDR"	offset="52"	intype="Int16_B"	inlength="1"	name="IH_img_version"		dicomtype="LO"	dicomtag="SoftwareVersions"	#  version of the header structure
block="FILEHDR"	offset="54"	intype="Uint16_B"	inlength="1"	name="IH_img_checksum"		dicomtype="?"	dicomtag="?"		#  16 bit end_around_carry sum of pixels
block="FILEHDR"	offset="56"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_id"		dicomtype="?"	dicomtag="?"		#  a byte disp to unique image identifier
block="FILEHDR"	offset="60"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_id"		dicomtype="?"	dicomtag="?"		#  byte length of unique image identifier
block="FILEHDR"	offset="64"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_unpack"	dicomtype="?"	dicomtag="?"		#  a byte disp to <unpack control>
block="FILEHDR"	offset="68"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_unpack"	dicomtype="?"	dicomtag="?"		#  byte length of <unpack control>
block="FILEHDR"	offset="72"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_compress"	dicomtype="?"	dicomtag="?"		#  a byte disp to <compression control>
block="FILEHDR"	offset="76"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_compress"	dicomtype="?"	dicomtag="?"		#  byte length of <compression control>
block="FILEHDR"	offset="80"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_histo"	dicomtype="?"	dicomtag="?"		#  a byte disp to <histogram control>
block="FILEHDR"	offset="84"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_histo"	dicomtype="?"	dicomtag="?"		#  byte length of <histogram control>
block="FILEHDR"	offset="88"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_text"		dicomtype="?"	dicomtag="?"		#  a byte disp to <text plane data>
block="FILEHDR"	offset="92"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_text"		dicomtype="?"	dicomtag="?"		#  byte length of <text plane data>
block="FILEHDR"	offset="96"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_graphics"	dicomtype="?"	dicomtag="?"		#  a byte disp to <graphics plane data>
block="FILEHDR"	offset="100"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_graphics"	dicomtype="?"	dicomtag="?"		#  byte length of <graphics plane data>
block="FILEHDR"	offset="104"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_dbHdr"	dicomtype="?"	dicomtag="?"		#  a byte disp to <data base header data>
block="FILEHDR"	offset="108"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_dbHdr"	dicomtype="?"	dicomtag="?"		#  byte length of <data base header data>
block="FILEHDR"	offset="112"	intype="Int32_B"	inlength="1"	keyword="IH_img_levelOffset"	dicomtype="DS"	dicomtag="RescaleIntercept"		#  value to add to stored Pixel Data values
block="FILEHDR"	offset="116"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_user"		dicomtype="?"	dicomtag="?"		#  byte displacement to user defined data
block="FILEHDR"	offset="120"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_user"		dicomtype="?"	dicomtag="?"		#  byte length of user defined data
block="FILEHDR"	offset="124"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_suite"	dicomtype="?"	dicomtag="?"		#  byte displacement to <suite> header data
block="FILEHDR"	offset="128"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_suite"	dicomtype="?"	dicomtag="?"		#  byte length of <suite> header data
block="FILEHDR"	offset="132"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_exam"		dicomtype="?"	dicomtag="?"		#  byte displacement to <exam> header data
block="FILEHDR"	offset="136"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_exam"		dicomtype="?"	dicomtag="?"		#  byte length of <exam> header data
block="FILEHDR"	offset="140"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_series"	dicomtype="?"	dicomtag="?"		#  byte displacement to <series> header data
block="FILEHDR"	offset="144"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_series"	dicomtype="?"	dicomtag="?"		#  byte length of <series> header data
block="FILEHDR"	offset="148"	intype="Int32_B"	inlength="1"	keyword="IH_img_p_image"	dicomtype="?"	dicomtag="?"		#  byte displacement to <image> header data
block="FILEHDR"	offset="152"	intype="Int32_B"	inlength="1"	keyword="IH_img_l_image"	dicomtype="?"	dicomtag="?"		#  byte length of <image> header data
block="UIDHDR" condition="hasuid"	offset="0"	intype="Int32_B"	inlength="1"	name="ID_id_version"		dicomtype="?"	dicomtag="?"		#  version of the uid area
block="UIDHDR" condition="hasuid"	offset="4"	intype="String"		inlength="80"	name="ID_id_text"		dicomtype="?"	dicomtag="?"		#  unique text area or orphan description
block="HISTOHDR" condition="hashisto"	offset="0"	intype="Int32_B"	inlength="1"	name="HS_hs_version"		dicomtype="?"	dicomtag="?"		#  version of the histogram structure
block="HISTOHDR"	condition="hashisto"	offset="4"	intype="IEEE_Float32_B"	inlength="1"	name="HS_hs_sd"			dicomtype="?"	dicomtag="?"		#  standard deviation of pixel data value
block="HISTOHDR"	condition="hashisto"	offset="8"	intype="Int16_B"	inlength="1"	name="HS_hs_mean"		dicomtype="?"	dicomtag="?"		#  rounded mean pixel data value
block="HISTOHDR"	condition="hashisto"	offset="10"	intype="Int16_B"	inlength="1"	name="HS_hs_min"		dicomtype="XS"	dicomtag="SmallestImagePixelValue"	#  minimum pixel data value
block="HISTOHDR"	condition="hashisto"	offset="12"	intype="Int16_B"	inlength="1"	name="HS_hs_max"		dicomtype="XS"	dicomtag="LargestImagePixelValue"	#  maximum pixel data value
block="HISTOHDR"	condition="hashisto"	offset="14"	intype="Int16_B"	inlength="1"	name="HS_hs_first"		dicomtype="?"	dicomtag="?"		#  first histogram entry used for <hs_sd>
block="HISTOHDR"	condition="hashisto"	offset="16"	intype="Int16_B"	inlength="1"	name="HS_hs_region"		dicomtype="?"	dicomtag="?"		#  region entries used for <hs_sd>
block="HISTOHDR"	condition="hashisto"	offset="18"	intype="Int16_B"	inlength="1"	name="HS_hs_length"		dicomtype="?"	dicomtag="?"		#  number of bins in the histogram area
block="SUITEHDR"	offset="0"	intype="String"	inlength="4"	name="SU_su_id"		dicomtype="?"	dicomtag="?"		#  Suite ID
block="SUITEHDR"	offset="4"	intype="Int16_B"	inlength="1"	name="SU_su_uniq"		dicomtype="?"	dicomtag="?"	#  Make Unique Flag
block="SUITEHDR"	offset="6"	intype="String"	inlength="1"	name="SU_su_diskid"		dicomtype="?"	dicomtag="?"	#  Disk ID
block="SUITEHDR"	offset="7"	intype="String"	inlength="13"	name="SU_prodid"		dicomtype="LO"	dicomtag="ManufacturerModelName"	#  Product ID
block="SUITEHDR"	offset="20"	intype="String"	inlength="2"	name="SU_su_verscre"		dicomtype="?"	dicomtag="?"	#  Original Version of Record
block="SUITEHDR"	offset="22"	intype="String"	inlength="2"	name="SU_su_verscur"		dicomtype="?"	dicomtag="?"	#  Current Version of Record
block="SUITEHDR"	offset="24"	intype="Uint32_B"	inlength="1"	name="SU_su_checksum"		dicomtype="?"	dicomtag="?"	#  Suite Record Checksum
block="SUITEHDR"	offset="28"	intype="String"	inlength="85"	name="SU_su_padding"		dicomtype="?"	dicomtag="?"	#  Spare Space
block="EXAMHDR"	offset="0"	intype="String"	inlength="4"	name="EX_ex_suid"		dicomtype="SH"	dicomtag="StationName"	#  Suite ID for this Exam
block="EXAMHDR"	offset="4"	intype="Int16_B"	inlength="1"	name="EX_ex_uniq"		dicomtype="?"	dicomtag="?"		#  Make-Unique Flag
block="EXAMHDR"	offset="6"	intype="String"	inlength="1"	name="EX_ex_diskid"		dicomtype="?"	dicomtag="?"		#  Disk ID for this Exam
block="EXAMHDR"	offset="8"	intype="Uint16_B"	inlength="1"	keyword="EX_ex_no"		dicomtype="SH"	dicomtag="StudyID"	#  Exam Number
block="EXAMHDR"	offset="10"	intype="String"	inlength="33"	name="EX_hospname"		dicomtype="LO"	dicomtag="InstitutionName"	#  Hospital Name
block="EXAMHDR"	offset="44"	intype="Int16_B"	inlength="1"	name="EX_detect"		dicomtype="?"	dicomtag="?"	enum="1=Xenon,2=Hilight"	#  Detector Type
block="EXAMHDR"	offset="46"	intype="Int32_B"	inlength="1"	name="EX_numcells"		dicomtype="?"	dicomtag="?"		#  Number of cells in det
block="EXAMHDR"	offset="50"	intype="IEEE_Float32_B"	inlength="1"	name="EX_zerocell"		dicomtype="?"	dicomtag="?"		#  Cell number at theta
block="EXAMHDR"	offset="54"	intype="IEEE_Float32_B"	inlength="1"	name="EX_cellspace"		dicomtype="?"	dicomtag="?"		#  Cell spacing
block="EXAMHDR"	offset="58"	intype="IEEE_Float32_B"	inlength="1"	keyword="EX_srctodet"		dicomtype="?"	dicomtag="?"		#  Distance from source to detector
block="EXAMHDR"	offset="62"	intype="IEEE_Float32_B"	inlength="1"	keyword="EX_srctoiso"		dicomtype="?"	dicomtag="?"		#  Distance from source to iso
block="EXAMHDR"	offset="66"	intype="Int16_B"	inlength="1"	name="EX_tubetyp"		dicomtype="?"	dicomtag="?"	enum="0=MX100_TUBE,1=MX125_TUBE,2=TT9800:9800_TUBE,3=TT9100:9100_TUBE,4=HPRI_TUBE,5=HPRII_TUBE,6=ADV650_TUBE,7=ADV681_TUBE,8=ST2000CT_6_TUBE,9=ST2000CT_10_TUBE,12=MX_135CT_TUBE,13=MX_165CT_TUBE,14=MX_165CT_I_TUBE"	#  Tube type
block="EXAMHDR"	offset="68"	intype="Int16_B"	inlength="1"	name="EX_dastyp"		dicomtype="?"	dicomtag="?"	enum="1=CDAS,2=EDAS"	#  DAS type
block="EXAMHDR"	offset="70"	intype="Int16_B"	inlength="1"	name="EX_num_dcnk"		dicomtype="?"	dicomtag="?"		#  Number of Decon Kernals
block="EXAMHDR"	offset="72"	intype="Int16_B"	inlength="1"	name="EX_dcn_len"		dicomtype="?"	dicomtag="?"		#  Number of elements in a Decon Kernal
block="EXAMHDR"	offset="74"	intype="Int16_B"	inlength="1"	name="EX_dcn_density"	dicomtype="?"	dicomtag="?"			#  Decon Kernal density
block="EXAMHDR"	offset="76"	intype="Int16_B"	inlength="1"	name="EX_dcn_stepsize"	dicomtype="?"	dicomtag="?"			#  Decon Kernal stepsize
block="EXAMHDR"	offset="78"	intype="Int16_B"	inlength="1"	name="EX_dcn_shiftcnt"	dicomtype="?"	dicomtag="?"			#  Decon Kernal Shift Count
block="EXAMHDR"	offset="80"	intype="Int32_B"	inlength="1"	keyword="EX_magstrength"	dicomtype="?"	dicomtag="?"		#  Magnet strength (in gauss)
block="EXAMHDR"	offset="84"	intype="String"	inlength="13"	name="EX_patid"		dicomtype="LO"	dicomtag="PatientID"		#  Patient ID for this Exam
block="EXAMHDR"	offset="97"	intype="String"	inlength="25"	name="EX_patname"	dicomtype="PN"	dicomtag="PatientName"		#  Patient Name
block="EXAMHDR"	offset="122"	intype="Int16_B"	inlength="1"	keyword="EX_patage"	dicomtype="?"	dicomtag="?"			#  Patient Age (years, months or days)
block="EXAMHDR"	offset="124"	intype="Int16_B"	inlength="1"	keyword="EX_patian"	dicomtype="?"	dicomtag="?"	enum="0=Years:Y,1=Months:M,2=Days:D,3=Weeks:W"	#  Patient Age Notation
block="EXAMHDR"	offset="126"	intype="Int16_B"	inlength="1"	keyword="EX_patsex"	dicomtype="?"	dicomtag="?"	enum="1=Male:M,2=Female:F"	#  Patient Sex
block="EXAMHDR"	offset="128"	intype="Int32_B"	inlength="1"	keyword="EX_patweight"	dicomtype="?"	dicomtag="?"			#  Patient Weight ? grams
block="EXAMHDR"	offset="132"	intype="Int16_B"	inlength="1"	name="EX_trauma"	dicomtype="?"	dicomtag="?"	enum="0=Trauma Flag Off,1=Trauma Flag On"	#  Trauma Flag
block="EXAMHDR"	offset="134"	intype="String"	inlength="61"	name="EX_hist"		dicomtype="LT"	dicomtag="AdditionalPatientHistory"	#  Patient History
block="EXAMHDR"	offset="195"	intype="String"	inlength="13"	name="EX_reqnum"	dicomtype="?"	dicomtag="?"			#  Requisition Number
block="EXAMHDR"	offset="208"	intype="Unix_DateTime_B"	inlength="1"	keyword="EX_ex_datetime"	dicomtype="?"	dicomtag="?"			#  Exam date/time stamp
block="EXAMHDR"	offset="212"	intype="String"	inlength="33"	name="EX_refphy"	dicomtype="PN"	dicomtag="ReferringPhysicianName"	#  Referring Physician
block="EXAMHDR"	offset="245"	intype="String"	inlength="33"	name="EX_diagrad"	dicomtype="PN"	dicomtag="PerformingPhysicianName"	#  Diagnostician/Radiologist
block="EXAMHDR"	offset="278"	intype="String"	inlength="4"	name="EX_op"			dicomtype="PN"	dicomtag="OperatorsName"	#  Operator
block="EXAMHDR"	offset="282"	intype="String"	inlength="23"	name="EX_ex_desc"		dicomtype="LO"	dicomtag="StudyDescription"	#  Exam Description
block="EXAMHDR"	offset="305"	intype="String"	inlength="3"	keyword="EX_ex_typ"		dicomtype="CS"	dicomtag="Modality"		#  Exam Type ('CT' or 'MR')
block="EXAMHDR"	offset="308"	intype="Int16_B"	inlength="1"	name="EX_ex_format"		dicomtype="?"	dicomtag="?"	enum="0=Foreign,1=Genesis Image,2=Technicare HPS1440,3=Technicare Delta Scan 2010,4=Technicare Delta Scan 2020,5=Technicare Delta Scan 2060,6=Signa,7=CT9800,8=CT9800 Q,9=CT9800 QHL,10=EMI_9800,11=CT_8800_9800,12=CT9600,13=Jupiter,14=Zeus:CT HiSpeed Advantage,15=CT HiSpeed Advantage VX,16=Resona,17=Vectra,18=MR Max,19=CT Max,20=CT Pace,21=CT Sytec 2000,22=CT Sytec 3000,23=CT Sytec 4000,24=CT Sytec 6000,25=CT Sytec 8000"	#  Exam Format
block="EXAMHDR"	offset="310"	intype="IEEE_Float64_B"	inlength="1"	name="EX_firstaxtime"	dicomtype="?"	dicomtag="?"		#  Start time(secs) of first axial in exam
block="EXAMHDR"	offset="318"	intype="String"	inlength="9"	name="EX_ex_sysid"		dicomtype="?"	dicomtag="?"		#  Creator Suite and Host
block="EXAMHDR"	offset="328"	intype="Unix_DateTime_B"	inlength="1"	keyword="EX_ex_lastmod"		dicomtype="?"	dicomtag="?"		#  Date/Time of Last Change
block="EXAMHDR"	offset="332"	intype="Int16_B"	inlength="1"	name="EX_protocolflag"	dicomtype="?"	dicomtag="?"		#  Non-Zero indicates Protocol Exam
block="EXAMHDR"	offset="334"	intype="String"	inlength="13"	name="EX_ex_alloc_key"	dicomtype="?"	dicomtag="?"		#  Process that allocated this record
block="EXAMHDR"	offset="348"	intype="Int32_B"	inlength="1"	name="EX_ex_delta_cnt"	dicomtype="?"	dicomtag="?"		#  Indicates number of updates to header
block="EXAMHDR"	offset="352"	intype="String"	inlength="2"	name="EX_ex_verscre"		dicomtype="?"	dicomtag="?"		#  Genesis Version - Created
block="EXAMHDR"	offset="354"	intype="String"	inlength="2"	name="EX_ex_verscur"		dicomtype="?"	dicomtag="?"		#  Genesis Version - Now
block="EXAMHDR"	offset="356"	intype="Uint32_B"	inlength="1"	name="EX_ex_checksum"	dicomtype="?"	dicomtag="?"		#  Exam Record Checksum
block="EXAMHDR"	offset="360"	intype="Int32_B"	inlength="1"	name="EX_ex_complete"	dicomtype="?"	dicomtag="?"		#  Exam Complete Flag
block="EXAMHDR"	offset="364"	intype="Int32_B"	inlength="1"	name="EX_ex_seriesct"	dicomtype="?"	dicomtag="?"		#  Last Series Number Used
block="EXAMHDR"	offset="368"	intype="Int32_B"	inlength="1"	name="EX_ex_numarch"	dicomtype="?"	dicomtag="?"		#  Number of Series Archived
block="EXAMHDR"	offset="372"	intype="Int32_B"	inlength="1"	name="EX_ex_numseries"	dicomtype="?"	dicomtag="?"		#  Number of Series Existing
block="EXAMHDR"	offset="376"	intype="Uint32_B"	inlength="1"	name="EX_ex_series_l"	dicomtype="?"	dicomtag="?"		#  Series Keys for this exam - length
block="EXAMHDR"	offset="380"	intype="Uint32_B"	inlength="1"	name="EX_ex_series_d"	dicomtype="?"	dicomtag="?"		#  Series Keys for this exam - data ptr
block="EXAMHDR"	offset="384"	intype="Int32_B"	inlength="1"	name="EX_ex_numunser"	dicomtype="?"	dicomtag="?"		#  Number of Unstored Series
block="EXAMHDR"	offset="388"	intype="Uint32_B"	inlength="1"	name="EX_ex_unseries_l"	dicomtype="?"	dicomtag="?"		#  Unstored Series Keys for this exam - length
block="EXAMHDR"	offset="392"	intype="Uint32_B"	inlength="1"	name="EX_ex_unseries_d"	dicomtype="?"	dicomtag="?"		#  Unstored Series Keys for this exam - data ptr
block="EXAMHDR"	offset="396"	intype="Int32_B"	inlength="1"	name="EX_ex_toarchcnt"	dicomtype="?"	dicomtag="?"		#  Number of Unarchived Series
block="EXAMHDR"	offset="400"	intype="Uint32_B"	inlength="1"	name="EX_ex_toarchive_l"	dicomtype="?"	dicomtag="?"	#  Unarchived Series Keys for this exam - length
block="EXAMHDR"	offset="404"	intype="Uint32_B"	inlength="1"	name="EX_ex_toarchive_d"	dicomtype="?"	dicomtag="?"	#  Unarchived Series Keys for this exam - data ptr
block="EXAMHDR"	offset="408"	intype="Int32_B"	inlength="1"	name="EX_ex_prospcnt"	dicomtype="?"	dicomtag="?"		#  Number of Prospective/Scout Series
block="EXAMHDR"	offset="412"	intype="Uint32_B"	inlength="1"	name="EX_ex_prosp_l"	dicomtype="?"	dicomtag="?"		#  Prospective Series Keys for this exam - length
block="EXAMHDR"	offset="416"	intype="Uint32_B"	inlength="1"	name="EX_ex_prosp_d"	dicomtype="?"	dicomtag="?"		#  Prospective Series Keys for this exam - data ptr
block="EXAMHDR"	offset="420"	intype="Int32_B"	inlength="1"	name="EX_ex_modelnum"	dicomtype="?"	dicomtag="?"		#  Last Model Number used
block="EXAMHDR"	offset="424"	intype="Int32_B"	inlength="1"	name="EX_ex_modelcnt"	dicomtype="?"	dicomtag="?"		#  Number of ThreeD Models
block="EXAMHDR"	offset="428"	intype="Uint32_B"	inlength="1"	name="EX_ex_models_l"	dicomtype="?"	dicomtag="?"		#  3D Model Keys for this exam - length
block="EXAMHDR"	offset="432"	intype="Uint32_B"	inlength="1"	name="EX_ex_models_d"	dicomtype="?"	dicomtag="?"		#  3D Model Keys for this exam - data ptr
block="EXAMHDR"	offset="436"	intype="Int16_B"	inlength="1"	name="EX_ex_stat"		dicomtype="?"	dicomtag="?"	enum="0=In Patient:IN,1=Out Patient:OUT,2=Emergency:EM,3=Referral:REF"	#  Patient Status
block="EXAMHDR"	offset="438"	intype="String"	inlength="16"	name="EX_uniq_sys_id"	dicomtype="?"	dicomtag="?"		#  Unique System ID
block="EXAMHDR"	offset="454"	intype="String"	inlength="16"	name="EX_service_id"		dicomtype="?"	dicomtag="?"		#  Unique Service ID
block="EXAMHDR"	offset="470"	intype="String"	inlength="4"	name="EX_mobile_loc"		dicomtype="?"	dicomtag="?"		#  Mobile Location Number
block="EXAMHDR"	offset="474"	intype="String"	inlength="32"	name="EX_study_uid"		dicomtype="?"	dicomtag="?"		#  Study Entity Unique ID
block="EXAMHDR"	offset="506"	intype="String"	inlength="518"	name="EX_ex_padding"		dicomtype="?"	dicomtag="?"		#  Spare Space
block="SERIESHDR"	offset="0"	intype="String"	inlength="4"	name="SE_se_suid"	dicomtype="?"	dicomtag="?"		#  Suite ID for this Series
block="SERIESHDR"	offset="4"	intype="Int16_B"	inlength="1"	name="SE_se_uniq"	dicomtype="?"	dicomtag="?"		#  The Make-Unique Flag
block="SERIESHDR"	offset="6"	intype="String"	inlength="1"	name="SE_se_diskid"	dicomtype="?"	dicomtag="?"		#  Disk ID for this Series
block="SERIESHDR"	offset="8"	intype="Uint16_B"	inlength="1"	name="SE_se_exno"	dicomtype="?"	dicomtag="?"		#  Exam Number
block="SERIESHDR"	offset="10"	intype="Int16_B"	inlength="1"	keyword="SE_se_no"	dicomtype="IS"	dicomtag="SeriesNumber"	#  Series Number
block="SERIESHDR"	offset="12"	intype="Unix_DateTime_B"	inlength="1"	keyword="SE_se_datetime"	dicomtype="?"	dicomtag="?"	#  Allocation Series Date/Time stamp
block="SERIESHDR"	offset="16"	intype="Unix_DateTime_B"	inlength="1"	keyword="SE_se_actual_dt"	dicomtype="?"	dicomtag="?"	#  Actual Series Date/Time stamp
block="SERIESHDR"	offset="20"	intype="String"	inlength="30"	name="SE_se_desc"	dicomtype="LO"	dicomtag="SeriesDescription"	#  Series Description
block="SERIESHDR"	offset="50"	intype="String"	inlength="9"	name="SE_pr_sysid"	dicomtype="?"	dicomtag="?"		#  Primary Receiver Suite and Host
block="SERIESHDR"	offset="59"	intype="String"	inlength="9"	name="SE_pansysid"	dicomtype="?"	dicomtag="?"		#  Archiver Suite and Host
block="SERIESHDR"	offset="68"	intype="Int16_B"	inlength="1"	keyword="SE_se_typ"	dicomtype="?"	dicomtag="?"	enum="1=prospective:PROSP,2=Retrospective:RETRO,3=Scout Series:SCOUT,4=Reformatted:REFMT,5=Screensave:SSAVE,6=Xenon:XENON,7=Service:SERV,9=Projected:PJN"	#  Series Type
block="SERIESHDR"	offset="70"	intype="Int16_B"	inlength="1"	name="SE_se_source"	dicomtype="?"	dicomtag="?"		#  Series from which prescribed
block="SERIESHDR"	offset="72"	intype="Int16_B"	inlength="1"	name="SE_se_plane"	dicomtype="?"	dicomtag="?"		#  Most-like Plane (for L/S)
block="SERIESHDR"	offset="74"	intype="Int16_B"	inlength="1"	name="SE_scan_type"	dicomtype="?"	dicomtag="?"	enum="1=Scout:SCT,2=Axial:AX,3=Screensave:SS"	#  Scout or Axial (for CT)
block="SERIESHDR"	offset="76"	intype="Int32_B"	inlength="1"	keyword="SE_position"	dicomtype="?"	dicomtag="?"	bitmap="0:none,Supine;1:none,Prone;2:none,Decubitus Left;3:none,Decubitus Right"	#  Patient Position
block="SERIESHDR"	offset="80"	intype="Int32_B"	inlength="1"	keyword="SE_entry"	dicomtype="?"	dicomtag="?"	enum="1=Head First,2=Feet First"	#  Patient Entry
block="SERIESHDR"	offset="84"	intype="String"	inlength="3"	name="SE_anref"	dicomtype="LO"	dicomtag="PositionReferenceIndicator"		#  Anatomical reference
block="SERIESHDR"	offset="88"	intype="IEEE_Float32_B"	inlength="1"	name="SE_lmhor"	dicomtype="?"	dicomtag="?"		#  Horizontal Landmark
block="SERIESHDR"	offset="92"	intype="String"	inlength="25"	name="SE_prtcl"	dicomtype="LO"	dicomtag="ProtocolName"	#  Scan Protocol Name
block="SERIESHDR"	offset="118"	intype="Int16_B"	inlength="1"	name="SE_se_contrast"	dicomtype="?"	dicomtag="?"	#  Non-zero if > 0 image used contrast(L/S)
block="SERIESHDR"	offset="120"	intype="String"	inlength="1"	name="SE_start_ras"	dicomtype="?"	dicomtag="?"		#  RAS letter for first scan location (L/S)
block="SERIESHDR"	offset="122"	intype="IEEE_Float32_B"	inlength="1"	name="SE_start_loc"	dicomtype="?"	dicomtag="?"		#  First scan location (L/S)
block="SERIESHDR"	offset="126"	intype="String"	inlength="1"	name="SE_end_ras"	dicomtype="?"	dicomtag="?"		#  RAS letter for last scan location (L/S)
block="SERIESHDR"	offset="128"	intype="IEEE_Float32_B"	inlength="1"	name="SE_end_loc"	dicomtype="?"	dicomtag="?"		#  Last scan location (L/S)
block="SERIESHDR"	offset="132"	intype="Int16_B"	inlength="1"	name="SE_se_pseq"	dicomtype="?"	dicomtag="?"		#  Last Pulse Sequence Used (L/S)
block="SERIESHDR"	offset="134"	intype="Int16_B"	inlength="1"	name="SE_se_sortorder"	dicomtype="?"	dicomtag="?"	#  Image Sort Order (L/S)
block="SERIESHDR"	offset="136"	intype="Int32_B"	inlength="1"	name="SE_se_lndmrkcnt"	dicomtype="?"	dicomtag="?"	#  Landmark Counter
block="SERIESHDR"	offset="140"	intype="Int16_B"	inlength="1"	name="SE_se_nacq"	dicomtype="IS"	dicomtag="ImagesInAcquisition"	#  Number of Acquisitions
block="SERIESHDR"	offset="142"	intype="Int16_B"	inlength="1"	name="SE_xbasest"		dicomtype="?"	dicomtag="?"	#  Starting number for baselines
block="SERIESHDR"	offset="144"	intype="Int16_B"	inlength="1"	name="SE_xbaseend"		dicomtype="?"	dicomtag="?"	#  Ending number for baselines
block="SERIESHDR"	offset="146"	intype="Int16_B"	inlength="1"	name="SE_xenhst"		dicomtype="?"	dicomtag="?"	#  Starting number for enhanced scans
block="SERIESHDR"	offset="148"	intype="Int16_B"	inlength="1"	name="SE_xenhend"		dicomtype="?"	dicomtag="?"	#  Ending number for enhanced scans
block="SERIESHDR"	offset="150"	intype="Unix_DateTime_B"	inlength="1"	keyword="SE_se_lastmod"		dicomtype="?"	dicomtag="?"	#  Date/Time of Last Change
block="SERIESHDR"	offset="154"	intype="String"	inlength="13"	name="SE_se_alloc_key"	dicomtype="?"	dicomtag="?"	#  Process that allocated this record
block="SERIESHDR"	offset="168"	intype="Int32_B"	inlength="1"	name="SE_se_delta_cnt"	dicomtype="?"	dicomtag="?"	#  Indicates number of updates to header
block="SERIESHDR"	offset="172"	intype="String"	inlength="2"	name="SE_se_verscre"		dicomtype="?"	dicomtag="?"	#  Genesis Version - Created
block="SERIESHDR"	offset="174"	intype="String"	inlength="2"	name="SE_se_verscur"		dicomtype="?"	dicomtag="?"	#  Genesis Version - Now
block="SERIESHDR"	offset="176"	intype="IEEE_Float32_B"	inlength="1"	name="SE_se_pds_a"		dicomtype="?"	dicomtag="?"	#  PixelData size - as stored
block="SERIESHDR"	offset="180"	intype="IEEE_Float32_B"	inlength="1"	name="SE_se_pds_c"		dicomtype="?"	dicomtag="?"	#  PixelData size - Compressed
block="SERIESHDR"	offset="184"	intype="IEEE_Float32_B"	inlength="1"	name="SE_se_pds_u"		dicomtype="?"	dicomtag="?"	#  PixelData size - UnCompressed
block="SERIESHDR"	offset="188"	intype="Uint32_B"	inlength="1"	name="SE_se_checksum"		dicomtype="?"	dicomtag="?"	#  Series Record checksum
block="SERIESHDR"	offset="192"	intype="Int32_B"	inlength="1"	name="SE_se_complete"		dicomtype="?"	dicomtag="?"	#  Series Complete Flag
block="SERIESHDR"	offset="196"	intype="Int32_B"	inlength="1"	name="SE_se_numarch"		dicomtype="?"	dicomtag="?"	#  Number of Images Archived
block="SERIESHDR"	offset="200"	intype="Int32_B"	inlength="1"	name="SE_se_imagect"		dicomtype="?"	dicomtag="?"	#  Last Image Number Used
block="SERIESHDR"	offset="204"	intype="Int32_B"	inlength="1"	name="SE_se_numimages"		dicomtype="?"	dicomtag="?"	#  Number of Images Existing
block="SERIESHDR"	offset="208"	intype="Uint32_B"	inlength="1"	name="SE_se_images_l"		dicomtype="?"	dicomtag="?"	#  Image Keys for this Series - length
block="SERIESHDR"	offset="212"	intype="Uint32_B"	inlength="1"	name="SE_se_images_d"		dicomtype="?"	dicomtag="?"	#  Image Keys for this Series - data ptr
block="SERIESHDR"	offset="216"	intype="Int32_B"	inlength="1"	name="SE_se_numunimg"		dicomtype="?"	dicomtag="?"	#  Number of Unstored Images
block="SERIESHDR"	offset="220"	intype="Uint32_B"	inlength="1"	name="SE_se_unimages_l"		dicomtype="?"	dicomtag="?"	#  Unstored Image Keys for this Series - length
block="SERIESHDR"	offset="224"	intype="Uint32_B"	inlength="1"	name="SE_se_unimages_d"		dicomtype="?"	dicomtag="?"	#  Unstored Image Keys for this Series - data ptr
block="SERIESHDR"	offset="228"	intype="Int32_B"	inlength="1"	name="SE_se_toarchcnt"		dicomtype="?"	dicomtag="?"	#  Number of Unarchived Images
block="SERIESHDR"	offset="232"	intype="Uint32_B"	inlength="1"	name="SE_se_toarchive_l"	dicomtype="?"	dicomtag="?"	#  Unarchived Image Keys for this Series - length
block="SERIESHDR"	offset="236"	intype="Uint32_B"	inlength="1"	name="SE_se_toarchive_d"	dicomtype="?"	dicomtag="?"	#  Unarchived Image Keys for this Series - data ptr
block="SERIESHDR"	offset="240"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo1_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 1 Alpha Value
block="SERIESHDR"	offset="244"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo1_beta"		dicomtype="?"	dicomtag="?"	#  Echo 1 Beta Value
block="SERIESHDR"	offset="248"	intype="Uint16_B"	inlength="1"	name="SE_echo1_window"	dicomtype="?"	dicomtag="?"	#  Echo 1 Window Value
block="SERIESHDR"	offset="250"	intype="Int16_B"	inlength="1"	name="SE_echo1_level"	dicomtype="?"	dicomtag="?"	#  Echo 1 Level Value
block="SERIESHDR"	offset="252"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo2_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 2 Alpha Value
block="SERIESHDR"	offset="256"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo2_beta"		dicomtype="?"	dicomtag="?"	#  Echo 2 Beta Value
block="SERIESHDR"	offset="260"	intype="Uint16_B"	inlength="1"	name="SE_echo2_window"	dicomtype="?"	dicomtag="?"	#  Echo 2 Window Value
block="SERIESHDR"	offset="262"	intype="Int16_B"	inlength="1"	name="SE_echo2_level"	dicomtype="?"	dicomtag="?"	#  Echo 2 Level Value
block="SERIESHDR"	offset="264"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo3_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 3 Alpha Value
block="SERIESHDR"	offset="268"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo3_beta"		dicomtype="?"	dicomtag="?"	#  Echo 3 Beta Value
block="SERIESHDR"	offset="272"	intype="Uint16_B"	inlength="1"	name="SE_echo3_window"	dicomtype="?"	dicomtag="?"	#  Echo 3 Window Value
block="SERIESHDR"	offset="274"	intype="Int16_B"	inlength="1"	name="SE_echo3_level"	dicomtype="?"	dicomtag="?"	#  Echo 3 Level Value
block="SERIESHDR"	offset="276"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo4_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 4 Alpha Value
block="SERIESHDR"	offset="280"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo4_beta"		dicomtype="?"	dicomtag="?"	#  Echo 4 Beta Value
block="SERIESHDR"	offset="284"	intype="Uint16_B"	inlength="1"	name="SE_echo4_window"	dicomtype="?"	dicomtag="?"	#  Echo 4 Window Value
block="SERIESHDR"	offset="286"	intype="Int16_B"	inlength="1"	name="SE_echo4_level"	dicomtype="?"	dicomtag="?"	#  Echo 4 Level Value
block="SERIESHDR"	offset="288"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo5_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 5 Alpha Value
block="SERIESHDR"	offset="292"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo5_beta"		dicomtype="?"	dicomtag="?"	#  Echo 5 Beta Value
block="SERIESHDR"	offset="296"	intype="Uint16_B"	inlength="1"	name="SE_echo5_window"	dicomtype="?"	dicomtag="?"	#  Echo 5 Window Value
block="SERIESHDR"	offset="298"	intype="Int16_B"	inlength="1"	name="SE_echo5_level"	dicomtype="?"	dicomtag="?"	#  Echo 5 Level Value
block="SERIESHDR"	offset="300"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo6_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 6 Alpha Value
block="SERIESHDR"	offset="304"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo6_beta"		dicomtype="?"	dicomtag="?"	#  Echo 6 Beta Value
block="SERIESHDR"	offset="308"	intype="Uint16_B"	inlength="1"	name="SE_echo6_window"	dicomtype="?"	dicomtag="?"	#  Echo 6 Window Value
block="SERIESHDR"	offset="310"	intype="Int16_B"	inlength="1"	name="SE_echo6_level"	dicomtype="?"	dicomtag="?"	#  Echo 6 Level Value
block="SERIESHDR"	offset="312"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo7_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 7 Alpha Value
block="SERIESHDR"	offset="316"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo7_beta"		dicomtype="?"	dicomtag="?"	#  Echo 7 Beta Value
block="SERIESHDR"	offset="320"	intype="Uint16_B"	inlength="1"	name="SE_echo7_window"	dicomtype="?"	dicomtag="?"	#  Echo 7 Window Value
block="SERIESHDR"	offset="322"	intype="Int16_B"	inlength="1"	name="SE_echo7_level"	dicomtype="?"	dicomtag="?"	#  Echo 7 Level Value
block="SERIESHDR"	offset="324"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo8_alpha"	dicomtype="?"	dicomtag="?"	#  Echo 8 Alpha Value
block="SERIESHDR"	offset="328"	intype="IEEE_Float32_B"	inlength="1"	name="SE_echo8_beta"		dicomtype="?"	dicomtag="?"	#  Echo 8 Beta Value
block="SERIESHDR"	offset="332"	intype="Uint16_B"	inlength="1"	name="SE_echo8_window"	dicomtype="?"	dicomtag="?"	#  Echo 8 Window Value
block="SERIESHDR"	offset="334"	intype="Int16_B"	inlength="1"	name="SE_echo8_level"	dicomtype="?"	dicomtag="?"	#  Echo 8 Level Value
block="SERIESHDR"	offset="336"	intype="String"	inlength="32"	name="SE_series_uid"		dicomtype="?"	dicomtag="?"	#  Series Entity Unique ID
block="SERIESHDR"	offset="368"	intype="String"	inlength="32"	name="SE_landmark_uid"	dicomtype="?"	dicomtag="?"	#  Landmark Unique ID
block="SERIESHDR"	offset="400"	intype="String"	inlength="32"	name="SE_equipmnt_uid"	dicomtype="?"	dicomtag="?"	#  Equipment Unique ID
block="SERIESHDR"	offset="432"	intype="String"	inlength="588"	name="SE_se_padding"		dicomtype="?"	dicomtag="?"	#  Spare Space
block="CTHDR"  condition="isct"	offset="0"	intype="String"	inlength="4"	name="CT_im_suid"		dicomtype="?"	dicomtag="?"		#  Suite id for this image
block="CTHDR"  condition="isct"	offset="4"	intype="Int16_B"	inlength="1"	name="CT_im_uniq"		dicomtype="?"	dicomtag="?"		#  The Make-Unique Flag
block="CTHDR"  condition="isct"	offset="6"	intype="String"	inlength="1"	name="CT_im_diskid"		dicomtype="?"	dicomtag="?"		#  Disk ID for this Image
block="CTHDR"  condition="isct"	offset="8"	intype="Uint16_B"	inlength="1"	name="CT_im_exno"		dicomtype="?"	dicomtag="?"		#  Exam number for this image
block="CTHDR"  condition="isct"	offset="10"	intype="Int16_B"	inlength="1"	name="CT_im_seno"		dicomtype="?"	dicomtag="?"		#  Series Number for this image
block="CTHDR"  condition="isct"	offset="12"	intype="Int16_B"	inlength="1"	keyword="CT_im_no"		dicomtype="IS"	dicomtag="InstanceNumber"	#  Image Number
block="CTHDR"  condition="isct"	offset="14"	intype="Unix_DateTime_B"	inlength="1"	keyword="CT_im_datetime"	dicomtype="?"	dicomtag="?"		#  Allocation Image date/time stamp
block="CTHDR"  condition="isct"	offset="18"	intype="Unix_DateTime_B"	inlength="1"	keyword="CT_im_actual_dt"	dicomtype="?"	dicomtag="?"		#  Actual Image date/time stamp
block="CTHDR"  condition="isct"	offset="22"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_sctime"		dicomtype="?"	dicomtag="?"		#  Duration of scan (secs)
block="CTHDR"  condition="isct"	offset="26"	intype="IEEE_Float32_B"	inlength="1"	name="CT_slthick"		dicomtype="DS"	dicomtag="SliceThickness"	#  Slice Thickness (mm)
block="CTHDR"  condition="isct"	offset="30"	intype="Int16_B"	inlength="1"	keyword="CT_imatrix_X"		dicomtype="?"	dicomtag="?"		#  Image matrix size - X
block="CTHDR"  condition="isct"	offset="32"	intype="Int16_B"	inlength="1"	keyword="CT_imatrix_Y"		dicomtype="?"	dicomtag="?"		#  Image matrix size - Y
block="CTHDR"  condition="isct"	offset="34"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_dfov"		dicomtype="?"	dicomtag="?"		#  Display field of view - X (mm)
block="CTHDR"  condition="isct"	offset="38"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_dfov_rect"		dicomtype="?"	dicomtag="?"		#  Display field of view - Y (if different)
block="CTHDR"  condition="isct"	offset="42"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_dim_X"		dicomtype="?"	dicomtag="?"		#  Image dimension - X
block="CTHDR"  condition="isct"	offset="46"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_dim_Y"		dicomtype="?"	dicomtag="?"		#  Image dimension - Y
block="CTHDR"  condition="isct"	offset="50"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_pixsize_X"		dicomtype="?"	dicomtag="?"	#  Image pixel size - X
block="CTHDR"  condition="isct"	offset="54"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_pixsize_Y"		dicomtype="?"	dicomtag="?"	#  Image pixel size - Y
block="CTHDR"  condition="isct"	offset="58"	intype="String"	inlength="14"	name="CT_pdid"		dicomtype="?"	dicomtag="?"		#  Pixel Data ID
block="CTHDR"  condition="isct"	offset="72"	intype="String"	inlength="17"	name="CT_contrastIV"		dicomtype="?"	dicomtag="?"		#  IV Contrast Agent
block="CTHDR"  condition="isct"	offset="89"	intype="String"	inlength="17"	name="CT_contrastOral"	dicomtype="?"	dicomtag="?"		#  Oral Contrast Agent
block="CTHDR"  condition="isct"	offset="106"	intype="Int16_B"	inlength="1"	name="CT_contmode"		dicomtype="?"	dicomtag="?"	bitmap="0:none,oral;1:none,intravenous"	#  Image Contrast Mode
block="CTHDR"  condition="isct"	offset="108"	intype="Int16_B"	inlength="1"	name="CT_serrx"		dicomtype="?"	dicomtag="?"		#  Series from which prescribed
block="CTHDR"  condition="isct"	offset="110"	intype="Int16_B"	inlength="1"	name="CT_imgrx"		dicomtype="?"	dicomtag="?"		#  Image from which prescribed
block="CTHDR"  condition="isct"	offset="112"	intype="Int16_B"	inlength="1"	name="CT_screenformat"	dicomtype="?"	dicomtag="?"		#  Screen Format(8/16 bit)
block="CTHDR"  condition="isct"	offset="114"	intype="Int16_B"	inlength="1"	name="CT_plane"		dicomtype="?"	dicomtag="?"	bitmap="0:none,Scout Plane(SCT);1:none,Axial Plane(Ax);2:none,Sagittal Plane(Sag);3:none,Coronal Plane(Cor);4:none,Oblique Plane(O);5:none,ParAxial Plane(PAX);6:none,Reformatted Plane(RFMT);7:none,Projected Plane(PJN);8:none,Mixed Plane(MIXED)"	#  Plane Type
block="CTHDR"  condition="isct"	offset="116"	intype="IEEE_Float32_B"	inlength="1"	name="CT_scanspacing"	dicomtype="DS"	dicomtag="SpacingBetweenSlices"	#  Spacing between scans (mm?)
block="CTHDR"  condition="isct"	offset="120"	intype="Int16_B"	inlength="1"	name="CT_im_compress"	dicomtype="?"	dicomtag="?"		#  Image compression type for allocation
block="CTHDR"  condition="isct"	offset="122"	intype="Int16_B"	inlength="1"	name="CT_im_scouttype"	dicomtype="?"	dicomtag="?"	enum="1=Scout Vertical,2=Scout Horizontal"	#  Scout Type (AP or lateral)
block="CTHDR"  condition="isct"	offset="124"	intype="String"	inlength="1"	name="CT_loc_ras"		dicomtype="?"	dicomtag="?"		#  RAS letter of image location
block="CTHDR"  condition="isct"	offset="126"	intype="IEEE_Float32_B"	inlength="1"	name="CT_loc"		dicomtype="DS"	dicomtag="SliceLocation"	#  Image location
block="CTHDR"  condition="isct"	offset="130"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_ctr_R"		dicomtype="?"	dicomtag="?"		#  Center R coord of plane image
block="CTHDR"  condition="isct"	offset="134"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_ctr_A"		dicomtype="?"	dicomtag="?"		#  Center A coord of plane image
block="CTHDR"  condition="isct"	offset="138"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_ctr_S"		dicomtype="?"	dicomtag="?"		#  Center S coord of plane image
block="CTHDR"  condition="isct"	offset="142"	intype="IEEE_Float32_B"	inlength="1"	name="CT_norm_R"		dicomtype="?"	dicomtag="?"		#  Normal R coord
block="CTHDR"  condition="isct"	offset="146"	intype="IEEE_Float32_B"	inlength="1"	name="CT_norm_A"		dicomtype="?"	dicomtag="?"		#  Normal A coord
block="CTHDR"  condition="isct"	offset="150"	intype="IEEE_Float32_B"	inlength="1"	name="CT_norm_S"		dicomtype="?"	dicomtag="?"		#  Normal S coord
block="CTHDR"  condition="isct"	offset="154"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_tlhc_R"		dicomtype="?"	dicomtag="?"		#  R Coord of Top Left Hand
block="CTHDR"  condition="isct"	offset="158"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_tlhc_A"		dicomtype="?"	dicomtag="?"		#  A Coord of Top Left Hand
block="CTHDR"  condition="isct"	offset="162"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_tlhc_S"		dicomtype="?"	dicomtag="?"		#  S Coord of Top Left Hand
block="CTHDR"  condition="isct"	offset="166"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_trhc_R"		dicomtype="?"	dicomtag="?"		#  R Coord of Top Right Hand
block="CTHDR"  condition="isct"	offset="170"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_trhc_A"		dicomtype="?"	dicomtag="?"		#  A Coord of Top Right Hand
block="CTHDR"  condition="isct"	offset="174"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_trhc_S"		dicomtype="?"	dicomtag="?"		#  S Coord of Top Right Hand
block="CTHDR"  condition="isct"	offset="178"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_brhc_R"		dicomtype="?"	dicomtag="?"		#  R Coord of Bottom Right Hand
block="CTHDR"  condition="isct"	offset="182"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_brhc_A"		dicomtype="?"	dicomtag="?"		#  A Coord of Bottom Right Hand
block="CTHDR"  condition="isct"	offset="186"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_brhc_S"		dicomtype="?"	dicomtag="?"		#  S Coord of Bottom Right Hand
block="CTHDR"  condition="isct"	offset="190"	intype="String"	inlength="4"	name="CT_forimgrev"		dicomtype="?"	dicomtag="?"		#  Foreign Image Revision
block="CTHDR"  condition="isct"	offset="194"	intype="IEEE_Float32_B"	inlength="1"	name="CT_sctstr"		dicomtype="?"	dicomtag="?"		#  Table Start Location
block="CTHDR"  condition="isct"	offset="198"	intype="IEEE_Float32_B"	inlength="1"	name="CT_sctend"		dicomtype="?"	dicomtag="?"		#  Table End Location
block="CTHDR"  condition="isct"	offset="202"	intype="IEEE_Float32_B"	inlength="1"	name="CT_tblspd"		dicomtype="?"	dicomtag="?"		#  Table Speed (mm/sec)
block="CTHDR"  condition="isct"	offset="206"	intype="IEEE_Float32_B"	inlength="1"	name="CT_tblht"		dicomtype="DS"	dicomtag="TableHeight"	#  Table Height
block="CTHDR"  condition="isct"	offset="210"	intype="IEEE_Float32_B"	inlength="1"	name="CT_midstime"		dicomtype="?"	dicomtag="?"		#  Mid Scan Time
block="CTHDR"  condition="isct"	offset="214"	intype="Int16_B"	inlength="1"	name="CT_midsflag"		dicomtype="?"	dicomtag="?"		#  does midstime apply
block="CTHDR"  condition="isct"	offset="216"	intype="Int32_B"	inlength="1"	name="CT_kvolt"			dicomtype="DS"	dicomtag="KVP"		#  KVolt generator setting
block="CTHDR"  condition="isct"	offset="220"	intype="Int32_B"	inlength="1"	keyword="CT_mamp"		dicomtype="IS"	dicomtag="XRayTubeCurrent"	#  MAmp generator setting
block="CTHDR"  condition="isct"	offset="224"	intype="IEEE_Float32_B"	inlength="1"	name="CT_gantilt"		dicomtype="DS"	dicomtag="GantryDetectorTilt"	#  Gantry Tilt (degrees)
block="CTHDR"  condition="isct"	offset="228"	intype="Int32_B"	inlength="1"	name="CT_azimuth"		dicomtype="?"	dicomtag="?"		#  Degrees of Azimuth
block="CTHDR"  condition="isct"	offset="232"	intype="IEEE_Float32_B"	inlength="1"	name="CT_ganvel"		dicomtype="?"	dicomtag="?"		#  Gantry Velocity
block="CTHDR"  condition="isct"	offset="236"	intype="Int32_B"	inlength="1"	name="CT_ganfilt"		dicomtype="?"	dicomtag="?"		#  Gantry Filter Position
block="CTHDR"  condition="isct"	offset="240"	intype="IEEE_Float32_B"	inlength="1"	name="CT_trigon"		dicomtype="?"	dicomtag="?"		#  Trigger on Position
block="CTHDR"  condition="isct"	offset="244"	intype="IEEE_Float32_B"	inlength="1"	name="CT_degrot"		dicomtype="?"	dicomtag="?"		#  Degrees of rotation
block="CTHDR"  condition="isct"	offset="248"	intype="IEEE_Float32_B"	inlength="1"	name="CT_xrayon"		dicomtype="?"	dicomtag="?"		#  X-Ray On Position
block="CTHDR"  condition="isct"	offset="252"	intype="IEEE_Float32_B"	inlength="1"	name="CT_xrayoff"		dicomtype="?"	dicomtag="?"		#  X-Ray Off Position
block="CTHDR"  condition="isct"	offset="256"	intype="Int32_B"	inlength="1"	name="CT_numtrig"		dicomtype="?"	dicomtag="?"		#  Number of Triggers
block="CTHDR"  condition="isct"	offset="260"	intype="Int16_B"	inlength="1"	name="CT_inviews"		dicomtype="?"	dicomtag="?"		#  Total input views
block="CTHDR"  condition="isct"	offset="262"	intype="IEEE_Float32_B"	inlength="1"	name="CT_view1ang"		dicomtype="?"	dicomtag="?"		#  Angle of first view
block="CTHDR"  condition="isct"	offset="266"	intype="IEEE_Float32_B"	inlength="1"	name="CT_trigfreq"		dicomtype="?"	dicomtag="?"		#  Trigger frequency
block="CTHDR"  condition="isct"	offset="270"	intype="Int32_B"	inlength="1"	name="CT_trigsrc"		dicomtype="?"	dicomtag="?"	enum="1=internal,2=external"	#  DAS trigger source
block="CTHDR"  condition="isct"	offset="274"	intype="Int32_B"	inlength="1"	name="CT_fpagain"		dicomtype="?"	dicomtag="?"	bitmap="0:none,auto;1:none,X1;3:none,X8;6:none,X64"	#  DAS fpa gain
block="CTHDR"  condition="isct"	offset="278"	intype="Int32_B"	inlength="1"	name="CT_scanopmode"		dicomtype="?"	dicomtag="?"	bitmap="0:none,???;1:none,???;2:none,???;3:none,scout mode;4:none,axial Xron mode;5:none,axial Xroff mode;6:none,static Xron mode;7:none,static Xroff mode;8:none,tube heat mode;9:none,DAS mode;10:none,tube cal mode;11:none,biopsy mode;12:none,cine mode;13:none,helical mode;14:none,rotgencal mode"	#  Scan Type
block="CTHDR"  condition="isct"	offset="282"	intype="Int32_B"	inlength="1"	name="CT_outsrc"		dicomtype="?"	dicomtag="?"	enum="1=AID,2=XM"	#  DAS output source
block="CTHDR"  condition="isct"	offset="286"	intype="Int32_B"	inlength="1"	name="CT_adin"		dicomtype="?"	dicomtag="?"	enum="1=DAS Filter Card,2=Aux"	#  DAS ad input
block="CTHDR"  condition="isct"	offset="290"	intype="Int32_B"	inlength="1"	name="CT_calmode"		dicomtype="?"	dicomtag="?"	enum="1=DC DAS Cal,2=AC DAS Cal,3=No DAS Cal"	#  DAS cal mode
block="CTHDR"  condition="isct"	offset="294"	intype="Int32_B"	inlength="1"	name="CT_calfreq"		dicomtype="?"	dicomtag="?"	enum="0=?,1=?"	#  DAS cal frequency
block="CTHDR"  condition="isct"	offset="298"	intype="Int32_B"	inlength="1"	name="CT_regxm"		dicomtype="?"	dicomtag="?"	enum="1=hold,2=shift"	#  DAS reg xm
block="CTHDR"  condition="isct"	offset="302"	intype="Int32_B"	inlength="1"	name="CT_autozero"		dicomtype="?"	dicomtag="?"		#  DAS auto zero
block="CTHDR"  condition="isct"	offset="306"	intype="Int16_B"	inlength="1"	name="CT_sfovtyp"		dicomtype="?"	dicomtag="?"	bitmap="0:none,Ped Head;1:none,Adult Head;2:none,Small;3:none,Medium;4:none,Large;5:none,No Sfovtype"	#  Axial Type
block="CTHDR"  condition="isct"	offset="308"	intype="Int16_B"	inlength="1"	name="CT_phantsize"		dicomtype="?"	dicomtag="?"	enum="1=No Psize,2=Small Psize,3=Medium Psize,4=Large Psize"	#  Calibration phantom size
block="CTHDR"  condition="isct"	offset="310"	intype="Int16_B"	inlength="1"	name="CT_phanttyp"		dicomtype="?"	dicomtag="?"	enum="1=None,2=Air,3=Water,4=Poly"	#  Calibration phantom type
block="CTHDR"  condition="isct"	offset="312"	intype="Int16_B"	inlength="1"	keyword="CT_filttyp"		dicomtype="?"	dicomtag="?"	enum="1=Air Filter,2=Body Filter,3=Bowtie Flat Filter:Adult Head,4=Flat Filter,5=High Filter:Adult Head/HF"	#  Calibration filter type
block="CTHDR"  condition="isct"	offset="314"	intype="Int16_B"	inlength="1"	keyword="CT_reconalg"		dicomtype="?"	dicomtag="?"	bitmap="0:none,smooth(SMTH);1:none,soft(SOFT);2:none,standard(STND);3:none,detail(DETL);4:none,bone(BONE);5:none,edge(EDGE);6:none,sharp(SHRP);7:none,experimental 2(EXP2)"	#  Recon Algorithm
block="CTHDR"  condition="isct"	offset="316"	intype="Int16_B"	inlength="1"	keyword="CT_perisflag"		dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes:/P"	#  Peristaltic flag
block="CTHDR"  condition="isct"	offset="318"	intype="Int16_B"	inlength="1"	keyword="CT_iboneflag"		dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes:/I"	#  IterBone flag
block="CTHDR"  condition="isct"	offset="320"	intype="Int16_B"	inlength="1"	name="CT_statflag"		dicomtype="?"	dicomtag="?"		#  Stat Recon flag
block="CTHDR"  condition="isct"	offset="322"	intype="Int16_B"	inlength="1"	name="CT_computetyp"		dicomtype="?"	dicomtag="?"	enum="1=axial,2=calcheck,3=scout,4=ppscan,5=viewsvschannels"	#  Compute Type
block="CTHDR"  condition="isct"	offset="324"	intype="Int16_B"	inlength="1"	name="CT_segnum"		dicomtype="?"	dicomtag="?"		#  Segment Number
block="CTHDR"  condition="isct"	offset="326"	intype="Int16_B"	inlength="1"	name="CT_segstotal"		dicomtype="?"	dicomtag="?"		#  Total Number of Segments Requested
block="CTHDR"  condition="isct"	offset="328"	intype="IEEE_Float32_B"	inlength="1"	name="CT_isd"		dicomtype="?"	dicomtag="?"		#  Inter scan delay (secs)
block="CTHDR"  condition="isct"	offset="332"	intype="IEEE_Float32_B"	inlength="1"	keyword="CT_sfovmm"		dicomtype="DS"	dicomtag="DataCollectionDiameter"		#  Scan field of view (mm)
block="CTHDR"  condition="isct"	offset="336"	intype="Int16_B"	inlength="1"	name="CT_scannum"		dicomtype="IS"	dicomtag="AcquisitionNumber"		#  Scan Number
block="CTHDR"  condition="isct"	offset="338"	intype="Int16_B"	inlength="1"	name="CT_viewstrtchan"	dicomtype="?"	dicomtag="?"		#  Starting Channel of View
block="CTHDR"  condition="isct"	offset="340"	intype="Int16_B"	inlength="1"	name="CT_viewcompfctr"	dicomtype="?"	dicomtag="?"		#  View Compression Factor
block="CTHDR"  condition="isct"	offset="342"	intype="Int16_B"	inlength="1"	name="CT_outviews"		dicomtype="?"	dicomtag="?"		#  Total Output Views
block="CTHDR"  condition="isct"	offset="344"	intype="Int16_B"	inlength="1"	name="CT_overranges"		dicomtype="?"	dicomtag="?"		#  Number of Overranges
block="CTHDR"  condition="isct"	offset="346"	intype="Int16_B"	inlength="1"	name="CT_totrefchan"		dicomtype="?"	dicomtag="?"		#  Total Number of Ref Channels
block="CTHDR"  condition="isct"	offset="348"	intype="Int32_B"	inlength="1"	name="CT_scdatasize"		dicomtype="?"	dicomtag="?"		#  data size for scan data
block="CTHDR"  condition="isct"	offset="352"	intype="Int16_B"	inlength="1"	name="CT_refchan1"		dicomtype="?"	dicomtag="?"		#  z or q channel
block="CTHDR"  condition="isct"	offset="354"	intype="Int16_B"	inlength="1"	name="CT_refchan2"		dicomtype="?"	dicomtag="?"		#  Reference channel 1
block="CTHDR"  condition="isct"	offset="356"	intype="Int16_B"	inlength="1"	name="CT_refchan3"		dicomtype="?"	dicomtag="?"		#  Reference channel 2
block="CTHDR"  condition="isct"	offset="358"	intype="Int16_B"	inlength="1"	name="CT_refchan4"		dicomtype="?"	dicomtag="?"		#  Reference channel 3
block="CTHDR"  condition="isct"	offset="360"	intype="Int16_B"	inlength="1"	name="CT_refchan5"		dicomtype="?"	dicomtag="?"		#  Reference channel 4
block="CTHDR"  condition="isct"	offset="362"	intype="Int16_B"	inlength="1"	name="CT_refchan6"		dicomtype="?"	dicomtag="?"		#  Reference channel 5
block="CTHDR"  condition="isct"	offset="364"	intype="Int16_B"	inlength="1"	name="CT_postproc"		dicomtype="?"	dicomtag="?"		#  Recon post processing flag
block="CTHDR"  condition="isct"	offset="366"	intype="Int32_B"	inlength="1"	name="CT_xmpat"		dicomtype="?"	dicomtag="?"		#  DAS xm pattern
block="CTHDR"  condition="isct"	offset="370"	intype="Int16_B"	inlength="1"	name="CT_rottyp"		dicomtype="?"	dicomtag="?"	bitmap="0:none,half scan(rotating);1:none,normal scan(rotating);2:none,over scan(rotating);3:none,stationary(stationary)"	#  Prescribed rotation type
block="CTHDR"  condition="isct"	offset="372"	intype="Int16_B"	inlength="1"	name="CT_rawdataflag"	dicomtype="?"	dicomtag="?"		#  Save Raw Data Flag
block="CTHDR"  condition="isct"	offset="374"	intype="IEEE_Float32_B"	inlength="1"	name="CT_ct_scalefact"	dicomtype="?"	dicomtag="?"		#  IBH Image scale factors
block="CTHDR"  condition="isct"	offset="378"	intype="Int16_B"	inlength="1"	name="CT_ct_water_num"	dicomtype="?"	dicomtag="?"		#  CT Water Number
block="CTHDR"  condition="isct"	offset="380"	intype="Int16_B"	inlength="1"	name="CT_ct_bone_num"	dicomtype="?"	dicomtag="?"		#  CT Bone Number
block="CTHDR"  condition="isct"	offset="382"	intype="IEEE_Float32_B"	inlength="1"	name="CT_bbh_coef1"		dicomtype="?"	dicomtag="?"		#  BBH coefficient 1
block="CTHDR"  condition="isct"	offset="386"	intype="IEEE_Float32_B"	inlength="1"	name="CT_bbh_coef2"		dicomtype="?"	dicomtag="?"		#  BBH coefficient 2
block="CTHDR"  condition="isct"	offset="390"	intype="IEEE_Float32_B"	inlength="1"	name="CT_bbh_coef3"		dicomtype="?"	dicomtag="?"		#  BBH coefficient 3
block="CTHDR"  condition="isct"	offset="394"	intype="Int16_B"	inlength="1"	name="CT_bbh_numblend"	dicomtype="?"	dicomtag="?"		#  Num of BBH channels to blend
block="CTHDR"  condition="isct"	offset="396"	intype="Int32_B"	inlength="1"	name="CT_firstchan"		dicomtype="?"	dicomtag="?"		#  Starting channel
block="CTHDR"  condition="isct"	offset="400"	intype="Int32_B"	inlength="1"	name="CT_numchan"		dicomtype="?"	dicomtag="?"		#  Number of channels (1..512)
block="CTHDR"  condition="isct"	offset="404"	intype="Int32_B"	inlength="1"	name="CT_chaninc"		dicomtype="?"	dicomtag="?"		#  Increment between channels
block="CTHDR"  condition="isct"	offset="408"	intype="Int32_B"	inlength="1"	name="CT_firstview"		dicomtype="?"	dicomtag="?"		#  Starting view
block="CTHDR"  condition="isct"	offset="412"	intype="Int32_B"	inlength="1"	name="CT_numview"		dicomtype="?"	dicomtag="?"		#  Number of views
block="CTHDR"  condition="isct"	offset="416"	intype="Int32_B"	inlength="1"	name="CT_viewinc"		dicomtype="?"	dicomtag="?"		#  Increment between views
block="CTHDR"  condition="isct"	offset="420"	intype="Int32_B"	inlength="1"	name="CT_windowrange"	dicomtype="?"	dicomtag="?"		#  Window Range (0..4095)
block="CTHDR"  condition="isct"	offset="424"	intype="IEEE_Float32_B"	inlength="1"	name="CT_scalemin"		dicomtype="?"	dicomtag="?"		#  Scaling value of the image data
block="CTHDR"  condition="isct"	offset="428"	intype="IEEE_Float32_B"	inlength="1"	name="CT_scalemax"		dicomtype="?"	dicomtag="?"		#  Scaling value of the image data
block="CTHDR"  condition="isct"	offset="432"	intype="Int32_B"	inlength="1"	name="CT_datamod"		dicomtype="?"	dicomtag="?"	enum="1=Raw,2=Offset Corrected,3=Normalized"	#  Amount of processing that will be
block="CTHDR"  condition="isct"	offset="436"	intype="String"	inlength="13"	name="CT_qcalfile"		dicomtype="?"	dicomtag="?"		#  Source of the qcal vectors
block="CTHDR"  condition="isct"	offset="449"	intype="String"	inlength="13"	name="CT_calmodfile"		dicomtype="?"	dicomtag="?"		#  Source of the cal vectors
block="CTHDR"  condition="isct"	offset="462"	intype="Int16_B"	inlength="1"	name="CT_wordsperview"	dicomtype="?"	dicomtag="?"		#  Number of words per view
block="CTHDR"  condition="isct"	offset="464"	intype="String"	inlength="1"	name="CT_rl_ras"		dicomtype="?"	dicomtag="?"		#  RAS letter for side of image
block="CTHDR"  condition="isct"	offset="465"	intype="String"	inlength="1"	name="CT_ap_ras"		dicomtype="?"	dicomtag="?"		#  RAS letter for anterior/posterior
block="CTHDR"  condition="isct"	offset="466"	intype="String"	inlength="1"	name="CT_sctstr_ras"		dicomtype="?"	dicomtag="?"		#  RAS letter for scout start loc
block="CTHDR"  condition="isct"	offset="467"	intype="String"	inlength="1"	name="CT_sctend_ras"		dicomtype="?"	dicomtag="?"		#  RAS letter for scout end loc
block="CTHDR"  condition="isct"	offset="468"	intype="String"	inlength="3"	name="CT_sct_anref"		dicomtype="?"	dicomtag="?"		#  Anatomical reference for scout
block="CTHDR"  condition="isct"	offset="472"	intype="Int16_B"	inlength="1"	name="CT_pps_scalwin"	dicomtype="?"	dicomtag="?"		#  PpScan window range for output Scaling
block="CTHDR"  condition="isct"	offset="474"	intype="Int16_B"	inlength="1"	name="CT_pps_qcalflag"	dicomtype="?"	dicomtag="?"		#  PpScan Qcal modification flag
block="CTHDR"  condition="isct"	offset="476"	intype="Int16_B"	inlength="1"	name="CT_pps_pcalflag"	dicomtype="?"	dicomtag="?"		#  PpScan Pcal modification flag
block="CTHDR"  condition="isct"	offset="478"	intype="Int16_B"	inlength="1"	name="CT_pps_thetafix"	dicomtype="?"	dicomtag="?"		#  PpScan Theta Fix (Angle Correction)
block="CTHDR"  condition="isct"	offset="480"	intype="Int16_B"	inlength="1"	name="CT_pps_bhflag"		dicomtype="?"	dicomtag="?"		#  PpScan Beam Hardening Flag
block="CTHDR"  condition="isct"	offset="482"	intype="Int16_B"	inlength="1"	name="CT_spot_size"		dicomtype="?"	dicomtag="?"	enum="1=small,2=large"	#  tube focal spot size
block="CTHDR"  condition="isct"	offset="484"	intype="Int16_B"	inlength="1"	name="CT_spot_pos"		dicomtype="?"	dicomtag="?"	enum="1=left,2=center,3=right"	#  tube focal spot position
block="CTHDR"  condition="isct"	offset="486"	intype="Int16_B"	inlength="1"	name="CT_recondataset"	dicomtype="?"	dicomtag="?"	bitmap="0:none,half scan views;1:none,normal scan views;2:none,over scan views;3:none,variable views"	#  Dependent on number of views processed
block="CTHDR"  condition="isct"	offset="488"	intype="Int16_B"	inlength="1"	name="CT_ndetcellsfov"	dicomtype="?"	dicomtag="?"		#  Field of view in detector cells
block="CTHDR"  condition="isct"	offset="490"	intype="IEEE_Float64_B"	inlength="1"	name="CT_strtscantime"	dicomtype="?"	dicomtag="?"		#  Start time(secs) of this scan
block="CTHDR"  condition="isct"	offset="498"	intype="Int16_B"	inlength="1"	keyword="CT_gandir"		dicomtype="?"	dicomtag="?"	enum="1=Clockwise:CW,2=CounterClockwise:CCW"	#  Gantry Rotation Direction
block="CTHDR"  condition="isct"	offset="500"	intype="Int16_B"	inlength="1"	name="CT_rotorspeed"		dicomtype="?"	dicomtag="?"	enum="1=off,2=low,3=medium,4=high"	#  Tube Rotor Speed
block="CTHDR"  condition="isct"	offset="502"	intype="Int16_B"	inlength="1"	name="CT_trigmode"		dicomtype="?"	dicomtag="?"	enum="1=Normal Trigger:XTTX,2=XRayoff Trigger:TT,3=TXXT,4=XXTT"	#  TGGC Trigger Mode
block="CTHDR"  condition="isct"	offset="504"	intype="IEEE_Float32_B"	inlength="1"	name="CT_sitilt"		dicomtype="?"	dicomtag="?"		#  Rx'd gantry tilt - not annotated
block="CTHDR"  condition="isct"	offset="508"	intype="IEEE_Float32_B"	inlength="1"	name="CT_targcen_R"		dicomtype="?"	dicomtag="?"		#  R/L coordinate for target recon center
block="CTHDR"  condition="isct"	offset="512"	intype="IEEE_Float32_B"	inlength="1"	name="CT_targcen_A"		dicomtype="?"	dicomtag="?"		#  A/P coordinate for target recon center
block="CTHDR"  condition="isct"	offset="516"	intype="Int16_B"	inlength="1"	name="CT_backprojflag"	dicomtype="?"	dicomtag="?"		#  Value of Back Projection button
block="CTHDR"  condition="isct"	offset="518"	intype="Int16_B"	inlength="1"	name="CT_fatqestflag"	dicomtype="?"	dicomtag="?"		#  Set if fatq estimates were used
block="CTHDR"  condition="isct"	offset="520"	intype="IEEE_Float32_B"	inlength="1"	name="CT_zavg"		dicomtype="?"	dicomtag="?"		#  Z chan avg over views
block="CTHDR"  condition="isct"	offset="524"	intype="IEEE_Float32_B"	inlength="1"	name="CT_leftrefavg"		dicomtype="?"	dicomtag="?"		#  avg of left ref chans over
block="CTHDR"  condition="isct"	offset="528"	intype="IEEE_Float32_B"	inlength="1"	name="CT_leftrefmax"		dicomtype="?"	dicomtag="?"		#  max left chan value over views
block="CTHDR"  condition="isct"	offset="532"	intype="IEEE_Float32_B"	inlength="1"	name="CT_rightrefavg"	dicomtype="?"	dicomtag="?"		#  avg of right ref chans over
block="CTHDR"  condition="isct"	offset="536"	intype="IEEE_Float32_B"	inlength="1"	name="CT_rightrefmax"	dicomtype="?"	dicomtag="?"		#  max right chan value over views
block="CTHDR"  condition="isct"	offset="540"	intype="String"	inlength="13"	name="CT_im_alloc_key"	dicomtype="?"	dicomtag="?"		#  process that last allocated this record
block="CTHDR"  condition="isct"	offset="554"	intype="Unix_DateTime_B"	inlength="1"	keyword="CT_im_lastmod"		dicomtype="?"	dicomtag="?"		#  Date/Time of Last Change
block="CTHDR"  condition="isct"	offset="558"	intype="String"	inlength="2"	name="CT_im_verscre"		dicomtype="?"	dicomtag="?"		#  Genesis version - Created
block="CTHDR"  condition="isct"	offset="560"	intype="String"	inlength="2"	name="CT_im_verscur"		dicomtype="?"	dicomtag="?"		#  Genesis version - Now
block="CTHDR"  condition="isct"	offset="562"	intype="Int32_B"	inlength="1"	name="CT_im_pds_a"		dicomtype="?"	dicomtag="?"		#  PixelData size - as stored
block="CTHDR"  condition="isct"	offset="566"	intype="Int32_B"	inlength="1"	name="CT_im_pds_c"		dicomtype="?"	dicomtag="?"		#  PixelData size - Compressed
block="CTHDR"  condition="isct"	offset="570"	intype="Int32_B"	inlength="1"	name="CT_im_pds_u"		dicomtype="?"	dicomtag="?"		#  PixelData size - UnCompressed
block="CTHDR"  condition="isct"	offset="574"	intype="Uint32_B"	inlength="1"	name="CT_im_checksum"	dicomtype="?"	dicomtag="?"		#  AcqRecon record checksum
block="CTHDR"  condition="isct"	offset="578"	intype="Int32_B"	inlength="1"	name="CT_im_archived"	dicomtype="?"	dicomtag="?"		#  Image Archive Flag
block="CTHDR"  condition="isct"	offset="582"	intype="Int32_B"	inlength="1"	name="CT_im_complete"	dicomtype="?"	dicomtag="?"		#  Image Complete Flag
block="CTHDR"  condition="isct"	offset="586"	intype="Int16_B"	inlength="1"	name="CT_biop_pos"		dicomtype="?"	dicomtag="?"	bitmap="0:none,superior;1:none,centered;2:none,inferior"	#  Biopsy Position
block="CTHDR"  condition="isct"	offset="588"	intype="IEEE_Float32_B"	inlength="1"	name="CT_biop_tloc"		dicomtype="?"	dicomtag="?"		#  Biopsy T Location
block="CTHDR"  condition="isct"	offset="592"	intype="IEEE_Float32_B"	inlength="1"	name="CT_biop_refloc"	dicomtype="?"	dicomtag="?"		#  Biopsy Ref Location
block="CTHDR"  condition="isct"	offset="596"	intype="Int16_B"	inlength="1"	name="CT_ref_chan"		dicomtype="?"	dicomtag="?"	enum="0=no,1=yes"	#  Reference Channel Used
block="CTHDR"  condition="isct"	offset="598"	intype="IEEE_Float32_B"	inlength="1"	name="CT_bp_coef"		dicomtype="?"	dicomtag="?"		#  Back Projector Coefficient
block="CTHDR"  condition="isct"	offset="602"	intype="Int16_B"	inlength="1"	name="CT_psc"		dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	#  Primary Speed Correction Used
block="CTHDR"  condition="isct"	offset="604"	intype="Int16_B"	inlength="1"	name="CT_overrng_corr"	dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	#  Overrange Correction Used
block="CTHDR"  condition="isct"	offset="606"	intype="IEEE_Float32_B"	inlength="1"	name="CT_dyn_z_alpha"	dicomtype="?"	dicomtag="?"		#  Dynamic Z Alpha Value
block="CTHDR"  condition="isct"	offset="610"	intype="String"	inlength="1"	name="CT_ref_img"		dicomtype="?"	dicomtag="?"		#  Reference Image Field
block="CTHDR"  condition="isct"	offset="611"	intype="String"	inlength="1"	name="CT_sum_img"		dicomtype="?"	dicomtag="?"		#  Summary Image Field
block="CTHDR"  condition="isct"	offset="612"	intype="Uint16_B"	inlength="1"	name="CT_img_window"		dicomtype="?"	dicomtag="?"		#  Window Value
block="CTHDR"  condition="isct"	offset="614"	intype="Int16_B"	inlength="1"	name="CT_img_level"		dicomtype="?"	dicomtag="?"		#  Level Value
block="CTHDR"  condition="isct"	offset="616"	intype="Int32_B"	inlength="1"	name="CT_slop_int_1"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 1
block="CTHDR"  condition="isct"	offset="620"	intype="Int32_B"	inlength="1"	name="CT_slop_int_2"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 2
block="CTHDR"  condition="isct"	offset="624"	intype="Int32_B"	inlength="1"	name="CT_slop_int_3"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 3
block="CTHDR"  condition="isct"	offset="628"	intype="Int32_B"	inlength="1"	name="CT_slop_int_4"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 4
block="CTHDR"  condition="isct"	offset="632"	intype="Int32_B"	inlength="1"	name="CT_slop_int_5"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 5
block="CTHDR"  condition="isct"	offset="636"	intype="IEEE_Float32_B"	inlength="1"	name="CT_slop_float_1"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 1
block="CTHDR"  condition="isct"	offset="640"	intype="IEEE_Float32_B"	inlength="1"	name="CT_slop_float_2"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 2
block="CTHDR"  condition="isct"	offset="644"	intype="IEEE_Float32_B"	inlength="1"	name="CT_slop_float_3"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 3
block="CTHDR"  condition="isct"	offset="648"	intype="IEEE_Float32_B"	inlength="1"	name="CT_slop_float_4"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 4
block="CTHDR"  condition="isct"	offset="652"	intype="IEEE_Float32_B"	inlength="1"	name="CT_slop_float_5"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 5
block="CTHDR"  condition="isct"	offset="656"	intype="String"	inlength="16"	name="CT_slop_str_1"		dicomtype="?"	dicomtag="?"		#  String Slop Field 1
block="CTHDR"  condition="isct"	offset="672"	intype="String"	inlength="16"	name="CT_slop_str_2"		dicomtype="?"	dicomtag="?"		#  String Slop Field 2
block="CTHDR"  condition="isct"	offset="688"	intype="IEEE_Float32_B"	inlength="1"	name="CT_deltastart"		dicomtype="?"	dicomtag="?"		#  Delta Start Time
block="CTHDR"  condition="isct"	offset="692"	intype="Int32_B"	inlength="1"	name="CT_maxOverrange"	dicomtype="?"	dicomtag="?"		#  Max Overranges In A View
block="CTHDR"  condition="isct"	offset="696"	intype="IEEE_Float32_B"	inlength="1"	name="CT_avgOverrange"	dicomtype="?"	dicomtag="?"		#  Avg Overranges All Views
block="CTHDR"  condition="isct"	offset="700"	intype="Int16_B"	inlength="1"	name="CT_afterglow"		dicomtype="?"	dicomtag="?"	bitmap="0:PSC term off,PSC term on;1:Second term off,Second term on;2:Third term off,Third term on;3:Fourth term off,Fourth term on"	#  Corrected Afterglow Terms
block="CTHDR"  condition="isct"	offset="702"	intype="Uint16_B"	inlength="1"	name="CT_z_blocked"		dicomtype="?"	dicomtag="?"		#  No Views z Channel Blocked
block="CTHDR"  condition="isct"	offset="704"	intype="Uint16_B"	inlength="1"	name="CT_ref1_blocked"	dicomtype="?"	dicomtag="?"		#  No Views Ref1 Channel Blocked
block="CTHDR"  condition="isct"	offset="706"	intype="Uint16_B"	inlength="1"	name="CT_ref2_blocked"	dicomtype="?"	dicomtag="?"		#  No Views Ref2 Channel Blocked
block="CTHDR"  condition="isct"	offset="708"	intype="Uint16_B"	inlength="1"	name="CT_ref3_blocked"	dicomtype="?"	dicomtag="?"		#  No Views Ref3 Channel Blocked
block="CTHDR"  condition="isct"	offset="710"	intype="Uint16_B"	inlength="1"	name="CT_ref4_blocked"	dicomtype="?"	dicomtag="?"		#  No Views Ref4 Channel Blocked
block="CTHDR"  condition="isct"	offset="712"	intype="Uint16_B"	inlength="1"	name="CT_ref5_blocked"	dicomtype="?"	dicomtag="?"		#  No Views Ref5 Channel Blocked
block="CTHDR"  condition="isct"	offset="714"	intype="Int16_B"	inlength="1"	name="CT_integrity"		dicomtype="?"	dicomtag="?"	enum="0=GE Image,1=Imported Image"	#  GE Image Integrity
block="CTHDR"  condition="isct"	offset="716"	intype="String"	inlength="8"	name="CT_pitchRatio"		dicomtype="?"	dicomtag="?"		#  Scan Pitch Ratio
block="CTHDR"  condition="isct"	offset="724"	intype="String"	inlength="32"	name="CT_image_uid"		dicomtype="?"	dicomtag="?"		#  Image Unique ID
block="CTHDR"  condition="isct"	offset="756"	intype="String"	inlength="32"	name="CT_sop_uid"		dicomtype="?"	dicomtag="?"		#  Service Obj Class Unique ID
block="CTHDR"  condition="isct"	offset="788"	intype="IEEE_Float32_B"	inlength="1"	name="CT_xraydelay"		dicomtype="?"	dicomtag="?"		#  Start Scan To XRay On Delay
block="CTHDR"  condition="isct"	offset="792"	intype="IEEE_Float32_B"	inlength="1"	name="CT_xrayduration"	dicomtype="?"	dicomtag="?"		#  Duration Of XRay On
block="CTHDR"  condition="isct"	offset="796"	intype="String"	inlength="223"	name="CT_ct_padding"		dicomtype="?"	dicomtag="?"		#  Spare Space
block="MRHDR"  condition="ismr"	offset="0"	intype="String"	inlength="4"	name="MR_im_suid"		dicomtype="?"	dicomtag="?"		#  Suite id for this image
block="MRHDR"  condition="ismr"	offset="4"	intype="Int16_B"	inlength="1"	name="MR_im_uniq"		dicomtype="?"	dicomtag="?"		#  The Make-Unique Flag
block="MRHDR"  condition="ismr"	offset="6"	intype="String"	inlength="1"	name="MR_im_diskid"		dicomtype="?"	dicomtag="?"		#  Disk ID for this Image
block="MRHDR"  condition="ismr"	offset="8"	intype="Uint16_B"	inlength="1"	name="MR_im_exno"		dicomtype="?"	dicomtag="?"		#  Exam number for this image
block="MRHDR"  condition="ismr"	offset="10"	intype="Int16_B"	inlength="1"	name="MR_im_seno"		dicomtype="?"	dicomtag="?"		#  Series Number for this image
block="MRHDR"  condition="ismr"	offset="12"	intype="Int16_B"	inlength="1"	keyword="MR_im_no"		dicomtype="IS"	dicomtag="InstanceNumber"	#  Image Number
block="MRHDR"  condition="ismr"	offset="14"	intype="Unix_DateTime_B"	inlength="1"	keyword="MR_im_datetime"	dicomtype="?"	dicomtag="?"		#  Allocation Image date/time stamp
block="MRHDR"  condition="ismr"	offset="18"	intype="Unix_DateTime_B"	inlength="1"	keyword="MR_im_actual_dt"	dicomtype="?"	dicomtag="?"		#  Actual Image date/time stamp
block="MRHDR"  condition="ismr"	offset="22"	intype="IEEE_Float32_B"	inlength="1"	name="MR_sctime"		dicomtype="?"	dicomtag="?"		#  Duration of scan (secs)
block="MRHDR"  condition="ismr"	offset="26"	intype="IEEE_Float32_B"	inlength="1"	name="MR_slthick"		dicomtype="DS"	dicomtag="SliceThickness"	#  Slice Thickness (mm)
block="MRHDR"  condition="ismr"	offset="30"	intype="Int16_B"	inlength="1"	keyword="MR_imatrix_X"		dicomtype="?"	dicomtag="?"		#  Image matrix size - X
block="MRHDR"  condition="ismr"	offset="32"	intype="Int16_B"	inlength="1"	keyword="MR_imatrix_Y"		dicomtype="?"	dicomtag="?"		#  Image matrix size - Y
block="MRHDR"  condition="ismr"	offset="34"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_dfov"		dicomtype="?"	dicomtag="?"		#  Display field of view - X (mm)
block="MRHDR"  condition="ismr"	offset="38"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_dfov_rect"		dicomtype="?"	dicomtag="?"		#  Display field of view - Y (if different)
block="MRHDR"  condition="ismr"	offset="42"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_dim_X"		dicomtype="?"	dicomtag="?"		#  Image dimension - X
block="MRHDR"  condition="ismr"	offset="46"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_dim_Y"		dicomtype="?"	dicomtag="?"		#  Image dimension - Y
block="MRHDR"  condition="ismr"	offset="50"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_pixsize_X"		dicomtype="?"	dicomtag="?"	#  Image pixel size - X
block="MRHDR"  condition="ismr"	offset="54"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_pixsize_Y"		dicomtype="?"	dicomtag="?"	#  Image pixel size - Y
block="MRHDR"  condition="ismr"	offset="58"	intype="String"	inlength="14"	name="MR_pdid"		dicomtype="?"	dicomtag="?"		#  Pixel Data ID
block="MRHDR"  condition="ismr"	offset="72"	intype="String"	inlength="17"	name="MR_contrastIV"		dicomtype="?"	dicomtag="?"		#  IV Contrast Agent
block="MRHDR"  condition="ismr"	offset="89"	intype="String"	inlength="17"	name="MR_contrastOral"	dicomtype="?"	dicomtag="?"		#  Oral Contrast Agent
block="MRHDR"  condition="ismr"	offset="106"	intype="Int16_B"	inlength="1"	name="MR_contmode"		dicomtype="?"	dicomtag="?"	bitmap="0:none,oral;1:none,intravenous"	#  Image Contrast Mode
block="MRHDR"  condition="ismr"	offset="108"	intype="Int16_B"	inlength="1"	name="MR_serrx"		dicomtype="?"	dicomtag="?"		#  Series from which prescribed
block="MRHDR"  condition="ismr"	offset="110"	intype="Int16_B"	inlength="1"	name="MR_imgrx"		dicomtype="?"	dicomtag="?"		#  Image from which prescribed
block="MRHDR"  condition="ismr"	offset="112"	intype="Int16_B"	inlength="1"	name="MR_screenformat"	dicomtype="?"	dicomtag="?"		#  Screen Format(8/16 bit)
block="MRHDR"  condition="ismr"	offset="114"	intype="Int16_B"	inlength="1"	name="MR_plane"		dicomtype="?"	dicomtag="?"	bitmap="0:none,Scout Plane(SCT);1:none,Axial Plane(Ax);2:none,Sagittal Plane(Sag);3:none,Coronal Plane(Cor);4:none,Oblique Plane(O);5:none,ParAxial Plane(PAX);6:none,Reformatted Plane(RFMT);7:none,Projected Plane(PJN);8:none,Mixed Plane(MIXED)"	#  Plane Type
block="MRHDR"  condition="ismr"	offset="116"	intype="IEEE_Float32_B"	inlength="1"	name="MR_scanspacing"	dicomtype="DS"	dicomtag="SpacingBetweenSlices"	#  Spacing between scans (mm?)
block="MRHDR"  condition="ismr"	offset="120"	intype="Int16_B"	inlength="1"	name="MR_im_compress"	dicomtype="?"	dicomtag="?"		#  Image compression type for allocation
block="MRHDR"  condition="ismr"	offset="122"	intype="Int16_B"	inlength="1"	name="MR_im_scouttype"	dicomtype="?"	dicomtag="?"		#  Scout Type (AP or lateral)
block="MRHDR"  condition="ismr"	offset="124"	intype="String"	inlength="1"	name="MR_loc_ras"		dicomtype="?"	dicomtag="?"		#  RAS letter of image location
block="MRHDR"  condition="ismr"	offset="126"	intype="IEEE_Float32_B"	inlength="1"	name="MR_loc"		dicomtype="DS"	dicomtag="SliceLocation"	#  Image location
block="MRHDR"  condition="ismr"	offset="130"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_ctr_R"		dicomtype="?"	dicomtag="?"		#  Center R coord of plane image
block="MRHDR"  condition="ismr"	offset="134"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_ctr_A"		dicomtype="?"	dicomtag="?"		#  Center A coord of plane image
block="MRHDR"  condition="ismr"	offset="138"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_ctr_S"		dicomtype="?"	dicomtag="?"		#  Center S coord of plane image
block="MRHDR"  condition="ismr"	offset="142"	intype="IEEE_Float32_B"	inlength="1"	name="MR_norm_R"		dicomtype="?"	dicomtag="?"		#  Normal R coord
block="MRHDR"  condition="ismr"	offset="146"	intype="IEEE_Float32_B"	inlength="1"	name="MR_norm_A"		dicomtype="?"	dicomtag="?"		#  Normal A coord
block="MRHDR"  condition="ismr"	offset="150"	intype="IEEE_Float32_B"	inlength="1"	name="MR_norm_S"		dicomtype="?"	dicomtag="?"		#  Normal S coord
block="MRHDR"  condition="ismr"	offset="154"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_tlhc_R"		dicomtype="?"	dicomtag="?"		#  R Coord of Top Left Hand Corner
block="MRHDR"  condition="ismr"	offset="158"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_tlhc_A"		dicomtype="?"	dicomtag="?"		#  A Coord of Top Left Hand Corner
block="MRHDR"  condition="ismr"	offset="162"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_tlhc_S"		dicomtype="?"	dicomtag="?"		#  S Coord of Top Left Hand Corner
block="MRHDR"  condition="ismr"	offset="166"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_trhc_R"		dicomtype="?"	dicomtag="?"		#  R Coord of Top Right Hand Corner
block="MRHDR"  condition="ismr"	offset="170"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_trhc_A"		dicomtype="?"	dicomtag="?"		#  A Coord of Top Right Hand Corner
block="MRHDR"  condition="ismr"	offset="174"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_trhc_S"		dicomtype="?"	dicomtag="?"		#  S Coord of Top Right Hand Corner
block="MRHDR"  condition="ismr"	offset="178"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_brhc_R"		dicomtype="?"	dicomtag="?"		#  R Coord of Bottom Right Hand Corner
block="MRHDR"  condition="ismr"	offset="182"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_brhc_A"		dicomtype="?"	dicomtag="?"		#  A Coord of Bottom Right Hand Corner
block="MRHDR"  condition="ismr"	offset="186"	intype="IEEE_Float32_B"	inlength="1"	keyword="MR_brhc_S"		dicomtype="?"	dicomtag="?"		#  S Coord of Bottom Right Hand Corner
block="MRHDR"  condition="ismr"	offset="190"	intype="String"	inlength="4"	name="MR_forimgrev"		dicomtype="?"	dicomtag="?"		#  Foreign Image Revision
block="MRHDR"  condition="ismr"	offset="194"	intype="Int32_B"	inlength="1"	keyword="MR_tr"			dicomtype="?"	dicomtag="?"		#  Pulse repetition time(usec)
block="MRHDR"  condition="ismr"	offset="198"	intype="Int32_B"	inlength="1"	keyword="MR_ti"			dicomtype="?"	dicomtag="?"		#  Pulse inversion time(usec)
block="MRHDR"  condition="ismr"	offset="202"	intype="Int32_B"	inlength="1"	keyword="MR_te"			dicomtype="?"	dicomtag="?"		#  Pulse echo time(usec)
block="MRHDR"  condition="ismr"	offset="206"	intype="Int32_B"	inlength="1"	name="MR_te2"			dicomtype="?"	dicomtag="?"		#  Second echo echo (usec)
block="MRHDR"  condition="ismr"	offset="210"	intype="Int16_B"	inlength="1"	name="MR_numecho"		dicomtype="?"	dicomtag="?"	#  Number of echoes
block="MRHDR"  condition="ismr"	offset="212"	intype="Int16_B"	inlength="1"	name="MR_echonum"		dicomtype="IS"	dicomtag="EchoNumbers"	#  Echo Number
block="MRHDR"  condition="ismr"	offset="214"	intype="IEEE_Float32_B"	inlength="1"	name="MR_tbldlta"		dicomtype="?"	dicomtag="?"		#  Table Delta
block="MRHDR"  condition="ismr"	offset="218"	intype="IEEE_Float32_B"	inlength="1"	name="MR_nex"		dicomtype="DS"	dicomtag="NumberOfAverages"	#  Number of Excitations
block="MRHDR"  condition="ismr"	offset="222"	intype="Int16_B"	inlength="1"	name="MR_contig"		dicomtype="?"	dicomtag="?"	enum="0=No:/I,1=Yes:/C"	#  Continuous Slices Flag
block="MRHDR"  condition="ismr"	offset="224"	intype="Int16_B"	inlength="1"	name="MR_hrtrate"		dicomtype="IS"	dicomtag="HeartRate"	#  Cardiac Heart Rate (bpm)
block="MRHDR"  condition="ismr"	offset="226"	intype="Int32_B"	inlength="1"	name="MR_tdel"		dicomtype="?"	dicomtag="?"		#  Delay time after trigger (msec)
block="MRHDR"  condition="ismr"	offset="230"	intype="IEEE_Float32_B"	inlength="1"	name="MR_saravg"		dicomtype="DS"	dicomtag="SAR"		#  Average SAR
block="MRHDR"  condition="ismr"	offset="234"	intype="IEEE_Float32_B"	inlength="1"	name="MR_sarpeak"		dicomtype="?"	dicomtag="?"		#  Peak SAR
block="MRHDR"  condition="ismr"	offset="238"	intype="Int16_B"	inlength="1"	name="MR_monsar"		dicomtype="?"	dicomtag="?"		#  Monitor SAR flag
block="MRHDR"  condition="ismr"	offset="240"	intype="Int16_B"	inlength="1"	name="MR_trgwindow"		dicomtype="IS"	dicomtag="TriggerWindow"	#  Trigger window (% of R-R interval)
block="MRHDR"  condition="ismr"	offset="242"	intype="IEEE_Float32_B"	inlength="1"	name="MR_reptime"		dicomtype="?"	dicomtag="?"		#  Cardiac repetition time
block="MRHDR"  condition="ismr"	offset="246"	intype="Int16_B"	inlength="1"	name="MR_imgpcyc"		dicomtype="IS"	dicomtag="CardiacNumberOfImages"	#  Images per cardiac cycle
block="MRHDR"  condition="ismr"	offset="248"	intype="Int16_B"	inlength="1"	name="MR_xmtgain"		dicomtype="?"	dicomtag="?"		#  Actual Transmit Gain (.1 db)
block="MRHDR"  condition="ismr"	offset="250"	intype="Int16_B"	inlength="1"	name="MR_rcvgain1"		dicomtype="?"	dicomtag="?"		#  Actual Receive Gain Analog (.1 db)
block="MRHDR"  condition="ismr"	offset="252"	intype="Int16_B"	inlength="1"	name="MR_rcvgain2"		dicomtype="?"	dicomtag="?"		#  Actual Receive Gain Digital (.1 db)
block="MRHDR"  condition="ismr"	offset="254"	intype="Int16_B"	inlength="1"	name="MR_mr_flip"		dicomtype="DS"	dicomtag="FlipAngle"	#  Flip Angle for GRASS scans (deg.)
block="MRHDR"  condition="ismr"	offset="256"	intype="Int32_B"	inlength="1"	name="MR_mindat"		dicomtype="?"	dicomtag="?"		#  Minimum Delay after Trigger (uSec)
block="MRHDR"  condition="ismr"	offset="260"	intype="Int16_B"	inlength="1"	name="MR_cphase"		dicomtype="?"	dicomtag="?"		#  Total Cardiac Phase prescribed
block="MRHDR"  condition="ismr"	offset="262"	intype="Int16_B"	inlength="1"	keyword="MR_swappf"		dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes:SPF"	#  Swap Phase/Frequency Axis
block="MRHDR"  condition="ismr"	offset="264"	intype="Int16_B"	inlength="1"	name="MR_pauseint"		dicomtype="?"	dicomtag="?"		#  Pause Interval (slices)
block="MRHDR"  condition="ismr"	offset="266"	intype="IEEE_Float32_B"	inlength="1"	name="MR_pausetime"		dicomtype="?"	dicomtag="?"		#  Pause Time
block="MRHDR"  condition="ismr"	offset="270"	intype="Int32_B"	inlength="1"	name="MR_obplane"		dicomtype="?"	dicomtag="?"		#  Oblique Plane
block="MRHDR"  condition="ismr"	offset="274"	intype="Int32_B"	inlength="1"	name="MR_slocfov"		dicomtype="?"	dicomtag="?"		#  Slice Offsets on Freq axis
block="MRHDR"  condition="ismr"	offset="278"	intype="Int32_B"	inlength="1"	keyword="MR_xmtfreq"		dicomtype="?"	dicomtag="?"		#  Center Frequency (0.1 Hz)
block="MRHDR"  condition="ismr"	offset="282"	intype="Int32_B"	inlength="1"	name="MR_autoxmtfreq"	dicomtype="?"	dicomtag="?"		#  Auto Center Frequency (0.1 Hz)
block="MRHDR"  condition="ismr"	offset="286"	intype="Int16_B"	inlength="1"	name="MR_autoxmtgain"	dicomtype="?"	dicomtag="?"		#  Auto Transmit Gain (0.1 dB)
block="MRHDR"  condition="ismr"	offset="288"	intype="Int16_B"	inlength="1"	name="MR_prescan_r1"		dicomtype="?"	dicomtag="?"		#  PreScan R1 - Analog
block="MRHDR"  condition="ismr"	offset="290"	intype="Int16_B"	inlength="1"	name="MR_prescan_r2"		dicomtype="?"	dicomtag="?"		#  PreScan R2 - Digital
block="MRHDR"  condition="ismr"	offset="292"	intype="Int32_B"	inlength="1"	name="MR_user_bitmap"	dicomtype="?"	dicomtag="?"		#  Bitmap defining user CVs
block="MRHDR"  condition="ismr"	offset="296"	intype="Int16_B"	inlength="1"	name="MR_cenfreq"		dicomtype="?"	dicomtag="?"		#  Center Frequency Method
block="MRHDR"  condition="ismr"	offset="298"	intype="Int16_B"	inlength="1"	keyword="MR_imode"		dicomtype="?"	dicomtag="?"	enum="1=Two D:2D,2=Three D Volume:3D,3=Three D Fourier:none,4=Cine:Cine,5=Angiography:ANGIO,6=Spectroscopy:SPECT,7=Flouroscopy:FLOURO,8=Research Mode:RM"	#  Imaging Mode
block="MRHDR"  condition="ismr"	offset="300"	intype="Int32_B"	inlength="1"	keyword="MR_iopt"		dicomtype="?"	dicomtag="?"	bitmap="0:none,EG;1:none,RESP;2:none,RC;3:none,FC;4:none,CL;5:none,ST;6:none,PG;7:none,NP;8:none,NF;9:none,RT;10:none,VB;11:none,ED;12:none,PM;13:none,SQ;14:none,CS;15:none,MP;16:none,SQPIX"	#  Imaging Options
block="MRHDR"  condition="ismr"	offset="304"	intype="Int16_B"	inlength="1"	keyword="MR_pseq"		dicomtype="?"	dicomtag="?"	enum="0=SE,1=IR,2=RM:RM,3=RMGE:none,4=GRE:GR,5=MPGR,6=MPIRS:IR/s,7=MPIRI:IR,8=VOGRE:3D/GR,9=CINEGRE:Cine/GRE,10=SPGR,11=SSFP,12=TF:TOF,13=PC,14=CINSPGR:Cine/SPGR,15=TOFGR:TOG/GR,16=TOFSPGR:TOF/SPGR,17=PCGR:PC/GR,18=PCSPGR:PC/SPGR,19=FSE,20=FGR,21=FMPGR,22=FSPGR,23=FMPSPGR,24=SPECT,25=PSEQ_MIXED:MIXED,26=FMPIR,27=SPECSTEAM,28=SPECPRESS"	#  Pulse Sequence
block="MRHDR"  condition="ismr"	offset="306"	intype="Int16_B"	inlength="1"	name="MR_pseqmode"		dicomtype="?"	dicomtag="?"	enum="1=Product,2=Research Mode,3=Research Mode GE"	#  Pulse Sequence Mode
block="MRHDR"  condition="ismr"	offset="308"	intype="String"	inlength="33"	name="MR_psdname"		dicomtype="SH"	dicomtag="SequenceName"		#  Pulse Sequence Name
block="MRHDR"  condition="ismr"	offset="342"	intype="Unix_DateTime_B"	inlength="1"	name="MR_psd_datetime"		dicomtype="?"	dicomtag="?"		#  PSD Creation Date and Time
block="MRHDR"  condition="ismr"	offset="346"	intype="String"	inlength="13"	name="MR_psd_iname"		dicomtype="?"	dicomtag="?"		#  PSD name from inside PSD
block="MRHDR"  condition="ismr"	offset="360"	intype="Int16_B"	inlength="1"	name="MR_ctyp"			dicomtype="?"	dicomtag="?"	enum="1=head,2=body,3=surface"	#  Coil Type
block="MRHDR"  condition="ismr"	offset="362"	intype="String"	inlength="17"	name="MR_cname"			dicomtype="SH"	dicomtag="ReceiveCoilName"	#  Coil Name
block="MRHDR"  condition="ismr"	offset="380"	intype="Int16_B"	inlength="1"	name="MR_surfctyp"		dicomtype="?"	dicomtag="?"		#  Surface Coil Type
block="MRHDR"  condition="ismr"	offset="382"	intype="Int16_B"	inlength="1"	name="MR_surfcext"		dicomtype="?"	dicomtag="?"		#  Extremity Coil Flag
block="MRHDR"  condition="ismr"	offset="384"	intype="Int32_B"	inlength="1"	name="MR_rawrunnum"		dicomtype="IS"	dicomtag="AcquisitionNumber"		#  RawData Run Number
block="MRHDR"  condition="ismr"	offset="388"	intype="Uint32_B"	inlength="1"	name="MR_cal_fldstr"		dicomtype="?"	dicomtag="?"		#  Calibrated Field Strength (x10 uGauss)
block="MRHDR"  condition="ismr"	offset="392"	intype="Int16_B"	inlength="1"	keyword="MR_supp_tech"		dicomtype="?"	dicomtag="?"	enum="0=none,1=fat:F,2=water:W"	#  SAT fat/water/none
block="MRHDR"  condition="ismr"	offset="394"	intype="IEEE_Float32_B"	inlength="1"	name="MR_vbw"			dicomtype="?"	dicomtag="?"		#  Variable Bandwidth (Hz)
block="MRHDR"  condition="ismr"	offset="398"	intype="Int16_B"	inlength="1"	name="MR_slquant"		dicomtype="?"	dicomtag="?"		#  Number of slices in this scan group
block="MRHDR"  condition="ismr"	offset="400"	intype="Int16_B"	inlength="1"	name="MR_gpre"			dicomtype="?"	dicomtag="?"	enum="0=No GRX,1=GRX 1 localizer,2=GRX 2 localizers"	#  Graphically prescribed
block="MRHDR"  condition="ismr"	offset="402"	intype="Int32_B"	inlength="1"	name="MR_intr_del"		dicomtype="?"	dicomtag="?"		#  Interimage/interloc delay (uSec)
block="MRHDR"  condition="ismr"	offset="406"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user0"		dicomtype="?"	dicomtag="?"		#  User Variable 0
block="MRHDR"  condition="ismr"	offset="410"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user1"		dicomtype="?"	dicomtag="?"		#  User Variable 1
block="MRHDR"  condition="ismr"	offset="414"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user2"		dicomtype="?"	dicomtag="?"		#  User Variable 2
block="MRHDR"  condition="ismr"	offset="418"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user3"		dicomtype="?"	dicomtag="?"		#  User Variable 3
block="MRHDR"  condition="ismr"	offset="422"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user4"		dicomtype="?"	dicomtag="?"		#  User Variable 4
block="MRHDR"  condition="ismr"	offset="426"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user5"		dicomtype="?"	dicomtag="?"		#  User Variable 5
block="MRHDR"  condition="ismr"	offset="430"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user6"		dicomtype="?"	dicomtag="?"		#  User Variable 6
block="MRHDR"  condition="ismr"	offset="434"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user7"		dicomtype="?"	dicomtag="?"		#  User Variable 7
block="MRHDR"  condition="ismr"	offset="438"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user8"		dicomtype="?"	dicomtag="?"		#  User Variable 8
block="MRHDR"  condition="ismr"	offset="442"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user9"		dicomtype="?"	dicomtag="?"		#  User Variable 9
block="MRHDR"  condition="ismr"	offset="446"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user10"		dicomtype="?"	dicomtag="?"		#  User Variable 10
block="MRHDR"  condition="ismr"	offset="450"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user11"		dicomtype="?"	dicomtag="?"		#  User Variable 11
block="MRHDR"  condition="ismr"	offset="454"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user12"		dicomtype="?"	dicomtag="?"		#  User Variable 12
block="MRHDR"  condition="ismr"	offset="458"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user13"		dicomtype="?"	dicomtag="?"		#  User Variable 13
block="MRHDR"  condition="ismr"	offset="462"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user14"		dicomtype="?"	dicomtag="?"		#  User Variable 14
block="MRHDR"  condition="ismr"	offset="466"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user15"		dicomtype="?"	dicomtag="?"		#  User Variable 15
block="MRHDR"  condition="ismr"	offset="470"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user16"		dicomtype="?"	dicomtag="?"		#  User Variable 16
block="MRHDR"  condition="ismr"	offset="474"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user17"		dicomtype="?"	dicomtag="?"		#  User Variable 17
block="MRHDR"  condition="ismr"	offset="478"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user18"		dicomtype="?"	dicomtag="?"		#  User Variable 18
block="MRHDR"  condition="ismr"	offset="482"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user19"		dicomtype="?"	dicomtag="?"		#  User Variable 19
block="MRHDR"  condition="ismr"	offset="486"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user20"		dicomtype="?"	dicomtag="?"		#  User Variable 20
block="MRHDR"  condition="ismr"	offset="490"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user21"		dicomtype="?"	dicomtag="?"		#  User Variable 21
block="MRHDR"  condition="ismr"	offset="494"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user22"		dicomtype="?"	dicomtag="?"		#  User Variable 22
block="MRHDR"  condition="ismr"	offset="498"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user23"		dicomtype="?"	dicomtag="?"		#  Projection Angle
block="MRHDR"  condition="ismr"	offset="502"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user24"		dicomtype="?"	dicomtag="?"		#  Concat Sat Type Flag
block="MRHDR"  condition="ismr"	offset="506"	intype="String"	inlength="13"	name="MR_im_alloc_key"	dicomtype="?"	dicomtag="?"			#  Process that allocated this record
block="MRHDR"  condition="ismr"	offset="520"	intype="Unix_DateTime_B"	inlength="1"	keyword="MR_im_lastmod"		dicomtype="?"	dicomtag="?"		#  Date/Time of Last Change
block="MRHDR"  condition="ismr"	offset="524"	intype="String"	inlength="2"	name="MR_im_verscre"		dicomtype="?"	dicomtag="?"		#  Genesis version - Created
block="MRHDR"  condition="ismr"	offset="526"	intype="String"	inlength="2"	name="MR_im_verscur"		dicomtype="?"	dicomtag="?"		#  Genesis version - Now
block="MRHDR"  condition="ismr"	offset="528"	intype="Int32_B"	inlength="1"	name="MR_im_pds_a"		dicomtype="?"	dicomtag="?"		#  PixelData size - as stored
block="MRHDR"  condition="ismr"	offset="532"	intype="Int32_B"	inlength="1"	name="MR_im_pds_c"		dicomtype="?"	dicomtag="?"		#  PixelData size -Compressed
block="MRHDR"  condition="ismr"	offset="536"	intype="Int32_B"	inlength="1"	name="MR_im_pds_u"		dicomtype="?"	dicomtag="?"		#  PixelData size -UnCompressed
block="MRHDR"  condition="ismr"	offset="540"	intype="Uint32_B"	inlength="1"	name="MR_im_checksum"		dicomtype="?"	dicomtag="?"		#  AcqRecon record checksum
block="MRHDR"  condition="ismr"	offset="544"	intype="Int32_B"	inlength="1"	name="MR_im_archived"		dicomtype="?"	dicomtag="?"		#  Image Archive Flag
block="MRHDR"  condition="ismr"	offset="548"	intype="Int32_B"	inlength="1"	name="MR_im_complete"		dicomtype="?"	dicomtag="?"		#  Image Complete Flag
block="MRHDR"  condition="ismr"	offset="552"	intype="Int16_B"	inlength="1"	keyword="MR_satbits"		dicomtype="?"	dicomtag="?"	bitmap="0:none,superior(S);1:none,inferior(I);2:none,right(R);3:none,left(R);4:none,anterior(A);5:none,posterior(P);6:none,superior(s);7:none,inferior(i);8:none,right(r);9:none,left(l);10:none,anterior(a);11:none,posterior(p)"	#  Bitmap of SAT selections
block="MRHDR"  condition="ismr"	offset="554"	intype="Int16_B"	inlength="1"	keyword="MR_scic"		dicomtype="?"	dicomtag="?"	enum="0=Off,1=On:IIC"	#  Surface Coil Intensity Correction Flag
block="MRHDR"  condition="ismr"	offset="556"	intype="Int16_B"	inlength="1"	name="MR_satxloc1"		dicomtype="?"	dicomtag="?"		#  R-side SAT pulse loc rel to lndmrk
block="MRHDR"  condition="ismr"	offset="558"	intype="Int16_B"	inlength="1"	name="MR_satxloc2"		dicomtype="?"	dicomtag="?"		#  L-side SAT pulse loc rel to lndmrk
block="MRHDR"  condition="ismr"	offset="560"	intype="Int16_B"	inlength="1"	name="MR_satyloc1"		dicomtype="?"	dicomtag="?"		#  A-side SAT pulse loc rel to lndmrk
block="MRHDR"  condition="ismr"	offset="562"	intype="Int16_B"	inlength="1"	name="MR_satyloc2"		dicomtype="?"	dicomtag="?"		#  P-side SAT pulse loc rel to lndmrk
block="MRHDR"  condition="ismr"	offset="564"	intype="Int16_B"	inlength="1"	name="MR_satzloc1"		dicomtype="?"	dicomtag="?"		#  S-side SAT pulse loc rel to lndmrk
block="MRHDR"  condition="ismr"	offset="566"	intype="Int16_B"	inlength="1"	name="MR_satzloc2"		dicomtype="?"	dicomtag="?"		#  I-side SAT pulse loc rel to lndmrk
block="MRHDR"  condition="ismr"	offset="568"	intype="Int16_B"	inlength="1"	name="MR_satxthick"		dicomtype="?"	dicomtag="?"		#  Thickness of X-axis SAT pulse
block="MRHDR"  condition="ismr"	offset="570"	intype="Int16_B"	inlength="1"	name="MR_satythick"		dicomtype="?"	dicomtag="?"		#  Thickness of Y-axis SAT pulse
block="MRHDR"  condition="ismr"	offset="572"	intype="Int16_B"	inlength="1"	name="MR_satzthick"		dicomtype="?"	dicomtag="?"		#  Thickness of Z-axis SAT pulse
block="MRHDR"  condition="ismr"	offset="574"	intype="Int16_B"	inlength="1"	keyword="MR_flax"		dicomtype="?"	dicomtag="?"	bitmap="0:none,S/I;1:none,A/P;2:none,R/L;3:none,SLC"	#  Phase contrast flow axis
block="MRHDR"  condition="ismr"	offset="576"	intype="Int16_B"	inlength="1"	name="MR_venc"		dicomtype="?"	dicomtag="?"		#  Phase contrast velocity encoding
block="MRHDR"  condition="ismr"	offset="578"	intype="Int16_B"	inlength="1"	name="MR_thk_disclmr"	dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes"	#  Slice Thickness Disclaimer
block="MRHDR"  condition="ismr"	offset="580"	intype="Int16_B"	inlength="1"	name="MR_ps_flag"		dicomtype="?"	dicomtag="?"	bitmap="0:none,AF(a);1:none,AS(A);2:none,M(M)"	#  Auto/Manual Prescan flag
block="MRHDR"  condition="ismr"	offset="582"	intype="Int16_B"	inlength="1"	name="MR_ps_status"		dicomtype="?"	dicomtag="?"	bitmap="0:none,CF;1:none,TA;2:none,R1;3:none,R2"	#  Bitmap of changed values
block="MRHDR"  condition="ismr"	offset="584"	intype="Int16_B"	inlength="1"	keyword="MR_image_type"		dicomtype="?"	dicomtag="?"	enum="0=Magnitude,1=Phase:PHASE,2=Real:REAL,3=Imaginary:IMAGINARY,4=Spectroscopy:SPECTRO"	#  Magnitude, Phase, Imaginary,Real,Spectroscopy
block="MRHDR"  condition="ismr"	offset="586"	intype="Int16_B"	inlength="1"	keyword="MR_vas_collapse"	dicomtype="?"	dicomtag="?"	enum="0=Off,1=:COL,2=:MAG,3=:R/L,4=:A/P,5=:S/I,6=:PJN,7=:ALL"	#  Collapse Image
block="MRHDR"  condition="ismr"	offset="588"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user23n"		dicomtype="?"	dicomtag="?"		#  User Variable 23
block="MRHDR"  condition="ismr"	offset="592"	intype="IEEE_Float32_B"	inlength="1"	name="MR_user24n"		dicomtype="?"	dicomtag="?"		#  User Variable 24
block="MRHDR"  condition="ismr"	offset="596"	intype="Int16_B"	inlength="1"	keyword="MR_proj_alg"	dicomtype="?"	dicomtag="?"	enum="0=None,1=Prototype,2=Minimum Pixel:Min,3=Maximum Pixel:Max"	#  Projection Algorithm
block="MRHDR"  condition="ismr"	offset="598"	intype="String"	inlength="13"	name="MR_proj_name"		dicomtype="?"	dicomtag="?"		#  Projection Algorithm Name
block="MRHDR"  condition="ismr"	offset="612"	intype="IEEE_Float32_B"	inlength="1"	name="MR_x_axis_rot"		dicomtype="?"	dicomtag="?"		#  X Axis Rotation
block="MRHDR"  condition="ismr"	offset="616"	intype="IEEE_Float32_B"	inlength="1"	name="MR_y_axis_rot"		dicomtype="?"	dicomtag="?"		#  Y Axis Rotation
block="MRHDR"  condition="ismr"	offset="620"	intype="IEEE_Float32_B"	inlength="1"	name="MR_z_axis_rot"		dicomtype="?"	dicomtag="?"		#  Z Axis Rotation
block="MRHDR"  condition="ismr"	offset="624"	intype="Int32_B"	inlength="1"	name="MR_thresh_min1"	dicomtype="?"	dicomtag="?"		#  Lower Range of Pixels 1
block="MRHDR"  condition="ismr"	offset="628"	intype="Int32_B"	inlength="1"	name="MR_thresh_max1"	dicomtype="?"	dicomtag="?"		#  Upper Range of Pixels 1
block="MRHDR"  condition="ismr"	offset="632"	intype="Int32_B"	inlength="1"	name="MR_thresh_min2"	dicomtype="?"	dicomtag="?"		#  Lower Range of Pixels 2
block="MRHDR"  condition="ismr"	offset="636"	intype="Int32_B"	inlength="1"	name="MR_thresh_max2"	dicomtype="?"	dicomtag="?"		#  Upper Range of Pixels 2
block="MRHDR"  condition="ismr"	offset="640"	intype="Int16_B"	inlength="1"	keyword="MR_echo_trn_len"	dicomtype="?"	dicomtag="?"		#  Echo Train Length for Fast Spin Echo
block="MRHDR"  condition="ismr"	offset="642"	intype="Int16_B"	inlength="1"	keyword="MR_frac_echo"		dicomtype="?"	dicomtag="?"	bitmap="0:none,Fractional(Fr);1:none,Effective(Ef)"	#  Effective TE Flag
block="MRHDR"  condition="ismr"	offset="644"	intype="Int16_B"	inlength="1"	keyword="MR_prep_pulse"		dicomtype="?"	dicomtag="?"	enum="0=None,1=Inversion Recovery:IR,2=Driven Equilibrium:DE"	#  Preparatory Pulse Option
block="MRHDR"  condition="ismr"	offset="646"	intype="Int16_B"	inlength="1"	name="MR_cphasenum"		dicomtype="?"	dicomtag="?"		#  Cardiac Phase Number
block="MRHDR"  condition="ismr"	offset="648"	intype="Int16_B"	inlength="1"	keyword="MR_var_echo"		dicomtype="?"	dicomtag="?"	enum="0=No,1=Yes:/V"	#  Variable Echo Flag
block="MRHDR"  condition="ismr"	offset="650"	intype="String"	inlength="1"	name="MR_ref_img"		dicomtype="?"	dicomtag="?"		#  Reference Image Field
block="MRHDR"  condition="ismr"	offset="651"	intype="String"	inlength="1"	name="MR_sum_img"		dicomtype="?"	dicomtag="?"		#  Summary Image Field
block="MRHDR"  condition="ismr"	offset="652"	intype="Uint16_B"	inlength="1"	name="MR_img_window"		dicomtype="?"	dicomtag="?"		#  Window Value
block="MRHDR"  condition="ismr"	offset="654"	intype="Int16_B"	inlength="1"	name="MR_img_level"		dicomtype="?"	dicomtag="?"		#  Level Value
block="MRHDR"  condition="ismr"	offset="656"	intype="Int32_B"	inlength="1"	name="MR_slop_int_1"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 1
block="MRHDR"  condition="ismr"	offset="660"	intype="Int32_B"	inlength="1"	name="MR_slop_int_2"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 2
block="MRHDR"  condition="ismr"	offset="664"	intype="Int32_B"	inlength="1"	name="MR_slop_int_3"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 3
block="MRHDR"  condition="ismr"	offset="668"	intype="Int32_B"	inlength="1"	name="MR_slop_int_4"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 4
block="MRHDR"  condition="ismr"	offset="672"	intype="Int32_B"	inlength="1"	name="MR_slop_int_5"		dicomtype="?"	dicomtag="?"		#  Integer Slop Field 5
block="MRHDR"  condition="ismr"	offset="676"	intype="IEEE_Float32_B"	inlength="1"	name="MR_slop_float_1"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 1
block="MRHDR"  condition="ismr"	offset="680"	intype="IEEE_Float32_B"	inlength="1"	name="MR_slop_float_2"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 2
block="MRHDR"  condition="ismr"	offset="684"	intype="IEEE_Float32_B"	inlength="1"	name="MR_slop_float_3"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 3
block="MRHDR"  condition="ismr"	offset="688"	intype="IEEE_Float32_B"	inlength="1"	name="MR_slop_float_4"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 4
block="MRHDR"  condition="ismr"	offset="692"	intype="IEEE_Float32_B"	inlength="1"	name="MR_slop_float_5"	dicomtype="?"	dicomtag="?"		#  Float Slop Field 5
block="MRHDR"  condition="ismr"	offset="696"	intype="String"		inlength="16"	name="MR_slop_str_1"		dicomtype="?"	dicomtag="?"		#  String Slop Field 1
block="MRHDR"  condition="ismr"	offset="712"	intype="String"		inlength="16"	name="MR_slop_str_2"		dicomtype="?"	dicomtag="?"		#  String Slop Field 2
block="MRHDR"  condition="ismr"	offset="728"	intype="Int16_B"	inlength="1"	name="MR_scanactno"		dicomtype="?"	dicomtag="?"		#  Scan Acquisition Number
block="MRHDR"  condition="ismr"	offset="730"	intype="Int16_B"	inlength="1"	name="MR_vasflags"		dicomtype="?"	dicomtag="?"	bitmap="0:none,NOFLAGS;1:none,MAGWEIGHT;2:none,CD RECON;3:none,PD RECON;4:none,PHASECOR OFF"	#  Magnitude Weighting Flag
block="MRHDR"  condition="ismr"	offset="732"	intype="IEEE_Float32_B"	inlength="1"	name="MR_vencscale"		dicomtype="?"	dicomtag="?"		#  Scale Weighted Venc
block="MRHDR"  condition="ismr"	offset="736"	intype="Int16_B"	inlength="1"	name="MR_integrity"		dicomtype="?"	dicomtag="?"	enum="0=GE Image,1=Imported Image"	#  GE Image Integrity
block="MRHDR"  condition="ismr"	offset="738"	intype="Int32_B"	inlength="1"	name="MR_fphase"		dicomtype="?"	dicomtag="?"		#  Number Of Phases
block="MRHDR"  condition="ismr"	offset="742"	intype="Int16_B"	inlength="1"	name="MR_freq_dir"		dicomtype="?"	dicomtag="?"	enum="0=Unknown,1=Row,2=Col"	#  Frequency Direction
block="MRHDR"  condition="ismr"	offset="744"	intype="Int16_B"	inlength="1"	name="MR_vas_mode"		dicomtype="?"	dicomtag="?"	num="0=None,1=TOF,2=PC"	#  Vascular Mode
block="MRHDR"  condition="ismr"	offset="746"	intype="String"		inlength="32"	name="MR_image_uid"		dicomtype="?"	dicomtag="?"		#  Image Unique ID
block="MRHDR"  condition="ismr"	offset="778"	intype="String"		inlength="32"	name="MR_sop_uid"		dicomtype="?"	dicomtag="?"		#  Service Obj Class Unique ID
block="MRHDR"  condition="ismr"	offset="810"	intype="String"		inlength="212"	name="MR_mr_padding"		dicomtype="?"	dicomtag="?"		#  Spare Space

constant="16" 							dicomtype="US"	dicomtag="BitsAllocated"		#
constant="1" 							dicomtype="US"	dicomtag="PixelRepresentation"		# Signed
constant="1" 							dicomtype="US"	dicomtag="SamplesPerPixel"		# 
constant="MONOCHROME2" 						dicomtype="CS"	dicomtag="PhotometricInterpretation"	# 
constant="1" 							dicomtype="DS"	dicomtag="RescaleSlope"			# 
constant="US" 				condition="ismr"	dicomtype="LO"	dicomtag="RescaleType"			# 
constant="HU" 				condition="isct"	dicomtype="LO"	dicomtag="RescaleType"			# 
constant="0" 							dicomtype="XS"	dicomtag="PixelPaddingValue"		# 
constant=""							dicomtype="SH"	dicomtag="AccessionNumber"		# 
constant=""							dicomtype="DA"	dicomtag="PatientBirthDate"		# 
constant="GE Medical Systems"					dicomtype="LO"	dicomtag="Manufacturer"			# 
constant="1.2.840.10008.5.1.4.1.1.4"	condition="ismr"	dicomtype="UI"	dicomtag="SOPClassUID"			# 
constant="1.2.840.10008.5.1.4.1.1.2"	condition="isct"	dicomtype="UI"	dicomtag="SOPClassUID"			# 

