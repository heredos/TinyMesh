# pragma once
#include "implicits.h"
#include "math.h"
#include "mathematics.h"

class Capsule: public AnalyticScalarField {
	private:
		Vector origin, end, direction;
		double size;
	public:
		Capsule(const Vector point1, const Vector point2);
		double Lipshitz();
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		//void Polygonize(int, Mesh&, const Box&, const double& = 1e-4) const override;
		Vector Normal(const Vector& p) const override;
};
