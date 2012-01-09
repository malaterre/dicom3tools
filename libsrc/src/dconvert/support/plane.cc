#include "plane.h"

AbstractImagePlane::AbstractImagePlane(
	ImagePlanePlane plane,double hfov,double vfov)
{
#ifdef DEBUGPLANE
	cerr << "AbstractImagePlane::AbstractImagePlane() start" << endl;
#endif
	Point3D  o(0,0,0); center=o;
	double dh=hfov/2;
	double dv=vfov/2;
	switch (plane) {
		case Axial:
		case AxialLP:
			// Conventional - viewed "bottom up"
		{
			Point3D  b(-dh,-dv, 0); tlhc=b;
			Vector3D r( 1,  0,  0); rowvec=r;
			Vector3D c( 0,  1,  0); colvec=c;
			break;
		}
		case AxialRP:
			// Old EMI style - viewed "top down"
		{
			Point3D  b( dh,-dv, 0); tlhc=b;
			Vector3D r(-1,  0,  0);  rowvec=r;
			Vector3D c( 0,  1,  0);  colvec=c;
			break;
		}
		case Sagittal:
		case SagittalPF:
			// Conventional MRI viewed "from left"
		{
			Point3D  b( 0,-dh, dv); tlhc=b;
			Vector3D r( 0,  1,  0);  rowvec=r;
			Vector3D c( 0,  0, -1);  colvec=c;
			break;
		}
		case SagittalIP:
			// Conventional CT Lateral Scout
		{
			Point3D  b( 0,-dh, dv); tlhc=b;
			Vector3D r( 0,  0, -1);  rowvec=r;
			Vector3D c( 0,  1,  0);  colvec=c;
			break;
		}
		case Coronal:
		case CoronalLI:
			// Conventional MRI viewed "from front"
			// Conventional CT AP Scout
		{
			Point3D  b(-dh, 0, dv); tlhc=b;
			Vector3D r( 1,  0,  0); rowvec=r;
			Vector3D c( 0,  0, -1); colvec=c;
			break;
		}
	}
#ifdef DEBUGPLANE
	cerr << "AbstractImagePlane::constructed this:" << endl;
	put(cerr);
#endif
}

#ifdef DEBUGPLANE
void
AbstractImagePlane::put(ostream &ostr)
{
	ostr << "AbstractImagePlane::put()" << endl;
	ostr << "\tcenter" << endl;
	center.put(ostr);
	ostr << "\ttlhc" << endl;
	tlhc.put(ostr);
	ostr << "\trowvec" << endl;
	rowvec.put(ostr);
	ostr << "\tcolvec" << endl;
	colvec.put(ostr);
}
#endif

#ifdef DEBUGPLANE
void
DerivedImagePlane::put(ostream &ostr)
{
	ostr << "DerivedImagePlane::put()" << endl;
	ostr << "\trotation" << endl;
	rotation.put(ostr);
	ostr << "\tunrotation" << endl;
	unrotation.put(ostr);
}
#endif

#ifdef DEBUGPLANE
void
PatientPlane::put(ostream &ostr)
{
	ostr << "PatientPlane::put()" << endl;
	DerivedImagePlane::put(ostr);
}
#endif

#ifdef DEBUGPLANE
void
MachinePlane::put(ostream &ostr)
{
	ostr << "MachinePlane::put()" << endl;
	DerivedImagePlane::put(ostr);
}
#endif

#ifdef DEBUGPLANE
void
ImagePlane::put(ostream &ostr)
{
	ostr << "ImagePlane::put()" << endl;
	AbstractImagePlane::put(ostr);
	MachinePlane::put(ostr);
	PatientPlane::put(ostr);
}
#endif

char *
DerivedImagePlane::getOrientation(Vector3D vector)
{
	char *orientation=new char[4];
	char *optr = orientation;
	*optr='\0';

	char orientationX = vector.getX() < 0 ? 'R' : 'L';
	char orientationY = vector.getY() < 0 ? 'A' : 'P';
	char orientationZ = vector.getZ() < 0 ? 'F' : 'H';

	double absX = fabs(vector.getX());
	double absY = fabs(vector.getY());
	double absZ = fabs(vector.getZ());

	int i;
	for (i=0; i<3; ++i) {
		if (absX>.0001 && absX>absY && absX>absZ) {
			*optr++=orientationX;
			absX=0;
		}
		else if (absY>.0001 && absY>absX && absY>absZ) {
			*optr++=orientationY;
			absY=0;
		}
		else if (absZ>.0001 && absZ>absX && absZ>absY) {
			*optr++=orientationZ;
			absZ=0;
		}
		else break;
		*optr='\0';
	}
	return orientation;
}

DerivedImagePlane::DerivedImagePlane(
	ImagePlaneHeadFeet hfff,ImagePlanePosition posn)
{
#ifdef DEBUGPLANE
	cerr << "DerivedImagePlane::DerivedImagePlane(hfff,posn) start" << endl;
#endif
	switch(hfff) {
		case HeadFirst:
			break;
		case FeetFirst:
		{
			Rotation3D r(Y,180);
			rotation*=r;
			Rotation3D unr(Y,-180);
			unrotation*=unr;
			break;
		}
	}

	switch(posn) {
		case Supine:
			break;
		case Prone:
		{
			Rotation3D r(Z,180);
			rotation*=r;
			Rotation3D unr(Z,-180);
			unrotation*=unr;
			break;
		}
		case LeftLateralDecubitus:
		{
			Rotation3D r(Z,90);
			rotation*=r;
			Rotation3D unr(Z,-90);
			unrotation*=unr;
			break;
		}
		case RightLateralDecubitus:
		{
			Rotation3D r(Z,-90);
			rotation*=r;
			Rotation3D unr(Z,90);
			unrotation*=unr;
			break;
		}
	}
#ifdef DEBUGPLANE
	cerr << "DerivedImagePlane::constructed this:" << endl;
	put(cerr);
#endif
}

void
DerivedImagePlane::setCorners(Point3D tlhc,Vector3D row,Vector3D col)
{
	_set_tlhc(tlhc*unrotation);
	_set_rowvec((row*unrotation)/row.magnitude());
	_set_colvec((col*unrotation)/col.magnitude());
	_set_center((tlhc+row/2+col/2)*unrotation);
}

void
DerivedImagePlane::angle(ImagePlaneAxis axis,double angle)
{
	Index3D plane;
	switch(axis) {
		case LeftRight:		plane=X; break;
		case AnteriorPosterior:	plane=Y; break;
		case HeadFeet:		plane=Z; break;
	}
	Rotation3D r(plane,angle);
	Rotation3D br=r*unrotation;

	_rotate_tlhc(br);
	_rotate_center(br);
	_rotate_rowvec(br);
	_rotate_colvec(br);
}

void
DerivedImagePlane::shift(Vector3D shift)
{
	_shift_center(shift*unrotation);
	_shift_tlhc(shift*unrotation);
}

void
DerivedImagePlane::scale(double factor)
{
	Point3D origin(0,0,0);
	Vector3D shift = origin - _get_center();
	_shift_tlhc(shift);
	_scale_tlhc(factor);
	_shift_tlhc(-shift);
}

