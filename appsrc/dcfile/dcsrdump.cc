#include "attrmxls.h"
#include "mesgtext.h"
#include "dcopt.h"
#include "elmconst.h"
#include "attrval.h"

static inline void
indent(TextOutputStream &log,unsigned depth,bool showtabs,bool showgt) {
	unsigned d=depth;
	if (showtabs) while (d--) log << "\t";
	d=depth;
	if (showgt) while (d--) log << ">";
}

static void
logStringValueOfAttribute(Attribute *a,TextOutputStream &log) {
	if (a) {
		char *v = AttributeValue(a);
		if (v) {
			log << v;
			delete[] v;
		}
	}
}

static void
processCodeSequenceItem(AttributeList *list,TextOutputStream &log) {
	log << "(";
	logStringValueOfAttribute((*list)[TagFromName(CodeValue)],log);
	log << ",";
	logStringValueOfAttribute((*list)[TagFromName(CodingSchemeDesignator)],log);
	log << "," << "\"" ;
	logStringValueOfAttribute((*list)[TagFromName(CodeMeaning)],log);
	log << "\")";
}

static void
processCodeSequence(Attribute *aCodeSequence,const char *label,TextOutputStream &log) {
	log << label;

	if (aCodeSequence) {
		int nItems = 0;
		AttributeList ** vItems = 0;
		nItems=aCodeSequence->getLists(&vItems);
		Assert(nItems==0 || vItems);

		int i;
		AttributeList **iptr;

		for (i=0,iptr=vItems; i<nItems; ++i,++iptr) {
			AttributeList *ptr = *iptr;
			Assert(ptr);
			if (i > 0) log << ", ";
			processCodeSequenceItem(ptr,log);
		}
	}
}

static void
processMeasuredValueSequence(Attribute *aMeasuredValueSequence,const char *label,TextOutputStream &log) {
	log << label;

	if (aMeasuredValueSequence) {
		int nItems = 0;
		AttributeList ** vItems = 0;
		nItems=aMeasuredValueSequence->getLists(&vItems);
		Assert(nItems==0 || vItems);

		int i;
		AttributeList **iptr;

		for (i=0,iptr=vItems; i<nItems; ++i,++iptr) {
			AttributeList *ptr = *iptr;
			Assert(ptr);
			if (i > 0) log << ", ";

			logStringValueOfAttribute((*ptr)[TagFromName(NumericValue)],log);

			Attribute *aMeasurementUnitsCodeSequence=(*ptr)[TagFromName(MeasurementUnitsCodeSequence)];
			if (aMeasurementUnitsCodeSequence) {
				log << " ";
				processCodeSequence(aMeasurementUnitsCodeSequence,"",log);
			}
		}
	}
}

static void
processReferencedSOPSequence(Attribute *aReferencedSOPSequence,const char *label,TextOutputStream &log) {
	log << label;

	if (aReferencedSOPSequence) {
		int nItems = 0;
		AttributeList ** vItems = 0;
		nItems=aReferencedSOPSequence->getLists(&vItems);
		Assert(nItems==0 || vItems);

		int i;
		AttributeList **iptr;

		for (i=0,iptr=vItems; i<nItems; ++i,++iptr) {
			AttributeList *ptr = *iptr;
			Assert(ptr);
			if (i > 0) log << ", ";

			log << "(";
			logStringValueOfAttribute((*ptr)[TagFromName(ReferencedSOPClassUID)],log);
			log << ",";
			logStringValueOfAttribute((*ptr)[TagFromName(ReferencedSOPInstanceUID)],log);
			log << ")";
			
			// the following items are specific to the usage in IMAGE content items (and hence should do nothing in others) ...
			
			Attribute *aReferencedFrameNumber=(*ptr)[TagFromName(ReferencedFrameNumber)];
			if (aReferencedFrameNumber) {
				log << " [Frame ";
				logStringValueOfAttribute(aReferencedFrameNumber,log);
				log << "]";
			}
			
			Attribute *aReferencedSegmentNumber=(*ptr)[TagFromName(ReferencedSegmentNumber)];
			if (aReferencedSegmentNumber) {
				log << " [Segment ";
				logStringValueOfAttribute(aReferencedSegmentNumber,log);
				log << "]";
			}

			Attribute *aPresentationStateReferencedSOPSequence=(*ptr)[TagFromName(ReferencedSOPSequence)];
			if (aPresentationStateReferencedSOPSequence) {
				int nItems = 0;
				AttributeList ** vItems = 0;
				nItems=aPresentationStateReferencedSOPSequence->getLists(&vItems);
				Assert(nItems==0 || vItems);
				Assert(nItems==0 || vItems);

				int i;
				AttributeList **iptr;

				for (i=0,iptr=vItems; i<nItems; ++i,++iptr) {
					AttributeList *ptr = *iptr;
					Assert(ptr);
					if (i > 0) log << ",";

					log << " (PS ";
					logStringValueOfAttribute((*ptr)[TagFromName(ReferencedSOPClassUID)],log);
					log << ",";
					logStringValueOfAttribute((*ptr)[TagFromName(ReferencedSOPInstanceUID)],log);
					log << ")";
				}
			}

			Attribute *aReferencedRealWorldValueMappingInstanceSequence=(*ptr)[TagFromName(ReferencedRealWorldValueMappingInstanceSequence)];
			if (aReferencedRealWorldValueMappingInstanceSequence) {
				int nItems = 0;
				AttributeList ** vItems = 0;
				nItems=aReferencedRealWorldValueMappingInstanceSequence->getLists(&vItems);
				Assert(nItems==0 || vItems);
				Assert(nItems==0 || vItems);

				int i;
				AttributeList **iptr;

				for (i=0,iptr=vItems; i<nItems; ++i,++iptr) {
					AttributeList *ptr = *iptr;
					Assert(ptr);
					if (i > 0) log << ",";

					log << " (RWV ";
					logStringValueOfAttribute((*ptr)[TagFromName(ReferencedSOPClassUID)],log);
					log << ",";
					logStringValueOfAttribute((*ptr)[TagFromName(ReferencedSOPInstanceUID)],log);
					log << ")";
				}
			}
			
		}
	}
}

static void
processScoordContentItem(AttributeList *list,TextOutputStream &log) {
	logStringValueOfAttribute((*list)[TagFromName(GraphicType)],log);
	log << " {";
	//log.unsetf(ios_base::fixed,ios_base::scientific);	// :) should really explicitly establish the default floating point notation, in which the precision field specifies the maximum number of meaningful digits to display in total counting both those before and those after the decimal point
	log << setprecision (15);							// this choice of precision is based on the fact that a DS VR is 16 characters, including the decimal point; this is excessive in that the FL VR (32 bit IEEE float) significand has a precision of 24 binary bits (about 7 decimal digits)
	Attribute *aGraphicData=(*list)[TagFromName(GraphicData)];
	int nGraphicData = aGraphicData ? aGraphicData->getVM() : 0;
	int i;
	for (i=0; i<nGraphicData; ++ i) {
		if (i > 0) log << ",";
		float v;
		if (aGraphicData->getValue(i,v)) log << v;
	}
	log << "}";
}

static void
processValueOfContentItem(AttributeList *list,TextOutputStream &log) {
	Attribute *aValueType=(*list)[TagFromName(ValueType)];
	char *vValueType=AttributeValue(aValueType);
	if (vValueType) {
		if      (strcmp(vValueType,"NUM") == 0) {
			log << " = ";
			processMeasuredValueSequence((*list)[TagFromName(MeasuredValueSequence)],"",log);
			Attribute *aNumericValueQualifierCodeSequence=(*list)[TagFromName(NumericValueQualifierCodeSequence)];
			if (aNumericValueQualifierCodeSequence) {
				log << " ";
				processCodeSequence(aNumericValueQualifierCodeSequence,"",log);
			}
		}
		else if (strcmp(vValueType,"TEXT") == 0) {
			log << " = \"";
			logStringValueOfAttribute((*list)[TagFromName(TextValue)],log);
			log << "\"";
		}
		else if (strcmp(vValueType,"CODE") == 0) {
			log << " = ";
			processCodeSequence((*list)[TagFromName(ConceptCodeSequence)],"",log);
		}
		else if (strcmp(vValueType,"DATE") == 0) {
			log << " = \"";
			logStringValueOfAttribute((*list)[TagFromName(Date)],log);
			log << "\"";
		}
		else if (strcmp(vValueType,"TIME") == 0) {
			log << " = \"";
			logStringValueOfAttribute((*list)[TagFromName(Time)],log);
			log << "\"";
		}
		else if (strcmp(vValueType,"DATETIME") == 0) {
			log << " = \"";
			logStringValueOfAttribute((*list)[TagFromName(DateTime)],log);
			log << "\"";
		}
		else if (strcmp(vValueType,"PNAME") == 0) {
			log << " = \"";
			logStringValueOfAttribute((*list)[TagFromName(PersonName)],log);
			log << "\"";
		}
		else if (strcmp(vValueType,"UIDREF") == 0) {
			log << " = \"";
			logStringValueOfAttribute((*list)[TagFromName(UID)],log);
			log << "\"";
		}
		else if (strcmp(vValueType,"IMAGE") == 0) {
			log << " = ";
			processReferencedSOPSequence((*list)[TagFromName(ReferencedSOPSequence)],"",log);
		}
		else if (strcmp(vValueType,"WAVEFORM") == 0) {
			processReferencedSOPSequence((*list)[TagFromName(ReferencedSOPSequence)],"",log);
		}
		else if (strcmp(vValueType,"SCOORD") == 0) {
			log << " = ";
			processScoordContentItem(list,log);
		}
		else if (strcmp(vValueType,"TCOORD") == 0) {
		}
		else if (strcmp(vValueType,"COMPOSITE") == 0) {
			processReferencedSOPSequence((*list)[TagFromName(ReferencedSOPSequence)],"",log);
		}
		else if (strcmp(vValueType,"CONTAINER") == 0) {
			log << " [";
			logStringValueOfAttribute((*list)[TagFromName(ContinuityOfContent)],log);
			log << "]";
		}
		else {
			log << " **** Unrecognized Value Type ****";
		}

		delete[] vValueType;
	}
}


static bool
processContentItem(AttributeList *list,TextOutputStream &log,bool verbose,bool veryverbose,unsigned depth,const char *identifierstring)   {

	bool success=true;

	Attribute *        aRelationshipType=(*list)[TagFromName(RelationshipType)];
	Attribute *         aContentSequence=(*list)[TagFromName(ContentSequence)];
	Attribute * aConceptNameCodeSequence=(*list)[TagFromName(ConceptNameCodeSequence)];
	Attribute *               aValueType=(*list)[TagFromName(ValueType)];

	indent(log,depth,true,true);
	if (identifierstring) {
		log << identifierstring << ": ";
	}
	
	logStringValueOfAttribute(aRelationshipType,log);
	log << ": ";

	logStringValueOfAttribute(aValueType,log);
	log << ": ";

	if (aConceptNameCodeSequence) {
		processCodeSequence(aConceptNameCodeSequence,"",log);
		log << " ";
	}

	processValueOfContentItem(list,log);

	log << endl;

	if (aContentSequence) {
		int nContentItems = 0;
		AttributeList ** vContentItems = 0;
		nContentItems=aContentSequence->getLists(&vContentItems);
		Assert(nContentItems==0 || vContentItems);

		int i;
		AttributeList **iptr;

		for (i=0,iptr=vContentItems; i<nContentItems; ++i,++iptr) {
			AttributeList *ptr = *iptr;
			Assert(ptr);
			char *childidentifierstring = NULL;
			if (identifierstring) {
				ostrstream childidentifierstream;
				childidentifierstream << identifierstring << "." << (i+1) << ends;
				childidentifierstring = childidentifierstream.str();
			}
			success&=processContentItem(ptr,log,verbose,veryverbose,depth+1,childidentifierstring);
			if (childidentifierstring) {
				delete [] childidentifierstring;
			}
		}
	}
	return success;
}


static bool
parseStructuredReport(ManagedAttributeList &list,TextOutputStream &log,bool verbose,bool veryverbose,bool showidentifier)
{
	bool result = processContentItem(&list,log,verbose,veryverbose,0,showidentifier ? "1" : NULL);
	return result;
}

int
main(int argc, char *argv[])
{
	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);

	bool veryverbose=options.get("veryverbose") || options.get("vv");
	bool verbose=options.get("verbose") || options.get("v") || veryverbose;
	bool showfilename=options.get("filename");
	bool showidentifier=options.get("identifier");

	dicom_input_options.done();
	options.done();

	DicomInputOpenerFromOptions input_opener(
		options,dicom_input_options.filename,cin);

	cerr << dicom_input_options.errors();
	cerr << options.errors();
	cerr << input_opener.errors();

	if (!dicom_input_options.good()
	 || !options.good()
	 || !input_opener.good()
	 || !options) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< " [-v|-verbose|-vv|-veryverbose]"
			<< " [-filename]"
			<< " [-identifier]"
			<< " [" << MMsgDC(InputFile) << "]"
			<< " <" << MMsgDC(InputFile)
			<< endl;
		exit(1);
	}

	DicomInputStream din(*(istream *)input_opener,
		dicom_input_options.transfersyntaxuid,
		dicom_input_options.usemetaheader);

	ManagedAttributeList list;

	bool success=true;
	TextOutputStream log(cerr);
	
	if (showfilename) {
		const char *filenameused = input_opener.getFilename();
		cerr << "Filename: \"" << (filenameused && strlen(filenameused) > 0 ? filenameused : "-") << "\"" << endl;
	}

	if (veryverbose) log << "******** While reading ... ********" << endl; 
	list.read(din,&log,veryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

	const char *errors=list.errors();
	if (errors) log << errors << flush;
	if (!list.good()) {
		log << EMsgDC(DatasetReadFailed) << endl;
		success=false;
	}

	//if (veryverbose) log << "******** As read ... ********" << endl; 
	//log << list;
	//list.write(log,veryverbose);

	success=success&&parseStructuredReport(list,log,verbose,veryverbose,showidentifier);

	return success ? 0 : 1;
}




