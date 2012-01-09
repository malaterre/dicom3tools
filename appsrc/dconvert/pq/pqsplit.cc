#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>	// for atoi()

int
main(int argc,char **argv)
{
	long file_offset = 0;
	char initialhdr[512];
	while (cin.read(initialhdr,512)
	    && cin.gcount() == 512
	    && initialhdr[0] != 0) {		// seems to end with a null 512 bytes

		file_offset+=cin.gcount();
//cerr << "filename: " << initialhdr << endl;

		char textline1[512];
		cin.getline(textline1,512,'\n');
		file_offset+=cin.gcount();
//cerr << "textline1: " << textline1 << endl;
		char textline2[512];
		cin.getline(textline2,512,'\n');
		file_offset+=cin.gcount();
//cerr << "textline2: " << textline2 << endl;
		char textline3[512];
		cin.getline(textline3,512,'\n');
		file_offset+=cin.gcount();
//cerr << "textline3: " << textline3 << endl;
		char textline4[512];
		cin.getline(textline4,512,'\n');
		file_offset+=cin.gcount();
//cerr << "textline4: " << textline4 << endl;
		char textline5[512];
		cin.getline(textline5,512,'\n');
		file_offset+=cin.gcount();
//cerr << "textline5: " << textline5 << endl;

		// istrstream only works with CC not g++ :( , so use strtok() instead ...

		// e.g. "dbrec recsz:  32 nrecs: 193"

		int dbrec_recsz = 0;
		int dbrec_nrec = 0;

		//istrstream istr_textline3(textline3);
		//istr_textline3 >> "dbrec" >> "recsz:" >> dbrec_recsz >> "nrecs:" >> dbrec_nrec;

		char *ptr;
		char *newptr;

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

		// e.g. "rdb   recsz:  64 nrecs:  25"

		int rdb_recsz = 0;
		int rdb_nrec = 0;

		//istrstream istr_textline4(textline4);
		//istr_textline4 >> "rdb" >> "recsz:" >> rdb_recsz >> "nrecs:" >> rdb_nrec;

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

		//istrstream istr_textline5(textline5);
		//istr_textline5 >> data_offset >> " =data_offset" >> data_offset_actual >> " =actual;"
		//	       >> magic_num_data_dict >> " =magic_#_data_dict;" >> num_sdr_fields >> " =#_sdr_flds;";

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

		// read the db records, i.e. the table of attributes and their names/offsets/lengths/types

		char **db_fieldname = new char *[dbrec_nrec];
		unsigned *db_offset = new unsigned[dbrec_nrec];
		unsigned *db_length = new unsigned[dbrec_nrec];
		unsigned *db_vtype  = new unsigned[dbrec_nrec];
		char **db_value     = new char *[dbrec_nrec];

		assert(db_fieldname);
		assert(db_offset);
		assert(db_length);
		assert(db_vtype);
		assert(db_value);

		int i;
		for (i=0; i < dbrec_nrec; ++i) {
			char s_fieldname[9];
			unsigned char s_offset[2];
			unsigned char s_length[2];
			unsigned char s_field3[2];
			unsigned char s_field4[2];
			unsigned char s_vtype[2];
			char s_filler[64];		// actually only need dbrec_recsz-18 (should be 14)

			cin.read(s_fieldname,8); s_fieldname[8]=0;
			file_offset+=cin.gcount();
			cin.read((char *)s_offset,2);
			file_offset+=cin.gcount();
			cin.read((char *)s_length,2);
			file_offset+=cin.gcount();
			cin.read((char *)s_field3,2);
			file_offset+=cin.gcount();
			cin.read((char *)s_field4,2);
			file_offset+=cin.gcount();
			cin.read((char *)s_vtype,2);
			file_offset+=cin.gcount();
			cin.read(s_filler,dbrec_recsz-18);
			file_offset+=cin.gcount();

			db_fieldname[i]=new char[strlen(s_fieldname)+1];
			strcpy(db_fieldname[i],s_fieldname);

			db_offset[i]=((unsigned)s_offset[0]<<8)+s_offset[1];
			db_length[i]=((unsigned)s_length[0]<<8)+s_length[1];
			db_vtype[i]=((unsigned)s_vtype[0] <<8)+s_vtype[1];
#ifdef CRAP
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

			cerr << "<" << db_fieldname[i] << ">"
			     << "\t = " << db_offset[i]
			     << " [" << db_length[i] << "]"
			     << " [" << db_vtype[i] << "]" << " " << vtype_label
			     << endl;
#endif
		}

		// read the rdb records (i.e. the actual values of the attributes) ...

		int rdb_length = rdb_recsz*rdb_nrec;
		char *rdb_values = new char[rdb_length];
		assert(rdb_values);
		cin.read(rdb_values,rdb_length);
		assert(cin.gcount() == rdb_length);
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
			assert(value);
			assert(db_offset[i]+db_length[i] <= rdb_length);
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

		// Find various useful attribute values ...

		int image_file_size=0;
		int image_file_offset=0;
		const char *image_file_name=0;
		const char *patient_name=0;
		const char *patient_id=0;
		int study_num=0;
		int study_num_extended=0;
		for (i=0; i < dbrec_nrec; ++i) {
			if (strcmp(db_fieldname[i],"IMFISIZ") == 0) {
				assert(db_vtype[i] == 1);
				assert(db_value[i] != 0);
				image_file_size=atoi(db_value[i]);
			}
			else if (strcmp(db_fieldname[i],"IMFIOFS") == 0) {
				assert(db_vtype[i] == 1);
				assert(db_value[i] != 0);
				image_file_offset=atoi(db_value[i]);
			}
			else if (strcmp(db_fieldname[i],"IMFILE") == 0) {
				assert(db_vtype[i] == 4);
				assert(db_value[i] != 0);
				image_file_name=db_value[i];
			}
			else if (strcmp(db_fieldname[i],"PATNAM") == 0) {
				assert(db_vtype[i] == 0);
				assert(db_value[i] != 0);
				patient_name=db_value[i];
			}
			else if (strcmp(db_fieldname[i],"PATID") == 0) {
				assert(db_vtype[i] == 0);
				assert(db_value[i] != 0);
				patient_id=db_value[i];
			}
			else if (strcmp(db_fieldname[i],"STNUM") == 0) {
				assert(db_vtype[i] == 1);
				assert(db_value[i] != 0);
				study_num=atoi(db_value[i]);
			}
			else if (strcmp(db_fieldname[i],"STNUM_X") == 0) {
				assert(db_vtype[i] == 1);
				assert(db_value[i] != 0);
				study_num_extended=atoi(db_value[i]);
			}
		}

//cerr << "image_file_size = " << image_file_size << endl;
//cerr << "image_file_offset = " << image_file_offset << endl;

		cerr << "image_file_name = " << image_file_name << endl;

//cerr << "patient_name = " << patient_name << endl;
//cerr << "patient_id = " << patient_id << endl;
//cerr << "study_num = " << study_num << endl;
//cerr << "study_num_extended = " << study_num_extended << endl;

		if (image_file_name && strncmp(image_file_name,"IM.",3) == 0 && image_file_size) {
			// is a real image file so write it ...

			ofstream out(image_file_name);
			assert(out);

			out.write(initialhdr,512);
//cerr << "textline1: " << textline1 << endl;
			out.write(textline1,strlen(textline1)); out.put('\n');
//cerr << "textline2: " << textline2 << endl;
			out.write(textline2,strlen(textline2)); out.put('\n');
//cerr << "textline3: " << textline3 << endl;
			out.write(textline3,strlen(textline3)); out.put('\n');
//cerr << "textline4: " << textline4 << endl;
			out.write(textline4,strlen(textline4)); out.put('\n');
//cerr << "textline5: " << textline5 << endl;
			out.write(textline5,strlen(textline5)); out.put('\n');

			for (i=0; i < dbrec_nrec; ++i) {
				char s_fieldname[8];
				int j;
				for (j=0; j<8; ++j) s_fieldname[j]=0;
				strncpy(s_fieldname,db_fieldname[i],8);
				out.write(s_fieldname,8);
				out.put(char((db_offset[i]>>8)&0xff)); out.put(char(db_offset[i]&0xff));
				out.put(char((db_length[i]>>8)&0xff)); out.put(char(db_length[i]&0xff));
				out.put(char(0)); out.put(char(0));
				out.put(char(0)); out.put(char(0));
				out.put(char((db_vtype[i]>>8)&0xff)); out.put(char(db_vtype[i]&0xff));
				for (j=18; j < dbrec_recsz; ++j) out.put(char(0));
			}

			out.write(rdb_values,rdb_length);

			//long copy_size = image_file_size-image_file_offset+8;	// include the "\nEND=\n--" as well

			long copy_size = (image_file_size-image_file_offset)+(data_offset-data_offset_actual);

//cerr << "copy size = " << copy_size << endl;

			char buffer[8192];

//cerr << "file_offset before copy: " << file_offset << endl;
			while (copy_size > 0 && cin) {
				cin.read(buffer,copy_size < 8192 ? copy_size : 8192);
				int count = cin.gcount();
				assert(count);
				file_offset+=count;
				out.write(buffer,count);
				copy_size-=count;
			}
//cerr << "file_offset after copy: " << file_offset << endl;

			out.close();
		}
		else {		// not an image file so skip it ...


			long skip_size = image_file_size == 0
				? (8192-(file_offset%8192))		// e.g. a TP.FHEAD
				:  image_file_size-image_file_offset+8;	// skip the "\nEND=\n--" as well

//cerr << "data size to skip = " << skip_size << endl;
//cerr << "data size to skip%8192 = " << (skip_size%8192) << endl;

			char buffer[8192];

//cerr << "file_offset before skip: " << file_offset << endl;
			while (skip_size > 0 && cin) {
				cin.read(buffer,skip_size < 8192 ? skip_size : 8192);
				int count = cin.gcount();
				assert(count);
				file_offset+=count;
				skip_size-=count;
			}
//cerr << "file_offset after skip: " << file_offset << endl;
		}
	}

	return 0;
}

