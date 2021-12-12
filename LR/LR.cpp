#include "LR.h"

char LR::Configuration::current_symbol() const {
    if (pos < rhs.size()) {
        return rhs[pos];
    }
    else {
        return EMPTY_SYMBOL;
    }
}

LR::Configuration::Configuration(const Grammar::Rule& rule, char b) : pos(0), lhs(rule.lhs), rhs(rule.rhs), next_terminal(b) {}

LR::Configuration::Configuration(int pos, char lhs, const string& rhs, char index) : pos(pos), lhs(lhs), rhs(rhs), next_terminal(index) {}

bool LR::Configuration::is_ending() const {
    return (pos >= rhs.size());
}

bool LR::Configuration::operator==(const LR::Configuration &other) const {
    return (pos == other.pos && lhs == other.lhs && rhs == other.rhs && next_terminal == other.next_terminal);
}

size_t LR::conf_hash::operator()(const LR::Configuration &conf) const {
    return std::hash<unsigned int>()(std::hash<string>()(conf.rhs) + std::hash<int>()(conf.pos));
}

bool LR::conf_eq::operator()(const LR::Configuration &conf1, const LR::Configuration &conf2) const {
    return (conf1 == conf2);
}

template <typename T>
unordered_set<T> operator+ (const unordered_set<T>& a, const unordered_set<T>& b) {
    unordered_set<T> copy = a;
    for (const T &value : b) {
        copy.insert(value);
    }
    return copy;
}

unordered_set<char> LR::FIRST(const string& s, char previous_neterm) const {
    if (s.empty()) {
        return unordered_set<char>();
    }
    for (char c : s) {
        if (c == TERMINATING_SYMBOL) {
            return unordered_set<char>({TERMINATING_SYMBOL});
        }
        if (grammar.is_terminal(c)) {
            return unordered_set<char>({c});
        }
        if (grammar.is_neterminal(c)) {
            unordered_set<char> first;
            for (auto& rule : grammar.rules) {
                if (rule.lhs == c && c != previous_neterm) {
                    first = first + FIRST(rule.rhs, c);
                }
            }
            if (!first.empty()) {
                return first;
            } else {
                continue;
            }
        }
    }
}

LR::set_conf LR::closure(const LR::set_conf& I) {
    set_conf J = I;
    bool flag = true;
    while (flag) {
        flag = false;
        set_conf Temp = J;
        for (auto& conf : Temp) {
            auto first = FIRST(conf.rhs.substr(min(conf.pos + 1, (int)conf.rhs.size())) + conf.next_terminal, EMPTY_SYMBOL);
            for (auto& rule : grammar.rules) {
                if (rule.lhs == conf.current_symbol()) {
                    for (char b : first) {
                        flag |= J.insert(Configuration(rule, b)).second;
                    }
                }
            }
        }
    }
    return J;
}

LR::set_conf LR::go_to(const LR::set_conf& I, char symbol) {
    set_conf J;
    for (auto& conf : I) {
        if (conf.current_symbol() == symbol) {
            auto new_conf = conf;
            new_conf.pos++;
            J.insert(new_conf);
        }
    }
    return closure(J);
}

unordered_map<int, LR::set_conf> LR::generate_situations() {
    unordered_map<int, set_conf> C;
    set_conf temp;
    temp.insert(Configuration(0, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, '$'));
    C.insert(make_pair(0, closure(temp)));
    int current_situation = 0;
    for (int cur_size = 1; current_situation < cur_size;) {
        auto I = C[current_situation];
        LR_Table.emplace_back();
        for (char c : (grammar.get_neterm_alphabet() + grammar.get_term_alphabet())) {
            auto go_to1 = go_to(I, c);
            if (!go_to1.empty()) {
                bool check = false;
                for (auto& set1 : C) {
                    if (set1.second == go_to1) {
                        check = true;
                        LR_Table[current_situation].insert(make_pair(c, "s" + to_string(set1.first)));
                        break;
                    }
                }
                if (!check) {
                    C.insert(make_pair(cur_size, go_to1));
                    LR_Table[current_situation].insert(make_pair(c, "s" + to_string(cur_size)));
                    ++cur_size;
                }
            }
        }
        current_situation++;
    }
    return C;
}

void LR::fit(const Grammar& G) {
    grammar = G;
    unordered_map<int, set_conf> C = generate_situations();
    string alphabet = grammar.get_neterm_alphabet() + grammar.get_term_alphabet() + TERMINATING_SYMBOL;
    auto EndingConfiguration = Configuration(1, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, TERMINATING_SYMBOL);
    for (auto& I : C) {
        for (auto& conf : I.second) {
            if (conf.is_ending()) {
                if (conf.lhs != PRESTARTING_NETERMINAL) {
                    for (int i = 0; i < grammar.rules.size(); ++i) {
                        auto rule = grammar.rules[i];
                        if (rule.lhs == conf.lhs && rule.rhs == conf.rhs) {
                            if (LR_Table[I.first].find(conf.next_terminal) != LR_Table[I.first].end()) {
                                throw std::invalid_argument("The given grammar does not fit LR(1)");
                            }
                            LR_Table[I.first][conf.next_terminal] = "r" + to_string(i + 1);
                        }
                    }
                }
            }
            if (EndingConfiguration == conf) {
                if (LR_Table[I.first].find(conf.next_terminal) != LR_Table[I.first].end()) {
                    throw std::invalid_argument("The given grammar does not fit LR(1)");
                }
                LR_Table[I.first][conf.next_terminal] = "acc";
            }

        }
    }
}

bool LR::predict(const string& word) const {
    string word_t = word + TERMINATING_SYMBOL;
    stack<string> path;
    path.push("&0");
    int i = 0;
    while (true) {
        char c = word_t[i];
        int state = stoi(path.top().substr(1));
        auto& cur_row = LR_Table[state];
        if (cur_row.find(c) == cur_row.end()) {
            return false;
        }
        string action = cur_row.at(c);
        if (action[0] == 'r') {
            int rule_number = stoi(action.substr(1));
            auto& rule = grammar.rules[rule_number - 1];
            for (int j = 0; j < (int)rule.rhs.length(); ++j) {
                path.pop();
            }
            state = stoi(path.top().substr(1));
            path.push(rule.lhs + LR_Table[state].at(rule.lhs).substr(1));
        }
        if (action[0] == 's') {
            ++i;
            path.push(c + action.substr(1));
        }
        if (action == "acc") {
            return true;
        }
    }
}