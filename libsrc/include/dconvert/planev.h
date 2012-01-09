//#define DEBUGPLANE

#ifndef __Header_planev__
#define __Header_planev__

#ifdef DEBUGPLANE
#include <iostream.h>
#include <iomanip.h>
#endif

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define	cosd(theta)	cos((theta)/180*M_PI)
#define	sind(theta)	sin((theta)/180*M_PI)

enum Index3D { X=0,Y,Z };

class Vector3DBase {
protected:
	double values[3];
public:
	Vector3DBase(void)
		{
			values[X]=0;
			values[Y]=0;
			values[Z]=0;
		}

	Vector3DBase(double x,double y,double z)
		{
			values[X]=x;
			values[Y]=y;
			values[Z]=z;
		}

	double& operator[](unsigned i)
		{
			return values[i];
		}

	Vector3DBase& operator=(int v)
		{
			int i;
			for (i=0;i<3;++i) values[i]=v;
			return *this;
		}

	double operator*(Vector3DBase v)
		{
			int i;
			double product=0;
			for (i=0;i<3;++i) product+=values[i]*v[i];
			return product;
		}

	Vector3DBase operator/(double d)
		{
			Vector3DBase v(values[X]/d,values[Y]/d,values[Z]/d);
			return v;
		}

	Vector3DBase operator-(void)
		{
			Vector3DBase v(-values[X],-values[Y],-values[Z]);
			return v;
		}

	double magnitude()
		{
			return sqrt(
				values[X]*values[X]+
				values[Y]*values[Y]+
				values[Z]*values[Z]
			);
		}

	double getX(void)	{ return values[X]; }
	double getY(void)	{ return values[Y]; }
	double getZ(void)	{ return values[Z]; }

#ifdef DEBUGPLANE
	void put(ostream& ost)
		{
			ost << dec << setiosflags(ios::fixed)
			    << setiosflags(ios::showpoint) << setprecision(2)
			    << values[0] << ","
			    << values[1] << ","
			    << values[2] << endl;
		}
#endif
};

class Matrix3D {
protected:
	Vector3DBase values[3];
public:
	Matrix3D(void)
		{
		}

	Matrix3D(double xx,double xy,double xz,
		 double yx,double yy,double yz,
		 double zx,double zy,double zz)
		{
			values[X][X]=xx;
			values[X][Y]=xy;
			values[X][Z]=xz;
			values[Y][X]=yx;
			values[Y][Y]=yy;
			values[Y][Z]=yz;
			values[Z][X]=zx;
			values[Z][Y]=zy;
			values[Z][Z]=zz;
		}

	Vector3DBase& operator[](unsigned i) { return values[i]; }

	Matrix3D& operator=(int v)
		{
			int i;
			for (i=0;i<3;++i) values[i]=0;
			for (i=0;i<3;++i) values[i][i]=v;
			return *this;
		}

	Vector3DBase operator*(Vector3DBase v)
		{
			int i;
			Vector3DBase product;
			for (i=0;i<3;++i) product[i]=values[i]*v;
			return product;
		}

	Matrix3D operator*(Matrix3D m)
		{
			int i,j,k;
			Matrix3D product;
			for (i=0;i<3;++i)
				for (j=0;j<3;++j) {
					product[i][j]=0;
					for (k=0;k<3;++k)
						product[i][j]+=
							(*this)[k][i]*m[j][k];
				}
			return product;
		}

	Matrix3D& operator*=(Matrix3D m)
		{
			Matrix3D work;
			work=*this*m;
			*this=work;
			return *this;
		}

#ifdef DEBUGPLANE
	void put(ostream& ost)
		{
			values[0].put(ost);
			values[1].put(ost);
			values[2].put(ost);
			ost << endl;
		}
#endif
};

class Rotation3D : public Matrix3D {
	void aboutX(double theta)
		{
			values[X][X]=1;
			values[Y][Y]=cosd(theta);
			values[Y][Z]=-sind(theta);
			values[Z][Y]=sind(theta);
			values[Z][Z]=cosd(theta);
		}
	void aboutY(double theta)
		{
			values[X][X]=cosd(theta);
			values[X][Z]=sind(theta);
			values[Y][Y]=1;
			values[Z][X]=-sind(theta);
			values[Z][Z]=cosd(theta);
		}
	void aboutZ(double theta)
		{
			values[X][X]=cosd(theta);
			values[X][Y]=-sind(theta);
			values[Y][X]=sind(theta);
			values[Y][Y]=cosd(theta);
			values[Z][Z]=1;
		}
public:
	Rotation3D(void) : Matrix3D() { Matrix3D::operator=(1); }

	Rotation3D(Matrix3D m) : Matrix3D(m) {}

	Rotation3D(int plane,double theta)
		{
			Matrix3D::operator=(1);
			if (plane == X) {
				aboutX(theta);
			}
			else if (plane == Y) {
				aboutY(theta);
			}
			else if (plane == Z) {
				aboutZ(theta);
			}
			//cerr << "Rotation3D() plane=" << plane
			//     << " theta=" << theta << "\n";
			//put(cerr);
		}
};

class Vector3D : public Vector3DBase {
public:
	Vector3D(void) : Vector3DBase() {}
	Vector3D(double x,double y,double z) : Vector3DBase(x,y,z) {}
	Vector3D(Vector3DBase v) : Vector3DBase(v) {}

	Vector3D operator*(double d)
		{
			Vector3DBase v(values[X]*d,values[Y]*d,values[Z]*d);
			return v;
		}

	Vector3D operator*(Rotation3D r)
		{
			//cerr << "Vector3D operator*(Rotation3D r)\n";
			//put(cerr);
			Vector3D p = r*(*this);
			//p.put(cerr);
			return p;
		}

	Vector3D& operator*=(Rotation3D r)
		{
			*this=r*(*this);
			return *this;
		}
};

class Point3D : public Vector3D {
public:
	Point3D(void) : Vector3D() {}
	Point3D(double x,double y,double z) : Vector3D(x,y,z) {}
	Point3D(Vector3D v) : Vector3D(v) {}

	Vector3D operator-(Point3D p)
		{
			Vector3D v(
				values[X]-p.getX(),
				values[Y]-p.getY(),
				values[Z]-p.getZ());
			return v;
		}

	Point3D operator-(Vector3D v)
		{
			Point3D p(
				values[X]-v.getX(),
				values[Y]-v.getY(),
				values[Z]-v.getZ());
			return p;
		}

	Point3D operator+(Vector3D v)
		{
			Point3D p(
				values[X]+v.getX(),
				values[Y]+v.getY(),
				values[Z]+v.getZ());
			return p;
		}
};

#endif /* __Header_planev__ */
