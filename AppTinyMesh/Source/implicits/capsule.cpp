#include "implicits/capsule.h"
#include "mathematics.h"
#include <cmath>

Capsule::Capsule(Vector p1, Vector p2){
	origin = p1;
	end = p2;
	direction = p2-p1;
	size = direction*direction;
}
double Capsule::Lipshitz(){
	return 1.;
}
double Capsule::Value(const Vector& p) const {
	Vector proj = p-origin;
	double coef = (proj*direction)/(size);
	if (coef<0){
		return Norm(proj);
	}else if(coef>1){
		return Norm(p-end);
	}else{
		proj=direction*coef;
		return Norm(p-proj);
	}
}
Vector Capsule::Gradient(const Vector& p) const {
	Vector proj = p-origin;
	double coef = (proj*direction)/(size);
	if (coef<0){
		return Normalized(proj);
	}else if(coef>1){
		return Normalized(p-end);
	}else{
		proj=direction*coef;
		return Normalized(p-proj);
	}
}

Vector Capsule::Normal(const Vector& p) const {
	return Gradient(p);
}
