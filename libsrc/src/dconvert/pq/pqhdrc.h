// For the Picker PQ CT format, the header is not simple enough to use
// the template based header mechanism

// The format consists of a table of attribute names, offsets, lengths and
// types, and then an area into which this table points to get the values

// The approach used here is to read those tables and values into the
// PQ_HeaderClass, and take note of where the pixel data starts after
// all that stuff ...

PQ_HeaderClass::PQ_HeaderClass(istream *ist)
{
	Assert(ist);

	long file_offset = 0;

	// Search for "<<< Resynch field >>>" then skip to line after next '\n' ...

	char c;
	do {
		ist->read(&c,1);
		++file_offset;
//cerr << "c: " << c << endl;
	} while (c != '<' && ist->good());
	do {
		ist->read(&c,1);
		++file_offset;
//cerr << "c: " << c << endl;
	} while (c == '<' && ist->good());
	do {
		ist->read(&c,1);
		++file_offset;
//cerr << "c: " << c << endl;
	} while (c != '>' && ist->good());
	do {
		ist->read(&c,1);
		++file_offset;
//cerr << "c: " << c << endl;
	} while (c == '>' && ist->good());
	do {
		ist->read(&c,1);
		++file_offset;
//cerr << "c: " << c << endl;
	} while (c != '\n' && ist->good());

	if (!ist->good()) return;

	char textline2[512];
	ist->getline(textline2,512,'\n');
	file_offset+=ist->gcount();
//cerr << "textline2: " << textline2 << endl;
	char textline3[512];
	ist->getline(textline3,512,'\n');
	file_offset+=ist->gcount();
//cerr << "textline3: " << textline3 << endl;
	char textline4[512];
	ist->getline(textline4,512,'\n');
	file_offset+=ist->gcount();
//cerr << "textline4: " << textline4 << endl;
	char textline5[512];
	ist->getline(textline5,512,'\n');
	file_offset+=ist->gcount();
//cerr << "textline5: " << textline5 << endl;

	char *ptr;
	char *newptr;

	// e.g. "dbrec recsz:  32 nrecs: 193"

	int dbrec_recsz = 0;
	dbrec_nrec = 0;		// kept as part of class state

	newptr=strdup(textline3);
	assert(newptr);
	if ((ptr=strtok(newptr," ")) && strncmp(ptr,"dbrec",strlen("dbrec")) == 0
	 && (ptr=strtok(0," ")) && strncmp(ptr,"recsz:",strlen("recsz:")) == 0
	 && (ptr=strtok(0," "))
	) {
		dbrec_recsz=atoi(ptr);
	}
	if ((ptr=strtok(0," ")) && strncmp(ptr,"nrecs:",strlen("nrecs:")) == 0
	 && (ptr=strtok(0," "))
	) {
		dbrec_nrec=atoi(ptr);
	}

//cerr << "dbrec_recsz: " << dbrec_recsz << endl;
//cerr << "dbrec_nrec: " << dbrec_nrec << endl;

	if (newptr) free(newptr);

	int rdb_recsz = 0;
	int rdb_nrec = 0;

	// e.g. "rdb   recsz:  64 nrecs:  25"

	newptr=strdup(textline4);
	assert(newptr);
	if ((ptr=strtok(newptr," ")) && strncmp(ptr,"rdb",strlen("rdb")) == 0
	 && (ptr=strtok(0," ")) && strncmp(ptr,"recsz:",strlen("recsz:")) == 0
	 && (ptr=strtok(0," "))
	) {
		rdb_recsz=atoi(ptr);
	}
	if ((ptr=strtok(0," ")) && strncmp(ptr,"nrecs:",strlen("nrecs:")) == 0
	 && (ptr=strtok(0," "))
	) {
		rdb_nrec=atoi(ptr);
	}

//cerr << "rdb_recsz: " << rdb_recsz << endl;
//cerr << "rdb_nrec: " << rdb_nrec << endl;

	if (newptr) free(newptr);

	int data_offset = 0;
	int data_offset_actual = 0;
	int magic_num_data_dict = 0;
	int num_sdr_fields = 0;

	// e.g. "8192 =data_offset  8056 =actual; 877355679 =magic_#_data_dict; 83 =#_sdr_flds"

	newptr=strdup(textline5);
	assert(newptr);
	if ((ptr=strtok(newptr," "))
	) {
		data_offset=atoi(ptr);
	}

	if ((ptr=strtok(0," ")) && strncmp(ptr,"=data_offset",strlen("=data_offset")) == 0
	 && (ptr=strtok(0," "))
	) {
		data_offset_actual=atoi(ptr);
	}
	if ((ptr=strtok(0," ")) && strncmp(ptr,"=actual;",strlen("=actual;")) == 0
	 && (ptr=strtok(0," "))
	) {
		magic_num_data_dict=atoi(ptr);
	}
	if ((ptr=strtok(0," ")) && strncmp(ptr,"=magic_#_data_dict;",strlen("=magic_#_data_dict;")) == 0
	 && (ptr=strtok(0," "))
	) {
		num_sdr_fields=atoi(ptr);
	}
	// don't bother checking for trailing "=#_sdr_flds;"

//cerr << "data_offset: " << data_offset << endl;
//cerr << "data_offset_actual: " << data_offset_actual << endl;
//cerr << "magic_num_data_dict: " << magic_num_data_dict << endl;
//cerr << "num_sdr_fields: " << num_sdr_fields << endl;

	if (newptr) free(newptr);

//cerr << "file_offset (start of db records): " << file_offset << endl;

	// read the db records, i.e. the table of attributes and their names/offsets/lengths/types

	// (this represents the persistent state in the class)

	db_fieldname = new char *  [dbrec_nrec];
	db_offset    = new unsigned[dbrec_nrec];
	db_length    = new unsigned[dbrec_nrec];
	db_vtype     = new unsigned[dbrec_nrec];
	db_value     = new char *  [dbrec_nrec];

	Assert(db_fieldname);
	Assert(db_offset);
	Assert(db_length);
	Assert(db_vtype);
	Assert(db_value);

	int i;
	for (i=0; i < dbrec_nrec; ++i) {
		char s_fieldname[9];
		unsigned char s_offset[2];
		unsigned char s_length[2];
		unsigned char s_field3[2];
		unsigned char s_field4[2];
		unsigned char s_vtype[2];
		char s_filler[64];		// actually only need dbrec_recsz-18 (should be 14)

		long file_offset_start=file_offset;

		ist->read(s_fieldname,8); s_fieldname[8]=0;
		file_offset+=ist->gcount();
		ist->read((char *)s_offset,2);
		file_offset+=ist->gcount();
		ist->read((char *)s_length,2);
		file_offset+=ist->gcount();
		ist->read((char *)s_field3,2);
		file_offset+=ist->gcount();
		ist->read((char *)s_field4,2);
		file_offset+=ist->gcount();
		ist->read((char *)s_vtype,2);
		file_offset+=ist->gcount();
		ist->read(s_filler,dbrec_recsz-18);
		file_offset+=ist->gcount();

		db_fieldname[i]=new char[strlen(s_fieldname)+1];
		strcpy(db_fieldname[i],s_fieldname);

		db_offset[i]=((unsigned)s_offset[0]<<8)+s_offset[1];
		db_length[i]=((unsigned)s_length[0]<<8)+s_length[1];
		db_vtype[i]=((unsigned)s_vtype[0] <<8)+s_vtype[1];
#ifdef CRAP
		cerr << file_offset_start << ": "
		     << "<" << db_fieldname[i] << ">"
		     << "\t = " << db_offset[i]
		     << " [" << db_length[i] << "]"
		     << " [" << db_vtype[i] << "]"
		     << endl;
#endif
	}

	// read the rdb records (i.e. the actual values of the attributes) ...

	int rdb_length = rdb_recsz*rdb_nrec;
	char *rdb_values = new char[rdb_length];
	Assert(rdb_values);
	ist->read(rdb_values,rdb_length);
	Assert(ist->gcount() == rdb_length);
	file_offset+=rdb_length;

	// fill in the attribute table with values ...

	for (i=0; i < dbrec_nrec; ++i) {
		char *vtype_label;
		switch (db_vtype[i]) {
			case 0:	vtype_label="asc"; break;
			case 1:	vtype_label="int"; break;
			case 2:	vtype_label="dat"; break;
			case 3:	vtype_label="tim"; break;
			case 4:	vtype_label="fil"; break;
			case 5:	vtype_label="flg"; break;
			case 6:	vtype_label="flt"; break;
			default:
				vtype_label="???"; break;
		}
		char *value=new char[db_length[i]+1];
		Assert(value);
		Assert(db_offset[i]+db_length[i] <= rdb_length);
		strncpy(value,rdb_values+db_offset[i],db_length[i]);
		value[db_length[i]]=0;

		db_value[i]=value;
#ifdef CRAP

		cerr << "<" << db_fieldname[i] << ">"
		     << "\t = " << db_offset[i]
		     << " [" << db_length[i] << "]"
		     << " [" << db_vtype[i] << "]" << " " << vtype_label
		     << "\t = <" << db_value[i] << ">" << endl;
#endif
	}

	if (rdb_values) delete[] rdb_values;	// anything useful already copied out

//cerr << "file_offset (end of db records): " << file_offset << endl;

	//pixel_offset=file_offset+8;		// skip the "\nEND=\n--"
	pixel_offset=file_offset+(data_offset-data_offset_actual);

//cerr << "pixel_offset: " << pixel_offset << endl;
}

PQ_HeaderClass::~PQ_HeaderClass(void)
{
	// all strings were copied ...

	if (db_fieldname) {
		int i;
		for (i=0; i < dbrec_nrec; ++i) {
			if (db_fieldname[i]) delete[] db_fieldname[i];
		}
		delete[] db_fieldname;
	}
	if (db_value) {
		int i;
		for (i=0; i < dbrec_nrec; ++i) {
			if (db_value[i]) delete[] db_value[i];
		}
		delete[] db_value;
	}
	if (db_offset) delete[] db_offset;
	if (db_length) delete[] db_length;
	if (db_vtype) delete[] db_vtype;
}

bool
PQ_HeaderClass::getAttributeEntry(const char *name,int &index) const
{
	Assert(name);
	int i;
	for (i=0; i < dbrec_nrec; ++i) {
		if (strcmp(db_fieldname[i],name) == 0) {
			Assert(db_value[i] != 0);
			index=i;
			return true;
		}
	}
	return false;
}

bool
PQ_HeaderClass::getStringAttribute(const char *name,int &index,char *&value,int &length) const
{
	if (getAttributeEntry(name,index)) {
		length=db_length[index];
		value=new char[length+1];
		Assert(value);
		strcpy(value,db_value[index]);
		return true;
	}
	return false;
}

bool
PQ_HeaderClass::getAsciiAttribute(const char *name,char *&value,int &length) const
{
	int index;
	if (getStringAttribute(name,index,value,length)) {
		//Assert(db_vtype[index] == 0);
		return true;
	}
	return false;
}

bool
PQ_HeaderClass::getIntegerAttribute(const char *name,Int32 &value) const
{
	int index;
	if (getAttributeEntry(name,index)) {
		//Assert(db_vtype[index] == 1 || db_vtype[index] == 6);	// allow float to int conversion
		Assert(db_length[index] > 0);
		Assert(*(db_value[index]));
		value=atoi(db_value[index]);
		return true;
	}
	return false;
}

bool
PQ_HeaderClass::getDateAttribute(const char *name,char *&value,int &length) const
{
	int index;
	if (getStringAttribute(name,index,value,length)) {
		//Assert(db_vtype[index] == 2);
		return true;
	}
	return false;
}

bool
PQ_HeaderClass::getTimeAttribute(const char *name,char *&value,int &length) const
{
	int index;
	if (getStringAttribute(name,index,value,length)) {
		//Assert(db_vtype[index] == 3);
		return true;
	}
	return false;
}

bool
PQ_HeaderClass::getFileAttribute(const char *name,char *&value,int &length) const
{
	int index;
	if (getStringAttribute(name,index,value,length)) {
		//Assert(db_vtype[index] == 4);
		return true;
	}
	return false;
}

bool
PQ_HeaderClass::getFlagAttribute(const char *name,char *&value,int &length) const
{
	int index;
	if (getStringAttribute(name,index,value,length)) {
		//Assert(db_vtype[index] == 5);
		return true;
	}
	return false;
}

bool
PQ_HeaderClass::getFloatAttribute(const char *name,Float64 &value) const
{
	int index;
	if (getAttributeEntry(name,index)) {
		//Assert(db_vtype[index] == 6 || db_vtype[index] == 1);	// allow int to float conversion
		Assert(db_length[index] > 0);
		Assert(*(db_value[index]));
		value=atof(db_value[index]);
		return true;
	}
	return false;
}

