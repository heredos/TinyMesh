#include "mathematics.h"
#include "implicits/add.h"

double ImplicitAdd::Value(const Vector& p) const {
	return a->Value(p)+b->Value(p);
}
Vector ImplicitAdd::Gradient(const Vector& p) const {
	return a->Gradient(p)+b->Gradient(p);
}
Vector ImplicitAdd::Normal(const Vector& p) const {
	return Normalized(-Gradient(p));
	//return Normalized(-(a->Normal(p) * a->Value(p) + b->Normal(p) * b->Value(p)));
}
