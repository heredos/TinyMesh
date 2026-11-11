#pragma once

#include <memory>
#include <vector>

#include <QJsonArray>
#include <QJsonObject>
#include <QtNodes/NodeDelegateModel>

#include "FieldData.h"
#include "box.h"
#include "mathematics.h"

class Mesh;
class MeshColor;
class MeshWidget;
class QCheckBox;
class QDoubleSpinBox;
class QFormLayout;
class QLabel;
class QRadioButton;
class QSpinBox;
class QTableWidget;

class NodeModelBase : public QtNodes::NodeDelegateModel{
    Q_OBJECT

public:
    QWidget* embeddedWidget() override;
    void setTimingText(const QString& text);
    double buildMs() const { return _buildMs; }

Q_SIGNALS:
    void parametersChanged();
    void recomputeRequested();

protected:
    virtual void buildParameters(QFormLayout* form);
    void setBuildTime(double ms) { _buildMs = ms; }
    void refreshWidgetGeometry();

    QWidget* _widget = nullptr;
    QLabel* _timingLabel = nullptr;
    QString _timingText;
    double _buildMs = 0.0;
};

class ImplicitFieldNode : public NodeModelBase{
public:
    using FieldPtr = AnalyticScalarField::ConstPtr;

    unsigned int nPorts(QtNodes::PortType portType) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                   QtNodes::PortIndex portIndex) const override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
    void setInData(std::shared_ptr<QtNodes::NodeData> data,
                   QtNodes::PortIndex port) override;

    FieldPtr field() const { return _field; }
    void refresh();

    virtual unsigned int inputCount() const = 0;

protected:
    virtual void rebuildField() = 0;
    void rebuildAndEmit();

    std::vector<std::shared_ptr<FieldData>> _inputs;
    std::shared_ptr<FieldData> _output;
    FieldPtr _field;
};

class BlobNodeModel : public ImplicitFieldNode{
public:
    BlobNodeModel();

    QString name() const override { return QStringLiteral("Blob"); }
    QString caption() const override { return QStringLiteral("Blob"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 0; }

    void setParameters(const Vector& position, double peak, double radius);

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();

    Vector _position{0.0, 0.0, 0.0};
    double _peak = 1.0;
    double _radius = 0.8;

    QDoubleSpinBox* _x = nullptr;
    QDoubleSpinBox* _y = nullptr;
    QDoubleSpinBox* _z = nullptr;
    QDoubleSpinBox* _peakBox = nullptr;
    QDoubleSpinBox* _radiusBox = nullptr;
};

class SphereNodeModel : public ImplicitFieldNode{
public:
    SphereNodeModel();

    QString name() const override { return QStringLiteral("Sphere"); }
    QString caption() const override { return QStringLiteral("Sphere"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 0; }

    void setParameters(const Vector& position, double radius);

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();

    Vector _position{0.0, 0.0, 0.0};
    double _radius = 1.0;

    QDoubleSpinBox* _x = nullptr;
    QDoubleSpinBox* _y = nullptr;
    QDoubleSpinBox* _z = nullptr;
    QDoubleSpinBox* _radiusBox = nullptr;
};

class RectangleNodeModel : public ImplicitFieldNode{
public:
    RectangleNodeModel();

    QString name() const override { return QStringLiteral("Rectangle"); }
    QString caption() const override { return QStringLiteral("Rectangle"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 0; }

    void setParameters(const Vector& point1, const Vector& point2);

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();

    Vector _point1{-1.0, -1.0, -1.0};
    Vector _point2{1.0, 1.0, 1.0};

    QDoubleSpinBox* _x1 = nullptr;
    QDoubleSpinBox* _y1 = nullptr;
    QDoubleSpinBox* _z1 = nullptr;
    QDoubleSpinBox* _x2 = nullptr;
    QDoubleSpinBox* _y2 = nullptr;
    QDoubleSpinBox* _z2 = nullptr;
};

class CapsuleNodeModel : public ImplicitFieldNode{
public:
    CapsuleNodeModel();

    QString name() const override { return QStringLiteral("Capsule"); }
    QString caption() const override { return QStringLiteral("Capsule"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 0; }

    void setParameters(const Vector& point1, const Vector& point2, double radius);

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();

    Vector _point1{0.0, 0.0, 0.0};
    Vector _point2{1.0, 0.0, 0.0};
    double _radius = 0.25;

    QDoubleSpinBox* _x1 = nullptr;
    QDoubleSpinBox* _y1 = nullptr;
    QDoubleSpinBox* _z1 = nullptr;
    QDoubleSpinBox* _x2 = nullptr;
    QDoubleSpinBox* _y2 = nullptr;
    QDoubleSpinBox* _z2 = nullptr;
    QDoubleSpinBox* _radiusBox = nullptr;
};

class ImplicitAddNodeModel : public ImplicitFieldNode{
public:
    QString name() const override { return QStringLiteral("ImplicitAdd"); }
    QString caption() const override { return QStringLiteral("Add"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex portIndex) const override;
    unsigned int inputCount() const override { return 2; }

protected:
    void rebuildField() override;
};

class ImplicitUnionNodeModel : public ImplicitFieldNode{
public:
    QString name() const override { return QStringLiteral("ImplicitUnion"); }
    QString caption() const override { return QStringLiteral("Union"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex portIndex) const override;
    unsigned int inputCount() const override { return 2; }

protected:
    void rebuildField() override;
};

class ImplicitIntersectionNodeModel : public ImplicitFieldNode{
public:
    QString name() const override { return QStringLiteral("ImplicitIntersection"); }
    QString caption() const override { return QStringLiteral("Intersection"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex portIndex) const override;
    unsigned int inputCount() const override { return 2; }

protected:
    void rebuildField() override;
};

class ImplicitNotNodeModel : public ImplicitFieldNode{
public:
    QString name() const override { return QStringLiteral("ImplicitNot"); }
    QString caption() const override { return QStringLiteral("Not"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex portIndex) const override;
    unsigned int inputCount() const override { return 1; }

protected:
    void rebuildField() override;
};

class MoveNodeModel : public ImplicitFieldNode{
public:
    MoveNodeModel();

    QString name() const override { return QStringLiteral("Move"); }
    QString caption() const override { return QStringLiteral("Move"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 1; }

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();

    Vector _offset{0.0, 0.0, 0.0};
    QDoubleSpinBox* _x = nullptr;
    QDoubleSpinBox* _y = nullptr;
    QDoubleSpinBox* _z = nullptr;
};

class RotateNodeModel : public ImplicitFieldNode{
public:
    RotateNodeModel();

    QString name() const override { return QStringLiteral("Rotate"); }
    QString caption() const override { return QStringLiteral("Rotate"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 1; }

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();

    Vector _rotation{0.0, 0.0, 0.0};
    QDoubleSpinBox* _x = nullptr;
    QDoubleSpinBox* _y = nullptr;
    QDoubleSpinBox* _z = nullptr;
};

class RingifyNodeModel : public ImplicitFieldNode{
public:
    RingifyNodeModel();

    QString name() const override { return QStringLiteral("ImplicitRingify"); }
    QString caption() const override { return QStringLiteral("make Ring"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 1; }

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();
};

class ThresholdNodeModel : public ImplicitFieldNode{
public:
    ThresholdNodeModel();

    QString name() const override { return QStringLiteral("Threshold"); }
    QString caption() const override { return QStringLiteral("Threshold"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 1; }

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();

    double _threshold = 0.5;
    QDoubleSpinBox* _thresholdBox = nullptr;
};

class BlobFieldNodeModel : public ImplicitFieldNode{
public:
    struct BlobSpec
    {
        Vector position{0.0, 0.0, 0.0};
        double peak = 1.0;
        double radius = 0.8;
    };

    BlobFieldNodeModel();

    QString name() const override { return QStringLiteral("BlobField"); }
    QString caption() const override { return QStringLiteral("Blob Field"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;
    unsigned int inputCount() const override { return 0; }

    void setBlobs(std::vector<BlobSpec> blobs);
    const std::vector<BlobSpec>& blobs() const { return _blobs; }

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void rebuildField() override;
    void buildParameters(QFormLayout* form) override;

private:
    void addRow(const BlobSpec& blob, bool updateModel = true);
    void removeSelectedRows();
    void readTable();
    void syncUi();

    std::vector<BlobSpec> _blobs;
    QTableWidget* _table = nullptr;
    bool _updatingTable = false;
};

class OutputNodeModel : public NodeModelBase{
public:
    using FieldPtr = AnalyticScalarField::ConstPtr;

    OutputNodeModel();
    ~OutputNodeModel() override;

    QString name() const override { return QStringLiteral("MeshOutput"); }
    QString caption() const override { return QStringLiteral("Mesh Output"); }
    QString portCaption(QtNodes::PortType portType,
                        QtNodes::PortIndex) const override;

    unsigned int nPorts(QtNodes::PortType portType) const override;
    QtNodes::NodeDataType dataType(QtNodes::PortType portType,
                                   QtNodes::PortIndex portIndex) const override;
    std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;
    void setInData(std::shared_ptr<QtNodes::NodeData> data,
                   QtNodes::PortIndex port) override;

    FieldPtr field() const;
    Box box() const;
    int resolution() const { return _resolution; }
    double epsilon() const { return _epsilon; }

    void setPreviewMesh(const Mesh& mesh);
    void clearPreview();

    QJsonObject save() const override;
    void load(const QJsonObject& data) override;

protected:
    void buildParameters(QFormLayout* form) override;

private:
    void syncUi();
    void applyPreviewMesh();
    void applyPreviewMaterial();
    void requestPreviewRepaint();
    void openPreviewWindow();

    std::shared_ptr<FieldData> _input;
    std::shared_ptr<MeshColor> _previewMesh;
    Vector _center{0.0, 0.0, 0.0};
    double _halfSize = 2.5;
    int _resolution = 64;
    double _epsilon = 1.0e-4;

    QDoubleSpinBox* _cx = nullptr;
    QDoubleSpinBox* _cy = nullptr;
    QDoubleSpinBox* _cz = nullptr;
    QDoubleSpinBox* _halfSizeBox = nullptr;
    QSpinBox* _resolutionBox = nullptr;
    QDoubleSpinBox* _epsilonBox = nullptr;
    QLabel* _connectionLabel = nullptr;
    MeshWidget* _preview = nullptr;
    QWidget* _previewWindow = nullptr;
    MeshWidget* _detachedPreview = nullptr;
    bool _detachedPreviewNeedsSync = false;
    QLabel* _vertexCountLabel = nullptr;
    QLabel* _triangleCountLabel = nullptr;
    QCheckBox* _wireframe = nullptr;
    QRadioButton* _normalShading = nullptr;
    QRadioButton* _colorShading = nullptr;
};
