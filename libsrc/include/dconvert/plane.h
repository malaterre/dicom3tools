#ifndef __Header_plane__
#define __Header_plane__

#include "planev.h"

enum ImagePlaneHeadFeet { HeadFirst,FeetFirst };

enum ImagePlanePosition { 
	Supine,Prone,LeftLateralDecubitus,RightLateralDecubitus };

enum ImagePlaneAxis { AnteriorPosterior,HeadFeet,LeftRight };

enum ImagePlanePlane {  Axial,Sagittal,Coronal,
			AxialLP, AxialRP,
			SagittalPF,SagittalIP,
			CoronalLI };

class AbstractImagePlane {
	Point3D center;
	Point3D tlhc;
	Vector3D rowvec;
	Vector3D colvec;

	friend class ImagePlane;	// uses the following methods ...

	Point3D	_get_center	(void)			{ return center; }
	void	_set_center	(Point3D p)		{ center=p; }
	void	_rotate_center	(Rotation3D r)		{ center=center*r; }
	void	_shift_center	(Vector3D shift)	{ center=center+shift; }

	Point3D	_get_tlhc	(void)			{ return tlhc; }
	void	_set_tlhc	(Point3D p)		{ tlhc=p; }
	void	_rotate_tlhc	(Rotation3D r)		{ tlhc=tlhc*r; }
	void	_shift_tlhc	(Vector3D shift)	{ tlhc=tlhc+shift; }

	void	_scale_tlhc	(double factor)	
		{
			Point3D origin(0,0,0);
			Vector3D v = (origin-tlhc)*factor;
			tlhc=origin-v;
		}

	Vector3D	_get_rowvec	(void)		{ return rowvec; }
	void		_set_rowvec	(Vector3D v)	{ rowvec=v; }
	void		_rotate_rowvec	(Rotation3D r)	{ rowvec=rowvec*r; }

	Vector3D	_get_colvec	(void)		{ return colvec; }
	void		_set_colvec	(Vector3D v)	{ colvec=v; }
	void		_rotate_colvec	(Rotation3D r)	{ colvec=colvec*r; }

public:
	AbstractImagePlane(void) {}
	AbstractImagePlane(ImagePlanePlane plane,double hfov,double vfov);

#ifdef DEBUGPLANE
	void put(ostream &);
#endif
};


class DerivedImagePlane
{
	Rotation3D	rotation;	// this to base
	Rotation3D	unrotation;	// base to this

	char *getOrientation(Vector3D vector);

	// derived classes pass access to VirtualImagePlane's methods ...

	virtual Point3D		_get_center	(void)			= 0;
	virtual void		_set_center	(Point3D p)		= 0;
	virtual void		_rotate_center	(Rotation3D r)		= 0;
	virtual void		_shift_center	(Vector3D shift)	= 0;

	virtual Point3D		_get_tlhc	(void)			= 0;
	virtual void		_set_tlhc	(Point3D p)		= 0;
	virtual void		_rotate_tlhc	(Rotation3D r)		= 0;
	virtual void		_shift_tlhc	(Vector3D shift)	= 0;
	virtual void		_scale_tlhc	(double factor)		= 0;

	virtual Vector3D	_get_rowvec	(void)			= 0;
	virtual void		_set_rowvec	(Vector3D v)		= 0;
	virtual void		_rotate_rowvec	(Rotation3D r)		= 0;

	virtual Vector3D	_get_colvec	(void)			= 0;
	virtual void		_set_colvec	(Vector3D v)		= 0;
	virtual void		_rotate_colvec	(Rotation3D r)		= 0;

public:
	DerivedImagePlane(ImagePlaneHeadFeet hfff,ImagePlanePosition posn);

	char *getRowOrientation(void)
		{
			return getOrientation(getRowVector());
		}
	char *getColOrientation(void)
		{
			return getOrientation(getColVector());
		}

	Point3D  getTLHC(void)		{ return _get_tlhc()*rotation; }
	Point3D  getCenter(void)	{ return _get_center()*rotation; }
	Vector3D getRowVector(void)	{ return _get_rowvec()*rotation; }
	Vector3D getColVector(void)	{ return _get_colvec()*rotation; }

	void setCorners(Point3D tlhc,Vector3D row,Vector3D col);
	void angle(ImagePlaneAxis axis,double angle);
	void shift(Vector3D shift);
	void scale(double factor);

#ifdef DEBUGPLANE
	void put(ostream &);
#endif
};

class PatientPlane : public DerivedImagePlane
{
public:
	PatientPlane(ImagePlaneHeadFeet hfff,ImagePlanePosition posn) :
		DerivedImagePlane(hfff,posn)
		{
#ifdef DEBUGPLANE
			cerr << "PatientPlane::PatientPlane(void)" << endl;
#endif
		}
#ifdef DEBUGPLANE
	void put(ostream &);
#endif
};

class MachinePlane : public DerivedImagePlane
{
public:
	MachinePlane(void) :
		DerivedImagePlane(HeadFirst,Supine)
		{
#ifdef DEBUGPLANE
			cerr << "MachinePlane::MachinePlane(void)" << endl;
#endif
		}
#ifdef DEBUGPLANE
	void put(ostream &);
#endif
};

class ImagePlane :	public virtual AbstractImagePlane,
			public PatientPlane,
			public MachinePlane
{
	// instances of virtual methods of DerivedImagePlane
	// allows DerivedImagePlane methods access to AbstractImagePlane

	Point3D		_get_center	(void)
			{
				return AbstractImagePlane::_get_center();
			}

	void		_set_center	(Point3D p)
			{
				AbstractImagePlane::_set_center(p);
			}

	void		_rotate_center	(Rotation3D r)
			{
				AbstractImagePlane::_rotate_center(r);
			}

	void		_shift_center	(Vector3D shift)
			{
				AbstractImagePlane::_shift_center(shift);
			}


	Point3D		_get_tlhc	(void)
			{
				return AbstractImagePlane::_get_tlhc();
			}

	void		_set_tlhc	(Point3D p)
			{
				AbstractImagePlane::_set_tlhc(p);
			}

	void		_rotate_tlhc	(Rotation3D r)
			{
				AbstractImagePlane::_rotate_tlhc(r);
			}

	void		_shift_tlhc	(Vector3D shift)
			{
				AbstractImagePlane::_shift_tlhc(shift);
			}

	void		_scale_tlhc	(double factor)
			{
				AbstractImagePlane::_scale_tlhc(factor);
			}



	Vector3D	_get_rowvec	(void)
			{
				return AbstractImagePlane::_get_rowvec();
			}

	void		_set_rowvec	(Vector3D v)
			{
				AbstractImagePlane::_set_rowvec(v);
			}

	void		_rotate_rowvec	(Rotation3D r)
			{
				AbstractImagePlane::_rotate_rowvec(r);
			}


	Vector3D	_get_colvec	(void)
			{
				return AbstractImagePlane::_get_colvec();
			}

	void		_set_colvec	(Vector3D v)
			{
				AbstractImagePlane::_set_colvec(v);
			}

	void		_rotate_colvec	(Rotation3D r)
			{
				AbstractImagePlane::_rotate_colvec(r);
			}
public:
	ImagePlane(
		ImagePlanePlane		plane,
		double fov,
		ImagePlaneHeadFeet	hfff,
		ImagePlanePosition	posn
	) :
		AbstractImagePlane(plane,fov,fov),
		PatientPlane(hfff,posn),
		MachinePlane()
		{
#ifdef DEBUGPLANE
			cerr << "ImagePlane::ImagePlane(plane,fov,hfff,posn)" << endl;
#endif
		}

	ImagePlane(
		ImagePlanePlane		plane,
		double hfov,
		double vfov,
		ImagePlaneHeadFeet	hfff,
		ImagePlanePosition	posn
	) :
		AbstractImagePlane(plane,hfov,vfov),
		PatientPlane(hfff,posn),
		MachinePlane()
		{
#ifdef DEBUGPLANE
			cerr << "ImagePlane::ImagePlane(plane,hfov,vfov,hfff,posn)" << endl;
#endif
		}

	ImagePlane(
		ImagePlaneHeadFeet	hfff,
		ImagePlanePosition	posn
	) :
		AbstractImagePlane(),
		PatientPlane(hfff,posn),
		MachinePlane()
		{
#ifdef DEBUGPLANE
			cerr << "ImagePlane::ImagePlane(hfff,posn)" << endl;
#endif
		}

#ifdef DEBUGPLANE
	void put(ostream &);
#endif
};

#endif /* __Header_plane__ */
