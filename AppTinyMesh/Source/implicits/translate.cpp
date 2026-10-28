#include "implicits/translate.h"

double ImplicitTranslate::Value(const Vector& p) const {
	return child->Value(p - offset);
}

Vector ImplicitTranslate::Gradient(const Vector& p) const {
	return child->Gradient(p - offset);
}

Vector ImplicitTranslate::Normal(const Vector& p) const {
	return child->Normal(p - offset);
}
