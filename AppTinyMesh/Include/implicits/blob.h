#pragma once
#include "implicits.h"
#include "math.h"
#include "mathematics.h"

class Blob: public AnalyticScalarField {
	private:
		Vector pos;
		double peak;
		double radius;
	public:
		Blob(const Vector pos, const double peak, const double radius) : pos(pos), peak(peak), radius(radius) {}
		double Lipshitz();
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		//void Polygonize(int, Mesh&, const Box&, const double& = 1e-4) const override;
		Vector Normal(const Vector&) const override;
};
