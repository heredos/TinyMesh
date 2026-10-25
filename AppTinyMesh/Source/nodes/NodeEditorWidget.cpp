#include "nodes/NodeEditorWidget.h"

#include <algorithm>

#include <QAction>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

#include "nodes/ImplicitNodeModels.h"

namespace {

std::shared_ptr<QtNodes::NodeDelegateModelRegistry> createRegistry()
{
    auto registry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    registry->registerModel<BlobNodeModel>("Generators");
    registry->registerModel<BlobFieldNodeModel>("Generators");
    registry->registerModel<ImplicitAddNodeModel>("Operators");
    registry->registerModel<ThresholdNodeModel>("Operators");
    registry->registerModel<OutputNodeModel>("Output");
    return registry;
}

}

NodeEditorWidget::NodeEditorWidget(QWidget* parent)
    : QWidget(parent)
    , _registry(createRegistry())
{
    _model = new QtNodes::DataFlowGraphModel(_registry);
    _model->setParent(this);

    _scene = new QtNodes::DataFlowGraphicsScene(*_model, this);

    buildUi();
    loadDemoGraph();
}

NodeEditorWidget::~NodeEditorWidget()
{
    delete _view;
    _view = nullptr;
    delete _scene;
    _scene = nullptr;
    delete _model;
    _model = nullptr;
}

void NodeEditorWidget::buildUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    auto* toolbar = new QHBoxLayout;
    toolbar->setContentsMargins(0, 0, 0, 0);

    _recomputeButton = new QPushButton(QStringLiteral("Recompute"));
    _recomputeButton->setToolTip(QStringLiteral("Rebuild the mesh from the node graph (F5)"));
    auto* demoButton = new QPushButton(QStringLiteral("Demo"));
    auto* saveButton = new QPushButton(QStringLiteral("Save graph"));
    auto* loadButton = new QPushButton(QStringLiteral("Load graph"));

    _statusLabel = new QLabel(QStringLiteral("Ready."));
    _statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    _statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    toolbar->addWidget(_recomputeButton);
    toolbar->addWidget(demoButton);
    toolbar->addWidget(saveButton);
    toolbar->addWidget(loadButton);
    toolbar->addSpacing(8);
    toolbar->addWidget(_statusLabel, 1);
    layout->addLayout(toolbar);

    _view = new QtNodes::GraphicsView(_scene, this);
    _view->setToolTip(QStringLiteral("Right-click to add a node."));
    _view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_view, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
        QMenu menu;
        const auto addAction = [&menu](const QString& label, const QString& type) {
            QAction* action = menu.addAction(label);
            action->setData(type);
        };
        addAction(QStringLiteral("Blob"), QStringLiteral("Blob"));
        addAction(QStringLiteral("Blob Field"), QStringLiteral("BlobField"));
        addAction(QStringLiteral("Implicit Add"), QStringLiteral("ImplicitAdd"));
        addAction(QStringLiteral("Threshold"), QStringLiteral("Threshold"));
        addAction(QStringLiteral("Mesh Output"), QStringLiteral("MeshOutput"));

        QAction* selected = menu.exec(_view->viewport()->mapToGlobal(pos));
        if (!selected)
            return;

        const QString type = selected->data().toString();
        const auto id = _model->addNode(type);
        _model->setNodeData(id, QtNodes::NodeRole::Position, _view->mapToScene(pos));
        setStatus(QStringLiteral("Added %1 node. Press Recompute to rebuild the mesh.")
                      .arg(type));
    });

    layout->addWidget(_view, 1);

    connect(_recomputeButton, &QPushButton::clicked, this, &NodeEditorWidget::recompute);
    connect(demoButton, &QPushButton::clicked, this, &NodeEditorWidget::loadDemoGraph);
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        if (_scene->save())
            setStatus(QStringLiteral("Graph saved."));
    });
    connect(loadButton, &QPushButton::clicked, this, [this]() {
        if (_scene->load()) {
            ++_generation;
            QTimer::singleShot(0, _view, &QtNodes::GraphicsView::centerScene);
            setStatus(QStringLiteral("Graph loaded. Press Recompute to rebuild the mesh."));
        }
    });

    connect(_scene, &QtNodes::DataFlowGraphicsScene::sceneLoaded,
            _view, &QtNodes::GraphicsView::centerScene);

    connect(_scene, &QtNodes::DataFlowGraphicsScene::modified, this,
            [this](QtNodes::BasicGraphicsScene*) { ++_generation; });

    connect(_model, &QtNodes::DataFlowGraphModel::nodeCreated, this,
            [this](QtNodes::NodeId id) {
                if (auto* node = _model->delegateModel<NodeModelBase>(id)) {
                    connect(node, &NodeModelBase::parametersChanged, this,
                            [this]() { ++_generation; });
                    connect(node, &NodeModelBase::recomputeRequested, this,
                            &NodeEditorWidget::recompute);
                }
            });
}

void NodeEditorWidget::loadDemoGraph()
{
    ++_generation;
    _scene->clearScene();

    const auto addNode = [this](const QString& type, const QPointF& position) {
        const auto id = _model->addNode(type);
        _model->setNodeData(id, QtNodes::NodeRole::Position, position);
        return id;
    };

    const auto blob1Id = addNode(QStringLiteral("Blob"), QPointF(-360, -120));
    const auto blob2Id = addNode(QStringLiteral("Blob"), QPointF(-360, 120));
    const auto addId = addNode(QStringLiteral("ImplicitAdd"), QPointF(-100, 0));
    const auto thresholdId = addNode(QStringLiteral("Threshold"), QPointF(150, 0));
    const auto outputId = addNode(QStringLiteral("MeshOutput"), QPointF(400, 0));

    if (auto* blob1 = _model->delegateModel<BlobNodeModel>(blob1Id))
        blob1->setParameters(Vector(-0.8, 0.0, 0.0), 1.0, 0.7);
    if (auto* blob2 = _model->delegateModel<BlobNodeModel>(blob2Id))
        blob2->setParameters(Vector(0.8, 0.0, 0.0), 1.0, 0.7);

    _model->addConnection(QtNodes::ConnectionId{blob1Id, 0, addId, 0});
    _model->addConnection(QtNodes::ConnectionId{blob2Id, 0, addId, 1});
    _model->addConnection(QtNodes::ConnectionId{addId, 0, thresholdId, 0});
    _model->addConnection(QtNodes::ConnectionId{thresholdId, 0, outputId, 0});

    QTimer::singleShot(0, _view, &QtNodes::GraphicsView::centerScene);

    setStatus(QStringLiteral("Demo graph loaded. Press Recompute to rebuild the mesh."));
}

void NodeEditorWidget::recompute()
{
    if (_busy)
        return;

    QString error;
    auto snapshot = GraphEvaluator::capture(*_model, error);
    if (!snapshot) {
        setStatus(error);
        return;
    }

    _busy = true;
    _recomputeButton->setEnabled(false);
    setNodeStatuses(QtNodes::NodeProcessingStatus::Processing);
    setStatus(QStringLiteral("Computing graph..."));

    const quint64 requestGeneration = _generation;
    auto* watcher = new QFutureWatcher<std::shared_ptr<EvaluationResult>>(this);

    connect(watcher, &QFutureWatcherBase::finished, this,
            [this, watcher, requestGeneration]() {
                const auto result = watcher->result();
                watcher->deleteLater();

                _busy = false;
                _recomputeButton->setEnabled(true);

                if (!result || !result->success) {
                    setNodeStatuses(QtNodes::NodeProcessingStatus::Failed);
                    setStatus(result ? result->error
                                     : QStringLiteral("Evaluation failed."));
                    return;
                }

                if (requestGeneration != _generation) {
                    setNodeStatuses(QtNodes::NodeProcessingStatus::Pending);
                    setStatus(QStringLiteral("Graph changed during computation. "
                                             "Press Recompute again."));
                    return;
                }

                setNodeStatuses(QtNodes::NodeProcessingStatus::Updated);

                updateNodeTimings(*result);

                const QString summary =
                    QStringLiteral("Total %1 ms | sample %2 ms | polygonize %3 ms | "
                                   "%4 vertices | %5 triangles | %6 samples")
                        .arg(result->totalMs, 0, 'f', 2)
                        .arg(result->benchmarkMs, 0, 'f', 2)
                        .arg(result->polygonizeMs, 0, 'f', 2)
                        .arg(result->vertices)
                        .arg(result->triangles)
                        .arg(result->sampledPoints);
                setStatus(summary);
            });

    watcher->setFuture(QtConcurrent::run([snapshot]() {
        return GraphEvaluator::evaluate(snapshot);
    }));
}

void NodeEditorWidget::setStatus(const QString& text)
{
    _statusLabel->setText(text);
    Q_EMIT statusMessage(text);
}

void NodeEditorWidget::setNodeStatuses(QtNodes::NodeProcessingStatus status)
{
    for (const auto id : _model->allNodeIds()) {
        if (auto* node = _model->delegateModel<NodeModelBase>(id))
            node->setNodeProcessingStatus(status);
    }
}

void NodeEditorWidget::updateNodeTimings(const EvaluationResult& result)
{
    for (const auto& profile : result.profiles) {
        if (auto* node = _model->delegateModel<NodeModelBase>(profile.id)) {
            const double share = result.benchmarkMs > 0.0
                                     ? 100.0 * profile.exclusiveMs / result.benchmarkMs
                                     : 0.0;
            node->setTimingText(
                QStringLiteral("self %1 ms | incl %2 ms\nshare %3% | build %4 us")
                    .arg(profile.exclusiveMs, 0, 'f', 2)
                    .arg(profile.inclusiveMs, 0, 'f', 2)
                    .arg(share, 0, 'f', 1)
                    .arg(profile.buildMs * 1000.0, 0, 'f', 0));
        }
    }

    for (const auto id : _model->allNodeIds()) {
        if (auto* output = _model->delegateModel<OutputNodeModel>(id)) {
            output->setTimingText(QStringLiteral("mesh %1 ms | graph %2 ms")
                                      .arg(result.polygonizeMs, 0, 'f', 2)
                                      .arg(result.totalMs, 0, 'f', 2));
            output->setPreviewMesh(result.mesh);
        }
    }
}
