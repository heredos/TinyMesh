#pragma once
#include "implicits.h"
#include "mathematics.h"

#include <utility>

class ImplicitNot: public AnalyticScalarField{
	private:
	ConstPtr ptr;

	public:
		ImplicitNot(ConstPtr ptr) : ptr(std::move(ptr)) {}
		ImplicitNot(AnalyticScalarField* ptr) : ImplicitNot(Borrow(ptr)) {}
		double Value(const Vector& p) const override{
			return -ptr->Value(p);
		}
		Vector Gradient(const Vector& p) const override{
			return -ptr->Gradient(p);
		}
		Vector Normal(const Vector& p) const override{
			return -ptr->Normal(p);
		}
};
