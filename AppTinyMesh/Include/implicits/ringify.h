#pragma once
#include "implicits.h"
#include "mathematics.h"

#include <utility>

class ImplicitRingify: public AnalyticScalarField{
	private:
	ConstPtr ptr;

	public:
		ImplicitRingify(ConstPtr ptr) : ptr(std::move(ptr)) {}
		ImplicitRingify(AnalyticScalarField* ptr) : ImplicitRingify(Borrow(ptr)) {}
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		Vector Normal(const Vector& p) const override;
};
