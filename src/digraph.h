#pragma once

#include <vector>
#include <string>
#include <istream>
#include <unordered_map>

namespace poincare {

struct Node {
    Node(std::string name_, int32_t enumeration_) : count_as_source(0), count_as_target(0), target_enums(), name(name_), enumeration(enumeration_) {}
    const std::string name;
    const int32_t enumeration;
    int64_t count_as_source;
    int64_t count_as_target;
    std::vector<int32_t> target_enums;
};

struct Edge {
    Node& source;
    Node& target;
    Edge(Node& source_, Node& target_) : source(source_), target(target_) {
        source.count_as_source++;
        target.count_as_target++;
        source.target_enums.push_back(target.enumeration);
    }
};

class Digraph {
    public:
        std::vector<Node*> enumeration2node;
        std::unordered_map<std::string, Node*> name2node;
        std::vector<Edge*> edges;

        /**
         * Given an input stream giving the edges of a graph in tab-separated
         * CSV format without header, with columns source and target (in that
         * order) e.g.
         *   sand_cat.n.01    cat.n.01
		 *   false_saber-toothed_tiger.n.01    feline.n.01
		 *   ...
         * read this data, creating a Digraph object.
         * Raises a runtime_error if does not conform to format.
         * Post:
         * + For each node X in referenced in the CSV, name2node[X] is a node
         * object `node` such that node.name == X and `node` reflects the
         * transitions and occurrence stats of that node in the file.
         * + The lines of the CSV correspond in order to `edges`.
         * + enumeration2node.size() == node_count() == number of distinct nodes in CSV
         * + enumeration2node[n].enumeration == n for all 0 <= n < node_count()
         */
        Digraph(std::istream& in);
        ~Digraph();
        int64_t const node_count();

    protected:
        /**
         * Return a reference to the Node with that name,
         * creating a new Node if necessary, updating
         * enumeration2node and name2node
         */
        Node& find_or_create_node(const std::string& node_name);

};

/**
 * Read characters into `word` until SEPARATOR, EOL or EOF
 * is read; this last character is not added to `word`,
 * but returned instead.
 */
int read_token(std::istream& in, std::string& word);
}
