#include "qte.h"
#include "ui_interface.h"
#include "nodes/NodeEditorWidget.h"

#include <QAction>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QStatusBar>

MainWindow::MainWindow() : QMainWindow(), uiw(new Ui::Assets)
{
	uiw->setupUi(this);

	nodeEditor = new NodeEditorWidget(uiw->centralwidget);
	auto* centralLayout = qobject_cast<QHBoxLayout*>(uiw->centralwidget->layout());
	centralLayout->insertWidget(0, nodeEditor);
	centralLayout->setStretch(0, 1);

	connect(nodeEditor, &NodeEditorWidget::statusMessage, this, [this](const QString& message) {
		statusBar()->showMessage(message, 10000);
	});

	CreateActions();
}

MainWindow::~MainWindow()
{
	delete uiw;
}

void MainWindow::CreateActions()
{
	auto* recomputeAction = uiw->menuFile->addAction(tr("Recompute Graph"));
	recomputeAction->setShortcut(QKeySequence(Qt::Key_F5));
	connect(recomputeAction, &QAction::triggered, nodeEditor, &NodeEditorWidget::recompute);
}
