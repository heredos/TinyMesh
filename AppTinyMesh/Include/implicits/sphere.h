# pragma once
#include "implicits.h"
#include "math.h"
#include "mathematics.h"

class Sphere: public AnalyticScalarField {
	private:
		Vector pos;
		double radius;
	public:
		Sphere(const Vector pos, const double radius) : pos(pos), radius(radius) {}
		double Lipshitz();
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		//void Polygonize(int, Mesh&, const Box&, const double& = 1e-4) const override;
		Vector Normal(const Vector& p) const override;
};
