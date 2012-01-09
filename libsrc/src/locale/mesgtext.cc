#include <string.h>
#include <iomanip.h>

#include "basetype.h"
#include "mesgtext.h"

struct EMSGDC_Table_Entry {
	const char *index;
	const char *description;
};

#include "mesgttbl.h"

static void
dummy(void)
{
	(void)&EMSGDC_Instance;		// shuts up unused warning
}

static const char *
findDescription(const char *index)
{
	Assert(index);
	// Efficiency is not an issue here, so linear search is fine
	EMSGDC_Table_Entry *ptr;
	for (ptr=EMSGDC_Table; ptr && ptr->index && strcmp(ptr->index,index) != 0; ++ptr);
	//Assert(ptr && ptr->index && ptr->description);
	if (!ptr || !ptr->index || !ptr->description) {
		return index;
	}
	else
		return ptr->description;
}

char *
EMSGDC_Class::error(const char *index)
{
	ostrstream stream;
	stream << findDescription("Error") << " - ";
	if (index && strlen(index)) stream << findDescription(index) << ends;
	return stream.str();
}

char *
EMSGDC_Class::warning(const char *index)
{
	ostrstream stream;
	stream << findDescription("Warning") << " - ";
	if (index && strlen(index)) stream << findDescription(index) << ends;
	return stream.str();
}

char *
EMSGDC_Class::abort(const char *index)
{
	ostrstream stream;
	stream << findDescription("Abort") << " - ";
	if (index && strlen(index)) stream << findDescription(index) << ends;
	return stream.str();
}

const char *
EMSGDC_Class::message(const char *index)
{
	return index && strlen(index) ? findDescription(index) : "";
}

