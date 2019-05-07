#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_map>
#include "model.h"

class RouteModel : public Model {
   public:
    class Node : public Model::Node {
       public:
        Node *parent = nullptr;
        float h_value = std::numeric_limits<float>::max();
        float g_value = 0;
        bool visited = false;
        std::vector<Node *> neighbors;

        float distance(Node other) const {
            return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));
        }

        Node() {}
        Node(int idx, RouteModel *search_model, Model::Node node) : Model::Node(node), parent_model(search_model), index(idx) {}
        void FindNeighbors();

       private:
        int index;
        RouteModel *parent_model = nullptr;
        Node *FindNeighbor(std::vector<int> node_indices);
    };

    // Add public RouteModel variables and methods here.
    RouteModel(const std::vector<std::byte> &xml);
    auto &SNodes() { return m_Nodes; }
    auto &GetNodeToRoadMap() { return node_to_road; }
    std::vector<Node> path;  // This variable will eventually store the path that is found by the A* search.
    Node &FindClosestNode(float x, float y);

   private:
    void CreateNodeToRoadHashmap();
    std::vector<Node> m_Nodes;
    std::unordered_map<int, std::vector<const Model::Road *>> node_to_road;
};
