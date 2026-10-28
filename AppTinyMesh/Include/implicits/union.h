#pragma once
#include "implicits.h"
#include "mathematics.h"
#include <algorithm>

class ImplicitUnion: public AnalyticScalarField{
	private:
	ConstPtr a;
	ConstPtr b;

	public:
		ImplicitUnion(ConstPtr a, ConstPtr b) : a(std::move(a)), b(std::move(b)) {}
		ImplicitUnion(AnalyticScalarField* a, AnalyticScalarField* b) : ImplicitUnion(Borrow(a), Borrow(b)) {}
		double Value(const Vector& p) const override{
			return std::min(a->Value(p), b->Value(p));
		}
		Vector Gradient(const Vector& p) const override{
			return a->Value(p)>b->Value(p)?b->Gradient(p):a->Gradient(p);
		}
		Vector Normal(const Vector& p) const override{
			return a->Value(p)>b->Value(p)?b->Normal(p):a->Normal(p);
		}
};
