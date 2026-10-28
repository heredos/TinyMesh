#pragma once
#include "implicits.h"
#include "mathematics.h"

#include <utility>

class ImplicitRotation: public AnalyticScalarField{
	private:
	ConstPtr ptr;
	Vector rot;
	Vector rotmat[3];
	Vector rotmatT[3];

	public:
		ImplicitRotation(ConstPtr ptr, Vector rot) : ptr(std::move(ptr)){
			// simple xyz euler rotation
			double cx,cy,cz,sx,sy,sz;
			cx = cos(rot[0]); sx = sin(rot[0]);
			cy = cos(rot[1]); sy = sin(rot[1]);
			cz = cos(rot[2]); sz = sin(rot[2]);
			// thanks wikipedia for the formula ()
			rotmat[0] = Vector(cy*cz, -cy*sz, sy);
			rotmat[1] = Vector(cx*sz+sx*sy*cz, cx*cz-sx*sy*sz, -sx*cy);
			rotmat[2] = Vector(sx*sz-cx*sy*cz, sx*cz+cx*sy*sz, cx*cy);
			// invert the rotation matrix (it's orthogonal so its inverse is its transpose)
			for (int i=0; i<3; i++){
				for (int j=0; j<3;j++){
					rotmatT[i][j] = rotmat[j][i];
				}
			}
		}
		ImplicitRotation(AnalyticScalarField* ptr, Vector rot) : ImplicitRotation(Borrow(ptr), rot){}
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		Vector Normal(const Vector& p) const override;
};
