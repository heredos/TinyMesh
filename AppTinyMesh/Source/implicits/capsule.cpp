#include "implicits/capsule.h"
#include "mathematics.h"
#include <cmath>

Capsule::Capsule(Vector p1, Vector p2, double radius){
	origin = p1;
	end = p2;
	direction = p2-p1;
	size = direction*direction;
	this->radius = radius;
}
double Capsule::Lipshitz(){
	return 1.;
}
double Capsule::Value(const Vector& p) const {
	Vector proj = p-origin;
	double coef = (proj*direction)/(size);
	if (coef<0){
		return Norm(proj)-radius;
	}else if(coef>1){
		return Norm(p-end)-radius;
	}else{
		Vector proj2=direction*coef;
		return Norm(proj-proj2)-radius;
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
		Vector proj2=direction*coef;
		return Normalized(proj-proj2);
	}
}

Vector Capsule::Normal(const Vector& p) const {
	return Gradient(p);
}
