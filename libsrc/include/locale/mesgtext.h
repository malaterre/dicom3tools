#ifndef __Header_mesgtext__
#define __Header_mesgtext__

#include "strtype.h"

class EMSGDC_Class {
public:
	char *		error(const char *index);
	char *		warning(const char *index);
	char *		abort(const char *index);
	const char *	message(const char *index);
};

static EMSGDC_Class EMSGDC_Instance;

#define EMsgDC(index)	String_Use(EMSGDC_Instance.error(#index))
#define WMsgDC(index)	String_Use(EMSGDC_Instance.warning(#index))
#define AMsgDC(index)	String_Use(EMSGDC_Instance.abort(#index))
#define MMsgDC(index)	EMSGDC_Instance.message(#index)

#define EMsgDCNull()	String_Use(EMSGDC_Instance.error("Null"))
#define WMsgDCNull()	String_Use(EMSGDC_Instance.warning("Null"))
#define AMsgDCNull()	String_Use(EMSGDC_Instance.abort("Null"))
#define MMsgDCNull()	EMSGDC_Instance.message("Null")

#endif /* __Header_mesgtext__ */
