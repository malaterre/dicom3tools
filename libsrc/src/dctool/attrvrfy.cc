#include "attr.h"
#include "mesgtext.h"
#include "elmdict.h"
#include "elmtype.h"

bool
Attribute::verifyDefinedTerms(char *(*method)(char *value),
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	int which) const
{
	Assert(dict);
	bool success=true;
	if (isString()) {
		unsigned vm=getVM();
		unsigned start = (which == -1) ? 0 : which;
		unsigned end = (which == -1) ? vm : (which+1);
		unsigned i;
		for (i=start; i < end; ++i) {
			char *value;
			if (getValue(i,value)) {
				char *desc;
				if (value && (desc=(*method)(value))) {
					if (verbose) {
						log << MMsgDC(RecognizedDefinedTerm)
						    << " <" << value << "> " << MMsgDC(Is) << " <" << desc
						    << "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
						    << " " << MMsgDC(OfAttribute) << " <"
						    << (dict ? dict->getDescription(getTag()) : "")
							<< ">" << endl;
					}
					delete desc;
				}
				else {
					log << WMsgDC(UnrecognizedDefinedTerm)
					    << " <" << (value ? value : "")
					    << "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
					    << " " << MMsgDC(OfAttribute) << " <"
					    << (dict ? dict->getDescription(getTag()) : "")
						<< ">" << endl;
					// does NOT cause failure (cf. EnumValues)
				}
				delete value;
			}
			else {
//cerr << "Attribute::verifyDefinedTerms: couldn't get value=" << dec << i << endl;
				// couldn't get a value - not a failure (eg. may be type 2 or 3)
			}
		}
	}
	else {
		log << EMsgDC(TriedToVerifyDefinedTermsForNonStringAttribute)
		    << MMsgDC(ForAttribute) << " <"
		    << (dict ? dict->getDescription(getTag()) : "")
		    << ">" << endl;
		success=false;
	}
//cerr << "Attribute::verifyDefinedTerms: return " << (success ? "T" : "F") << endl;
	return success;
}

bool
Attribute::verifyEnumValues(char *(*method)(char *value),
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	int which) const
{
//cerr << "Attribute::verifyEnumValues: which=" << dec << which << endl;
	Assert(dict);
	bool success=true;
	if (isString()) {
		unsigned vm=getVM();
//cerr << "Attribute::verifyEnumValues: vm=" << dec << vm << endl;
		unsigned start = (which == -1) ? 0 : which;
		unsigned end = (which == -1) ? vm : (which+1);
//cerr << "Attribute::verifyEnumValues: start=" << dec << start << endl;
//cerr << "Attribute::verifyEnumValues: end=" << dec << end << endl;
		unsigned i;
		for (i=start; i < end; ++i) {
//cerr << "Attribute::verifyEnumValues: doing=" << dec << i << endl;
			char *value;
			if (getValue(i,value)) {
				char *desc;
//if (value) cerr << "Attribute::verifyEnumValues: value=" << value << endl;
				if (value && (desc=(*method)(value))) {
					if (verbose) {
						log << MMsgDC(RecognizedEnumeratedValue)
						    << " <" << value << "> " << MMsgDC(Is) << " <" << desc
							<< "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
						    << " " << MMsgDC(OfAttribute) << " <"
						    << ">" << (dict ? dict->getDescription(getTag()) : "")
							<< endl;
					}
					delete desc;
				}
				else {
					log << EMsgDC(UnrecognizedEnumeratedValue)
					    << " <" << (value ? value : "")
					    << "> " << MMsgDC(ForValue) << " " << dec << (i+1)
					    << " " << MMsgDC(OfAttribute) << " <"
					    << (dict ? dict->getDescription(getTag()) : "")
						<< ">" << endl;
					success=false;
				}
				delete value;
			}
			else {
//cerr << "Attribute::verifyEnumValues: couldn't get value=" << dec << i << endl;
				// couldn't get a value - not a failure (eg. may be type 2 or 3)
			}
		}
	}
	else {
		log << EMsgDC(TriedToVerifyEnumeratedValueForNonStringAttribute)
		    << MMsgDC(ForAttribute) << " <"
		    << (dict ? dict->getDescription(getTag()) : "")
		    << ">" << endl;
		success=false;
	}
//cerr << "Attribute::verifyEnumValues: return " << (success ? "T" : "F") << endl;
	return success;
}

bool
Attribute::verifyEnumValues(char *(*method)(Uint16 value),
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	int which) const
{
//cerr << "Attribute::verifyEnumValues: which=" << dec << which << endl;
	Assert(dict);
	bool success=true;
	if (isNumeric()) {
		unsigned vm=getVM();
//cerr << "Attribute::verifyEnumValues: vm=" << dec << vm << endl;
		unsigned start = (which == -1) ? 0 : which;
		unsigned end = (which == -1) ? vm : (which+1);
//cerr << "Attribute::verifyEnumValues: start=" << dec << start << endl;
//cerr << "Attribute::verifyEnumValues: end=" << dec << end << endl;
		unsigned i;
		for (i=start; i < end; ++i) {
//cerr << "Attribute::verifyEnumValues: doing=" << dec << i << endl;
			Uint16 value;
			if (getValue(i,value)) {
//cerr << "Attribute::verifyEnumValues: value=" << hex << value << dec << endl;
				char *desc=(*method)(value);
				if (desc) {
					if (verbose) {
						log << MMsgDC(RecognizedEnumeratedValue)
						    << " <" << hex << value << dec << "> " << MMsgDC(Is) << " <" << desc
						    << "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
							<< " " << MMsgDC(OfAttribute) << " <"
						    << (dict ? dict->getDescription(getTag()) : "")
							<< ">" << endl;
					}
					delete desc;
				}
				else {
					log << EMsgDC(UnrecognizedEnumeratedValue)
					    << " <" << hex << value << dec
					    << "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
					    << " " << MMsgDC(OfAttribute) << " <"
					    << (dict ? dict->getDescription(getTag()) : "")
						<< ">" << endl;
					success=false;
				}
			}
			else {
//cerr << "Attribute::verifyEnumValues: couldn't get value=" << dec << i << endl;
				// couldn't get a value - not a failure (eg. may be type 2 or 3)
			}
		}
	}
	else {
		log << EMsgDC(TriedToVerifyEnumeratedValueForNonNumericAttribute)
		    << MMsgDC(ForAttribute) << " <"
		    << (dict ? dict->getDescription(getTag()) : "")
		    << ">" << endl;
		success=false;
	}
//cerr << "Attribute::verifyEnumValues: return " << (success ? "T" : "F") << endl;
	return success;
}

bool
Attribute::verifyBitMap(char *(*method)(Uint16 value),
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	int which) const
{
	Assert(dict);
	bool success=true;
	if (isNumeric()) {
		unsigned vm=getVM();
		unsigned start = (which == -1) ? 0 : which;
		unsigned end = (which == -1) ? vm : (which+1);
		unsigned i;
		for (i=start; i < end; ++i) {
			Uint16 value;
			if (getValue(i,value)) {
				char *desc=(*method)(value);
				if (desc) {
					if (verbose) {
						log << MMsgDC(RecognizedBitMap)
						    << " <" << hex << value << dec << "> " << MMsgDC(Is) << " <" << desc
							<< "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
						    << " " << MMsgDC(OfAttribute) << " <"
						    << (dict ? dict->getDescription(getTag()) : "")
						    << ">" << endl;
					}
					delete desc;
				}
				else {
					log << EMsgDC(UnrecognizedBitMap)
					    << " <" << hex << value << dec
						<< "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
					    << " " << MMsgDC(OfAttribute) << " <"
					    << (dict ? dict->getDescription(getTag()) : "")
					    << ">" << endl;
					success=false;
				}
			}
			else {
//cerr << "Attribute::verifyBitMap: couldn't get value=" << dec << i << endl;
				// couldn't get a value - not a failure (eg. may be type 2 or 3)
			}
		}
	}
	else {
		log << EMsgDC(TriedToVerifyBitMapForNonNumericAttribute)
		    << MMsgDC(ForAttribute) << " <"
		    << (dict ? dict->getDescription(getTag()) : "")
		    << ">" << endl;
		success=false;
	}
//cerr << "Attribute::verifyBitMap: return " << (success ? "T" : "F") << endl;
	return success;

}

bool
Attribute::verifyEnumValues(char *(*method)(Uint16 group,Uint16 element),
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	int which) const
{
//cerr << "Attribute::verifyEnumValues: which=" << dec << which << endl;
	Assert(dict);
	bool success=true;
	if (isTag()) {
		unsigned vm=getVM();
//cerr << "Attribute::verifyEnumValues: vm=" << dec << vm << endl;
		unsigned start = (which == -1) ? 0 : which;
		unsigned end = (which == -1) ? vm : (which+1);
//cerr << "Attribute::verifyEnumValues: start=" << dec << start << endl;
//cerr << "Attribute::verifyEnumValues: end=" << dec << end << endl;
		unsigned i;
		for (i=start; i < end; ++i) {
//cerr << "Attribute::verifyEnumValues: doing=" << dec << i << endl;
			Tag value;
			if (getValue(i,value)) {
				Uint16 group=value.getGroup();
				Uint16 element=value.getElement();
//cerr << "Attribute::verifyEnumValues: value=(" << hex << group << "," << element << ")" << dec << endl;
				char *desc=(*method)(group,element);
				if (desc) {
					if (verbose) {
						log << MMsgDC(RecognizedEnumeratedValue)
						    << " (" << hex << group << "," << element << ") " << dec << MMsgDC(Is) << " <" << desc
							<< "> " << MMsgDC(ForValue) << " "  << dec << (i+1)
						    << " " << MMsgDC(OfAttribute) << " <"
						    << (dict ? dict->getDescription(getTag()) : "")
						    << ">" << endl;
					}
					delete desc;
				}
				else {
					log << EMsgDC(UnrecognizedEnumeratedValue)
					    << " (" << hex << group << "," << element << ") " << dec
						<< MMsgDC(ForValue) << " "  << dec << (i+1)
					    << " " << MMsgDC(OfAttribute) << " <"
					    << (dict ? dict->getDescription(getTag()) : "")
					    << ">" << endl;
					success=false;
				}
			}
			else {
//cerr << "Attribute::verifyEnumValues: couldn't get value=" << dec << i << endl;
				// couldn't get a value - not a failure (eg. may be type 2 or 3)
			}
		}
	}
	else {
		log << EMsgDC(TriedToVerifyEnumeratedValueForNonTagAttribute)
		    << MMsgDC(ForAttribute) << " <"
		    << (dict ? dict->getDescription(getTag()) : "")
		    << ">" << endl;
		success=false;
	}
//cerr << "Attribute::verifyEnumValues: return " << (success ? "T" : "F") << endl;
	return success;
}

bool
Attribute::verifyNotZero(
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	int which,
	bool warningNotError) const
{
//cerr << "Attribute::verifyNotZero: which=" << dec << which << endl;
	Assert(dict);
	bool success=true;
	if (isNumeric()) {
		unsigned vm=getVM();
//cerr << "Attribute::verifyNotZero: vm=" << dec << vm << endl;
		unsigned start = (which == -1) ? 0 : which;
		unsigned end = (which == -1) ? vm : (which+1);
//cerr << "Attribute::verifyNotZero: start=" << dec << start << endl;
//cerr << "Attribute::verifyNotZero: end=" << dec << end << endl;
		unsigned i;
		for (i=start; i < end; ++i) {
//cerr << "Attribute::verifyNotZero: doing=" << dec << i << endl;
			Float64 value;
			if (getValue(i,value)) {
//cerr << "Attribute::verifyNotZero: value=" << dec << value << endl;
				if (value == 0) {
					log << (warningNotError ? WMsgDC(ZeroValue) : EMsgDC(ZeroValue))
						<< " " << MMsgDC(ForValue) << " "  << dec << (i+1)
					    << " " << MMsgDC(OfAttribute) << " <"
					    << (dict ? dict->getDescription(getTag()) : "")
					    << ">" << endl;
					success=false;
				}
			}
			else {
//cerr << "Attribute::verifyNotZero: couldn't get value=" << dec << i << endl;
				// couldn't get a value - not a failure (eg. may be type 2 or 3)
			}
		}
	}
	else {
		log << EMsgDC(TriedToVerifyNotZeroForNonNumericAttribute)
		    << MMsgDC(ForAttribute) << " <"
		    << (dict ? dict->getDescription(getTag()) : "")
		    << ">" << endl;
		success=false;
	}
//cerr << "Attribute::verifyNotZero: return " << ((warningNotError || success) ? "T" : "F") << endl;
	return warningNotError || success;
}

bool
Attribute::verifyVR(const char *module,const char *element,
	TextOutputStream& log,
	ElementDictionary *dict) const
{
	Assert(dict);
	Tag tag=getTag();
	const char *vrd=dict->getValueRepresentation(tag);
	const char *vre=getVR();

	if (!vrd) {
		log << EMsgDC(NoSuchElementInDictionary);
		if (element) log << MMsgDC(Element) << "=<" << element << ">";
		if (module)  log << MMsgDC(Module)  << "=<" << module  << ">";
		log << endl;
		return false;
	}
	else {
		if (strncmp(vre,vrd,2) != 0
		    && !( strncmp(vrd,"OX",2) == 0
		          && (strncmp(vre,"OB",2) == 0
		              || strncmp(vre,"OW",2) == 0)
		        )
		    && !( strncmp(vrd,"XS",2) == 0
		          && (strncmp(vre,"US",2) == 0
		              || strncmp(vre,"SS",2) == 0)
		        )
		    && !( strncmp(vrd,"XO",2) == 0
		          && (strncmp(vre,"US",2) == 0
		              || strncmp(vre,"SS",2) == 0
		              || strncmp(vre,"OW",2) == 0)
		        )
		    && !( strncmp(vrd,"XL",2) == 0
		          && (strncmp(vre,"UL",2) == 0
		              || strncmp(vre,"SL",2) == 0)
		        )
		    ) {
			log << EMsgDC(BadValueRepresentation)
			    << " " << vre << " (" << vrd
			    << " " << MMsgDC(Required) << ")";
			if (element) log << " " << MMsgDC(Element) << "=<" << element << ">";
			if (module)  log << " " << MMsgDC(Module)  << "=<" << module  << ">";
			log << endl;
			return false;
		}
		else {
			return true;
		}
	}
}

bool
Attribute::verifyVM(const char *module,const char *element,
	TextOutputStream& log,
	ElementDictionary *dict,
	Uint16 multiplicityMin,Uint16 multiplicityMax,const char *specifiedSource) const
{
	Assert(dict);
	Tag tag=getTag();
	Uint16 vm=getVM();
//log << "Attribute::verifyVM(): getVM() = " << vm << endl;
	Uint16 dictmin=dict->getValueMultiplicityMinimum(tag);
	Uint16 dictmax=dict->getValueMultiplicityMaximum(tag);
	Uint16 errmin,errmax;
	int err=0;
	const char *source;
	Assert(VMNONE == 0);
//log << "Attribute::verifyVM(): multiplicityMin = " << multiplicityMin << " multiplicityMax = " << multiplicityMax << endl;
	if (multiplicityMin == 0 && multiplicityMax == 0) {	// ie. don't override dictionary
		if (vm < dictmin || vm >dictmax) {
			err=1;
			errmin=dictmin;
			errmax=dictmax;
			source=MMsgDC(Dictionary);
		}
	}
	else {
//log << "Attribute::verifyVM(): checking using multiplicityMin and multiplicityMax" << endl;
		if (vm < multiplicityMin || vm >multiplicityMax) {
//log << "Attribute::verifyVM(): error" << endl;
			err=1;
			errmin=multiplicityMin;
			errmax=multiplicityMax;
			source= specifiedSource ? specifiedSource : MMsgDC(ModuleDefinition);
		}
	}
	if (err) {
		log << EMsgDC(BadAttributeValueMultiplicity)
		    <<  " " << dec << vm << " (" << errmin;
		if (errmin != errmax)
			if (errmax == VMUNLIMITED)
				log << "-n";
			else
				log << "-" << errmax;
		log << " " << MMsgDC(RequiredBy) << " " << source << ")";
		if (element) log << " " << MMsgDC(Element) << "=<" << element << ">";
		if (module)  log << " " << MMsgDC(Module)  << "=<" << module  << ">";
		log << endl;
		return false;
	}
	else {
		return true;
	}
}

