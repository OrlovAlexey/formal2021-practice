#pragma once
#include "../CommonFiles/Grammar.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <gtest/gtest_prod.h>
using namespace std;

const char PRESTARTING_NETERMINAL = '&'; // & is S'
const string STARTING_NETERMINAL = "S";
const char EMPTY_SYMBOL = '\0';

class Earley {
    Grammar current_grammar;
    string handling_word;
    struct Configuration {
        int pos; // position of dot (before pos)
        char lhs; // left-hand side
        string rhs; // right-hand side
        int index_of_read_prefix; // index_of_read_prefix of prefix that have been read
        char current_symbol() const;
        bool operator== (const Configuration& other) const {
            return (pos == other.pos && lhs == other.lhs && rhs == other.rhs && index_of_read_prefix == other.index_of_read_prefix);
        }
    };

    struct conf_hash {
        size_t operator()(const Configuration& conf) const {
            return std::hash<unsigned int>()(std::hash<string>()(conf.rhs) + std::hash<int>()(conf.pos));
        }
    };

    struct conf_eq {
        bool operator()(const Configuration& conf1, const Configuration& conf2) const {
            return (conf1 == conf2);
        }
    };

    using set_conf = unordered_set<Configuration, conf_hash, conf_eq>;
    void Scan(vector<set_conf>& D, int j);
    bool Predict(vector<set_conf>& D, int j); // return true iff D[j] has changed
    bool Complete(vector<set_conf>& D, int j, vector<unordered_map<Configuration, bool, conf_hash, conf_eq>>& flags_for_complete); // similarly

    // for tests of private methods
    FRIEND_TEST(EarleyAlgoTests, Scan_test);
    FRIEND_TEST(EarleyAlgoTests, Predict_test);
    FRIEND_TEST(EarleyAlgoTests, Complete_test);

public:
    void fit(const Grammar& G);
    bool predict(const string& word);
};


