#include "nodes/ImplicitNodeModels.h"

#include <algorithm>
#include <chrono>
#include <functional>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QRadioButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "implicits/add.h"
#include "implicits/blob.h"
#include "implicits/blobField.h"
#include "implicits/capsule.h"
#include "implicits/intersection.h"
#include "implicits/rotate.h"
#include "implicits/sphere.h"
#include "implicits/rectangle.h"
#include "implicits/threshold.h"
#include "implicits/blobField.h"
#include "implicits/move.h"
#include "implicits/not.h"
#include "implicits/rotate.h"
#include "implicits/ringify.h"
#include "implicits/union.h"
#include "realtime.h"

#include <QtNodes/StyleCollection>

namespace {

QDoubleSpinBox* makeDoubleSpinBox(double minimum,
                                 double maximum,
                                 double step,
                                 int decimals,
                                 double value)
{
    auto* box = new QDoubleSpinBox;
    box->setRange(minimum, maximum);
    box->setSingleStep(step);
    box->setDecimals(decimals);
    box->setValue(value);
    box->setMinimumWidth(90);
    box->setMaximumWidth(120);
    box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    return box;
}

void applyNodePalette(QWidget* widget)
{
    const auto& nodeStyle = QtNodes::StyleCollection::nodeStyle();

    const QColor text = nodeStyle.FontColor;
    const QColor faded = nodeStyle.FontColorFaded;
    const QColor base = nodeStyle.GradientColor2.darker(115);
    const QColor alternate = nodeStyle.GradientColor2.darker(105);
    const QColor button = nodeStyle.GradientColor2.lighter(135);
    const QColor border = nodeStyle.ConnectionPointColor.darker(145);
    const QColor accent = nodeStyle.FilledConnectionPointColor;

    QPalette palette = widget->palette();
    palette.setColor(QPalette::Window, Qt::transparent);
    palette.setColor(QPalette::WindowText, text);
    palette.setColor(QPalette::Base, base);
    palette.setColor(QPalette::AlternateBase, alternate);
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::Button, button);
    palette.setColor(QPalette::ButtonText, text);
    palette.setColor(QPalette::BrightText, text);
    palette.setColor(QPalette::Highlight, accent);
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::ToolTipBase, button);
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::Mid, border);
    palette.setColor(QPalette::Midlight, border.lighter(125));
    palette.setColor(QPalette::Dark, border.darker(150));
    palette.setColor(QPalette::Light, border.lighter(150));
    palette.setColor(QPalette::Shadow, nodeStyle.ShadowColor);
    palette.setColor(QPalette::Link, accent);
    palette.setColor(QPalette::LinkVisited, accent.darker(120));
    palette.setColor(QPalette::Disabled, QPalette::Base, base.darker(105));
    palette.setColor(QPalette::Disabled, QPalette::Text, faded);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, faded);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, faded);

    widget->setPalette(palette);
    widget->setAutoFillBackground(false);
}

double elapsedMs(const std::chrono::steady_clock::time_point& start)
{
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

QTableWidgetItem* makeItem(double value)
{
    auto* item = new QTableWidgetItem(QString::number(value, 'g', 8));
    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    return item;
}

}

QWidget* NodeModelBase::embeddedWidget()
{
    if (_widget)
        return _widget;

    _widget = new QWidget;
    applyNodePalette(_widget);

    auto* layout = new QVBoxLayout(_widget);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(3);

    auto* form = new QFormLayout;
    form->setContentsMargins(0, 0, 0, 0);
    form->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    form->setHorizontalSpacing(6);
    form->setVerticalSpacing(3);
    layout->addLayout(form);

    buildParameters(form);

    _timingLabel = new QLabel(_timingText.isEmpty() ? QStringLiteral("Not computed")
                                                   : _timingText);
    _timingLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _timingLabel->setToolTip(
        QStringLiteral("Self: inclusive sampled cost minus direct children.\n"
                       "Incl: sampled cost including all inputs.\n"
                       "Share: self / graph total.\n"
                       "Build: field construction time."));
    QPalette timingPalette = _timingLabel->palette();
    timingPalette.setColor(QPalette::WindowText, QtNodes::StyleCollection::nodeStyle().FontColorFaded);
    _timingLabel->setPalette(timingPalette);
    layout->addWidget(_timingLabel);

    _widget->adjustSize();
    return _widget;
}

void NodeModelBase::setTimingText(const QString& text)
{
    _timingText = text;
    if (_timingLabel) {
        _timingLabel->setText(text);
        refreshWidgetGeometry();
    }
}

void NodeModelBase::refreshWidgetGeometry()
{
    if (!_widget)
        return;

    if (auto* layout = _widget->layout())
        layout->activate();
    _widget->adjustSize();
    Q_EMIT requestNodeUpdate();
}

void NodeModelBase::buildParameters(QFormLayout*)
{
}

unsigned int ImplicitFieldNode::nPorts(QtNodes::PortType portType) const
{
    return portType == QtNodes::PortType::In ? inputCount() : 1u;
}

QtNodes::NodeDataType ImplicitFieldNode::dataType(QtNodes::PortType,
                                                  QtNodes::PortIndex) const
{
    return FieldData().type();
}

std::shared_ptr<QtNodes::NodeData> ImplicitFieldNode::outData(QtNodes::PortIndex)
{
    return _output;
}

void ImplicitFieldNode::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                  QtNodes::PortIndex port)
{
    if (port >= inputCount())
        return;

    if (_inputs.size() != inputCount())
        _inputs.resize(inputCount());

    _inputs[port] = std::dynamic_pointer_cast<FieldData>(data);
    rebuildAndEmit();
}

void ImplicitFieldNode::refresh()
{
    rebuildAndEmit();
}

void ImplicitFieldNode::rebuildAndEmit()
{
    const auto start = std::chrono::steady_clock::now();
    rebuildField();
    setBuildTime(elapsedMs(start));

    if (_field) {
        _output = std::make_shared<FieldData>(_field);
        Q_EMIT dataUpdated(0);
    } else {
        _output.reset();
        Q_EMIT dataInvalidated(0);
    }

    Q_EMIT parametersChanged();
}

BlobNodeModel::BlobNodeModel()
{
    rebuildAndEmit();
}

QString BlobNodeModel::portCaption(QtNodes::PortType portType,
                                   QtNodes::PortIndex) const
{
    return portType == QtNodes::PortType::Out ? QStringLiteral("Out") : QString();
}

void BlobNodeModel::rebuildField()
{
    _field = std::make_shared<Blob>(_position, _peak, _radius);
}

void BlobNodeModel::setParameters(const Vector& position, double peak, double radius)
{
    _position = position;
    _peak = peak;
    _radius = radius;
    rebuildAndEmit();
    syncUi();
}

void BlobNodeModel::buildParameters(QFormLayout* form)
{
    _x = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _position[0]);
    _y = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _position[1]);
    _z = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _position[2]);
    _peakBox = makeDoubleSpinBox(-10.0, 10.0, 0.05, 4, _peak);
    _radiusBox = makeDoubleSpinBox(0.001, 100.0, 0.05, 4, _radius);

    form->addRow(QStringLiteral("X"), _x);
    form->addRow(QStringLiteral("Y"), _y);
    form->addRow(QStringLiteral("Z"), _z);
    form->addRow(QStringLiteral("Peak"), _peakBox);
    form->addRow(QStringLiteral("Radius"), _radiusBox);

    const auto update = [this]() {
        _position = Vector(_x->value(), _y->value(), _z->value());
        _peak = _peakBox->value();
        _radius = _radiusBox->value();
        rebuildAndEmit();
    };

    connect(_x, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z, &QDoubleSpinBox::valueChanged, this, update);
    connect(_peakBox, &QDoubleSpinBox::valueChanged, this, update);
    connect(_radiusBox, &QDoubleSpinBox::valueChanged, this, update);
}

void BlobNodeModel::syncUi()
{
    if (!_x)
        return;

    {
        const QSignalBlocker bx(_x);
        const QSignalBlocker by(_y);
        const QSignalBlocker bz(_z);
        const QSignalBlocker bp(_peakBox);
        const QSignalBlocker br(_radiusBox);
        _x->setValue(_position[0]);
        _y->setValue(_position[1]);
        _z->setValue(_position[2]);
        _peakBox->setValue(_peak);
        _radiusBox->setValue(_radius);
    }
}

QJsonObject BlobNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["x"] = _position[0];
    json["y"] = _position[1];
    json["z"] = _position[2];
    json["peak"] = _peak;
    json["radius"] = _radius;
    return json;
}

void BlobNodeModel::load(const QJsonObject& data)
{
    _position = Vector(data.value("x").toDouble(_position[0]),
                       data.value("y").toDouble(_position[1]),
                       data.value("z").toDouble(_position[2]));
    _peak = data.value("peak").toDouble(_peak);
    _radius = data.value("radius").toDouble(_radius);
    rebuildAndEmit();
    syncUi();
}

SphereNodeModel::SphereNodeModel()
{
    rebuildAndEmit();
}

QString SphereNodeModel::portCaption(QtNodes::PortType portType,
                                     QtNodes::PortIndex) const
{
    return portType == QtNodes::PortType::Out ? QStringLiteral("Out") : QString();
}

void SphereNodeModel::rebuildField()
{
    _field = std::make_shared<Sphere>(_position, _radius);
}

void SphereNodeModel::setParameters(const Vector& position, double radius)
{
    _position = position;
    _radius = radius;
    rebuildAndEmit();
    syncUi();
}

void SphereNodeModel::buildParameters(QFormLayout* form)
{
    _x = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _position[0]);
    _y = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _position[1]);
    _z = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _position[2]);
    _radiusBox = makeDoubleSpinBox(0.001, 100.0, 0.05, 4, _radius);

    form->addRow(QStringLiteral("X"), _x);
    form->addRow(QStringLiteral("Y"), _y);
    form->addRow(QStringLiteral("Z"), _z);
    form->addRow(QStringLiteral("Radius"), _radiusBox);

    const auto update = [this]() {
        _position = Vector(_x->value(), _y->value(), _z->value());
        _radius = _radiusBox->value();
        rebuildAndEmit();
    };

    connect(_x, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z, &QDoubleSpinBox::valueChanged, this, update);
    connect(_radiusBox, &QDoubleSpinBox::valueChanged, this, update);
}

void SphereNodeModel::syncUi()
{
    if (!_x)
        return;

    const QSignalBlocker bx(_x);
    const QSignalBlocker by(_y);
    const QSignalBlocker bz(_z);
    const QSignalBlocker br(_radiusBox);
    _x->setValue(_position[0]);
    _y->setValue(_position[1]);
    _z->setValue(_position[2]);
    _radiusBox->setValue(_radius);
}

QJsonObject SphereNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["x"] = _position[0];
    json["y"] = _position[1];
    json["z"] = _position[2];
    json["radius"] = _radius;
    return json;
}

void SphereNodeModel::load(const QJsonObject& data)
{
    _position = Vector(data.value("x").toDouble(_position[0]),
                       data.value("y").toDouble(_position[1]),
                       data.value("z").toDouble(_position[2]));
    _radius = data.value("radius").toDouble(_radius);
    rebuildAndEmit();
    syncUi();
}

RectangleNodeModel::RectangleNodeModel()
{
    rebuildAndEmit();
}

QString RectangleNodeModel::portCaption(QtNodes::PortType portType,
                                        QtNodes::PortIndex) const
{
    return portType == QtNodes::PortType::In ? QString() : QStringLiteral("Field");
}

void RectangleNodeModel::rebuildField()
{
    _field = std::make_shared<Rectangle>(_point1, _point2);
}

void RectangleNodeModel::setParameters(const Vector& point1, const Vector& point2)
{
    _point1 = point1;
    _point2 = point2;
    if (_x1) _x1->setValue(_point1[0]);
    if (_y1) _y1->setValue(_point1[1]);
    if (_z1) _z1->setValue(_point1[2]);
    if (_x2) _x2->setValue(_point2[0]);
    if (_y2) _y2->setValue(_point2[1]);
    if (_z2) _z2->setValue(_point2[2]);
    syncUi();
    rebuildAndEmit();
}

void RectangleNodeModel::buildParameters(QFormLayout* form)
{
    _x1 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point1[0]);
    _y1 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point1[1]);
    _z1 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point1[2]);
    _x2 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point2[0]);
    _y2 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point2[1]);
    _z2 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point2[2]);

    form->addRow(QStringLiteral("Point1 X"), _x1);
    form->addRow(QStringLiteral("Point1 Y"), _y1);
    form->addRow(QStringLiteral("Point1 Z"), _z1);
    form->addRow(QStringLiteral("Point2 X"), _x2);
    form->addRow(QStringLiteral("Point2 Y"), _y2);
    form->addRow(QStringLiteral("Point2 Z"), _z2);

    const auto update = [this]() {
        _point1 = Vector(_x1->value(), _y1->value(), _z1->value());
        _point2 = Vector(_x2->value(), _y2->value(), _z2->value());
        syncUi();
        rebuildAndEmit();
        Q_EMIT parametersChanged();
    };

    connect(_x1, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y1, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z1, &QDoubleSpinBox::valueChanged, this, update);
    connect(_x2, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y2, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z2, &QDoubleSpinBox::valueChanged, this, update);
}

void RectangleNodeModel::syncUi()
{
    if (!_x1 || !_y1 || !_z1 || !_x2 || !_y2 || !_z2)
        return;

    const QSignalBlocker b1(_x1), b2(_y1), b3(_z1), b4(_x2), b5(_y2), b6(_z2);
    _x1->setValue(_point1[0]);
    _y1->setValue(_point1[1]);
    _z1->setValue(_point1[2]);
    _x2->setValue(_point2[0]);
    _y2->setValue(_point2[1]);
    _z2->setValue(_point2[2]);
}

QJsonObject RectangleNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["x1"] = _point1[0];
    json["y1"] = _point1[1];
    json["z1"] = _point1[2];
    json["x2"] = _point2[0];
    json["y2"] = _point2[1];
    json["z2"] = _point2[2];
    return json;
}

void RectangleNodeModel::load(const QJsonObject& data)
{
    _point1 = Vector(data.value("x1").toDouble(_point1[0]),
                      data.value("y1").toDouble(_point1[1]),
                      data.value("z1").toDouble(_point1[2]));
    _point2 = Vector(data.value("x2").toDouble(_point2[0]),
                      data.value("y2").toDouble(_point2[1]),
                      data.value("z2").toDouble(_point2[2]));
    rebuildAndEmit();
    syncUi();
}

CapsuleNodeModel::CapsuleNodeModel()
{
    rebuildAndEmit();
}

QString CapsuleNodeModel::portCaption(QtNodes::PortType portType,
                                      QtNodes::PortIndex) const
{
    return portType == QtNodes::PortType::Out ? QStringLiteral("Out") : QString();
}

void CapsuleNodeModel::rebuildField()
{
    _field = std::make_shared<Capsule>(_point1, _point2, _radius);
}

void CapsuleNodeModel::setParameters(const Vector& point1, const Vector& point2, double radius)
{
    _point1 = point1;
    _point2 = point2;
    _radius = radius;
    rebuildAndEmit();
    syncUi();
}

void CapsuleNodeModel::buildParameters(QFormLayout* form)
{
    _x1 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point1[0]);
    _y1 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point1[1]);
    _z1 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point1[2]);
    _x2 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point2[0]);
    _y2 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point2[1]);
    _z2 = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _point2[2]);
    _radiusBox = makeDoubleSpinBox(0.0, 100.0, 0.05, 4, _radius);

    form->addRow(QStringLiteral("Point1 X"), _x1);
    form->addRow(QStringLiteral("Point1 Y"), _y1);
    form->addRow(QStringLiteral("Point1 Z"), _z1);
    form->addRow(QStringLiteral("Point2 X"), _x2);
    form->addRow(QStringLiteral("Point2 Y"), _y2);
    form->addRow(QStringLiteral("Point2 Z"), _z2);
    form->addRow(QStringLiteral("Radius"), _radiusBox);

    const auto update = [this]() {
        _point1 = Vector(_x1->value(), _y1->value(), _z1->value());
        _point2 = Vector(_x2->value(), _y2->value(), _z2->value());
        _radius = _radiusBox->value();
        rebuildAndEmit();
    };

    connect(_x1, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y1, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z1, &QDoubleSpinBox::valueChanged, this, update);
    connect(_x2, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y2, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z2, &QDoubleSpinBox::valueChanged, this, update);
    connect(_radiusBox, &QDoubleSpinBox::valueChanged, this, update);
}

void CapsuleNodeModel::syncUi()
{
    if (!_x1 || !_y1 || !_z1 || !_x2 || !_y2 || !_z2 || !_radiusBox)
        return;

    const QSignalBlocker b1(_x1), b2(_y1), b3(_z1), b4(_x2), b5(_y2), b6(_z2),
        b7(_radiusBox);
    _x1->setValue(_point1[0]);
    _y1->setValue(_point1[1]);
    _z1->setValue(_point1[2]);
    _x2->setValue(_point2[0]);
    _y2->setValue(_point2[1]);
    _z2->setValue(_point2[2]);
    _radiusBox->setValue(_radius);
}

QJsonObject CapsuleNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["x1"] = _point1[0];
    json["y1"] = _point1[1];
    json["z1"] = _point1[2];
    json["x2"] = _point2[0];
    json["y2"] = _point2[1];
    json["z2"] = _point2[2];
    json["radius"] = _radius;
    return json;
}

void CapsuleNodeModel::load(const QJsonObject& data)
{
    _point1 = Vector(data.value("x1").toDouble(_point1[0]),
                      data.value("y1").toDouble(_point1[1]),
                      data.value("z1").toDouble(_point1[2]));
    _point2 = Vector(data.value("x2").toDouble(_point2[0]),
                      data.value("y2").toDouble(_point2[1]),
                      data.value("z2").toDouble(_point2[2]));
    _radius = data.value("radius").toDouble(_radius);
    rebuildAndEmit();
    syncUi();
}

QString ImplicitAddNodeModel::portCaption(QtNodes::PortType portType,
                                          QtNodes::PortIndex portIndex) const
{
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    if (portType == QtNodes::PortType::In)
        return portIndex == 0 ? QStringLiteral("A") : QStringLiteral("B");
    return QString();
}

void ImplicitAddNodeModel::rebuildField()
{
    if (_inputs.size() < 2 || !_inputs[0] || !_inputs[1]
        || !_inputs[0]->field() || !_inputs[1]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<ImplicitAdd>(_inputs[0]->field(), _inputs[1]->field());
}

QString ImplicitUnionNodeModel::portCaption(QtNodes::PortType portType,
                                            QtNodes::PortIndex portIndex) const
{
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    if (portType == QtNodes::PortType::In)
        return portIndex == 0 ? QStringLiteral("A") : QStringLiteral("B");
    return QString();
}

void ImplicitUnionNodeModel::rebuildField()
{
    if (_inputs.size() < 2 || !_inputs[0] || !_inputs[1]
        || !_inputs[0]->field() || !_inputs[1]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<ImplicitUnion>(_inputs[0]->field(), _inputs[1]->field());
}

QString ImplicitIntersectionNodeModel::portCaption(QtNodes::PortType portType,
                                                   QtNodes::PortIndex portIndex) const
{
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    if (portType == QtNodes::PortType::In)
        return portIndex == 0 ? QStringLiteral("A") : QStringLiteral("B");
    return QString();
}

void ImplicitIntersectionNodeModel::rebuildField()
{
    if (_inputs.size() < 2 || !_inputs[0] || !_inputs[1]
        || !_inputs[0]->field() || !_inputs[1]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<ImplicitIntersection>(_inputs[0]->field(),
                                                    _inputs[1]->field());
}

QString ImplicitNotNodeModel::portCaption(QtNodes::PortType portType,
                                          QtNodes::PortIndex) const
{
    if (portType == QtNodes::PortType::In)
        return QStringLiteral("Field");
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    return QString();
}

void ImplicitNotNodeModel::rebuildField()
{
    if (_inputs.empty() || !_inputs[0] || !_inputs[0]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<ImplicitNot>(_inputs[0]->field());
}

MoveNodeModel::MoveNodeModel()
{
    rebuildAndEmit();
}

QString MoveNodeModel::portCaption(QtNodes::PortType portType,
                                   QtNodes::PortIndex) const
{
    if (portType == QtNodes::PortType::In)
        return QStringLiteral("Field");
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    return QString();
}

void MoveNodeModel::rebuildField()
{
    if (_inputs.empty() || !_inputs[0] || !_inputs[0]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<ImplicitMove>(_inputs[0]->field(), _offset);
}

void MoveNodeModel::buildParameters(QFormLayout* form)
{
    _x = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _offset[0]);
    _y = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _offset[1]);
    _z = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _offset[2]);

    form->addRow(QStringLiteral("X"), _x);
    form->addRow(QStringLiteral("Y"), _y);
    form->addRow(QStringLiteral("Z"), _z);

    const auto update = [this]() {
        _offset = Vector(_x->value(), _y->value(), _z->value());
        rebuildAndEmit();
    };

    connect(_x, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z, &QDoubleSpinBox::valueChanged, this, update);
}

void MoveNodeModel::syncUi()
{
    if (!_x)
        return;

    const QSignalBlocker bx(_x);
    const QSignalBlocker by(_y);
    const QSignalBlocker bz(_z);
    _x->setValue(_offset[0]);
    _y->setValue(_offset[1]);
    _z->setValue(_offset[2]);
}

QJsonObject MoveNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["x"] = _offset[0];
    json["y"] = _offset[1];
    json["z"] = _offset[2];
    return json;
}

void MoveNodeModel::load(const QJsonObject& data)
{
    _offset = Vector(data.value("x").toDouble(_offset[0]),
                     data.value("y").toDouble(_offset[1]),
                     data.value("z").toDouble(_offset[2]));
    rebuildAndEmit();
    syncUi();
}
//------------------------------------------------
RingifyNodeModel::RingifyNodeModel()
{
    rebuildAndEmit();
}

QString RingifyNodeModel::portCaption(QtNodes::PortType portType,
                                   QtNodes::PortIndex) const
{
    if (portType == QtNodes::PortType::In)
        return QStringLiteral("Field");
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    return QString();
}

void RingifyNodeModel::rebuildField()
{
    if (_inputs.empty() || !_inputs[0] || !_inputs[0]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<ImplicitRingify>(_inputs[0]->field());
}

void RingifyNodeModel::buildParameters(QFormLayout* form){}

void RingifyNodeModel::syncUi(){}

QJsonObject RingifyNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    return json;
}

void RingifyNodeModel::load(const QJsonObject& data)
{
    rebuildAndEmit();
    syncUi();
}
//------------------------------------------------

RotateNodeModel::RotateNodeModel()
{
    rebuildAndEmit();
}

QString RotateNodeModel::portCaption(QtNodes::PortType portType,
                                     QtNodes::PortIndex) const
{
    if (portType == QtNodes::PortType::In)
        return QStringLiteral("Field");
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    return QString();
}

void RotateNodeModel::rebuildField()
{
    if (_inputs.empty() || !_inputs[0] || !_inputs[0]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<ImplicitRotation>(_inputs[0]->field(), _rotation);
}

void RotateNodeModel::buildParameters(QFormLayout* form)
{
    _x = makeDoubleSpinBox(-3.15, 3.15, 0.01, 2, _rotation[0]);
    _y = makeDoubleSpinBox(-3.15, 3.15, 0.01, 2, _rotation[1]);
    _z = makeDoubleSpinBox(-3.15, 3.15, 0.01, 2, _rotation[2]);

    form->addRow(QStringLiteral("X (rad)"), _x);
    form->addRow(QStringLiteral("Y (rad)"), _y);
    form->addRow(QStringLiteral("Z (rad)"), _z);

    const auto update = [this]() {
        _rotation = Vector(_x->value(), _y->value(), _z->value());
        rebuildAndEmit();
    };

    connect(_x, &QDoubleSpinBox::valueChanged, this, update);
    connect(_y, &QDoubleSpinBox::valueChanged, this, update);
    connect(_z, &QDoubleSpinBox::valueChanged, this, update);
}

void RotateNodeModel::syncUi()
{
    if (!_x)
        return;

    const QSignalBlocker bx(_x);
    const QSignalBlocker by(_y);
    const QSignalBlocker bz(_z);
    _x->setValue(_rotation[0]);
    _y->setValue(_rotation[1]);
    _z->setValue(_rotation[2]);
}

QJsonObject RotateNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["x"] = _rotation[0];
    json["y"] = _rotation[1];
    json["z"] = _rotation[2];
    return json;
}

void RotateNodeModel::load(const QJsonObject& data)
{
    _rotation = Vector(data.value("x").toDouble(_rotation[0]),
                       data.value("y").toDouble(_rotation[1]),
                       data.value("z").toDouble(_rotation[2]));
    rebuildAndEmit();
    syncUi();
}

ThresholdNodeModel::ThresholdNodeModel()
{
    rebuildAndEmit();
}

QString ThresholdNodeModel::portCaption(QtNodes::PortType portType,
                                        QtNodes::PortIndex) const
{
    if (portType == QtNodes::PortType::In)
        return QStringLiteral("Field");
    if (portType == QtNodes::PortType::Out)
        return QStringLiteral("Out");
    return QString();
}

void ThresholdNodeModel::rebuildField()
{
    if (_inputs.empty() || !_inputs[0] || !_inputs[0]->field()) {
        _field.reset();
        return;
    }

    _field = std::make_shared<Threshold>(_inputs[0]->field(), _threshold);
}

void ThresholdNodeModel::buildParameters(QFormLayout* form)
{
    _thresholdBox = makeDoubleSpinBox(-10.0, 10.0, 0.05, 4, _threshold);
    form->addRow(QStringLiteral("Threshold"), _thresholdBox);

    connect(_thresholdBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        _threshold = value;
        rebuildAndEmit();
    });
}

void ThresholdNodeModel::syncUi()
{
    if (!_thresholdBox)
        return;

    const QSignalBlocker blocker(_thresholdBox);
    _thresholdBox->setValue(_threshold);
}

QJsonObject ThresholdNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["threshold"] = _threshold;
    return json;
}

void ThresholdNodeModel::load(const QJsonObject& data)
{
    _threshold = data.value("threshold").toDouble(_threshold);
    rebuildAndEmit();
    syncUi();
}

BlobFieldNodeModel::BlobFieldNodeModel()
{
    _blobs = {
        BlobSpec{Vector(-0.8, 0.0, 0.0), 1.0, 0.7},
        BlobSpec{Vector(0.8, 0.0, 0.0), 1.0, 0.7},
        BlobSpec{Vector(0.0, 0.9, 0.0), 0.8, 0.6},
    };
    rebuildAndEmit();
}

QString BlobFieldNodeModel::portCaption(QtNodes::PortType portType,
                                        QtNodes::PortIndex) const
{
    return portType == QtNodes::PortType::Out ? QStringLiteral("Out") : QString();
}

void BlobFieldNodeModel::setBlobs(std::vector<BlobSpec> blobs)
{
    _blobs = std::move(blobs);
    rebuildAndEmit();
    syncUi();
}

void BlobFieldNodeModel::rebuildField()
{
    if (_blobs.empty()) {
        _field.reset();
        return;
    }

    const auto& first = _blobs.front();
    auto blobField = std::make_shared<BlobField>(
        Blob(first.position, first.peak, first.radius));

    for (std::size_t i = 1; i < _blobs.size(); ++i) {
        const auto& blob = _blobs[i];
        blobField->add(Blob(blob.position, blob.peak, blob.radius));
    }

    _field = blobField;
}

void BlobFieldNodeModel::buildParameters(QFormLayout* form)
{
    _table = new QTableWidget(0, 5);
    _table->setHorizontalHeaderLabels({QStringLiteral("X"),
                                       QStringLiteral("Y"),
                                       QStringLiteral("Z"),
                                       QStringLiteral("Peak"),
                                       QStringLiteral("Radius")});
    _table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _table->verticalHeader()->setVisible(false);
    _table->setSelectionBehavior(QAbstractItemView::SelectRows);
    _table->setMinimumWidth(320);
    _table->setMaximumHeight(150);
    form->addRow(_table);

    auto* buttons = new QWidget;
    auto* buttonLayout = new QHBoxLayout(buttons);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    auto* addButton = new QPushButton(QStringLiteral("Add"));
    auto* removeButton = new QPushButton(QStringLiteral("Remove"));
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(removeButton);
    form->addRow(buttons);

    connect(_table, &QTableWidget::itemChanged, this, [this]() { readTable(); });
    connect(addButton, &QPushButton::clicked, this, [this]() {
        addRow(BlobSpec{Vector(0.0, 0.0, 0.0), 1.0, 0.8});
    });
    connect(removeButton, &QPushButton::clicked, this, [this]() {
        removeSelectedRows();
    });

    syncUi();
}

void BlobFieldNodeModel::addRow(const BlobSpec& blob, bool updateModel)
{
    if (!_table)
        return;

    _updatingTable = true;
    const int row = _table->rowCount();
    _table->insertRow(row);
    _table->setItem(row, 0, makeItem(blob.position[0]));
    _table->setItem(row, 1, makeItem(blob.position[1]));
    _table->setItem(row, 2, makeItem(blob.position[2]));
    _table->setItem(row, 3, makeItem(blob.peak));
    _table->setItem(row, 4, makeItem(blob.radius));
    _updatingTable = false;

    if (updateModel) {
        _blobs.push_back(blob);
        rebuildAndEmit();
        refreshWidgetGeometry();
    }
}

void BlobFieldNodeModel::removeSelectedRows()
{
    if (!_table)
        return;

    auto* selection = _table->selectionModel();
    if (!selection)
        return;

    const auto rows = selection->selectedRows();
    if (rows.isEmpty())
        return;

    _updatingTable = true;
    std::vector<int> indexes;
    indexes.reserve(rows.size());
    for (const auto& index : rows)
        indexes.push_back(index.row());
    std::sort(indexes.begin(), indexes.end(), std::greater<int>());
    for (int row : indexes)
        _table->removeRow(row);
    _updatingTable = false;

    readTable();
}

void BlobFieldNodeModel::readTable()
{
    if (_updatingTable || !_table)
        return;

    std::vector<BlobSpec> blobs;
    blobs.reserve(_table->rowCount());

    for (int row = 0; row < _table->rowCount(); ++row) {
        auto value = [this, row](int column, double fallback) {
            auto* item = _table->item(row, column);
            bool ok = false;
            const double parsed = item ? item->text().toDouble(&ok) : fallback;
            return ok ? parsed : fallback;
        };

        BlobSpec blob;
        blob.position = Vector(value(0, 0.0), value(1, 0.0), value(2, 0.0));
        blob.peak = value(3, 1.0);
        blob.radius = value(4, 0.8);
        blobs.push_back(blob);
    }

    _blobs = std::move(blobs);
    rebuildAndEmit();
    refreshWidgetGeometry();
}

void BlobFieldNodeModel::syncUi()
{
    if (!_table)
        return;

    _updatingTable = true;
    _table->clearContents();
    _table->setRowCount(static_cast<int>(_blobs.size()));
    for (int row = 0; row < static_cast<int>(_blobs.size()); ++row) {
        const auto& blob = _blobs[row];
        _table->setItem(row, 0, makeItem(blob.position[0]));
        _table->setItem(row, 1, makeItem(blob.position[1]));
        _table->setItem(row, 2, makeItem(blob.position[2]));
        _table->setItem(row, 3, makeItem(blob.peak));
        _table->setItem(row, 4, makeItem(blob.radius));
    }
    _updatingTable = false;
    if (_widget) {
        if (auto* layout = _widget->layout())
            layout->activate();
        _widget->adjustSize();
    }
}

QJsonObject BlobFieldNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    QJsonArray blobs;
    for (const auto& blob : _blobs) {
        QJsonObject entry;
        entry["x"] = blob.position[0];
        entry["y"] = blob.position[1];
        entry["z"] = blob.position[2];
        entry["peak"] = blob.peak;
        entry["radius"] = blob.radius;
        blobs.append(entry);
    }
    json["blobs"] = blobs;
    return json;
}

void BlobFieldNodeModel::load(const QJsonObject& data)
{
    const QJsonArray array = data.value("blobs").toArray();
    if (data.contains("blobs")) {
        _blobs.clear();
        for (const auto& value : array) {
            const QJsonObject entry = value.toObject();
            BlobSpec blob;
            blob.position = Vector(entry.value("x").toDouble(0.0),
                                   entry.value("y").toDouble(0.0),
                                   entry.value("z").toDouble(0.0));
            blob.peak = entry.value("peak").toDouble(1.0);
            blob.radius = entry.value("radius").toDouble(0.8);
            _blobs.push_back(blob);
        }
    }
    rebuildAndEmit();
    syncUi();
    refreshWidgetGeometry();
}

OutputNodeModel::OutputNodeModel() = default;

OutputNodeModel::~OutputNodeModel()
{
    delete _previewWindow;
    _previewWindow = nullptr;
    _detachedPreview = nullptr;
}

QString OutputNodeModel::portCaption(QtNodes::PortType portType,
                                     QtNodes::PortIndex) const
{
    return portType == QtNodes::PortType::In ? QStringLiteral("Field") : QString();
}

unsigned int OutputNodeModel::nPorts(QtNodes::PortType portType) const
{
    return portType == QtNodes::PortType::In ? 1u : 0u;
}

QtNodes::NodeDataType OutputNodeModel::dataType(QtNodes::PortType,
                                                QtNodes::PortIndex) const
{
    return FieldData().type();
}

std::shared_ptr<QtNodes::NodeData> OutputNodeModel::outData(QtNodes::PortIndex)
{
    return nullptr;
}

void OutputNodeModel::setInData(std::shared_ptr<QtNodes::NodeData> data,
                                QtNodes::PortIndex)
{
    _input = std::dynamic_pointer_cast<FieldData>(data);
    if (_connectionLabel) {
        _connectionLabel->setText(_input && _input->field() ? QStringLiteral("connected")
                                                           : QStringLiteral("no field"));
    }
    refreshWidgetGeometry();
    Q_EMIT parametersChanged();
}

OutputNodeModel::FieldPtr OutputNodeModel::field() const
{
    return _input ? _input->field() : nullptr;
}

Box OutputNodeModel::box() const
{
    return Box(_center, _halfSize);
}

void OutputNodeModel::setPreviewMesh(const Mesh& mesh)
{
    std::vector<Color> colors(static_cast<std::size_t>(mesh.Vertexes()),
                              Color(0.8, 0.8, 0.8));
    _previewMesh = std::make_shared<MeshColor>(mesh, colors, mesh.VertexIndexes());
    applyPreviewMesh();
}

void OutputNodeModel::clearPreview()
{
    _previewMesh.reset();
    applyPreviewMesh();
}

void OutputNodeModel::applyPreviewMesh()
{
    if (_vertexCountLabel) {
        _vertexCountLabel->setText(
            QString::number(_previewMesh ? _previewMesh->Vertexes() : 0));
    }
    if (_triangleCountLabel) {
        _triangleCountLabel->setText(
            QString::number(_previewMesh ? _previewMesh->Triangles() : 0));
    }

    const auto updateView = [this](MeshWidget* view) {
        if (!view || !view->isValid())
            return;

        view->ClearAll();
        if (_previewMesh)
            view->AddMesh(QStringLiteral("Mesh"), *_previewMesh);
    };

    updateView(_preview);
    updateView(_detachedPreview);
    applyPreviewMaterial();
}

void OutputNodeModel::applyPreviewMaterial()
{
    const bool wireframe = _wireframe && _wireframe->isChecked();
    const MeshMaterial material = (_colorShading && _colorShading->isChecked())
                                      ? MeshMaterial::Color
                                      : MeshMaterial::Normal;

    for (MeshWidget* view : {_preview, _detachedPreview}) {
        if (!view)
            continue;
        view->UseWireframeGlobal(wireframe);
        view->SetMaterialGlobal(material);
    }
    requestPreviewRepaint();
}

void OutputNodeModel::requestPreviewRepaint()
{
    if (_preview)
        _preview->update();
    if (_detachedPreview)
        _detachedPreview->update();

    if (_widget) {
        if (auto* proxy = _widget->graphicsProxyWidget()) {
            proxy->update();
            if (auto* scene = proxy->scene())
                scene->update(proxy->sceneBoundingRect());
        }
    }

    Q_EMIT requestNodeUpdate();
}

void OutputNodeModel::openPreviewWindow()
{
    if (!_previewWindow) {
        _previewWindow = new QWidget(nullptr, Qt::Window);
        _previewWindow->setAttribute(Qt::WA_NativeWindow);
        _previewWindow->setWindowTitle(QStringLiteral("Mesh Output Preview"));

        auto* layout = new QVBoxLayout(_previewWindow);
        layout->setContentsMargins(0, 0, 0, 0);

        _detachedPreview = new MeshWidget;
        _detachedPreview->setMinimumSize(480, 360);
        _detachedPreview->setToolTip(QStringLiteral("Ctrl + left drag: rotate\n"
                                                    "Ctrl + right drag: zoom\n"
                                                    "Ctrl + middle drag: pan\n"
                                                    "Mouse wheel: zoom"));
        layout->addWidget(_detachedPreview);

        connect(_detachedPreview, &MeshWidget::_signalCameraChanged, this, [this]() {
            requestPreviewRepaint();
        });
        connect(_detachedPreview, &MeshWidget::_signalUpdate, this, [this]() {
            if (_detachedPreviewNeedsSync && _detachedPreview->isValid()) {
                _detachedPreviewNeedsSync = false;
                applyPreviewMesh();
            }
        });

        if (_preview && _preview->isValid()) {
            _detachedPreview->SetCameraMode(_preview->GetPerspectiveProjection());
            _detachedPreview->SetCamera(_preview->GetCamera());
        }

        _previewWindow->resize(640, 480);
        _detachedPreviewNeedsSync = true;
        applyPreviewMesh();
    }

    _previewWindow->show();
    _previewWindow->raise();
    _previewWindow->activateWindow();
}

void OutputNodeModel::buildParameters(QFormLayout* form)
{
    _preview = new MeshWidget;
    _preview->setFixedSize(360, 240);
    _preview->setToolTip(QStringLiteral("Ctrl + left drag: rotate\n"
                                        "Ctrl + right drag: zoom\n"
                                        "Ctrl + middle drag: pan\n"
                                        "Mouse wheel: zoom\n"
                                        "Double-click: open in a new window"));
    connect(_preview, &MeshWidget::_signalCameraChanged, this, [this]() {
        requestPreviewRepaint();
    });
    connect(_preview, &MeshWidget::_signalDoubleClicked, this, [this]() {
        openPreviewWindow();
    });
    form->addRow(_preview);

    auto* stats = new QWidget;
    auto* statsLayout = new QHBoxLayout(stats);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->setSpacing(6);
    _vertexCountLabel = new QLabel(QStringLiteral("0"));
    _triangleCountLabel = new QLabel(QStringLiteral("0"));
    statsLayout->addWidget(new QLabel(QStringLiteral("Vertices")));
    statsLayout->addWidget(_vertexCountLabel);
    statsLayout->addSpacing(10);
    statsLayout->addWidget(new QLabel(QStringLiteral("Triangles")));
    statsLayout->addWidget(_triangleCountLabel);
    statsLayout->addStretch(1);

    auto* recomputeButton = new QPushButton(QStringLiteral("Recompute"));
    recomputeButton->setToolTip(QStringLiteral("Rebuild only this Mesh Output node."));
    statsLayout->addWidget(recomputeButton);
    form->addRow(stats);

    auto* viewControls = new QWidget;
    auto* viewLayout = new QHBoxLayout(viewControls);
    viewLayout->setContentsMargins(0, 0, 0, 0);
    viewLayout->setSpacing(8);
    _wireframe = new QCheckBox(QStringLiteral("Wireframe"));
    _wireframe->setChecked(true);
    _normalShading = new QRadioButton(QStringLiteral("Normal"));
    _colorShading = new QRadioButton(QStringLiteral("Color"));
    _normalShading->setChecked(true);
    auto* resetCameraButton = new QPushButton(QStringLiteral("Reset camera"));
    viewLayout->addWidget(_wireframe);
    viewLayout->addWidget(_normalShading);
    viewLayout->addWidget(_colorShading);
    viewLayout->addStretch(1);
    viewLayout->addWidget(resetCameraButton);
    form->addRow(viewControls);

    connect(_wireframe, &QCheckBox::toggled, this, [this](bool) {
        applyPreviewMaterial();
    });
    connect(_normalShading, &QRadioButton::toggled, this, [this](bool) {
        applyPreviewMaterial();
    });
    connect(_colorShading, &QRadioButton::toggled, this, [this](bool) {
        applyPreviewMaterial();
    });
    connect(recomputeButton, &QPushButton::clicked, this, [this]() {
        Q_EMIT recomputeRequested();
    });
    connect(resetCameraButton, &QPushButton::clicked, this, [this]() {
        if (_preview) {
            _preview->SetCamera(Camera(Vector(-10.0), Vector(0.0)));
            requestPreviewRepaint();
        }
    });

    _cx = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _center[0]);
    _cy = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _center[1]);
    _cz = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _center[2]);
    _halfSizeBox = makeDoubleSpinBox(0.01, 100.0, 0.1, 3, _halfSize);
    _resolutionBox = new QSpinBox;
    _resolutionBox->setRange(8, 512);
    _resolutionBox->setSingleStep(8);
    _resolutionBox->setValue(_resolution);
    _resolutionBox->setMinimumWidth(90);
    _resolutionBox->setMaximumWidth(120);
    _resolutionBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _epsilonBox = makeDoubleSpinBox(1.0e-8, 1.0, 1.0e-5, 8, _epsilon);

    form->addRow(QStringLiteral("Center X"), _cx);
    form->addRow(QStringLiteral("Center Y"), _cy);
    form->addRow(QStringLiteral("Center Z"), _cz);
    form->addRow(QStringLiteral("Half size"), _halfSizeBox);
    form->addRow(QStringLiteral("Resolution"), _resolutionBox);
    form->addRow(QStringLiteral("Epsilon"), _epsilonBox);

    _connectionLabel = new QLabel(_input && _input->field() ? QStringLiteral("connected")
                                                            : QStringLiteral("no field"));
    form->addRow(QStringLiteral("Input"), _connectionLabel);

    const auto update = [this]() {
        _center = Vector(_cx->value(), _cy->value(), _cz->value());
        _halfSize = _halfSizeBox->value();
        _resolution = _resolutionBox->value();
        _epsilon = _epsilonBox->value();
        Q_EMIT parametersChanged();
    };

    connect(_cx, &QDoubleSpinBox::valueChanged, this, update);
    connect(_cy, &QDoubleSpinBox::valueChanged, this, update);
    connect(_cz, &QDoubleSpinBox::valueChanged, this, update);
    connect(_halfSizeBox, &QDoubleSpinBox::valueChanged, this, update);
    connect(_resolutionBox, &QSpinBox::valueChanged, this, update);
    connect(_epsilonBox, &QDoubleSpinBox::valueChanged, this, update);

    applyPreviewMesh();
}

void OutputNodeModel::syncUi()
{
    if (!_cx)
        return;

    const QSignalBlocker bcx(_cx);
    const QSignalBlocker bcy(_cy);
    const QSignalBlocker bcz(_cz);
    const QSignalBlocker bhs(_halfSizeBox);
    const QSignalBlocker br(_resolutionBox);
    const QSignalBlocker be(_epsilonBox);
    _cx->setValue(_center[0]);
    _cy->setValue(_center[1]);
    _cz->setValue(_center[2]);
    _halfSizeBox->setValue(_halfSize);
    _resolutionBox->setValue(_resolution);
    _epsilonBox->setValue(_epsilon);
    if (_connectionLabel) {
        _connectionLabel->setText(_input && _input->field() ? QStringLiteral("connected")
                                                            : QStringLiteral("no field"));
    }
}

QJsonObject OutputNodeModel::save() const
{
    QJsonObject json = NodeDelegateModel::save();
    json["center-x"] = _center[0];
    json["center-y"] = _center[1];
    json["center-z"] = _center[2];
    json["half-size"] = _halfSize;
    json["resolution"] = _resolution;
    json["epsilon"] = _epsilon;
    return json;
}

void OutputNodeModel::load(const QJsonObject& data)
{
    _center = Vector(data.value("center-x").toDouble(_center[0]),
                     data.value("center-y").toDouble(_center[1]),
                     data.value("center-z").toDouble(_center[2]));
    _halfSize = data.value("half-size").toDouble(_halfSize);
    _resolution = data.value("resolution").toInt(_resolution);
    _epsilon = data.value("epsilon").toDouble(_epsilon);
    syncUi();
    refreshWidgetGeometry();
    Q_EMIT parametersChanged();
}
