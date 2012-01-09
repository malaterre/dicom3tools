#include <fstream.h>
#include <ctype.h>

#include "attrmxls.h"
#include "attrlsln.h"
#include "attrtype.h"
#include "attrseq.h"
#include "attrothr.h"
#include "attrval.h"
#include "elmdict.h"
#include "elmconst.h"
#include "sopclu.h"
#include "sopcli.h"
#include "transynu.h"
#include "uidgen.h"
#include "mesgtext.h"
#include "dcopt.h"

// In a lot of places, there is an assumption that sequences and items will
// be written delimited, ie. with undefined lengths ... this will break if
// this changes (write() in attrseq.cc) :(

// The icon image stuff uses a non-pixel data OB, and hence there is no
// means of taking into account CP 14 setOutputEncoding stuff ... this
// shouldn't be a problem as the TS is supposed to be explicit, but it
// does mean a loss of generality :(

// Writing this highlighted the fact the getVL() for sequences is not
// working and reviseVL() was never implemented :(

// The approach to making the icon image is make everything 8 bit, and
// use the first "plane" as is regardless of whether it is color or indexed
// (or even worse, interleaved) :( ... this will work really well for
// MONOCHROME2 images where BitsStored is not much bigger than the real
// maximum and the PixelRepresentaion is unsigned !

static ElementDictionary staticDictionary;

static bool
errorIfStringValuesDontMatch(const char *dirValue,
	AttributeList &list,Tag valueTag,
	const char *valueDescription,
	const char *uniqueKeyDescription,
	const char *uniqueKeyValue,
	const char *filename,TextOutputStream &log)
{	Assert(filename);
	Assert(dirValue);
	Assert(valueDescription);
	Assert(uniqueKeyDescription);
	char *listValue=AttributeValue(list[valueTag],"");
	Assert(listValue);

	bool result;
	if (strcmp(dirValue,listValue) != 0) {
		log << filename << ": "<< "Error - Different " << valueDescription << " in instance <"
		    << (listValue ? listValue : "")
		    << "> for existing " << uniqueKeyDescription << " <"
		    << (uniqueKeyValue ? uniqueKeyValue : "")
		    << "> already entered in directory as " << valueDescription << " <"
		    << (dirValue ? dirValue : "")
		    << "> - using original value and assuming they are the same entity"
		    << endl;
		result=false;
	}
	else {
		result=true;
	}
	if (listValue) delete[] listValue;
	return result;
}

static Attribute *
isAttributePresentElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Assert(label);
	Assert(filename);
	Attribute *a=list[tag];
	if (a)
		return a;
	else {
		log << filename << ": "
		    << EMsgDC(MissingAttribute)
		    << " - " << label
		    << endl;
		return 0;
	}
}

static Attribute *
isAttributePresentElseNull(AttributeList &list,Tag tag)
{
	return list[tag];
}

static Attribute *
isValuePresentElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Assert(label);
	Assert(filename);
	Attribute *a=list[tag];
	if (a && a->getVM())
		return a;
	else {
		log << filename << ": " 
		    << EMsgDC(MissingAttribute)
		    << " - " << label
		    << endl;
		return 0;
	}
}

static const char *
getStringValueElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *a=isValuePresentElseError(list,tag,label,filename,log);
	if (a) {
		return AttributeValue(a);	//SC 2.0.1 didn't like it as ? arg :(
	}
	else {
		return "";
	}
}

static const char *
getStringValueElseWarningAndDefault(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log,const char *def)
{
	Attribute *a=list[tag];
	if (a && a->getVM() > 0) {
		return AttributeValue(a);	//SC 2.0.1 didn't like it as ? arg :(
	}
	else {
		log << filename << ": " 
		    << WMsgDC(MissingAttribute)
		    << " - " << label << " - using default value <" << def << ">"
		    << endl;
		return def;
	}
}

static const char *
getStringValueElseZeroLength(AttributeList &list,Tag tag)
{
	Attribute *a=list[tag];
	if (a && a->getVM() > 0) {
		const char *s = AttributeValue(a);
		return AttributeValue(a);	//SC 2.0.1 didn't like it as ? arg :(
	}
	else {
		return "";
	}
}

static const char *
getStringValueElseNull(AttributeList &list,Tag tag)
{
	Attribute *a=list[tag];
	if (a && a->getVM() > 0) {
		return AttributeValue(a);	//SC 2.0.1 didn't like it as ? arg :(
	}
	else {
		return 0;
	}
}

static Attribute *
getCopyOfMultiValuedCodeStringAttributeElseNull(AttributeList &list,Tag tag)
{
	Attribute *attr=isAttributePresentElseNull(list,tag);
        CodeStringAttribute *newAttr = 0;

        if (attr) {
                newAttr=new CodeStringAttribute(tag);
                Assert(newAttr);
                unsigned count = attr->getVM();
                unsigned i;
                for (i=0; i < count; ++i) {
                        char *s;
                        bool gotit=attr->getValue(i,s);
                        Assert(gotit);
                        Assert(s);      // should be "" rather than 0 if zero length
                        newAttr->addValue(s);
                        // should we now free s ???? :(
                }
        }
        return newAttr;
}

static Attribute *
getCopyOfMultiValuedDecimalStringAttributeElseNull(AttributeList &list,Tag tag)
{
	Attribute *attr=isAttributePresentElseNull(list,tag);
        DecimalStringAttribute *newAttr = 0;

        if (attr) {
                newAttr=new DecimalStringAttribute(tag);
                Assert(newAttr);
                unsigned count = attr->getVM();
                unsigned i;
                for (i=0; i < count; ++i) {
                        double v;							// NOT string ... want to fix any illegal too long decimal strings in source
                        bool gotit=attr->getValue(i,v);
                        Assert(gotit);
                        newAttr->addValue(v);
                }
        }
        return newAttr;
}


static Attribute *
getCopyOfCodeSequenceAttributeElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *attr=isAttributePresentElseError(list,tag,label,filename,log);
	SequenceAttribute *newAttr = 0;

	// really would be nice to have a deep copy (clone) of any sequence attribute and its items, but for now ...

	if (attr) {
		Assert(attr->isSequence());
		newAttr = new SequenceAttribute(tag);
		Assert(newAttr);
		AttributeList **items;
		int itemCount = attr->getLists(&items);
		AttributeList **iptr;
		int i;
		for (i=0,iptr=items; i<itemCount; ++i,++iptr) {
			AttributeList *item=*iptr;
			Assert(item);
			AttributeList *newItem = new AttributeList();
			Assert(newItem);
			(*newAttr)+=newItem;

			const char *codeValue=getStringValueElseError((*item),TagFromName(CodeValue),"Code Value",filename,log);
			Assert(codeValue);
			(*newItem)+=new ShortStringAttribute(TagFromName(CodeValue),codeValue);

			const char *codingSchemeDesignator=getStringValueElseError((*item),TagFromName(CodingSchemeDesignator),"Coding Scheme Designator",filename,log);
			Assert(codingSchemeDesignator);
			(*newItem)+=new ShortStringAttribute(TagFromName(CodingSchemeDesignator),codingSchemeDesignator);

			const char *codeMeaning=getStringValueElseError((*item),TagFromName(CodeMeaning),"Code Meaning",filename,log);
			Assert(codeMeaning);
			(*newItem)+=new LongStringAttribute(TagFromName(CodeMeaning),codeMeaning);
		}
	}
	return newAttr;
}


static Attribute *
getCopyOfCodeSequenceAttributeElseEmpty(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *attr=isAttributePresentElseNull(list,tag);
	SequenceAttribute *newAttr = new SequenceAttribute(tag);
	Assert(newAttr);

	// really would be nice to have a deep copy (clone) of any sequence attribute and its items, but for now ...

	if (attr) {
		Assert(attr->isSequence());
		AttributeList **items;
		int itemCount = attr->getLists(&items);
		AttributeList **iptr;
		int i;
		for (i=0,iptr=items; i<itemCount; ++i,++iptr) {
			AttributeList *item=*iptr;
			Assert(item);
			AttributeList *newItem = new AttributeList();
			Assert(newItem);
			(*newAttr)+=newItem;

			const char *codeValue=getStringValueElseError((*item),TagFromName(CodeValue),"Code Value",filename,log);
			Assert(codeValue);
			(*newItem)+=new ShortStringAttribute(TagFromName(CodeValue),codeValue);

			const char *codingSchemeDesignator=getStringValueElseError((*item),TagFromName(CodingSchemeDesignator),"Coding Scheme Designator",filename,log);
			Assert(codingSchemeDesignator);
			(*newItem)+=new ShortStringAttribute(TagFromName(CodingSchemeDesignator),codingSchemeDesignator);

			const char *codeMeaning=getStringValueElseError((*item),TagFromName(CodeMeaning),"Code Meaning",filename,log);
			Assert(codeMeaning);
			(*newItem)+=new LongStringAttribute(TagFromName(CodeMeaning),codeMeaning);
		}
	}
	return newAttr;
}

static Attribute *
getCopyOfReferencedImageSequenceAttribute(Attribute *attr,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	SequenceAttribute *newAttr = 0;

	// really would be nice to have a deep copy (clone) of any sequence attribute and its items, but for now ...

	if (attr) {
		Assert(attr->isSequence());
		newAttr = new SequenceAttribute(tag);
		Assert(newAttr);
		AttributeList **items;
		int itemCount = attr->getLists(&items);
		AttributeList **iptr;
		int i;
		for (i=0,iptr=items; i<itemCount; ++i,++iptr) {
			AttributeList *item=*iptr;
			Assert(item);
			AttributeList *newItem = new AttributeList();
			Assert(newItem);
			(*newAttr)+=newItem;

			const char *referencedSOPClassUID=getStringValueElseError((*item),TagFromName(ReferencedSOPClassUID),"Referenced SOP Class UID",filename,log);
			Assert(referencedSOPClassUID);
			(*newItem)+=new UIStringAttribute(TagFromName(ReferencedSOPClassUID),referencedSOPClassUID);

			const char *referencedSOPInstanceUID=getStringValueElseError((*item),TagFromName(ReferencedSOPInstanceUID),"Referenced SOP Instance UID",filename,log);
			Assert(referencedSOPInstanceUID);
			(*newItem)+=new UIStringAttribute(TagFromName(ReferencedSOPInstanceUID),referencedSOPInstanceUID);
		}
	}
	return newAttr;
}


static Attribute *
getCopyOfReferencedImageSequenceAttributeElseNull(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *attr=isAttributePresentElseNull(list,tag);
	return getCopyOfReferencedImageSequenceAttribute(attr,tag,label,filename,log);
}


static Attribute *
getCopyOfReferencedImageSequenceAttributeElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *attr=isAttributePresentElseError(list,tag,label,filename,log);
	return getCopyOfReferencedImageSequenceAttribute(attr,tag,label,filename,log);
}


static Attribute *
getCopyOfReferencedSeriesSequenceAttributeElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *attr=isAttributePresentElseError(list,tag,label,filename,log);
	SequenceAttribute *newAttr = 0;

	// really would be nice to have a deep copy (clone) of any sequence attribute and its items, but for now ...

	if (attr) {
		Assert(attr->isSequence());
		newAttr = new SequenceAttribute(tag);
		Assert(newAttr);
		AttributeList **items;
		int itemCount = attr->getLists(&items);
		AttributeList **iptr;
		int i;
		for (i=0,iptr=items; i<itemCount; ++i,++iptr) {
			AttributeList *item=*iptr;
			Assert(item);
			AttributeList *newItem = new AttributeList();
			Assert(newItem);
			(*newAttr)+=newItem;

			const char *seriesInstanceUID=getStringValueElseError((*item),TagFromName(SeriesInstanceUID),"Series Instance UID",filename,log);
			Assert(seriesInstanceUID);
			(*newItem)+=new UIStringAttribute(TagFromName(SeriesInstanceUID),seriesInstanceUID);

			Attribute *referencedImageSequence=getCopyOfReferencedImageSequenceAttributeElseError((*item),TagFromName(ReferencedImageSequence),"Referenced Image Sequence",filename,log);
			Assert(referencedImageSequence);
			(*newItem)+=referencedImageSequence;
		}
	}
	return newAttr;
}

static Attribute *
getCopyOfSequenceContainingStudySeriesImageReferenceElseError(AttributeList &list,Tag tag,const char *label,const char *filename,TextOutputStream &log)
{
	Attribute *attr=isAttributePresentElseError(list,tag,label,filename,log);
	SequenceAttribute *newAttr = 0;

	// really would be nice to have a deep copy (clone) of any sequence attribute and its items, but actually want to be selective and not copy every attribute anyway (e.g., Blending Sequence)

	if (attr) {
		Assert(attr->isSequence());
		newAttr = new SequenceAttribute(tag);
		Assert(newAttr);
		AttributeList **items;
		int itemCount = attr->getLists(&items);
		AttributeList **iptr;
		int i;
		for (i=0,iptr=items; i<itemCount; ++i,++iptr) {
			AttributeList *item=*iptr;
			Assert(item);
			AttributeList *newItem = new AttributeList();
			Assert(newItem);
			(*newAttr)+=newItem;

			const char *studyInstanceUID=getStringValueElseError((*item),TagFromName(StudyInstanceUID),"Study Instance UID",filename,log);
			Assert(studyInstanceUID);
			(*newItem)+=new UIStringAttribute(TagFromName(StudyInstanceUID),studyInstanceUID);

			Attribute *referencedSeriesSequence=getCopyOfReferencedSeriesSequenceAttributeElseError((*item),TagFromName(ReferencedSeriesSequence),"Referenced Series Sequence",filename,log);
			Assert(referencedSeriesSequence);
			(*newItem)+=referencedSeriesSequence;
		}
	}
	return newAttr;
}


static SequenceAttribute *
makeIconImageSequence(AttributeList *srclist,const TransferSyntax *transfersyntax,Uint16 iRows=64,Uint16 iColumns=64)
{
	SequenceAttribute *aIconImageSequence = 0;

	Assert(srclist);
	Assert(transfersyntax);
	Assert(!transfersyntax->isImplicitVR());	// don't support CP 14 or length calculation here

	Uint16 vRows 				= AttributeValue((*srclist)[TagFromName(Rows)]);
	Uint16 vColumns 			= AttributeValue((*srclist)[TagFromName(Columns)]);
	Uint16 vNumberOfFrames 			= AttributeValue((*srclist)[TagFromName(NumberOfFrames)]);
	char * vPhotometricInterpretation 	= AttributeValue((*srclist)[TagFromName(PhotometricInterpretation)]);
	Uint16 vSamplesPerPixel 		= AttributeValue((*srclist)[TagFromName(SamplesPerPixel)]);
	Uint16 vBitsAllocated 			= AttributeValue((*srclist)[TagFromName(BitsAllocated)]);
	Uint16 vBitsStored 			= AttributeValue((*srclist)[TagFromName(BitsStored)]);
	Uint16 vHighBit 			= AttributeValue((*srclist)[TagFromName(HighBit)]);
	Uint16 vPixelRepresentation 		= AttributeValue((*srclist)[TagFromName(PixelRepresentation)],0xffff);
	Uint16 vPlanarConfiguration 		= AttributeValue((*srclist)[TagFromName(PlanarConfiguration)],0xffff);

	Attribute *aPixelData = (*srclist)[TagFromName(PixelData)];

	if (vRows && vColumns && vPhotometricInterpretation
	 && vSamplesPerPixel && vBitsAllocated && vBitsStored && vHighBit
	 && vPixelRepresentation != 0xffff
	 && (vSamplesPerPixel <= 1 || vPlanarConfiguration != 0xffff)
	 && vRows >= iRows && vColumns >= iColumns	// upsampling not supported
	) {

		Uint16 downRows=vRows/iRows;
		Uint16 downColumns=vColumns/iColumns;

		Assert(downRows);	// < icon size not yet supported
		Assert(downColumns);

		SourceBase<Uint16> *source=SupplySourceFromAttribute(aPixelData).getSource();
		Assert(source);

		unsigned char *iconPixelData=new unsigned char[iRows*iColumns];
		Assert(iconPixelData);

		unsigned row=0;
		unsigned column=0;
		unsigned irow=0;
		unsigned icolumn=0;
		unsigned char *p=iconPixelData;
		while (row < vRows && source->read()) {
			size_t n=source->getBufferCount();
			Assert(n);
			const Uint16 *buffer=source->getBuffer();
			Assert(buffer);
			while (n-- && row < vRows) {
				if (row%downRows == 0 && column%downColumns == 0 && irow < iRows && icolumn < iColumns) {
					Assert(p-iconPixelData < iRows*iColumns);
					*p++=*buffer>>(vBitsStored-8);
					if (++icolumn >= iColumns) {
						icolumn=0;
						++irow;
					}
				}
				buffer++;
				if (++column >= vColumns) {
					column=0;
					++row;
				}
			}
		}
		Assert(p-iconPixelData == iRows*iColumns);
		//Assert(column == 0 && row == vRows*vNumberOfFrames);
		aIconImageSequence=new SequenceAttribute(TagFromName(IconImageSequence));
		AttributeList *iconlist = new AttributeList;
		Assert(iconlist);

		Attribute *aIconPixelData=new OtherByteSmallNonPixelAttribute(TagFromName(PixelData));
		Assert(aIconPixelData);
		aIconPixelData->setValue(iconPixelData,Uint32(iRows*iColumns));
		(*iconlist)+=aIconPixelData;

		(*iconlist)+=new UnsignedShortAttribute(TagFromName(SamplesPerPixel),1);
		(*iconlist)+=new CodeStringAttribute(TagFromName(PhotometricInterpretation),"MONOCHROME2");
		(*iconlist)+=new UnsignedShortAttribute(TagFromName(Rows),iRows);
		(*iconlist)+=new UnsignedShortAttribute(TagFromName(Columns),iColumns);
		(*iconlist)+=new UnsignedShortAttribute(TagFromName(BitsAllocated),8);
		(*iconlist)+=new UnsignedShortAttribute(TagFromName(BitsStored),8);
		(*iconlist)+=new UnsignedShortAttribute(TagFromName(HighBit),7);
		(*iconlist)+=new UnsignedShortAttribute(TagFromName(PixelRepresentation),0);

		(*aIconImageSequence)+=iconlist;
		// aIconImageSequence->reviseVL();	// Not yet implemented :(
	}
	
	return aIconImageSequence;
}


static bool
addLengths(AttributeList &list,const TransferSyntax *metats,const TransferSyntax *datats)		// Derived from attrmxls.cc
{
	// Fixup and/or insert group lengths

	// each (nnnn,0000) group length element
	// contains the length in bytes of the rest of the group
	// not including the length element itself

	bool metaseen=false;
	bool dothisgroup=true;
	Uint16 doinggroup=0;
	Uint32 length=0;
	AttributeList lengthlist;
	AttributeListIterator listi(list);
	while (1) {
		Attribute *a;
		Tag tag;
		if (!listi) {
			a=listi();
			Assert(a);
			tag=a->getTag();
		}
		if (!!listi || tag.getGroup() > doinggroup) {
			if (length && dothisgroup && doinggroup == 0x0002) {
				lengthlist+=new UnsignedLongAttribute(Tag(doinggroup,0),length);
			}
			if (!!listi)
				break;
			else {
				doinggroup=tag.getGroup();
				dothisgroup=true;
				length=0;
			}
		}
		else if (tag.getGroup() == doinggroup) {
                        Uint32 add=lengthOfEntireAttribute(metats,datats,a);
			if (add == 0xffffffff)
				dothisgroup=false;
			else
				length+=add;
			++listi;
		}
		else
			++listi;

		if (tag.isMetaheaderGroup())
			metaseen=true;
		else
			if (metaseen) break;
	}

	list+=(lengthlist);

	return true;
}

class DirectoryRecordWriteOrder {
	int order;
public:
	static const int ParentChildSibling = 1;
	static const int ParentSiblingChild = 2;
	static const int SiblingChildParent = 3;
	static const int SiblingParentChild = 4;
	static const int ChildSiblingParent = 5;
	static const int ChildParentSibling = 6;

	DirectoryRecordWriteOrder(int i) { order = i; }

	int getOrder(void) { return order; }
};

class DirectoryRecordBase {
protected:
	DirectoryRecordBase *sibling;
	DirectoryRecordBase *child;

	AttributeList *list;

	Uint32 selfoffset;

	virtual TextOutputStream& writeSpecific(TextOutputStream &log) = 0;
	virtual TextOutputStream& indentSpecific(TextOutputStream &log) = 0;

public:
	DirectoryRecordBase(DirectoryRecordBase *nextsib,AttributeList *srclist)
		{
			child=0;
			sibling=nextsib;

			list=new AttributeList();
			(*list)+=new UnsignedLongAttribute(TagFromName(OffsetOfTheNextDirectoryRecord),0);
			(*list)+=new UnsignedShortAttribute(TagFromName(RecordInUseFlag),0xffff);
			(*list)+=new UnsignedLongAttribute(TagFromName(OffsetOfReferencedLowerLevelDirectoryEntity),0);

			selfoffset=0;

			Attribute *specificCharacterSet=getCopyOfMultiValuedCodeStringAttributeElseNull((*srclist),TagFromName(SpecificCharacterSet));
			if (specificCharacterSet) (*list)+=specificCharacterSet;
		}

	virtual ~DirectoryRecordBase()
		{
			if (child) delete child;
			if (sibling) delete sibling;
		}

	void setChild(DirectoryRecordBase *newchild)
		{
			child=newchild;
		}

	TextOutputStream& write(TextOutputStream &log,const TransferSyntax *ts)
		{
			writeSpecific(log);
			indentSpecific(log);
			log << "Length = " << getLength(ts) << endl;
			if (list) {
				AttributeListIterator i(*list);
				while (!i) {
					Attribute *a=i();
					Assert(a);
					indentSpecific(log);
					a->write(log,&staticDictionary,false);
					log << endl;
					++i;
				}
			}

			if (child)     child->write(log,ts);
			if (sibling) sibling->write(log,ts);
			return log;
		}

	Uint32 getLength(const TransferSyntax *ts)	// NOT including leading Item (8) and possible trailing Item Delimitation Item (8)
		{
			Uint32 length=0;
			if (list) {
				AttributeListIterator i(*list);
				while (!i) {
					Attribute *a=i();
					Assert(a);
                                        Uint32 add=lengthOfEntireAttribute(ts,a);
                                        if (add == 0xffffffff) {
                                                Assert(0);              // no unsupported undefined length attributes allowed
                                        }
                                        else {
                                                length+=add;
                                        }
					++i;
				}
			}
			return length;
		}

	void linkOffsetChild(void)
		{
			Attribute *aLowerLevelDirectoryOffset=(*list)[TagFromName(OffsetOfReferencedLowerLevelDirectoryEntity)];
			if (child) {
				Assert(aLowerLevelDirectoryOffset);
				aLowerLevelDirectoryOffset->setValue(0u,(unsigned long)child->getOffset());
				child->linkOffset();
			}
			else if (aLowerLevelDirectoryOffset) {
				aLowerLevelDirectoryOffset->setValue(0u,(unsigned long)0);
			}
			// else must be testing with doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel option ... standard requires LowerLevelDirectoryOffset
		}

	void linkOffsetSibling(void)
		{
			Attribute *aNextDirectoryRecordOffset=(*list)[TagFromName(OffsetOfTheNextDirectoryRecord)];
			Assert(aNextDirectoryRecordOffset);
			if (sibling) {
				aNextDirectoryRecordOffset->setValue(0u,(unsigned long)sibling->getOffset());
				sibling->linkOffset();
			}
			else {
				aNextDirectoryRecordOffset->setValue(0u,(unsigned long)0);
			}
		}

	void linkOffset(void)
		{
			linkOffsetChild();
			linkOffsetSibling();
		}

	Uint32 setOffsetSelf(const TransferSyntax *ts,Uint32 startoffset,Uint32 &lastoffset)
		{
			selfoffset=startoffset;		// save this ... will be needed at top level to locate 1st directory record

			lastoffset=startoffset;

			startoffset+=8;			// Item tag and length
			startoffset+=getLength(ts);	// Ourself
			startoffset+=8;			// Item delimiter tag and length

			return startoffset;
		}

	Uint32 setOffsetChild(const TransferSyntax *ts,Uint32 startoffset,TextOutputStream &log,bool verbose,Uint32 &lastoffset,DirectoryRecordWriteOrder order)
		{
			if (child) {
				startoffset=child->setOffset(ts,startoffset,log,verbose,lastoffset,order);
			}

			return startoffset;
		}

	Uint32 setOffsetSibling(const TransferSyntax *ts,Uint32 startoffset,TextOutputStream &log,bool verbose,Uint32 &lastoffset,DirectoryRecordWriteOrder order)
		{
			if (sibling) {
				startoffset=sibling->setOffset(ts,startoffset,log,verbose,lastoffset,order);
			}

			return startoffset;
		}

	Uint32 setOffset(const TransferSyntax *ts,Uint32 startoffset,TextOutputStream &log,bool verbose,Uint32 &lastoffset,DirectoryRecordWriteOrder order)
		{
			if (verbose) {
				writeSpecific(log);
				indentSpecific(log);
				log << "Start offset = " << startoffset << endl;
			}

			if (order.getOrder() == DirectoryRecordWriteOrder::ParentChildSibling) {
				startoffset=setOffsetSelf   (ts,startoffset,lastoffset);
				startoffset=setOffsetChild  (ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetSibling(ts,startoffset,log,verbose,lastoffset,order);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::ParentSiblingChild) {
				startoffset=setOffsetSelf   (ts,startoffset,lastoffset);
				startoffset=setOffsetSibling(ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetChild  (ts,startoffset,log,verbose,lastoffset,order);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::SiblingChildParent) {
				startoffset=setOffsetSibling(ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetChild  (ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetSelf   (ts,startoffset,lastoffset);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::SiblingParentChild) {
				startoffset=setOffsetSibling(ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetSelf   (ts,startoffset,lastoffset);
				startoffset=setOffsetChild  (ts,startoffset,log,verbose,lastoffset,order);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::ChildSiblingParent) {
				startoffset=setOffsetChild  (ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetSibling(ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetSelf   (ts,startoffset,lastoffset);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::ChildParentSibling) {
				startoffset=setOffsetChild  (ts,startoffset,log,verbose,lastoffset,order);
				startoffset=setOffsetSelf   (ts,startoffset,lastoffset);
				startoffset=setOffsetSibling(ts,startoffset,log,verbose,lastoffset,order);
			}
			else {
				Assert(0);
			}

			return startoffset;
		}

	Uint32 getOffset(void) { return selfoffset; }

	void flatten(SequenceAttribute *aDirectoryRecordSequence,DirectoryRecordWriteOrder order)
		{
			if (order.getOrder() == DirectoryRecordWriteOrder::ParentChildSibling) {
				(*aDirectoryRecordSequence)+=list;
				if (child)     child->flatten(aDirectoryRecordSequence,order);
				if (sibling) sibling->flatten(aDirectoryRecordSequence,order);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::ParentSiblingChild) {
				(*aDirectoryRecordSequence)+=list;
				if (sibling) sibling->flatten(aDirectoryRecordSequence,order);
				if (child)     child->flatten(aDirectoryRecordSequence,order);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::SiblingChildParent) {
				if (sibling) sibling->flatten(aDirectoryRecordSequence,order);
				if (child)     child->flatten(aDirectoryRecordSequence,order);
				(*aDirectoryRecordSequence)+=list;
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::SiblingParentChild) {
				if (sibling) sibling->flatten(aDirectoryRecordSequence,order);
				(*aDirectoryRecordSequence)+=list;
				if (child)     child->flatten(aDirectoryRecordSequence,order);
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::ChildSiblingParent) {
				if (child)     child->flatten(aDirectoryRecordSequence,order);
				if (sibling) sibling->flatten(aDirectoryRecordSequence,order);
				(*aDirectoryRecordSequence)+=list;
			}
			else if (order.getOrder() == DirectoryRecordWriteOrder::ChildParentSibling) {
				if (child)     child->flatten(aDirectoryRecordSequence,order);
				(*aDirectoryRecordSequence)+=list;
				if (sibling) sibling->flatten(aDirectoryRecordSequence,order);
			}
			else {
				Assert(0);
			}
		}
};

class InstanceDirectoryRecord : public DirectoryRecordBase {
	const char *sopInstanceUID;
	const char *sopClassUID;
	const char *instanceNumber;

	TextOutputStream& indentSpecific(TextOutputStream &log)
		{
			log << "\t\t\t";
			return log;
		}

	TextOutputStream& writeSpecific(TextOutputStream &log)
		{
			indentSpecific(log);
			log << "Instance "
			    << (sopInstanceUID ? sopInstanceUID : "") << " "
			    << (instanceNumber ? instanceNumber : "")
			    << endl;
			return log;
		}
public:
	InstanceDirectoryRecord(const char *uid,AttributeList *srclist,InstanceDirectoryRecord *nextsib,
			const char *filename,const char *inputTransferSyntaxUID,
			const TransferSyntax *outputTransferSyntax,
			bool doicon,bool doposition,bool validatefilenamevr,bool doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel,TextOutputStream &log)
		: DirectoryRecordBase(nextsib,srclist)
		{
			if (!doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel) {
				(*list)-=TagFromName(OffsetOfReferencedLowerLevelDirectoryEntity);		// do not send it rather than sending 0 as per standard (for creating tests to simulate Kodak Algotec bug)
			}
		
			sopInstanceUID=uid;
			Assert(sopInstanceUID);
			(*list)+=new UIStringAttribute(TagFromName(ReferencedSOPInstanceUIDInFile),sopInstanceUID);

			sopClassUID=getStringValueElseError((*srclist),TagFromName(SOPClassUID),"SOP Class UID",filename,log);
			Assert(sopClassUID);
			(*list)+=new UIStringAttribute(TagFromName(ReferencedSOPClassUIDInFile),sopClassUID);

			Assert(inputTransferSyntaxUID);
			(*list)+=new UIStringAttribute(TagFromName(ReferencedTransferSyntaxUIDInFile),inputTransferSyntaxUID);

			const char *directoryRecordType = getDirectoryRecordTypeFromUID(sopClassUID);
			if (!directoryRecordType) directoryRecordType="IMAGE";	// Assume image if unrecognzied SOP Class

			(*list)+=new CodeStringAttribute(TagFromName(DirectoryRecordType),directoryRecordType);

			{
				Assert(filename);
				Attribute *a=new CodeStringFileComponentAttribute(TagFromName(ReferencedFileID),filename);
				(*list)+=a;
				Assert(a);
				if (validatefilenamevr) a->validateVR(log,NULL,&staticDictionary);
			}

			if (strcmp(directoryRecordType,"IMAGE") == 0) {
				instanceNumber=getStringValueElseWarningAndDefault((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log,"0");
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				if (doicon) {
					Attribute *aIconImageSequence = makeIconImageSequence(srclist,outputTransferSyntax);
					if (aIconImageSequence) (*list)+=aIconImageSequence;
				}

                                // The following are not required in the IOD, but some profiles ...

                                // ImageType (XABC-CD,XA1K-CD:1)

			        Attribute *imageType=getCopyOfMultiValuedCodeStringAttributeElseNull((*srclist),TagFromName(ImageType));
			        if (imageType) (*list)+=imageType;

                                // CalibrationImage (XABC-CD,XA1K-CD:2)

				const char *calibrationImage=getStringValueElseZeroLength((*srclist),TagFromName(CalibrationImage));
				Assert(calibrationImage);
				(*list)+=new CodeStringAttribute(TagFromName(CalibrationImage),calibrationImage);

                                // ReferencedImageSequence (XABC-CD,XA1K-CD:1C if ImageType value 3 is BIPLANE A or B)
                                // ReferencedImageSequence (CTMR:1C if present in image)

			        Attribute *referencedImageSequence=getCopyOfReferencedImageSequenceAttributeElseNull((*srclist),TagFromName(ReferencedImageSequence),"Referenced Image Sequence",filename,log);
			        if (referencedImageSequence) (*list)+=referencedImageSequence;

                                // The Image Plane Module attributes are all (CTMR:1C if present in image)
                                // (Rows and Columns are actually 1 since always present)
				if (doposition) {
					// ReferencedImageSequence already included above
					
					Attribute *a;
					
					a=getCopyOfMultiValuedDecimalStringAttributeElseNull((*srclist),TagFromName(ImagePositionPatient));
					if (a) { (*list)+=a; }

					a=getCopyOfMultiValuedDecimalStringAttributeElseNull((*srclist),TagFromName(ImageOrientationPatient));
					if (a) { (*list)+=a; }	

					a=(*srclist)[TagFromName(FrameOfReferenceUID)];
					if (a) { (*srclist)-=a; (*list)+=a; }			// Do it safely safe in the absence of reference counting ...

					a=(*srclist)[TagFromName(TableHeight)];
					if (a) { (*srclist)-=a; (*list)+=a; }

					a=(*srclist)[TagFromName(Rows)];
					if (a) { (*srclist)-=a; (*list)+=a; }

					a=(*srclist)[TagFromName(Columns)];
					if (a) { (*srclist)-=a; (*list)+=a; }

					a=getCopyOfMultiValuedDecimalStringAttributeElseNull((*srclist),TagFromName(PixelSpacing));
					if (a) { (*list)+=a; }
				}
                                // The following might hypothetically be required in a future enhanced CT/MR object profile ...

				if (sopClassUID && (strcmp(sopClassUID,EnhancedMRImageStorageSOPClassUID) == 0 || strcmp(sopClassUID,EnhancedCTImageStorageSOPClassUID) == 0)) {
					const char *acquisitionDateTime=getStringValueElseNull((*srclist),TagFromName(AcquisitionDateTime));
					if (acquisitionDateTime)
						(*list)+=new DateTimeStringAttribute(TagFromName(AcquisitionDateTime),acquisitionDateTime);

					const char *volumetricProperties=getStringValueElseNull((*srclist),TagFromName(VolumetricProperties));
					if (volumetricProperties)
						(*list)+=new CodeStringAttribute(TagFromName(VolumetricProperties),volumetricProperties);

					const char *volumeBasedCalculationTechnique=getStringValueElseNull((*srclist),TagFromName(VolumeBasedCalculationTechnique));
					if (volumeBasedCalculationTechnique)
						(*list)+=new CodeStringAttribute(TagFromName(VolumeBasedCalculationTechnique),volumeBasedCalculationTechnique);

					const char *complexImageComponent=getStringValueElseNull((*srclist),TagFromName(ComplexImageComponent));
					if (complexImageComponent)
						(*list)+=new CodeStringAttribute(TagFromName(ComplexImageComponent),complexImageComponent);

					const char *acquisitionContrast=getStringValueElseNull((*srclist),TagFromName(AcquisitionContrast));
					if (acquisitionContrast)
						(*list)+=new CodeStringAttribute(TagFromName(AcquisitionContrast),acquisitionContrast);

					const char *pixelPresentation=getStringValueElseNull((*srclist),TagFromName(PixelPresentation));
					if (pixelPresentation)
						(*list)+=new CodeStringAttribute(TagFromName(PixelPresentation),pixelPresentation);
					
					if (doposition) {
						// check and see if ImageOrientationPatient is shared and if so include it
						SequenceAttribute *aSharedFunctionalGroupsSequence=(SequenceAttribute *)((*srclist)[TagFromName(SharedFunctionalGroupsSequence)]);
						if (aSharedFunctionalGroupsSequence) {
							AttributeList **lSharedFunctionalGroupsSequence;
							int countSharedFunctionalGroupsSequenceItems = aSharedFunctionalGroupsSequence->getLists(&lSharedFunctionalGroupsSequence);
							if (lSharedFunctionalGroupsSequence && countSharedFunctionalGroupsSequenceItems == 1) {
								AttributeList *iSharedFunctionalGroupsSequence = lSharedFunctionalGroupsSequence[0];
								if (iSharedFunctionalGroupsSequence) {
									SequenceAttribute *aPlaneOrientationSequence=(SequenceAttribute *)
										((*iSharedFunctionalGroupsSequence)[TagFromName(PlaneOrientationSequence)]);
									if (aPlaneOrientationSequence) {
										AttributeList **lPlaneOrientationSequence;
										int countPlaneOrientationSequenceItems = aPlaneOrientationSequence->getLists(&lPlaneOrientationSequence);
										if (lPlaneOrientationSequence && countPlaneOrientationSequenceItems == 1) {
											AttributeList *iPlaneOrientationSequence = lPlaneOrientationSequence[0];
											if (iPlaneOrientationSequence) {
												Attribute *aImageOrientationPatient=(*iPlaneOrientationSequence)[TagFromName(ImageOrientationPatient)];
												if (aImageOrientationPatient) {
													(*iPlaneOrientationSequence)-=aImageOrientationPatient; (*list)+=aImageOrientationPatient;
												}
											}
										}
									}
								}
							}
						}
					}
				}
				
				// regardless of SOP Class, if concatenation stuff is present, include it ...
				{
					const char *concatenationUID=getStringValueElseNull((*srclist),TagFromName(ConcatenationUID));
					Uint16 inConcatenationNumber=AttributeValue((*srclist)[TagFromName(InConcatenationNumber)]);
					Uint32 concatenationFrameOffsetNumber=AttributeValue((*srclist)[TagFromName(ConcatenationFrameOffsetNumber)]);
					if (concatenationUID) {
						(*list)+=new UIStringAttribute(TagFromName(ConcatenationUID),concatenationUID);
						(*list)+=new UnsignedShortAttribute(TagFromName(InConcatenationNumber),inConcatenationNumber);
						(*list)+=new UnsignedLongAttribute(TagFromName(ConcatenationFrameOffsetNumber),concatenationFrameOffsetNumber);
					}
				}
				
				// regardless of SOP Class, add number of frames only if present in source ...
				{
					Attribute *aNumberOfFrames = (*srclist)[TagFromName(NumberOfFrames)];
					if (aNumberOfFrames) {
						Uint16 vNumberOfFrames = AttributeValue(aNumberOfFrames);
						(*list)+=new IntegerStringAttribute(TagFromName(NumberOfFrames),vNumberOfFrames);
					}
				}
				
				// Optional but potentially useful ...
			
				const char *imageComments=getStringValueElseNull((*srclist),TagFromName(ImageComments));
				if (imageComments && strlen(imageComments) > 0)
					(*list)+=new LongTextAttribute(TagFromName(ImageComments),imageComments);

				const char *photometricInterpretation=getStringValueElseNull((*srclist),TagFromName(PhotometricInterpretation));
				if (photometricInterpretation)
					(*list)+=new CodeStringAttribute(TagFromName(PhotometricInterpretation),photometricInterpretation);
				
			}
			else if (strcmp(directoryRecordType,"SR DOCUMENT") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *completionFlag=getStringValueElseError((*srclist),TagFromName(CompletionFlag),"Completion Flag",filename,log);
				Assert(completionFlag);
				(*list)+=new CodeStringAttribute(TagFromName(CompletionFlag),completionFlag);

				const char *verificationFlag=getStringValueElseError((*srclist),TagFromName(VerificationFlag),"Verification Flag",filename,log);
				Assert(verificationFlag);
				(*list)+=new CodeStringAttribute(TagFromName(VerificationFlag),verificationFlag);

				if (strcmp(verificationFlag,"VERIFIED") == 0) {
					Attribute *aVerifyingObserverSequence = (*srclist)[TagFromName(VerifyingObserverSequence)];
					if (aVerifyingObserverSequence->isSequence()) {
						AttributeList **items;
						int itemCount = aVerifyingObserverSequence->getLists(&items);
						if (itemCount && items) {
							AttributeList *itemList=*items;
							const char *verificationDateTime=getStringValueElseError((*itemList),TagFromName(VerificationDateTime),"Verification DateTime",filename,log);
							Assert(verificationDateTime);
							(*list)+=new DateTimeStringAttribute(TagFromName(VerificationDateTime),verificationDateTime);
						}
						else {
							log << filename << ": " << EMsgDC(EmptyAttribute) << " - " << "Verifying Observer Sequence (for Verification DateTime)" << endl;
						}
					}
					else {
						log << filename << ": " << EMsgDC(MissingAttribute) << " - " << "Verifying Observer Sequence (for Verification DateTime)" << endl;
					}
				}

				const char *contentDate=getStringValueElseError((*srclist),TagFromName(ContentDate),"Content Date",filename,log);
				Assert(contentDate);
				(*list)+=new DateStringAttribute(TagFromName(ContentDate),contentDate);

				const char *contentTime=getStringValueElseError((*srclist),TagFromName(ContentTime),"Content Time",filename,log);
				Assert(contentTime);
				(*list)+=new TimeStringAttribute(TagFromName(ContentTime),contentTime);

				Attribute *conceptNameCodeSequence=getCopyOfCodeSequenceAttributeElseError((*srclist),TagFromName(ConceptNameCodeSequence),"Concept Name Code Sequence",filename,log);
				Assert(conceptNameCodeSequence);
				(*list)+=conceptNameCodeSequence;

				// Should also do Content Sequence with HAS CONCEPT MOD Relationship Type :(
			}
			else if (strcmp(directoryRecordType,"KEY OBJECT DOC") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *contentDate=getStringValueElseError((*srclist),TagFromName(ContentDate),"Content Date",filename,log);
				Assert(contentDate);
				(*list)+=new DateStringAttribute(TagFromName(ContentDate),contentDate);

				const char *contentTime=getStringValueElseError((*srclist),TagFromName(ContentTime),"Content Time",filename,log);
				Assert(contentTime);
				(*list)+=new TimeStringAttribute(TagFromName(ContentTime),contentTime);

				Attribute *conceptNameCodeSequence=getCopyOfCodeSequenceAttributeElseError((*srclist),TagFromName(ConceptNameCodeSequence),"Concept Name Code Sequence",filename,log);
				Assert(conceptNameCodeSequence);
				(*list)+=conceptNameCodeSequence;

				// Should also do Content Sequence with HAS CONCEPT MOD Relationship Type :(
			}
			else if (strcmp(directoryRecordType,"ENCAP DOC") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *contentDate=getStringValueElseZeroLength((*srclist),TagFromName(ContentDate));
				Assert(contentDate);
				(*list)+=new DateStringAttribute(TagFromName(ContentDate),contentDate);

				const char *contentTime=getStringValueElseZeroLength((*srclist),TagFromName(ContentTime));
				Assert(contentTime);
				(*list)+=new TimeStringAttribute(TagFromName(ContentTime),contentTime);

				const char *documentTitle=getStringValueElseZeroLength((*srclist),TagFromName(DocumentTitle));
				Assert(documentTitle);
				(*list)+=new ShortTextAttribute(TagFromName(DocumentTitle),documentTitle);
				
				const char *hl7InstanceIdentifier=getStringValueElseNull((*srclist),TagFromName(HL7InstanceIdentifier));
				if (hl7InstanceIdentifier) {
					(*list)+=new ShortTextAttribute(TagFromName(HL7InstanceIdentifier),hl7InstanceIdentifier);
				}
				
				const char *mimeTypeOfEncapsulatedDocument=getStringValueElseError((*srclist),TagFromName(MIMETypeOfEncapsulatedDocument),"MIME Type of Encapsulated Document",filename,log);
				Assert(mimeTypeOfEncapsulatedDocument);
				(*list)+=new LongStringAttribute(TagFromName(MIMETypeOfEncapsulatedDocument),mimeTypeOfEncapsulatedDocument);

				Attribute *conceptNameCodeSequence=getCopyOfCodeSequenceAttributeElseEmpty((*srclist),TagFromName(ConceptNameCodeSequence),"Concept Name Code Sequence",filename,log);
				Assert(conceptNameCodeSequence);
				(*list)+=conceptNameCodeSequence;
			}
			else if (strcmp(directoryRecordType,"PRESENTATION") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *contentLabel=getStringValueElseError((*srclist),TagFromName(ContentLabel),"Content Label",filename,log);
				Assert(contentLabel);
				(*list)+=new CodeStringAttribute(TagFromName(ContentLabel),contentLabel);

				const char *contentDescription=getStringValueElseZeroLength((*srclist),TagFromName(ContentDescription));
				Assert(contentDescription);
				(*list)+=new LongStringAttribute(TagFromName(ContentDescription),contentDescription);

				const char *presentationCreationDate=getStringValueElseError((*srclist),TagFromName(PresentationCreationDate),"Presentation Creation Date",filename,log);
				Assert(presentationCreationDate);
				(*list)+=new DateStringAttribute(TagFromName(PresentationCreationDate),presentationCreationDate);

				const char *presentationCreationTime=getStringValueElseError((*srclist),TagFromName(PresentationCreationTime),"Presentation Creation Time",filename,log);
				Assert(presentationCreationTime);
				(*list)+=new TimeStringAttribute(TagFromName(PresentationCreationTime),presentationCreationTime);

				const char *contentCreatorName=getStringValueElseZeroLength((*srclist),TagFromName(ContentCreatorName));
				Assert(contentCreatorName);
				(*list)+=new PersonNameAttribute(TagFromName(ContentCreatorName),contentCreatorName);

				if (isAttributePresentElseNull((*srclist),TagFromName(ReferencedSeriesSequence))) {
					Attribute *referencedSeriesSequence=getCopyOfReferencedSeriesSequenceAttributeElseError((*srclist),TagFromName(ReferencedSeriesSequence),"Referenced Series Sequence",filename,log);
					Assert(referencedSeriesSequence);
					(*list)+=referencedSeriesSequence;
				}
				
				if (isAttributePresentElseNull((*srclist),TagFromName(BlendingSequence))) {
					Attribute *blendingSequence=getCopyOfSequenceContainingStudySeriesImageReferenceElseError((*srclist),TagFromName(BlendingSequence),"Blending Sequence",filename,log);
					Assert(blendingSequence);
					(*list)+=blendingSequence;
				}
			}
			else if (strcmp(directoryRecordType,"REGISTRATION") == 0 || strcmp(directoryRecordType,"FIDUCIAL") == 0 || strcmp(directoryRecordType,"VALUE MAP") == 0) {
				const char *contentDate=getStringValueElseError((*srclist),TagFromName(ContentDate),"Content Date",filename,log);
				Assert(contentDate);
				(*list)+=new DateStringAttribute(TagFromName(ContentDate),contentDate);

				const char *contentTime=getStringValueElseError((*srclist),TagFromName(ContentTime),"Content Time",filename,log);
				Assert(contentTime);
				(*list)+=new TimeStringAttribute(TagFromName(ContentTime),contentTime);

				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *contentLabel=getStringValueElseError((*srclist),TagFromName(ContentLabel),"Content Label",filename,log);
				Assert(contentLabel);
				(*list)+=new CodeStringAttribute(TagFromName(ContentLabel),contentLabel);

				const char *contentDescription=getStringValueElseZeroLength((*srclist),TagFromName(ContentDescription));
				Assert(contentDescription);
				(*list)+=new LongStringAttribute(TagFromName(ContentDescription),contentDescription);

				const char *contentCreatorName=getStringValueElseZeroLength((*srclist),TagFromName(ContentCreatorName));
				Assert(contentCreatorName);
				(*list)+=new PersonNameAttribute(TagFromName(ContentCreatorName),contentCreatorName);
			}
			else if (strcmp(directoryRecordType,"STEREOMETRIC") == 0) {
				// nothing actually required
			}
			else if (strcmp(directoryRecordType,"SPECTROSCOPY") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *contentDate=getStringValueElseError((*srclist),TagFromName(ContentDate),"Content Date",filename,log);
				Assert(contentDate);
				(*list)+=new DateStringAttribute(TagFromName(ContentDate),contentDate);

				const char *contentTime=getStringValueElseError((*srclist),TagFromName(ContentTime),"Content Time",filename,log);
				Assert(contentTime);
				(*list)+=new TimeStringAttribute(TagFromName(ContentTime),contentTime);

			        Attribute *imageType=getCopyOfMultiValuedCodeStringAttributeElseNull((*srclist),TagFromName(ImageType));
			        if (imageType) (*list)+=imageType;

				{
					Attribute *a;
					a=(*srclist)[TagFromName(Rows)];
					if (a) { (*srclist)-=a; (*list)+=a; }

					a=(*srclist)[TagFromName(Columns)];
					if (a) { (*srclist)-=a; (*list)+=a; }
					
					a=(*srclist)[TagFromName(DataPointRows)];
					if (a) { (*srclist)-=a; (*list)+=a; }

					a=(*srclist)[TagFromName(DataPointColumns)];
					if (a) { (*srclist)-=a; (*list)+=a; }
				}
				
				// add number of frames only if present in source (should always be) ...
				{
					Attribute *aNumberOfFrames = (*srclist)[TagFromName(NumberOfFrames)];
					if (aNumberOfFrames) {
						Uint16 vNumberOfFrames = AttributeValue(aNumberOfFrames);
						(*list)+=new IntegerStringAttribute(TagFromName(NumberOfFrames),vNumberOfFrames);
					}
				}
				
				// Optional but potentially useful ...
			
				const char *imageComments=getStringValueElseNull((*srclist),TagFromName(ImageComments));
				if (imageComments && strlen(imageComments) > 0)
					(*list)+=new LongTextAttribute(TagFromName(ImageComments),imageComments);
				
                        }
			else if (strcmp(directoryRecordType,"RAW DATA") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *contentDate=getStringValueElseError((*srclist),TagFromName(ContentDate),"Content Date",filename,log);
				Assert(contentDate);
				(*list)+=new DateStringAttribute(TagFromName(ContentDate),contentDate);

				const char *contentTime=getStringValueElseError((*srclist),TagFromName(ContentTime),"Content Time",filename,log);
				Assert(contentTime);
				(*list)+=new TimeStringAttribute(TagFromName(ContentTime),contentTime);
                        }
			else if (strcmp(directoryRecordType,"WAVEFORM") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *contentDate=getStringValueElseError((*srclist),TagFromName(ContentDate),"Content Date",filename,log);
				Assert(contentDate);
				(*list)+=new DateStringAttribute(TagFromName(ContentDate),contentDate);

				const char *contentTime=getStringValueElseError((*srclist),TagFromName(ContentTime),"Content Time",filename,log);
				Assert(contentTime);
				(*list)+=new TimeStringAttribute(TagFromName(ContentTime),contentTime);
                        }
			else if (strcmp(directoryRecordType,"OVERLAY") == 0) {
				const char *overlayNumber=getStringValueElseError((*srclist),TagFromName(OverlayNumber),"Overlay Number",filename,log);
				Assert(overlayNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(OverlayNumber),overlayNumber);
                        }
			else if (strcmp(directoryRecordType,"MODALITY LUT") == 0 || strcmp(directoryRecordType,"MODALITY LUT") == 0) {
				const char *lutNumber=getStringValueElseError((*srclist),TagFromName(LUTNumber),"LUT Number",filename,log);
				Assert(lutNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(LUTNumber),lutNumber);
                        }
			else if (strcmp(directoryRecordType,"CURVE") == 0) {
				const char *curveNumber=getStringValueElseError((*srclist),TagFromName(CurveNumber),"Curve Number",filename,log);
				Assert(curveNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(CurveNumber),curveNumber);
                        }
			else if (strcmp(directoryRecordType,"STORED PRINT") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);
                        }
			else if (strcmp(directoryRecordType,"RT DOSE") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *doseSummationType=getStringValueElseError((*srclist),TagFromName(DoseSummationType),"Dose Summation Type",filename,log);
				Assert(doseSummationType);
				(*list)+=new CodeStringAttribute(TagFromName(DoseSummationType),doseSummationType);

                                const char *doseComment=getStringValueElseNull((*srclist),TagFromName(DoseComment));
                                if (doseComment) (*list)+=new LongStringAttribute(TagFromName(DoseComment),doseComment);
			}
			else if (strcmp(directoryRecordType,"RT STRUCTURE SET") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *structureSetLabel=getStringValueElseError((*srclist),TagFromName(StructureSetLabel),"Structure Set Label",filename,log);
				Assert(structureSetLabel);
				(*list)+=new CodeStringAttribute(TagFromName(StructureSetLabel),structureSetLabel);

				const char *structureSetDate=getStringValueElseZeroLength((*srclist),TagFromName(StructureSetDate));
				Assert(structureSetDate);
				(*list)+=new DateStringAttribute(TagFromName(StructureSetDate),structureSetDate);

				const char *structureSetTime=getStringValueElseZeroLength((*srclist),TagFromName(StructureSetTime));
				Assert(structureSetTime);
				(*list)+=new TimeStringAttribute(TagFromName(StructureSetTime),structureSetTime);
			}
			else if (strcmp(directoryRecordType,"RT PLAN") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *rtPlanLabel=getStringValueElseError((*srclist),TagFromName(RTPlanLabel),"RT Plan Label",filename,log);
				Assert(rtPlanLabel);
				(*list)+=new CodeStringAttribute(TagFromName(RTPlanLabel),rtPlanLabel);

				const char *rtPlanDate=getStringValueElseZeroLength((*srclist),TagFromName(RTPlanDate));
				Assert(rtPlanDate);
				(*list)+=new DateStringAttribute(TagFromName(RTPlanDate),rtPlanDate);

				const char *rtPlanTime=getStringValueElseZeroLength((*srclist),TagFromName(RTPlanTime));
				Assert(rtPlanTime);
				(*list)+=new TimeStringAttribute(TagFromName(RTPlanTime),rtPlanTime);
			}
			else if (strcmp(directoryRecordType,"RT TREAT RECORD") == 0) {
				instanceNumber=getStringValueElseError((*srclist),TagFromName(InstanceNumber),"Instance Number",filename,log);
				Assert(instanceNumber);
				(*list)+=new IntegerStringAttribute(TagFromName(InstanceNumber),instanceNumber);

				const char *treatmentDate=getStringValueElseZeroLength((*srclist),TagFromName(TreatmentDate));
				Assert(treatmentDate);
				(*list)+=new DateStringAttribute(TagFromName(TreatmentDate),treatmentDate);

				const char *treatmentTime=getStringValueElseZeroLength((*srclist),TagFromName(TreatmentTime));
				Assert(treatmentTime);
				(*list)+=new TimeStringAttribute(TagFromName(TreatmentTime),treatmentTime);
			}
		}

	~InstanceDirectoryRecord()
		{
			if (sopInstanceUID) delete[] sopInstanceUID;
			if (instanceNumber)    delete[] instanceNumber;
		}

	InstanceDirectoryRecord *getSibling()	{ return (InstanceDirectoryRecord *)sibling; }

	const char *getSOPInstanceUID()		{ return sopInstanceUID; }
	const char *getInstanceNumber()		{ return instanceNumber; }

	bool matches(const char *uid)
		{
			Assert(uid);
			Assert(sopInstanceUID);
			return strcmp(uid,sopInstanceUID) == 0 ? true : false;
		}
};

class SeriesDirectoryRecord : public DirectoryRecordBase {
	const char *seriesInstanceUID;
	const char *seriesNumber;
	const char *modality;

	TextOutputStream& indentSpecific(TextOutputStream &log)
		{
			log << "\t\t";
			return log;
		}

	TextOutputStream& writeSpecific(TextOutputStream &log)
		{
			indentSpecific(log);
			log << "Series "
			    << (seriesInstanceUID ? seriesInstanceUID : "") << " "
			    << (seriesNumber ? seriesNumber : "")
			    << endl;
			return log;
		}
public:
	SeriesDirectoryRecord(const char *uid,AttributeList *srclist,SeriesDirectoryRecord *nextsib,const char *filename,TextOutputStream &log)
		: DirectoryRecordBase(nextsib,srclist)
		{
			// the following are Type 1 in directory record ...

			seriesInstanceUID=uid;
			Assert(seriesInstanceUID);
			(*list)+=new UIStringAttribute(TagFromName(SeriesInstanceUID),seriesInstanceUID);

			seriesNumber=getStringValueElseWarningAndDefault((*srclist),TagFromName(SeriesNumber),"Series Number",filename,log,"0");
			Assert(seriesNumber);
			(*list)+=new IntegerStringAttribute(TagFromName(SeriesNumber),seriesNumber);

			modality=getStringValueElseWarningAndDefault((*srclist),TagFromName(Modality),"Modality",filename,log,"OT");
			Assert(modality);
			(*list)+=new CodeStringAttribute(TagFromName(Modality),modality);

			(*list)+=new CodeStringAttribute(TagFromName(DirectoryRecordType),"SERIES");

                        // The following are not required in the IOD, but some profiles ...

                        // InstitutionName (XABC-CD,XA1K-CD:2)

			const char *institutionName=getStringValueElseZeroLength((*srclist),TagFromName(InstitutionName));
			Assert(institutionName);
			(*list)+=new LongStringAttribute(TagFromName(InstitutionName),institutionName);

                        // InstitutionAddress (XABC-CD,XA1K-CD:2)

			const char *institutionAddress=getStringValueElseZeroLength((*srclist),TagFromName(InstitutionAddress));
			Assert(institutionAddress);
			(*list)+=new ShortTextAttribute(TagFromName(InstitutionAddress),institutionAddress);

                        // PerformingPhysicianName (XABC-CD,XA1K-CD:2)
                        // should really check for and copy multiple values :(

			const char *performingPhysicianName=getStringValueElseZeroLength((*srclist),TagFromName(PerformingPhysicianName));
			Assert(performingPhysicianName);
			(*list)+=new PersonNameAttribute(TagFromName(PerformingPhysicianName),performingPhysicianName);
			
			// Optional but potentially useful ...
			
			const char *seriesDescription=getStringValueElseNull((*srclist),TagFromName(SeriesDescription));
			if (seriesDescription && strlen(seriesDescription) > 0)
				(*list)+=new LongStringAttribute(TagFromName(SeriesDescription),seriesDescription);
				
			const char *bodyPartExamined=getStringValueElseNull((*srclist),TagFromName(BodyPartExamined));
			if (bodyPartExamined && strlen(bodyPartExamined) > 0)
				(*list)+=new CodeStringAttribute(TagFromName(BodyPartExamined),bodyPartExamined);
				
			const char *laterality=getStringValueElseNull((*srclist),TagFromName(Laterality));
			if (laterality && strlen(laterality) > 0)
				(*list)+=new CodeStringAttribute(TagFromName(Laterality),laterality);
			
			const char *manufacturer=getStringValueElseNull((*srclist),TagFromName(Manufacturer));
			if (manufacturer && strlen(manufacturer) > 0)
				(*list)+=new LongStringAttribute(TagFromName(Manufacturer),manufacturer);
			
			const char *manufacturerModelName=getStringValueElseNull((*srclist),TagFromName(ManufacturerModelName));
			if (manufacturerModelName && strlen(manufacturerModelName) > 0)
				(*list)+=new LongStringAttribute(TagFromName(ManufacturerModelName),manufacturerModelName);
		}

	~SeriesDirectoryRecord()
		{
			if (seriesInstanceUID) delete[] seriesInstanceUID;
			if (seriesNumber)      delete[] seriesNumber;
			if (modality)          delete[] modality;
		}

	SeriesDirectoryRecord *getSibling()	{ return (SeriesDirectoryRecord *)sibling; }
	InstanceDirectoryRecord   *getChild()	{ return (InstanceDirectoryRecord *)child; }

	const char *getSeriesInstanceUID()	{ return seriesInstanceUID; }
	const char *getSeriesNumber()		{ return seriesNumber; }

	bool matches(const char *uid)
		{
			Assert(uid);
			Assert(seriesInstanceUID);
			return strcmp(uid,seriesInstanceUID) == 0 ? true : false;
		}

	bool checkMatch(AttributeList *srclist,const char *filename,TextOutputStream &log,bool verbose)
		{
			Assert(seriesInstanceUID);
			Assert(seriesNumber);
			Assert(modality);

			if (verbose) log << filename << " matches "
					     << seriesInstanceUID
					     << " "
					     << seriesNumber
					     << endl;

			(void)errorIfStringValuesDontMatch(seriesNumber,*srclist,
				TagFromName(SeriesNumber),"Series Number",
				"Series Instance UID",seriesInstanceUID,filename,log);

			(void)errorIfStringValuesDontMatch(modality,*srclist,
				TagFromName(Modality),"Modality",
				"Series Instance UID",seriesInstanceUID,filename,log);
		}
};

class StudyDirectoryRecord : public DirectoryRecordBase {
	const char *studyInstanceUID;
	const char *studyID;
	const char *studyDate;
	const char *studyTime;
	const char *studyDescription;
	const char *accessionNumber;

	TextOutputStream& indentSpecific(TextOutputStream &log)
		{
			log << "\t";
			return log;
		}

	TextOutputStream& writeSpecific(TextOutputStream &log)
		{
			indentSpecific(log);
			log << "Study "
			    << (studyInstanceUID ? studyInstanceUID : "") << " "
			    << (studyID ? studyID : "")
			    << endl;
			return log;
		}
public:
	StudyDirectoryRecord(const char *uid,AttributeList *srclist,StudyDirectoryRecord *nextsib,const char *filename,TextOutputStream &log)
		: DirectoryRecordBase(nextsib,srclist)
		{
			// the following are Type 1 in directory record ...

			studyInstanceUID=uid;
			Assert(studyInstanceUID);
			(*list)+=new UIStringAttribute(TagFromName(StudyInstanceUID),studyInstanceUID);

			Attribute *a;

			studyDate=getStringValueElseWarningAndDefault((*srclist),TagFromName(StudyDate),"Study Date",filename,log,"19000101");
			Assert(studyDate);
			(*list)+=new DateStringAttribute(TagFromName(StudyDate),studyDate);

			studyTime=getStringValueElseWarningAndDefault((*srclist),TagFromName(StudyTime),"Study Time",filename,log,"000000");
			Assert(studyTime);
			(*list)+=new TimeStringAttribute(TagFromName(StudyTime),studyTime);

			studyID=getStringValueElseWarningAndDefault((*srclist),TagFromName(StudyID),"Study ID",filename,log,studyDate);
			Assert(studyID);
			(*list)+=new ShortStringAttribute(TagFromName(StudyID),studyID);

			// the following are Type 2 in directory record ...

			studyDescription=AttributeValue((*srclist)[TagFromName(StudyDescription)],"");
			Assert(studyDescription);
			(*list)+=new LongStringAttribute(TagFromName(StudyDescription),studyDescription);

			accessionNumber=AttributeValue((*srclist)[TagFromName(AccessionNumber)],"");
			Assert(accessionNumber);
			(*list)+=new ShortStringAttribute(TagFromName(AccessionNumber),accessionNumber);

			(*list)+=new CodeStringAttribute(TagFromName(DirectoryRecordType),"STUDY");
		}

	~StudyDirectoryRecord()
		{
			if (studyInstanceUID) delete[] studyInstanceUID;
			if (studyID)          delete[] studyID;
			if (studyDate)        delete[] studyDate;
			if (studyTime)        delete[] studyTime;
			if (studyDescription) delete[] studyDescription;
			if (accessionNumber)  delete[] accessionNumber;
		}

	StudyDirectoryRecord *getSibling()	{ return (StudyDirectoryRecord *)sibling; }
	SeriesDirectoryRecord   *getChild()	{ return (SeriesDirectoryRecord *)child; }

	const char *getStudyInstanceUID()	{ return studyInstanceUID; }
	const char *getStudyID()		{ return studyID; }

	bool matches(const char *uid)
		{
			Assert(uid);
			Assert(studyInstanceUID);
			return strcmp(uid,studyInstanceUID) == 0 ? true : false;
		}

	bool checkMatch(AttributeList *srclist,const char *filename,TextOutputStream &log,bool verbose)
		{
			Assert(studyInstanceUID);
			Assert(studyID);
			Assert(studyDate);
			Assert(studyTime);
			Assert(studyDescription);
			Assert(accessionNumber);

			if (verbose) log << filename << " matches "
					     << studyInstanceUID
					     << " "
					     << studyID
					     << endl;

			(void)errorIfStringValuesDontMatch(studyID,*srclist,
				TagFromName(StudyID),"Study ID",
				"Study Instance UID",studyInstanceUID,filename,log);

			(void)errorIfStringValuesDontMatch(studyDate,*srclist,
				TagFromName(StudyDate),"Study Date",
				"Study Instance UID",studyInstanceUID,filename,log);

			(void)errorIfStringValuesDontMatch(studyTime,*srclist,
				TagFromName(StudyTime),"Study Time",
				"Study Instance UID",studyInstanceUID,filename,log);

			(void)errorIfStringValuesDontMatch(studyDescription,*srclist,
				TagFromName(StudyDescription),"Study Description",
				"Study Instance UID",studyInstanceUID,filename,log);

			(void)errorIfStringValuesDontMatch(accessionNumber,*srclist,
				TagFromName(AccessionNumber),"Accession Number",
				"Study Instance UID",studyInstanceUID,filename,log);
		}
};

class PatientDirectoryRecord : public DirectoryRecordBase {
	const char *patientID;
	const char *patientName;

	TextOutputStream& indentSpecific(TextOutputStream &log)
		{
			return log;
		}

	TextOutputStream& writeSpecific(TextOutputStream &log)
		{
			indentSpecific(log);
			log << "Patient "
			    << (patientID ? patientID : "") << " "
			    << (patientName ? patientName : "")
			    << endl;
			return log;
		}
public:
	PatientDirectoryRecord(const char *id,const char *name,AttributeList *srclist,PatientDirectoryRecord *nextsib,const char *filename,TextOutputStream &log)
		: DirectoryRecordBase(nextsib,srclist)
		{
			patientID=id;
			Assert(patientID);
			(*list)+=new LongStringAttribute(TagFromName(PatientID),patientID);

			patientName=name;
			Assert(patientName);
			(*list)+=new PersonNameAttribute(TagFromName(PatientName),patientName);

			(*list)+=new CodeStringAttribute(TagFromName(DirectoryRecordType),"PATIENT");

                        // The following are not required in the IOD, but some profiles ...

                        // PatientBirthDate (XABC-CD,XA1K-CD:2)

			const char *patientBirthDate=getStringValueElseZeroLength((*srclist),TagFromName(PatientBirthDate));
			Assert(patientBirthDate);
			(*list)+=new DateStringAttribute(TagFromName(PatientBirthDate),patientBirthDate);

                        // PatientSex (XABC-CD,XA1K-CD:2)

			const char *patientSex=getStringValueElseZeroLength((*srclist),TagFromName(PatientSex));
			Assert(patientSex);
			(*list)+=new CodeStringAttribute(TagFromName(PatientSex),patientSex);
		}

	~PatientDirectoryRecord()
		{
			if (patientID)   delete[] patientID;
			if (patientName) delete[] patientName;
		}

	PatientDirectoryRecord *getSibling()	{ return (PatientDirectoryRecord *)sibling; }
	StudyDirectoryRecord   *getChild()	{ return (StudyDirectoryRecord *)child; }

	const char *getPatientID()	{ return patientID; }
	const char *getPatientName()	{ return patientName; }
	
	static int indexOfLastSignificantPartOfPersonName(const char *name) {
		int index = 0;
		if (name != NULL) {
			int length = strlen(name);
			if (length > 0) {
				const char *endptr = name + length - 1;
				while (endptr >= name && *endptr && (*endptr == 0 || isspace(*endptr) || *endptr == '^')) --endptr;
				index = endptr - name + 1;	// i.e. if empty, then index will be 0
			}
		}
		return index;
	}
	
	static bool compareOnlySignificantPartsOfPersonName(const char *name1,const char *name2) {
//cerr << "PatientDirectoryRecord.compareOnlySignificantPartsOfPersonName(): name1 " << name1 << endl;
//cerr << "PatientDirectoryRecord.compareOnlySignificantPartsOfPersonName(): name2 " << name2 << endl;
		int n = indexOfLastSignificantPartOfPersonName(name1);
//cerr << "PatientDirectoryRecord.compareOnlySignificantPartsOfPersonName(): name1 index " << n << endl;
		int index2 = indexOfLastSignificantPartOfPersonName(name2);
//cerr << "PatientDirectoryRecord.compareOnlySignificantPartsOfPersonName(): name2 index " << index2 << endl;
		if (index2 > n) {
			n=index2;
		}
		return strncmp(name1,name2,n) == 0;	// "compares not more than n characters"
	}

	bool matches(const char *id,const char *name)
		{
			Assert(id);
			Assert(patientID);
			Assert(name);
			Assert(patientName);
			//return strcmp(id,patientID) == 0 && strcmp(name,patientName) == 0 ? true : false;
			return strcmp(id,patientID) == 0 && compareOnlySignificantPartsOfPersonName(name,patientName);
		}

	bool checkMatch(AttributeList *srclist,const char *filename,TextOutputStream &log,bool verbose)
		{
			Assert(patientID);
			Assert(patientName);

			(void)errorIfStringValuesDontMatch(patientName,*srclist,
				TagFromName(PatientName),"Patient Name",
				"Patient ID",patientID,filename,log);

			if (verbose) log << filename << " matches "
					     << patientID
					     << " "
					     << patientName
					     << endl;
		}
};

// Derived from attrmxls.cc ...

static bool
addMetaHeader(AttributeList &list,const TransferSyntax *ts,const char *uidstamp)
{
//cerr << "addMetaHeader" << endl;
	Attribute *a;

	{
		Assert(ts);
		const char *tsuid=ts->getUID();
		Assert(tsuid);
		a=new UIStringAttribute(TagFromName(TransferSyntaxUID),tsuid);
		Assert(a);
		list+=(a);
	}

	{
		a=new OtherByteSmallNonPixelAttribute(TagFromName(FileMetaInformationVersion));
		Assert(a);
		unsigned char values[2];
		values[0]=0x00;
		values[1]=0x01;
		a->setValue(values,2);
		list+=(a);
	}

	{
		a=new UIStringAttribute(TagFromName(MediaStorageSOPClassUID),MediaStorageDirectoryStorageSOPClassUID);
		Assert(a);
		list+=(a);
	}

	{
		a=new UIStringAttribute(TagFromName(MediaStorageSOPInstanceUID),GeneratedDirInstanceUID(uidstamp));
		Assert(a);
		list+=(a);
	}

#ifndef DEFAULTIMPLEMENTATIONCLASSUID
#define DEFAULTIMPLEMENTATIONCLASSUID "0.0.0.0"
#endif
	{
		a=new UIStringAttribute(TagFromName(ImplementationClassUID));
		Assert(a);
		a->addValue(DEFAULTIMPLEMENTATIONCLASSUID);
		list+=(a);
	}

#ifndef DEFAULTIMPLEMENTATIONVERSIONNAME
#define DEFAULTIMPLEMENTATIONVERSIONNAME "NOTSPECIFIED"
#endif
	{
		a=new ShortStringAttribute(TagFromName(ImplementationVersionName));
		Assert(a);
		a->addValue(DEFAULTIMPLEMENTATIONVERSIONNAME);
		list+=(a);
	}

#ifndef DEFAULTSOURCEAPPLICATIONENTITYTITLE
#define DEFAULTSOURCEAPPLICATIONENTITYTITLE "NOTSPECIFIED"
#endif
	{
		a=new ApplicationEntityAttribute(TagFromName(SourceApplicationEntityTitle));
		Assert(a);
		a->addValue(DEFAULTSOURCEAPPLICATIONENTITYTITLE);
		list+=(a);
	}

	return true;
}

static void handleOneFile(const char *filename,
			PatientDirectoryRecord *&headPatient,
			DicomInputOptions &dicom_input_options,
			DicomOutputOptions &dicom_output_options,
			bool listnames,bool success,bool veryverbose,bool veryveryverbose,
			bool doicon,bool doposition,bool dovalidatename,bool doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel,
			TextOutputStream &log) {

		Assert(filename);
		if (listnames) log << "Reading \"" << filename << "\"" << endl;

#ifdef USEBINARYFLAGFORINPUTOPENMODE
		ifstream *fstr=new ifstream(filename,ios::in|ios::binary);
#else
		ifstream *fstr=new ifstream(filename);
#endif
		if (!fstr || !*fstr || !fstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filename) cerr <<" - \"" << filename << "\"";
			success=false;
			return;
		}

		DicomInputStream din(*(istream *)fstr,
			dicom_input_options.transfersyntaxuid,
			dicom_input_options.usemetaheader);

		ManagedAttributeList list;

		if (veryverbose) log << "******** While reading ... " << filename << " ... ********" << endl;
		list.read(din,&log,veryveryverbose,0xffffffff,true,dicom_input_options.uselengthtoend,dicom_input_options.ignoreoutofordertags,dicom_input_options.useUSVRForLUTDataIfNotExplicit);

		if (!list.good()) {
			log << list.errors()
			    << EMsgDC(DatasetReadFailed) << endl;
			success=false;
			return;
		}

		const char *patientID=getStringValueElseWarningAndDefault(list,TagFromName(PatientID),"Patient ID",filename,log,"NOPATIENTID");
		Assert(patientID);

		const char *patientName=getStringValueElseWarningAndDefault(list,TagFromName(PatientName),"Patient Name",filename,log,"Nobody^");
		Assert(patientName);

		PatientDirectoryRecord *ptrPatient=headPatient;
		while (ptrPatient && !ptrPatient->matches(patientID,patientName)) ptrPatient=ptrPatient->getSibling();

		StudyDirectoryRecord *headStudy,*ptrStudy;
		if (ptrPatient) {
			ptrPatient->checkMatch(&list,filename,log,veryverbose);

			ptrStudy=headStudy=ptrPatient->getChild();
		}
		else {
			if (veryverbose) log << filename << " no match for ID <" << patientID << "> and Name <" << patientName << ">" << endl;

			ptrPatient=headPatient=new PatientDirectoryRecord(patientID,patientName,&list,headPatient,filename,log);
			ptrStudy=headStudy=0;
		}

		const char *studyInstanceUID=getStringValueElseError(list,TagFromName(StudyInstanceUID),"Study Instance UID",filename,log);
		Assert(studyInstanceUID);

		while (ptrStudy && !ptrStudy->matches(studyInstanceUID)) ptrStudy=ptrStudy->getSibling();

		SeriesDirectoryRecord *headSeries,*ptrSeries;
		if (ptrStudy) {
			ptrStudy->checkMatch(&list,filename,log,veryverbose);

			ptrSeries=headSeries=ptrStudy->getChild();
		}
		else {
			if (veryverbose) log << filename << " no match for " << studyInstanceUID << endl;

			ptrStudy=headStudy=new StudyDirectoryRecord(studyInstanceUID,&list,headStudy,filename,log);
			Assert(ptrPatient);
			ptrPatient->setChild(headStudy);
			ptrSeries=headSeries=0;
		}

		const char *seriesInstanceUID=getStringValueElseError(list,TagFromName(SeriesInstanceUID),"Series Instance UID",filename,log);
		Assert(seriesInstanceUID);

		while (ptrSeries && !ptrSeries->matches(seriesInstanceUID)) ptrSeries=ptrSeries->getSibling();

		InstanceDirectoryRecord *headInstance,*ptrInstance;
		if (ptrSeries) {
			ptrSeries->checkMatch(&list,filename,log,veryverbose);

			ptrInstance=headInstance=ptrSeries->getChild();
		}
		else {
			if (veryverbose) log << filename << " no match for " << seriesInstanceUID << endl;

			ptrSeries=headSeries=new SeriesDirectoryRecord(seriesInstanceUID,&list,headSeries,filename,log);
			Assert(ptrStudy);
			ptrStudy->setChild(headSeries);
			ptrInstance=headInstance=0;
		}

		const char *sopInstanceUID=getStringValueElseError(list,TagFromName(SOPInstanceUID),"SOP Instance UID",filename,log);
		Assert(sopInstanceUID);

		while (ptrInstance && !ptrInstance->matches(sopInstanceUID)) ptrInstance=ptrInstance->getSibling();

		if (ptrInstance) {
			log << "Error - duplicate SOP instance within Series for file " << filename << " matches "
			    << ptrInstance-> getSOPInstanceUID()
			    << " Instance Number "
			    << (ptrInstance-> getInstanceNumber() ? ptrInstance-> getInstanceNumber() : "")
			    << endl;

			return; // because of error
		}
		else {
			if (veryverbose) log << filename << " no match for " << sopInstanceUID << endl;

			TransferSyntax outts(dicom_output_options.transfersyntaxuid);	// SGI CC doesn't like this inline :(
			ptrInstance=headInstance=new InstanceDirectoryRecord(sopInstanceUID,&list,headInstance,
				filename,
				din.getTransferSyntaxToReadDataSet()->getUID(),
				&outts,
				doicon,doposition,dovalidatename,doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel,log);
			Assert(ptrSeries);
			ptrSeries->setChild(headInstance);
		}

		if (fstr) {
			fstr->close();
			delete fstr;
		}
}

int
main(int argc, char *argv[])
{
	bool bad=false;

	GetNamedOptions 	options(argc,argv);
	DicomInputOptions 	dicom_input_options(options);
	DicomOutputOptions 	dicom_output_options(options);

	bool verbose=options.get("verbose") || options.get("v");
	bool veryverbose=options.get("veryverbose") || options.get("vv");
	bool veryveryverbose=options.get("veryveryverbose") || options.get("vvv");
	if (veryveryverbose) veryverbose=true;
	if (veryverbose) verbose=true;
	bool listnames=options.get("l");

	const char *descriptor=0;
	(void)(options.get("descriptor",descriptor) || options.get("readme",descriptor));

	const char *identifier=0;
	(void)(options.get("identifier",identifier) || options.get("id",identifier));

	bool doicon=!options.get("noicons");
	bool doposition=options.get("position");
	bool dovalidatename=!options.get("novalidatename");
	bool doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel=!options.get("noinstancechildoffset");

	DirectoryRecordWriteOrder order(DirectoryRecordWriteOrder::ParentChildSibling);
	{
		const char *order_arg=0;
		if (options.get("dirrecorder",order_arg)) {
			if (!order_arg || !*order_arg) bad=true;
			else if (strcmp(order_arg,"pcs") == 0) order=DirectoryRecordWriteOrder(DirectoryRecordWriteOrder::ParentChildSibling);
			else if (strcmp(order_arg,"psc") == 0) order=DirectoryRecordWriteOrder(DirectoryRecordWriteOrder::ParentSiblingChild);
			else if (strcmp(order_arg,"scp") == 0) order=DirectoryRecordWriteOrder(DirectoryRecordWriteOrder::SiblingChildParent);
			else if (strcmp(order_arg,"spc") == 0) order=DirectoryRecordWriteOrder(DirectoryRecordWriteOrder::SiblingParentChild);
			else if (strcmp(order_arg,"csp") == 0) order=DirectoryRecordWriteOrder(DirectoryRecordWriteOrder::ChildSiblingParent);
			else if (strcmp(order_arg,"cps") == 0) order=DirectoryRecordWriteOrder(DirectoryRecordWriteOrder::ChildParentSibling);
			else bad=true;
		}
	}

	dicom_input_options.done();
	Assert(!dicom_input_options.filename);

	dicom_output_options.done();

	const char *filelistfile=0;
	(void)(options.get("filelist",filelistfile) || options.get("f",filelistfile));

	int numberofinputfiles=!options;


	const char **listoffilenames = new const char * [numberofinputfiles];
	const char **ptr = listoffilenames;
	const char *filename;

	while(!options && (filename=options())) {
		++options;
		*ptr++=filename;
	}

	options.done();

	DicomOutputOpenerFromOptions output_opener(
		options,dicom_output_options.filename,cout);

	cerr << dicom_input_options.errors();
	cerr << dicom_output_options.errors();
	cerr << options.errors();
	cerr << output_opener.errors();

	if (!dicom_input_options.good()
	 || !dicom_output_options.good()
	 || !options.good()
	 || !output_opener.good()
	 || !options
	 || bad) {
		cerr 	<< MMsgDC(Usage) << ": " << options.command()
			<< dicom_input_options.usage()
			<< dicom_output_options.usage()
			<< " [-v|-verbose|-vv|-veryverbose|-vvv|-veryveryverbose]"
			<< " [-descriptor|-readme filename]"
			<< " [-identifier|-id filesetid]"
			<< " [-noicons]"
			<< " [-position]"
			<< " [-novalidatename]"
			<< " [-dirrecorder pcs|psc|scp|spc|csp|cps]"
			<< " [-noinstancechildoffset]"
			<< " [-f|-filelist filename]"
			<< " " << MMsgDC(InputFile) << " ["<< MMsgDC(InputFile) << " ...]"
			<< " >" << MMsgDC(OutputFile)
			<< endl;
		exit(1);
	}

	if (!dicom_output_options.transfersyntaxuid)
		dicom_output_options.transfersyntaxuid=ExplicitVRLittleEndianTransferSyntaxUID;

	bool success=true;
	TextOutputStream log(cerr);

	PatientDirectoryRecord *headPatient=0;

	int i;
	for (i=0; i < numberofinputfiles; ++i) {
		handleOneFile(listoffilenames[i],headPatient,dicom_input_options,dicom_output_options,
			listnames,success,veryverbose,veryveryverbose,doicon,doposition,dovalidatename,doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel,
			log);
	}

	if (filelistfile) {
		ifstream *flfstr=new ifstream(filelistfile);
		if (!flfstr || !*flfstr || !flfstr->rdbuf()->is_open()) {
			cerr << AMsgDC(FileReadOpenFailed);
			if (filelistfile) cerr <<" - \"" << filename << "\"";
			cerr << endl;
			bad=true;
		}
		else {
			while (flfstr->peek() != EOF) {
				const int lineBufferSize=2048;
				char lineBuffer[lineBufferSize];
				flfstr->getline(lineBuffer,2048);
				if (strlen(lineBuffer))
					handleOneFile(lineBuffer,headPatient,dicom_input_options,dicom_output_options,
						listnames,success,veryverbose,veryveryverbose,doicon,doposition,dovalidatename,doNotIncludeLowerLevelDirectoryOffsetForInstanceLevel,
						log);
				// else skip blank lines
			}
		}
	}

	if (veryverbose && headPatient) {
		TransferSyntax outts(dicom_output_options.transfersyntaxuid);	// SGI CC doesn't like this inline :(
		headPatient->write(log,&outts);
	}

	if (numberofinputfiles && listoffilenames) delete[] listoffilenames;

	if (success) {
		AttributeList list;

		DicomOutputStream dout(*(ostream *)output_opener,
			dicom_output_options.transfersyntaxuid,
			dicom_output_options.usemetaheader,
			dicom_output_options.useimplicitmetaheader,
			dicom_output_options.addtiff);

		if (dicom_output_options.usemetaheader) {
			addMetaHeader(list,dout.getTransferSyntaxToWriteDataSet(),dicom_output_options.stamp);
			(void)addLengths(list,dout.getTransferSyntaxToWriteMetaHeader(),dout.getTransferSyntaxToWriteDataSet());
		}

		if (identifier)
			list+=new CodeStringAttribute(TagFromName(FileSetID),identifier);
		else
			list+=new CodeStringAttribute(TagFromName(FileSetID));

		if (descriptor)
			list+=new CodeStringFileComponentAttribute(TagFromName(FileSetDescriptorFileID),descriptor);

		list+=new UnsignedShortAttribute(TagFromName(FileSetConsistencyFlag),0x0000);

		Uint32 predictedOffset=0;

		if (dicom_output_options.usemetaheader)
			predictedOffset+=132;	// preamble

		AttributeListIterator listi(list);
		while (!listi) {
			Attribute *a = listi();
			Assert(a);
			if (veryverbose) { a->write(log,&staticDictionary); log << endl; }
			predictedOffset+=lengthOfFixedPartOfAttribute(
					dout.getTransferSyntaxToWriteMetaHeader(),
					dout.getTransferSyntaxToWriteDataSet(),a)
						+a->getVL();
			++listi;
		}

		if (veryverbose) log << "Predicted Offset before Root Directory Entity First Directory Record attribute = " << dec << predictedOffset << endl;

		Attribute *aRootDirectoryFirstRecord=new UnsignedLongAttribute(TagFromName(OffsetOfTheFirstDirectoryRecordOfTheRootDirectoryEntity),0);

		predictedOffset+=lengthOfFixedPartOfAttribute(
					dout.getTransferSyntaxToWriteMetaHeader(),
					dout.getTransferSyntaxToWriteDataSet(),aRootDirectoryFirstRecord)
						+aRootDirectoryFirstRecord->getVL();

		Attribute *aRootDirectoryLastRecord=new UnsignedLongAttribute(TagFromName(OffsetOfTheLastDirectoryRecordOfTheRootDirectoryEntity),0);

		predictedOffset+=lengthOfFixedPartOfAttribute(
					dout.getTransferSyntaxToWriteMetaHeader(),
					dout.getTransferSyntaxToWriteDataSet(),aRootDirectoryLastRecord)
						+aRootDirectoryLastRecord->getVL();

		if (veryverbose) log << "Predicted Offset before Directory Record Sequence = " << dec << predictedOffset << endl;

		SequenceAttribute *aDirectoryRecordSequence=new SequenceAttribute(TagFromName(DirectoryRecordSequence));

		predictedOffset+=lengthOfFixedPartOfAttribute(
					dout.getTransferSyntaxToWriteMetaHeader(),
					dout.getTransferSyntaxToWriteDataSet(),aDirectoryRecordSequence);

		if (veryverbose) log << "Predicted Offset before first item = " << dec << predictedOffset << endl;

		if (headPatient) {
			Uint32 lastoffset;

			(void)headPatient->setOffset(
					dout.getTransferSyntaxToWriteDataSet(),predictedOffset,log,veryverbose,lastoffset,order);

			headPatient->linkOffset();

			if (veryverbose) {
				TransferSyntax outts(dicom_output_options.transfersyntaxuid);	// SGI CC doesn't like this inline :(
				headPatient->write(log,&outts);
			}

			aRootDirectoryFirstRecord->setValue(0u,headPatient->getOffset());
			list+=aRootDirectoryFirstRecord;

			aRootDirectoryLastRecord->setValue(0u,lastoffset);
			list+=aRootDirectoryLastRecord;

			headPatient->flatten(aDirectoryRecordSequence,order);
			list+=aDirectoryRecordSequence;
		}

		listi.first();
		while (!listi) {
			Attribute *a = listi();
			Assert(a);
			if (a->getTag().isMetaheaderGroup())
				dout.writingMetaHeader();
			else
				dout.writingDataSet();
			a->write(dout);
			if (verbose) { a->write(log,&staticDictionary); log << endl; }
			++listi;
		}
	}

	return success ? 0 : 1;
}

	





