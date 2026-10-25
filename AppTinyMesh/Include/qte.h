#ifndef __Qte__
#define __Qte__

#include <QtWidgets/qmainwindow.h>

QT_BEGIN_NAMESPACE
	namespace Ui { class Assets; }
QT_END_NAMESPACE

class NodeEditorWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT
private:
  Ui::Assets* uiw;           //!< Interface

  NodeEditorWidget* nodeEditor;

public:
  MainWindow();
  ~MainWindow();
  void CreateActions();
};

#endif
