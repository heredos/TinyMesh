#pragma once

#include <memory>

#include <QWidget>

#include "nodes/GraphEvaluator.h"

class QLabel;
class QPushButton;

namespace QtNodes {
enum class NodeProcessingStatus : int;
class DataFlowGraphModel;
class DataFlowGraphicsScene;
class GraphicsView;
class NodeDelegateModelRegistry;
}

class NodeEditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NodeEditorWidget(QWidget* parent = nullptr);
    ~NodeEditorWidget() override;

    void loadDemoGraph();

public Q_SLOTS:
    void recompute();

Q_SIGNALS:
    void statusMessage(const QString& message);

private:
    void buildUi();
    void setStatus(const QString& text);
    void setNodeStatuses(QtNodes::NodeProcessingStatus status);
    void updateNodeTimings(const EvaluationResult& result);

    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> _registry;
    QtNodes::DataFlowGraphModel* _model = nullptr;
    QtNodes::DataFlowGraphicsScene* _scene = nullptr;
    QtNodes::GraphicsView* _view = nullptr;

    QPushButton* _recomputeButton = nullptr;
    QLabel* _statusLabel = nullptr;

    quint64 _generation = 0;
    bool _busy = false;
};
