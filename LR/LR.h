#include "vector"
#include "unordered_set"
#include "unordered_map"
#include "stack"
#include "Grammar.h"
using namespace std;

const char EMPTY_SYMBOL = '\0';
const char PRESTARTING_NETERMINAL = '&'; // & is S'
const string STARTING_NETERMINAL = "S";
const char TERMINATING_SYMBOL = '$';

template <typename T>
unordered_set<T> operator+ (const unordered_set<T>& a, const unordered_set<T>& b);

class LR {
    Grammar grammar;
    vector<unordered_map<char, string>> LR_Table;
    struct Configuration {
        int pos = 0; // position of the dot
        char lhs = 0; // left-hand side
        string rhs; // right-hand side
        char next_terminal = 0; // next_terminal that is deduced in parsing tree
        Configuration() = default;
        Configuration(int pos, char lhs, const string& rhs, char index);
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


    using set_conf = unordered_set<Configuration, conf_hash, conf_eq>;

    unordered_set<char> FIRST(const string& s, char previous_neterm) const;

    set_conf closure(const set_conf& I);
    set_conf go_to(const set_conf& I, char symbol);
    unordered_map<int, set_conf> generate_situations();
public:
    void fit(const Grammar& G);
    bool predict(const string& word) const;
};