#include "implicits/rotate.h"
#include "mathematics.h"

double ImplicitRotation::Value(const Vector& p) const{
	return ptr->Value(Vector(p*rotmat[0], p*rotmat[1], p*rotmat[2]));
}

Vector ImplicitRotation::Gradient(const Vector& p) const{
	Vector p2 = ptr->Gradient(Vector(p*rotmat[0], p*rotmat[1], p*rotmat[2]));
	return Vector(p2*rotmatT[0], p2*rotmatT[1], p2*rotmatT[2]);
}

Vector ImplicitRotation::Normal(const Vector& p) const{
	Vector p2 = ptr->Normal(Vector(p*rotmat[0], p*rotmat[1], p*rotmat[2]));
	return Vector(p2*rotmatT[0], p2*rotmatT[1], p2*rotmatT[2]);
}
