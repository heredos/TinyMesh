#pragma once
#include "implicits.h"
#include "mathematics.h"

class ImplicitAdd: public AnalyticScalarField{
	private:
	AnalyticScalarField* a;
	AnalyticScalarField* b;

	public:
		ImplicitAdd(AnalyticScalarField* a, AnalyticScalarField* b) : a(a), b(b) {}
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		Vector Normal(const Vector& p) const override;
};
