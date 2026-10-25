#pragma once

#include <memory>

#include <QtNodes/NodeData>

#include "implicits.h"

class FieldData : public QtNodes::NodeData
{
public:
    using FieldPtr = AnalyticScalarField::ConstPtr;

    explicit FieldData(FieldPtr field = nullptr)
        : _field(std::move(field))
    {}

    QtNodes::NodeDataType type() const override
    {
        return QtNodes::NodeDataType{QStringLiteral("field"), QStringLiteral("Field")};
    }

    const FieldPtr& field() const { return _field; }

private:
    FieldPtr _field;
};
