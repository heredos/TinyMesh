#pragma once
#include "implicits.h"
#include "mathematics.h"
#include <algorithm>

class ImplicitIntersection: public AnalyticScalarField{
	private:
	ConstPtr a;
	ConstPtr b;

	public:
		ImplicitIntersection(ConstPtr a, ConstPtr b) : a(std::move(a)), b(std::move(b)) {}
		ImplicitIntersection(AnalyticScalarField* a, AnalyticScalarField* b) : ImplicitIntersection(Borrow(a), Borrow(b)) {}
		double Value(const Vector& p) const override{
			return std::max(a->Value(p), b->Value(p));
		}
		Vector Gradient(const Vector& p) const override{
			return a->Value(p)>b->Value(p)?a->Gradient(p):b->Gradient(p);
		}
		Vector Normal(const Vector& p) const override{
			return a->Value(p)>b->Value(p)?a->Normal(p):b->Normal(p);
		}
};
