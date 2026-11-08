#include "implicits/blob.h"
#include "implicits.h"
#include "mathematics.h"
#include <cmath>
#include <math.h>

#define NORMAL		-1
#define HERMITE		0
#define POLYNOMIAL	1
#define WYVILL		4	// wyvill is defined to be the wtvill constant as well

#define CURRENT		POLYNOMIAL

double Blob::Lipshitz() {
	return 0;
}

double Blob::Value(const Vector& p) const {
	double d2 = SquaredNorm(p - pos) / (radius/*radius*/);

	#if (CURRENT == NORMAL)
	d2/=radius;
	return peak * exp(-d2 / 2 );

	#elif (CURRENT == HERMITE)
	if (d2 >= 1) return 0;
	return peak * (2*(d2*sqrt(d2)) - 3*d2 +1);

	#elif (CURRENT == WYVILL)
	if (d2 >= 1) return 0;
	return peak * pow((1-d2),WYVILL);

	#elif (CURRENT == POLYNOMIAL)
	if (d2 >= 1) return 0;
	return peak * (1-d2)*(1-d2) * (1- 4./9. * d2);
	#endif
}

Vector Blob::Gradient(const Vector& p) const {
	Vector dir = p-pos;
	double d2 = SquaredNorm(dir) / (radius/*radius*/);

	#if (CURRENT == NORMAL)
	d2/=radius;
	return -2 * peak * dir * exp(-d2 / 2);

	#elif (CURRENT == HERMITE)
	if (d2>=1) return Vector(0);
	return (6.0 * peak * (sqrt(d2) - 1.0) / radius) * dir;

	#elif (CURRENT == WYVILL)
	if (d2>=1) return Vector(0);
	return (WYVILL * pow(1-d2, WYVILL-1) * -2*sqrt(d2)) * dir * peak;

	#elif (CURRENT == POLYNOMIAL)
	if (d2 >= 1) return Vector(0);
	double d = sqrt(d2);
	return peak * 4./9. * d * (6*d2*d2 - 17*d2 + 11) * -dir;
	#endif
}

Vector Blob::Normal(const Vector& p) const {
	return Normalized(p - pos);
}
/*void Blob::Polygonize(int disc, Mesh& mesh, const Box& box, const double& eps ) const {
	AnalyticScalarField::Polygonize(disc, mesh, box, eps);
	}*/
