#include "implicits/blob.h"
#include "implicits.h"
#include "mathematics.h"
#include <math.h>


double Blob::Lipshitz() {
	return 0;
}

double Blob::Value(const Vector& p) const {
	// compute the gaussian
	return peak * exp(-SquaredNorm(p - pos) / (2 * radius * radius));
}

Vector Blob::Gradient(const Vector& p) const {
	return -2 * peak * (p - pos) * exp(-SquaredNorm(p - pos) / (2 * radius * radius));
}

Vector Blob::Normal(const Vector& p) const {
	return Normalized(p - pos);
}
/*void Blob::Polygonize(int disc, Mesh& mesh, const Box& box, const double& eps ) const {
	AnalyticScalarField::Polygonize(disc, mesh, box, eps);
	}*/
