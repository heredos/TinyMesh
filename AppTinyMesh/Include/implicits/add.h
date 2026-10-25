#pragma once
#include "implicits.h"
#include "mathematics.h"

#include <utility>

class ImplicitAdd: public AnalyticScalarField{
	private:
	ConstPtr a;
	ConstPtr b;

	public:
		ImplicitAdd(ConstPtr a, ConstPtr b) : a(std::move(a)), b(std::move(b)) {}
		ImplicitAdd(AnalyticScalarField* a, AnalyticScalarField* b) : ImplicitAdd(Borrow(a), Borrow(b)) {}
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		Vector Normal(const Vector& p) const override;
};
