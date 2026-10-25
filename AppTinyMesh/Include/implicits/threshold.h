#pragma once
#include "implicits.h"
#include "mathematics.h"

#include <utility>

class Threshold : public AnalyticScalarField{
	private:
		ConstPtr field;
		double threshold;
	public:
		Threshold(ConstPtr object, double threshold): field(std::move(object)), threshold(threshold){}
		Threshold(AnalyticScalarField* object, double threshold): Threshold(Borrow(object), threshold){}
		double Value(const Vector& p) const override {
			return field->Value(p) - threshold;
		}
		Vector Gradient(const Vector& p ) const override {
			return field->Gradient(p);
		}
		Vector Normal(const Vector& p) const override{
			return field->Normal(p);
		}
		/*void Polygonize(int disc, Mesh& mesh, const Box& box, const double& eps = 1e-4) const override {
			AnalyticScalarField::Polygonize(disc, mesh, box, eps);
			}*/
};
