#include "nodes/GraphEvaluator.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include <QtNodes/DataFlowGraphModel>

#include "nodes/ImplicitNodeModels.h"

namespace {

constexpr int kSampleCount = 4096;

double elapsedMs(const std::chrono::steady_clock::time_point& start)
{
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

}

std::shared_ptr<GraphSnapshot> GraphEvaluator::capture(QtNodes::DataFlowGraphModel& model,
                                                       QString& error)
{
    std::vector<OutputNodeModel*> outputs;
    QtNodes::NodeId outputId = QtNodes::InvalidNodeId;
    for (const auto id : model.allNodeIds()) {
        if (auto* output = model.delegateModel<OutputNodeModel>(id)) {
            outputs.push_back(output);
            outputId = id;
        }
    }

    if (outputs.empty()) {
        error = QStringLiteral("Add one Mesh Output node to the graph.");
        return nullptr;
    }

    if (outputs.size() > 1) {
        error = QStringLiteral("The graph must contain exactly one Mesh Output node.");
        return nullptr;
    }

    auto* output = outputs.front();
    if (!output->field()) {
        error = QStringLiteral("The Mesh Output node is not connected to a valid field.");
        return nullptr;
    }

    std::unordered_set<QtNodes::NodeId> reachable;
    std::function<void(QtNodes::NodeId)> collectReachable = [&](QtNodes::NodeId id) {
        if (!reachable.insert(id).second)
            return;

        auto* node = model.delegateModel<QtNodes::NodeDelegateModel>(id);
        if (!node)
            return;

        for (unsigned int port = 0; port < node->nPorts(QtNodes::PortType::In); ++port) {
            const auto connections = model.connections(id, QtNodes::PortType::In, port);
            for (const auto& connection : connections)
                collectReachable(connection.outNodeId);
        }
    };
    collectReachable(outputId);

    auto snapshot = std::make_shared<GraphSnapshot>();
    snapshot->root = output->field();
    snapshot->box = output->box();
    snapshot->resolution = output->resolution();
    snapshot->epsilon = output->epsilon();

    for (const auto id : model.allNodeIds()) {
        if (!reachable.count(id))
            continue;

        auto* node = model.delegateModel<ImplicitFieldNode>(id);
        if (!node)
            continue;

        auto field = node->field();
        if (!field)
            continue;

        ProfileNode profile;
        profile.id = id;
        profile.type = node->caption();
        profile.buildMs = node->buildMs();
        profile.field = std::move(field);

        for (unsigned int port = 0; port < node->inputCount(); ++port) {
            const auto connections = model.connections(id, QtNodes::PortType::In, port);
            for (const auto& connection : connections)
                profile.children.push_back(connection.outNodeId);
        }

        snapshot->nodes.push_back(std::move(profile));
    }

    return snapshot;
}

std::shared_ptr<EvaluationResult> GraphEvaluator::evaluate(
    std::shared_ptr<const GraphSnapshot> snapshot)
{
    auto result = std::make_shared<EvaluationResult>();

    if (!snapshot || !snapshot->root) {
        result->error = QStringLiteral("No field to evaluate.");
        return result;
    }

    try {
        result->sampledPoints = kSampleCount;

        const Vector center = snapshot->box.Center();
        const Vector halfSize = snapshot->box.Size() * 0.5;

        std::mt19937 generator(0x5EED1234u);
        std::uniform_real_distribution<double> distribution(-1.0, 1.0);

        std::vector<Vector> points;
        points.reserve(kSampleCount);
        for (int i = 0; i < kSampleCount; ++i) {
            points.emplace_back(center[0] + halfSize[0] * distribution(generator),
                                center[1] + halfSize[1] * distribution(generator),
                                center[2] + halfSize[2] * distribution(generator));
        }

        double accumulator = 0.0;
        result->profiles.reserve(snapshot->nodes.size());

        for (const auto& node : snapshot->nodes) {
            const auto* field = node.field.get();
            const auto start = std::chrono::steady_clock::now();

            for (const auto& point : points) {
                accumulator += field->Value(point);
                const Vector normal = field->Normal(point);
                accumulator += normal[0] + normal[1] + normal[2];
            }

            NodeProfileResult profile;
            profile.id = node.id;
            profile.type = node.type;
            profile.buildMs = node.buildMs;
            profile.inclusiveMs = elapsedMs(start);
            result->profiles.push_back(std::move(profile));
        }

        volatile double sink = accumulator;
        (void)sink;

        std::unordered_map<unsigned int, std::size_t> indexById;
        std::unordered_map<unsigned int, double> inclusiveById;
        for (std::size_t i = 0; i < snapshot->nodes.size(); ++i) {
            indexById[snapshot->nodes[i].id] = i;
            inclusiveById[snapshot->nodes[i].id] = result->profiles[i].inclusiveMs;
        }

        for (std::size_t i = 0; i < snapshot->nodes.size(); ++i) {
            double self = result->profiles[i].inclusiveMs;
            std::unordered_set<unsigned int> subtractedChildren;
            for (const auto childId : snapshot->nodes[i].children) {
                if (!subtractedChildren.insert(childId).second)
                    continue;
                const auto child = inclusiveById.find(childId);
                if (child != inclusiveById.end())
                    self -= child->second;
            }
            result->profiles[i].exclusiveMs = std::max(0.0, self);
            result->benchmarkMs = std::max(result->benchmarkMs,
                                           result->profiles[i].inclusiveMs);
        }

        Mesh mesh;
        const auto polygonizeStart = std::chrono::steady_clock::now();
        snapshot->root->Polygonize(snapshot->resolution,
                                   mesh,
                                   snapshot->box,
                                   snapshot->epsilon);
        result->polygonizeMs = elapsedMs(polygonizeStart);

        result->mesh = std::move(mesh);
        result->vertices = result->mesh.Vertexes();
        result->triangles = result->mesh.Triangles();
        result->totalMs = result->benchmarkMs + result->polygonizeMs;
        result->success = true;
    } catch (const std::exception& exception) {
        result->error = QString::fromUtf8(exception.what());
    } catch (...) {
        result->error = QStringLiteral("Unknown error while evaluating the graph.");
    }

    return result;
}
