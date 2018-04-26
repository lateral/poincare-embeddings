#include "digraph.h"
#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace poincare {
    const char SEPARATOR = '\t';

Digraph::Digraph(std::istream& in) {
    std::string line;
    std::vector<std::string> fields;
    std::string field;
    while (std::getline(in, line)) {
        std::stringstream line_stream(line);
        fields.clear();
        while (std::getline(line_stream, field, '\t')) {
            fields.push_back(field);
        }
        if (fields.size() != 2) {
            throw std::runtime_error("expected exactly two tab-separated columns at line " + std::to_string(edges.size()));
        }
        Node& source = find_or_create_node(fields[0]);
        Node& target = find_or_create_node(fields[1]);
        Edge* edge_ptr = new Edge(source, target);
        edges.push_back(edge_ptr);
    }
    std::cerr << "\rRead " << edges.size() << " edges." << std::endl;
    std::cerr << "Number of nodes: " << node_count() << std::endl;
}

Digraph::~Digraph() {
    for (int32_t i=0; i < edges.size(); i++) {
        delete edges[i];
    }
    for (int32_t i=0; i < enumeration2node.size(); i++) {
        delete enumeration2node[i];
    }
}

int64_t const Digraph::node_count() {
    return enumeration2node.size();
}

Node& Digraph::find_or_create_node(const std::string& node_name) {
    try {
        return *(name2node.at(node_name));
    } catch (const std::out_of_range) {
        Node* node_ptr = new Node(node_name, enumeration2node.size());
        name2node[node_name] = node_ptr;
        enumeration2node.push_back(node_ptr);
        return *node_ptr;
    }
}

}
