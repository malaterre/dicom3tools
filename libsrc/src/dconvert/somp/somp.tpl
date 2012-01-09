# Offsets are in bytes from 0
# Lengths are in intype units

block="BINHDR"  offset="0"    intype="Int16_L"         inlength="1"    name="unknown0"       dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="2"    intype="Int16_L"         inlength="1"    name="unknown2"       dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="4"    intype="Int16_L"         inlength="1"    keyword="blksize"     dicomtype="?"   dicomtag="?"    enum="512=512 bytes"	# Block size
block="BINHDR"  offset="6"    intype="Int16_L"         inlength="1"    keyword="blkstotal"   dicomtype="?"   dicomtag="?"    # Total blocks in file
block="BINHDR"  offset="8"    intype="Int16_L"         inlength="1"    name="unknown8"       dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="10"   intype="Int16_L"         inlength="1"    name="unknown10"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="12"   intype="Int16_L"         inlength="1"    name="unknown12"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="14"   intype="Int16_L"         inlength="1"    name="unknown14"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="16"   intype="Int16_L"         inlength="1"    name="unknown16"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="18"   intype="Int16_L"         inlength="1"    name="unknown18"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="20"   intype="Int16_L"         inlength="1"    name="unknown20"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="22"   intype="Int16_L"         inlength="1"    name="unknown22"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="24"   intype="Int16_L"         inlength="1"    name="unknown24"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="26"   intype="Int16_L"         inlength="1"    name="unknown26"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="28"   intype="Int16_L"         inlength="1"    name="unknown28"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="30"   intype="Int16_L"         inlength="1"    name="unknown30"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="32"   intype="Int16_L"         inlength="1"    name="unknown32"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="34"   intype="Int16_L"         inlength="1"    name="unknown34"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="36"   intype="Int16_L"         inlength="1"    name="unknown36"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="38"   intype="Int16_L"         inlength="1"    name="unknown38"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="40"   intype="Int32_L"         inlength="1"    name="unknown40"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="44"   intype="Int16_L"         inlength="1"    name="unknown44"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="46"   intype="Int16_L"         inlength="1"    name="unknown46"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="48"   intype="Int16_L"         inlength="1"    name="unknown48"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="50"   intype="Int16_L"         inlength="1"    name="unknown50"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="52"   intype="Int16_L"         inlength="1"    name="unknown52"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="54"   intype="Int16_L"         inlength="1"    keyword="blkshdr"     dicomtype="?"   dicomtag="?"    enum="8=4096 byte header"	# Blocks in header
block="BINHDR"  offset="56"   intype="Int16_L"         inlength="1"    name="unknown56"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="58"   intype="Int16_L"         inlength="1"    name="unknown58"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="60"   intype="Int16_L"         inlength="1"    name="unknown60"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="62"   intype="Int16_L"         inlength="1"    name="unknown62"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="64"   intype="Int32_L"         inlength="1"    name="unknown64"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="68"   intype="Int32_L"         inlength="1"    name="unknown68"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="72"   intype="Int32_L"         inlength="1"    name="unknown72"      dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="76"   intype="Int16_L"         inlength="1"    keyword="ptrdata"     dicomtype="?"   dicomtag="?"    enum="9=data after 4096 byte header"	# Block offset (from 1) of image pixel data
block="BINHDR"  offset="78"   intype="Int16_L"         inlength="1"    keyword="blksdata"    dicomtype="?"   dicomtag="?"    enum="1024=length of uncompressed 512 matrix"	# Blocks in image pixel data

block="BINHDR"  offset="512"  intype="Int32_L"         inlength="1"    name="unknown512"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="516"  intype="String"          inlength="4"    keyword="version"     dicomtype="LO"  dicomtag="SoftwareVersions"    #
block="BINHDR"  offset="520"  intype="String_Date_DMY" inlength="12"   keyword="studydate"   dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="532"  intype="String_Time"     inlength="12"   keyword="studytime"   dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="544"  intype="String"          inlength="44"   name="unknown544"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="588"  intype="Int32_L"         inlength="1"    name="unknown588"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="592"  intype="Int32_L"         inlength="1"    name="unknown592"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="596"  intype="Int32_L"         inlength="1"    name="unknown596"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="600"  intype="Int32_L"         inlength="1"    name="unknown600"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="604"  intype="Int32_L"         inlength="1"    name="unknown604"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="608"  intype="Int32_L"         inlength="1"    name="unknown608"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="612"  intype="Int32_L"         inlength="1"    name="unknown612"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="616"  intype="Int32_L"         inlength="1"    name="unknown616"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="620"  intype="Int32_L"         inlength="1"    name="unknown620"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="624"  intype="Vax_Float_F"     inlength="1"    name="unknown624"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="628"  intype="Vax_Float_F"     inlength="1"    name="unknown628"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="632"  intype="Vax_Float_F"     inlength="1"    name="unknown632"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="636"  intype="Int32_L"         inlength="1"    name="unknown636"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="640"  intype="Int32_L"         inlength="1"    name="unknown640"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="644"  intype="Int32_L"         inlength="1"    name="unknown644"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="648"  intype="Int32_L"         inlength="1"    name="unknown648"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="652"  intype="Int32_L"         inlength="1"    name="unknown652"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="656"  intype="Int32_L"         inlength="1"    name="unknown656"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="660"  intype="Int32_L"         inlength="1"    name="unknown660"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="664"  intype="String"          inlength="14"   name="model"          dicomtype="LO"  dicomtag="ManufacturerModelName"    #
block="BINHDR"  offset="678"  intype="Int16_L"         inlength="1"    name="unknown678"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="680"  intype="Int32_L"         inlength="1"    name="unknown680"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="684"  intype="Int32_L"         inlength="1"    name="unknown684"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="688"  intype="Int32_L"         inlength="1"    name="unknown688"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="692"  intype="Int32_L"         inlength="1"    name="unknown692"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="696"  intype="String"          inlength="12"   name="uid1"           dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="708"  intype="String"          inlength="64"   name="uid2"           dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="772"  intype="Int32_L"         inlength="1"    name="unknown772"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="776"  intype="Int32_L"         inlength="1"    name="unknown776"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="780"  intype="String"          inlength="72"   name="unknown780"     dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="852"  intype="String"          inlength="35"   name="unknown852"     dicomtype="?"   dicomtag="?"    #

block="BINHDR"  offset="1024" intype="Int32_L"         inlength="1"    name="unknown1024"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1028" intype="Int32_L"         inlength="1"    name="unknown1028"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1032" intype="Int32_L"         inlength="1"    name="unknown1032"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1036" intype="Int32_L"         inlength="1"    name="unknown1036"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1040" intype="Int32_L"         inlength="1"    name="unknown1040"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1044" intype="Int32_L"         inlength="1"    name="rotation"       dicomtype="?"   dicomtag="?"    enum="360=full,270=quick"	# Gantry rotation (?)
block="BINHDR"  offset="1048" intype="String"          inlength="20"   keyword="description" dicomtype="LO"  dicomtag="StudyDescription"    #

block="BINHDR"  offset="1072" intype="Int32_L"         inlength="1"    keyword="kv"          dicomtype="DS"  dicomtag="KVP"    #
block="BINHDR"  offset="1076" intype="Int32_L"         inlength="1"    name="unknown1076"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1080" intype="Int32_L"         inlength="1"    keyword="mas"         dicomtype="IS"  dicomtag="Exposure"    #
block="BINHDR"  offset="1084" intype="Vax_Float_F"     inlength="1"    keyword="acqtimef"    dicomtype="?"   dicomtag="?"    # TI secs
block="BINHDR"  offset="1088" intype="Int32_L"         inlength="1"    keyword="slthick"     dicomtype="DS"  dicomtag="SliceThickness"    # mm
block="BINHDR"  offset="1092" intype="Int32_L"         inlength="1"    name="unknown1092"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1096" intype="Int32_L"         inlength="1"    name="unknown1096"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1100" intype="Int32_L"         inlength="1"    name="unknown1100"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1108" intype="Int32_L"         inlength="1"    name="unknown1108"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1112" intype="Int32_L"         inlength="1"    name="unknown1112"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1116" intype="Int32_L"         inlength="1"    name="unknown1116"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1120" intype="Int32_L"         inlength="1"    name="unknown1120"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1124" intype="Int32_L"         inlength="1"    name="unknown1124"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1128" intype="Int32_L"         inlength="1"    name="unknown1128"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1132" intype="Int32_L"         inlength="1"    name="unknown1132"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1136" intype="Int32_L"         inlength="1"    name="unknown1136"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1140" intype="Vax_Float_F"     inlength="1"    name="unknown1140"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1148" intype="Int32_L"         inlength="1"    name="unknown1148"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1152" intype="Vax_Float_F"     inlength="1"    name="unknown1152"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1156" intype="String"          inlength="4"    keyword="acqtimes"    dicomtype="?"   dicomtag="?"    # TI secs
block="BINHDR"  offset="1160" intype="Int32_L"         inlength="1"    name="unknown1160"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1164" intype="Int32_L"         inlength="1"    name="unknown1164"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1168" intype="Int32_L"         inlength="1"    name="unknown1168"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1172" intype="Int32_L"         inlength="1"    name="unknown1172"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1176" intype="Int32_L"         inlength="1"    name="unknown1176"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1180" intype="Int32_L"         inlength="1"    name="unknown1180"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1184" intype="Vax_Float_F"     inlength="1"    name="unknown1184"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1188" intype="Int32_L"         inlength="1"    name="unknown1188"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1192" intype="Int32_L"         inlength="1"    name="unknown1192"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1196" intype="Int32_L"         inlength="1"    keyword="ma"          dicomtype="IS"   dicomtag="XRayTubeCurrent"    # mA
block="BINHDR"  offset="1200" intype="Int32_L"         inlength="1"    name="unknown1200"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1204" intype="Int32_L"         inlength="1"    name="unknown1204"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1208" intype="Int32_L"         inlength="1"    name="unknown1208"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1212" intype="Int32_L"         inlength="1"    name="unknown1212"    dicomtype="?"   dicomtag="?"    #

block="BINHDR"  offset="1280" intype="Int32_L"         inlength="1"    name="unknown1280"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1284" intype="Int32_L"         inlength="1"    name="unknown1284"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1288" intype="Int32_L"         inlength="1"    name="unknown1288"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1292" intype="Int32_L"         inlength="1"    name="unknown1292"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1296" intype="Int32_L"         inlength="1"    name="unknown1296"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1300" intype="Int16_L"         inlength="1"    name="unknown1300"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1302" intype="Int16_L"         inlength="1"    name="unknown1302"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1304" intype="Vax_Float_F"     inlength="1"    name="unknown1304"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1308" intype="Vax_Float_F"     inlength="1"    name="unknown1308"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1312" intype="Vax_Float_F"     inlength="1"    name="unknown1312"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1316" intype="Vax_Float_F"     inlength="1"    name="unknown1316"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1320" intype="Vax_Float_F"     inlength="1"    name="unknown1320"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1324" intype="Vax_Float_F"     inlength="1"    name="unknown1324"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1328" intype="Int32_L"         inlength="1"    keyword="matrix"      dicomtype="?"   dicomtag="?"    # Matrix size (?)
block="BINHDR"  offset="1332" intype="Vax_Float_F"     inlength="1"    keyword="zoomfactor"  dicomtype="DS"   dicomtag="ZoomFactor"    # Zoom factor (?)
block="BINHDR"  offset="1336" intype="Vax_Float_F"     inlength="1"    name="unknown1336"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1340" intype="Vax_Float_F"     inlength="1"    name="unknown1340"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1344" intype="Int32_L"         inlength="1"    name="unknown1344"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1348" intype="Int32_L"         inlength="1"    name="unknown1348"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1352" intype="Int32_L"         inlength="1"    name="unknown1352"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1356" intype="Int32_L"         inlength="1"    name="unknown1356"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1360" intype="Int32_L"         inlength="1"    name="unknown1360"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1364" intype="Int32_L"         inlength="1"    name="unknown1364"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1368" intype="Vax_Float_F"     inlength="1"    name="unknown1368"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1372" intype="String"          inlength="4"    name="unknown1372"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1376" intype="Int32_L"         inlength="1"    name="unknown1376"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1380" intype="Int8"            inlength="1"    name="unknown1380"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1381" intype="String"          inlength="3"    name="unknown1381"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1384" intype="Vax_Float_F"     inlength="1"    name="unknown1384"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1388" intype="Int32_L"         inlength="1"    name="unknown1388"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1392" intype="Int32_L"         inlength="1"    name="unknown1392"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1396" intype="Int32_L"         inlength="1"    name="unknown1396"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1400" intype="Int32_L"         inlength="1"    name="unknown1400"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1404" intype="Vax_Float_F"     inlength="1"    name="unknown1404"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1408" intype="Int32_L"         inlength="1"    name="unknown1408"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1412" intype="Int32_L"         inlength="1"    name="unknown1412"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1416" intype="Int32_L"         inlength="1"    name="unknown1416"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1420" intype="Int32_L"         inlength="1"    name="unknown1420"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1424" intype="Int32_L"         inlength="1"    name="unknown1424"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1428" intype="Int32_L"         inlength="1"    name="unknown1428"    dicomtype="?"   dicomtag="?"    #

block="BINHDR"  offset="1540" intype="Int32_L"         inlength="1"    name="unknown1540"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1544" intype="Int32_L"         inlength="1"    keyword="tableposn"   dicomtype="DS"  dicomtag="SliceLocation"    # TP mm
block="BINHDR"  offset="1548" intype="Int32_L"         inlength="1"    keyword="tableheight" dicomtype="DS"  dicomtag="TableHeight"    # Table height mm ?
block="BINHDR"  offset="1552" intype="Int32_L"         inlength="1"    name="unknown1552"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1556" intype="Int32_L"         inlength="1"    name="unknown1556"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1560" intype="Int32_L"         inlength="1"    keyword="scannumber"  dicomtype="IS"  dicomtag="InstanceNumber"    # Scan number
block="BINHDR"  offset="1564" intype="String_Date_DMY" inlength="12"   keyword="scandate"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1576" intype="String_Time"     inlength="12"   keyword="scantime"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1588" intype="String"          inlength="24"   name="unknown1588"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1612" intype="Int32_L"         inlength="1"    name="unknown1612"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1616" intype="Int32_L"         inlength="1"    name="unknown1616"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1620" intype="Int32_L"         inlength="1"    name="unknown1620"    dicomtype="?"   dicomtag="?"    #

block="BINHDR"  offset="1664" intype="String"          inlength="40"   name="unknown1664"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1704" intype="Int32_L"         inlength="1"    name="unknown1704"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1708" intype="Int32_L"         inlength="1"    name="unknown1708"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1718" intype="Int16_L"         inlength="1"    name="unknown1718"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1720" intype="Int16_L"         inlength="1"    name="unknown1720"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1722" intype="Int16_L"         inlength="1"    name="unknown1722"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1724" intype="Int16_L"         inlength="1"    name="unknown1724"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1726" intype="Int16_L"         inlength="1"    name="unknown1726"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1728" intype="Vax_Float_F"     inlength="1"    name="unknown1728"    dicomtype="?"   dicomtag="?"    #

block="BINHDR"  offset="1792" intype="Int16_L"         inlength="1"    name="unknown1792"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1794" intype="Int16_L"         inlength="1"    name="unknown1794"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1796" intype="Int16_L"         inlength="1"    name="unknown1796"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1798" intype="Int16_L"         inlength="1"    name="unknown1798"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1800" intype="Int16_L"         inlength="1"    name="unknown1800"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1802" intype="Int16_L"         inlength="1"    name="unknown1802"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1804" intype="Int32_L"         inlength="1"    name="unknown1804"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1808" intype="Int16_L"         inlength="1"    name="unknown1808"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1810" intype="Int16_L"         inlength="1"    name="unknown1810"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1812" intype="Int16_L"         inlength="1"    name="unknown1812"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1814" intype="Int16_L"         inlength="1"    name="unknown1814"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1816" intype="Int32_L"         inlength="1"    name="unknown1816"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1820" intype="Int32_L"         inlength="1"    name="unknown1820"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1824" intype="Int16_L"         inlength="1"    name="unknown1824"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1826" intype="Int16_L"         inlength="1"    name="unknown1826"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1828" intype="Int16_L"         inlength="1"    name="unknown1828"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1830" intype="Int16_L"         inlength="1"    name="unknown1830"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1832" intype="Int16_L"         inlength="1"    name="unknown1832"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1834" intype="Int16_L"         inlength="1"    name="unknown1834"    dicomtype="?"   dicomtag="?"    #
block="BINHDR"  offset="1836" intype="Int16_L"         inlength="1"    name="unknown1836"    dicomtype="?"   dicomtag="?"    #

block="TEXTHDR"	offset="4"    intype="String"          inlength="21"   keyword="tproduct"    dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="25"   intype="String"          inlength="5"    keyword="tversion"    dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="30"   intype="String"          inlength="30"   keyword="thospital"   dicomtype="LO"  dicomtag="InstitutionName"    #
block="TEXTHDR"	offset="60"   intype="String_Time"     inlength="8"    keyword="ttime"       dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="72"   intype="String"          inlength="12"   keyword="tsliceno"    dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="84"   intype="String"          inlength="2"    keyword="ttableposnf" dicomtype="?"   dicomtag="?"    enum="'TP'=Table position mm"	#
block="TEXTHDR"	offset="86"   intype="String"          inlength="10"   keyword="ttableposn"  dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="96"   intype="String"          inlength="24"   keyword="tpatname"    dicomtype="PN"  dicomtag="PatientName"    #
block="TEXTHDR"	offset="120"  intype="String"          inlength="13"   keyword="tpatid"	     dicomtype="LO"  dicomtag="PatientID"      #
block="TEXTHDR"	offset="133"  intype="String_Date_DMY" inlength="12"   keyword="tdate"       dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="145"  intype="String"          inlength="2"    keyword="tacqtimesf"  dicomtype="?"   dicomtag="?"    enum="'TI'=Acquisition time secs"	#
block="TEXTHDR"	offset="147"  intype="String"          inlength="10"   keyword="tacqtimes"   dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="157"  intype="String"          inlength="3"    keyword="tmasf"       dicomtype="?"   dicomtag="?"    enum="'mAs'=Exposure mAs"    #
block="TEXTHDR"	offset="160"  intype="String"          inlength="9"    keyword="tmas"        dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="169"  intype="String"          inlength="2"    keyword="tkvf"        dicomtype="?"   dicomtag="?"    enum="'kV'=Tube voltage kV"    #
block="TEXTHDR"	offset="171"  intype="String"          inlength="10"   keyword="tkv"         dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="181"  intype="String"          inlength="2"    keyword="tslf"        dicomtype="?"   dicomtag="?"    enum="'SL'=Slice thickness mm" #
block="TEXTHDR"	offset="183"  intype="String"          inlength="10"   keyword="tsl"         dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="193"  intype="String"          inlength="2"    keyword="tgtf"        dicomtype="?"   dicomtag="?"    enum="'GT'=Gantry Tilt degrees"    #
block="TEXTHDR"	offset="195"  intype="String"          inlength="10"   keyword="tgt"         dicomtype="DS"  dicomtag="GantryDetectorTilt"    #
block="TEXTHDR"	offset="205"  intype="String"          inlength="2"    keyword="tzof"        dicomtype="?"   dicomtag="?"    enum="'ZO'=Zoom factor"    #
block="TEXTHDR"	offset="207"  intype="String"          inlength="10"   keyword="tzo"         dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="217"  intype="String"          inlength="2"    keyword="tcef"        dicomtype="?"   dicomtag="?"    enum="'CE'=Zoom center"    #
block="TEXTHDR"	offset="219"  intype="String"          inlength="12"   keyword="tcex"        dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="231"  intype="String"          inlength="10"   keyword="tcey"        dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="241"  intype="String"          inlength="2"    keyword="tmaf"        dicomtype="?"   dicomtag="?"    enum="'CE'=Tube current mA"    #
block="TEXTHDR"	offset="243"  intype="String"          inlength="10"   keyword="tma"         dicomtype="?"   dicomtag="?"    #
block="TEXTHDR"	offset="253"  intype="String"          inlength="10"   keyword="tlabelright" dicomtype="?"   dicomtag="?"	enum="'LEFT'=left,'RIGHT'=Right,'ANTERIOR'=Anterior,'POSTERIOR'=Posterior,'HEAD'=Head,'FEET'=Feet"    #
block="TEXTHDR"	offset="263"  intype="String"          inlength="10"   keyword="tlabeltop"   dicomtype="?"   dicomtag="?"	enum="'LEFT'=left,'RIGHT'=Right,'ANTERIOR'=Anterior,'POSTERIOR'=Posterior,'HEAD'=Head,'FEET'=Feet"    #
block="TEXTHDR"	offset="273"  intype="String"          inlength="40"   keyword="tcomment"    dicomtype="LT"   dicomtag="ImageComments"				   #

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

constant=""			dicomtype="LO"	dicomtag="DeviceSerialNumber"		# 
constant="SIEMENS"		dicomtype="LO"	dicomtag="Manufacturer"			# 
constant=""			dicomtype="DS"	dicomtag="PatientWeight"		# 
constant=""			dicomtype="CS"	dicomtag="PatientSex"			# 
constant=""			dicomtype="DA"	dicomtag="PatientBirthDate"		# 
constant=""			dicomtype="AS"	dicomtag="PatientAge"			# 

constant=""			dicomtype="CS"	dicomtag="BodyPartExamined"		# 
constant=""			dicomtype="SH"	dicomtag="FilterType"			# 
constant="CT"			dicomtype="CS"	dicomtag="Modality"			# 
