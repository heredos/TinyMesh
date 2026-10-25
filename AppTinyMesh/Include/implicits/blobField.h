#pragma once
#include "implicits/blob.h"
#include "implicits.h"
#include "mathematics.h"

class BlobField: public AnalyticScalarField{
	protected:
		Blob blob;
		int childs;
		BlobField *left, *right, *parent;
		void rotateLeft();
		void rotateRight();

	public:
		BlobField(Blob b, BlobField *parent = nullptr): blob(b), parent(parent){
			childs  = 0;
			left = right = nullptr;
		}
		void add(Blob b);
		double Value(const Vector& p) const override;
		Vector Gradient(const Vector& p) const override;
		Vector Normal(const Vector& p) const override;
};
