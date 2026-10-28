#include "implicits/ringify.h"
#include "mathematics.h"
#include <cmath>

double ImplicitRingify::Value(const Vector& p) const{
	//we rotate along the z axis
	// so the Z value stays the same, while we normalize x and y together to make it the x value of our field
	return ptr->Value(Vector(sqrt(p[0]*p[0]+p[1]*p[1]), 0, p[2]));
}

Vector ImplicitRingify::Gradient(const Vector& p) const{
	// use the same sampling point as Value
	Vector initialG = ptr->Gradient(Vector(sqrt(p[0]*p[0]+p[1]*p[1]), 0, p[2])); // do not mistake with initialD.
	// then project the vector's X according to the XZ direction
	double full = fabs(p[0])+fabs(p[1]);
	return Vector(initialG[0]*(p[0]/full), initialG[0]*(p[1]/full), initialG[2]);
}

Vector ImplicitRingify::Normal(const Vector& p) const{
	return Normalized(Gradient(p));
}
