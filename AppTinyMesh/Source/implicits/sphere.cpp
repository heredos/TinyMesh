#include "implicits/sphere.h"

double Sphere::Lipshitz(){
	return 1.;
}
double Sphere::Value(const Vector& p) const {
	return Norm(p-pos) - radius;
}
Vector Sphere::Gradient(const Vector& p) const {
	return Normalized(p-pos);
}
Vector Sphere::Normal(const Vector& p) const {
	return Normalized(p-pos);
}
