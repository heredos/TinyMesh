#pragma once
#include "implicits.h"
#include "mathematics.h"

#include <utility>

class ImplicitTranslate: public AnalyticScalarField{
private:
	ConstPtr child;
	Vector offset;

public:
	ImplicitTranslate(ConstPtr child, const Vector& offset)
		: child(std::move(child)), offset(offset) {}
	double Value(const Vector& p) const override;
	Vector Gradient(const Vector& p) const override;
	Vector Normal(const Vector& p) const override;
};
