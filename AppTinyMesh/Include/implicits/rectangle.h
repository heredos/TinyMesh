# pragma once
#include "implicits.h"
#include "math.h"
#include "mathematics.h"

class Rectangle: public AnalyticScalarField {
	private:
		Vector p1, p2;
	public:
		Rectangle(const Vector point1, const Vector point2);
		double Lipshitz();
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		//void Polygonize(int, Mesh&, const Box&, const double& = 1e-4) const override;
		Vector Normal(const Vector& p) const override;
};
