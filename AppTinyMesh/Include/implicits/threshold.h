#pragma once
#include "implicits.h"
#include "mathematics.h"

class Threshold : public AnalyticScalarField{
	private:
		AnalyticScalarField* field;
		double threshold;
	public:
		Threshold(AnalyticScalarField* object, double threshold): field(object), threshold(threshold){}
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
