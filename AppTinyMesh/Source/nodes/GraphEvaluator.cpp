#include "nodes/GraphEvaluator.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <QtNodes/DataFlowGraphModel>

#include "nodes/ImplicitNodeModels.h"

namespace {

constexpr int kSampleCount = 4096;

double elapsedMs(const std::chrono::steady_clock::time_point& start)
{
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

using OutputEntry = std::pair<QtNodes::NodeId, OutputNodeModel*>;

void collectReachable(QtNodes::DataFlowGraphModel& model,
                      QtNodes::NodeId id,
                      std::unordered_set<QtNodes::NodeId>& reachable)
{
    if (!reachable.insert(id).second)
        return;

    auto* node = model.delegateModel<QtNodes::NodeDelegateModel>(id);
    if (!node)
        return;

    for (unsigned int port = 0; port < node->nPorts(QtNodes::PortType::In); ++port) {
        const auto connections = model.connections(id, QtNodes::PortType::In, port);
        for (const auto& connection : connections)
            collectReachable(model, connection.outNodeId, reachable);
    }
}

std::shared_ptr<GraphSnapshot> makeSnapshot(QtNodes::DataFlowGraphModel& model,
                                            const std::vector<OutputEntry>& outputs,
                                            bool includeProfiles)
{
    auto snapshot = std::make_shared<GraphSnapshot>();
    std::unordered_set<QtNodes::NodeId> reachable;

    for (const auto& [outputId, output] : outputs) {
        OutputSnapshot outputSnapshot;
        outputSnapshot.id = outputId;
        outputSnapshot.box = output->box();
        outputSnapshot.resolution = output->resolution();
        outputSnapshot.epsilon = output->epsilon();

        const auto field = output->field();
        if (!field) {
            outputSnapshot.error = QStringLiteral(
                "The Mesh Output node is not connected to a valid field.");
        } else {
            outputSnapshot.root = field;

            if (includeProfiles) {
                collectReachable(model, outputId, reachable);

                if (!snapshot->hasProfileBox) {
                    snapshot->profileBox = outputSnapshot.box;
                    snapshot->hasProfileBox = true;
                } else {
                    snapshot->profileBox = Box(snapshot->profileBox, outputSnapshot.box);
                }
            }
        }

        snapshot->outputs.push_back(std::move(outputSnapshot));
    }

    if (includeProfiles) {
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
    }

    return snapshot;
}

}

std::shared_ptr<GraphSnapshot> GraphEvaluator::capture(QtNodes::DataFlowGraphModel& model,
                                                       QString& error)
{
    std::vector<OutputEntry> outputs;
    for (const auto id : model.allNodeIds()) {
        if (auto* output = model.delegateModel<OutputNodeModel>(id))
            outputs.emplace_back(id, output);
    }

    if (outputs.empty()) {
        error = QStringLiteral("Add at least one Mesh Output node to the graph.");
        return nullptr;
    }

    return makeSnapshot(model, outputs, true);
}

std::shared_ptr<GraphSnapshot> GraphEvaluator::captureOutput(
    QtNodes::DataFlowGraphModel& model,
    unsigned int outputId,
    QString& error)
{
    auto* output = model.delegateModel<OutputNodeModel>(outputId);
    if (!output) {
        error = QStringLiteral("Mesh Output node not found.");
        return nullptr;
    }

    const std::vector<OutputEntry> outputs{{outputId, output}};
    return makeSnapshot(model, outputs, false);
}

std::shared_ptr<EvaluationResult> GraphEvaluator::evaluate(
    std::shared_ptr<const GraphSnapshot> snapshot)
{
    auto result = std::make_shared<EvaluationResult>();

    if (!snapshot || snapshot->outputs.empty()) {
        result->error = QStringLiteral("No Mesh Output node to evaluate.");
        return result;
    }

    try {
        if (snapshot->hasProfileBox) {
            result->sampledPoints = kSampleCount;

            const Vector center = snapshot->profileBox.Center();
            const Vector halfSize = snapshot->profileBox.Size() * 0.5;

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

            std::unordered_map<unsigned int, double> inclusiveById;
            for (std::size_t i = 0; i < snapshot->nodes.size(); ++i) {
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
        }

        int successfulOutputs = 0;
        result->outputs.reserve(snapshot->outputs.size());

        for (const auto& outputSnapshot : snapshot->outputs) {
            OutputResult outputResult;
            outputResult.id = outputSnapshot.id;

            if (!outputSnapshot.root) {
                outputResult.error = outputSnapshot.error.isEmpty()
                                         ? QStringLiteral("No field to evaluate.")
                                         : outputSnapshot.error;
                result->outputs.push_back(std::move(outputResult));
                continue;
            }

            try {
                Mesh mesh;
                const auto polygonizeStart = std::chrono::steady_clock::now();
                outputSnapshot.root->Polygonize(outputSnapshot.resolution,
                                                mesh,
                                                outputSnapshot.box,
                                                outputSnapshot.epsilon);
                outputResult.polygonizeMs = elapsedMs(polygonizeStart);
                outputResult.mesh = std::move(mesh);
                outputResult.vertices = outputResult.mesh.Vertexes();
                outputResult.triangles = outputResult.mesh.Triangles();
                outputResult.success = true;

                ++successfulOutputs;
                result->polygonizeMs += outputResult.polygonizeMs;
                result->vertices += outputResult.vertices;
                result->triangles += outputResult.triangles;
            } catch (const std::exception& exception) {
                outputResult.error = QString::fromUtf8(exception.what());
            } catch (...) {
                outputResult.error = QStringLiteral("Unknown error while polygonizing this output.");
            }

            result->outputs.push_back(std::move(outputResult));
        }

        result->success = successfulOutputs > 0;
        if (!result->success)
            result->error = QStringLiteral("No output could be evaluated.");

        result->totalMs = result->benchmarkMs + result->polygonizeMs;
    } catch (const std::exception& exception) {
        result->error = QString::fromUtf8(exception.what());
    } catch (...) {
        result->error = QStringLiteral("Unknown error while evaluating the graph.");
    }

    return result;
}
