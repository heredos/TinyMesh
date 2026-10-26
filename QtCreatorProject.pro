QT += core gui opengl concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

CONFIG += c++17
DEFINES += NODE_EDITOR_STATIC

INCLUDEPATH += AppTinyMesh/Include
INCLUDEPATH += external/nodeeditor/include
INCLUDEPATH += external/nodeeditor/include/QtNodes/internal
INCLUDEPATH += external/nodeeditor/src
INCLUDEPATH += $$(GLEW_DIR)
INCLUDEPATH += $$(OUT_PWD)

VPATH += AppTinyMesh

SOURCES += \
    AppTinyMesh/Source/box.cpp \
    AppTinyMesh/Source/evector.cpp \
    AppTinyMesh/Source/implicits.cpp \
    AppTinyMesh/Source/main.cpp \
    AppTinyMesh/Source/camera.cpp \
    AppTinyMesh/Source/mesh.cpp \
    AppTinyMesh/Source/meshcolor.cpp \
    AppTinyMesh/Source/mesh-widget.cpp \
    AppTinyMesh/Source/qtemainwindow.cpp \
    AppTinyMesh/Source/ray.cpp \
    AppTinyMesh/Source/shader-api.cpp \
    AppTinyMesh/Source/triangle.cpp \
    AppTinyMesh/Source/implicits/blob.cpp \
    AppTinyMesh/Source/implicits/add.cpp \
    AppTinyMesh/Source/implicits/blobField.cpp \
    AppTinyMesh/Source/implicits/sphere.cpp \
    AppTinyMesh/Source/nodes/GraphEvaluator.cpp \
    AppTinyMesh/Source/nodes/ImplicitNodeModels.cpp \
    AppTinyMesh/Source/nodes/NodeEditorWidget.cpp \
    external/nodeeditor/src/AbstractGraphModel.cpp \
    external/nodeeditor/src/AbstractNodeGeometry.cpp \
    external/nodeeditor/src/BasicGraphicsScene.cpp \
    external/nodeeditor/src/ConnectionGraphicsObject.cpp \
    external/nodeeditor/src/ConnectionState.cpp \
    external/nodeeditor/src/ConnectionStyle.cpp \
    external/nodeeditor/src/DataFlowGraphModel.cpp \
    external/nodeeditor/src/DataFlowGraphicsScene.cpp \
    external/nodeeditor/src/DefaultConnectionPainter.cpp \
    external/nodeeditor/src/DefaultHorizontalNodeGeometry.cpp \
    external/nodeeditor/src/DefaultNodePainter.cpp \
    external/nodeeditor/src/DefaultVerticalNodeGeometry.cpp \
    external/nodeeditor/src/Definitions.cpp \
    external/nodeeditor/src/GraphicsView.cpp \
    external/nodeeditor/src/GraphicsViewStyle.cpp \
    external/nodeeditor/src/NodeConnectionInteraction.cpp \
    external/nodeeditor/src/NodeDelegateModel.cpp \
    external/nodeeditor/src/NodeDelegateModelRegistry.cpp \
    external/nodeeditor/src/NodeGraphicsObject.cpp \
    external/nodeeditor/src/NodeState.cpp \
    external/nodeeditor/src/NodeStyle.cpp \
    external/nodeeditor/src/StyleCollection.cpp \
    external/nodeeditor/src/UndoCommands.cpp \
    external/nodeeditor/src/locateNode.cpp \

HEADERS += \
    AppTinyMesh/Include/box.h \
    AppTinyMesh/Include/camera.h \
    AppTinyMesh/Include/color.h \
    AppTinyMesh/Include/implicits.h \
    AppTinyMesh/Include/mathematics.h \
    AppTinyMesh/Include/mesh.h \
    AppTinyMesh/Include/meshcolor.h \
    AppTinyMesh/Include/qte.h \
    AppTinyMesh/Include/realtime.h \
    AppTinyMesh/Include/shader-api.h \
    AppTinyMesh/Include/implicits/blob.h \
    AppTinyMesh/Include/implicits/threshold.h \
    AppTinyMesh/Include/implicits/add.h \
    AppTinyMesh/Include/implicits/blobField.h \
    AppTinyMesh/Include/implicits/sphere.h \
    AppTinyMesh/Include/nodes/FieldData.h \
    AppTinyMesh/Include/nodes/GraphEvaluator.h \
    AppTinyMesh/Include/nodes/ImplicitNodeModels.h \
    AppTinyMesh/Include/nodes/NodeEditorWidget.h \
    external/nodeeditor/include/QtNodes/internal/AbstractConnectionPainter.hpp \
    external/nodeeditor/include/QtNodes/internal/AbstractGraphModel.hpp \
    external/nodeeditor/include/QtNodes/internal/AbstractNodeGeometry.hpp \
    external/nodeeditor/include/QtNodes/internal/AbstractNodePainter.hpp \
    external/nodeeditor/include/QtNodes/internal/BasicGraphicsScene.hpp \
    external/nodeeditor/include/QtNodes/internal/Compiler.hpp \
    external/nodeeditor/include/QtNodes/internal/ConnectionGraphicsObject.hpp \
    external/nodeeditor/include/QtNodes/internal/ConnectionIdHash.hpp \
    external/nodeeditor/include/QtNodes/internal/ConnectionIdUtils.hpp \
    external/nodeeditor/include/QtNodes/internal/ConnectionState.hpp \
    external/nodeeditor/include/QtNodes/internal/ConnectionStyle.hpp \
    external/nodeeditor/include/QtNodes/internal/DataFlowGraphicsScene.hpp \
    external/nodeeditor/include/QtNodes/internal/DataFlowGraphModel.hpp \
    external/nodeeditor/include/QtNodes/internal/DefaultConnectionPainter.hpp \
    external/nodeeditor/include/QtNodes/internal/DefaultHorizontalNodeGeometry.hpp \
    external/nodeeditor/include/QtNodes/internal/DefaultNodePainter.hpp \
    external/nodeeditor/include/QtNodes/internal/DefaultVerticalNodeGeometry.hpp \
    external/nodeeditor/include/QtNodes/internal/Definitions.hpp \
    external/nodeeditor/include/QtNodes/internal/Export.hpp \
    external/nodeeditor/include/QtNodes/internal/GraphicsView.hpp \
    external/nodeeditor/include/QtNodes/internal/GraphicsViewStyle.hpp \
    external/nodeeditor/include/QtNodes/internal/locateNode.hpp \
    external/nodeeditor/include/QtNodes/internal/NodeConnectionInteraction.hpp \
    external/nodeeditor/include/QtNodes/internal/NodeData.hpp \
    external/nodeeditor/include/QtNodes/internal/NodeDelegateModel.hpp \
    external/nodeeditor/include/QtNodes/internal/NodeDelegateModelRegistry.hpp \
    external/nodeeditor/include/QtNodes/internal/NodeGraphicsObject.hpp \
    external/nodeeditor/include/QtNodes/internal/NodeState.hpp \
    external/nodeeditor/include/QtNodes/internal/NodeStyle.hpp \
    external/nodeeditor/include/QtNodes/internal/OperatingSystem.hpp \
    external/nodeeditor/include/QtNodes/internal/QStringStdHash.hpp \
    external/nodeeditor/include/QtNodes/internal/QUuidStdHash.hpp \
    external/nodeeditor/include/QtNodes/internal/Serializable.hpp \
    external/nodeeditor/include/QtNodes/internal/Style.hpp \
    external/nodeeditor/include/QtNodes/internal/StyleCollection.hpp \
    external/nodeeditor/include/QtNodes/internal/UndoCommands.hpp \

FORMS += \
    AppTinyMesh/UI/interface.ui

RESOURCES += \
    external/nodeeditor/resources/resources.qrc

win32 {
    LIBS += -L$$(GLEW_DIR) -lglew32
    LIBS += -lopengl32 -lglu32
}
unix:!macx {
    LIBS += -lGLEW -lGL -lGLU
}
macx {
    LIBS += -lGLEW -lGL -lGLU
}

# Copy shader files
# $$shell_quote puts quote around the path, to make it work if it contains space or other special characters.
copydata.commands = $(COPY_DIR) $$shell_quote($$PWD/AppTinyMesh/Shaders) $$shell_quote($$OUT_PWD/Shaders)
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
