#include "attr.h"
#include "attrlist.h"
#include "attrval.h"
#include "elmdict.h"
#include "elmconst.h"
#include "elmtype.h"
#include "binvalc.h"
#include "strvalc.h"
#include "tagvalc.h"
#include "condnc.h"
#include "mesgtext.h"

#include "module.h"

#include "modulec.h"

static void
LogElementAndModule(const char *module,const char *element,
		TextOutputStream& log)
{
	if (element) log << " " << MMsgDC(Element) << "=<" << element << ">";
	if (module)  log << " " << MMsgDC(Module)  << "=<" << module  << ">";
}

static void
ViolationMessage(const char *error,const char *elementtype,
		const char *module,const char *element,
		TextOutputStream& log,bool verbose)
{
	(void)verbose;
	log << EMsgDC(Null) << error << " " << elementtype;
	LogElementAndModule(module,element,log);
	log << endl;
}

static void
WarningMessage(const char *error,const char *elementtype,
		const char *module,const char *element,
		TextOutputStream& log,bool verbose)
{
	(void)verbose;
	log << WMsgDC(Null) << error << " " << elementtype;
	LogElementAndModule(module,element,log);
	log << endl;
}

static void
ValidMessage(const char *elementtype,
		const char *module,const char *element,
		TextOutputStream& log,bool verbose)
{
	if (verbose) {
		log << MMsgDC(ValidElement) << " - " << elementtype;
		LogElementAndModule(module,element,log);
		log << endl;
	}
}

static bool
verifyRequired(Attribute *attr,
	const char *module,const char *element,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	Uint16 multiplicityMin,Uint16 multiplicityMax)
{
	// Normalized Required Data Element

	const char *reason=0;
	if (attr) {
		if (attr->getVL() == 0) {
			reason=MMsgDC(EmptyAttribute);
		}
		else {
			if (!attr->verifyVR(module,element,log,dict)) {
				reason=MMsgDC(BadValueRepresentation);
			}
			else {
				if (!attr->verifyVM(module,element,log,dict,multiplicityMin,multiplicityMax)) {
					reason=MMsgDC(BadAttributeValueMultiplicity);
				}
			}
		}
	}
	else {
		reason=MMsgDC(MissingAttribute);
	}

	if (reason)
		ViolationMessage(reason,MMsgDC(NormalizedRequired),module,element,log,verbose);
	else if (attr)
		ValidMessage(MMsgDC(NormalizedRequired),module,element,log,verbose);

	return !reason;
}

static bool
verifyType1 (Attribute *attr,
	const char *module,const char *element,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	Uint16 multiplicityMin,Uint16 multiplicityMax)
{
	// Type 1 - Required Data Element

	const char *reason=0;
	if (attr) {
		if (attr->isEmptyOrHasAllEmptyValues()) {
			reason=MMsgDC(EmptyAttribute);
		}
		else {
			if (!attr->verifyVR(module,element,log,dict)) {
				reason=MMsgDC(BadValueRepresentation);
			}
			else {
				if (!attr->verifyVM(module,element,log,dict,multiplicityMin,multiplicityMax)) {
					reason=MMsgDC(BadAttributeValueMultiplicity);
				}
			}
		}
	}
	else {
		reason=MMsgDC(MissingAttribute);
	}

	if (reason)
		ViolationMessage(reason,MMsgDC(Type1),module,element,log,verbose);
	else if (attr)
		ValidMessage(MMsgDC(Type1),module,element,log,verbose);

	return !reason;
}

static bool
verifyType1C(Attribute *attr,
	const char *module,const char *element,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	bool (*condition)(AttributeList *,AttributeList *,AttributeList *),
	bool mbpo,
	AttributeList *list,
	AttributeList *parentlist,
	AttributeList *rootlist,
	Uint16 multiplicityMin,Uint16 multiplicityMax)
{
//cerr << "verifyType1C(): " << element << endl;
	// Type 1C - Conditional Data Element

	Assert(list);

	const char *reason=0;
	if (attr) {
//cerr << "verifyType1C(): " << element << " is present" << endl;
		bool conditionNotSatisfied = false;
		if (condition && !(*condition)(list,parentlist,rootlist)) {
			conditionNotSatisfied=true;
			if (!mbpo) {
				ViolationMessage(MMsgDC(AttributePresentWhenConditionUnsatisfiedWithoutMayBePresentOtherwise),
					MMsgDC(Type1C),module,element,log,verbose);
			}
		}
//cerr << "verifyType1C(): " << element << " conditionNotSatisfied = " << conditionNotSatisfied << endl;
//cerr << "verifyType1C(): " << element << " VL = " << attr->getVL() << endl;
//cerr << "verifyType1C(): " << element << " VM = " << attr->getVM() << endl;
//cerr << "verifyType1C(): " << element << " isEmpty() = " << attr->isEmpty() << endl;
//cerr << "verifyType1C(): " << element << " isEmptyOrHasAllEmptyValues() = " << attr->isEmptyOrHasAllEmptyValues() << endl;
		if (attr->isEmptyOrHasAllEmptyValues()) {
			reason= conditionNotSatisfied ? MMsgDC(EmptyAttributeWhenConditionUnsatisfied) : MMsgDC(EmptyAttribute);
		}
		else {
			if (!attr->verifyVR(module,element,log,dict)) {
				reason=MMsgDC(BadValueRepresentation);
			}
			else {
				if (!attr->verifyVM(module,element,log,dict,multiplicityMin,multiplicityMax)) {
					reason=MMsgDC(BadAttributeValueMultiplicity);
					attr=0;
				}
			}
		}
	}
	else {
		if (condition && (*condition)(list,parentlist,rootlist))
			reason=MMsgDC(MissingAttribute);
	}

	if (reason)
		ViolationMessage(reason,MMsgDC(Type1C),module,element,log,verbose);
	else if (attr)
		ValidMessage(MMsgDC(Type1C),module,element,log,verbose);

	return !reason;
}

static bool
verifyType2 (Attribute *attr,
	const char *module,const char *element,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	Uint16 multiplicityMin,Uint16 multiplicityMax)
{
	// Type 2 - Required Data Element (May be Empty)

	const char *reason=0;
	if (attr) {
		if (attr->getVL() == 0) {
			// may be empty
		}
		else {
			if (!attr->verifyVR(module,element,log,dict)) {
				reason=MMsgDC(BadValueRepresentation);
			}
			else {
				if (!attr->verifyVM(module,element,log,dict,multiplicityMin,multiplicityMax)) {
					reason=MMsgDC(BadAttributeValueMultiplicity);
				}
			}
		}
	}
	else {
		reason=MMsgDC(MissingAttribute);
	}

	if (reason)
		ViolationMessage(reason,MMsgDC(Type2),module,element,log,verbose);
	else if (attr)
		ValidMessage(MMsgDC(Type2),module,element,log,verbose);

	return !reason;
}

static bool
verifyType2C(Attribute *attr,
	const char *module,const char *element,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	bool (*condition)(AttributeList *,AttributeList *,AttributeList *),
	bool mbpo,
	AttributeList *list,
	AttributeList *parentlist,
	AttributeList *rootlist,
	Uint16 multiplicityMin,Uint16 multiplicityMax)
{
	// Type 2C - Conditional Data Element (May be Empty)

	Assert(list);

	const char *reason=0;
	if (attr) {
		if (condition && !(*condition)(list,parentlist,rootlist)) {
			if (!mbpo) {
				ViolationMessage(MMsgDC(AttributePresentWhenConditionUnsatisfiedWithoutMayBePresentOtherwise),
					MMsgDC(Type2C),module,element,log,verbose);
			}
		}
		if (attr->getVL() == 0) {
			// may be empty
		}
		else {
			if (!attr->verifyVR(module,element,log,dict)) {
				reason=MMsgDC(BadValueRepresentation);
			}
			else {
				if (!attr->verifyVM(module,element,log,dict,multiplicityMin,multiplicityMax)) {
					reason=MMsgDC(BadAttributeValueMultiplicity);
				}
			}
		}
	}
	else {
		if (condition && (*condition)(list,parentlist,rootlist))
			reason=MMsgDC(MissingAttribute);
	}

	if (reason)
		ViolationMessage(reason,MMsgDC(Type2C),module,element,log,verbose);
	else if (attr)
		ValidMessage(MMsgDC(Type2C),module,element,log,verbose);

	return !reason;
}

static bool
verifyType3 (Attribute *attr,
	const char *module,const char *element,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	Uint16 multiplicityMin,Uint16 multiplicityMax)
{
	// Type 3 - Optional Data Element

	const char *reason=0;
	if (attr) {
		if (attr->getVL() == 0) {
			// may be empty
		}
		else {
			if (!attr->verifyVR(module,element,log,dict)) {
				reason=MMsgDC(BadValueRepresentation);
			}
			else {
				if (!attr->verifyVM(module,element,log,dict,multiplicityMin,multiplicityMax)) {
					reason=MMsgDC(BadAttributeValueMultiplicity);
				}
			}
		}
	}
	else {
		// May be absent
	}

	if (reason)
		ViolationMessage(reason,MMsgDC(Type3),module,element,log,verbose);
	else if (attr)
		ValidMessage(MMsgDC(Type3),module,element,log,verbose);

	return !reason;
}

static bool
verifyType3C(Attribute *attr,
	const char *module,const char *element,
	bool verbose,TextOutputStream& log,
	ElementDictionary *dict,
	bool (*condition)(AttributeList *,AttributeList *,AttributeList *),
	// mbpo never applies
	AttributeList *list,
	AttributeList *parentlist,
	AttributeList *rootlist,
	Uint16 multiplicityMin,Uint16 multiplicityMax)
{
	// Type 3C - Optional Data Element that can only be present when condition is true

	Assert(list);

	const char *reason=0;
	if (attr) {
		if (condition && !(*condition)(list,parentlist,rootlist)) {
			WarningMessage(MMsgDC(Unexpected),
				MMsgDC(Type3C),module,element,log,verbose);
		}
		if (attr->getVL() == 0) {
			// may be empty
		}
		else {
			if (!attr->verifyVR(module,element,log,dict)) {
				reason=MMsgDC(BadValueRepresentation);
			}
			else {
				if (!attr->verifyVM(module,element,log,dict,multiplicityMin,multiplicityMax)) {
					reason=MMsgDC(BadAttributeValueMultiplicity);
					attr=0;
				}
			}
		}
	}
	else {
		// May be absent
	}

	if (reason)
		ViolationMessage(reason,MMsgDC(Type3C),module,element,log,verbose);
	else if (attr)
		ValidMessage(MMsgDC(Type3C),module,element,log,verbose);

	return !reason;
}

#include "modulev.h"



