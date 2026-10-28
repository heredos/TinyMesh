#pragma once
#include "implicits.h"
#include "mathematics.h"

#include <utility>

class ImplicitMove: public AnalyticScalarField{
	private:
	ConstPtr ptr;
	Vector offset;

	public:
		ImplicitMove(ConstPtr ptr, Vector offset) : ptr(std::move(ptr)), offset(offset) {}
		ImplicitMove(AnalyticScalarField* ptr, Vector offset) : ImplicitMove(Borrow(ptr), offset) {}
		double Value(const Vector& p) const override{return ptr->Value(p-offset);}
		Vector Gradient(const Vector& p) const override{return ptr->Gradient(p-offset);}
		Vector Normal(const Vector& p) const override{return ptr->Normal(p-offset);}
};
