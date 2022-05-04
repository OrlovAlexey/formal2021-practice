#include "vector"
#include "unordered_set"
#include "unordered_map"
#include "stack"
#include "../CommonFiles/Grammar.h"

const char EMPTY_SYMBOL = '\0';
const char PRESTARTING_NETERMINAL = '&'; // & is S'
const std::string STARTING_NETERMINAL = "S";
const char TERMINATING_SYMBOL = '$';

template <typename T>
std::unordered_set<T> operator+ (const std::unordered_set<T>& a, const std::unordered_set<T>& b);

class LR {
    Grammar grammar;
    std::vector<std::unordered_map<char, std::string>> LR_Table;
    struct Configuration {
        int pos = 0; // position of the dot
        char lhs = 0; // left-hand side
        std::string rhs; // right-hand side
        char next_terminal = 0; // next_terminal that is deduced in parsing tree
        Configuration() = default;
        Configuration(int pos, char lhs, const std::string& rhs, char index);
        Configuration(const Grammar::Rule& rule, char b);
        char current_symbol() const;
        bool is_ending() const;
        bool operator== (const Configuration& other) const;
    };

    struct conf_hash {
        size_t operator()(const Configuration& conf) const;
    };

    struct conf_eq {
        bool operator()(const Configuration& conf1, const Configuration& conf2) const;
    };


    using set_conf = std::unordered_set<Configuration, conf_hash, conf_eq>;

    std::unordered_set<char> FIRST(const std::string& s, char previous_neterm) const;

    set_conf closure(const set_conf& I);
    set_conf go_to(const set_conf& I, char symbol);
    struct AutomatVertex {
        int vertex_number;
        set_conf vertex_set;
        std::vector<int> edges;
        AutomatVertex() : vertex_number(), vertex_set(), edges() {}
        AutomatVertex(int vertex_number, const set_conf& vertex_set, const std::vector<int>& edges) : vertex_number(vertex_number), vertex_set(vertex_set), edges(edges) {}
    };
    struct vertex_hash {
        size_t operator()(const AutomatVertex& vertex) const;
    };
    struct vertex_eq {
        bool operator()(const AutomatVertex& vertex1, const AutomatVertex& vertex2) const;
    };

    std::unordered_map<int, AutomatVertex> generate_situations();
public:
    void fit(const Grammar& G);
    bool predict(const std::string& word) const;
};