#pragma once
#include "implicits/blobField.h"
#include "mathematics.h"
#include <omp.h>

#define LINEAR


void BlobField::add(Blob b){
	BlobField *toChange;
	#ifndef LINEAR
	// automagically balance the tree
	if (++childs%2){
		if (left==nullptr){
			left = new BlobField(b, this);
		}else{
			left->add(b);
		}
	}else{
		if (right==nullptr){
			right = new BlobField(b, this);
		}else{
			right->add(b);
		}
	}
	#else
	// automagically keep the tree in a rack
	if (childs==0){
		left = new BlobField(b, this);
		childs = 1;
	}else{
		left->add(b);
	}
	#endif
}
double BlobField::Value(const Vector& p)const {
	double left = 0;
	double right = 0;
	if (childs>0){
		left = this->left->Value(p);
	}
	if (childs>1){
		right = this->right->Value(p);
	}
	return left+right+blob.Value(p);
}
Vector BlobField::Gradient(const Vector& p) const {
	Vector left = Vector(0);
	Vector right = Vector(0);
	if (childs>0){
		left = this->left->Gradient(p);
	}
	if (childs>1){
		right = this->right->Gradient(p);
	}
	return left+right+blob.Gradient(p);
}
Vector BlobField::Normal(const Vector& p) const {
	return Normalized(-Gradient(p));
}
