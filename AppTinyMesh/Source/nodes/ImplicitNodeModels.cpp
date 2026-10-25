#include "nodes/ImplicitNodeModels.h"

#include <algorithm>
#include <chrono>
#include <functional>

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "implicits/add.h"
#include "implicits/blob.h"
#include "implicits/blobField.h"
#include "implicits/threshold.h"

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
    return box;
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
    auto* layout = new QVBoxLayout(_widget);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    auto* form = new QFormLayout;
    form->setContentsMargins(0, 0, 0, 0);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->addLayout(form);

    buildParameters(form);

    _timingLabel = new QLabel(_timingText.isEmpty() ? QStringLiteral("not computed")
                                                   : _timingText);
    _timingLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _timingLabel->setToolTip(QStringLiteral("Inclusive sampled evaluation time"));
    layout->addWidget(_timingLabel);

    return _widget;
}

void NodeModelBase::setTimingText(const QString& text)
{
    _timingText = text;
    if (_timingLabel)
        _timingLabel->setText(text);
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
    _table->setMinimumWidth(360);
    _table->setMaximumHeight(180);
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
}

OutputNodeModel::OutputNodeModel() = default;

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

void OutputNodeModel::buildParameters(QFormLayout* form)
{
    _cx = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _center[0]);
    _cy = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _center[1]);
    _cz = makeDoubleSpinBox(-100.0, 100.0, 0.1, 3, _center[2]);
    _halfSizeBox = makeDoubleSpinBox(0.01, 100.0, 0.1, 3, _halfSize);
    _resolutionBox = new QSpinBox;
    _resolutionBox->setRange(8, 512);
    _resolutionBox->setSingleStep(8);
    _resolutionBox->setValue(_resolution);
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
    Q_EMIT parametersChanged();
}
