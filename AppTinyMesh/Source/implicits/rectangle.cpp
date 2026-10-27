#include "implicits/rectangle.h"
#include "mathematics.h"
#include <cmath>

Rectangle::Rectangle(Vector p1, Vector p2){
	this->p1 = Vector(Math::Min(p1[0], p2[0]), Math::Min(p1[1], p2[1]), Math::Min(p1[2], p2[2]));
	this->p2 = Vector(Math::Max(p1[0], p2[0]), Math::Max(p1[1], p2[1]), Math::Max(p1[2], p2[2]));
}
double Rectangle::Lipshitz(){
	return 1.;
}
double Rectangle::Value(const Vector& p) const {
	Vector v = Vector::Max(p1-p, p-p2);
	double add = 0;
	double max = -INFINITY;
	for (int i=0; i<3; i++){
		if (v[i]>max)
			max = v[i];
		if (v[i]>0)
			add += v[i]*v[i];
	}
	if (max>0){
		return sqrt(add);
	}else{
		return max;
	}
}
Vector Rectangle::Gradient(const Vector& p) const {
	Vector p1p = p1-p;
	Vector p2p = p-p2;
	Vector v = Vector();
	bool allin = true;
	double max = -INFINITY;
	for (int i=0; i<3; i++){
		if (p1p[i] > p2p[i]){
			if (p1p[i]>0){
				if (allin){
					allin = false;
					v = Vector();
					v[i] = -p1p[i];
				}else{
					v[i] = -p1p[i];
				}
			}else if (allin && p1p[i] > max) {
				v = Vector();
				v[i] = -1;
				max = p1p[i];
			}
		}else{
			if (p2p[i]>0){
				if (allin){
					allin = false;
					v = Vector();
					v[i] = p2p[i];
				}else{
					v[i] = p2p[i];
				}
			}else if (allin && p2p[i] > max) {
				v = Vector();
				v[i] = 1;
				max = p2p[i];
			}
		}
	}
	return Normalized(v);
}

Vector Rectangle::Normal(const Vector& p) const {
	return Gradient(p);
}
