#include "Earley.h"


char Earley::Configuration::current_symbol() const {
    if (pos < rhs.size()) {
        return rhs[pos];
    }
    else {
        return EMPTY_SYMBOL;
    }
}

void Earley::Scan(vector<set_conf>& D, int j) {
    if (j == 0) {
        return;
    }
    for (auto& conf : D[j - 1]) {
        if (current_grammar.is_terminal(conf.current_symbol()) && conf.current_symbol() == handling_word[j - 1]) {
            Configuration new_conf = conf;
            new_conf.pos++;
            D[j].insert(new_conf);
        }
    }
}

bool Earley::Predict(vector<set_conf>& D, int j) {
    bool flag = false;
    for (auto& conf : D[j]) {
        char handled_neterminal = conf.current_symbol();
        if (current_grammar.is_neterminal(handled_neterminal)) {
            for (auto &rule : current_grammar.rules) {
                if (rule.lhs == handled_neterminal) {
                    Configuration new_conf;
                    new_conf.lhs = rule.lhs;
                    new_conf.rhs = rule.rhs;
                    new_conf.pos = 0;
                    new_conf.index_of_read_prefix = j;
                    flag |= D[j].insert(new_conf).second;
                }
            }
        }
    }
    return flag;
}

bool Earley::Complete(vector<set_conf>& D, int j, vector<unordered_map<Configuration, bool, conf_hash, conf_eq>>& flags_for_complete) {
    bool flag = false;
    for (auto & conf1 : D[j]) {
        if (conf1.pos == conf1.rhs.size() && !flags_for_complete[j][conf1]) {
            flags_for_complete[j][conf1] = true;
            for (auto& conf2 : D[conf1.index_of_read_prefix]) {
                if (current_grammar.is_neterminal(conf2.current_symbol()) && conf2.current_symbol() == conf1.lhs) {
                    Configuration new_conf = conf2;
                    new_conf.pos++;
                    flag |= D[j].insert(new_conf).second;
                }
            }
        }
    }
    return flag;
}

void Earley::fit(const Grammar& G) {
    current_grammar = G;
}

bool Earley::predict(const string& word) {
    size_t word_len = word.length();
    handling_word = word;
    vector<set_conf> D(word_len + 1);
    vector<unordered_map<Configuration, bool, conf_hash, conf_eq>> flags_for_complete(word_len + 1);
    D[0].insert({0, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, 0});
    for (int j = 0; j < word_len + 1; ++j) {
        Scan(D, j);
        bool flag = true;
        while (flag) {
            flag = false;
            flag |= Complete(D, j, flags_for_complete);
            flag |= Predict(D, j);
        }
    }
    Configuration ending_conf({1, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, 0});
    for (auto& conf : D[word_len]) {
        if (conf == ending_conf) {
            return true;
        }
    }
    return false;
}


