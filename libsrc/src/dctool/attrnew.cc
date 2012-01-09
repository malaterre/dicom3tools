#include "attrtype.h"
#include "attrnew.h"
#include "attrseq.h"

Attribute *
newAttribute(const char* vr,Tag tag)
{
	Attribute *a = 0;

	if (vr && vr[0]) {
		switch (vr[0]) {
		case 'A':
			switch (vr[1]) {
			case 'E':
				a=new ApplicationEntityAttribute(tag);
				break;
			case 'S':
				a=new AgeStringAttribute(tag);
				break;
			case 'T':
				a=new AttributeTagAttribute(tag);
				break;
			}
			break;
		case 'C':
			switch (vr[1]) {
			case 'S':
				a=new CodeStringAttribute(tag);
				break;
			}
			break;
		case 'D':
			switch (vr[1]) {
			case 'A':
				a=new DateStringAttribute(tag);
				break;
			case 'S':
				a=new DecimalStringAttribute(tag);
				break;
			case 'T':
				a=new DateTimeStringAttribute(tag);
				break;
			}
			break;
		case 'F':
			switch (vr[1]) {
			case 'L':
				a=new FloatSingleAttribute(tag);
				break;
			case 'D':
				a=new FloatDoubleAttribute(tag);
				break;
			default:
				break;
			}
			break;
		case 'I':
			switch (vr[1]) {
			case 'S':
				a=new IntegerStringAttribute(tag);
				break;
			}
			break;
		case 'L':
			switch (vr[1]) {
			case 'O':
				a=new LongStringAttribute(tag);
				break;
			case 'T':
				a=new LongTextAttribute(tag);
				break;
			}
			break;
		case 'O':
			switch (vr[1]) {
			case 'B':
				a=new OtherByteSmallNonPixelAttribute(tag);
				break;
			//case 'F':
			//	a=new OtherFloatSmallAttribute(tag);
			//	break;
			case 'W':
				a=new OtherWordSmallNonPixelAttribute(tag);
				break;
			case 'X':
				// This is probably not cool :( ...
				a=new OtherWordSmallNonPixelAttribute(tag);
				break;
			}
			break;
		case 'P':
			switch (vr[1]) {
			case 'N':
				a=new PersonNameAttribute(tag);
				break;
			}
			break;
		case 'S':
			switch (vr[1]) {
			case 'H':
				a=new ShortStringAttribute(tag);
				break;
			case 'L':
				a=new SignedLongAttribute(tag);
				break;
			case 'Q':
				a=new SequenceAttribute(tag);
				break;
			case 'S':
				a=new SignedShortAttribute(tag);
				break;
			case 'T':
				a=new ShortTextAttribute(tag);
				break;
			}
			break;
		case 'T':
			switch (vr[1]) {
			case 'M':
				a=new TimeStringAttribute(tag);
				break;
			}
			break;
		case 'U':
			switch (vr[1]) {
			case 'I':
				a=new UIStringAttribute(tag);
				break;
			case 'L':
				a=new UnsignedLongAttribute(tag);
				break;
			case 'N':
				a=new UnknownSmallAttribute(tag);
				break;
			case 'S':
				a=new UnsignedShortAttribute(tag);
				break;
			case 'T':
				a=new UnlimitedTextAttribute(tag);
				break;
			}
			break;
		case 'X':
			switch (vr[1]) {
			case 'L':
				a=new UnspecifiedLongAttribute(tag);
				break;
			case 'O':
				a=new OtherWordSmallNonPixelAttribute(tag);
				break;
			case 'S':
				a=new UnspecifiedShortAttribute(tag);
				break;
			}
			break;
		}
	}

	return a;
}

