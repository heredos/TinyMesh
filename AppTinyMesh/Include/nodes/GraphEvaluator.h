#pragma once

#include <memory>
#include <vector>

#include <QString>

#include "box.h"
#include "implicits.h"
#include "mesh.h"

namespace QtNodes {
class DataFlowGraphModel;
}

struct ProfileNode
{
    unsigned int id = 0;
    QString type;
    double buildMs = 0.0;
    AnalyticScalarField::ConstPtr field;
    std::vector<unsigned int> children;
};

struct GraphSnapshot
{
    AnalyticScalarField::ConstPtr root;
    std::vector<ProfileNode> nodes;
    Box box;
    int resolution = 64;
    double epsilon = 1.0e-4;
};

struct NodeProfileResult
{
    unsigned int id = 0;
    QString type;
    double buildMs = 0.0;
    double inclusiveMs = 0.0;
    double exclusiveMs = 0.0;
};

struct EvaluationResult
{
    bool success = false;
    QString error;
    Mesh mesh;
    std::vector<NodeProfileResult> profiles;
    double benchmarkMs = 0.0;
    double polygonizeMs = 0.0;
    double totalMs = 0.0;
    int sampledPoints = 0;
    int vertices = 0;
    int triangles = 0;
};

class GraphEvaluator
{
public:
    static std::shared_ptr<GraphSnapshot> capture(QtNodes::DataFlowGraphModel& model,
                                                  QString& error);
    static std::shared_ptr<EvaluationResult> evaluate(
        std::shared_ptr<const GraphSnapshot> snapshot);
};
